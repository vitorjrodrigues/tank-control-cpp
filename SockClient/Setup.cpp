#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "Setup.h"
#include "sysfsHelper.h"

void setupPWM(unsigned long long perd) {
	printf("**Configuring PWM Handler**\n");
	//%%%%%%%%%%%%%%%%Defining GPIO Pins as PWM Pins%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	printf("--Defining GPIO Pins as PWM Pins... ");
	setValue("/sys/devices/platform/ocp/ocp:P9_14_pinmux/state", "pwm");
	setValue("/sys/devices/platform/ocp/ocp:P9_16_pinmux/state", "pwm");
	setValue("/sys/devices/platform/ocp/ocp:P8_19_pinmux/state", "pwm");
	setValue("/sys/devices/platform/ocp/ocp:P8_13_pinmux/state", "pwm");
	printf("DONE\n");
	//%%%%%%%%%Exporting the link to PWM Pins%%%%%%%%%%%%%%%%%
	printf("--Exporting the link to PWM Pins... ");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/export", "0");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/export", "1");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/export", "0");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/export", "1");
	printf("DONE\n");
	//%%%%%%%%%Enabling PWM Pins%%%%%%%%%%%%%%%%%
	printf("--Enabling PWM Pins... ");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm0/enable", "1");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm1/enable", "1");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm0/enable", "1");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm1/enable", "1");
	printf("DONE\n");
	//%%%%%%%%%%%%Defining PWM Period (Must have the same value)%%%%%%%%%%%%%%%%%%%
	printf("--Defining PWM Period... ");
	char s[100];
	sprintf(s,"%llu",perd);
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm0/period", s);
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm1/period", s);
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm0/period", s);
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm1/period", s);
	printf("DONE\n");
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
}

int startValue(char const *path) {
	int fd = -1;
	fd = open(path, O_RDWR);
	int sent = write(fd, "0", sizeof("0"));
	return fd;
}

void updatePWM(int la, int lb, int ra, int rb, int fd1[], int fd2[]) {
	int sent;
	char buffer[100];
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	sprintf(buffer,"%d",la);
	sent = write(fd1[0], buffer, sizeof(buffer));
	sprintf(buffer,"%d",lb);
	sent = write(fd1[1], buffer, sizeof(buffer));
	sprintf(buffer,"%d",ra);
	sent = write(fd2[0], buffer, sizeof(buffer));
	sprintf(buffer,"%d",rb);
	sent = write(fd2[1], buffer, sizeof(buffer));
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
}

void endPWM(int fd1[], int fd2[]) {
	write(fd1[0], "0", sizeof("0"));
	close(fd1[0]);
	write(fd1[1], "0", sizeof("0"));
	close(fd1[1]);
	write(fd2[0], "0", sizeof("0"));
	close(fd2[0]);
	write(fd2[1], "0", sizeof("0"));
	close(fd2[1]);
}


