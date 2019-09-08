#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <sys/types.h>
#include <semaphore.h>
#include "PLedger.h"
#include "VNotes.h"

//struct that represents config.file's data
typedef struct slotData{
	char type;
	int capacity;
	int cost;
}slotData;


//represent everything that will be put in the shared memory
typedef struct shmData{
	sem_t semSmall;		sem_t semSmallNext;
	sem_t semMedium;	sem_t semMediumNext;
	sem_t semLarge;		sem_t semLargeNext;
	sem_t semExit;		sem_t semExitNext;
	sem_t manMutex;
	sem_t mutex;
	sem_t pmMutex;				//sems
	pub_ledger pl;				//public ledger
	vnotes vn;					//notes to port-master ...
	int vessel_count;			//at vessel count port master stops functioning.
	char logfile[20];
}shmData;


//functions
int readConFile(char *, slotData **);
int makeChargesFile(slotData *);
int readChargesFile(char *, slotData **);
int makePortMaster(char *, int);
int makeMonitor(int);
int makeVessel(int);
int validPost(char, char);
void getData(double *, clock_t, int *, int, int);
int roundFloat(double);

//functions for notes.

#endif
