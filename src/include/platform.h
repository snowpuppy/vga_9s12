// Define platform structure used to keep track of platforms
// that are in field images and make sure the player doesn't
// pass through them.

#ifndef PLATFORM_H
#define PLATFORM_H

#define MAXPLATFORMS 7

struct platform {
	char x, y;
	char w, h;
};


#endif
