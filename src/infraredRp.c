/*
 * Copyright (C) 2020 Jeff Pal.
 * All rights reserved.
*/

#include "infraredRp.h"

int readGPIO(const char *port){

  sprintf(file, "/sys/class/gpio/gpio%s/value", port);
  while ((gpio = fopen(file, "rb")) == NULL);

  fseek(gpio, 0, SEEK_SET);
  fread(input, sizeof(char), 3, gpio);
  fflush(gpio);
  fclose(gpio);
  return atoi(input);
}

void setupGPIO(const char *port, const char *mode)
{

  while((gpio = fopen("/sys/class/gpio/export", "wb")) == NULL);
  fprintf(gpio, port);
  fclose(gpio);

  sprintf(file, "/sys/class/gpio/gpio%s/direction", port);
  
  while((gpio = fopen(file, "rb+")) == NULL);

  fprintf(gpio, mode);
  fclose(gpio);
}

void readIR(void)
{
  if (wiringPiSetup() == -1)
  exit(1);

  printf("1. Starting IR reading\n");
  FILE *gpio18, *rawfile;
    
  setupGPIO("18", "in");
  while((gpio18 = fopen("/sys/class/gpio/gpio18/value", "rb")) == NULL);

  while((rawfile = fopen("sensorData.rec", "w")) == NULL);

  int i=0;
  char raw[100070], buffer[3];
  int NZs = 20;   
  int DELz = 20;  
  while(i<NZs) {

    fread(buffer, sizeof(char), 3, gpio18);
    fseek(gpio18, 0, SEEK_SET);

    i=0;
    while(buffer[0]=='0' && i<NZs) {
     
      raw[i]=buffer[0];
      delayMicroseconds(DELz);
      fread(buffer, sizeof(char), 3, gpio18);
      fseek(gpio18, 0, SEEK_SET);

      i++;
    }
  }

  printf("Reading signal...\n");
  while(i<20000){
    fread(buffer, sizeof(char), 3, gpio18);
    raw[i]=buffer[0];
    delayMicroseconds(DELz);
    fseek(gpio18, 0, SEEK_SET);

    i++;
  } 

  fprintf(rawfile, "%s", raw );
  fclose(gpio18);
  fclose(rawfile);
  printf("Finishing IR reading!\n");
  generateRawIr();
}

void generateRawIr(void)
{
  printf("2. Starting raw generation\n");
  FILE *sensorDataFile, *rawFile;

  sensorDataFile = fopen("sensorData.rec", "r");
  rawFile = fopen("raw.cnf", "w");
  char character;
  unsigned int *generic_code; 
  generic_code = malloc (sizeof (unsigned int));

  if (sensorDataFile == NULL || rawFile == NULL)
  {
      printf("Error Open Files\n");
      exit (0);
  }

  char flag;
  int i=1;
  unsigned int count=0;
  int status=fscanf(sensorDataFile, "%c", &character );
  flag=character;
   
  while (status==1)
  {
    while(character==flag && status==1)
      {
      count++;
      status=fscanf(sensorDataFile, "%c", &character );
    }

    if(count>0)
    {
      generic_code = (unsigned int*)realloc (generic_code, i * sizeof (unsigned int));
      generic_code[i-1]=count;
      
      i++;
      flag=character;
          count=0;
      }
  }
  int j, sampleDurationInNano=28200, lagInNano=100000;
  for(j=0;j<(i-2);j++){
    generic_code[j]=(generic_code[j]*sampleDurationInNano)-lagInNano;
          fprintf(rawFile, "%u ", generic_code[j]);
  }
  sendRaw(generic_code);
  fclose(sensorDataFile);
  fclose(rawFile);
  printf("Finishing raw generation\n");
}

int getLength(unsigned int* array){
  int i=0;
  while (array[i]) {
    i++;
  }
  return i;
} 

void sendRaw(unsigned int *raw)
{
  if (wiringPiSetup() == -1)
    exit(1);

  printf("3. Starting raw sending\n");

  int elements=getLength(raw);
  int i,level = 11, pwmPin=26;

  struct timespec tim;
  tim.tv_sec = 0;

  pinMode(pwmPin,PWM_OUTPUT);
  pwmSetMode(PWM_MODE_MS);
  pwmSetClock(23); 
  pwmSetRange (22);
  printf("Length: %d\n", elements);

//RAW -----------------------------------------------------//    
  for (i = 0; i < elements; i++)
  { 
    pwmWrite (pwmPin, level);  
    tim.tv_nsec=raw[i];
    nanosleep(&tim ,NULL);
    level = 11-level;
  }
//END RAW ------------------------------------------------//
  pwmWrite (pwmPin, 0);
  printf("Finishing raw sending\n");
}

unsigned int * readRawFile(char *file)
{

  FILE *raw_file;
  char str_code[10];

  int i=1;
  unsigned int *generic_code;
  generic_code = malloc (sizeof (unsigned int));

  if ((raw_file = fopen(file, "rb")) == NULL){
      printf("Cannot open value file: %s.\n", file);
      exit(1);
  }
  
  while(fscanf(raw_file, "%s", str_code)==1) {
    if(strcmp(str_code, "\0") == 0)
      break;

    generic_code = (unsigned int*)realloc (generic_code, i * sizeof (unsigned int));
    generic_code[i-1]=(unsigned int)strtoul(str_code, NULL, 10);
    i++;
  }
  fclose(raw_file);
  return generic_code;
}
