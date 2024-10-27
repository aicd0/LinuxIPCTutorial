import json
import matplotlib.pylab as plt
import numpy as np
import os
import random
import dependence.evaluation as eval
import dependence.utils as utils

from sklearn import metrics
from _04_combine import output_file_path as input_file_path
from _06_add_labels import output_file_path as labels_file

output_path = 'outputs\\07_k_means\\'
log_file = 'log.txt'

def k_means(data, k) -> list:
    data_count = data.shape[0]
    dims = data.shape[1]
    if data_count < k:
        raise ValueError()

    # initialize class centers randomly
    centers = np.empty((k, dims))
    order = [i for i in range(data_count)]
    random.shuffle(order)
    for i in range(k):
        centers[i] = data[order[i]]

    results = np.zeros((data_count), dtype=np.uint8)

    while True:
        old_results = np.copy(results)
        results = np.zeros((data_count), dtype=np.uint8)

        # reclassification for each sample
        for i in range(data_count):
            dist = np.linalg.norm(data[i] - centers, axis=1)
            results[i] = dist.argmin()
        
        # stops iteration if nothing changed
        if np.array_equal(old_results, results):
            break
        
        # update class centers
        centers = np.zeros((k, dims))
        sizes = np.zeros((k), dtype=np.uint32)
        for i in range(data_count):
            centers[results[i]] += data[i]
            sizes[results[i]] += 1
        sizes = sizes.reshape((k, 1))
        centers /= sizes
    
    return results.tolist()

def main():
    log_file_path = output_path + log_file

    if not os.path.exists(output_path):
        os.makedirs(output_path)
    
    utils.check_04()
    data_all = np.load(input_file_path)['data']
    
    utils.check_06()
    with open(labels_file, 'r') as f:
        labels_src = json.load(f)

    label_titles = labels_src[0][1:]
    label_count = len(label_titles)
    labels_all = [i[1:] for i in labels_src[1]]
    data_all, labels_all = utils.shuffle(zip(data_all, labels_all))
    data_all = np.array(data_all)

    k_start = 2
    k_end = 20
    k_axis = range(k_start, k_end + 1)
    l_k_v = np.empty((label_count, len(k_axis)))
    l_k_s = np.empty((len(k_axis)))

    for k in k_axis:
        new_labels = k_means(data_all, k)
        output_file = 'k=%d.txt' % k
        output_file_path = output_path + output_file

        with open(output_file_path, 'w') as f:
            json.dump(new_labels, f)
        utils.atomic_print_and_log(output_file_path, file=log_file_path)

        # silhouette coefficient
        s = metrics.silhouette_score(data_all, new_labels)
        utils.atomic_print_and_log('s=%f' % s, file=log_file_path)
        l_k_s[k - k_start] = s

        for l, label_title in enumerate(label_titles):
            utils.atomic_print_and_log('label=%s' % label_title, end='', file=log_file_path)
            old_labels = np.array([v[l] for v in labels_all], dtype=np.uint32)

            # v-measure
            p, r, v = eval.v_measure(new_labels, old_labels)
            l_k_v[l][k - k_start] = v
            utils.atomic_print_and_log(', p=%.1f%%, r=%.1f%%, v=%f' % (p*100, r*100, v), file=log_file_path)
    
    # plot v-measure
    fig = plt.figure(figsize=(8, 5.3 * len(label_titles)))

    for l, label_title in enumerate(label_titles):
        ax = fig.add_subplot(label_count, 1, l + 1)
        ax.set_title(label_title)
        plt.xlabel('k')
        plt.ylabel('v')
        plt.plot(k_axis, l_k_v[l], color='r')

    fig.tight_layout()
    output_file_path = output_path + 'v-measure.png'
    plt.savefig(output_file_path)
    print(output_file_path)
    plt.close('all')

    # plot silhouette coefficient
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)
    ax.set_title('Silhouette Coefficient')
    plt.xlabel('k')
    plt.ylabel('s')
    plt.plot(k_axis, l_k_s, color='r')

    fig.tight_layout()
    output_file_path = output_path + 'silhouette.png'
    plt.savefig(output_file_path)
    print(output_file_path)
    plt.close('all')

if __name__ == '__main__':
    main()