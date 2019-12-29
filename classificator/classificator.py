import os
import sys
import glob
import numpy as np
import random
import tensorflow as tf
from numpy import mean
from numpy import std
from matplotlib import pyplot as plt
from tensorflow import keras
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense
from tensorflow.keras.layers import Dropout
from tensorflow.keras.layers import LSTM
from tensorflow.keras.callbacks import EarlyStopping
from tensorflow.keras.initializers import TruncatedNormal
from tensorflow.keras.initializers import glorot_normal

DEFAULT_TRAIN_PATH = "../out"
DEFAULT_CLASS_PATH = "../class"
DEFAULT_SAVING_PATH = "saved_models"


def load_fileset(path=DEFAULT_TRAIN_PATH):
	#loads content of files in provided directory into list
	files_dir = glob.glob(path)
	files_list = []

	for file in files_dir:
		with open(file, 'r') as fo:
			lines = fo.readlines()
			files_list.append(lines)

	return files_list


def load_single_file(path):
	#loads content of single file in provided directory into numpy array and normalizes it
	with open(path, 'r') as fo:
		lines = fo.readlines()
	data = [float(i.split(';', 2)[1]) for i in lines[2:]]
	data = [(x - min(data)) / (max(data) - min(data)) for x in data]
	data = np.array([data])
	data = np.expand_dims(data, axis=2)
	return data


def get_data_set(files_list, normalized = True):
	#gets file list and normalizes it
	#returns normalized data and labels
	label_list = []
	data_list  = []
	data_list_normalzed = []

	for file in files_list:
		label_list.append(str(file[0].rsplit('_', 1)[0]))
		data = [float(i.split(';', 2)[1]) for i in file[2:]]

		if not float('-inf') in data:
			mean     = np.mean (data)
			std_dev  = np.std  (data)
			z_score  = np.abs  ((data - mean) / std_dev)
			outliers = np.array(np.where(z_score > 3))[0]
			
			for ind in outliers:
				if ind == 0 or ind == (len(data) - 1):
					data[ind] = mean
				else:
					data[ind] = (data[ind - 1] + data[ind + 1]) / 2.0

			data_normalized = [(x - min(data)) / (max(data) - min(data)) for x in data]
			data_list.append(data)
			data_list_normalzed.append(data_normalized)

	if normalized:
		return data_list_normalzed, label_list

	return data_list, label_list


def get_labels_as_vector(label_list):
	#convert labels into vectors
	label_map = {}
	ind = 0
	uniques = list(set(label_list))

	for label in uniques:
		label_map[label] = ind
		ind += 1
	np.save(DEFAULT_SAVING_PATH + '/species_dict.npy', label_map)
	label_list = list(map(label_map.get, label_list))
	label_vect = []

	for label in label_list:
		vector = np.zeros(len(uniques))
		vector[label] = 1
		label_vect.append(vector.tolist())

	return label_vect


def get_random_indicies(data_list):
	#returns random indicies for train and test data 
	train_ind = random.sample(range(0, len(data_list)), int(0.8 * len(data_list)))
	test_ind = []
	
	for i in range(len(data_list)):
		if i not in train_ind:
			test_ind.append(i)
	
	return train_ind, test_ind


def create_model(n_input, n_output):
	#returns nn model
	model = Sequential()
	model.add( LSTM(128, input_shape = (n_input, 1), return_sequences = False,
		       kernel_initializer=TruncatedNormal(stddev=1. / np.sqrt(n_output))) )
	model.add(Dropout(0.5))
	model.add(Dense(32, activation = 'sigmoid', kernel_initializer = TruncatedNormal(stddev=1. / np.sqrt(n_output))))
	model.add(Dense(n_output, activation = 'softmax', kernel_initializer = TruncatedNormal(stddev=1. / np.sqrt(n_output))))
	model.compile(loss = 'categorical_crossentropy', optimizer = 'adam', metrics = ['accuracy'])
	return model


def evaluate_model(x_train, y_train, x_test, y_test):
	#fits data into model and trains a model
	#returns history of teaching
	verbose, epochs, batch_size = 1, 10, 10
	n_timesteps, n_outputs = x_train.shape[1], y_train.shape[1]

	model = create_model(n_timesteps, n_outputs)
	es = EarlyStopping(monitor = 'loss', mode = 'min', min_delta = 0.001)
	
	# fit network
	history = model.fit( x_train, y_train, validation_data = (x_test, y_test), epochs = epochs, batch_size = batch_size,
	                     verbose = verbose)
	
	# evaluate model
	_, train_accuracy = model.evaluate(x_train, y_train, batch_size = batch_size, verbose = 0)
	_, accuracy = model.evaluate(x_test, y_test, batch_size = batch_size, verbose = 0)

	model.save(DEFAULT_SAVING_PATH + '/model_trained.h5')

	return history, accuracy


def run_experiment(trainX, trainy, testX, testy):
	# runs an experiment and plots results
	history, score = evaluate_model(trainX, trainy, testX, testy)
	plt.plot(history.history['loss'], label = 'train')
	plt.plot(history.history['val_loss'], label = 'test')
	plt.legend()
	plt.show()
	score = score * 100.0
	print('>#result: %.3f' % (score))


def load_model(model_path=DEFAULT_SAVING_PATH+'/model_trained.h5', n_input=720, n_output=4):
	#loads saved model
	model = keras.models.load_model(model_path)
	return model


path = ""
training = False

if (len(sys.argv) >= 1):
	for i in range(1, len(sys.argv)):
		if ((sys.argv[i] == "--path") and ((len(sys.argv) - 1) >= (i + 1))): path = sys.argv[i + 1]
		if (sys.argv[i] == "-t"): training = True
		# elif ((sys.argv[i] == "-n") and ((len(sys.argv) - 1) >= (i + 1)) and not training): path = sys.argv[i + 1]

if (path == ""): 
	if (training): path = DEFAULT_TRAIN_PATH
	else: path = DEFAULT_CLASS_PATH 

if (training):
	# run the experiment
	files_list = load_fileset((path + "/*"))
	data_list, labels_list = get_data_set(files_list)
	labels_list = get_labels_as_vector(labels_list)

	train_ind, test_ind = get_random_indicies(data_list)

	x_train = np.array([data_list[i] for i in train_ind])
	x_train = np.expand_dims(x_train, axis = 2)
	y_train = np.array([labels_list[i] for i in train_ind])

	x_test = np.array([data_list[i] for i in test_ind])
	x_test = np.expand_dims(x_test, axis = 2)
	y_test = np.array([labels_list[i] for i in test_ind])
	print (x_test.shape)

	run_experiment(x_train, y_train, x_test, y_test)

else:
	#load saved model 
	model = load_model()
	data = load_single_file(path)
	pred = model.predict(data)[0]
	species_dict = np.load(DEFAULT_SAVING_PATH + '/species_dict.npy',allow_pickle='TRUE').item()
	species = species_dict.keys()
	print("Predykcja dla podanego przykladu to:")
	for spec in species:
		print(spec, ": {0:.2f}%".format(pred[species_dict[spec]] * 100))
