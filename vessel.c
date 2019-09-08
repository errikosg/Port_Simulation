#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/time.h>
#include "functions.h"
#include "PLedger.h"
#include "Port-Visual.h"
#include "queue.h"

//must be free'd always
char *vessel_name;
void int_handler(int);

FILE *fp;

int main(int argc, char *argv[])
{
	int i, shmid, tflag=0, uflag=0, pflag=0, mflag=0, sflag=0, parkperiod, mantime, value, charge, waited;
	char type, postype;
	sem_t sem1;
	int start, enter, end;
	struct timeval tv;
	

	//check given arguments
	if(argc != 11){
		printf("vessel: Error: wrong arguments given.\nExpected input:\t./vessel -t type -u postype -p parkperiod -m mantime -s shmid\n");
		exit(1);
	}

	for(i=0; i<argc; i++){
		if(strcmp(argv[i], "-t") == 0){
			type = *(argv[i+1]);
			if(type != 'S' && type != 'M' && type != 'L'){
				printf("vessel: Error, type can be only 'S', 'M' or 'L'.\n");
				exit(1);
			}
			tflag = 1;
		}
		if(strcmp(argv[i], "-u") == 0){
			postype = *(argv[i+1]);
			if(postype != 'S' && postype != 'M' && postype != 'L'){
				printf("vessel: Error, postype can be only 'S', 'M' or 'L'.\n");
				exit(1);
			}
			uflag = 1;
		}
		if(strcmp(argv[i], "-p") == 0){
			parkperiod = atoi(argv[i+1]);
			if(parkperiod <= 0){
				printf("vessel: Error, wrong parkperiod given (cant be zero or negative!)\n");
				exit(1);
			}
			pflag = 1;
		}
		if(strcmp(argv[i], "-m") == 0){
			mantime = atoi(argv[i+1]);
			if(mantime <= 0){
				printf("vessel: Error, wrong mantime given (cant be zero or negative!)\n");
				exit(1);
			}
			mflag = 1;
		}
		if(strcmp(argv[i], "-s") == 0){
			shmid = atoi(argv[i+1]);
			if(shmid <= 0){
				printf("vessel: Error, wrong shared memory id given (cant be zero or negative!)\n");
				exit(1);
			}
			sflag = 1;
		}
	}

	if(tflag == 0){
		printf("vessel: Error: Vessel type was not given.\n");
		exit(1);
	}
	if(uflag == 0){
		printf("vessel: Error: Vessel postype was not given.\n");
		exit(1);
	}
	if(pflag == 0){
		printf("vessel: Error: Park period was not given.\n");
		exit(1);
	}
	if(mflag == 0){
		printf("vessel: Error: Man time  was not given.\n");
		exit(1);
	}
	if(sflag == 0){
		printf("vessel: Error: Shmid was not given.\n");
		exit(1);
	}
	if(!validPost(type, postype)){
		printf("vessel: Error, postype has to be greater(or equal to) given type.\n");
		exit(1);
	}

	vessel_name = malloc(13*sizeof(char));
	sprintf(vessel_name, "Vessel_%d", getpid());
	//printf("\t%s: I got %c, %c, %d, %d, %d\n", vessel_name, type, postype, parkperiod, mantime, shmid);
	signal(SIGINT, int_handler);

	//manage shared memory
	shmData *shm;
	pub_ledger pl;

	//attach
	if((shm = (shmData *)shmat(shmid, 0, 0)) == (shmData *)-1){
		perror("shmat p");
		exit(1);
	}

	//open logfile
	if((fp = fopen(shm[0].logfile, "a")) == NULL){
		perror("fopen");
		exit(1);
	}

	//wait at queue
	gettimeofday(&tv, NULL);
	start = tv.tv_sec;
	if(type == 'S'){
		sem_wait(&(shm[0].semSmall));
		sem_wait(&(shm[0].semSmallNext));
	}
	else if(type == 'M'){
		sem_wait(&(shm[0].semMedium));
		sem_wait(&(shm[0].semMediumNext));
	}
	else{
		sem_wait(&(shm[0].semLarge));
		sem_wait(&(shm[0].semLargeNext));
	}

	gettimeofday(&tv, NULL);
	enter = tv.tv_sec;
	waited = (enter-start);	
	fprintf(fp, "%s entered the port at %d seconds\n", vessel_name, enter);

	//man - in a mutex
	sem_wait(&(shm[0].manMutex));
	sleep(mantime);
	sem_post(&(shm[0].manMutex));

	vn_pushIn(&(shm[0].vn), vessel_name, type, postype, parkperiod, waited);
	sem_post(&(shm[0].mutex));									//wake up port-master !!

	//enter port - sleep
	sleep(parkperiod);

	//exiting
	sem_wait(&(shm[0].semExit));
	sem_wait(&(shm[0].semExitNext));


	//man - in a mutex
	sem_wait(&(shm[0].manMutex));
	sleep(mantime);
	sem_post(&(shm[0].manMutex));

	vn_pushOut(&(shm[0].vn), vessel_name, type, parkperiod);
	charge = ht_findCharge(&(shm[0].pl), vessel_name);

	gettimeofday(&tv, NULL);
	end = tv.tv_sec;
	fprintf(fp, "%s left the port at %d seconds with cost %d (total time spent = %d)\n", vessel_name, end, charge, (end-start));
	sem_post(&(shm[0].mutex));									//wake up port-master !!

	//detach
	if(shmdt(shm) == -1){
		perror("shmdt");
		exit(1);
	}


	//clear space
	fclose(fp);
	free(vessel_name);
	exit(0);
}

void int_handler(int sig)
{
	free(vessel_name);
	fclose(fp);
	printf("vessel exiting..\n");
	exit(0);
}
