# find the most (least) similar signals within a 15 sec singal
# last update: 2021/10/4

import matplotlib.pylab as plt
import numpy as np
import os
import dependence.utils as utils

from _01_data2npz import output_path as input_path

input_file = '20151026_113.npz'
output_path = 'outputs\\03_similarity_measurement\\'
log_file = 'log.txt'

def save_fig_2(data_1, data_2, title_1, title_2, file):
    fig = plt.figure()

    ax1 = fig.add_subplot(211)
    plt.plot(data_1, color='r')
    ax1.set_title(title_1)

    ax2 = fig.add_subplot(212)
    plt.plot(data_2, color='r')
    ax2.set_title(title_2)

    fig.tight_layout()
    plt.savefig(file)
    plt.close('all')

def distance(data_1, data_2) -> float:
    # Cosine Similarity
    similarity = (data_1 * data_2).sum() / (np.linalg.norm(data_1) * np.linalg.norm(data_2))
    dist = -similarity

    # Euclidean Distance
    # dist = np.linalg.norm(data_1 - data_2)

    # Manhattan Distance
    # dist = np.abs(data_1 - data_2).sum()

    return dist

def main():
    global log_file
    input_file_path = input_path + input_file
    log_file = output_path + log_file
    
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    utils.check_01()
    data = np.load(input_file_path)['data']

    step = 50
    size = 500
    stop = data.size - size + 1
    distances = []

    for start_a in range(0, stop, step):
        for start_b in range(start_a + size, stop, step):
            data_1 = data[start_a : start_a + size]
            data_2 = data[start_b : start_b + size]
            distances.append([start_a, start_b, distance(data_1, data_2)])

    distances = sorted(distances, key=lambda x : x[2])

    utils.atomic_print_and_log('Most similar:', file=log_file)
    for i in range(3):
        e = distances[i]
        data_1 = data[e[0] : e[0] + size]
        data_2 = data[e[1] : e[1] + size]
        save_fig_2(data_1, data_2, 'start=%d' % e[0], 'start=%d' % e[1], output_path + 'ms_%d' % (i + 1))
        utils.atomic_print_and_log('start_a=%d, start_b=%d, distance=%f' % (e[0], e[1], e[2]), file=log_file)

    utils.atomic_print_and_log('Least similar:', file=log_file)
    for i in range(3):
        e = distances[len(distances) - i - 1]
        data_1 = data[e[0] : e[0] + size]
        data_2 = data[e[1] : e[1] + size]
        save_fig_2(data_1, data_2, 'start=%d' % e[0], 'start=%d' % e[1], output_path + 'ls_%d' % (i + 1))
        utils.atomic_print_and_log('start_a=%d, start_b=%d, distance=%f' % (e[0], e[1], e[2]), file=log_file)

if __name__ == '__main__':
    main()