/*///////////////////////////////////
/// Joystick Reader v1.9, bld 3  ///
/// Vítor Rodrigues, Student@UFPB ///
/// ☼ 22-Sep-2018, ☾ 22-Sep-2018 ///
///////////////////////////////////*/

#define gmsg "Welcome to DualShock Joystick Reader v1.9.3"

//Libraries Included
#include <stdio.h>		//Standard Library. For usage of Input/Output Buffers.
#include <stdlib.h>		//Extended variable and function definitions.
#include <string.h>		//String-based Operations Library.
#include <unistd.h>		//A Basic Library for IPC Applications.
#include <pthread.h>		//Basic thread library
#include <poll.h>		//Definitions for the poll() function
#include <fcntl.h>		//Defines open() function (used in pair with unistd).
#include <sys/stat.h>		//Defines data returned by the stat() function.
#include <errno.h>		//Defines errno (system error value) symbol.
#include <asm/types.h>		//Memory-based variable types definitions.
#include <math.h>		//Extension Library for Math operations
#include <ctype.h>		//Assembly Type Definitions
#include <sys/socket.h>		//Socket Library
#include "create_socket.h"	//Defines create_socket() function

//Server IP is his own IP
#define Server_IP "127.0.0.1" //(LOCAL)
//#define Server_IP "150.165.164.116"

//Joystick Definition for Inputs (Use only ONE at a time)
#include "js_multilaser.h"
//#include "js_dualshock1.h"

//Creates a Logic Value based on PWM direction for each wheel
int makeLogic(int leftInput, int rightInput);

//Closes Socket and Joystick
void bigClosure ( int fdJS, int fdSock);

//Detect Key Pressing
void pressKey (char K);

int main(int argc, char *argv[])
{
	//Here we define the struct format for the JS input event
	struct js_event {
		__u32 time;     // event timestamp in milliseconds 
		__s16 value;    // value 
		__u8 type;      // event type 
		__u8 number;    // axis/button number 
	};
	
	//Here we define the struct format for the JS data
	struct js_data {
		int left;	//Left PWM Value (0 to 32767)
		int right;	//Right PWM Value (0 to 32767)
		int logic;  //Logic Storage Variable (based on lsign and rsign values)
		int lsign;
		int rsign; //Left and Right Sign Vectors (Can be 1, 0 or -1)
		char rsignus;
		char lsignus; //Ascii Sign Characters for Left and Right PWM ('+' or '-')
		int fd; //Stores the File Descriptor value for the Joystick (His connection number)
		ssize_t sz;
		int output[3]; //Array to store payload to be sent
	};

	//Here we define the struct format for the Socket Server
	struct socket {
		int status;
		int client;
		int sent;
		int label;
	};

	//Here we create the variable e with the js_event format
	struct js_event e;
	
	//Here we create the variable tank with the js_data format
	struct js_data tank;

	//Here we create the variable serv with the socket format
	struct socket server;
	
	//Intermediate Variables
	int hold1=0, hold2=0; //Temp Boolean Values for key combinations
	int mode=500;	//Temp Boolean Value for operation mode

	//Current Value Variables
	int sign=0;	//Stores Sign Vector for the current event (Can be 1, 0 or -1)
	int val=0;	//Stores Absolute Value for the current event
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	//Greeting Message
	printf("%s!\n",gmsg);

	//Wait until Enter Key is pressed
	pressKey('\r');

	//Open Joystick Port, Read-Only Mode
	printf("Connecting with your device... ");
	tank.fd = open ("/dev/input/js0", O_RDONLY);
	
	//Return Value of open() function shows if the connection has succeeded
	if(tank.fd<0) { //**************************************************************
		//Connection Failed. Report it on a Status Message
		printf("Connection Failed!! :(\nPlease try again.\n");
		return 1;
	}
	//Connection Succeeded. Report Status to User
	else { printf("Connection Successful!! :)\n"); }
	
	//Wait until Enter Key is pressed
	pressKey('\r');
	
	// Creates a server socket and terminate on error
	server.status = create_socket(1, Server_IP);
	if (server.status == -1) { return 1; }
		
	// Listener loop
	while (1) {
		printf("Ready for clients...\n");
		
		// Wait for connections on the socket server.
		// Each client gets a new private socket.
		server.client = accept(server.status, 0, 0);
		printf("Got a client!\nAll Ready!\n");
		printf(">>>Press SELECT to Exit<<<\n>>>Press R1+△ to Turn ON/OFF the Vehicle<<<\n");
		server.label = server.client - 3;
		
		// Client message loop:
		while (1) { //If all goes well, the program should start HERE
			
			//read a value from the event
			tank.sz = read (tank.fd, &e, sizeof(e));
			
			//If value is an Initialization Value, ignore it.
			if(e.type<T_INITIAL) { //*************************************************************
				//Stores current absolute value
				val = abs(e.value);
				
				//Check if input is either an Axis value or a Button value
				//Button values are unsigned booleans
				if(e.type==T_BUTTON) { sign=0; }
				else if(e.type==T_STICK) { //Stick Values are Signed Integers
					//Determine signs on a Stick Value (negative, positive or null)
					if(e.value<0) 		{ sign=1; }
					else if(e.value>0)	{ sign=-1; }
					else 				{ sign=0; }
				}
				
				//If SELECT is pressed, then abort the program
				if(e.number==BTN_SELECT) {
					printf("\nSELECT Button was pressed.\n");
					break;
				}
				
				//Detects if R1+△ was pressed
				if(e.type==T_BUTTON&&e.number==BTN_R1)			{ hold1=!hold1; }
				if(e.type==T_BUTTON&&e.number==BTN_TRIANGLE)	{ hold2=!hold2; }

				//If it was pressed, then do this
				if(hold1!=0&&hold2!=0) {
					if(mode>1) { mode = 0; }
					mode = !mode;	//Change the operation mode
					sleep(1);	//Wait for one second

					//Announces current mode
					if(mode==0)	{ printf("\nTank Controller (TC) Mode OFF\nDebug Mode ON\n"); }
					else		{ printf("\nTank Controller (TC) Mode ON\nDebug Mode OFF\n"); }
				}
				
				if(mode==0) { //Debug Mode
					//Show all inputs read on screen but don't write any on socket
					printf("\nValue = %c%u;\nType = %x\n",(44-sign),val,e.type);
				}
				else if (mode == 1){ //TC Mode //*******************************************************
					//Ignore all inputs but LAS Y Axis and RAS Y Axis
					if((e.type==T_STICK)&&((e.number==STK_YLEFT)||(e.number==STK_YRIGHT))) {
						if(e.number==STK_YLEFT) { //If current value is LAS Y Axis, then Update all Left PWM Values 
							tank.left = val;
							tank.lsign = sign;
							if(tank.lsign!=0)	{ tank.lsignus = 44 - tank.lsign; }
							else			{ tank.lsignus = ' '; }
						}
						else if(e.number==STK_YRIGHT) { //If current value is RAS Y Axis, then Update all Right PWM Values
							tank.right = val;
							tank.rsign = sign;
							if(tank.rsign!=0)	{ tank.rsignus = 44 - tank.rsign; }
							else			{ tank.rsignus = ' '; }
						}

						//Creates a logic value corresponding each possible direction of movement
						tank.logic = makeLogic(tank.lsign, tank.rsign);

						//Print Current PWM Values Stored
						printf("\n||\tLPWM\t,\tRPWM\t||");
						printf("\n||\t%c%d\t,\t%c%d\t||\n", tank.lsignus,tank.left,tank.rsignus,tank.right);
					}
					
					tank.output[0] = tank.logic;
					tank.output[1] = tank.left;
					tank.output[2] = tank.right;
					
					//Now Transmit the values through the Socket
					server.sent = write(server.client,  tank.output , sizeof(tank.output));
					
					if(server.sent == -1) {
						printf("Client %d is lost!\n",server.label);
						break;
					}
				}	
			}
		
		}
	
		// Terminate the client connection.
		printf("Closing Client %d Connection\n",server.label);
		close(server.client);
		
		//If no client is connected, ask if the user want to end the server
		pressKey('x');
	}
	
	//Close Socket and then close Joystick Port
	bigClosure(tank.fd,server.status);

	//End of Program
	printf("PROGRAM TERMINATED");
	return 0;
}

int makeLogic(int leftInput, int rightInput) {
	int l = 0;
	//Logic Values are Arbitrary
	if((leftInput>=0)&&(rightInput>=0))	{ // left>0 && right>0
		l = 12;
		printf("\n\tGO FORWARD");
	}
	else if((leftInput<=0)&&(rightInput<=0)) { // left<0 && right<0
		l = 18;
		printf("\n\tGO BACKWARDS");
	}
	else if((leftInput<=0)&&(rightInput>=0)) { // left<0 && right>0
		l = 20;
		printf("\n\tTURN LEFT");
	}
	else if((leftInput>=0)&&(rightInput<=0)) { 	// left>0 && right<0		
		l = 10;
		printf("\n\tTURN RIGHT");
	}
	else if((leftInput==0)&&(rightInput==0)) {
		l = 0;
		printf("\n\tSTOP");
	}
	return l;
}

void bigClosure ( int fdJS, int fdSock) {
	// Close and remove the server socket
	printf("Closing Server...");
	close(fdSock);
	unlink(Server_IP);
	printf("DONE\n");
	
	//Close the Port in which the joystick was opened
	printf("Closing Joystick Port...");
	close(fdJS);
	printf("DONE\n");
}

void pressKey (char K) {
	char key = 0;
	if (K == '\r') {
		printf("Press Enter to connect with the Socket Client...");
		while (key != K && key != '\n') { key = getchar(); } 
	}
	else if (K == 'x') {
		printf("There are no more clients left, do you wish to exit server? (y/n) ");
		while (key != 'y' && key != 'n' && key!= 'Y' && key!= 'N') { key = getchar(); }
		if (key =='Y' || key == 'y')		{ exit(0); }
		else if (key == 'n' || key == 'N')	{ printf("OK. The Server will remain open.\n"); }
	}
	else {
		printf("Press %c to continue...",K);
		while (key != K) { key = getchar(); } 
	}
}
