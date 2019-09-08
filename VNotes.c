#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VNotes.h"

void vn_init(vnotes *vn)
{
	vn->in_count = 0;
	vn->out_count = 0;
	vn->size = 15;
}

int vn_isInEmpty(vnotes *vn)
{	
	return (vn->in_count==0);
}

int vn_isOutEmpty(vnotes *vn)
{	
	return (vn->out_count==0);
}

int vn_pushIn(vnotes *vn, char *name, char type, char up, int parkp, int wt)
{
	//push in
	if(vn->in_count == vn->size)
		return 0;

	strcpy(vn->in[vn->in_count].name, name);
	vn->in[vn->in_count].type = type;
	vn->in[vn->in_count].upgrade = up;
	vn->in[vn->in_count].parkperiod = parkp;
	vn->in[vn->in_count].wait_time = wt;
	vn->in_count++;
	return 1;
}

int vn_pushOut(vnotes *vn, char *name, char type, int parkp)
{
	//push out
	if(vn->out_count == vn->size)
		return 0;

	strcpy(vn->out[vn->out_count].name, name);
	vn->out[vn->out_count].type = type;
	vn->out[vn->out_count].parkperiod = parkp;
	vn->out_count++;
	return 1;
}

int vn_popIn(vnotes *vn, char **name, char *type, char *up, int *parkp, int *wt)
{
	if(vn_isInEmpty(vn))
		return 0;

	*(name) = strdup(vn->in[vn->in_count-1].name);
	*(type) = vn->in[vn->in_count-1].type;
	*(up) = vn->in[vn->in_count-1].upgrade;
	*(parkp) = vn->in[vn->in_count-1].parkperiod;
	*(wt) = vn->in[vn->in_count-1].wait_time;
	vn->in_count--;
	return 1;
}

int vn_popOut(vnotes *vn, char **name, char *type, int *parkp)
{
	if(vn_isOutEmpty(vn))
		return 0;

	*(name) = strdup(vn->out[vn->out_count-1].name);
	*(type) = vn->out[vn->out_count-1].type;
	*(parkp) = vn->out[vn->out_count-1].parkperiod;
	vn->out_count--;
	return 1;
}
