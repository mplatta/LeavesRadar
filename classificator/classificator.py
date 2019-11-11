import os
import sys
import glob
import numpy as np
import random
import tensorflow as tf
from numpy import mean
from numpy import std
from matplotlib import pyplot as plt
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense
from tensorflow.keras.layers import Dropout
from tensorflow.keras.layers import LSTM
from tensorflow.keras.callbacks import EarlyStopping
from tensorflow.keras.initializers import TruncatedNormal

DEFAULT_TRAIN_PATH = "../out"
DEFAULT_CLASS_PATH = "../class"
CHECKPOINT_PATH = "checkpoints/cp.ckpt"

def load_fileset(path=DEFAULT_TRAIN_PATH):
	files_dir = glob.glob(path)
	files_list = []

	for file in files_dir:
		with open(file, 'r') as fo:
			lines = fo.readlines()
			files_list.append(lines)

	return files_list


def get_data_set(files_list, normalized = True):
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
	label_map = {}
	ind = 0
	uniques = list(set(label_list))

	for label in uniques:
		label_map[label] = ind
		ind += 1

	label_list = list(map(label_map.get, label_list))
	label_vect = []

	for label in label_list:
		vector = np.zeros(len(uniques))
		vector[label] = 1
		label_vect.append(vector.tolist())

	return label_vect


def get_random_indicies(data_list):
	train_ind = random.sample(range(0, len(data_list)), int(0.8 * len(data_list)))
	test_ind = []
	
	for i in range(len(data_list)):
		if i not in train_ind:
			test_ind.append(i)
	
	return train_ind, test_ind


# fit and evaluate a model
def evaluate_model(x_train, y_train, x_test, y_test, checkpoint_path):
	verbose, epochs, batch_size = 1, 20, 10
	n_timesteps, n_outputs = x_train.shape[1], y_train.shape[1]
	
	cp_callback = tf.keras.callbacks.ModelCheckpoint(filepath = checkpoint_path, save_weights_only = True, verbose = 1)

	model = Sequential()
	model.add( LSTM(128, input_shape = (n_timesteps, 1), return_sequences = False,
		       kernel_initializer=TruncatedNormal(stddev=1. / np.sqrt(n_outputs))) )
	model.add(Dropout(0.5))
	model.add(Dense(32, activation = 'sigmoid', kernel_initializer = TruncatedNormal(stddev = 1. / np.sqrt(n_outputs))))
	model.add(Dense(n_outputs, activation = 'softmax', kernel_initializer = TruncatedNormal(stddev = 1. / np.sqrt(n_outputs))))
	model.compile(loss = 'categorical_crossentropy', optimizer = 'adam', metrics = ['accuracy'])
	es = EarlyStopping(monitor = 'acc', mode = 'max', min_delta = 0.001)
	
	# fit network
	history = model.fit( x_train, y_train, validation_data = (x_test, y_test), epochs = epochs, batch_size = batch_size,
	                     verbose = verbose, callbacks = [cp_callback] ) #, callbacks=[es]
	
	# evaluate model
	_, train_accuracy = model.evaluate(x_train, y_train, batch_size = batch_size, verbose = 0)
	_, accuracy = model.evaluate(x_test, y_test, batch_size = batch_size, verbose = 0)
	return history, accuracy


# run an experiment
def run_experiment(trainX, trainy, testX, testy, checkpoint_path):
	history, score = evaluate_model(trainX, trainy, testX, testy, checkpoint_path)
	plt.plot(history.history['loss'], label = 'train')
	plt.plot(history.history['val_loss'], label = 'test')
	plt.legend()
	plt.show()
	score = score * 100.0
	print('>#result: %.3f' % (score))


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
	checkpoint_dir = os.path.dirname(CHECKPOINT_PATH)

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

	run_experiment(x_train, y_train, x_test, y_test, CHECKPOINT_PATH)
else:
	pass
	# Loads the weights
	# model.load_weights(CHECKPOINT_PATH)

	# # Re-evaluate the model
	# loss,acc = model.evaluate(test_images,  test_labels, verbose=2)
	# print("Restored model, accuracy: {:5.2f}%".format(100*acc))



































	## OLD VERSIONs
	# labels = []
	# data = []

	# files = [f for f in glob.glob(path + "**/*", recursive=True)]

	# for f in files:
	# 	with open(f, 'r') as csvfile:
	# 		dataset = csv.reader(csvfile, delimiter=';')
			
	# 		ind = 0
	# 		tmp = []
	# 		tmp_name = ""

	# 		for row in dataset:
	# 			if (ind == 0): tmp_name = extract_names(row[0])
	# 			if (ind > 1): tmp.append(float(row[0]))
	# 			ind += 1
			
	# 		data.append(tmp)
	# 		labels.append(tmp_name)

	# unique = list(set(labels))

	# label_map = {}
	# ind = 0

	# for u in unique:
	# 	label_map[u] = ind
	# 	ind += 1 

	# number_of_labels = np.zeros(int(number))

	# for l in labels:
	# 	# label_vector = np.zeros(int(number))
	# 	# label_vector[label_map[l]] = 1
	# 	# number_of_labels[label_map[l]] += 1
	# 	# label_in_vector.append(label_vector.tolist())
	# 	label_in_vector.append(label_map[l])


	# class_names = unique
	# train_images = np.array(data[:300])
	# test_images = np.array(data[300:])
	# train_labels = np.array(label_in_vector[:300])
	# test_labels = np.array(label_in_vector[300:])

	# uu = list(set(test_labels))

	# label_ma = {}
	# indd = 0

	# for u in uu:
	# 	label_ma[u] = indd
	# 	indd += 1 

	# for t in test_labels:
	# 	number_of_labels[label_ma[t]] += 1

	# model = keras.Sequential([
	#     keras.layers.Dense(720, input_shape=(720,)),
	#     keras.layers.Dense(100, activation='sigmoid'),
	# 	# keras.layers.SimpleRNN(2, activation='tanh', use_bias=True, kernel_initializer='glorot_uniform', recurrent_initializer='orthogonal', bias_initializer='zeros', kernel_regularizer=None, recurrent_regularizer=None, bias_regularizer=None, activity_regularizer=None, kernel_constraint=None, recurrent_constraint=None, bias_constraint=None, dropout=0.0, recurrent_dropout=0.0, return_sequences=False, return_state=False, go_backwards=False, stateful=False, unroll=False),
	#     keras.layers.Dense(number, activation='softmax')
	# ])

	# model.compile(optimizer='RMSprop',
	#               loss='sparse_categorical_crossentropy',
	#               metrics=['accuracy'])


	# model.fit(train_images, train_labels, epochs=100)


	# test_loss, test_acc = model.evaluate(test_images,  test_labels, verbose=2)

	# print('\nTest accuracy:', test_acc)

	# data_list = data
	# labels_list = label_in_vector

	# # --------------------OLDER----------------------------------------------------------------------

	# train_ind = random.sample(range(0, len(data_list)), int(0.8*len(data_list)))
	# test_ind = []
	# for i in range(len(data_list)):
	# 	if i not in train_ind:
	# 		test_ind.append(i)
	# print(test_ind)

	# x_train = [data_list[i] for i in train_ind]
	# y_train = [labels_list[i] for i in train_ind]

	# x_val = [data_list[i] for i in test_ind]
	# y_val = [labels_list[i] for i in test_ind]

	# x_test = [data_list[i] for i in test_ind]
	# y_test = [labels_list[i] for i in test_ind]



	# # Model sieci

	# input_length = len(data_list[0])
	# num_classes = 4

	# x = tf.placeholder(tf.float32, [None, input_length])
	# y_prim = tf.placeholder(tf.float32, [None, num_classes])

	# ile_neuronow = 200
	# ile_neuronow2 = 400
	# ile_neuronow3 = 100

	# ####    Warstwa 1 ukryta:  200 neuronów

	# wagi1 = tf.Variable(tf.truncated_normal([input_length,ile_neuronow], stddev=1./math.sqrt(input_length)))
	# bias1 = tf.Variable(tf.random_normal([ile_neuronow]))
	# z1 = tf.add(tf.matmul(x,wagi1), bias1)
	# a1 = tf.nn.relu(z1)
	# dropout1 = tf.nn.dropout(a1, keep_prob=0.5)

	# ####    Warstwa 2 ukryta:  400 neuronów

	# wagi2 = tf.Variable(tf.truncated_normal([ile_neuronow,ile_neuronow2], stddev=1./math.sqrt(ile_neuronow)))
	# bias2 = tf.Variable(tf.random_normal([ile_neuronow2]))
	# z2 = tf.add(tf.matmul(dropout1,wagi2), bias2)
	# a2 = tf.nn.sigmoid(z2)
	# dropout2 = tf.nn.dropout(a2, keep_prob=0.3)

	# ####    Warstwa 3 ukryta:  100 neuronów

	# wagi3 = tf.Variable(tf.truncated_normal([ile_neuronow2,ile_neuronow3], stddev=1./math.sqrt(ile_neuronow2)))
	# bias3 = tf.Variable(tf.random_normal([ile_neuronow3]))
	# z3 = tf.add(tf.matmul(dropout2 ,wagi3), bias3)
	# a3 = tf.nn.relu(z3)


	# ####    Warstwa wyjsciowa: 4 neuronów wyjściowych

	# wagi_out = tf.Variable(tf.truncated_normal([ile_neuronow3, num_classes], 1./math.sqrt(num_classes)))
	# bias_out = tf.Variable(tf.random_normal([num_classes]))
	# y = tf.add(tf.matmul(a3,wagi_out), bias_out)


	# ####    Funkcja kosztu: cross entropy

	# f_bledu = tf.nn.softmax_cross_entropy_with_logits(labels=y_prim, logits=y)
	# koszt = tf.reduce_mean(f_bledu)


	# ####    Szukanie globalnego minimum funkcji kosztu metodą gradientu

	# optymalizuj = tf.train.GradientDescentOptimizer(0.001).minimize(koszt)


	# ####    Zainicjowanie sesji obliczeniowej

	# sesja = tf.InteractiveSession()
	# inicjuj=tf.global_variables_initializer()
	# sesja.run(inicjuj)


	# ####   Sprawdzenie czy wynik odpowiada wartości y

	# dobry_wynik = tf.equal(tf.argmax(y,1), tf.argmax(y_prim,1))
	# poprawnosc = tf.reduce_mean(tf.cast(dobry_wynik, tf.float32))

	# ile_epok = 100
	# rozmiar_batcha = 10
	# ile_batchow = int(len(y_train) / rozmiar_batcha)
	# print(len(data_list))

	# ####    Listy do przechowywnia wyników uczenia

	# train_popr = []  # poprawność klasyfikacji na zbiorze uczącym
	# valid_popr = []  # poprawność klasyfikacji na zbiorze sprawdzającym
	# koszt_avg = []  # wartośc funkcji kosztu dla poszczególnych epok uczenia

	# for epoka in range(ile_epok):
	# 	koszt_suma = 0.
	# 	epoka_popr = []
	# 	for i in range(ile_batchow):
	# 		x_b = x_train[(i*rozmiar_batcha):(i*rozmiar_batcha+rozmiar_batcha)]
	# 		y_b = y_train[(i*rozmiar_batcha):(i*rozmiar_batcha+rozmiar_batcha)]
	# 		A = poprawnosc.eval(feed_dict={x: x_b, y_prim: y_b})
	# 		epoka_popr.append(A)

	# 		C = koszt.eval(feed_dict={x: x_b, y_prim: y_b}) / rozmiar_batcha
	# 		koszt_suma += C

	# 		sesja.run(optymalizuj, feed_dict={x: x_b, y_prim: y_b})

	# 	T = sesja.run(poprawnosc, feed_dict={x: x_val, y_prim: y_val})
	# 	valid_popr.append(T)
	# 	train_popr.append(np.average(epoka_popr))
	# 	koszt_avg.append(koszt_suma)
	# 	print ("Epoch ", epoka, ", koszt: ", koszt_suma )

	# popr = sesja.run(poprawnosc, feed_dict={x: x_test, y_prim: y_test})
	# print ("Poprawnosc modelu, na zbiorze testowym: ", popr)
	# valid_popr = np.array(valid_popr) * 100
	# train_popr = np.array(train_popr) * 100
	# plt.figure(1)
	# plt.subplot(211)
	# plt.plot(valid_popr, 'r', label='Validation accuracy')
	# plt.plot(train_popr, 'b', label='Training accuracy')
	# plt.xlabel('Training epoch')
	# plt.ylabel('Percentage accuracy')
	# plt.legend()

	# plt.subplot(212)
	# plt.plot(koszt_avg, 'g', label='Cost Function')
	# plt.xlabel('Training epoch')
	# plt.ylabel('Cost function Value')
	# plt.legend()

	# plt.show()