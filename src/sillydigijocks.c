#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */



// GLOBAL SCREEN BUFFER (2304 pixels)
char screen[1152];

// GLOBAL ANALOG INPUTS
char joy0hor = 0;
char joy1hor = 0;
char joy0vert = 0;
char joy1vert = 0;

// Define threshold voltages
#define THRESHVERTUP 3.5
#define THRESHVERTDO 1.5

void main(void) {
  /* put code here */

	// Load/Display Spalsh Screen

	/* Main Loop. We never need to exit from here. */
	while(1)
	{
			// Display Menu Screen
			// Check for Menu Selection
			// Use case statement to branch to appropriate
			// Sub Function
			// Menu:
			//	Select Character
			//	Select Field
			//	Start Match
	}
	

	/* Don't modify below here. */
	EnableInterrupts;


  for(;;) {
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
