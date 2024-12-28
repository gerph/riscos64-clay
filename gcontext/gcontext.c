/*******************************************************************
 * File:     gcontext
 * Purpose:  Graphics context operations
 * Author:   Justin Fletcher
 * Date:     11 Jun 2006
 ******************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gcontext.h"
#include "graphics.h"
#include "font.h"
#include "vdu5.h"
#include "vdushape.h"

/* Define this to debug printed output */
//#undef DEBUG
//#define DEBUG

#ifdef DEBUG
#define dprintf if (1) _swix(0x104,0), printf
#else
#define dprintf if (0) _swix(0x104,0), printf
#endif

/*************************************************** Gerph *********
 Function:     gcontext_generic_rectoutline
 Description:  Outline a rectangle
 Parameters:   col = the colour to fill with
               x0,y0 = bottom left
               x1,y1 = top right
 Returns:      none
 ******************************************************************/
static void gcontext_generic_rectoutline(gcontext_t *gc,
                                         uint32_t colour,
                                         int x0, int y0, int x1, int y1)
{
    gc->line_start(gc, colour);
    gc->line_line(gc, x0,y0,x0,y1);
    gc->line_line(gc, x0,y1,x1,y1);
    gc->line_line(gc, x1,y1,x1,y0);
    gc->line_line(gc, x1,y0,x0,y0);
    gc->line_end(gc);
}


/*************************************************** Gerph *********
 Function:     gcontext_initvdu
 Description:  Initialise the graphics context for the VDU system
 Parameters:   fonts = 1 if we should use fonts
 Returns:      gcontext_t to use for plotting, or NULL if unavailable
 ******************************************************************/
gcontext_t *gcontext_initvdu(int fonts)
{
    gcontext_t *gc = calloc(sizeof(*gc), 1);
    if (gc == NULL)
        return NULL;

    gc->rectangle_fill = gcontext_vdu_rectfill;
    gc->rectangle_outline = gcontext_generic_rectoutline;
    gc->line_start = gcontext_vdu_linestart;
    gc->line_line = gcontext_vdu_line;
    gc->line_dotted = gcontext_vdu_linedotted;
    gc->line_dashed = gcontext_vdu_linedashed;
    gc->line_end = gcontext_vdu_lineend;
    gc->fill_start = NULL; /* We don't have any 'fill' routines */
    gc->triangle_fill = gcontext_vdu_trifill;
    if (!fonts)
    {
        gc->text_findfont = vdu5_findfont;
        gc->text_losefont = vdu5_losefont;
        gc->text_getstringsize = vdu5_getstringsize;
        gc->text_getemsize = vdu5_getem;
        gc->text_paint = vdu5_paint;
        gc->text_paintattrib = vdu5_paintattrib;
    }
    else
    {
        gc->text_findfont = font_findfont;
        gc->text_losefont = font_losefont;
        gc->text_getstringsize = font_getstringsize;
        gc->text_getemsize = font_getem;
        gc->text_paint = font_paint;
        gc->text_paintattrib = font_paintattrib;
    }
#ifdef DEBUG_FILL
    gc->fill_start = gcontext_vdu_fill_start;
    gc->fill_move = gcontext_vdu_fill_move;
    gc->fill_line = gcontext_vdu_fill_line;
    gc->fill_close = gcontext_vdu_fill_close;
    gc->fill_end = gcontext_vdu_fill_end;
#endif
    return gc;
}
