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
#define ATTACKLEFT 1
#define ATTACKRIGHT 2
#define ATTACKUP 3
#define ATTACKDOWN 4
#define DEFAULTATTACKLENGTH 50

struct character
{
		char player; // indicates either player0 or player1. Used when figuring out who attacked who.
		char x,y;
		char defaultx, defaulty;
		int horvel, vervel;
		int horvelcnt, vervelcnt;
		unsigned char movever_v; // flag for moving up or moving down. bset(MOVEUP,moveflag)
		unsigned char movehor_v;
		unsigned char movever_r;
		unsigned char movehor_r;
		unsigned char jumpflag;
		int horacc, veracc;
		int horacccnt, veracccnt;
		unsigned char damage;
		unsigned char lives;
		char name[3+1];
		void (*attack)(struct character *); // type of attack and direction of attack
		void (*move)(struct character *); // up/down and left/right atd values
		char attacking;
		unsigned int attacklength;
		unsigned int attackcount;
		char attackdirection;
		char hit;
		char crouching;
		const unsigned char *frame;
		unsigned char currframe;
		unsigned char numframes;
		unsigned char framew, frameh;
};

// DEFUALT FUNCTIONS
void defaultMove(struct character *self);
void defaultAttack(struct character *self);
void defaultAttackImpl(struct character *self, char attackdir);

// Define characters 1 and 2
struct character player0 = {
		0, // player
		28, // x
		43, // y
		28, // defaultx
		43, // defaulty
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
		5, // lives
		"def", // name
		defaultAttack, // attack
		defaultMove, // move
		0, // attacking
		DEFAULTATTACKLENGTH, // attacklength
		0, // attackcount
		0, // attackdirection
		0, // hit
		0, // crouching
		image_yoshi, // frame
		0, // currframe
		2, // numframes
		4, // framew
		4, // frameh
};

struct character player1 = {
		1, // player
		15, // x
		43, // y
		15, // defaultx
		43, // defaulty
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
		5, // lives
		"def", // name
		defaultAttack, // attack
		defaultMove, // move
		0, // attacking
		DEFAULTATTACKLENGTH, // attacklength
		0, // attackcount
		0, // attackdirection
		0, // hit
		0, // crouching
		image_kirby, // frame
		0, // currframe
		2, // numframes
		4, // framew
		4, // frameh
};

/*
;***********************************************************************
; Name:         defaultAttack
; Description:  Implements an attack for the player who calls it.
;								
;***********************************************************************/
void defaultAttack(struct character *self)
{
		int coll = 0;
		if (self->attacking == 1) // normal attacks
		{
				switch (self->attackdirection) // determine where collision detected.
				{
						case ATTACKLEFT:
							// check for collision starting two pixels to the left of the
							// player and moving to the right by three positions
							coll = checkCharHitChar(self, self->x - 2, self->y, 3, self->frameh);
							if (coll)
							{
									defaultAttackImpl(self, ATTACKLEFT);
							}
						  break;
						case ATTACKRIGHT:
							// check for collision starting two pixels to the left of the
							// player and moving to the right by three positions
							coll = checkCharHitChar(self, self->x + self->framew - 2, self->y, 3, self->frameh);
							if (coll)
							{
									defaultAttackImpl(self, ATTACKRIGHT);
							}
						  break;
						case ATTACKUP:
							// check for collision starting two pixels to the left of the
							// player and moving to the right by three positions
							coll = checkCharHitChar(self, self->x, self->y + 2, self->framew, 3);
							if (coll)
							{
									defaultAttackImpl(self, ATTACKUP);
							}
						  break;
						case ATTACKDOWN:
							// check for collision starting two pixels to the left of the
							// player and moving to the right by three positions
							coll = checkCharHitChar(self, self->x, self->y + self->frameh + 2, self->framew, 3);
							if (coll)
							{
									defaultAttackImpl(self, ATTACKDOWN);
							}
						  break;
						default:
						  break;
				}
				self->frame = image_link;
				display_character(self);
		}
}

void defaultAttackImpl(struct character *self, char attackdir)
{
		if (self->player == 0) 
		// attacking player1
		{
				// increase the players damage
				player1.damage += 3; // arbitrary damage
				// notify the main loop to adjust the players velocity
				// and what direction to adjust it.
				player1.hit = attackdir;
		}
		else // same thing except attacking player0
		{
				// increase the players damage
				player0.damage += 3; // arbitrary damage
				// notify the main loop to adjust the players velocity
				// and what direction to adjust it.
				player0.hit = attackdir;
		}
}

/*
;***********************************************************************
; Name:         defaultMove
; Description:  Implements a move for the player who calls it.
;								
;***********************************************************************/
void defaultMove(struct character *self)
{
    int coll = 0;
    clear_character(self);
		//if ( (self->moveflag & MOVEUP == MOVEUP) && (self->vervel > 0 ) )
		if ( self->movever_r && self->vervel > 0 )
		{
				// clear character
				//clear_character(self);
				// move up one pixel.
				self->y -= 1;
				// check for collisions
				coll = checkCharCollisions(self);
				if (coll)
				{
						self->vervel = 0;
						self->y += 1;
						if (self->veracc > 0)
						{
								// gravity adjusted above
								self->veracc = GRAVITY;
						}
				}
				self->movever_r = 0;
				//display_character(self);
		}
		//else if ( (self->moveflag & MOVEUP == MOVEUP ) && ( self->vervel < 0) )
		else if ( self->movever_r && self->vervel < 0)
		{
				// clear character
				//clear_character(self);
				// move down one pixel.
				self->y += 1;
				// check for collisions
				coll = checkCharCollisions(self);
				if (coll)
				{
						self->vervel = 0;
						self->veracc = 0;
						self->y -= 1;
				}
				self->movever_r = 0;
				//display_character(self);
		}
		//if ( ( (self->moveflag & MOVERI) == MOVERI) && (self->horvel > 0) )
		if ( self->movehor_r && self->horvel > 0 )
		{
				// clear character
				//clear_character(self);
				// move right one pixel.
				self->x += 1;
				self->currframe = 1;
				// check for collisions
				coll = checkCharCollisions(self);
				if (coll)
				{
						self->vervel = 0;
						self->veracc = 0;
						self->x -= 1;
				}
				self->movehor_r = 0;
				//display_character(self);

				// check for gravity
				self->y += 1;
				coll = checkCharCollisions(self);
				if (!coll)
				{
						self->veracc = -20;
				}
				self->y -= 1;
		}
		//else if ( (self->moveflag & MOVERI == MOVERI ) && ( self->horvel < 0 ) )
		else if (self->movehor_r && self->horvel < 0 )
		{
				// clear character
				//clear_character(self);
				// move left one pixel.
				self->x -= 1;
				self->currframe = 0;
				// check for collisions
				coll = checkCharCollisions(self);
				if (coll)
				{
						self->horvel = 0;
						self->horacc = 0;
						self->x += 1;
				}
				self->movehor_r = 0;
				//display_character(self);

				// check for gravity
				self->y += 1;
				coll = checkCharCollisions(self);
				if (!coll)
				{
						self->veracc = -20;
				}
				self->y -= 1;
		}
		display_character(self);
}

#endif
