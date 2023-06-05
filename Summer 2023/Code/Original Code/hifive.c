#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <stdint.h>

/*
 * This is the C source code for the HiFive
 *
 * Author: Fraser Dougall | fdougall@purdue.edu
 *
 * A full explanation of the architecture can be found in this pdf:
 *
 * The summary is:
 *
 * Using a state machine
*/


/*
 * random notes section:
 *
 *  - remember the file size for this MUST be below 500 kilobytes when complete
 *  - the smaller this C file, the more data we can collect (only 3 megabytes to work w/)
 *  - maybe have the heartbeat process do alternating bytes 
 */


/*
 * ----------------------------
 * Custom variable declarations
 * ----------------------------
 */

// Create our states
typedef enum {
	HOLD,
	HSKE,
	PAT0,
	PAT1,
	PAT2,
	PAT3,
	PAT4,
	PAT5,
	CPTE,
	WDTERR
} systemState; 

/*
 * --------------
 * UART Functions
 * --------------
 */

/*
void sendPacket(uint8_t data) {

}

uint8_t readPacket() {

}
*/

/* -----------------------
 * State Machine Functions
 * -----------------------
 */

int handshake(){
	
}

int patternRunner(){
	/*
	 * Variables required
	 */

	systemState state = HOLD;  // status of the system
	bool runStop = false;      // run status 
	int dogErr = 0;			   // watchdog error status
	
	/*
	 * These should honestly probably be put into a header file
	 */

							   // tx: we are sending it to FPGA rx: we are receiving it from fpga
	uint8_t pat;               // pattern to write to RAM  		| tx
	uint8_t handshakePkt = 0xA;// handshake packet         		| tx/rx
	uint8_t endPatPkt = 0xB;   // end of pattern packet    		| rx
	uint8_t endRunPkt = 0xF;   // end of the run           		| rx
	uint8_t readErr = 0xC;     // there was a memory mismatch	| rx

	/*
	 * space to put my thoughts
	 *
	 *  - need a time variable
	 *  - for data analysis purporses, might want total # of writes & reads for FIT rate
	 *  - maybe have a file name variable for ease of use
	 *  - maybe have an acknowledgement packet to tell AFT the failure has recorded
	 *  - send the data to write over UART to minimize size of code on AFT
	 */

	/*
	 * Open the log file here
	 */`

	/*
	 * Start our timer
	 */


	/*
	 * Run the patterns
	 *
	 *
	 * this could probably be optimized so it's DRY
	 * maybe use an array of pattern types and do that and get rid of multiple states
	 * thing with using an array is for things like 0f, f0, and DECAFBAD, it's hard to really do it nicely
	 * at least, i haven't thought of anything good just yet 
	 *
	 * could have an array like {0, F, 5, A, D, E, C, A, F, B, A, D}
	 * and loop through it or something? 
	 */
	while(!runStop) {
		switch(state) {
			case HOLD:
				// Hold state
				
				// wait until some pins are bridged on the hifive
				// not exactly sure how to do this

				break;
			case HSKE:
				// Handshake state
			
				// while we haven't received the echo'd packet, send handshake
				while(readPacket() != handshakePkt) {
					sendPacket(handshakePkt); 
				}

				state = PAT0; 

				break;
			case PAT0:
				// Run the zero pattern

				pat = 0x0; 

				// while we haven't received the end packet, send pattern & do error handling
				while(readPacket() != endPatPkt) {
					/*
					 * for optimization, can probably use a switch statement here
					 * results in only one call to readPacket
					 * unless there are compiler optimizations
					 */

					if(readPacket() == readErr) {
						/*
						 * Log the error to log file
						 */

						
					} else if(readPacket() == endPatPkt) {
						/* 
						 * We're done here, move on 
						 */

						state = PAT1;
					} else if(readPacket() == endRunPkt) {
						/*
						 * We're done in total
						 */

						state = CPTE;
					} else {
						/* No errors or stopage so send pattern over UART
						 */

						sendPacket(pat);
					}

					
					/*
					 * maybe have a wait here (either set time or acknowledgement wait)
					 * so we know AFT is ready for next packet?
					 */

				}

				break;
			case PAT1:
				// Run the 0F pattern
				
				pat = 0x0; 
				
				// while we haven't received the end packet, send it and do error handling
				while(readPacket() != endPatPkt) {
					if(readPacket() == readErr) {
						/*
						 * Log error to log file
						 */
					} else if(readPacket() = endPatPkt) {
						/*
						 * Done with this pattern, go to next pattern
						 */

						state = PAT2;
					} else if(readPacket == endRunPkt) {
						/* 
						 * We're done in total
						 */
						
						state = CPTE;
					} else {
						/*
						 * Just send packet over UART
						 */
						
						/*
						 * maybe send while AFT hasn't acknowledged packet reception?
						 * inb4 two generals problem :kappa:
						 */

						sendPacket(pat);
					}
				}


				break;
			case PAT2:
				// Run the F0 pattern
				
				break;
			case PAT3:
				// Run the 55 pattern

				break;
			case PAT4:
				// Run the AA pattern

				break;
			case PAT5:
				// Run the arbitrary data pattern ( DECAFBAF )

				break;
			case CPTE:
				// Complete State

				break;
			case WDTERR:
				// Watchdog Error

				break;
			default:
				// what

				break;
		} // end case
	}     // end while loop

	/*
	 * End our timer
	 */



	/*
	 * Close the file
	 */

	return dogErr;
}

/*
void complete(){

}

void watchdogErr(){

}
*/

void main() {
    /*
	 * Do any init work
	 */
	int res;

	res = patternRunner();

	if(res == 0) {
		complete();
	} else {
		// will need some more error handling logic here
		// so that we can restart the FPGA and start the handshake process again
		watchdogErr();
	}
}

