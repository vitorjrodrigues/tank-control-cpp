void setValue(const char *path, const void *data) {
	char buf1[4], buf2[4];
  int fildes = open(path, O_RDWR);
  if (fildes == -1) {
    printf("Failed opening.\n");
    exit(0);
  }
	read(fildes, buf1, sizeof(buf1));
	write(fildes,  data , sizeof(data));
	read(fildes, buf2, sizeof(buf2));
	if (buf1 != (buf2+4)) {
	    printf("Failed writing.\n");
	    exit(0);
}
	close(fildes);
}

void exportPWM() {
	int fd;
	char s[140];
	fd = open("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm0/enable", O_RDONLY);
	if(fd==-1){ setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/export", "0"); }
	fd = open("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm1/enable", O_RDONLY);
	if(fd==-1){ setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/export", "1"); }
	fd = open("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm0/enable", O_RDONLY);
	if(fd==-1){ setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/export", "0"); }
	fd = open("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm1/enable", O_RDONLY);
	if(fd==-1){ setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/export", "1"); }
}

void enablePWM() {
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm0/enable", "1");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm1/enable", "1");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm0/enable", "1");
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm1/enable", "1");
}

int startPWM(char *path) {
	char *zero="0";
	int fd = -1;
	
	fd = open(path, O_RDWR);
	if (fd == -1) {
		printf("Failed!\n");
		exit(0);
	}
	int sent = write(fd, zero, sizeof(zero));
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
	int suc = -1;
	char *zero="0";
	int sent = write(fd1a, zero, sizeof(zero));
	while(suc==-1) { suc = close(fd1a); }
	sent = write(fd1b, zero, sizeof(zero));
	while(suc==-1) { suc = close(fd1b); }
	sent = write(fd2a, zero, sizeof(zero));
	while(suc==-1) { suc = close(fd2a); }
	sent = write(fd2b, zero, sizeof(zero));
	while(suc==-1) { suc = close(fd2b); }
}

void setupPWM() {
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
	exportPWM();
	printf("DONE\n");
	//%%%%%%%%%Enabling PWM Pins%%%%%%%%%%%%%%%%%
	printf("--Enabling PWM Pins... ");
	enablePWM();
	printf("DONE\n");
	//%%%%%%%%%%%%Defining PWM Period (Must have the same value)%%%%%%%%%%%%%%%%%%%
	printf("--Defining PWM Period... ");
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm0/period", sPeriod);
	setValue("/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm1/period", sPeriod);
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm0/period", sPeriod);
	setValue("/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm1/period", sPeriod);
	printf("DONE\n");
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
}