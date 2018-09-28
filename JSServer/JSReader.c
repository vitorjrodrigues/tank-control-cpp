/*///////////////////////////////////
/// Joystick Reader v1.9, bld 6  ///
/// Vítor Rodrigues, Student@UFPB ///
/// ☼ 22-Sep-2018, ☾ 23-Sep-2018 ///
///////////////////////////////////*/

#define gmsg "Welcome to DualShock Joystick Reader v1.9.6"

//Libraries Included
#include <stdio.h>		//Standard Library. For usage of Input/Output Buffers.
#include <asm/types.h>		//Memory-based variable types definitions.
#include <unistd.h>		//A Basic Library for IPC Applications.
#include <fcntl.h>		//Defines open() function (used in pair with unistd).
#include <sys/socket.h>		//Socket Library
#include <netinet/in.h>     //INET Library
#include <netinet/tcp.h>
#include "create_socket.h"	//Defines create_socket() function


//Server IP is his own IP
//#define Server_IP "127.0.0.1" //(LOCAL)
#define Server_IP "192.168.1.103"
//#define Server_IP "150.165.164.199"

//Joystick Definition for Inputs (Use only ONE at a time)
#include "js_multilaser.h"
//#include "js_dualshock1.h"

//Closes Socket and Joystick
void closure ( int fdJS, int fdSock);

//Detect Key Pressing
void pressKey (char K);

//Disable Buffering from Socket
int disableNag(int sock);

int main(int argc, char *argv[])
{
	//Here we define the struct format for the JS input event
	struct js_event {
		__u32 time;     // event timestamp in milliseconds 
		__s16 value;    // value 
		__u8 type;      // event type 
		__u8 number;    // axis/button number 
	};
	
	//Here we define the struct format for the Tank data
	struct tank {
		int left;	//Left PWM Value (-32767 to 32767)
		int right;	//Right PWM Value (-32767 to 32767)
		int fd; 	//Stores the File Descriptor value for the Joystick
		ssize_t sz;
		int output[2]; //Array to store current pwm values for both motors
	};

	//Here we define the struct format for the Socket Server
	struct socket {
		int status;
		int client;
		int sent;
		int label;
	};

	//Here we create variables for all struct formats
	struct js_event e;
	struct tank 	t;
	struct socket 	server;
	
	//Intermediary Variables
	int hold1=0, hold2=0;	//Temp Boolean Values for key combinations
	int mode=500;		//Temp Boolean Value for operation mode
	int val=0;		//Stores Absolute Value for the current event
	
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	//Greeting Message
	printf("%s!\n",gmsg);

	//Wait until Enter Key is pressed
	pressKey('\r');

	//Open Joystick Port, Read-Only Mode
	printf("Connecting with your device... ");
	t.fd = open ("/dev/input/js0", O_RDONLY);
	
	//Return Value of open() function shows if the connection has succeeded
	if(t.fd<0) { //**************************************************************
		printf("Connection Failed!! :(\nPlease try again.\n");
		return 1;
	}
	else { printf("Connection Successful!! :)\n"); }
	
	//Wait until Enter Key is pressed
	pressKey('\r');
	
	// Creates a server socket and terminate on error
	server.status = create_socket(1, Server_IP);
	if (server.status == -1) { return 1; }
	
	// Disable Nagle
    int res = disableNag(server.status);
     if (res < 0) {
        printf("Nagle could not be disabled\n");
        return 1;
     }
		
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
			//Check if client is responsive
			if(server.sent == -1) {
				printf("Client %d is lost!\n",server.label);
				break;
			}			
			
			//read a value from the event
			t.sz = read (t.fd, &e, sizeof(e));
			
			//If value is an Initialization Value, ignore it.
			if(e.type<T_INITIAL) { //************************************************
				//If SELECT is pressed, abort the program
				if(e.number==BTN_SELECT) {
					printf("\nSELECT Button was pressed.\n");
					break;
				}

				//Check if input is either Axis or Button and stores current value
				if(e.type==T_BUTTON) { //Button values are unsigned booleans
					val = e.value;
				}
				else if(e.type==T_STICK) { //Stick Values are Signed Integers
					val = (-1)*e.value; //Y Axis on Sticks are inverted
				}		
				
				//Detects if R1+△ was pressed
				if(e.type==T_BUTTON&&e.number==BTN_R1)		{ hold1=!hold1; }
				if(e.type==T_BUTTON&&e.number==BTN_TRIANGLE)	{ hold2=!hold2; }

				//If it was pressed, do this
				if(hold1!=0&&hold2!=0) {
					if(mode>1) { mode = 0; }
					mode = !mode;	//Change the operation mode
					sleep(1);	//Wait for one second

					//Announces current mode
					if(mode==0)	{ printf("\nDebug Mode (Motors are OFF)\n"); }
					else		{ printf("\nController Mode (Motors are ON)\n"); }
				}
				
				if(mode==0) { //Debug Mode
					//Show all inputs read on screen but don't write any on socket
					printf ("\nValue = %d;\n",val);
					printf ("Number = %x;\nType = %x\n",e.number,e.type);
				}
				else if (mode == 1){ //Controller Mode //*****************************
					//Ignore all inputs but LAS Y Axis and RAS Y Axis
					if((e.type==T_STICK)&&((e.number==STK_YLEFT)||(e.number==STK_YRIGHT))) {
						//If value is LAS Y Axis, then Update Left PWM 
						if(e.number==STK_YLEFT) { t.left = val; }
						//If value is RAS Y Axis, then Update Right PWM
						else if(e.number==STK_YRIGHT) { t.right = val; }
						//If value is DPAD Y Axis, update both PWM	
						else if(e.number==STK_YDPAD) {
							t.left = val;
							t.right = val;
						}
						//Print Current PWM Values Stored
						printf("\n||\tLPWM\t,\tRPWM\t||");
						printf("\n||\t%d\t,\t%d\t||\n",t.left,t.right);
					}
					
					t.output[0] = t.left;
					t.output[1] = t.right;
					
					//Now Transmit the values through the Socket
					server.sent = write(server.client,  t.output , sizeof(t.output));
				}	
			}
		}
	
		// Terminate the client connection.
		printf("Closing Client %d Connection\n",server.label);
		close(server.client);	
	}
	
	//If no client is connected, end Program
	printf("There are no more clients left. Program will begin termination.");
	closure(t.fd,server.status);
	printf("PROGRAM TERMINATED");
	return 0;
}

void closure ( int fdJS, int fdSock) {
	// Close and remove the server socket
	printf("Closing Server...");
	close(fdSock);
	unlink(Server_IP);
	printf("DONE\n");
	
	//Close the port used by the Joystick 
	printf("Closing Joystick Port...");
	close(fdJS);
	printf("DONE\n");
}

void pressKey (char K) {
	char key = 0;
	if (K == '\r') {
		printf("Press Enter to continue...");
		while (key != K && key != '\n') { key = getchar(); } 
	}
	else {
		printf("Press %c to continue...",K);
		while (key != K) { key = getchar(); } 
	}
}

int disableNag(int sock) {
    int flag = 1;
    int result = setsockopt(sock,            /* socket affected */
                            IPPROTO_TCP,     /* set option at TCP level */
                            TCP_NODELAY,     /* name of option */
                            (char *) &flag,  /* the cast is historical cruft */
                            sizeof(int));    /* length of option value */
    return result;
}
