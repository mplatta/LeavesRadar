CC		= g++
CFLAGS	= -g -Wall

SRC_DIR	= src/
OUT_DIR = bin/

SRCS		= src/*.cpp
SRC_SCRATCH	= src/scratchpad.cpp

PROG	= bin/mgr
SCRATCH = bin/scratchpad

OPENCV	= `pkg-config opencv --cflags --libs`
LIBS	= $(OPENCV)

DEBUG	?= 0;
ifneq ($(DEBUG), 0)
	CFLAGS += -DDEBUG
endif

MKDIR_P	= mkdir -p

$(PROG):$(SRCS)
	${MKDIR_P} ${OUT_DIR}
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

scratchpad:
	${MKDIR_P} ${OUT_DIR}
	$(CC) $(CFLAGS) -o $(SCRATCH) $(SRC_SCRATCH) $(LIBS)

clean:
	rm -R bin