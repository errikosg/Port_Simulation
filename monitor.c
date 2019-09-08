#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include "functions.h"
#include "PLedger.h"
#include "Port-Visual.h"
#include "queue.h"

Queue q;
void int_handler(int);


int main(int argc, char *argv[])
{
	int i, shmid, time, stattime, tflag=0, stflag=0, sflag=0;

	//check arguments
	if(argc != 7){
		printf("monitor: Error: wrong arguments given.\nExpected input:\t./monitor -d time -t stattimes -s shmid\n");
		exit(1);
	}

	for(i=0; i<argc; i++){
		if(strcmp(argv[i], "-d") == 0){
			time = atoi(argv[i+1]);
			if(time <= 0){
				printf("monitor: Error, wrong time value given (cant be zero or negative!)\n");
				exit(1);
			}
			tflag = 1;
		}
		if(strcmp(argv[i], "-t") == 0){
			stattime = atoi(argv[i+1]);
			if(stattime <= 0){
				printf("monitor: Error, wrong stattime value given (cant be zero or negative!)\n");
				exit(1);
			}
			stflag = 1;
		}
		if(strcmp(argv[i], "-s") == 0){
			shmid = atoi(argv[i+1]);
			if(shmid <= 0){
				printf("monitor: Error, wrong shared memory id given (cant be zero or negative!)\n");
				exit(1);
			}
			sflag = 1;
		}
	}

	if(tflag == 0){
		printf("monitor: Error: Time was not given.\n");
		exit(1);
	}
	if(stflag == 0){
		printf("monitor: Error: Stat time type was not given.\n");
		exit(1);
	}
	if(sflag == 0){
		printf("monitor: Error: Shmid was not given.\n");
		exit(1);
	}

	printf("monitor: I got %d, %d, %d\n", time, stattime, shmid);


	//attach to shared mem ...
	signal(SIGINT, int_handler);
	shmData *shm;

	//attach
	if((shm = (shmData *)shmat(shmid, 0, 0)) == (shmData *)-1){
		perror("shmat p");
		exit(1);
	}

	queue_initialize(&q);								//keeps all vessels and stats about them
	int temp_earn, temp_wt;
	int	small_count, med_count, large_count;
	float total_earn_av, total_wait_av;

	//fix sleep times
	int small, next;
	if(time <= stattime){
		small = time;
		next = stattime - small;
	}
	else{
		small = stattime;
		next = time - small;
	}

	

	while(1){
		//sleep(small) and then print current state of port + calculate stats
		sleep(small);

		sem_wait(&(shm[0].pmMutex));

		printf("\n*-------------------------------------------*\n");
		printf("-Monitor: Current Port State:\n");
		ht_print(&(shm[0].pl));							//print
		printf("*-------------------------------------------*\n");
		ht_getVessels(&(shm[0].pl), &q);

		sem_post(&(shm[0].pmMutex));

		//sleep(next) if not zero and the print statistics
		if(next > 0)
			sleep(next);
		total_earn_av = (float)q.total_earnings / (float) q.count;
		total_wait_av = (float)q.total_wtime / (float) q.count;

		if(q.total_earnings > 0)
			printf("\n- Monitor statistics:\nTotal:\tEarnings: %d, Earning average: %.3f, Vessel Wait time average: %.3f\n", q.total_earnings, total_earn_av, total_wait_av);

		small_count = queue_getTypeCount(&q, 'S');
		if(small_count){
			queue_getSpecData(&q, 'S', &temp_earn, &temp_wt);
			printf("Small vessels:\tEarning average: %.3f, Vessel Wait time average: %.3f\n", temp_earn/(float)small_count, temp_wt/(float)small_count);
		}
		med_count = queue_getTypeCount(&q, 'M');
		if(med_count){
			queue_getSpecData(&q, 'M', &temp_earn, &temp_wt);
			printf("Medium vessels:\tEarning average: %.3f, Vessel Wait time average: %.3f\n", temp_earn/(float)med_count, temp_wt/(float)med_count);
		}
		large_count = queue_getTypeCount(&q, 'L');
		if(large_count){
			queue_getSpecData(&q, 'L', &temp_earn, &temp_wt);
			printf("Large vessels:\tEarning average: %.3f, Vessel Wait time average: %.3f\n", temp_earn/(float)large_count, temp_wt/(float)large_count);
		}
	}


	//detach
	if(shmdt(shm) == -1){
		perror("shmdt");
		exit(1);
	}


	//clear space
	queue_freeQ(&q);
	exit(1);
}

void int_handler(int sig)
{
	queue_freeQ(&q);
	printf("Monitor terminating\n");
	exit(0);
}
