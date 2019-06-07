CC		= g++
CFLAGS	= -g -Wall

SRC_DIR	= src/
OUT_DIR = bin/

SRCS	= src/*.cpp
# SRCS_H	= src/*.hpp
PROG	= bin/mgr

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

clean:
	rm -R bin