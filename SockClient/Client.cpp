/*///////////////////////////////////
/// JS Socket Client v1.9, bld 12 ///
/// Vítor Rodrigues, Student@UFPB ///
/// ☼ 23-Sep-2018, ☾ 25-Sep-2018  ///
///////////////////////////////////*/
/*///////////////////////////////////
/// Based on Inet Socket Client   ///
/// Isaac Maia & Lucas Hartmann   ///
/// 2015-2016,  RTS@DEE-UFPB      ///
///////////////////////////////////*/

#define gmsg "Welcome to DualShock Socket Client v1.9.12"

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

#include "create_socket.h"
#include "sysfsHelper.h"
#include "Setup.h"
#include "speedFeedback.h"
#include "Socket.h"

using namespace std;

//Defining variables for all struct formats
pwm_data_t out;
socket_input_t client;

int main() {
	//Greeting Message
	printf("%s!\n",gmsg);

	//Create and Start the MUTEX
	pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, 0);
    
    //Setup PWM
	setupPWM(Period);

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
	
	//Create Feedback Sensors for both Motors
	printf("Creating Feedback...\n");
	auto fb_left  = feedback_create(48, &out.cmd_l, &mutex);
    auto fb_right = feedback_create(60, &out.cmd_r, &mutex);
    printf("DONE\n");

	// Open the INET Socket for writing.
	printf("Connecting to the Socket...\n");
	auto sck_cl = client_create(Server_IP, &mutex);

	//Configuration is Complete
	printf("All Went Well!!\n");

  	// Message receiver loop
	printf("Controller connected...\n");
	for (int i=0; i<10; i++) {
        sleep(1);
        pthread_mutex_lock(&mutex);
        
        cout << i << ": "
        << "(left: pos=" << fb_left->pos << ", speed=" << fb_left->speed << ") "
        << "(right: pos=" << fb_right->pos << ", speed=" << fb_right->speed << ")" 
        << "(cmd[0] = "<< sck_cl->cmd[0] << ", cmd[1] = " << sck_cl->cmd[1] << ")" << endl;
        
        pthread_mutex_unlock(&mutex);
    }
	while (0) {

		//
		out.cmd_l = (double)sck_cl->cmd[0];
		out.cmd_r = (double)sck_cl->cmd[1];
		
		// Print back the received values (debug only)
		//printf("\t%d\t|\t%d\t|\t%d\t|\t%d\n", sck_cl->LIA,sck_cl->LIB,sck_cl->RIA,sck_cl->RIB);
		
		// Here we get duty cycle as a function of Period
		out.la = (sck_cl->LIA*Period)/PWMMax;
		out.lb = (sck_cl->LIB*Period)/PWMMax;
		out.ra = (sck_cl->RIA*Period)/PWMMax;
		out.rb = (sck_cl->RIB*Period)/PWMMax;
		
		// And their relative period reference values
		double lperiod = (double)out.la - (double)out.lb;
		double rperiod = (double)out.ra - (double)out.rb;
		
		// Which are used to calculate reference speed values
		out.lspeed = 1/lperiod;
		out.rspeed = 1/rperiod;

		// Update the PWM Duty Cycle on the four Output pins
		updatePWM(out.la,out.lb,out.ra,out.rb,out.fd1,out.fd2);

		// Print back the received speeds.
		printf("|\t\tLeft\t\t|\t\tRight\t\t|\n");
		printf("|\t\t%lf mm/s\t\t|\t\t%lf mm/s\t\t|", out.lspeed, out.rspeed);
	}
	
	// Guarantee that the threads will end
	pthread_mutex_lock(&mutex);
    fb_left->gameOver = true;
    fb_right->gameOver = true;
    sck_cl->gameOver = true;
    pthread_mutex_unlock(&mutex);

	// Close Output Pins
	printf("Closing Output Pins\n");
	endPWM(out.fd1, out.fd2);
	
	// Join Threads for closure
	printf("Closing Feedback Sensors\n");
    pthread_join(fb_left->thread_id, NULL); 
    pthread_join(fb_right->thread_id, NULL); 

	// Close the Socket and remove it.
	printf("Closing Socket\n");
	pthread_join(sck_cl->thread_id, NULL);
	
	// All went well, let the OS know it.
	printf("Program Terminated.\n");
	return 0;

}