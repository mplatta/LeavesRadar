CC		= g++
CFLAGS	= -g -Wall

SRC_DIR	= src/
OUT_DIR = bin/

SRCS	= src/main.cpp
PROG	= bin/mgr

OPENCV	= `pkg-config opencv --cflags --libs`
LIBS	= $(OPENCV)

MKDIR_P	= mkdir -p

# ${OUT_DIR}:
# 	${MKDIR_P} ${OUT_DIR}

$(PROG):$(SRCS)
	${MKDIR_P} ${OUT_DIR}
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -R bin