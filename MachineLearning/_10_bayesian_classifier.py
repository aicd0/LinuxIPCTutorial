import json
import numpy as np
import dependence.evaluation as eval
import dependence.utils as utils

from sklearn.naive_bayes import GaussianNB
from _05_feature_extraction import output_file_path as features_file
from _06_add_labels import output_file_path as labels_file

def main():
    utils.check_05()
    utils.check_06()

    with open(features_file, 'r') as f:
        features_src = json.load(f)
    with open(labels_file, 'r') as f:
        labels_src = json.load(f)

    label_titles = labels_src[0][1:]

    features_all = [i[1:] for i in features_src[1]]
    labels_all = [i[1:] for i in labels_src[1]]
    features_all, labels_all = utils.shuffle(zip(features_all, labels_all))

    trainset_ratio = 0.9
    train_count = round(len(features_all) * trainset_ratio)

    features_train = features_all[0 : train_count]
    features_test = features_all[train_count:]
    labels_train = labels_all[0 : train_count]
    labels_test = labels_all[train_count:]
    
    for i, label in enumerate(label_titles):
        # train
        labels_train_sub = np.array([v[i] for v in labels_train], dtype=np.uint32)
        clf = GaussianNB()
        clf.fit(features_train, labels_train_sub)

        # test
        predict = clf.predict(features_test)
        labels_test_sub = np.array([v[i] for v in labels_test], dtype=np.uint32)
        cm = eval.confusion_matrix(predict, labels_test_sub, 0)
        print('label=%s, ' % label, end='')
        eval.print_confusion_matrix(cm)

if __name__ == '__main__':
    main()