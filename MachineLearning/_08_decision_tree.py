import json
import numpy as np
import os
import dependence.evaluation as eval
import dependence.utils as utils

from _05_feature_extraction import output_file_path as features_file
from _06_add_labels import output_file_path as labels_file

output_path = 'outputs\\08_decision_tree\\'
output_file = 'tree.txt'
output_file_path = output_path + output_file

class Node:
    def __init__(self, feature=None, val=0, less=None, larger=None, res=None) -> None:
        self.feature = feature
        self.val = val
        self.less = less
        self.larger = larger
        self.res = res

    def test(self, features: dict):
        if utils.is_none(self.res):
            child_node = self.less if features[self.feature] < self.val else self.larger
            return child_node.test(features)
        return self.res

    def tolist(self) -> list:
        node_less = None if utils.is_none(self.less) else self.less.tolist()
        node_larger = None if utils.is_none(self.larger) else self.larger.tolist()
        return [self.feature, self.val, node_less, node_larger, self.res]

    @staticmethod
    def fromlist(x: list):
        node = Node()
        node.feature = x[0]
        node.val = x[1]
        node.less = None if utils.is_none(x[2]) else Node.fromlist(x[2])
        node.larger = None if utils.is_none(x[3]) else Node.fromlist(x[3])
        node.res = x[4]
        return node

def get_node(data: list, feature_labels: list[str], depth=0) -> Node:
    """
Parameters
----------
data : array_like
    A list of all training data. In the forms of `[data1, data2, ...]`
    while each data has the form of `[[feature1, feature2, ...], label]`.
feature_labels : list[str]
    A list with all the feature names. For example
    `['skew', 'kurtosis', ...]`.
    The order has to be consistent to the features order defined in
    `data`.
depth : int, optional
    Specify the depth of the current node. This parameter also determines
    which feature will be used in the current node. By default is 0.
    """
    labels_count = max([i[1] for i in data]) + 1
    labels_total = np.zeros((labels_count), dtype=np.uint32)
    for d in data:
        labels_total[d[1]] += 1

    if depth >= len(feature_labels) or (labels_total > 0).sum() == 1:
        most_common = labels_total.argmax()
        return Node(res=int(most_common))

    data = sorted(data, key=lambda x : x[0][depth])

    n1_labels = np.zeros((labels_count), dtype=np.uint32)
    n2_labels = labels_total
    n1_sum = 0
    n2_sum = n2_labels.sum()
    gini_min = float('inf')
    i_opt = -1
    
    for i, d in enumerate(data[:-1]):
        n1_labels[d[1]] += 1
        n2_labels[d[1]] -= 1
        n1_sum += 1
        n2_sum -= 1
        n1_p = n1_labels / n1_sum
        n2_p = n2_labels / n2_sum
        n1_gini = 1 - (n1_p * n1_p).sum()
        n2_gini = 1 - (n2_p * n2_p).sum()
        gini = n1_sum * n1_gini + n2_sum * n2_gini

        if gini < gini_min:
            gini_min = gini
            i_opt = i
    
    n1_data = data[0 : i_opt + 1]
    n2_data = data[i_opt + 1:]
    n1_node = get_node(n1_data, feature_labels, depth + 1)
    n2_node = get_node(n2_data, feature_labels, depth + 1)

    return Node(
        feature=feature_labels[depth],
        val=(data[i_opt][0][depth] + data[i_opt + 1][0][depth])/2,
        less=n1_node,
        larger=n2_node)

def test(root_node: Node, features: list, feature_titles: list[str]):
    predict = np.empty((len(features)), np.int32)

    for i, features in enumerate(features):
        features_dict = {}
        for j, name in enumerate(feature_titles):
            features_dict[name] = features[j]

        predict[i] = root_node.test(features_dict)
    
    return predict

def main():
    utils.check_05()
    utils.check_06()
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    with open(features_file, 'r') as f:
        features_src = json.load(f)
    with open(labels_file, 'r') as f:
        labels_src = json.load(f)

    feature_titles = features_src[0][1:]
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

    # train
    results = {}

    for i, label in enumerate(label_titles):
        labels_train_sub = np.array([v[i] for v in labels_train], dtype=np.uint32)
        data = list(zip(features_train, labels_train_sub))
        root = get_node(data, feature_titles)
        results[label] = root.tolist()

    with open(output_file_path, 'w') as f:
        json.dump(results, f)
    print(output_file_path)

    # test
    with open(output_file_path, 'r') as f:
        decision_tree_src = json.load(f)
    
    for k, v in decision_tree_src.items():
        print('label=%s' % k)
        root = Node.fromlist(v)
        label_idx = label_titles.index(k)

        labels_train_sub = np.array([v[label_idx] for v in labels_train], dtype=np.uint32)
        predict = test(root, features_train, feature_titles)
        cm = eval.confusion_matrix(predict, labels_train_sub, 0)
        print('trainset: ', end='')
        eval.print_confusion_matrix(cm)
        
        labels_test_sub = np.array([v[label_idx] for v in labels_test], dtype=np.uint32)
        predict = test(root, features_test, feature_titles)
        cm = eval.confusion_matrix(predict, labels_test_sub, 0)
        print('testset: ', end='')
        eval.print_confusion_matrix(cm)

if __name__ == '__main__':
    main()