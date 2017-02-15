/*
* first masking all the data entry to achieve k-anonymization
*	repeating this in various conditions to get a IL-sth diagram
* threshould should be based on the trade off
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <math.h>
#include "check.h"

/* Global Variables */
int mask_bit = 0xffffffff;
int flag;
double IF = 0;

/*
 * Prototype Declaration
 *
 * Init()		:Initialize all the data
 * K_count()	:How much K has been done so far
 * Masking()	:Mask the input data
 * info_loss()	:Calculate the information loss
 * */
void Prep(int *d_num, int *mask_k, int *pk_k, int argc, char **argv);
void Init(D *Data);
void K_count(D *Data, int data_num, int mask_k, int *group_num);
void Masking(D *Data, int data_num);
void Pk_noise(D *Data, L* Laplace, int data_num, int group_num, int pk_k);
void Init_Laplace(L *Laplace, int group_num);
void info_loss(D *Data, int data_num);
double Uniform(void);
double Laplace_Rand(double mu, double sigma);

/* The Main Function for K Anonymization */
int main(int argc, char **argv) {
	/* main variables */
	FILE *fp;
	int i, j;
	static int loop;
	int data_num, mask_k, pk_k;
	int group_num;
	D *Data;
	L *Laplace;

	/* Store the arguments */
	Prep(&data_num, &mask_k, &pk_k, argc, argv);

	printf("data_num: %d\r\n", data_num);
	printf("mask_k: %d\r\n", mask_k);
	printf("pk_k: %d\r\n", pk_k);

	Data = malloc(sizeof(D)*data_num);

	/* initialization */
	srand((unsigned int) time (NULL));
	for (i=0; i<data_num; i++) {
		Init(&Data[i]);
	}

	/* main processing */
	/* MASKING */
	do {
		printf("K count START!!\n");
		K_count(Data, mask_k, data_num, &group_num);
		printf("Masking START!!\n");
		Masking(Data, data_num);
		if(flag) printf("No need masking!! DONE!!\n");
		else printf("%d round over!!\n", ++loop);
	} while (!flag);

	/* Pk-Noising */
	Laplace = malloc(sizeof(L)*group_num);
	Init_Laplace(Laplace, group_num);
	Pk_noise(Data, Laplace, data_num, group_num, pk_k);

	for (i=0; i<data_num; i++) {
		printf("%d: IP = %d, TIME = %d, NOISED_T = %d\r\n", i, Data[i].ip, Data[i].tstamp, Data[i].noised_tstamp);
	}

	/* information loss check */
	info_loss(Data, data_num);
	printf("*********************\r\n");
	printf("Information Loss: %f%\n", IF);
	printf("*********************\r\n");
	printf("%d & P%d Anonymization Accomplished\r\n", mask_k, pk_k);
	printf("*********************\r\n");

	if ((fp=fopen("result.csv", "w")) != NULL) {
		for (i=0; i<data_num; i++) {
			fprintf(fp, "%d, %d\n", Data[i].tstamp, Data[i].noised_tstamp);
		}
		fclose(fp);
	}

	free(Data);
	free(Laplace);

	return 0;
}

void Init_Laplace(L *Laplace, int group_num) {
	int i;
	
	for (i=0; i<group_num; i++) {
		Laplace[i].record = 0;
		Laplace[i].max = -100000.0;
		Laplace[i].min = 100000.0;

		Laplace[i].range = 0.0;
		Laplace[i].tstamp_avg = 0.0;
		Laplace[i].sigma = 0.0;
	}
}

void Prep(int *d_num, int *mask_k, int *pk_k, int argc, char **argv) {
	int opt;
	while(1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"the number of input data", 1, NULL, 'n'},
			{"k-anonymity", 1, NULL, 'k'},
			{0,0,0,0}
		};
		opt = getopt_long(argc, argv, "n:k:p:", long_options, &option_index);

		if(opt==-1) {
			break;
		} else if (opt==0) {
			continue;
		} else {
			switch(opt) {
			case 'n':
				*d_num = atoi(optarg);
				break;
			case 'k':
				*mask_k = atoi(optarg);
				break;
			case 'p':
				*pk_k = atoi(optarg);
				break;
			default:
				exit(0);
				break;
			}
		}
	}
	if (optind < argc) {
		printf("non-option ARGV-elements: ");
		while (optind < argc) {
			printf("%s ", argv[optind++]);
		}
		printf("\n");
	}

	return;
}

void Init(D *Data) {
	Data->ip = rand() % 100000;
	Data->tstamp = rand() % 35;
	Data->temp = rand()%10;
	Data->count = 0;
	Data->k = 0;
	Data->k_done = 0;
}

void K_count(D *Data, int mask_k, int data_num, int *group_num) {
	int i, j;
	int count = 0;

	//Init
	for (i=0; i<data_num; i++) {
		Data[i].group_done = 0;
	}

	for (i=0; i<data_num; i++) {
		Data[i].k = 0;	//init
		if (!Data[i].group_done) {
			Data[i].group_id = count;
			Data[i].group_done = 1;
			count++;
		}
		for (j=0; j<data_num; j++) {
			if (Data[i].ip == Data[j].ip) {
				Data[i].k++;
				if (!Data[j].group_done) {
					Data[j].group_id = Data[i].group_id;
					Data[j].group_done = 1;
				}
			}
			if (Data[i].k >= mask_k) Data[i].k_done = 1;
		}
		printf("%d:k anon = %d\n", i, Data[i].k);
	}

	*group_num = count;
}

void Masking(D *Data, int data_num) {
	int i;
	flag = 1;

	//count++;
	for (i=0; i<data_num; i++) {
		if (!Data[i].k_done) {
			flag = 0;
			if (!FAST_MASK) Data[i].count++;
			else Data[i].count += 2;
			Data[i].ip &= (mask_bit << Data[i].count);
		}
	}
}

void info_loss(D *Data, int data_num) {
	int i;

	for(i=0; i<data_num; i++) {
		IF += (double) Data[i].count/32;
	}

	IF = IF/data_num * 100;
}

double Uniform(void) {
	return ((double)rand()+1.0)/((double)RAND_MAX+2.0);
}

double Laplace_Rand(double mu, double sigma) {
	double U = Uniform();

	if (U<0.5) return sigma*log(2*U)+mu;
	else return -(sigma*log(2*(1-U))+mu);
}

void Pk_noise(D *Data, L* Laplace, int data_num, int group_num, int pk_k) {
	int i, j;

	for (i=0; i<data_num; i++) {
		for (j=0; j<group_num; j++) {
			if (Data[i].group_id == j) {
				Laplace[j].record++;
				Laplace[j].tstamp_avg += (float)Data[i].tstamp;
				if (Laplace[j].min > Data[i].tstamp) Laplace[j].min = Data[i].tstamp;
				if (Laplace[j].max < Data[i].tstamp) Laplace[j].max = Data[i].tstamp;
			}
		}
	}

	for (j=0; j<group_num; j++) {
		Laplace[j].tstamp_avg /= Laplace[j].record;
		Laplace[j].range = Laplace[j].max - Laplace[j].min;
		Laplace[j].sigma = 2*Laplace[j].range*1/log((Laplace[j].record-1)/(pk_k-1));
	}

	float *noise;
	noise = malloc(sizeof(float)*data_num);
	for (i=0; i<data_num; i++) {
		for (j=0; j<group_num; j++) {
			if (Data[i].group_id == j) {
				noise[i] = Laplace_Rand(Laplace[j].tstamp_avg, Laplace[j].sigma);
				Data[i].noised_tstamp = Data[i].tstamp + (int)noise[i];
			}
		}
	}

	free(noise);
}

	//for (i=0; i<data_num; i++) {
	//	for (j=0; j<group_num; j++) {
	//		if (Data[i].group_id == j) {
	//			Laplace[j].record++;
	//			Laplace[j].tstamp_avg += Data[i].tstamp;
	//			if (Laplace[j].min > Data[i].tstamp) Laplace[j].min = Data[i].tstamp;
	//			if (Laplace[j].max < Data[i].tstamp) Laplace[j].max = Data[i].tstamp;
	//		}
	//	}
	//}

	//for (j=0; j<group_num; j++) {
	//	Laplace[j].tstamp_avg /= Laplace[j].record;
	//	Laplace[j].range = Laplace[j].max - Laplace[j].min;
	//	Laplace[j].sigma = 2*Laplace[j].range*1/log((Laplace[j].record-1)/(pk_k-1));
	//}

	//float *noise;
	//noise = malloc(sizeof(float)*data_num);
	//for (i=0; i<data_num; i++) {
	//	for (j=0; j<group_num; j++) {
	//		if (Data[i].group_id == j) {
	//			noise[i] = Laplace_Rand(Laplace[j].tstamp_avg, Laplace[j].sigma);
	//			printf("Data[%d]: %d, noise[%d]: %f\r\n", i, Data[i].tstamp, i, noise[i]);
	//			Data[i].tstamp += (int)noise[i];
	//			printf("Data[%d]: %d\r\n", i, Data[i].tstamp);
	//		}
	//	}
