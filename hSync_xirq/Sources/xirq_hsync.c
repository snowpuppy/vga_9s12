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
   DDRT = 0x07; //port 0 is xirq input
   PTT = 0x00;	      
   
   
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
  	// set CRGFLG bit 
  //	CRGFLG = CRGFLG | 0x80;
  
  hCnt++;
  
  if((hCnt > 50)  & (hCnt <300)){
  	 asm {   

      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      

     }
  
  	bset(PTT,0x01);
  	  asm {   // 30 cycles

      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      

     }
    PTT = 0x00;   
  	}

  
   
  		

}


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