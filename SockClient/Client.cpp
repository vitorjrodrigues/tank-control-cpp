/*///////////////////////////////////
/// JS Socket Client v1.9, bld 10 ///
/// Vítor Rodrigues, Student@UFPB ///
/// ☼ 23-Sep-2018, ☾ 25-Sep-2018  ///
///////////////////////////////////*/
/*///////////////////////////////////
/// Based on Inet Socket Client   ///
/// Isaac Maia & Lucas Hartmann   ///
/// 2015-2016,  RTS@DEE-UFPB      ///
///////////////////////////////////*/

#define gmsg "Welcome to DualShock Socket Client v1.9.10"

//Define IP Adress for the Server
#define Server_IP "127.0.0.1" //(LOCAL)
//#define Server_IP "150.165.164.116"
//#define Server_IP "192.168.1.103"

#define Period	10000000ull
#define PWMMax	32767;

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>		//Basic thread library
#include <time.h>
#include <poll.h>		//Definitions for the poll() function

#include "create_socket.h"
#include "sysfsHelper.h"
#include "Setup.h"

using namespace std;

//Defining variables for all struct formats
pwm_data out;
socket_client client;
input_data in;

int main() {

	//Greeting Message
	printf("%s!\n",gmsg);

	//Setups
	setupPWM(Period);
	setupSPD();
	
	//Starting PWM
	printf("--Initializing PWM Pins... ");
	for (int i = 0; i < 2; i++) {
		char b[128];
		snprintf(b, 128, "/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/pwmchip3/pwm%d/duty_cycle",i);
		out.fd1[i] = startValue(b);
		snprintf(b, 128, "/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/pwmchip6/pwm%d/duty_cycle",i);
		out.fd2[i] = startValue(b);
	}
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
			printf("Server Lost!\n");
			break;
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
		
		///Print back the received values (for debug only).
		//printf("\t%d\t|\t%d\t|\t%d\t|\t%d\n", in.LIA,in.LIB,in.RIA,in.RIB);
		
		//Here we get duty cycle as a function of Period
		out.la = (in.LIA*Period)/PWMMax;
		out.lb = (in.LIB*Period)/PWMMax;
		out.ra = (in.RIA*Period)/PWMMax;
		out.rb = (in.RIB*Period)/PWMMax;
		
		//And their relative period reference values
		double lperiod = (double)out.la - (double)out.lb;
		double rperiod = (double)out.ra - (double)out.rb;
		
		//Which are used to calculate reference speed values
		out.lspeed = 1/lperiod;
		out.rspeed = 1/rperiod;

		//Update the PWM Duty Cycle on the four Output pins
		updatePWM(out.la,out.lb,out.ra,out.rb,out.fd1,out.fd2);

		// Print back the received speeds.
		printf("|\t\tLeft\t\t|\t\tRight\t\t|\n");
		printf("|\t\t%lf mm/s\t\t|\t\t%lf mm/s\t\t|", out.lspeed, out.rspeed);
	}
	
	// Close Output Pins
	printf("Closing Output Pins\n");
	endPWM(out.fd1, out.fd2);
	
	// Close the Socket and remove it.
	printf("Closing Socket\n");
	close(client.sock);

	// All went well, let the OS know it.
	printf("Program Terminated.\n");
	return 0;

}