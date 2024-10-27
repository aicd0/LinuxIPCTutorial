import numpy as np

from typing import Tuple

def confusion_matrix(predict, expect, positive_label) -> Tuple:
    if predict.shape != expect.shape:
        raise ValueError()
    if len(predict.shape) != 1:
        raise ValueError()

    tp = 0
    tn = 0
    fp = 0
    fn = 0

    for i in range(len(predict)):
        if predict[i] == expect[i]:
            if predict[i] == positive_label:
                tp += 1
            else:
                tn += 1
        else:
            if predict[i] == positive_label:
                fp += 1
            else:
                fn += 1

    return tp, fn, fp, tn

def print_confusion_matrix(tp_fn_fp_tn: Tuple):
    tp, fn, fp, tn = tp_fn_fp_tn

    print('tp=%d, fn=%d, fp=%d, tn=%d, accuracy=%.1f%%, precision=%.1f%%, recall=%.1f%%' % (
        tp, fn, fp, tn,
        (tp + tn) / (tp + fn + fp + tn) * 100,
        tp / (tp + fp) * 100,
        tp / (tp + fn) * 100))

def v_measure(new_labels, old_labels, beta=1) -> Tuple:
    new_labels = np.array(new_labels, np.uint32)
    old_labels = np.array(old_labels, np.uint32)

    if new_labels.shape != old_labels.shape:
        raise ValueError()
    if len(new_labels.shape) != 1:
        raise ValueError()

    data_count = len(new_labels)
    new_categories = np.max(new_labels) + 1
    old_categories = np.max(old_labels) + 1
    new_to_old = np.empty((new_categories), np.uint32)

    # calculate precision
    precision = np.empty((new_categories))
    
    for new_label in range(new_categories):
        votes = np.zeros((old_categories), np.uint32)

        for i in range(data_count):
            if new_labels[i] == new_label:
                votes[old_labels[i]] += 1

        new_to_old[new_label] = old_label = np.argmax(votes)
        precision[new_label] = votes[old_label] / votes.sum()
    
    # calculate recall
    correct = np.zeros((old_categories), np.uint32)
    total = np.zeros((old_categories), np.uint32)

    for i in range(data_count):
        predict = new_to_old[new_labels[i]]
        expect = old_labels[i]
        total[expect] += 1

        if predict == expect:
            correct[expect] += 1

    recall = correct / total

    # calculate v
    p = precision.mean()
    r = recall.mean()
    v = (1 + beta * beta) * p * r / (beta * beta * p + r)
    return p, r, v