#ifndef PORT_VISUAL_H
#define PORT_VISUAL_H


//struct that depicts each slot type - type=Small/Medium/Large, ids=int pointer of vessel ids, size=size of array when initialized, count=current count.
typedef struct vslot{
	int slot_id;
	char *vessel_name;
}vslot;

typedef struct vslot_group{
	char type;
	vslot *slots;
	int size;
	int count;
}vslot_group;

typedef struct vslot_array{
	vslot_group vsg[3];
	int total_size;
	int total_count;
}vslot_array;


//functions
//vslot_group
void vsgroup_init(vslot_group *, char, int, int);
int vsgroup_insert(vslot_group *, char *);
int vsgroup_delete(vslot_group *, char *);
int vsgroup_findVessel(vslot_group *, char *);
int vsgroup_checkSpace(vslot_group *);
void vsgroup_freeAll(vslot_group *);
void vsgroup_print(vslot_group *);


//array
void vsarr_init(vslot_array *, int, int, int);
int vsarr_insert(vslot_array *, char, char *);
int vsarr_delete(vslot_array *, char, char *);
int vsarr_delete_v2(vslot_array *, char *);
int vsarr_findVessel(vslot_array *, char, char *);
int vsarr_smallSpace(vslot_array *);
int vsarr_mediumSpace(vslot_array *);
int vsarr_largeSpace(vslot_array *);
void vsarr_freeAll(vslot_array *);
void vsarr_print(vslot_array *);


/*typedef struct vslot{
	int slot_id;
	char name[15];
	char type;
	int parkperiod;
}vslot;


typedef struct vslot_array{
	vslot slots[15];
	int count;
}vslot_array;*/


#endif
