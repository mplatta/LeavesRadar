# LeavesRadar
Program to extract contours from leaves and cast it to histograms

cartographer - class to find and hold contours<br/>
folding rule - class to find and hold histogram of distance between point "zero" and contours<br/>

## Make options:
* Build program in debug mode:
```sh
$ make
```

* Build program in release mode:
```sh
$ make DEBUG=0
```

* Build scratchpad:
```sh
$ make scratchpad
```

* Clean project:
```sh
$ make clean
```

## How to run
Navigate to bin/ file and run:

* for generate histogram for all image in path:
```sh
$ ./leavesradar --path ../dir/ [--out] [/path/to/out/dir]
```

* for generate histogram from one image:
```sh
$ ./leavesradar --file ../dir/image.jpg [--out] [/path/to/out/dir]
```

Default out path is /out

### Scratchpad
Place to test any indeas and solutions, only for local repo


# Classificator
Simple neural network

## How to run

* training network
```sh
$ python3 --path path/to/histograms -t
```

* classification
```sh
$ python3 --path path/to/histogra.txt
```