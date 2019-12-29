PROG	= bin/leavesradar
SCRATCH	= bin/scratchpad
CXX		= g++
OPTS	= -std=c++11 -g -Wall
CFLAGS	= $(OPTS)

INCDIR	= ./include
OBJDIR	= ./obj
BINDIR	= ./bin
SRCDIR	= ./src/leavesradar

CFLAGS	+= -I$(INCDIR) -I$(SRCDIR)

SOURCES	 =  $(wildcard $(SRCDIR)/*.cpp)
SRC_SCR  =  $(wildcard $(SRCDIR)/*.cpp)
SOURCES	 += ./src/main.cpp
SRC_SCR  += ./src/scratchpad/scratchpad.cpp

# _OBJ	= $(SOURCES:.cpp=.o)
# OBJ		= $(patsubst %,$(OBJDIR)/%,$(_OBJ))

OPENCV	= `pkg-config opencv --cflags --libs`
LIBS	= -lpthread $(OPENCV)

OUT_DIR = bin/
RESULT_DRI = out/
MKDIR_P	= mkdir -p

DEBUG	?= 0;
ifneq ($(DEBUG), 0)
	CFLAGS += -DDEBUG
endif

$(PROG):$(SRCS)
	${MKDIR_P} ${OUT_DIR}
	${MKDIR_P} ${RESULT_DRI}
	$(CXX) $(CFLAGS) -o $(PROG) $(SOURCES) $(LIBS)

scratchpad:
	${MKDIR_P} ${OUT_DIR}
	${MKDIR_P} ${RESULT_DRI}
	$(CXX) $(CFLAGS) -o $(SCRATCH) $(SRC_SCR) $(LIBS)

clean:
	rm -fR $(BINDIR)
