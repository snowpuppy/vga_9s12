/***********************************************************************
; ECE 362 - Mini-Project ASM Source File - Spring 2012                     
;***********************************************************************
;	 	   			 		  			 		  		
; Team ID: < ? >
;
; Project Name: < ? >
;
; Team Members:
;
;   - Team/Doc Leader: < ? >      Signature: ______________________
;   
;   - Software Leader: < ? >      Signature: ______________________
;
;   - Interface Leader: < ? >     Signature: ______________________
;
;   - Peripheral Leader: < ? >    Signature: ______________________
;
;
; Academic Honesty Statement:  In signing above, we hereby certify that we 
; are the individuals who created this HC(S)12 source file and that we have
; not copied the work of any other student (past or present) while completing 
; it. We understand that if we fail to honor this agreement, we will receive 
; a grade of ZERO and be subject to possible disciplinary action.
;
;***********************************************************************
;
; The objective of this Mini-Project is to .... < ? >
;
;
;***********************************************************************
;
; List of project-specific success criteria (functionality that will be
; demonstrated):
;
; 1.
;
; 2.
;
; 3.
;
; 4.
;
; 5.
;
;***********************************************************************
;
;  Date code started: 3/23/2012
;
;  Update history (add an entry every time a significant change is made):
;
;  Date: 3/31/2012  Name: Thor Smith   Update: Added Menu Logic. Started gameplay logic.
;
;  Date: < ? >  Name: < ? >   Update: < ? >
;
;  Date: < ? >  Name: < ? >   Update: < ? >
;
;
;***********************************************************************/
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>

// Enable/disable debugging with serial port (i.e. inchar/outchar/pmsg)
#define USESCIDEBUGGING 1

// Define threshold voltages
#define JOYTHRESH 15
#define BASETHRESH 5
#define THRESHUP ( 25 + JOYTHRESH)
#define THRESHDO ( 25 - JOYTHRESH)

// define button layouts/masks
#define LEFTPB 0x06


// define screen resolution
#define SCREENW 48
#define SCREENH 48

// include images. These are in a separate file
// because they're dynamically generated.
#include "ourimages.h"

// All funtions after main should be initialiezed here
char inchar(void);
void outchar(char x);
void displaySplash(void);
void displayMenu(char selection);
void checkMenuInputs(unsigned char joyin);
void selectCharacter(void);
void selectField(void);
void startMatch(void);


// Variable declarations  
// Count horizontal pulses
unsigned int hCnt = 0;

//Vsync IRQ flag (used once at start to position screen)
unsigned char vSyncFlag = 0;

// count vertical lines across the screen to verify that we have the
// right number of pixel height
unsigned char line_hold_count = 0;
	   			 		  			 		       
// GLOBAL SCREEN BUFFER (2304 pixels)
unsigned char screen[1152];
unsigned char *screen_itterator = screen;

// GLOBAL ANALOG INPUTS   --- 0 is for player 0; 1 is for player 1
unsigned char joy0hor = 0;
unsigned char joy1hor = 0;
unsigned char joy0vert = 0;
unsigned char joy1vert = 0;

// Menu selection variables
// button select
char select = 0;
// joystic selections
char selection = -1;

// ASCII character definitions
//int CR = 0x0D;//Return       ***** Use '\r' instead and use '\n' for newline
	 	   		
/***********************************************************************
Initializations
***********************************************************************/
void  initializations(void) {

//; Set the PLL speed (bus clock = 24 MHz)
  CLKSEL = CLKSEL & 0x80; //; disengage PLL from system
  PLLCTL = PLLCTL | 0x40; //; turn on PLL
  SYNR = 0x02;            //; set PLL multiplier
  REFDV = 0;              //; set PLL divider
  while (!(CRGFLG & 0x08)){  }
  CLKSEL = CLKSEL | 0x80; //; engage PLL

// Disable watchdog timer (COPCTL register)
  COPCTL = 0x40   ; //COP off; RTI and COP stopped in BDM-mode

#if USESCIDEBUGGING
// Initialize asynchronous serial port (SCI) for 9600 baud, no interrupts
  SCIBDH =  0x00; //set baud rate to 9600
  SCIBDL =  0x9C; //24,000,000 / 16 / 156 = 9600 (approx)  dec=26
  SCICR1 =  0x00; //$9C = 156
  SCICR2 =  0x0C; //initialize SCI for program-driven operation
  DDRB   =  0x10; //set PB4 for output mode
  PORTB  =  0x10; //assert DTR pin on COM port
#endif
            
// Initialize interrupts
   // RBG,PWM,TIM outputs
   // PTT 0 - PWM
   // PTT 1 - TIM
   // PTT 2 - B2
   // PTT 3 - G2
   // PTT 4 - R2
   // PTT 5 - B1
   // PTT 6 - G1
   // PTT 7 - R1
   DDRT = 0xFF; 
   PTT = 0x00;
        
}

	 		  			 		  		
/***********************************************************************
Main
***********************************************************************/
void main(void) {
  DisableInterrupts;
	initializations(); 		  			 		  		
	EnableInterrupts;
	
	//enables external xirq after vSync IRQ
	vSyncFlag = 0;
	while((vSyncFlag != 1) & (hCnt == 0)) {}	      
  asm andcc #$BF 

//////////////////////////////////////////////////////////////
//;  START OF CODE FOR Spring 2012 MINI-PROJECT
//////////////////////////////////////////////////////////////
  
	// Load/Display Spalsh Screen
	displaySplash();

  for(;;) {
   // write code here (Insert Code down here because we need an infinite loop.)

	// Display Menu Screen
	displayMenu(selection);
	// Check for Menu Selection
	checkMenuInputs(joy0vert);
	// Use case statement to branch to appropriate selection.
	// Don't branch unless the user has triggered the 'select' button.
	if (select == 1)
	{
			select = 0;
			switch (selection)
			{
			// Sub Function
			// Menu:
			case 1:
					//	Select Character
					selectCharacter();
					break;
			case 2:
					//	Select Field
					selectField();
					break;
			case 3:
					//	Start Match
					startMatch();
					break;
			// the default case is '-1' if nothing has been selected
			default:
					break;
			}
	}
    
	 // We don't need the watchdog timer, but I don't think it can hurt to feed it anyway.
	 // The watchdog was disabled in the initialization code.
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}

/***********************************************************************                       
; HSYNC_XIRQ interrupt service routine: HSYNC_XISR
;
; Make sure you add it to the interrupts vector table (HSYNC_XISR) 
; under: Project Settings/Linker Files/Project.PRM 		  			 		  		
;***********************************************************************/
interrupt 5 void HSYNC_XISR( void)
{
 /* movb #$80,PTT -> red
    movb #$40,PTT -> green
    movb #$20,PTT -> blue
    movb #$A0,PTT -> yellow
    movb #$C0,PTT -> pink
    movb #$60,PTT -> teal
    movb #$E0,PTT -> white
  */

 //enable timer irq
//TIE = 0x80;

 hCnt++; 

  // We need the difference between hCnt lower limit
  // and hCnt upper limit plus 1 to be 480. This is
  // becuase we need to display 480 horizontal lines.
  // If this is not so, then the display will scroll.
if(hCnt > 39 & hCnt < 520){
 //first 80 lines of black
 asm{
 ldx screen_itterator
 
 nop
 nop
 nop
 nop
 nop
 nop
 nop
  nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
  nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
  nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
  nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 
 //colors on screen
 
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 
 
 //last lines of black
 movb #$00,PTT
  
 }//end of asm
 

 
 line_hold_count++;
 if (line_hold_count >= 10)
 {
 	line_hold_count = 0;
 	screen_itterator += 24;
 }
 if(screen_itterator >= screen+1152)
 {
   screen_itterator = screen;
 }
 
 }//end of if
 
 //diasble timer irq 		
 //TIE = 0x00;
 
}//end of xirq

/***********************************************************************                       
; VSYNC interrupt service routine: VSYNC_ISR
;
; Make sure you add it to the interrupts vector table (VECTOR 7 RTI_ISR) 
; under: Project Settings/Linker Files/Project.PRM 		  			 		  		
;***********************************************************************/
interrupt 6 void VSYNC_ISR( void)
{
    vSyncFlag = 1;
  	hCnt = 0;
}


/***********************************************************************                       
; RTI interrupt service routine: RTI_ISR
;
; Make sure you add it to the interrupts vector table (VECTOR 7 RTI_ISR) 
; under: Project Settings/Linker Files/Project.PRM 		  			 		  		
;***********************************************************************/
interrupt 7 void RTI_ISR(void)
{
  	// set CRGFLG bit 
  	CRGFLG = CRGFLG | 0x80; 
// No need to add anything in the .PRM file, the interrupt number is included above
}

/***********************************************************************                       
;  TIM interrupt service routine  (If needed!)
;
; Make sure you add it to the interrupts vector table (VECTOR 15 TIM_ISR) 
; under: Project Settings/Linker Files/Project.PRM 					 		  			 		  		
;***********************************************************************/
interrupt 15 void TIM_ISR(void)
{
  // set TFLG1 bit 
 	TFLG1 = TFLG1 | 0x80; 
// No need to add anything in the .PRM file, the interrupt number is included above
}


/*
;***********************************************************************
; Name:         displaySplash
; Description:  Displays Splash screen for the game and gives the user
;								time to see it.
;***********************************************************************/
void displaySplash(void)
{
    int r,l;

    // copy the splash screen to the screen
    // note that the screen now needs to be 
    // output to the monitor using the real
    // time interrupt service routine (ISR)
    for (r = 0; r < SCREENH; r++)
    {
        for (l = 0; l < SCREENW/2; l++)
        {
            screen[r*(SCREENW/2) + l] = image_splash[r][l];
        }
    }
}

/*
;***********************************************************************
; Name:         displayMenu
; Description:  Displays Menu for user and their current selection.
;								There are 3 menu selections that need to be drawn.
;
;***********************************************************************/
void displayMenu(char selection)
{
}

/*
;***********************************************************************
; Name:         checkMenuInputs
; Description:  Check if the Joysticks have transitioned from below
;				the specified threshold to above it for both the up
;				and down directions. Essentially, we want to check
;				a transition from the netural area in the middle to
;			    the top or bottom (Up/Down). Note that we pass by
;				value to this function instead of using the global
;				variable because it could change in the middle of
;				this function call.
;
;		D		0v -> (2.5v - JOYTHRESH)
;		M		(2.5v - JOYTHRESH) -> (2.5 + JOYTHRESH)
;		U		(2.5v + JOYTHRESH) -> 5v
;
;***********************************************************************/
void checkMenuInputs(unsigned char joyin)
{
		// use a static variable so we can reuse the value when we return
		// to this function. This was used as apposed to a global variable
		// because we don't want anyone else modifying this value.
		static char joyvertprev = 25;
		static char prevleft = 0;
		// Check pushing joystick up
		if ( joyin > THRESHUP )
		{
				if ( joyvertprev < THRESHUP )
				{
						selection++;
				}
				// don't allow the selection to overflow
				if (selection > 3)
				{
						selection = 0;
				}
		}

		// Check pushing joystick down
		if ( joyin < THRESHDO )
		{
				if ( joyvertprev > THRESHDO )
				{
						selection++;
				}
				// don't allow selection to underflow
				if (selection < 0)
				{
						selection = 3;
				}
		}

		// Check for button push (check if selection confirmed)
		if ( (PTAD & LEFTPB) == 0 )
		{
				if (prevleft == 1)
				{
						select = 1;
				}
				prevleft = 0;
		}
		else if ( (PTAD & LEFTPB) == LEFTPB)
		{
				prevleft = 1;
		}

		// update previous value
		joyvertprev = joyin;
}

void selectCharacter(void)
{
}
void selectField(void)
{
}
void startMatch(void)
{
}

#if USESCIDEBUGGING

/***********************************************************************
; Character I/O Library Routines for 9S12C32
;***********************************************************************
; Name:         inchar
; Description:  inputs ASCII character from SCI serial port and returns it
;***********************************************************************/
char  inchar(void) {
  /* receives character from the terminal channel */
        while (!(SCISR1 & 0x20)); /* wait for input */
    return SCIDRL;
 
}

/***********************************************************************
; Name:         outchar
; Description:  outputs ASCII character passed in outchar()
;                  to the SCI serial port
;***********************************************************************/
void outchar(char ch) {
  /* sends a character to the terminal channel */
    while (!(SCISR1 & 0x80));  /* wait for output buffer empty */
    SCIDRL = ch;
}
#endif


/***********************************************************************
; ECE 362 - Mini-Project ASM Source File - Spring 2012                         
;***********************************************************************/
