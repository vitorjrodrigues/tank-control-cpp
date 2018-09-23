# JS Kernel Tank Control

## Requirements
- C++ Compiler
- Joystick with two Analog Sticks and at least 3 buttons
  - Preferrably DualShock or similar
- BeagleBone Black
- Also an USB Adapter for your Joystick if it uses another standard interface

## How to Install

#### Step 1 - Downloading
- Clone the repository via terminal:
```
$>  git clone https://github.com/vitorjrodrigues/tank-control.git
```
or
- Download the directory as .zip and extract it wherever

#### Step 2 - Configuring Client
- Open your Beaglebone via Browser:
  - 192.168.7.2 for Windows Users
  - 192.168.6.2 for Linux Users
- Open up the Cloud9 IDE
- Drag _SockClient_ folder to your Beaglebone's main folder
- To compile everything, type on your cloud9 terminal:
```
$> cd /var/lib/cloud9/SockClient 
$> make
```

#### Step 3 - Configuring Joystick
- Connect your Joystick
- Check if its functioning properly:
```
$> cat /dev/input/js0 | hexdump −C
```
- Compile everything on JS Reader. In your main computer, open a terminal:
```
$> cd /tank-control/JSServer
$> make
```

#### Step 4 - Configuring Tank
- Find your L+, L-, R+ and R- motor pins
- Connect these pins on their respective Beaglebone Black pins
  - L+ on P9_14
  - L- on P9_16
  - R+ on P8_19
  - R- on P8_13 
- Connect your Beaglebone Black to your network source (Cable or Antenna)
- Connect your power supply (not needed if connected on USB cable)

#### Step 5 - Executing Programs
- On your Computer Terminal, execute the server:
```
$> ./JSReader
```
- Follow the instructions on the screen until the server connects
- After the server is on, go to your Beaglebone's Cloud9 Terminal and execute the client:
```
$> ./Client
```
- Follow the instructions until the client connects.
- Server will tell whenever it's ready to start transmitting

#### Step 6 - Sending Commands
- With your Joystick in hands, enter _Controller Mode_
- Use Left and Right Analog Sticks to control the tank
  - Sticks can only send up and down values, meaning forwards and backwards to their respective motors
  
## Change log

###### 23-Set-2018 09h42
- Joystick Reader v1.9, bld 1
  - Equal to Joystick Reader v0.9, bld 15
- JS Socket Client v1.9, bld 1
  - Equal to JS Socket Client v0.9, bld 24
  
###### 23-Set-2018 11h05
- Joystick Reader v1.9, bld 2
  - Output Variables and Socket Variables are now part of their own Structs
  
###### 23-Set-2018 11h26
- Joystick Reader v1.9, bld 3
  - Key Pressing detection is now an independent function
  - PWM now goes from 0 to 32767 (Previously 0 to 254)

###### 23-Set-2018 12h37
- Joystick Reader v1.9, bld 4
  - PWM now goes from -32767 to 32767 (Previously 0 to 32767)
  - Output buffer now only contains 2 values (Logic values were discarded)
  
###### 23-Set-2018 16h42
- Joystick Reader v1.9, bld 5
  - Reader now sends DPAD inputs (32767, 0 and -32767)
  
###### 23-Set-2018 17h00
- JS Socket Client v1.9, bld 3
  - Client compatible with new PWM value (-32767 to 32767)
  
###### 23-Set-2018 18h23
- JS Socket Client v1.9, bld 6
  - Now independent from logic value
  - Structs holding main variables
