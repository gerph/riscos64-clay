/*******************************************************************
 * File:     border
 * Purpose:  Simple bordering
 * Author:   Justin Fletcher
 * Date:     17 Jan 2006
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swis.h"

#include "gcontext.h"
#include "border.h"

#ifdef FORTIFY
#include "fortify.h"
#endif

/* Define this to debug this file */
//#define DEBUG

#ifdef DEBUG
#define dprintf if (1) _swix(0x104,0), printf
#else
#define dprintf if (0) _swix(0x104,0), printf
#endif

static void get_shadow_colours(uint32_t colour,
                               uint32_t *lightp, uint32_t *darkp)
{
  int32_t r,g,b;
  /* Make r,g,b into the dark colour */
#define SLAB_DEPTH (0x60)
  r = (colour >> 8) & 255;
  if (r<SLAB_DEPTH/2) r = 0;
  else if (r > 255-SLAB_DEPTH/2) r = 255-SLAB_DEPTH;
  else r = r - SLAB_DEPTH/2;

  g = (colour >> 16) & 255;
  if (g<SLAB_DEPTH/2) r = 0;
  else if (g > 255-SLAB_DEPTH/2) g = 255-SLAB_DEPTH;
  else g = g - SLAB_DEPTH/2;

  b = (colour >> 24) & 255;
  if (b<SLAB_DEPTH/2) b = 0;
  else if (b > 255-SLAB_DEPTH/2) b = 255-SLAB_DEPTH;
  else b = b - SLAB_DEPTH/2;

  *darkp = COLOUR_RGB(r,g,b);
  *lightp = COLOUR_RGB(r+SLAB_DEPTH, g+SLAB_DEPTH, b+SLAB_DEPTH);
}

static void border_intdraw(gcontext_t *gc,
                           borderedges_t *edges,
                           int x, int y, int width, int height,
                           uint32_t tl, uint32_t br)
{
  int more;
#if 0
  borderedges_t left;
  borderedges_t coord;
  y-=1; width-=1; height-=1;
  coord.top = y;
  coord.bottom = y-height;
  coord.left = x;
  coord.right = x + width;
  left = *edges;
  if (tl == br)
    gcol(tl);
  do
  {
    move(      coord.left,   coord.bottom);
    if (tl!=br)
      gcol(br);
    drawnofirst(coord.right,  coord.bottom);
    drawnofirst(coord.right,  coord.top);
    if (tl!=br)
      gcol(tl);
    drawnofirst(coord.left,   coord.top);
    drawnofirst(coord.left,   coord.bottom);
    more=0;
    if (--left.top > 0) left.top--, coord.top-=2, more=1;
    if (--left.right > 0) left.right--, coord.right-=2, more=1;
    if (--left.bottom > 0) left.bottom--, coord.bottom+=2, more=1;
    if (--left.left > 0) left.left--, coord.left+=2, more=1;
  } while (more);
#else
  borderedges_t outer;
  borderedges_t inner;
  outer.top = y;
  outer.bottom = y-height;
  outer.left = x;
  outer.right = x + width;
  inner.top = outer.top - edges->top;
  inner.right = outer.right - edges->right;
  inner.bottom = outer.bottom + edges->bottom;
  inner.left = outer.left + edges->left;
  dprintf("border_intdraw: %08lx/%08lx\n", tl,br);
  if (tl == br)
  {
    /* It's a 'solid' external border; no slab effect, so we can optimise
       the fill a little */
    if (gc->fill_start)
    {
      gc->fill_start(gc, tl);
      /* We can use a path */
      /* 0-------------------1
         | 5---------------6 |
         | |               | |
         | |               | |
         | |               | |
         | 8---------------7 |
         3-------------------2 */
      gc->fill_move(gc, outer.left,  outer.top);     /* 0 */
      gc->fill_line(gc, outer.right, outer.top);     /* 1 */
      gc->fill_line(gc, outer.right, outer.bottom);  /* 2 */
      gc->fill_line(gc, outer.left,  outer.bottom);  /* 3 */
      gc->fill_line(gc, outer.left,  outer.top);     /* 0 */
      gc->fill_close(gc);

      gc->fill_move(gc, inner.left,  inner.top);     /* 4 */
      gc->fill_line(gc, inner.right, inner.top);     /* 5 */
      gc->fill_line(gc, inner.right, inner.bottom);  /* 6 */
      gc->fill_line(gc, inner.left,  inner.bottom);  /* 7 */
      gc->fill_line(gc, inner.left,  inner.top);     /* 4 */
      gc->fill_close(gc);
      gc->fill_end(gc);
    }
    else
    {
      /* Have to do things the simple way */
      /* 0-+---------------3-+
         | 4---------------+ |
         | |               | |
         | |               | |
         | |               | |
         | +---------------+-2
         +-1-----------------+ */
      gc->rectangle_fill(gc, tl, outer.left, outer.top, inner.left, outer.bottom); /* 0->1 */
      gc->rectangle_fill(gc, tl, inner.left, outer.bottom, outer.right, inner.bottom); /* 1->2 */
      gc->rectangle_fill(gc, tl, outer.right, inner.bottom, inner.right, outer.top); /* 2->3 */
      gc->rectangle_fill(gc, tl, inner.right, outer.top, inner.left, inner.top); /* 2->3 */
    }
  }
  else
  {
    /* It's a slabbed block */
    if (gc->fill_start)
    {
      gc->fill_start(gc, tl);
      /*  0-------------5
          | 3----------4
          | |
          | |
          | 2
          1/
       */
      gc->fill_move(gc, outer.left,  outer.top);     /* 0 */
      gc->fill_line(gc, outer.left,  outer.bottom);  /* 1 */
      gc->fill_line(gc, inner.left,  inner.bottom);  /* 2 */
      gc->fill_line(gc, inner.left,  inner.top);     /* 3 */
      gc->fill_line(gc, inner.right, inner.top);     /* 4 */
      gc->fill_line(gc, outer.right, outer.top);     /* 5 */
      gc->fill_line(gc, outer.left,  outer.top);     /* 0 */
      gc->fill_close(gc);
      gc->fill_end(gc);

      /*              /1
                     2 |
                     | |
                     | |
           4---------3 |
         5/------------0
       */
      gc->fill_start(gc, br);
      gc->fill_move(gc, outer.right,  outer.bottom); /* 0 */
      gc->fill_line(gc, outer.right,  outer.top);    /* 1 */
      gc->fill_line(gc, inner.right,  inner.top);    /* 2 */
      gc->fill_line(gc, inner.right,  inner.bottom); /* 3 */
      gc->fill_line(gc, inner.left,   inner.bottom); /* 4 */
      gc->fill_line(gc, outer.left,   outer.bottom); /* 5 */
      gc->fill_line(gc, outer.right,  outer.bottom); /* 0 */
      gc->fill_close(gc);
      gc->fill_end(gc);
    }
    else
    {
      /*  2-------------4
          | 3----------5
          | |
          | |
          | 1
          0/
       */
      gc->triangle_fill(gc, tl, outer.left,  outer.bottom, /* 0 */
                                inner.left,  inner.bottom, /* 1 */
                                outer.left,  outer.top     /* 2 */);

      gc->triangle_fill(gc, tl, inner.left,  inner.bottom, /* 1 */
                                outer.left,  outer.top,    /* 2 */
                                inner.left,  inner.top     /* 3 */);

      gc->triangle_fill(gc, tl, outer.left,  outer.top,    /* 2 */
                                inner.left,  inner.top,    /* 3 */
                                outer.right, outer.top   /* 4 */ );

      gc->triangle_fill(gc, tl, inner.left,  inner.top,    /* 3 */
                                outer.right, outer.top,    /* 4 */
                                inner.right, inner.top     /* 5 */);

      /*              /0
                     1 |
                     | |
                     | |
           5---------3 |
         4/------------2
       */
      gc->triangle_fill(gc, br, outer.right,  outer.top,    /* 0 */
                                inner.right,  inner.top,    /* 1 */
                                outer.right,  outer.bottom  /* 2 */);

      gc->triangle_fill(gc, br, inner.right,  inner.top,    /* 1 */
                                outer.right,  outer.bottom, /* 2 */
                                inner.right,  inner.bottom  /* 3 */);

      gc->triangle_fill(gc, br, outer.right,  outer.bottom, /* 2 */
                                inner.right,  inner.bottom, /* 3 */
                                outer.left,   outer.bottom  /* 4 */ );

      gc->triangle_fill(gc, br, inner.right,  inner.bottom, /* 3 */
                                outer.left,   outer.bottom, /* 4 */
                                inner.left,   inner.bottom  /* 5 */);
    }
  }
#endif
}

/*************************************************** Gerph *********
 Function:     border_draw
 Description:  Draw a border around a region
 Parameters:   gc-> the context to use
               type = the type of border we should draw
               edges = a block describing the size of the edges
               x,y,width,height = the thing that we're bordering,
                                      top left corner
               face = the lighter colour, or COLOUR_NONE
               opp = the darker colour, or COLOUR_NONE
 Returns:      none
 ******************************************************************/
void border_draw(gcontext_t *gc,
                 bordertype_t type,
                 borderedges_t *edges,
                 int x, int y, int width, int height,
                 uint32_t face, uint32_t opp)
{
  int hasborder;
  hasborder = (edges->top > 0 ||
               edges->right > 0 ||
               edges->bottom > 0 ||
               edges->left > 0);
  if (!hasborder || type == bt_none)
    return;

  if (face == COLOUR_NONE && opp == COLOUR_NONE)
    return;

  if (face == COLOUR_NONE || opp == COLOUR_NONE)
  {
    uint32_t colour;
    if (face == COLOUR_NONE) colour = opp;
    else                     colour = face;
    if (type != bt_solid &&
        type != bt_double)
      get_shadow_colours(colour, &face, &opp);
    else
      face = opp = colour;
  }

  /* dprintf("type = %i, face = %08lx, opp = %08lx\n", type, face, opp); */

  switch (type)
  {
    default:
    case bt_solid:
      dprintf("solid\n");
      border_intdraw(gc, edges, x,y,width,height, opp, opp);
      break;

    case bt_double:
      {
        borderedges_t inner;
        borderedges_t outer;
        outer.top = (edges->top+1)/3;
        outer.left = (edges->left+1)/3;
        outer.right = (edges->right+1)/3;
        outer.bottom = (edges->bottom+1)/3;
        inner = outer;
        border_intdraw(gc, &outer, x,y,width,height, opp, opp);
        x+= edges->left - inner.left;
        y-= edges->top - inner.top;
        width -=  (edges->right - inner.right) + (edges->left - inner.left);
        height -=  (edges->bottom - inner.bottom) + (edges->top - inner.top);
        border_intdraw(gc, &inner, x,y,width,height, opp, opp);
      }
      break;

    case bt_inset:
      border_intdraw(gc, edges, x,y,width,height, opp, face);
      break;

    case bt_outset:
      border_intdraw(gc, edges, x,y,width,height, face, opp);
      break;

    case bt_ridge:
      {
        borderedges_t inner;
        borderedges_t outer;
        outer.top = edges->top/2;
        outer.left = edges->left/2;
        outer.right = edges->right/2;
        outer.bottom = edges->bottom/2;
        inner = outer;
        border_intdraw(gc, &outer, x,y,width,height, face, opp);
        x+= inner.left;
        y-= inner.top;
        width -=  inner.right + inner.left;
        height -=  inner.bottom + inner.top;
        border_intdraw(gc, &inner, x,y,width,height, opp, face);
      }
      break;

    case bt_groove:
      {
        borderedges_t inner;
        borderedges_t outer;
        outer.top = edges->top/2;
        outer.left = edges->left/2;
        outer.right = edges->right/2;
        outer.bottom = edges->bottom/2;
        inner = outer;
        border_intdraw(gc, &outer, x,y,width,height, opp, face);
        x+= inner.left;
        y-= inner.top;
        width -= inner.right + inner.left;
        height -=  inner.bottom + inner.top;
        border_intdraw(gc, &inner, x,y,width,height, face, opp);
      }
      break;
  }
}


/*************************************************** Gerph *********
 Function:     border_drawsimple
 Description:  Draw a simplified border
 Parameters:   gc-> the context to use
               type = the type of border we should draw
               size = the border size
               x,y,width,height = the thing that we're bordering,
                                      top left corner
               face = the lighter colour, or COLOUR_NONE
               opp = the darker colour, or COLOUR_NONE
 Returns:      none
 ******************************************************************/
void border_drawsimple(gcontext_t *gc,
                       bordertype_t type, int size,
                       int x, int y, int width, int height,
                       uint32_t face, uint32_t opp)
{
  borderedges_t edges;
  edges.top = edges.left = edges.right = edges.bottom = size;
  border_draw(gc, type,&edges,x,y,width,height,face,opp);
}



/*************************************************** Gerph *********
 Function:     border_drawfull
 Description:  Draw a border around a region with fully specified colours
 Parameters:   colours = a block describing the colours of the edges,
                         or COLOUR_NONE for no colour.
               edges = a block describing the size of the edges
               x,y,width,height = the thing that we're bordering,
                                      top left corner
 Returns:      none
 ******************************************************************/
void border_drawfull(gcontext_t *gc,
                     bordercolours_t *colours,
                     borderedges_t *edges,
                     int x, int y, int width, int height)
{
    borderedges_t outer;
    borderedges_t inner;
    outer.top = y + height;
    outer.bottom = y;
    outer.left = x;
    outer.right = x + width;
    inner.top = y + height - edges->top;
    inner.right = x + width - edges->right;
    inner.bottom = y + edges->bottom;
    inner.left = x + edges->left;

    /* Shapes are in the form:
        a b
        c d
     */
    if (edges->top > 0 && colours->top != COLOUR_NONE)
    {
        uint32_t col = colours->top;
        int xa, ya;
        int xb, yb;
        int xc, yc;
        int xd, yd;
        xa = outer.left;    ya = outer.top;
        xb = outer.right;   yb = outer.top;
        if (edges->left > 0 && colours->left != COLOUR_NONE)
        {
            xc = inner.left;    yc = inner.top;
        }
        else
        {
            xc = outer.left;    yc = inner.top;
        }
        if (edges->right > 0 && colours->right != COLOUR_NONE)
        {
            xd = inner.right;   yd = inner.top;
        }
        else
        {
            xd = outer.right;   yd = inner.top;
        }
        if (xa == xc && xb == xd)
        {
            gc->rectangle_fill(gc, col, xa, ya, xd, yd);
        }
        else
        {
            gc->triangle_fill(gc, col, xa, ya,
                                       xc, yc,
                                       xb, yb);
            gc->triangle_fill(gc, col, xc, yc,
                                       xb, yb,
                                       xd, yd);
        }
    }

    if (edges->right > 0 && colours->right != COLOUR_NONE)
    {
        uint32_t col = colours->right;
        int xa, ya;
        int xb, yb;
        int xc, yc;
        int xd, yd;
        xb = outer.right;   yb = outer.top;
        xd = outer.right;   yd = outer.bottom;
        if (edges->top > 0 && colours->top != COLOUR_NONE)
        {
            xa = inner.right;   ya = inner.top;
        }
        else
        {
            xa = inner.right;   ya = outer.top;
        }
        if (edges->bottom > 0 && colours->bottom != COLOUR_NONE)
        {
            xc = inner.right;   yc = inner.bottom;
        }
        else
        {
            xc = inner.right;   yc = outer.bottom;
        }
        if (ya == yb && yc == yd)
        {
            gc->rectangle_fill(gc, col, xa, ya, xd, yd);
        }
        else
        {
            gc->triangle_fill(gc, col, xa, ya,
                                       xc, yc,
                                       xb, yb);
            gc->triangle_fill(gc, col, xc, yc,
                                       xb, yb,
                                       xd, yd);
        }
    }

    if (edges->bottom > 0 && colours->bottom != COLOUR_NONE)
    {
        uint32_t col = colours->bottom;
        int xa, ya;
        int xb, yb;
        int xc, yc;
        int xd, yd;
        xc = outer.left;    yc = outer.bottom;
        xd = outer.right;   yd = outer.bottom;
        if (edges->left > 0 && colours->left != COLOUR_NONE)
        {
            xa = inner.left;    ya = inner.bottom;
        }
        else
        {
            xa = outer.left;    ya = inner.bottom;
        }
        if (edges->right > 0 && colours->right != COLOUR_NONE)
        {
            xb = inner.right;   yb = inner.bottom;
        }
        else
        {
            xb = outer.right;   yb = inner.bottom;
        }
        if (xa == xc && xb == xd)
        {
            gc->rectangle_fill(gc, col, xa, ya, xd, yd);
        }
        else
        {
            gc->triangle_fill(gc, col, xa, ya,
                                       xc, yc,
                                       xb, yb);
            gc->triangle_fill(gc, col, xc, yc,
                                       xb, yb,
                                       xd, yd);
        }
    }

    if (edges->left > 0 && colours->left != COLOUR_NONE)
    {
        uint32_t col = colours->left;
        int xa, ya;
        int xb, yb;
        int xc, yc;
        int xd, yd;
        xa = outer.left;    ya = outer.top;
        xc = outer.left;    yc = outer.bottom;
        if (edges->top > 0 && colours->top != COLOUR_NONE)
        {
            xb = inner.left;    yb = inner.top;
        }
        else
        {
            xb = inner.left;    yb = outer.top;
        }
        if (edges->bottom > 0 && colours->bottom != COLOUR_NONE)
        {
            xd = inner.left;    yd = inner.bottom;
        }
        else
        {
            xd = inner.left;    yd = outer.bottom;
        }
        if (ya == yb && yc == yd)
        {
            gc->rectangle_fill(gc, col, xa, ya, xd, yd);
        }
        else
        {
            gc->triangle_fill(gc, col, xa, ya,
                                       xc, yc,
                                       xb, yb);
            gc->triangle_fill(gc, col, xc, yc,
                                       xb, yb,
                                       xd, yd);
        }
    }
}
