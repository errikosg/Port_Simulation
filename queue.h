#ifndef QUEUE_H
#define QUEUE_H

//this is Queue implemented with linked list --> use for monitoring...
//(header of queue.c)

typedef struct QueueNode* linkk;

struct QueueNode{
	char *name;
	char type;
	int earnings;
	int wait_time;
	linkk next;
};

typedef struct{
	linkk front;
	linkk rear;
	int count;
	int total_earnings;
	int total_wtime;
}Queue;

//functions
void queue_initialize(Queue *);
int queue_isEmpty(Queue *);
void queue_insert(Queue *, char *, char, int, int);
void queue_remove(Queue *, char **, char *, int *, int *);			//here we extract the item.
void queue_print(Queue *);
int queue_search(Queue *, char *);
int queue_getSpecData(Queue *, char, int *, int *);				//retruns earnings and wait time for a specific group (small, medium...)
int queue_getTypeCount(Queue *, char);
void queue_freeQ(Queue *);

#endif
