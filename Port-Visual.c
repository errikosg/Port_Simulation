#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Port-Visual.h"

//group
void vsgroup_init(vslot_group *vsg, char type, int size, int start)
{
	vsg->type = type;
	vsg->slots = malloc(size*sizeof(vslot));
	vsg->size = size;
	vsg->count = 0;

	int i;
	for(i=0; i<size; i++){
		vsg->slots[i].slot_id = i;
		vsg->slots[i].vessel_name = NULL;
	}
}

int vsgroup_insert(vslot_group *vsg, char *vessel_name)
{
	if(vsg->count == vsg->size)
		return -1;

	int id, i;
	for(i=0; i<vsg->size; i++){
		if(vsg->slots[i].vessel_name == NULL){
			vsg->slots[i].vessel_name = strdup(vessel_name);
			id = vsg->slots[i].slot_id;
			break;
		}
	}
	vsg->count++;
	return id;
}

int vsgroup_delete(vslot_group *vsg, char *vessel_name)
{
	if(vsg->count == 0)
		return 0;
	
	int i, found=0;
	for(i=0; i<vsg->size; i++){
		if(vsg->slots[i].vessel_name != NULL && strcmp(vsg->slots[i].vessel_name, vessel_name)==0){
			free(vsg->slots[i].vessel_name);
			vsg->slots[i].vessel_name = NULL;
			vsg->count--;
			found = 1;
		}
	}
	return found;
}

int vsgroup_findVessel(vslot_group *vsg, char *vessel_name)
{
	if(vsg->count == 0)
		return -1;

	int i;
	for(i=0; i<vsg->size; i++){
		if(vsg->slots[i].vessel_name != NULL && strcmp(vsg->slots[i].vessel_name, vessel_name)==0){
			return vsg->slots[i].slot_id;
		}
	}
	return -1;
}

int vsgroup_checkSpace(vslot_group *vsg)
{
	if(vsg->count == vsg->size)
		return 0;
	else
		return 1;
}

void vsgroup_print(vslot_group *vsg)
{
	if(vsg->count == 0){
		//printf("Empty!\n");
		return;
	}
	int i;
	for(i=0; i<vsg->size; i++){
		if(vsg->slots[i].vessel_name != NULL)
			printf("Slot %d --> Vessel: %s\n", vsg->slots[i].slot_id, vsg->slots[i].vessel_name);
	}
}

void vsgroup_freeAll(vslot_group *vsg)
{
	int i;
	for(i=0; i<vsg->size; i++){
		if(vsg->slots[i].vessel_name != NULL)
			free(vsg->slots[i].vessel_name);
	}
	free(vsg->slots);
}

//array
void vsarr_init(vslot_array *va, int size1, int size2, int size3)
{
	va->total_size = size1+size2+size3;
	va->total_count = 0;
	vsgroup_init(&va->vsg[0], 'S', size1, 0);
	vsgroup_init(&va->vsg[1], 'M', size2, size1);
	vsgroup_init(&va->vsg[2], 'L', size3, size2);
}

int vsarr_insert(vslot_array *va, char type, char *vessel_name)
{
	if(va->total_count == va->total_size)
		return -1;

	//returns the slot that the vessel entered
	int ret;
	if(type == 'S')
		ret = vsgroup_insert(&va->vsg[0], vessel_name);
	else if(type == 'M')
		ret = vsgroup_insert(&va->vsg[1], vessel_name);
	else if(type == 'L')
		ret = vsgroup_insert(&va->vsg[2], vessel_name);
	else
		return 0;

	va->total_count++;
	return ret;
}

int vsarr_delete(vslot_array *va, char type, char *vessel_name)
{
	if(va->total_count == 0)
		return 0;

	int ret;
	if(type == 'S')
		ret = vsgroup_delete(&va->vsg[0], vessel_name);
	else if(type == 'M')
		ret = vsgroup_delete(&va->vsg[1], vessel_name);
	else if(type == 'L')
		ret = vsgroup_delete(&va->vsg[2], vessel_name);
	else
		return 0;

	va->total_count--;
	return ret;
}

int vsarr_delete_v2(vslot_array *va, char *vessel_name)
{
	if(va->total_count == 0)
		return 0;

	int ret1, ret2, ret3;
	ret1 = vsgroup_delete(&va->vsg[0], vessel_name);
	ret2 = vsgroup_delete(&va->vsg[1], vessel_name);
	ret3 = vsgroup_delete(&va->vsg[2], vessel_name);

	if(!ret1 && !ret2 && !ret3)
		return 0;

	va->total_count--;
	return 1;
}

int vsarr_findVessel(vslot_array *va, char type, char *vessel_name)
{
	if(va->total_count == 0)
		return 0;

	if(type == 'S')
		return vsgroup_findVessel(&va->vsg[0], vessel_name);
	else if(type == 'M')
		return vsgroup_findVessel(&va->vsg[1], vessel_name);
	else if(type == 'L')
		return vsgroup_findVessel(&va->vsg[2], vessel_name);
	else
		return 0;
}

int vsarr_smallSpace(vslot_array *va)
{
	return vsgroup_checkSpace(&va->vsg[0]);
}

int vsarr_mediumSpace(vslot_array *va)
{
	return vsgroup_checkSpace(&va->vsg[1]);
}

int vsarr_largeSpace(vslot_array *va)
{
	return vsgroup_checkSpace(&va->vsg[2]);
}


void vsarr_print(vslot_array *va)
{
	if(va->total_count == 0){
		printf("vslot_array empty\n");
		return;
	}

	printf("1) Small slots:\n");
	vsgroup_print(&va->vsg[0]);

	printf("\n2) Medium slots:\n");
	vsgroup_print(&va->vsg[1]);

	printf("\n3) Large slots:\n");
	vsgroup_print(&va->vsg[2]);
	printf("\n\n");
}

void vsarr_freeAll(vslot_array *va)
{
	int i;
	for(i=0; i<3; i++){
		vsgroup_freeAll(&va->vsg[i]);
	}
}


