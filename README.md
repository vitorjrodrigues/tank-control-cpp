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
$> cd /tank-control-master/JSServer
$> make
```

## Change log

###### 23-Set-2018 09h42
- Joystick Reader v1.9, bld 1
  - Equal to Joystick Reader v1.0, bld 1
- JS Socket Client v1.9, bld 1
  - Equal to JS Socket Client v1.1, bld 5
