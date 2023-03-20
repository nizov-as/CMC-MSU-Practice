import numpy as np
from sklearn.svm import SVC


def train_svm_and_predict(train_features, train_target, test_features):
    svc = SVC(kernel='rbf', C=0.3, gamma=0.9)
    svc.fit(train_features[:, [3, 4]], train_target)
    return svc.predict(test_features[:, [3, 4]])
