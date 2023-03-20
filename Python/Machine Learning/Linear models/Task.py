import numpy as np


class Preprocessor:

    def __init__(self):
        pass

    def fit(self, X, Y=None):
        pass

    def transform(self, X):
        pass

    def fit_transform(self, X, Y=None):
        pass


class MyOneHotEncoder(Preprocessor):

    def __init__(self, dtype=np.float64):
        super(Preprocessor).__init__()
        self.dtype = dtype
        self.arr = []

    def fit(self, X, Y=None):
        for j in X:
            self.arr.append(np.unique(X[j]))

    def transform(self, X):
        vect = []
        for i in self.arr:
            vect.append(len(i))
        final = np.zeros((len(X), sum(vect)))
        index = 0
        ind2 = 0
        summa = 0
        nposition = 0
        for i in X:
            for ind in X.index:
                a = X.loc[ind]
                final[nposition % len(final)][list(self.arr[index]).index(a[i]) + summa] = 1
                nposition = nposition+1
            summa = summa+vect[ind2]
            ind2 = ind2+1
            index = index+1
        return final

    def fit_transform(self, X, Y=None):
        self.fit(X)
        return self.transform(X)

    def get_params(self, deep=True):
        return {"dtype": self.dtype}


class SimpleCounterEncoder:

    def __init__(self, dtype=np.float64):
        self.dtype = dtype
        self.aarr = []

    def fit(self, X, Y):
        for i in X:
            _, my_l = np.unique(X[i], return_counts=True)
            index = 0
            self.aarr.append({})
            for j in np.unique(X[i]):
                numm = my_l[index]
                k = Y.loc[X[j == X[i]].index]
                self.aarr[-1][j] = (sum(k)/numm, numm/len(X.index))
                index += 1
            my_l = []

    def transform(self, X, a=1e-5, b=1e-5):
        init_pos = len(X.columns)*3
        result = np.zeros((len(X), init_pos))
        pposition = 0
        ind1 = 0
        ind2 = 0
        new_arr = self.aarr[pposition]
        for i in X:
            for j in X.index:
                new_arr = self.aarr[pposition]
                new_arr2 = result[ind2]
                tmp_val = X.loc[j][i]
                new_arr2[ind1 + 2] = (new_arr[tmp_val][0] + a) / (new_arr[tmp_val][1] + b)
                result[ind2] = new_arr2
                new_arr2[ind1 + 1] = new_arr[tmp_val][1]
                result[ind2] = new_arr2
                new_arr2[ind1 + 0] = new_arr[tmp_val][0]
                result[ind2] = new_arr2
                ind2 = ind2+1
            ind1 = ind1+3
            pposition = pposition+1
            ind2 = 0
        return result

    def fit_transform(self, X, Y, a=1e-5, b=1e-5):
        self.fit(X, Y)
        return self.transform(X, a, b)

    def get_params(self, deep=True):
        return {"dtype": self.dtype}


def group_k_fold(size, n_splits=3, seed=1):
    idx = np.arange(size)
    np.random.seed(seed)
    idx = np.random.permutation(idx)
    n_ = size // n_splits
    for i in range(n_splits - 1):
        yield idx[i * n_: (i + 1) * n_], np.hstack((idx[:i * n_], idx[(i + 1) * n_:]))
    yield idx[(n_splits - 1) * n_:], idx[:(n_splits - 1) * n_]


class FoldCounters:

    def __init__(self, n_folds=3, dtype=np.float64):
        self.dtype = dtype
        self.n_folds = n_folds
        self.aarr = []

    def fit(self, X, Y, seed=1):
        self.aarr = np.zeros((len(X), len(X.columns)*3))
        aarr = np.array(X)
        index = 0
        ssumma = 0
        ssumma2 = 0

        for i1, j1 in group_k_fold(len(aarr), n_splits=self.n_folds, seed=seed):
            for i in range(aarr[i1].shape[1]):
                for j in range(len(aarr[i1])):
                    imp_place = i1[j]
                    for k in range(len(aarr[j1])):
                        ssumma = ssumma+int(aarr[j1][k][i] == aarr[i1][j][i])
                        ssumma2 = ssumma2+np.array(Y[j1])[k]*int(aarr[j1][k][i] == aarr[i1][j][i])
                    self.aarr[imp_place][index] = ssumma2/ssumma
                    self.aarr[imp_place][index+1] = ssumma/len(aarr[j1])
                    ssumma = 0
                    ssumma2 = 0
                    imp_place = i1[j]
                index = index+3
            index = 0

    def transform(self, X, a=1e-5, b=1e-5):
        result = self.aarr.copy()
        for i in range(len(self.aarr)):
            for j in range(2, self.aarr.shape[1], 3):
                result[i][j] = (self.aarr[i][j-2]+a)
                result[i][j] = result[i][j]/(self.aarr[i][j-1]+b)
        return result

    def fit_transform(self, X, Y, a=1e-5, b=1e-5):
        self.fit(X, Y)
        return self.transform(X, a, b)

    def fit_transform(self, X, Y, a=1e-5, b=1e-5):
        self.fit(X, Y)
        return self.transform(X, a, b)


def weights(x, y):
    uniq_list = np.unique(x)
    np.unique(x)
    tmp_list = np.bincount(x)
    result = []
    ssumma = 0
    for a in range(0, len(tmp_list)):
        for b in range(0, len(y)):
            t = int(uniq_list[a] == x[b])
            print(t)
            ssumma = ssumma + int(y[b] and t)
        t = ssumma / tmp_list[a]
        result.append(t)
        ssumma = 0
    return np.array(result)
