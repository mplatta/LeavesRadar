import sys
import csv
import glob
import tensorflow as tf
import matplotlib.pyplot as plt
from tensorflow import keras
import numpy as np
import random
import numpy
from keras.datasets import imdb
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers.embeddings import Embedding
from keras.preprocessing import sequence


DEFAULT_TRAIN_PATH = "../out"
DEFAULT_CLASS_PATH = "../calss"


def extract_names(_name):
	index = -1

	for l in range(len(_name) - 1, 0, -1):
		if (_name[l] == '_'): 
			index = l
			break

	return _name[:index]

path = ""
training = False
number = 0;

if (len(sys.argv) >= 1):
	for i in range(1, len(sys.argv)):
		if ((sys.argv[i] == "--path") and ((len(sys.argv) - 1) >= (i + 1))): path = sys.argv[i + 1]
		if (sys.argv[i] == "-t"): training = True
		if ((sys.argv[i] == "-n") and ((len(sys.argv) - 1) >= (i + 1))): number = sys.argv[i + 1]

if (path == ""): 
	if (training): path = DEFAULT_TRAIN_PATH
	else: path = DEFAULT_CLASS_PATH 

print(training)
print(path)
print("Number of species: ", number)

label_in_vector = []

if (training):
	labels = []
	data_ = []

	files = [f for f in glob.glob(path + "**/*", recursive=True)]

	for f in files:
		with open(f, 'r') as csvfile:
			dataset = csv.reader(csvfile, delimiter=';')
			
			ind = 0
			tmp = []
			tmp_name = ""

			for row in dataset:
				if (ind == 0): tmp_name = extract_names(row[0])
				if (ind > 1): tmp.append(float(row[0]))
				ind += 1
			
			data_.append(tmp)
			# data = np.append(data, tmp)
			# np.concatenate(data, tmp)
			# data = np.array(data, tmp)
			labels.append(tmp_name)

	unique = list(set(labels))

	label_map = {}
	ind = 0

	for u in unique:
		label_map[u] = ind
		ind += 1 

	number_of_labels = np.zeros(int(number))

	for l in labels:
		# label_vector = np.zeros(int(number))
		# label_vector[label_map[l]] = 1
		# number_of_labels[label_map[l]] += 1
		# label_in_vector.append(label_vector.tolist())
		label_in_vector.append(label_map[l])


	# data = np.array(data)
	# for i in range(data.size[0]):
	# 	data[i] = data[i].tolist()

	data = np.empty(len(data_), dtype=np.object)

	for d in range(len(data_)):
		data[d] = data_[d]


	class_names = unique
	X_train = data[:300]
	X_test = data[300:]
	y_train = np.array(label_in_vector[:300])
	y_test = np.array(label_in_vector[300:])


	print(type(X_train))
	print(X_train.shape)
	print(type(X_train[0]))

	print(type(y_train))
	print(y_train.shape)
	print(type(y_train[0]))


	# create the model
	vector_length = 720
	model = Sequential()
	model.add(LSTM(100, input_shape=(vector_length, 1)))
	model.add(Dense(int(number), activation='sigmoid'))
	model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])
	print(model.summary())
	model.fit(X_train, y_train, validation_data=(X_test, y_test), epochs=3, batch_size=53)

	scores = model.evaluate(X_test, y_test, verbose=2)
	print("Accuracy: %.2f%%" % (scores[1]*100))
