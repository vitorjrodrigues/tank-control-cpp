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
- Drag 'SockClient' folder to your Beaglebone's main folder
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
