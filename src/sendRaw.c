/*
 * Copyright (C) 2020 Jeff Pal.
 * All rights reserved.
*/

#include "infraredRp.h"
#include <wiringPi.h>

int main (int argc, char *argv[]){

    if(argc <2)
        printf("Missing argument: raw file\nex.: sendRaw inputfile.cnf\n");
    else 
        sendRaw(readRawFile(argv[1]));
    return 0;
}
