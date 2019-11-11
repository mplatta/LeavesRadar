import sys
import csv
import glob
import tensorflow as tf
import matplotlib.pyplot as plt
from tensorflow import keras
import numpy as np
import random

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
	data = []

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
			
			data.append(tmp)
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


	class_names = unique
	train_images = np.array(data[:300])
	test_images = np.array(data[300:])
	train_labels = np.array(label_in_vector[:300])
	test_labels = np.array(label_in_vector[300:])

	uu = list(set(test_labels))

	label_ma = {}
	indd = 0

	for u in uu:
		label_ma[u] = indd
		indd += 1 

	for t in test_labels:
		number_of_labels[label_ma[t]] += 1

	model = keras.Sequential([
	    keras.layers.Dense(720, input_shape=(720,)),
	    keras.layers.Dense(100, activation='sigmoid'),
		# keras.layers.SimpleRNN(2, activation='tanh', use_bias=True, kernel_initializer='glorot_uniform', recurrent_initializer='orthogonal', bias_initializer='zeros', kernel_regularizer=None, recurrent_regularizer=None, bias_regularizer=None, activity_regularizer=None, kernel_constraint=None, recurrent_constraint=None, bias_constraint=None, dropout=0.0, recurrent_dropout=0.0, return_sequences=False, return_state=False, go_backwards=False, stateful=False, unroll=False),
	    keras.layers.Dense(number, activation='softmax')
	])

	model.compile(optimizer='RMSprop',
	              loss='sparse_categorical_crossentropy',
	              metrics=['accuracy'])


	model.fit(train_images, train_labels, epochs=100)


	test_loss, test_acc = model.evaluate(test_images,  test_labels, verbose=2)

	print('\nTest accuracy:', test_acc)

	# data_list = data
	# labels_list = label_in_vector

	# # ------------------------------------------------------------------------------------------

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
