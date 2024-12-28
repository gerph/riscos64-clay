/*******************************************************************
 * File:     vdushape
 * Purpose:  Plain shape rendering (VDU calls)
 * Author:   Justin Fletcher
 * Date:     13 Sep 2020
 ******************************************************************/

#ifndef VDUSHAPE_H
#define VDUSHAPE_H

#include "gcontext.h"
#include "graphics.h"


/*************************************************** Gerph *********
 Function:     gcontext_vdu_rectfill
 Description:  Fill a rectangle
 Parameters:   gc-> graphics context
               col = the colour to fill with
               x0,y0 = bottom left
               x1,y1 = top right
 Returns:      none
 ******************************************************************/
void gcontext_vdu_rectfill(gcontext_t *gc, uint32_t colour,
                           int x0, int y0, int x1, int y1);

/*************************************************** Gerph *********
 Function:     gcontext_vdu_trifill
 Description:  Fill a triangle
 Parameters:   gc-> graphics context
               col = the colour to fill with
               x0,y0 = first
               x1,y1 = second
               x2,y2 = third
 Returns:      none
 ******************************************************************/
void gcontext_vdu_trifill(gcontext_t *gc, uint32_t colour,
                          int x0, int y0, int x1, int y1, int x2, int y2);

/*************************************************** Gerph *********
 Function:     gcontext_vdu_linestart
 Description:  Start a line group
 Parameters:   gc-> graphics context
               col = the colour to draw with
 Returns:      none
 ******************************************************************/
void gcontext_vdu_linestart(gcontext_t *gc, uint32_t colour);

/*************************************************** Gerph *********
 Function:     gcontext_vdu_lineend
 Description:  End a line group
 Parameters:   gc-> graphics context
 Returns:      none
 ******************************************************************/
void gcontext_vdu_lineend(gcontext_t *gc);

/*************************************************** Gerph *********
 Function:     gcontext_vdu_line
 Description:  Draw a line
 Parameters:   gc-> graphics context
 Returns:      none
 ******************************************************************/
void gcontext_vdu_line(gcontext_t *gc, int x0, int y0, int x1, int y1);

/*************************************************** Gerph *********
 Function:     gcontext_vdu_linedotted
 Description:  Draw a dotted line
 Parameters:   gc-> graphics context
 Returns:      none
 ******************************************************************/
void gcontext_vdu_linedotted(gcontext_t *gc, int x0, int y0, int x1, int y1);

/*************************************************** Gerph *********
 Function:     gcontext_vdu_linedashed
 Description:  Draw a dashed line
 Parameters:   gc-> graphics context
 Returns:      none
 ******************************************************************/
void gcontext_vdu_linedashed(gcontext_t *gc, int x0, int y0, int x1, int y1);


/* Fill routines only for debugging */
void gcontext_vdu_fill_start(gcontext_t *gc, uint32_t fill_colour);
void gcontext_vdu_fill_move(gcontext_t *gc, int x, int y);
void gcontext_vdu_fill_line(gcontext_t *gc, int x, int y);
void gcontext_vdu_fill_close(gcontext_t *gc);
void gcontext_vdu_fill_end(gcontext_t *gc);

#endif
