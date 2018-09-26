#ifndef SETUP_H
#define SETUP_H

#include <asm/types.h>

struct pwm_data_t {
	__u64 la;
	__u64 lb;
	__u64 ra;
	__u64 rb;
	int LIA;
	int LIB;
	int RIA;
	int RIB;
	int fd1[2];
	int fd2[2];
	double lspeed;
	double rspeed;
	double cmd_l;
	double cmd_r;
};

void setupPWM(unsigned long long period);
int startValue(char const *path);
void updatePWM(int la, int lb, int ra, int rb, int fd1[], int fd2[]);
void endPWM(int fd1[], int fd2[]);

#endif