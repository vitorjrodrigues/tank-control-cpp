#define PWM1A "/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm0/duty_cycle"

#define PWM1B "/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm1/duty_cycle"

#define PWM2A "/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm0/duty_cycle"

#define PWM2B "/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm0/duty_cycle"

struct pwm_data {
	__u64 la;
	__u64 lb;
	__u64 ra;
	__u64 rb;
	int fd1a;
	int fd1b;
	int fd2a;
	int fd2b;
	__u64 perla;
	__u64 perlb;
	__u64 perra;
	__u64 perrb;
};


void setValue(const char *path, const void *data) {
	char bf1[4], bf2[4];
	int fildes = open(path, O_RDWR);
	if (fildes == -1) {
    	printf("Failed opening.\n");
    	exit(0);
	}
	read(fildes, bf1, sizeof(bf1));
	write(fildes,  data , sizeof(data));
	read(fildes, bf2, sizeof(bf2));
	if (bf1 != (bf2+4)) {
	    printf("Failed writing.\n");
	    exit(0);
	}
	close(fildes);
}


void statePWM() {
	printf("--Defining GPIO Pins as PWM Pins... ");
	setValue("/sys/devices/platform/ocp/ocp:P9_14_pinmux/state", "pwm");
	setValue("/sys/devices/platform/ocp/ocp:P9_16_pinmux/state", "pwm");
	setValue("/sys/devices/platform/ocp/ocp:P8_19_pinmux/state", "pwm");
	setValue("/sys/devices/platform/ocp/ocp:P8_13_pinmux/state", "pwm");
	printf("DONE\n");
}
void exportPWM() {
	printf("--Exporting the link to PWM Pins... ");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/export", "0");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/export", "1");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/export", "0");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/export", "1");
	printf("DONE\n");
}

void enablePWM() {
	printf("--Enabling PWM Pins... ");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm0/enable", "1");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm1/enable", "1");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm0/enable", "1");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm1/enable", "1");
	printf("DONE\n");
}

void periodPWM() {
	char s[100] = "0";
	sprintf(s,"%l",Period);
	printf("--Defining PWM Period... ");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm0/period", s);
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm1/period", s);
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm0/period", s);
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm1/period", s);
	printf("DONE\n");
}

void setupPWM() {
	printf("**Configuring PWM Handler**\n");
	//%%%%%%%%%%%%%%%%Defining GPIO Pins as PWM Pins%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	statePWM();
	//%%%%%%%%%Exporting the link to PWM Pins%%%%%%%%%%%%%%%%%
	exportPWM();
	//%%%%%%%%%Enabling PWM Pins%%%%%%%%%%%%%%%%%
	enablePWM();
	//%%%%%%%%%%%%Defining PWM Period (Must have the same value)%%%%%%%%%%%%%%%%%%%
	periodPWM();
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
}

int startPWM(char *path) {
	int fd = -1;
	
	fd = open(path, O_RDWR);
	if (fd == -1) {
		printf("Failed!\n");
		exit(0);
	}
	int sent = write(fd, "0", sizeof("0"));
	return fd;
}

void updatePWM(int la, int lb, int ra, int rb, int fd1a, int fd1b, int fd2a, int fd2b) {
	int sent = -1;
	char buffer[100] = "0";
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	sprintf(buffer,"%d",la);
	sent = write(fd1a, buffer, sizeof(buffer));
	sprintf(buffer,"%d",lb);
	sent = write(fd1a, buffer, sizeof(buffer));
	sprintf(buffer,"%d",ra);
	sent = write(fd1a, buffer, sizeof(buffer));
	sprintf(buffer,"%d",rb);
	sent = write(fd1a, buffer, sizeof(buffer));
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
}

void endPWM(int fd1a, int fd1b, int fd2a, int fd2b) {
	write(fd1a, "0", sizeof("0"));
	close(fd1a);
	write(fd1b, "0", sizeof("0"));
	close(fd1b);
	write(fd2a, "0", sizeof("0"));
	close(fd2a);
	write(fd2b, "0", sizeof("0"));
	close(fd2b);
}