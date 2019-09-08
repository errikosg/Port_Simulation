#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue.h"

void queue_initialize(Queue *Q)
{
	Q->front = NULL;
	Q->rear = NULL;
	Q->count = 0;
	Q->total_earnings = 0;
	Q->total_wtime = 0;
}

int queue_isEmpty(Queue *Q)
{
	return (Q->front==NULL);
}

void queue_insert(Queue *Q, char *name, char type, int earn, int wt)
{
	linkk temp;
	temp = (linkk)malloc(sizeof(struct QueueNode));
	temp->name = strdup(name);
	temp->type = type;
	temp->earnings = earn;
	temp->wait_time = wt;
	temp->next = NULL;

	if(Q->rear==NULL){
		Q->front = temp;
		Q->rear = temp;
	}
	else{
		Q->rear->next=temp;
		Q->rear = temp;
	}
	Q->count++;
	Q->total_earnings += earn;
	Q->total_wtime += wt;
}

void queue_remove(Queue *Q, char **name, char *type, int *earn, int *wt)
{
	linkk temp;
	if(Q->front==NULL){
		//printf("--Attempting to remove from empty queue\n");
		return;
	}
	else{
		*(name) = strdup(Q->front->name);
		*(type) = Q->front->type;
		*(earn) = Q->front->earnings;
		*(wt) = Q->front->wait_time;

		temp = Q->front;
		Q->front = temp->next;
		free(temp->name);
		free(temp);
		if(Q->front==NULL){
			Q->rear = NULL;
		}
	}
	Q->count--;
	Q->total_earnings -= *(earn);
	Q->total_wtime -= *(wt);
}

void queue_print(Queue *Q)
{
	if(queue_isEmpty(Q)){
		//printf("print: Queue is empty!\n");
		return;
	}
	else{
		linkk curr = Q->front;
		printf("\nQueue:\n ");
		while(curr != NULL){
			printf("%s, %d, %d\n", curr->name, curr->earnings, curr->wait_time);
			curr = curr->next;
		}
	}
}

int queue_search(Queue *Q, char *name)
{
	//return 1 if item found, 0 if not.
	if(queue_isEmpty(Q)){
		//printf("search: Queue is empty!\n");
		return 0;
	}
	else{
		linkk curr = Q->front;
		while(curr != NULL){
			if(strcmp(curr->name, name)==0){
				return 1;
			}
			curr = curr->next;
		}
	}
	return 0;
}

int queue_getSpecData(Queue *Q, char type, int *earn, int *wt)
{
	if(type != 'S' && type != 'M' && type != 'L')
		return 0;

	linkk curr = Q->front;
	int found = 0, total_e=0, total_w=0;
	while(curr != NULL){
		if(curr->type == type){
			total_e += curr->earnings;
			total_w += curr->wait_time;
			found = 1;
		}
		curr = curr->next;
	}
	if(found){
		*(earn) = total_e;
		*(wt) = total_w;
	}
	return found;
}

int queue_getTypeCount(Queue *Q, char type)
{
	if(type != 'S' && type != 'M' && type != 'L')
		return 0;

	linkk curr = Q->front;
	int count=0;
	while(curr != NULL){
		if(curr->type == type){
			count++;
		}
		curr = curr->next;
	}
	return count;
}

void queue_freeQ(Queue *Q)
{
	if(queue_isEmpty(Q)){
		//printf("search: Queue is empty!\n");
		return;
	}
	
	linkk curr = Q->front;
	linkk prev = curr;
	while(curr != NULL){
		curr = curr->next;
		free(prev->name);
		free(prev);
		prev = curr;
	}
}

