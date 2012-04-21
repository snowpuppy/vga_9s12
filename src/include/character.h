/*
	Define Character Structure. The character structure needs the following
	parameters:
		1. x, y
		2. horvel, vertvel
		3. horacc, veracc
		4. damage
		5. name
		6. attack
		7. move
		8. frame
		9. currframe
		10. framew
		11. frameh

	Also We define here the various character attack and move functions.
	I will define default ones initially.
*/
#ifndef CHARACTER_H
#define CHARACTER_H

// These are used to set the movement flags. The character will move one
// pixel at a time.
#define MOVEUP 0x01
#define MOVEDO 0x02
#define MOVERI 0x04
#define MOVELE 0x08
#define VELUP  0x10 // update up/down velocity
#define VELRI  0x20 // update right/left velocity

struct character
{
		char x,y;
		int horvel, vervel;
		int horvelcnt, vervelcnt;
		unsigned char movever_v; // flag for moving up or moving down. bset(MOVEUP,moveflag)
		unsigned char movehor_v;
		unsigned char movever_r;
		unsigned char movehor_r;
		int horacc, veracc;
		int horacccnt, veracccnt;
		unsigned char damage;
		char name[3+1];
		void (*attack)(struct character *, char, char); // type of attack and direction of attack
		void (*move)(struct character *); // up/down and left/right atd values
		const unsigned char *frame;
		unsigned char currframe;
		unsigned char numframes;
		unsigned char framew, frameh;
};

// DEFUALT FUNCTIONS
void defaultMove(struct character *self);
void defautAttack(struct character *self, char type, char direction);

/*
;***********************************************************************
; Name:         defaultAttack
; Description:  Implements an attack for the player who calls it.
;								
;***********************************************************************/
void defautAttack(struct character *self, char type, char direction)
{
}

/*
;***********************************************************************
; Name:         defaultMove
; Description:  Implements a move for the player who calls it.
;								
;***********************************************************************/
void defaultMove(struct character *self)
{
    
		//if ( (self->moveflag & MOVEUP == MOVEUP) && (self->vervel > 0 ) )
		if ( self->movever_r && self->vervel > 0 )
		{
				// check for collisions
				// move up one pixel.
				self->y -= 1;
				//bclr(self->moveflag,MOVEUP);
				self->movever_r = 0;
				display_character(self);
		}
		//else if ( (self->moveflag & MOVEUP == MOVEUP ) && ( self->vervel < 0) )
		else if ( self->movever_r && self->vervel < 0)
		{
				// check for collisions
				// move down one pixel.
				self->y += 1;
				//bclr(self->moveflag,MOVEUP);
				self->movever_r = 0;
				display_character(self);
		}
		//if ( ( (self->moveflag & MOVERI) == MOVERI) && (self->horvel > 0) )
		if ( self->movehor_r && self->horvel > 0 )
		{
				// check for collisions
				// move right one pixel.
				self->x += 1;
				//bclr(self->moveflag,MOVERI);
				self->movehor_r = 0;
				display_character(self);
		}
		//else if ( (self->moveflag & MOVERI == MOVERI ) && ( self->horvel < 0 ) )
		else if (self->movehor_r && self->horvel < 0 )
		{
				// check for collisions
				// move left one pixel.
				self->x -= 1;
				//bclr(self->moveflag,MOVERI);
				self->movehor_r = 0;
				display_character(self);
		}
}

#endif
