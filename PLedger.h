#ifndef PLEDGER_H
#define PLEDGER_H

#define ARRIVED 1
#define DEPARTED -1

#include "queue.h"

//public ledger implementation with static hashtable.
typedef struct{
	char name[15];
	char type;
	int	port_cost;
	double arrival_time;
	int wait_time;
	int status;
}ht_node;

typedef struct{
	ht_node items[15];
	int size;
	int count;
}pub_ledger;


//functions
void ht_initialize(pub_ledger *);						//initialize hash table.
int ht_isEmpty(pub_ledger *);
int ht_isFull(pub_ledger *);
int ht_insert(pub_ledger *, char *, char, int, double, int);	
int ht_delete(pub_ledger *, char *, char *);	
int ht_search(pub_ledger *, char *);
int ht_findWhole(pub_ledger *, char *, ht_node *);
int ht_findCharge(pub_ledger *, char *);
void ht_getVessels(pub_ledger *, Queue *);
void ht_print(pub_ledger *);

static int getHash(char *, int, int);
unsigned long hashfunction1(char *);
unsigned long hashfunction2(char *);


#endif
