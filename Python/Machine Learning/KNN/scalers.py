import numpy as np


class MinMaxScaler:
    def __init__(self):
        self.minimum = 0
        self.maximum = 1

    def fit(self, data):
        self.minimum = np.min(data, axis=0)
        self.maximum = np.max(data, axis=0)

    def transform(self, data):
        return (data-self.minimum) / (self.maximum-self.minimum)


class StandardScaler:
    def __init__(self):
        self.disp = 1
        self.math_exp = 0

    def fit(self, data):
        self.math_exp = np.mean(data, axis=0)
        self.disp = np.std(data, axis=0)

    def transform(self, data):
        return (data-self.math_exp) / self.disp
