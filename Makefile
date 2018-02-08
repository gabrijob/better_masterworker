CC	= g++  -std=c++11
CFLAGS = -Wall -g3

INSTALL_PATH = $$HOME/simgrid
INCLUDES = -I$(INSTALL_PATH)/include
DEFS = -L$(INSTALL_PATH)/lib
LDADD = -lsimgrid

all: better_masterworker

better_masterworker: better_masterworker.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -o $@ $^ $(LDADD)


.PHONY: clean cleanest

clean:
	rm *.o

cleanest:
	rm better_masterworker