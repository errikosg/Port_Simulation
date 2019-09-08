#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>
#include "functions.h"
#include "PLedger.h"
#include "Port-Visual.h"
#include "queue.h"

//must be free'd always
char *charges_file;
vslot_array va;
int counter = 0;
slotData *sdata;				//data about slots of port(type, cost, capacity).

int read_flag=0;
void int_handler(int);

int main(int argc, char *argv[])
{
	int i, shmid, cflag=0, sflag=0, slot_countS, slot_countM, slot_countL, value, parkp, p_cost, waited;
	sem_t sem1;
	char *name;
	char type, upgrade;
	clock_t start, now;
	double a_time;
	struct timeval tv;


	//check given arguments
	if(argc != 5){
		printf("port-master: Error: wrong arguments given.\nExpected input:\t./port-master -c charges -s shmid\n");
		exit(1);
	}

	for(i=0; i<argc; i++){
		if(strcmp(argv[i], "-c") == 0){
			if(access(argv[i+1], F_OK) < 0){
				printf("port-master: Error: Charges file given cannot be accessed.\n");
				exit(1);
			}
			cflag = 1;
			charges_file = strdup(argv[i+1]);
		}
		if(strcmp(argv[i], "-s") == 0){
			shmid = atoi(argv[i+1]);
			if(shmid <= 0){
				printf("port-master: Error, wrong shared memory id given (cant be zero or negative!)\n");
				exit(1);
			}
			sflag = 1;
		}
	}

	if(cflag == 0){
		printf("port-master: Error: Charges file was not given.\n");
		exit(1);
	}
	if(sflag == 0){
		printf("port-master: Error: Shmid not given.\n");
		exit(1);
	}

	//printf("port-master: Charges file: %s, shmid: %d\n", charges_file, shmid);
	signal(SIGINT, int_handler);
	start = clock();

	
	//read charges file
	if(!readChargesFile(charges_file, &sdata)){
		printf("port-master: Error in reading from charges file.\n");
		exit(1);
	}

	//printf("port-master: i got\n%c-%d-%d\n%c-%d-%d\n%c-%d-%d\n\n", sdata[0].type, sdata[0].capacity, sdata[0].cost, sdata[1].type, sdata[1].capacity, sdata[1].cost, sdata[2].type, sdata[2].capacity, sdata[2].cost);
	printf("port-master: Hello, operation is now starting.\n");

	//manage shared memory
	shmData *shm;

	//attach
	if((shm = (shmData *)shmat(shmid, 0, 0)) == (shmData *)-1){
		perror("shmat p");
		exit(1);
	}

	//initliaze port-visual.
	
	slot_countS = sdata[0].capacity;
	slot_countM = sdata[1].capacity;
	slot_countL = sdata[2].capacity;
	vsarr_init(&va, slot_countS, slot_countM, slot_countL);
	int vessel_count = shm[0].vessel_count;

    //! here. Clear History.log if it exists
    if(access("History.log", F_OK) >= 0)
        unlink("History.log");

	int counter = 0;
	while(1){
		if(counter == vessel_count)
			break;

		//check exit
		sem_getvalue(&(shm[0].semExit), &value);
		if(value <= 0){
			sem_getvalue(&(shm[0].manMutex), &value);
			if(value > 0){											//if no man
				sem_post(&(shm[0].semExitNext));
				sem_post(&(shm[0].semExit));
			}
			else
				continue;

			sem_wait(&(shm[0].mutex));								//wait for vessel to write.
			if(vn_popOut(&(shm[0].vn), &name, &type, &parkp) == 0)
				printf("ERROR\n");

			//clear space of vessel
			vsarr_delete_v2(&va, name);

			sem_wait(&(shm[0].pmMutex));
			ht_delete(&(shm[0].pl), name, "History.log");
			sem_post(&(shm[0].pmMutex));
			counter++;
		}

		//check in-small
		if(vsarr_smallSpace(&va)){
			sem_getvalue(&(shm[0].semSmall), &value);
			if(value <= 0){
				sem_getvalue(&(shm[0].manMutex), &value);
				if(value > 0){											//if no man
					sem_post(&(shm[0].semSmallNext));
					sem_post(&(shm[0].semSmall));
				}
				else
					continue;

				sem_wait(&(shm[0].mutex));								//wait for vessel to write.
				vn_popIn(&(shm[0].vn), &name, &type, &upgrade, &parkp, &waited);
	
				//check upgrade type and fix.
				if(upgrade == 'M' && vsarr_mediumSpace(&va)){
					vsarr_insert(&va, upgrade, name);							//insert this vessel in port-vis.
					getData(&a_time, start, &p_cost, parkp, sdata[1].cost);

					sem_wait(&(shm[0].pmMutex));
					ht_insert(&(shm[0].pl), name, upgrade, p_cost, a_time, waited);	//insert this vessel in public ledger
					sem_post(&(shm[0].pmMutex));
				}
				else if(upgrade == 'L' && vsarr_largeSpace(&va)){
					vsarr_insert(&va, upgrade, name);							//insert this vessel in port-vis.
					getData(&a_time, start, &p_cost, parkp, sdata[2].cost);

					sem_wait(&(shm[0].pmMutex));
					ht_insert(&(shm[0].pl), name, upgrade, p_cost, a_time, waited);	//insert this vessel in public ledger
					sem_post(&(shm[0].pmMutex));
				}
				else{
					vsarr_insert(&va, type, name);								//insert this vessel in port-vis.
					getData(&a_time, start, &p_cost, parkp, sdata[0].cost);

					sem_wait(&(shm[0].pmMutex));
					ht_insert(&(shm[0].pl), name, type, p_cost, a_time, waited);	//insert this vessel in public ledger
					sem_post(&(shm[0].pmMutex));
				}
			}
		}

		//check in-medium
		if(vsarr_mediumSpace(&va)){
			sem_getvalue(&(shm[0].semMedium), &value);
			if(value <= 0){
				sem_getvalue(&(shm[0].manMutex), &value);
				if(value > 0){											//if no man
					sem_post(&(shm[0].semMediumNext));
					sem_post(&(shm[0].semMedium));
				}
				else
					continue;

				sem_wait(&(shm[0].mutex));								//wait for vessel to write.
				vn_popIn(&(shm[0].vn), &name, &type, &upgrade, &parkp, &waited);

				//check upgrade type and fix.
				if(upgrade == 'L' && vsarr_largeSpace(&va)){
					vsarr_insert(&va, upgrade, name);							//insert this vessel in port-vis.
					getData(&a_time, start, &p_cost, parkp, sdata[2].cost);

					sem_wait(&(shm[0].pmMutex));
					ht_insert(&(shm[0].pl), name, upgrade, p_cost, a_time, waited);	//insert this vessel in public ledger
					sem_post(&(shm[0].pmMutex));
				}
				else{
					vsarr_insert(&va, type, name);							//insert this vessel in port-vis.
					getData(&a_time, start, &p_cost, parkp, sdata[1].cost);

					sem_wait(&(shm[0].pmMutex));
					ht_insert(&(shm[0].pl), name, type, p_cost, a_time, waited);	//insert this vessel in public ledger
					sem_post(&(shm[0].pmMutex));
				}
			}
		}

		//check in-large
		if(vsarr_largeSpace(&va)){
			sem_getvalue(&(shm[0].semLarge), &value);
			if(value <= 0){
				sem_getvalue(&(shm[0].manMutex), &value);
				if(value > 0){											//if no man
					sem_post(&(shm[0].semLargeNext));
					sem_post(&(shm[0].semLarge));
				}
				else
					continue;

				sem_wait(&(shm[0].mutex));								//wait for vessel to write.
				vn_popIn(&(shm[0].vn), &name, &type, &upgrade, &parkp, &waited);
	
				vsarr_insert(&va, type, name);							//insert this vessel in port-vis.
				getData(&a_time, start, &p_cost, parkp, sdata[2].cost);

				sem_wait(&(shm[0].pmMutex));
				ht_insert(&(shm[0].pl), name, type, p_cost, a_time, waited);	//insert this vessel in public ledger
				sem_post(&(shm[0].pmMutex));
			}
		}
	}

	//detach
	if(shmdt(shm) == -1){
		perror("shmdt");
		exit(1);
	}	


	printf("port-master: End of operation, served %d vessels\n", counter);
	vsarr_freeAll(&va);
	free(charges_file);
	free(sdata);
	exit(0);
}

void int_handler(int sig)
{
	free(charges_file);
	vsarr_freeAll(&va);
	free(sdata);
	printf("port-master: End of operation, served %d vessels\n", counter);
	exit(0);
}
