#
# Makefile IMS
#

CXX=g++ -m64
CXXFLAGS  = -Wall -std=c++98
CXXFLAGS += -g  # debugging info
CXXFLAGS += -O2 # optimization level
#CXXFLAGS += -pg # profiling support

SIMLIB_DIR = /usr/local/include
SIMLIB_DIR_SO = /usr/local/lib

CXXFLAGS += -I$(SIMLIB_DIR)

SIMLIB_DEPEND = $(SIMLIB_DIR)/simlib.h \
		$(SIMLIB_DIR)/delay.h \
		$(SIMLIB_DIR)/zdelay.h \
		$(SIMLIB_DIR)/simlib2D.h \
		$(SIMLIB_DIR)/simlib3D.h \
		$(SIMLIB_DIR_SO)/libsimlib.so 

% : %.cc  $(SIMLIB_DEPEND)
	$(CXX) $(CXXFLAGS) -o $@  $< $(SIMLIB_DIR_SO)/libsimlib.so -lm


# list of all test models
INPUT_FILE = src/main

#############################################################################
# RULES

all: $(INPUT_FILE)

#############################################################################
# cleaning, backup, etc

clean: 
	rm -f $(INPUT_FILE) *.o *~

clean-all: clean
	rm -f *.dat *.out

pack:
	tar czf ims.tar.gz  *.cc Makefile* *.txt *.output *.plt

# end of Makefile
