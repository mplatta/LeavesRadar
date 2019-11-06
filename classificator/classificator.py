import sys
import csv
import glob
import tensorflow as tensorflow

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

if (len(sys.argv) >= 1):
	for i in range(1, len(sys.argv)):
		if ((sys.argv[i] == "--path") and ((len(sys.argv) - 1) >= (i + 1))): path = sys.argv[i + 1]
		if (sys.argv[i] == "-t"): training = True

if (path == ""): 
	if (training): path = DEFAULT_TRAIN_PATH
	else: path = DEFAULT_CLASS_PATH 

print(training)
print(path)

if (training):
	labes = []
	data = []

	files = [f for f in glob.glob(path + "**/*", recursive=True)]

	index_of_file = 0

	for f in files:
		with open(f, 'r') as csvfile:
			dataset = csv.reader(csvfile, delimiter=';')
			
			ind = 0
			tmp = []
			tmp_name = ""

			for row in dataset:
				if (ind == 0): tmp_name = extract_names(row[0])
					# print(extract_names(row[0]))
				if (ind > 1): tmp.append(row[0])
				ind += 1
			
			data.append(tmp)
			labes.append(tmp_name)

		index_of_file += 1

	# print(labes)
	# print(len(data))

	# for d in data:
	# 	print(len(d))
