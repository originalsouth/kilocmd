# kilocmd
Commandline interface to the Kilobots

By BC van Zuiden <zuiden@ilorentz.org> -- Leiden, September 2014 

Probably very buggy USE AT OWN RISK this will brick everything you own

NOBODY but YOU is liable for anything that happened in result of using this

WARNING: DON'T RUN THIS PROGRAM THIS WILL DESTROY YOUR COMPUTER AND/OR KILOBOTS

Based (loosly) on kilogui by Alex Cornejo https://github.com/acornejo/kilogui

## Building
To build run something like:
```
make
```
To clean run:
```
make clean
```

## Running
Make sure you followed the instructions at [Kilobotics](https://www.kilobotics.com/documentation)
After compilation run something like:
```
./kilocmd --help
```
Which produces something like this:
```
kilocmd: WILL DESTROY YOUR COMPUTER AND/OR KILOBOTS!
kilocmd: USE AT OWN RISK!

usage: kilocmd [options] "filename"
if no filename is provided stdin is used (use ^D to exit)
options:
	-s		stfu or silent mode
	-h or --help	produce this output
commands:
	VOLTAGE
	CHARGE
	PAUSE
	RUN
	RESET
	SLEEP
	TOGGLE
	STOP
	BOOTLOAD
	UPLOAD filename (to be implemented)

```
When you are done reading:
```
./kilocmd
```
Should get you started.

## Todo 
* Implement the ability to upload a new program
* Calibration 
* Expand commands
