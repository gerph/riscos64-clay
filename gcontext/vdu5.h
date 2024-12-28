/*******************************************************************
 * File:     vdu5
 * Purpose:  Plain VDU5 text rendering
 * Author:   Justin Fletcher
 * Date:     13 Sep 2020
 ******************************************************************/

#ifndef VDU5_H
#define VDU5_H

#include "gcontext.h"


/*************************************************** Gerph *********
 Function:     vdu5_findfont
 Description:  Find a font
 Parameters:   gc-> graphics context
               fontname-> the name of the font to find
               xsize = the size (points * 16)
               ysize = the size (points * 16)
 Returns:      font handle, or FONT_NONE if could not claim
 ******************************************************************/
font_t vdu5_findfont(gcontext_t *gc, const char *name, int xsize, int ysize);

/*************************************************** Gerph *********
 Function:     vdu5_losefont
 Description:  Lose a font we've claimed
 Parameters:   gc-> graphics context
               fh = the handle to lose
 Returns:      none
 ******************************************************************/
void vdu5_losefont(gcontext_t *gc, font_t fh);


/*************************************************** Gerph *********
 Function:     vdu5_getem
 Description:  Return the em size
 Parameters:   fh = the font handle, or NULL for system font
 Returns:      bounds for the em
 ******************************************************************/
bounds_t vdu5_getem(gcontext_t *gc, font_t fh);


/*************************************************** Gerph *********
 Function:     vdu5_getstringsize
 Description:  Return the size of the string into the structure
 Parameters:   gc-> graphics context
               fh = the font handle
               bounds-> the box to fill in
               xlimit = the widest the string may be, or -1 for unlimited
               str-> the string to read
               len = the number of characters to process, or -1 for all
               splitchar = character to split on, or 0 for none
 Returns:      none
 ******************************************************************/
void vdu5_getstringsize(gcontext_t *gc, font_t fh, stringbounds_t *bounds,
                        int xlimit,
                        const char *str, int strsize, char split_char);

/*************************************************** Gerph *********
 Function:     font_paint
 Description:  Paint our font somewhere
 Parameters:   gc-> graphics context
               fh = the font handle
               x = the x position to plot at (OS units)
               y = the y position to plot at (OS units)
                   (of the left base line)
               bg,fg = colours to use
               str-> the string to plot
               len = the number of characters to plot, or -1 for all
 Returns:      coords_t for the end position
 ******************************************************************/
coords_t vdu5_paint(gcontext_t *gc, font_t fh, int x, int y,
                    uint32_t bg, uint32_t fg,
                    const char *str, int len);

/*************************************************** Gerph *********
 Function:     vdu5_paintattrib
 Description:  Put a string, with the given attributes
 Parameters:   gc-> graphics context
               fh = the handle to plot with
               x0,y0,x1,y1 = box to centre in
               str-> the string to use
               len = the length to use
               bg,fg = colour to use
               attrib = the attributes to use
 Returns:      coords_t for the end position
 ******************************************************************/
coords_t vdu5_paintattrib(gcontext_t *gc,
                          font_t fh, int x0, int y0, int x1, int y1,
                          const char *str, int strsize,
                          uint32_t bg, uint32_t fg,
                          uint32_t attr);

#endif
