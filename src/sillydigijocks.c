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
;  Date: 4/8/2012 	Name: Thor Smith/David Kauer Update: Merged vga assembly 
;		with main C file and displayed splash screen.
;
;  Date: 4/20/2012  Name: Thor Smith   Update: Added movement logic.
;
;
;***********************************************************************/
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>

#include "bset_clr.h"

// Enable/disable debugging with serial port (i.e. inchar/outchar/pmsg)
#define USESCIDEBUGGING 1

// Define threshold voltages (approx 2.5V +- .5V)
#define ZEROTHRESH 0x19
#define BASETHRESH 5
#define THRESHUP (ZEROTHRESH)
#define THRESHDO (-ZEROTHRESH)

// define button layouts/masks
#define LEFTPB 0x20
#define P0BUTTON1 0x20
#define P0BUTTON2 0x10
#define P1BUTTON1 0x02
#define P1BUTTON2 0x01


// define screen resolution
#define SCREENW 48
#define SCREENH 48
#define SCREENSIZE 1152

// Define Timing specifications
#define TIMEFORONESECOND 2*100

// Define gravity constant
#define GRAVITY -50

// All funtions after main should be initialiezed here
char inchar(void);
void outchar(char x);
void displaySplash(void);
void displayMenu(char selection);
void checkMenuInputs(char joyin);
void selectCharacter(void);
void selectField(void);
void startMatch(void);
void display_character(struct character *self);
void writeBackground(const unsigned char *image);
int abs(int value);
char checkCollisions(struct character *self);
char checkCollision(struct character *self, struct platform *platform);
char checkButtons(unsigned char button1, unsigned char button2, unsigned char *button1prev, unsigned char *button2prev);
char debounceJoystick(char joyin, char *joyinprev);
void checkPlayerJump(struct character *self, char joyin, char *joyinprev);
void updateVelAcc(struct character *self, char inhor,char inver);
void clear_character(struct character *self);

// include images. These are in a separate file
// because they're dynamically generated.
#include "ourimages.h"

// include character definitions
#include "character.h"

// include platform structure.
#include "platform.h"


// Variable declarations  
// Count horizontal pulses
unsigned int hCnt = 0;

//Vsync IRQ flag (used once at start to position screen)
unsigned char vSyncFlag = 0;

// count vertical lines across the screen to verify that we have the
// right number of pixel height
unsigned char line_hold_count = 0;
	   			 		  			 		       
// GLOBAL SCREEN BUFFER (2304 pixels)
unsigned char screen[SCREENSIZE];
unsigned char *screen_itterator = screen;

// Define Field that is selected.
const unsigned char *selected_field = image_battlefield1;

// GLOBAL ANALOG INPUTS   --- 0 is for player 0; 1 is for player 1
char joy0hor = 0;
char joy0horprev = 0;
char joy1hor = 0;
char joy1horprev = 0;
char joy0ver = 0;
char joy0verprev = 0;
char joy1ver = 0;
char joy1verprev = 0;

// Menu selection variables
// button select
char select = 0;
// joystic selections
char selection = 1;

// splash screen enable.
unsigned char splash_screen_enable = 0;

// Define characters 1 and 2
struct character player0 = {
		0, // player
		28, // x
		43, // y
		0, // horvel
		0, // vervel
		0, // horvelcnt
		0, // vervelcnt
		0, // movever_v
		0, // movehor_v
		0, // movever_r
		0, // movehor_r
		0, // jumpflag
		0, // horacc
		0, // veracc
		0, // horacccnt
		0, // veracccnt
		0, // damage
		"def", // name
		defautAttack, // attack
		defaultMove, // move
		0, // attacking
		0, // crouching
		image_yoshi, // frame
		0, // currframe
		2, // numframes
		4, // framew
		4, // frameh
};
struct character player1 = {
		1, // player
		28, // x
		28, // y
		0, // horvel
		0, // vervel
		0, // horvelcnt
		0, // vervelcnt
		0, // movever_v
		0, // movehor_v
		0, // movever_r
		0, // movehor_r
		0, // jumpflag
		0, // horacc
		0, // veracc
		0, // horacccnt
		0, // veracccnt
		0, // damage
		"def", // name
		defautAttack, // attack
		defaultMove, // move
		0, // attacking
		0, // crouching
		image_kirby, // frame
		0, // currframe
		2, // numframes
		4, // framew
		4, // frameh
};


// define global platform structure.
struct platform *all_platforms[MAXPLATFORMS] = {NULL};

// define ground as default platform
struct platform ground = {
		0, // x
		48, // y
		48, // w
		1, // h
};

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
        
  // Timer Used To Keep Track of Timing for user Application
  // Such as displaying the splash screen for a decent period
  // of time.
  // enable timer system
  TSCR1 = 0x80;
  // set prescale and enable counter reset
  TSCR2 = 0x04;
  // set channel 0 for output compare
  TIOS = 0x01;
  // set 1ms interrupts (needs to be changed to 1/60s of a second)
  TC0 = 15000;
  TIE = 0x01; 


  ATDCTL2 = 0x80;
  ATDCTL3 = 0x08;
  ATDCTL4 = 0x85;
  // 0011 0011 where 0's represent analog inputs and 1's represent
  // digital inputs. Controller 0 is high byte, Controller 1 is low byte.
  ATDDIEN = 0x33; 

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
	// load default choice for menu
	displayMenu(selection);

	// set ground as default platform
	all_platforms[0] = &ground;

  for(;;) {
   // write code here (Insert Code down here because we need an infinite loop.)

	// Check for Menu Selection
	checkMenuInputs(joy0ver);
	// Use case statement to branch to appropriate selection.
	// Don't branch unless the user has triggered the 'select' button.
	if (select == 1)
	{
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
			select = 0;
			displayMenu(selection);
	}
    //startMatch();
    /*
    player0.x += 1;
    player0.y += 1;
    if (player0.x + player0.framew > 48) 
    {
      player0.x = 0;
    }
    if (player0.y + player0.frameh > 48) 
    {
      player0.y = 0;
    }
    */
    //display_character(&player0);
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
 
// nop
// nop
// nop
// nop
// nop
// nop
// nop
//  nop
// nop
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
 
 ldy #24
loop:
 movb 1,x+,PTT
 nop
 nop
 nop
 nop
// nop
 //nop
 bset PTT,$02
 nop
 nop
 nop
 dbne y,loop
 
 
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

	// wait for sampled inputs (note that they are signed)
	// bit 7: analog horizontal controller 0
	// bit 6: analog vertical   controller 0
	// bit 5: digital button 1  controller 0
	// bit 4: digital button 2  controller 0

	// bit 3: analog horizontal controller 1
	// bit 2: analog vertical   controller 1
	// bit 1: digital button 1  controller 1
	// bit 0: digital button 2  controller 1
	asm {
		movb	#$47, ATDCTL5
await:
        brclr	ATDSTAT0,$80,await
        movb	ATDDR0H, joy0hor
        
        movb	#$46, ATDCTL5
await1:	
        brclr	ATDSTAT0,$80,await1
        movb	ATDDR0H, joy0ver

        movb	#$43, ATDCTL5
await2:	
        brclr	ATDSTAT0,$80,await2
        movb	ATDDR0H, joy1hor

        movb	#$42, ATDCTL5
await3:	
        brclr	ATDSTAT0,$80,await3
        movb	ATDDR0H, joy1ver
	}
	if (selection == 3 && select == 1)
	{
		// set jumping flags.
		if (debounceJoystick(joy0ver, &joy0verprev) == 1)
		{
			player0.jumpflag = 1;
		}
		if (debounceJoystick(joy1ver, &joy1verprev) == 1)
		{
			player1.jumpflag = 1;
		}
	}
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
interrupt 8 void TIM_ISR(void)
{
  // set TFLG1 bit 
 	TFLG1 = TFLG1 | 0x01;
 	
 	// increment the counter for TC0 (it should wrap around and create reliable interrupt rate)
 	TC0 += 15000; 
// No need to add anything in the .PRM file, the interrupt number is included above

		// handle splash screen logic
    if (splash_screen_enable < TIMEFORONESECOND)
    {
        splash_screen_enable++;
    }

	// If game is in progress....
	if (selection == 3 && select == 1)
	{
		// #################################
		// set player0 velocity update flag
		// #################################
		if (player0.horacc != 0)
		{
				player0.horacccnt++;
				if (player0.horacccnt >= abs(player0.horacc) )
				{
						// need to update velocity up/down
						//bset(player0.moveflag, VELRI);
						player0.movehor_v = 1;
						// restart count
						player0.horacccnt = 0;
				}
		}
		if (player0.veracc != 0)
		{
				player0.veracccnt++;
				if (player0.veracccnt >= abs(player0.veracc) )
				{
						// need to update velocity left/right
						//bset(player0.moveflag, VELUP);
						player0.movever_v = 1;
						player0.veracccnt = 0;
				}
		}
		// set player0 movement flag
		if (player0.horvel != 0) 
		{
				player0.horvelcnt++;
				if (player0.horvelcnt >= abs(player0.horvel) )
				{
						//bset(player0.moveflag, MOVERI);
						player0.movehor_r = 1;
						player0.horvelcnt = 0;
				}
		}
		if (player0.vervel != 0)
		{
				player0.vervelcnt++;
				if (player0.vervelcnt >= abs(player0.vervel) )
				{
						//bset(player0.moveflag, MOVEUP);
						player0.movever_r = 1;
						player0.vervelcnt = 0;
				}
		}

		// ############################
		// player 1 acceleration values
		// ############################
		if (player1.horacc != 0)
		{
				player1.horacccnt++;
				if (player1.horacccnt >= abs(player1.horacc) )
				{
						// need to update velocity up/down
						//bset(player1.moveflag, VELRI);
						player1.movehor_v = 1;
						// restart count
						player1.horacccnt = 0;
				}
		}
		if (player1.veracc != 0)
		{
				player1.veracccnt++;
				if (player1.veracccnt >= abs(player1.veracc) )
				{
						// need to update velocity left/right
						//bset(player1.moveflag, VELUP);
						player1.movever_v = 1;
						player1.veracccnt = 0;
				}
		}

		// set player1 movement flags
		if (player1.horvel != 0)
		{
				player1.horvelcnt++;
				if (player1.horvelcnt >= abs(player1.horvel) )
				{
						//bset(player1.moveflag, MOVEUP);
						player1.movehor_r = 1;
						player1.horvelcnt = 0;
				}
		}
		if (player1.vervel != 0)
		{
				player1.vervelcnt++;
				if (player1.vervelcnt >= abs(player1.vervel) )
				{
						//bset(player1.moveflag, MOVERI);
						player1.movever_r = 1;
						player1.vervelcnt = 0;
				}
		}
		
		//player0.jumpflag = debounceJoystick(joy0hor, &joy0horprev);
		//player1.jumpflag = debounceJoystick(joy1hor, &joy1horprev);
		
	}
	
	// END OF TIM ISR
}


/*
;***********************************************************************
; Name:         displaySplash
; Description:  Displays Splash screen for the game and gives the user
;								time to see it.
;***********************************************************************/
void displaySplash(void)
{
    //int r,l;

    // copy the splash screen to the screen
    // note that the screen now needs to be 
    // output to the monitor using the non-maskable
    // interrupt service routine (IRQ)
    /*
    for (r = 0; r < SCREENH; r++)
    {
        for (l = 0; l < SCREENW/2; l++)
        {
            screen[r*(SCREENW/2) + l] = image_splash[r][l];
        }
    }
    */
    writeBackground(image_splash);

    while (splash_screen_enable < TIMEFORONESECOND);
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
	//int r,l;
	//unsigned char **menu_image = NULL;
	// pick image to draw
	switch(selection)
	{
		case 1:
            writeBackground(image_menu_select1);
			break;
		case 2:
            writeBackground(image_menu_select2);
			break;
		case 3:
            writeBackground(image_menu_select3);
			break;
		default:
			break;
	}
    
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
;		D		0v -> (2.5v - ZEROTHRESH)
;		M		(2.5v - ZEROTHRESH) -> (2.5 + ZEROTHRESH)
;		U		(2.5v + ZEROTHRESH) -> 5v
;
;***********************************************************************/
void checkMenuInputs(char joyin)
{
		// use a static variable so we can reuse the value when we return
		// to this function. This was used as apposed to a global variable
		// because we don't want anyone else modifying this value.
		static char prevleft = 0;
		static char joyvertprev = 0;
		// Check pushing joystick up
		if ( joyin < THRESHUP )
		{
				if ( joyvertprev > THRESHUP )
				{
						selection--;
						displayMenu(selection);
				}
				// don't allow the selection to overflow
				if (selection < 0)
				{
						selection = 0;
				}
		}

		// Check pushing joystick down
		if ( joyin > THRESHDO )
		{
				if ( joyvertprev < THRESHDO )
				{
						selection++;
						displayMenu(selection);
				}
				// don't allow selection to underflow
				if (selection > 3)
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
	writeBackground(image_character_select);
}
void selectField(void)
{
	// set the appropriate ledges
	all_platforms[1] = &batt1_plat1;
	all_platforms[2] = &batt1_plat2;
	all_platforms[3] = &batt1_plat3;
	all_platforms[4] = &batt1_plat4;
	all_platforms[5] = &batt1_plat5;
	all_platforms[6] = &batt1_plat6;
	selected_field = image_battlefield1;
}

void startMatch(void)
{
	char quit = 0;
	unsigned char button1player0prev = 0;
	unsigned char button2player0prev = 0;
	unsigned char button1player1prev = 0;
	unsigned char button2player1prev = 0;

	writeBackground(selected_field);
	display_character(&player0);

	while (!quit)
	{
		if (hCnt > 520 || hCnt < 32)
		{
			// update horizontal and vertical velocity for player 0
			updateVelAcc(&player0,joy0hor,joy0ver);
			// check for jumping and button presses.
			checkPlayerJump(&player0, joy0ver, &joy0verprev);
			//joy0verprev = joy0ver; // this may need to be updated in the function above.
			player0.attacking = checkButtons(P0BUTTON1, P0BUTTON2, &button1player0prev, &button2player0prev);
			// TEMPORARY EXIT CONDITION
			if (player0.attacking == 2) // pushbutton 2 pressed
			{
					quit = 1;
			}

			// move player (it can be any function)
			player0.move(&player0);
			//  display the character at his location
			//display_character(&player0);
		}
	}
}

void clear_character(struct character *self)
{
	unsigned char r,l;
	unsigned int location = 0;
	
	location = self->y*(SCREENW/2) + self->x/2;

	for (r = 0; r < self->frameh; r++)
	{
		for (l = 0; l < self->framew/2 + 1; l++)
		{
			if ( location >= 0 && location < SCREENSIZE )
			{
				screen[location] = selected_field[location];
			}
			// increment our location by one column
			location++;
		}
		// increment our location by one row
		location += SCREENW/2 - self->framew/2 - 1;
	}
}

void display_character(struct character *self)
{
	unsigned char r,l;
	unsigned int location = 0;
	unsigned char odd = 0; // checks start on odd pixel.
	unsigned char temp1 = 0,temp2 = 0, temp3 = 0;

    // 0 or 1 value. If 1, then we're starting on an odd pixel
    // modulus logic needs to be kept out of for looping.
    odd = self->x % 2;
    
    // calculate starting location to draw character
    // Note that each pixel is stored in two bytes. Thus the
    // width of our array is SCREENW/2 or 24 in this case.
    // To get "y" lines down, we need to multiply by the width
    // of our array. To get "x" lines over, we need to add that
    // value to the beginning our line. Thus we calculate the
    // position to display the image in our one-dimensional
    // array.
    location = self->y*(SCREENW/2) + self->x/2;

		for (r = 0; r < self->frameh; r++)
		{
				for (l = 0; l < self->framew/2; l++)
				{
				    // do a different process if we start on an odd pixel.
						if (location >= 0 && location < SCREENSIZE)
						{
							if (odd)
							{
								// STORE FIRST PIXEL

								// note that we've already calculated the starting location
								// We are masking off the lower nibble [E0]
								temp1 = screen[location] & 0xe0;
								// we want the pixel at row "r" and column "l"
								// a four by four pixel picture's array will
								// only have 2 columns (2pixels/byte).
								// We are again masking off the lower nibble [E0]
								temp3 = self->frame[(self->framew/2)*self->numframes*r + l + self->framew*self->currframe/2];
								temp2 = temp3 & 0xe0;
								// Shift the upper nibble to the lower nibble [1C]
								temp2 = temp2 / 0x08;
								// add the bytes together [FC]
								temp1 = temp1 + temp2;
								// store the result in the screen.
								screen[location] = temp1;

								// STORE SECOND PIXEL

								// get our current location plus one
								// We are masking off the upper nibble this time [1C]
								temp1 = screen[location+1] & 0x1c;
								// Again we mask off the upper nibble, this time [1C]
								// for our picture's byte
								temp2 = temp3 & 0x1c;
								// shift our low nibble to our high nibble. [E0]
								temp2 *= 0x08;
								// add the bytes together [FC]
								temp1 = temp1 + temp2;
								screen[location+1] = temp1;
							}
							else
							{
								// We start on an even byte, so just copy our
								// picture over two pixels at a time.
								temp1 = self->frame[(self->framew/2)*self->numframes*r + l + self->framew*self->currframe/2];
								screen[location] = temp1;
							}
						}
						// increment our location by one column
						location++;
				}
				// increment our location by one row
				location += SCREENW/2 - self->framew/2;
		}
}

/***********************************************************************
; Name:         checkPlayerJump
; Description:  This function checks to see if the player has chosen to
;								jump by pushing up on the joystick. It will also test
;								to see if the player is crouching by pressing down on
;								the joystick. (down still being implemented.)
;***********************************************************************/
void checkPlayerJump(struct character *self, char joyin, char *joyinprev)
{
		char jump = 0;

		//jump = debounceJoystick(joyin, joyinprev);
		jump = self->jumpflag;
		if (jump == 1)
		{
				if (self->vervel == 0)
				{
						// give initial velocity and constant acceleration.
						// values may need to be adjusted.
						self->veracc = -20;
						self->vervel = 4;
				}
				self->jumpflag = 0;
		}
		else if (jump == -1)
		{
				// implement crouching logic.
		}
}

/***********************************************************************
; Name:         debounceJoystick
; Description:  This function debounces vertical and horizontal joystick
;								inputs. It says THRESHUP and THRESHDO, but the thresholds
;								are the same for vertical and horizontal. A 1 is returned
;								if the joystick was bounced up or right. A -1 is returned
;								if the joystick was bounced down or left. (depending
;								on how the function is called)
;***********************************************************************/
char debounceJoystick(char joyin, char *joyinprev)
{
		char ret = 0;
		// check for moving joystick up
		// transition from up to down
		if ( joyin < THRESHUP)
		{
				if (*joyinprev > THRESHUP)
				{
						ret = 1;
				}
		}
		// check for moving joystick down
		// transision from down to up
		else if (joyin > THRESHDO )
		{
				if (*joyinprev < THRESHDO )
				{
						ret = -1;
				}
		}
		*joyinprev = joyin;
		return ret;
}

/***********************************************************************
; Name:         checkButtons
; Description:  This function checks the requested buttons and their
;								previous inputs to see if either button1 or button2
;								was pressed. An appropriate flag is returned to indicate
;								the button that was pressed.
;
;			0 -- No button pressed
;			1 -- First button pressed
;			2 -- Second button pressed
;***********************************************************************/
char checkButtons(unsigned char button1, unsigned char button2, unsigned char *button1prev, unsigned char *button2prev)
{
		char ret = 0; // 0 means no button pressed.

		// DEBOUNCE BUTTON 1
		if ( (PTAD & button1) == 0 )
		{
				if (*button1prev == 1)
				{
						ret = 1;
				}
		}
		else if ( (PTAD & button1) == button1)
		{
				*button1prev = 1;
		}

		// DEBOUNCE BUTTON 2
		if ( (PTAD & button2) == 0)
		{
				if ( *button2prev == 1)
				{
						ret = 2;
				}
				*button2prev = 0;
		}
		else if ( (PTAD & button2) == button2)
		{
				*button2prev = 1;
		}
		return ret;
}

/***********************************************************************
; Name:         checkCollision
; Description:  This function detects a collision between a character
;								and a platform object. A 0 is returned if a collision is
;								detected. A 1 is returned if no collision detected.
;***********************************************************************/
char checkCollision(struct character *self, struct platform *platform)
{
		char ret = 0; // 0 means no collision
		if ( (self->x + self->framew) > platform->x )
		{
				if ( self->x < ( platform->x + platform->w ) )
				{
						if ( (self->y + self->frameh) > platform->y )
						{
								if ( self->y < (platform->y + platform->h) )
								{
										ret = 1;
								}
						}
				}
		}
		return ret;
}

/***********************************************************************
; Name:         checkCollisions
; Description:  This function checks for collisions between a character
;								and all platforms that are currently in the list. The
;								list of platforms is set in the selectField() function.
;								Note that the ground is enabled by default.
;***********************************************************************/
char checkCollisions(struct character *self)
{
		char i;
		char ret = 0; // 0 means no collision detected.
		for (i = 0; i < MAXPLATFORMS; i++)
		{
				// if we've run out of platforms to test against, then return.
				// note that the platforms variable is an array of pointers to
				// structures. The end may be signified by a NULL character.
				if ( all_platforms[i] == NULL)
				{
						break;
				}
				ret = checkCollision(self, all_platforms[i]);
				// if a collison was detected. Don't bother checking for other collisions.
				// We need to undo our move and set flags appropriately.
				if (ret)
				{
						break;
				}
		}
		return ret;
}

/***********************************************************************
; Name:         updateVelAcc
; Description:  This function updates a players horizontal and vertical
;								acceleration and velocity.
;***********************************************************************/
void updateVelAcc(struct character *self, char inhor, char inver)
{
		// update acceleration 
		// 50 means 500ms and 150 means 1500ms. These correspond to 
		// 2p/s and 1.5p/s correspondingly

		// UPDATE HORIZONTAL ACCELERATION
		if (-ZEROTHRESH < inhor && inhor < ZEROTHRESH)
		{
				self->horacc = 0;
				self->horvel = 0;
		}
		else if (inhor > 0)
		{
				self->horacc = 50 + (75 - (inhor *75)/(128 - ZEROTHRESH) );
		}
		else
		{
				self->horacc = -50 + (-75 - (inhor *75)/(128 - ZEROTHRESH) );
		}
		/*
		// UPDATE VERTICAL ACCELERATION
		if (-ZEROTHRESH < inver && inver < ZEROTHRESH)
		{
				self->veracc = 0;
				self->vervel = 0;
		}
		else if (inver > 0)
		{
				self->veracc = 50 + (75 - (inver *75)/(128 - ZEROTHRESH) );
		}
		else
		{
				self->veracc = -50 + (-75 - (inver *75)/(128 - ZEROTHRESH) );
		}
		*/
		// UPDATE VERTICAL VELOCITY PLAYER0
		//if (self->moveflag & VELUP == VELUP)
		if (self->movever_v)
		{
				if (self->vervel != 0)
				{
						self->vervel = (self->vervel*self->veracc)/ (self->vervel + self->veracc);
				}
				else
				{
						self->vervel = self->veracc;
				}
				//bclr(self->moveflag, VELUP);
				self->movever_v = 0;
		}
		// UPDATE HORIZONTAL VELOCITY PLAYER0
		//if (self->moveflag & VELRI == VELRI)
		if (self->movehor_v)
		{
				if (self->horvel != 0)
				{
						self->horvel = (self->horvel*self->horacc)/ (self->horvel + self->horacc);
				}
				else
				{
						self->horvel = self->horacc;
				}
				//bclr(self->moveflag, VELRI);
				self->movehor_v = 0;
		}
}

/***********************************************************************
; Name:         writeBackground
; Description:  writes a large full sized 48x48 image to the screen
;***********************************************************************/
void writeBackground(const unsigned char *image)
{
    int loc;
    for (loc = 0; loc < SCREENSIZE; loc++)
    {
        screen[loc] = image[loc];
    }
}


/***********************************************************************
; Name:         abs
; Description:  find absolute value of an integer
;***********************************************************************/
int abs(int value)
{
	if (value < 0)
	{
		return -value;
	}
	else
	{
		return value;
	}
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
