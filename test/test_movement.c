#include <stdio.h>
#include "../src/include/character.h"
#include "../src/include/kirby.h"
#include "../src/include/splash.h"

// define screen resolution
#define SCREENW 48
#define SCREENH 48

// FUNCTIONS
void display_character(struct character *self);
void print_screen(void);
void initialize_screen(void);

// GLOBAL SCREEN BUFFER (2304 pixels)
unsigned char screen[1152];

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
		4, // framew
		4, // frameh
};

int main(void)
{
		// initialize the screen.
		initialize_screen();
		//print_screen();
		display_character(&player0);
		//print_screen();
		return 0;
}


void display_character(struct character *self)
{
		unsigned char r,l;
		unsigned int location = 0;
		unsigned char odd = 0; // checks start on odd pixel.
		unsigned char temp1 = 0,temp2 = 0, temp3 = 0;
		unsigned char digit0, digit1, digit2, digit3;

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
				/*
				    digit0 = location / 1000;
				    digit3 = location - digit0*1000;
				    digit1 = digit3 / 100;
				    digit3 = digit3 - digit1*100;
				    digit2 = digit3 / 10;
				    digit3 = digit3 - digit2*10;
				    outchar(digit0 + '0');
				    outchar(digit1 + '0');
				    outchar(digit2 + '0');
				    outchar(digit3 + '0');
				    outchar('\n');
				    outchar('\r');
				  */  
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
								temp3 = self->frame[(self->framew/2)*r + l];
								printf("location: %d is %x\n", (self->framew/2)*r + l, temp3);
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
								temp1 = self->frame[(self->framew/2)*r + l];
								screen[location] = temp1;
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
						screen[r*SCREENW/2 + l] = 0xf0;
				}
		}
}
