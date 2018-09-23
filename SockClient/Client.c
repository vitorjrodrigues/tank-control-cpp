/*///////////////////////////////////
/// JS Socket Client v1.9, bld 6 ///
/// Vítor Rodrigues, Student@UFPB ///
/// ☼ 23-Sep-2018, ☾ 23-Sep-2018 ///
///////////////////////////////////*/
/*///////////////////////////////////
/// Based on Inet Socket Client   ///
/// Isaac Maia & Lucas Hartmann   ///
/// 2015-2016,  RTS@DEE-UFPB      ///
///////////////////////////////////*/

#define gmsg "Welcome to DualShock Socket Client v1.9.6"

//Define IP Adress for the Server
//#define Server_IP "127.0.0.1" //(LOCAL)
//#define Server_IP "150.165.164.116"
#define Server_IP "192.168.1.103"

#define Period	10000000ull
#define PWMMax	32767;

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <asm/types.h>		//Memory-based variable types definitions.
#include "create_socket.h"
//#include <pthread.h>		//Basic thread library
//#include <poll.h>		//Definitions for the poll() function

#include "PWMFunc.h"

int main() {

	//Defining Socket Client Struct
	struct socket_client {
		int sock;
		ssize_t sz;
	};

	//Defining Input Data Struct
	struct input_data {
		int put[2];
		int LIA;
		int LIB;
		int RIA;
		int RIB;
	};


	//Defining variables for all struct formats
	struct socket_client client;
	struct input_data in;
	struct pwm_data out;
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//Greeting Message
	printf("%s!\n",gmsg);

	//Configuring PWM
	setupPWM();
	
	//Starting PWM
	printf("--Initializing PWM Pins... ");
	out.fd1a = startPWM(PWM1A);
	out.fd1b = startPWM(PWM1B);
	out.fd2a = startPWM(PWM2A);
	out.fd2b = startPWM(PWM2B);
	printf("DONE\n");
	
	//Configuration is Complete
	printf("All Went Well!!\n");

	// Open the INET Socket for writing.
	printf("Connecting to the Socket...\n");
	client.sock = create_socket(0, Server_IP);
	if (client.sock == -1) { return 1; }

  	// Message receiver loop
	printf("Controller connected...\n");
	while (1) {

		//Reads the payload
		client.sz = read(client.sock, in.put, sizeof(in.put));

		// read returns -1 if read has failed.
		if (client.sz == -1) {
			printf("Failed!\n");
			return 1;
		}
		else if (client.sz == 0) {
			printf("Server Lost!\nTERMINATING");
			return 1;
		}
		
		if(in.put[0]>=0) {
			in.LIA = in.put[0];
			in.LIB = 0;
		}
		else {
			in.LIA = 0;
			in.LIB = (-1)*in.put[0];
		}
		
		if(in.put[1]>=0) {
			in.RIA = in.put[1];
			in.RIB = 0;
		}
		else {
			in.RIA = 0;
			in.RIB = (-1)*in.put[1];
		}
		
		//Here we get duty cycle as a function of Period
		out.la = (in.LIA*Period)/PWMMax;
		out.lb = (in.LIB*Period)/PWMMax;
		out.ra = (in.RIA*Period)/PWMMax;
		out.rb = (in.RIB*Period)/PWMMax;
		
		//And their percentual value
		out.perla = out.la*100/Period;
		out.perlb = out.lb*100/Period;
		out.perra = out.ra*100/Period;
		out.perrb = out.rb*100/Period;

		///Print back the received message.
		//printf("\t%d\t|\t%d\t|\t%d\t|\t%d\n", in.LIA,in.LIB,in.RIA,in.RIB);

		//Update the PWM Duty Cycle on the four Output pins
		updatePWM(out.la,out.lb,out.ra,out.rb,out.fd1a,out.fd1b,out.fd2a,out.fd2b);

		// Print back the received message.
		printf("\t%lli%\t\t%lli%\t\t%lli%\t\t%lli%\n", out.perla,out.perlb,out.perra,out.perrb);
	}

	// Close Output Pins
	printf("Closing Output Pins\n");
	endPWM(out.fd1a, out.fd1b, out.fd2a, out.fd2b);
	
	// Close the Socket and remove it.
	printf("Closing Socket\n");
	close(client.sock);

	// All went well, let the OS know it.
	printf("Program Terminated.\n");
	return 0;

}