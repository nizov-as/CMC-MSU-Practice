import numpy as np
from collections import defaultdict


def kfold_split(num_objects, num_folds):
    group_size = num_objects // num_folds
    arr_res = []
    arr1 = [i for i in range(num_objects)]
    for i in range(num_folds):
        without_i = arr1[:(i * group_size)]
        if i == (num_folds - 1):
            i_fold = arr1[(i * group_size):]
        else:
            without_i += (arr1[((i+1) * group_size):])
            i_fold = arr1[(i * group_size):((i+1) * group_size)]
        arr_res.append((np.array(without_i), np.array(i_fold)))
    return arr_res


def knn_cv_score(X, y, parameters, score_function, folds, knn_class):
    res_dict = {}
    size = len(folds)
    for neighbour in parameters['n_neighbors']:
        for weight in parameters['weights']:
            for metric in parameters['metrics']:
                for normalizer in parameters['normalizers']:
                    sum = 0
                    for fold in folds:
                        X_train = X[fold[0]]
                        X_test = X[fold[1]]
                        y_train = y[fold[0]]
                        y_true = y[fold[1]]
                        if normalizer[0] is not None:
                            normalizer[0].fit(X_train)
                            X_train = normalizer[0].transform(X_train)
                            X_test = normalizer[0].transform(X_test)
                        knn_model = knn_class(n_neighbors=neighbour, metric=metric, weights=weight)
                        knn_model.fit(X_train, y_train)
                        y_predict = knn_model.predict(X_test)
                        sum += score_function(y_true, y_predict)
                    res_dict[(normalizer[1], neighbour, metric, weight)] = sum/size
    return res_dict
