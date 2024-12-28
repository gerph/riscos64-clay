/*******************************************************************
 * File:     graphics
 * Purpose:  Simplified graphics calls
 * Author:   Justin Fletcher
 * Date:     16 Jan 2006
 ******************************************************************/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "swis.h"

#define dotpattern(len, n1, n2, n3, n4, n5, n6, n7, n8) \
    do { \
        char bytes[10] = {23, 6, (n1), (n2), (n3), (n4), (n5), (n6), (n7), (n8)}; \
        _swix(OS_Byte, _INR(0,2), 163, 242, (len)); \
        _swix(OS_WriteN, _INR(0, 1), &bytes[0], 10); \
    } while (0)

#define plot(o,x,y)         _swix(OS_Plot, _INR(0,2), o, x,y)

#define move(x,y)           plot(4, x,y)
#define draw(x,y)           plot(5, x,y)
#define drawdotted(x,y)     plot(16+5, x,y)
#define drawnofirst(x,y)    plot(32+5, x,y)
#define rect(x,y,x1,y1)     move(x,y), plot(96+5, x1,y1)
#define tri(x,y)            plot(80+5, x,y)
#define rectwh(x,y,w,h)     move(x,y), plot(96+5, w,h)

#define gcol(col)           _swix(ColourTrans_SetGCOL, _IN(0)|_INR(3,4), col, 1<<8, 0)

#endif
