// Define bset and bclr macros
#ifndef BSET_CLR_H
#define BSET_CLR_H

// define bset and bclr macros
#define bset(x,y) \
    ( x = x | y )
#define bclr(x,y) \
    ( x = x & ~(y) )

#endif

