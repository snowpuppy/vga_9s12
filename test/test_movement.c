#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "../src/include/bset_clr.h"
#include "../src/include/character.h"
#include "../src/include/kirby.h"
#include "../src/include/splash.h"

// define screen resolution
#define SCREENW 48
#define SCREENH 48
#define SCREENSIZE 1152

// define input settings
#define ZEROTHRESH 0x19

// FUNCTIONS
void display_character(struct character *self);
void print_screen(void);
void initialize_screen(void);
void *TIM_ISR(void *);
void startMatch(void);

// GLOBAL SCREEN BUFFER (2304 pixels)
unsigned char screen[1152];

unsigned char quit_timer = 0;

// GLOBAL ANALOG INPUTS   --- 0 is for player 0; 1 is for player 1
char joy0hor = 122;
char joy0ver = 122;

struct character player0 =
{
		27, // x
		28, // y
		500, // horvel
		0, // vervel
		0, // horvelcnt
		0, // vervelcnt
		0, // moveflag
		500, // horacc
		0, // veracc
		0, // horacccnt
		0, // veracccnt
		0, // damage
		"def", // name
		defautAttack, // attack
		defaultMove, // move
		image_kirby, // frame
		0, // currframe
		2, // numframes
		4, // framew
		4, // frameh
};

int main(void)
{
		pthread_t timer_isr;
		pthread_attr_t attr;
		void *status;
		int rc;

		// create/initialize the attribute.
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		// initialize the timer interrupt
		rc = pthread_create(&timer_isr, &attr, TIM_ISR, NULL);
		if (rc) {
				printf("Error with pthreads.\n");
				exit(1);
		}
		// free attribute
		pthread_attr_destroy(&attr);

		/////////////////////////////////
		// TESTING CODE!!!!
		/////////////////////////////////

		// initialize the screen.
		initialize_screen();
		//print_screen();
		display_character(&player0);
		//print_screen();

		startMatch();

		// end the TIM_ISR
		//quit_timer = 1;
		rc = pthread_join(timer_isr, &status);
		return 0;
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
						if (location < SCREENSIZE)
						{
								// do a different process if we start on an odd pixel.
								if (odd)
								{
										// STORE FIRST PIXEL

										// note that we've already calculated the starting location
										// We are masking off the lower nibble [F0]
										temp1 = screen[location] & 0xf0;
										// we want the pixel at row "r" and column "l"
										// a four by four pixel picture's array will
										// only have 2 columns (2pixels/byte).
										// We are again masking off the lower nibble [F0]
										temp3 = self->frame[(self->framew/2)*self->numframes*r + l];
										//printf("location: %d is %x\n", (self->framew/2)*self->numframes*r + l, temp3);
										temp2 = temp3 & 0xf0;
										// Shift the upper nibble to the lower nibble [0F]
										temp2 = temp2 / 0x10;
										// add the bytes together [FF]
										temp1 = temp1 + temp2;
										// store the result in the screen.
										screen[location] = temp1;

										// STORE SECOND PIXEL

										// get our current location plus one
										// We are masking off the upper nibble this time [0F]
										temp1 = screen[location+1] & 0x0f;
										// Again we mask off the upper nibble, this time [0F]
										// for our picture's byte
										temp2 = temp3 & 0x0f;
										// shift our low nibble to our high nibble. [F0]
										temp2 *= 0x10;
										// add the bytes together [FF]
										temp1 = temp1 + temp2;
										screen[location+1] = temp1;
								}
								else
								{
										// We start on an even byte, so just copy our
										// picture over two pixels at a time.
										temp1 = self->frame[(self->framew/2)*self->numframes*r + l];
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


void print_screen(void)
{
		int r,l;

		for (r = 0; r < SCREENH; r++)
		{
				for (l = 0; l < SCREENW/2; l++)
				{
						printf(" %2x", screen[r*SCREENW/2 + l] );
				}
				printf("\n");
		}
		printf("\n\n");
}

void initialize_screen(void)
{
		int r,l;

		for (r = 0; r < SCREENH; r++)
		{
				for (l = 0; l < SCREENW/2; l++)
				{
						screen[r*SCREENW/2 + l] = 0x00;
				}
		}
}

void *TIM_ISR(void *input)
{
		struct timespec now_time, then_time;
		time_t now, then;
		gettimeofday(&then_time, NULL);
		then = then_time.tv_sec * 1000 + then_time.tv_nsec / 1000;
		printf("TIM_ISR started.\n");
		while(!quit_timer)
		{
				// wait and do nothing till we've updated a tenth of a millisecond
				do
				{
						gettimeofday(&now_time,NULL);
						now = now_time.tv_sec * 1000 + now_time.tv_nsec / 1000;
				}while (now < then + 10);
				then = now;

				// do any timer processing tasks.
				//printf("Time is %ld\n", now );
				// #################################
				// set player0 velocity update flag
				// #################################
				if (player0.horacc != 0)
				{
						player0.horacccnt++;
						if (player0.horacccnt >= player0.horacc)
						{
								// need to update velocity up/down
								bset(player0.moveflag, VELRI);
								// restart count
								player0.horacccnt = 0;
						}
				}
				if (player0.veracc != 0)
				{
						player0.veracccnt++;
						if (player0.veracccnt >= player0.veracc)
						{
								// need to update velocity left/right
								bset(player0.moveflag, VELUP);
								player0.veracccnt = 0;
						}
				}
				// set player0 movement flag
				if (player0.horvel != 0) 
				{
						player0.horvelcnt++;
						if (player0.horvelcnt >= player0.horvel)
						{
								bset(player0.moveflag, MOVEUP);
								player0.horvelcnt = 0;
						}
				}
				if (player0.vervel != 0)
				{
						player0.vervelcnt++;
						if (player0.vervelcnt >= player0.vervel)
						{
								bset(player0.moveflag, MOVERI);
								player0.vervelcnt = 0;
						}
				}

				// ############################
				// player 1 acceleration values
				// ############################
				/*
					 if (player1.horacc != 0)
					 {
					 player1.horacccnt++;
					 if (player1.horacccnt >= player1.horacc)
					 {
				// need to update velocity up/down
				bset(player1.moveflag, VELRI);
				// restart count
				player1.horacccnt = 0;
				}
				}
				if (player1.veracc != 0)
				{
				player1.veracccnt++;
				if (player1.veracccnt >= player1.veracc)
				{
				// need to update velocity left/right
				bset(player1.moveflag, VELUP);
				player1.veracccnt = 0;
				}
				}

				// set player1 movement flags
				if (player1.horvel != 0)
				{
				player1.horvelcnt++;
				if (player1.horvelcnt >= player1.horvel)
				{
				bset(player1.moveflag, MOVEUP);
				player1.horvelcnt = 0;
				}
				}
				if (player1.vervel != 0)
				{
				player1.vervelcnt++;
				if (player1.vervelcnt >= player1.vervel)
				{
				bset(player1.moveflag, MOVERI);
				player1.vervelcnt = 0;
				}
				}
				 */
		}
		printf("TIM_ISR closing.\n");
}

void startMatch(void)
{
		char quit = 0;

		while (!quit)
		{
				// update acceleration for each player
				// 50 means 500ms and 150 means 1500ms. These correspond to 
				// 2p/s and 1.5p/s correspondingly
				if (-ZEROTHRESH < joy0hor && joy0hor < ZEROTHRESH)
				{
						player0.horacc = 0;
				}
				else if (joy0hor > 0)
				{
						player0.horacc = 50 + (150 - joy0hor* (150/(128 - ZEROTHRESH) ) );
				}
				else
				{
						player0.horacc = -50 + (-150 - joy0hor*(150/(128 - ZEROTHRESH) ) );
				}
				if (-ZEROTHRESH < joy0ver && joy0ver < ZEROTHRESH)
				{
						player0.veracc = 0;
				}
				else if (joy0ver > 0)
				{
						player0.veracc = 50 + (150 - joy0ver* (150/(128 - ZEROTHRESH) ) );
				}
				else
				{
						player0.veracc = -50 + (-150 - joy0ver*(150/(128 - ZEROTHRESH) ) );
				}
				// update velociety for each player
				//printf("moveflag: %x\n", player0.moveflag);
				if (player0.moveflag & VELUP == VELUP)
				{
						if (player0.vervel != 0)
						{
								player0.vervel = (player0.vervel*player0.veracc)/ (player0.vervel + player0.veracc);
						}
						else
						{
								player0.vervel = player0.veracc;
						}
						bclr(player0.moveflag, VELUP);
				}
				if (player0.moveflag & VELRI == VELRI)
				{
						if (player0.horvel != 0)
						{
								player0.horvel = (player0.horvel*player0.horacc)/ (player0.horvel + player0.horacc);
						}
						else
						{
								player0.horvel = player0.horacc;
						}
						bclr(player0.moveflag, VELRI);
				}
				// move player (it can be any function, I may move the above logic
				// into this function to avoid typing it for both players.
				player0.move(&player0);
				//  display the character at his location
				display_character(&player0);
				//printf("PlayerX: %d PlayerY: %d\n", player0.x, player0.y);
				//print_screen();
				//system("clear");
		}
}
