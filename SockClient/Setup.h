#ifndef SETUP_H
#define SETUP_H

#include <asm/types.h>

struct input_data {
	int put[2];
	int LIA;
	int LIB;
	int RIA;
	int RIB;
};

struct pwm_data {
	__u64 la;
	__u64 lb;
	__u64 ra;
	__u64 rb;
	int fd1[2];
	int fd2[2];
	double lspeed;
	double rspeed;
};

void setupPWM(unsigned long long period);
void setupSPD();
int startValue(char const *path);
void updatePWM(int la, int lb, int ra, int rb, int fd1[], int fd2[]);
void endPWM(int fd1[], int fd2[]);

#endif