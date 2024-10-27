# feature extraction
# last update: 2021/10/5

import json
import math
import numpy as np
import os
import dependence.utils as utils

from collections import Counter
from scipy import stats
from _04_combine import output_file_path as input_file_path

output_path = 'outputs\\05_feature_extraction\\'
output_file = 'features.txt'
output_file_path = output_path + output_file

def renyi_entropy(data, alpha):
    return math.log(np.sum(data ** alpha)) / (1 - alpha)

def approximate_entropy(data, m, r=0.15):
    f = r * np.std(data, ddof=1)
    
    def phi(m):
        nonlocal data, f
        k = len(data) - m + 1
        x = np.empty((k, m))
        for i in range(k):
            x[i] = data[i : i + m]
        
        d = np.empty((k, k))
        for i in range(k):
            d[i] = np.abs(x[i] - x).max(axis=1)
                
        t = np.sum(d < f, axis=1) / k
        return np.average(np.log(t))
        
    return phi(m) - phi(m + 1)

def sample_entropy(data, m, r=0.15):
    f = r * np.std(data, ddof=1)

    def phi(m):
        nonlocal data, f
        k = len(data) - m + 1
        x = np.empty((k, m))
        for i in range(k):
            x[i] = data[i : i + m]
        
        d = np.empty((k, k))
        for i in range(k):
            d[i] = np.abs(x[i] - x).max(axis=1)
                
        t = np.sum(d < f, axis=1) / (k - 1)
        return np.average(t)
        
    return math.log(phi(m) / phi(m + 1))

def permutation_entropy(data, m, step=1):
    if step > m:
        step = m
    if step == 1:
        k = int(len(data) - m + 1)
    else:
        k = int((len(data) - m + 1) / step) + 1

    x = np.empty((k, m))
    for i in range(k):
        x[i] = data[i * step : i * step + m]

    index = np.argsort(x, axis=1)
    index_str = [str(i)[1 : -1] for i in index]
    count = Counter(index_str)
    pe = 0
    
    for val in count.values():
        p = val / k
        pe -= p * math.log(p)

    return pe / math.log(math.factorial(m))

def main():
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    utils.check_04()
    data = np.load(input_file_path)['data']
    
    sample_count = data.shape[0]
    sample_dims = data.shape[1]

    titles = ['start', 'skew', 'kurtosis', 're_en', 'ap_en', 'sa_en', 'pe_en']
    all_features = []

    for i in range(0, sample_count):
        features = []
        features.append(i * sample_dims)
        # Skewness
        features.append(stats.skew(data[i]))
        # Kurtosis
        features.append(stats.kurtosis(data[i]))
        # Renyi Entropy
        features.append(renyi_entropy(data[i], alpha=2))
        # Approximate Entropy
        features.append(approximate_entropy(data[i], m=6))
        # Sample Entropy
        features.append(sample_entropy(data[i], m=6))
        # Permutation Entropy
        features.append(permutation_entropy(data[i], m=6, step=1))
        all_features.append(features)
        print('\r%d of %d processed.' % (i + 1, sample_count), end='')
    print()

    with open(output_file_path, 'w') as f:
        json.dump([titles, all_features], f)
    print(output_file_path)

if __name__ == '__main__':
    main()