/*///////////////////////////////////
/// JS Socket Client v1.9, bld 3 ///
/// Vítor Rodrigues, Student@UFPB ///
/// ☼ 23-Sep-2018, ☾ 23-Sep-2018 ///
///////////////////////////////////*/
/*///////////////////////////////////
/// Based on Inet Socket Client   ///
/// Isaac Maia & Lucas Hartmann   ///
/// 2015-2016,  RTS@DEE-UFPB      ///
///////////////////////////////////*/

#define gmsg "Welcome to DualShock Socket Client v0.9.24"

//Define IP Adress for the Server
//#define Server_IP "127.0.0.1" //(LOCAL)
//#define Server_IP "150.165.164.116"
#define Server_IP "192.168.1.103"

#define Period	10000000ul
#define PWMMax	32767;

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <math.h>
#include "create_socket.h"

#include "PWMFunc.h"

int main() {

	//Intermediate Variables
	char key = 0;

	//Socket Variables
	int sock = -1;
	ssize_t sz = -1;

	//Input Variables
	int buffer[3]={0,0,0};

	//Output Variables
	int d_LIA = 0, d_LIB = 0;
	int d_RIA = 0, d_RIB = 0;
	long int la=0, lb=0, ra=0, rb=0;
	
	//PWM Handler Variables
	int fd1a = -1, fd1b = -1, fd2a = -1, fd2b = -1;
	char *zero="0";
	int sent=-1;
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//Greeting Message
	printf("%s!\n",gmsg);

	//Configuring PWM
	setupPWM();
	
	//Starting PWM
	printf("--Initializing PWM Pins... ");
	fd1a = startPWM(PWM1A);
	fd1b = startPWM(PWM1B);
	fd2a = startPWM(PWM2A);
	fd2b = startPWM(PWM2B);
	printf("DONE\n");
	
	//Configuration is Complete
	printf("All Went Well!!\n");
	
	//Wait until Enter Key is pressed
	printf("Press Enter to start connecting...");
	while (key != '\r' && key != '\n') { key = getchar(); }
	key = 0;

	// Open the INET Socket for writing.
	printf("Connecting to the Socket...\n");
	sock = create_socket(0, Server_IP);
	if (sock == -1) { return 1; }

  	// Message receiver loop
	printf("Ready for messages...\n");
	while (1) {

		//Reads the payload
		sz = read(sock, buffer, sizeof(buffer));
		//printf("\nsize %zd\n",sz);

		// read returns -1 if read has failed.
		if (sz == -1) {
			printf("Failed!\n");
			return 1;
		}
		else if (sz == 0) {
			printf("Server Lost!\nTERMINATING");
			return 1;
		}

		switch(buffer[0]) { // IA is positive pole and IB is negative pole of PWM
			case 12:
				d_LIA = buffer[1];
				d_LIB = 0;
				d_RIA = buffer[2];
				d_RIB = 0;
			break;
			case 18:
				d_LIA = 0;
				d_LIB = buffer[1];
				d_RIA = 0;
				d_RIB = buffer[2];
			break;
			case 10:
				d_LIA = buffer[1];
				d_LIB = 0;
				d_RIA = 0;
				d_RIB = buffer[2];
			break;
			case 20:
				d_LIA = 0;
				d_LIB = buffer[1];
				d_RIA = buffer[2];
				d_RIB = 0;
			break;
			case 0:
				d_LIA = 0;
				d_LIB = 0;
				d_RIA = 0;
				d_RIB = 0;
			break;
		}

		//Here we get duty cycle as a function of Period
		la = (d_LIA*Period)/PWMMax;
		lb = (d_LIB*Period)/PWMMax;
		ra = (d_RIA*Period)/PWMMax;
		rb = (d_RIB*Period)/PWMMax;

		// Print back the received message.
		//printf("\t%d\t|\t%d\t|\t%d\t|\t%d\n", d_LIA,d_LIB,d_RIA,d_RIB);

		//PUT HERE THE CODE TO SEND VALUES TO CORRESPONDING PWM PORTS
		//updatePWM(la,lb,ra,rb,fd1a,fd1b,fd2a,fd2b);

		// Print back the received message.
		printf("\t%ld\t\t|\t\t%ld\t\t|\t\t%ld\t\t|\t\t%ld\n", la,lb,ra,rb);
	}

	// Close Output Pins
	printf("Closing Output Pins\n");
	endPWM(fd1a, fd1b, fd2a, fd2b);
	
	// Close the Socket and remove it.
	printf("Closing Socket\n");
	close(sock);

	// All went well, let the OS know it.
	printf("Program Terminated.\n");
	return 0;

}