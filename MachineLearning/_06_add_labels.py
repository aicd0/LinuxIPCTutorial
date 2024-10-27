import json
import math
import numpy as np
import os
import dependence.utils as utils

from _04_combine import output_file_path as input_file_path

output_path = 'outputs\\06_add_labels\\'
output_file = 'labels.txt'
output_file_path = output_path + output_file

def data_descretization(data, ratio) -> list:
    if len(data.shape) != 1:
        raise ValueError()

    sample_count = data.shape[0]

    if not isinstance(ratio, list):
        raise TypeError()
    if len(ratio) == 0 or sum(ratio) >= 1:
        raise ValueError()

    for i in range(1, len(ratio)):
        ratio[i] += ratio[i - 1]
    labels = np.full((sample_count), len(ratio), dtype=np.uint32)

    for i in range(len(ratio)):
        target = ratio[i] * sample_count
        rmin = 0
        r2 = 1

        while (data <= r2).sum() < target:
            rmin = r2
            r2 *= 2
        rmax = r2

        while True:
            r2 = (rmin + rmax) / 2
            cres = data <= r2
            scnt = cres.sum()

            if rmax - rmin >= 1e-5:
                if scnt > target:
                    rmax = r2
                    continue
                elif scnt < target:
                    rmin = r2
                    continue

            labels -= cres
            break

    return labels.tolist()

def hypersphere_labels(data, center=None, ratio: list=[0.5], w=None) -> list:
    sample_size = data.shape[1]

    if utils.is_none(center):
        c_average = data.mean(axis=0)
        c_std = data.std(axis=0, ddof=1)
        center = np.random.normal(c_average, c_std, (sample_size))
    if center.shape != (sample_size,):
        raise ValueError()

    if utils.is_none(w):
        w = 1 / data.var(axis=0)
    if w.shape != (sample_size,):
        raise ValueError()

    dif = data - center
    data_r2 = (dif * dif * w).sum(axis=1)
    
    return data_descretization(data_r2, ratio)

def custom_labels_1(data, ratio: list=[0.5]) -> list:
    dmin = data.min(axis=1, keepdims=True)
    dmax = data.max(axis=1, keepdims=True)
    val = np.sin(2 * math.pi * (data - dmin) / (dmax - dmin)).sum(axis=1)
    return data_descretization(val, ratio)

def custom_labels_2(data, ratio: list=[0.5]) -> list:
    dims = data.shape[1]
    w = np.random.uniform(-1, 1, (1, dims))
    val = (data * w).sum(axis=1)
    return data_descretization(val, ratio)

def main():
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    utils.check_04()
    data = np.load(input_file_path)['data']

    data_count = data.shape[0]
    dims = data.shape[1]
    
    labels_1 = hypersphere_labels(data)
    labels_2 = custom_labels_1(data)
    labels_3 = custom_labels_2(data)

    titles = ['start', 'l1', 'l2', 'l3']
    all_labels = []
    for i in range(data_count):
        labels = []
        labels.append(i * dims)
        labels.append(labels_1[i])
        labels.append(labels_2[i])
        labels.append(labels_3[i])
        all_labels.append(labels)

    with open(output_file_path, 'w') as f:
        json.dump([titles, all_labels], f)
    print(output_file_path)

if __name__ == '__main__':
    main()