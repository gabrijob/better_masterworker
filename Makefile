CC	= g++  -std=c++11
CFLAGS = -Wall -g3

INSTALL_PATH = $$HOME/simgrid
INCLUDES = -I$(INSTALL_PATH)/include
DEFS = -L$(INSTALL_PATH)/lib
LDADD = -lsimgrid
DEPS = common.hpp

OBJ = better_masterworker.o common.o

all: better_masterworker

better_masterworker: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDES) $(DEFS) $(LDADD)

%.o : %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES) $(DEFS)


.PHONY: clean cleanest

clean:
	rm *.o

cleanest:
	rm better_masterworker