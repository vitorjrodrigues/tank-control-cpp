/*///////////////////////////////////
/// JS Socket Client v1.9, bld 8 ///
/// Vítor Rodrigues, Student@UFPB ///
/// ☼ 23-Sep-2018, ☾ 23-Sep-2018 ///
///////////////////////////////////*/
/*///////////////////////////////////
/// Based on Inet Socket Client   ///
/// Isaac Maia & Lucas Hartmann   ///
/// 2015-2016,  RTS@DEE-UFPB      ///
///////////////////////////////////*/

#define gmsg "Welcome to DualShock Socket Client v1.9.8"

//Define IP Adress for the Server
//#define Server_IP "127.0.0.1" //(LOCAL)
//#define Server_IP "150.165.164.116"
#define Server_IP "192.168.1.103"

#define Period	10000000ull
#define PWMMax	32767;

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <asm/types.h>		//Memory-based variable types definitions.
#include <pthread.h>		//Basic thread library
#include <time.h>
#include <poll.h>		//Definitions for the poll() function

#include "create_socket.h"
#include "PWMFunc.h"

using namespace std;

pwm_data out;

int fd_value;

uint64_t getTimeNanoseconds();

void waitForRisingEdge();

void *speedy(void *);

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
	
	pthread_t lspeed=0, rspeed=0, position=0, thPWM=0;
	pthread_create(&lspeed, 0, &speedy, 0);
	pthread_create(&rspeed, 0, &speedy, 0);
	
	
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

	// Join Threads Created
	pthread_join(lspeed, 0);
	pthread_join(rspeed, 0);
	
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

uint64_t getTimeNanoseconds() {
    timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * 1000000000ULL + tv.tv_nsec;
}

void waitForRisingEdge() {
    pollfd pfd[1];
    pfd[0].fd = fd_value;
    pfd[0].events = POLLPRI | POLLERR;
    pfd[0].revents = 0;
    
    if (poll(pfd, 1, -1) < 0) {
        exit(1);
    }
    lseek(fd_value, 0, SEEK_SET);
    char dummy[16];
    read(fd_value, dummy, 16);
}

int set_sysfs(const char *fname, string value) {
    int fd = open(fname, O_WRONLY);
    if (fd < 0) return fd;
     
    int n = write(fd, value.c_str(), value.length());

    close(fd);
    
    return n;
}

#define FUU(deu) { cout << "FUU" << deu << endl; return 1; }

void *speedy(void *) {
    // Setup stuff
    set_sysfs("/sys/class/gpio/export", "48");
    set_sysfs("/sys/class/gpio/gpio48/direction", "in");
    set_sysfs("/sys/class/gpio/gpio48/edge", "rising");
    
    fd_value = open("/sys/class/gpio/gpio48/value", O_RDWR);
    if (fd_value < 0) { exit(0); }
    
    // Wait for a rising edge
    waitForRisingEdge();
    
    // Get "old" time 
    uint64_t old_time = getTimeNanoseconds();
    
    //
    uint64_t lastUpdated = old_time;

    while (true) {
        // Wait for a rising edge
        waitForRisingEdge();
        
        // Get current time 
        uint64_t new_time = getTimeNanoseconds();
        
        // Compute period
        double period = (new_time - old_time) * 1e-9;

        // Update old time
        old_time = new_time;
        
        // Print
        if (new_time - lastUpdated > 1000000000) {
            lastUpdated = new_time;
            cout << 1/period << " mm/s" << endl;
        }
    }
    
}
