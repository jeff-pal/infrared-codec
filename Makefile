# Copyright (C) 2020 Jeff Pal.
# All rights reserved.

CC=gcc
CFLAGS=-g -Wall
PROJECT = $(shell basename $(CURDIR))
SRC = ./src/
BUILD = ../build_${PROJECT}
BIN = ${BUILD}/bin/
GOBJS = ${BIN}xbee.o ${BIN}file_parsing.o ${BIN}transcod.o ${BIN}infrared.o
INCLUDES= -I./src
LIBS =  -lwiringPi

	

all: create_bin readIR sendRaw copy_resources
	

######## BINS

readIR: infraredRp.o readIR.o 	
	$(CC) $(CFLAGS) $(INCLUDES) $(BIN)infraredRp.o $(BIN)readIR.o -o $(BIN)$@ $(LIBS)

sendRaw: infraredRp.o sendRaw.o 	
	$(CC) $(CFLAGS) $(INCLUDES) $(BIN)infraredRp.o $(BIN)sendRaw.o -o $(BIN)$@ $(LIBS)


######## OBJECTS

readIR.o: ./src/readIR.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/readIR.c -o $(BIN)$@

sendRaw.o: ./src/sendRaw.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/sendRaw.c -o $(BIN)$@

infraredRp.o: $(SRC)infraredRp.c $(SRC)infraredRp.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/infraredRp.c -o $(BIN)$@


######## DIRECTORY OPERATIONS
create_bin:
	$(shell mkdir -p $(BIN)) #create directory if it doesn't exist.

copy_resources:
	@cp -r resources ${BUILD}

clean:
	rm $(BIN)*.o $(BIN)*. $(BIN)*~

.PHONY: create_bin copy_resources clean