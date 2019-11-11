import glob
import numpy as np
import random
from numpy import mean
from numpy import std
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense
from tensorflow.keras.layers import Dropout
from tensorflow.keras.layers import LSTM


def load_fileset(path='../out/*'):

    files_dir = glob.glob(path)
    files_list = []

    for file in files_dir:
        with open(file, 'r') as fo:
            lines = fo.readlines()
            files_list.append(lines)
    return files_list

def get_data_set(files_list, normalized = True):
    label_list = []
    data_list = []
    data_list_normalzed = []
    for file in files_list:
        label_list.append(str(file[0].rsplit('_', 1)[0]))
        data = [float(i.split(';', 2)[1]) for i in file[2:]]
        data_normalized = [(x - min(data)) / (max(data) - min(data)) for x in data]
        data_list.append(data)
        data_list_normalzed.append(data_normalized)
    if normalized:
        return data_list_normalzed, label_list
    return data_list, label_list

def get_labels_as_vector(label_list):
    label_map = {}
    ind = 0
    uniques = list(set(label_list))
    for label in uniques:
        label_map[label] = ind
        ind+=1

    label_list = list(map(label_map.get, label_list))
    label_vect = []
    for label in label_list:
        vector = [0,0,0,0]
        vector[label] = 1
        label_vect.append(vector)
    return label_vect


files_list = load_fileset()
data_list, labels_list = get_data_set(files_list)
labels_list = get_labels_as_vector(labels_list)


## Dziele dane na 3 zbiory: training, validation i test

train_ind = random.sample(range(0, len(data_list)), int(0.8*len(data_list)))
test_ind = []
for i in range(len(data_list)):
    if i not in train_ind:
        test_ind.append(i)
print(test_ind)

x_train = np.array([data_list[i] for i in train_ind])
x_train = np.expand_dims(x_train, axis=2)
y_train = np.array([labels_list[i] for i in train_ind])

x_test = np.array([data_list[i] for i in test_ind])
x_test = np.expand_dims(x_test, axis=2)
y_test = np.array([labels_list[i] for i in test_ind])


# fit and evaluate a model
def evaluate_model(trainX, trainy, testX, testy):
    verbose, epochs, batch_size = 2, 10, 300
    n_timesteps, n_outputs = trainX.shape[1], trainy.shape[1]
    model = Sequential()
    model.add(LSTM(100, input_shape=(n_timesteps, 1)))
    model.add(Dropout(0.5))
    model.add(Dense(100, activation='relu'))
    model.add(Dense(n_outputs, activation='softmax'))
    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    # fit network
    model.fit(trainX, trainy, epochs=epochs, batch_size=batch_size, verbose=verbose)
    # evaluate model
    _, accuracy = model.evaluate(testX, testy, batch_size=batch_size, verbose=0)
    return accuracy


# summarize scores
def summarize_results(scores):
    print(scores)
    m, s = mean(scores), std(scores)
    print('Accuracy: %.3f%% (+/-%.3f)' % (m, s))


# run an experiment
def run_experiment(trainX, trainy, testX, testy, repeats=10):

    # repeat experiment
    scores = list()
    for r in range(repeats):
        score = evaluate_model(trainX, trainy, testX, testy)
        score = score * 100.0
        print('>#%d: %.3f' % (r + 1, score))
        scores.append(score)
    # summarize results
    summarize_results(scores)


# run the experiment
run_experiment(x_train, y_train, x_test, y_test)
