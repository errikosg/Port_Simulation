#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "functions.h"


int readConFile(char *name, slotData **data)
{
	FILE *fp = NULL;
	int i, index;
	char *buffer, *token;

	//open and manage file
	if((fp = fopen(name, "r")) == NULL)
		return 0;

	buffer = malloc(200*sizeof(char));
	(*data) = malloc(3*sizeof(slotData));


	//types
	fscanf(fp, "%s\n", buffer);
	index=0;
	token = strtok(buffer, ",");
	while(token != NULL){
		(*data)[index].type = *token;
		index++;
		token = strtok(NULL, ",");
	}
	
	//capacities
	fscanf(fp, "%s\n", buffer);
	index=0;
	token = strtok(buffer, ",");
	while(token != NULL){
		(*data)[index].capacity = atoi(token);
		index++;
		token = strtok(NULL, ",");
	}

	//costs
	fscanf(fp, "%s\n\n", buffer);
	index=0;
	token = strtok(buffer, ",");
	while(token != NULL){
		(*data)[index].cost = atoi(token);
		index++;
		token = strtok(NULL, ",");
	}


	fclose(fp);
	free(buffer);
	return 1;
}

int makeChargesFile(slotData *sd)
{
	FILE *fp;

	if((fp = fopen("Charges.txt", "w")) == NULL)
		return 0;

	fprintf(fp, "%c,%c,%c\n", sd[0].type, sd[1].type, sd[2].type);
	fprintf(fp, "%d,%d,%d\n", sd[0].capacity, sd[1].capacity, sd[2].capacity);
	fprintf(fp, "%d,%d,%d\n", sd[0].cost, sd[1].cost, sd[2].cost);
	fclose(fp);
	return 1;
}

int readChargesFile(char *name, slotData **data)
{
	FILE *fp = NULL;
	int i, index;
	char *buffer, *token;

	//open and manage file
	if((fp = fopen(name, "r")) == NULL)
		return 0;

	buffer = malloc(200*sizeof(char));
	(*data) = malloc(3*sizeof(slotData));


	//types
	fscanf(fp, "%s\n", buffer);
	index=0;
	token = strtok(buffer, ",");
	while(token != NULL){
		(*data)[index].type = *token;
		index++;
		token = strtok(NULL, ",");
	}
	
	//capacities
	fscanf(fp, "%s\n", buffer);
	index=0;
	token = strtok(buffer, ",");
	while(token != NULL){
		(*data)[index].capacity = atoi(token);
		index++;
		token = strtok(NULL, ",");
	}

	//costs
	fscanf(fp, "%s\n", buffer);
	index=0;
	token = strtok(buffer, ",");
	while(token != NULL){
		(*data)[index].cost = atoi(token);
		index++;
		token = strtok(NULL, ",");
	}

	fclose(fp);
	free(buffer);
	return 1;
}

int makePortMaster(char *chfile, int shmid)
{
	int pid = fork();
	if(pid < 0){
		perror("fork");
		return -1;
	}
	else if(pid == 0){
		char *cmd = "./port-master";
		char *args[6];
		args[0] = "./port-master";
		args[1] = malloc(3*sizeof(char));
		sprintf(args[1], "-c");
		args[2] = strdup(chfile);
		args[3] = malloc(3*sizeof(char));
		sprintf(args[3], "-s");
		args[4] = malloc(15*sizeof(char));
		sprintf(args[4], "%d", shmid);
		args[5] = NULL;

		execvp(cmd, args);
		printf("Error in making port master: execvp\n");
		exit(-1);
	}
	return pid;
}

int makeMonitor(int shmid)
{
	int time, stattime;

	//HERE	
	time = 3;
	stattime = 6;

	int pid = fork();
	if(pid < 0){
		perror("fork");
		return -1;
	}
	else if(pid == 0){
		char *cmd = "./monitor";
		char *args[8];
		args[0] = "./monitor";
		args[1] = malloc(3*sizeof(char));
		sprintf(args[1], "-d");
		args[2] = malloc(15*sizeof(char));
		sprintf(args[2], "%d", time);
		args[3] = malloc(3*sizeof(char));
		sprintf(args[3], "-t");
		args[4] = malloc(15*sizeof(char));
		sprintf(args[4], "%d", stattime);
		args[5] = malloc(3*sizeof(char));
		sprintf(args[5], "-s");
		args[6] = malloc(15*sizeof(char));
		sprintf(args[6], "%d", shmid);
		args[7] = NULL;

		execvp(cmd, args);
		printf("Error in making port master: execvp\n");
		exit(-1);
	}
	return pid;
}

int makeVessel(int shmid)
{
	char type, postype;
	int parkperiod, mantime, intype1, intype2, ran;

	//make random values - 1=S, 2=M, 3=L
	intype1 = rand() % 3 + 1;
	switch(intype1){
		case 1:
			type = 'S'; break;
		case 2:
			type = 'M'; break;
		case 3:
			type = 'L'; break;
	}
	if(intype1 < 3){
		ran = rand() % 2;
		intype2 = intype1+ran;
	}
	else
		intype2 = intype1;
	switch(intype2){
		case 1:
			postype = 'S'; break;
		case 2:
			postype = 'M'; break;
		case 3:
			postype = 'L'; break;
	}
	
	parkperiod = rand() % 3 + 3;
	mantime = rand() % 3 + 1;

	int pid = fork();
	if(pid < 0){
		perror("fork");
		return -1;
	}
	else if(pid == 0){
		char *cmd = "./vessel";
		char *args[12];
		args[0] = "./vessel";
		args[1] = malloc(3*sizeof(char));
		sprintf(args[1], "-t");
		args[2] = malloc(2*sizeof(char));
		sprintf(args[2], "%c", type);
		args[3] = malloc(3*sizeof(char));
		sprintf(args[3], "-u");
		args[4] = malloc(2*sizeof(char));
		sprintf(args[4], "%c", postype);
		args[5] = malloc(3*sizeof(char));
		sprintf(args[5], "-p");
		args[6] = malloc(4*sizeof(char));
		sprintf(args[6], "%d", parkperiod);
		args[7] = malloc(3*sizeof(char));
		sprintf(args[7], "-m");
		args[8] = malloc(4*sizeof(char));
		sprintf(args[8], "%d", mantime);
		args[9] = malloc(3*sizeof(char));
		sprintf(args[9], "-s");
		args[10] = malloc(15*sizeof(char));
		sprintf(args[10], "%d", shmid);
		args[11] = NULL;

		execvp(cmd, args);
		printf("Error in making vessel: execvp\n");
		exit(-1);
	}
	return pid;
}

int validPost(char t, char p)
{
	switch(t){
		case 'M':
			if(p == 'S')
				return 0;
			break;
		case 'L':
			if(p == 'S' || p == 'M')
				return 0;
			break;
	}
	return 1;
}

void getData(double *a_time, clock_t start, int *p_cost, int parkp, int slot_cost)
{
	//being in the port costs 'slot_cost' every 3 secs

	double halfs = parkp / (double) 3;
	double cost = halfs * (double) slot_cost;
	*(p_cost) = roundFloat(cost);

	clock_t now = clock();
	*(a_time) = (double)(now - start) / CLOCKS_PER_SEC;
}


int roundFloat(double num)
{
	int ret;
	double temp;

	ret = (int)num;
	temp = (double)ret + 0.5;
	if(temp < num)
		ret += 1;

	return ret;
}
