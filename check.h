/*
*	check.h
*
*	1. you do not particularly need to include any header files
*	2. you should define variables and functions that are shared across multiple c files
*	2-1. Shared variables need a prefix "extern"
*	2-2. Shared functions must be declared
*
*/
//#define DATA_NUM 1024
//#define K_NUM 4
#define FAST_MASK 0

typedef struct data {
	unsigned int ip;		//mask: non-sensitive
	unsigned int tstamp;	//micro: non-sensitive
	unsigned int noised_tstamp;
	int temp;				//sensitive
	int count;				//infomation loss count
	int k;					//k anonymization & the number of data for k-means clustering
	int k_done;				//k anon done? 0:Not 1:Yes
	int group_id;			//the group ID
	int group_done;			//group ID dist done? 0:Not 1:Yes
	int class_id;			//the class ID in a group
} D;

typedef struct LAPLACE {
	unsigned int record;	//means just a total number of the group
	float max;
	float min;

	float range;
	float tstamp_avg;				//aka mu
	float sigma;
} L;


typedef struct CLUSTER {
	unsigned int tstamp_c;
	int num;
} C;

typedef struct PRE {
	unsigned int class_k;
} P;
/* Global Variables */
//extern D *Data;
//extern int count;

/* Prototype Declaration */
