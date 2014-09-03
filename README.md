# kilocmd
A command line interface for the Kilobots
```
/* By BC van Zuiden -- Leiden, September 2014 */
/* Probably very buggy USE AT OWN RISK this will brick everything you own */
/* NOBODY but YOU is liable for anything that happened in result of using this */
/* WARNING: DON'T RUN THIS PROGRAM THIS WILL DESTROY YOUR COMPUTER AND/OR KILOBOTS */
/* Based (loosly) on kilogui by Alex Cornejo https://github.com/acornejo/kilogui */
```
This project acknowledges:
* [kilogui](https://github.com/acornejo/kilogui)
* [libintelhex](https://github.com/bfoz/libintelhex)

## Building
This program requires [libftdi](http://www.intra2net.com/en/developer/libftdi/index.php) (libftdi-devel,libftdi-c++-devel in Fedora 20). 
To build run something like:
```
make
```
To clean run:
```
make clean
```
You can install by copying the compiled binary to your path.

## Running
Make sure you have followed the instructions at [Kilobotics](https://www.kilobotics.com/documentation)  
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
	-s or --stfu	startup in silent mode
	-h or --help	produce this output
commands:
	VOLTAGE		    display voltage level using LED (blue/green = fully charged, yellow/red = need battery)
	CHARGE		    sets the kilobot in charge mode
	PAUSE		    pause the user program
	RUN		        runs the currently uploaded program
	RESET		    jump to the user program starting point
	SLEEP		    switch to low-power sleep mode
	TOGGLE		    toggle LEDs on controller
	STOP		    sends an empty message
	BOOTLOAD	    jump to their bootloader to accept a new program
	UPLOAD filename	send a new program to kilobots that are in bootloader mode
	STFU		    toggle silent mode

```
When you are done reading:
```
./kilocmd
```
Should get you started.  
If you are lazy try:
```
rlwrap ./kilocmd
```
