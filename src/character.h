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

struct character
{
		char x,y;
		char horvel, vertvel;
		char horacc, veracc;
		char damage;
		char name[3+1];
		void (*attack)(char, char) // type of attack and direction of attack
		void (*move)(char, char) // up/down and left/right atd values
		char *frame;
		char currframe;
		char framew, frameh;
};

/*
;***********************************************************************
; Name:         defaultAttack
; Description:  Implements an attack for the player who calls it.
;								
;***********************************************************************/
void defautAttack(char type, char direction)
{
}

/*
;***********************************************************************
; Name:         defaultMove
; Description:  Implements a move for the player who calls it.
;								
;***********************************************************************/
void defaultMove(char vertical, char horizontal)
{
}

#endif
