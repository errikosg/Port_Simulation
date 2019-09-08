#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PLedger.h"


void ht_initialize(pub_ledger *ht)
{
	(*ht).count = 0;
	(*ht).size = 15;

	int i;
	for(i=0; i<15; i++){
		(*ht).items[i].status = DEPARTED;
	}
}


int ht_isEmpty(pub_ledger *ht)
{
	return (ht->count==0);
}

int ht_isFull(pub_ledger *ht)
{
	return (ht->count==ht->size);
}

unsigned long hashfunction1(char* str){			//algorithm djb2 from:	www.cse.yorku.ca/~oz/hash.html
	unsigned long hash = 5381;
	int c;
	while(c = *str++)
		hash = ((hash<<5)+hash)+c;	
	return hash;
}

unsigned long hashfunction2(char *str)
{
	unsigned long hash = 0;
	int c;
	while (c = *str++)
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

static int getHash(char *str, int bucket_num, int att)
{
	//unsigned long v1 = hashfunction1(str)%bucket_num;
	//unsigned long v2 = hashfunction2(str);
	int v1 = (int)hashfunction1(str);
	int v2 = (int)hashfunction2(str);
	return (v1 + (att * (v2+1))) % bucket_num;
}


int ht_insert(pub_ledger *ht, char *name, char type, int p_cost, double a_time, int wt)
{
	if(ht->count == ht->size)
		return 0;

	int i=0, index;
	index = getHash(name, ht->size, i);
	index = abs(index);
	while(ht->items[index].status == ARRIVED){
		index = (index+1) % 15;
	}

	strcpy(ht->items[index].name, name);
	ht->items[index].type = type;
	ht->items[index].port_cost = p_cost;
	ht->items[index].arrival_time = a_time;
	ht->items[index].wait_time = wt;
	ht->items[index].status = ARRIVED;

	ht->count++;
	return 1;
}

int ht_delete(pub_ledger *ht, char *name, char *history_log)
{
	if(ht->count == 0)
		return 0;
	int index, i=0, start;
	index = getHash(name, ht->size, i);
	index = abs(index);
	start = index;
	while(ht->items[index].status == DEPARTED || (ht->items[index].status == ARRIVED && strcmp(ht->items[index].name, name) != 0)){
		index = (index+1) % 15;
		if(index == start)
			return 0;
	}
	ht->items[index].status = DEPARTED;
	ht->count--;

	//append to file this move as it is now history to the port
	FILE *fp;
	if((fp = fopen(history_log, "a")) == NULL)
		return 0;

    //! here. Index instead of i
	fprintf(fp, "Slot %d:\n-> Name = %s, Type = %c, Cost = %d, Arrival time = %f, Status = %d, Wait time = %d\n\n", index, ht->items[index].name, ht->items[index].type, ht->items[index].port_cost, ht->items[index].arrival_time, ht->items[index].status, ht->items[index].wait_time);

	fclose(fp);
	return 1;
}

int ht_search(pub_ledger *ht, char *name)
{
	int index, i=0, start;
	index = getHash(name, ht->size, i);
	index = abs(index);
	start = index;
	while(ht->items[index].status == DEPARTED || (ht->items[index].status == ARRIVED && strcmp(ht->items[index].name, name) != 0)){
		index = (index+1) % 15;
		if(index == start)
			return 0;
	}
	return 1;
}

int ht_findWhole(pub_ledger *ht, char *name, ht_node *node)
{
	int index, i=0, start;
	index = getHash(name, ht->size, i);
	index = abs(index);
	start = index;
	while(ht->items[index].status == DEPARTED || (ht->items[index].status == ARRIVED && strcmp(ht->items[index].name, name) != 0)){
		index = (index+1) % 15;
		if(index == start)
			return 0;
	}
	(*node) = ht->items[index];
	return 1;
}

int ht_findCharge(pub_ledger *ht, char *name)
{
	int index, i=0, start;
	index = getHash(name, ht->size, i);
	index = abs(index);
	start = index;
	while(ht->items[index].status == DEPARTED || (ht->items[index].status == ARRIVED && strcmp(ht->items[index].name, name) != 0)){
		index = (index+1) % 15;
		if(index == start)
			return 0;
	}
	return ht->items[index].port_cost;
}

void ht_getVessels(pub_ledger *ht, Queue *q)
{
	int i;
	for(i=0; i<ht->size; i++){
		if(ht->items[i].status == DEPARTED)
			continue;
		if(!queue_search(q, ht->items[i].name))
			queue_insert(q, ht->items[i].name, ht->items[i].type, ht->items[i].port_cost, ht->items[i].wait_time);
	}
}

void ht_print(pub_ledger *ht)
{
	int i;
	for(i=0; i<ht->size; i++){
		if(ht->items[i].status == DEPARTED)
			continue;
		printf("Slot %d:\n-> Name = %s, Type = %c, Cost = %d, Arrival time = %f, Status = %d, Wait time = %d\n\n", i, ht->items[i].name, ht->items[i].type, ht->items[i].port_cost, ht->items[i].arrival_time, ht->items[i].status, ht->items[i].wait_time);
	}
}
