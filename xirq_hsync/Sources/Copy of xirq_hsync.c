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
;  Date code started: < ? >
;
;  Update history (add an entry every time a significant change is made):
;
;  Date: < ? >  Name: < ? >   Update: < ? >
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

// All funtions after main should be initialiezed here
char inchar(void);
void outchar(char x);


// Variable declarations  	   			 		  			 		       
int hCnt = 0;


// ASCII character definitions
int CR = 0x0D;//Return   

	 	   		
/***********************************************************************
Initializations
***********************************************************************/
void  initializations(void) {

#define bset(x,y) \
x = x | y

#define bclr(x,y) \
x = x & (0xFF-y)

//; Set the PLL speed (bus clock = 24 MHz)
  CLKSEL = CLKSEL & 0x80; //; disengage PLL from system
  PLLCTL = PLLCTL | 0x40; //; turn on PLL
  SYNR = 0x02;            //; set PLL multiplier
  REFDV = 0;              //; set PLL divider
  while (!(CRGFLG & 0x08)){  }
  CLKSEL = CLKSEL | 0x80; //; engage PLL

// Disable watchdog timer (COPCTL register)
  COPCTL = 0x40   ; //COP off; RTI and COP stopped in BDM-mode

// Initialize asynchronous serial port (SCI) for 9600 baud, no interrupts
  SCIBDH =  0x00; //set baud rate to 9600
  SCIBDL =  0x9C; //24,000,000 / 16 / 156 = 9600 (approx)  dec=26
  SCICR1 =  0x00; //$9C = 156
  SCICR2 =  0x0C; //initialize SCI for program-driven operation
  DDRB   =  0x10; //set PB4 for output mode
  PORTB  =  0x10; //assert DTR pin on COM port
            
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
   
/*; Initialize TIM Ch 1 (TC1) for periodic interrupts every 10.0 ms
;    Enable timer subsystem                         
;    Set channel 7 for output compare
;    Set appropriate pre-scale factor and enable counter reset after OC7
;    Set up channel 7 to generate 10 ms interrupt rate
;    Initially disable TIM Ch 7 interrupts	 	   			 		  			 		  		
*/	 	   			 		  			 		  		
//;  < add TIM initialization code here >
/*  TSCR1 = 0x80; 
  TIOS = 0x80;
  TSCR2 = 0x0C;        
  TC1 = 0x3A98;
  TIE = 0x00;   
  */ 
   
   asm andcc #$BF  //enables external xirq
	      
}

	 		  			 		  		
/***********************************************************************
Main
***********************************************************************/
void main(void) {
  DisableInterrupts;
	initializations(); 		  			 		  		
	EnableInterrupts;

//////////////////////////////////////////////////////////////
//;  START OF CODE FOR Spring 2012 MINI-PROJECT
//////////////////////////////////////////////////////////////
  //asm ldx //...

  while(1){
  	
  	
  	
  }
  
  
  

  for(;;) {
   // No need to write any code here
    
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}



/***********************************************************************                       
; HSYNC_IRQ interrupt service routine: HSYNC_XISR
;
; Make sure you add it to the interrupts vector table (HSYNC_XISR) 
; under: Project Settings/Linker Files/Project.PRM 		  			 		  		
;***********************************************************************/
void interrupt HSYNC_XISR( void)
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

if(hCnt > 28 & hCnt < 515){
 //first 80 lines of black
 asm{
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
 nop
 nop
 nop
 
 //colors on screen
 
 movb #$80,PTT
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
 movb #$40,PTT
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
 movb #$20,PTT
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
 movb #$A0,PTT
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
 movb #$C0,PTT
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
 movb #$60,PTT
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
 movb #$E0,PTT
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
 movb #$80,PTT
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
 movb #$40,PTT
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
 movb #$20,PTT
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
 movb #$A0,PTT
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
 movb #$C0,PTT
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
 movb #$60,PTT
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
 movb #$E0,PTT
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
 movb #$80,PTT
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
 movb #$40,PTT
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
 movb #$20,PTT
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
 movb #$A0,PTT
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
 movb #$C0,PTT
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
 movb #$60,PTT
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
 movb #$E0,PTT
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
 movb #$80,PTT
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
 movb #$20,PTT
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
 movb #$A0,PTT
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
 
 
 //last lines of black
 movb #$00,PTT
  
 }//end of asm
 
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
void interrupt VSYNC_ISR( void)
{
  	// set CRGFLG bit 
  	//CRGFLG = CRGFLG | 0x80;

  	hCnt = 0;
  	 

}

/***********************************************************************                       
; RTI interrupt service routine: RTI_ISR
;
; Make sure you add it to the interrupts vector table (VECTOR 7 RTI_ISR) 
; under: Project Settings/Linker Files/Project.PRM 		  			 		  		
;***********************************************************************/
void interrupt RTI_ISR( void)
{
  	// set CRGFLG bit 
  	//CRGFLG = CRGFLG | 0x80;
  	 

}

/***********************************************************************                       
;  TIM interrupt service routine  (If needed!)
;
; Make sure you add it to the interrupts vector table (VECTOR 15 TIM_ISR) 
; under: Project Settings/Linker Files/Project.PRM 					 		  			 		  		
;***********************************************************************/
void interrupt TIM_ISR( void)
{
  // set TFLG1 bit 
 	TFLG1 = TFLG1 | 0x80; 

}



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




/***********************************************************************
; ECE 362 - Mini-Project ASM Source File - Spring 2012                         
;***********************************************************************/