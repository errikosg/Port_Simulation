#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <errno.h>
#include "functions.h"
#include "PLedger.h"
#include "Port-Visual.h"
#include "queue.h"

int shmid, child_count=0;
void int_handler(int);

int main(int argc, char *argv[])
{  
	int i, value, lflag=0, status, total_slots;
	char *confile, *logfile;
	FILE *fp;
	slotData *sdata;				//data about slots of port(type, cost, capacity).
	key_t key;
	srand(time(NULL));

	//analyzing input
	if(argc != 3){
		printf("myport: Error: Wrong arguments given.\nCorrect input: ./myport -l configfile\n");
		exit(1); 
	}

	for(i=0; i<argc; i++){
		if(strcmp(argv[i], "-l") == 0){
			if(access(argv[i+1], F_OK) < 0){
				printf("myport: Error: Configuration file given cannot be accessed.\n");
				exit(1);
			}
			lflag = 1;
			confile = strdup(argv[i+1]);
		}
	}
	if(lflag == 0){
		printf("myport: Error: Configuration file was not given.\n");
		exit(1);
	}

	
	//read configuration file, extract data and setup logfile.
	if(!readConFile(confile, &sdata)){
		printf("myport: Error in reading from configuration file.\n");
		exit(1);
	}

	//printf("myport: i got\n%c-%d-%d\n%c-%d-%d\n%c-%d-%d\n\n", sdata[0].type, sdata[0].capacity, sdata[0].cost, sdata[1].type, sdata[1].capacity, sdata[1].cost, sdata[2].type, sdata[2].capacity, sdata[2].cost);



	total_slots = sdata[0].capacity + sdata[1].capacity + sdata[2].capacity;

	if(!makeChargesFile(sdata)){									//make charges file
		printf("myport: Error in making Charges.txt\n");
		exit(2);
	}
	signal(SIGINT, int_handler);

	//Make shared memory segment and manage data
	shmData sd;
	shmData *shm;
	if((key = ftok("/tmp", 'a')) == -1){
		perror("ftok shm");
		exit(1);
	}

	if((shmid = shmget(key, sizeof(shmData), IPC_CREAT | IPC_EXCL | 0660)) == -1){
		perror("shmget p");
		exit(1);
	}

	//attach
	if((shm = (shmData *)shmat(shmid, 0, 0)) == (shmData *)-1){
		perror("shmat p");
		exit(1);
	} 

	//set / init semaphores and structs.
	sem_init(&(sd.semSmall), 1, 1);
	sem_init(&(sd.semMedium), 1, 1);
	sem_init(&(sd.semLarge), 1, 1);
	sem_init(&(sd.semSmallNext), 1, 0);
	sem_init(&(sd.semMediumNext), 1, 0);
	sem_init(&(sd.semLargeNext), 1, 0);
	sem_init(&(sd.semExit), 1, 1);
	sem_init(&(sd.semExitNext), 1, 0);
	sem_init(&(sd.manMutex), 1, 1);
	sem_init(&(sd.mutex), 1, 0);
	sem_init(&(sd.pmMutex), 1, 1);

	ht_initialize(&(sd.pl));
	vn_init(&(sd.vn));
	sd.vessel_count = rand() % 10 + 15;				//exei dokimastei mexri 150 vessels.
    if(access("logfile.log", F_OK) >= 0)
        unlink("logfile.log");
	strcpy(sd.logfile, "logfile.log");

	memcpy(&shm[0], &sd, sizeof(shmData));

	
	//make all processes !
	//make port master
	int pm_pid, mon_pid, ves_pid[2], pd;
	if((pm_pid = makePortMaster("Charges.txt", shmid)) < 0){
		printf("myport: Error in making Port Master process. Exiting...\n");
		exit(2);
	}
	
	//make monitor
	if((mon_pid = makeMonitor(shmid)) < 0){
		printf("myport: Error in making Monitor process. Exiting...\n");
		exit(2);
	}
	//make vessels
	for(i=0; i<sd.vessel_count; i++){
		if(i % 3 == 0)
			sleep(1);
		if((ves_pid[i] = makeVessel(shmid)) < 0){
			printf("myport: Error in making Vessel process. Exiting...\n");
			exit(2);
		}
	}

	//wait all children
	while((pd = wait(&status)) > 0){
		child_count++;
		if(child_count == sd.vessel_count)
			kill(mon_pid, SIGINT);
	}

	sem_destroy(&(sd.semSmall));
	sem_destroy(&(sd.semMedium));
	sem_destroy(&(sd.semLarge));
	sem_destroy(&(sd.semSmallNext));
	sem_destroy(&(sd.semMediumNext));
	sem_destroy(&(sd.semLargeNext));
	sem_destroy(&(sd.semExit));
	sem_destroy(&(sd.semExitNext));
	sem_destroy(&(sd.manMutex));
	sem_destroy(&(sd.mutex));
	sem_destroy(&(sd.pmMutex));

	//detach
	if(shmdt(shm) == -1){
		perror("shmdt");
		exit(1);
	}


	//Delete shmem - clear space
	if(shmctl(shmid, IPC_RMID, NULL) == -1){
		perror("shmctl-rm");
		exit(1);
	}
	free(sdata);
	free(confile);

	printf("myport: bye bye. (%d)\n", child_count);
	exit(0);
}

void int_handler(int sig)
{
	//Delete shmem - clear space
	if(shmctl(shmid, IPC_RMID, NULL) == -1){
		perror("shmctl-rm");
		exit(1);
	}
	printf("myport exiting, %d\n", child_count);
	exit(0);
}
