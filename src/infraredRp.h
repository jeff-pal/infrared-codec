/*
 * Copyright (C) 2020 Jeff Pal.
 * All rights reserved.
*/

#ifndef INFRAREDRP_H
#define INFRAREDRP_h

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

void testRead(void);
void setupGPIO(const char *port, const char *mode);
int readGPIO(const char *port);
void readIR(void);
void generateRawIr(void);
void sendRaw(unsigned int *raw);
int getLength(unsigned int* array);
unsigned int * readRawFile(char *file);

char command[100], message[36], runLCD[20];
FILE *gpio;
char input[3], file[100];

#endif
