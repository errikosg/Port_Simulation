#ifndef VNOTES_H
#define VNOTES_H

//"note" that vessel leaves when entering port.
typedef struct vesselNote{
	char name[15];
	char type;
	char upgrade;
	int parkperiod;
	int wait_time;
}vesselNote;


typedef struct vnotes{
	vesselNote in[15];
	vesselNote out[15];
	int in_count;
	int out_count;
	int size;
}vnotes;


//functions
void vn_init(vnotes *);
int vn_isInEmpty(vnotes *);
int vn_isOutEmpty(vnotes *);
int vn_pushIn(vnotes *, char *, char, char, int, int);
int vn_pushOut(vnotes *, char *, char, int);
int vn_popIn(vnotes *, char **, char *, char *, int *, int *);
int vn_popOut(vnotes *, char **, char *, int *);
//void vn_freeAll


#endif
