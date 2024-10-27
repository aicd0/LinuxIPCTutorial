import json
import numpy as np
import dependence.evaluation as eval
import dependence.utils as utils

from _04_combine import output_file_path as data_file_path
from _06_add_labels import output_file_path as labels_file_path

def main():
    utils.check_06()

    data_all = np.load(data_file_path)['data']
    with open(labels_file_path, 'r') as f:
        labels_src = json.load(f)

    labels_all = [i[1:] for i in labels_src[1]]
    data_all, labels_all = utils.shuffle(zip(data_all, labels_all))

    label_titles = labels_src[0][1:]

    trainset_ratio = 0.9
    train_count = round(len(data_all) * trainset_ratio)

    data_train = np.array(data_all[0 : train_count])
    data_test = np.array(data_all[train_count:])
    labels_train = np.array(labels_all[0 : train_count], dtype=np.int32)
    labels_test = np.array(labels_all[train_count:], dtype=np.int32)
    
    k = 10

    for l, label_title in enumerate(label_titles):
        labels_train_sub = np.array([v[l] for v in labels_train], dtype=np.uint32)
        labels_test_sub = np.array([v[l] for v in labels_test], dtype=np.uint32)
        categories_count = np.max(labels_test_sub) + 1
        predict = np.empty((len(data_test)))

        for d, data in enumerate(data_test):
            # Cosine Similarity
            similarities = (data * data_train).sum(axis=1) / (np.linalg.norm(data) * np.linalg.norm(data_train, axis=1))
            distances = -similarities
            order = distances.argsort()
            votes = np.zeros((categories_count), dtype=np.uint32)

            for i in order[:k]:
                votes[labels_train_sub[i]] += 1
            
            predict[d] = np.argmax(votes)
        
        cm = eval.confusion_matrix(predict, labels_test_sub, 0)
        print('label=%s, ' % label_title, end='')
        eval.print_confusion_matrix(cm)

if __name__ == '__main__':
    main()