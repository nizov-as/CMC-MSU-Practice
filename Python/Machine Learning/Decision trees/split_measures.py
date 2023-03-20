import numpy as np


def evaluate_measures(sample):
    sample = np.array(np.sort(sample))
    _, values = np.unique(sample, return_counts=True)
    new_sample = values / np.sum(values)
    res_gini = 1 - np.sum(new_sample**2)
    res_entropy = (-1) * np.sum(new_sample * np.log(new_sample))
    res_error = 1 - np.max(new_sample)
    measures = {'gini': float(res_gini), 'entropy': float(res_entropy), 'error': float(res_error)}
    return measures
