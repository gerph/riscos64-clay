/*******************************************************************
 * File:     gcontext
 * Purpose:  Graphics context operations
 * Author:   Justin Fletcher
 * Date:     11 Jun 2006
 ******************************************************************/

#ifndef GCONTEXT_H
#define GCONTEXT_H

#include <stdint.h>

#define FONTATTRIB_ALIGN_HLEFT   (0<<0)
#define FONTATTRIB_ALIGN_HCENTRE (1<<0)
#define FONTATTRIB_ALIGN_HRIGHT  (2<<0)
#define FONTATTRIB_ALIGN_HMASK   (3<<0)
#define FONTATTRIB_ALIGN_VTOP    (0<<2)
#define FONTATTRIB_ALIGN_VCENTRE (1<<2)
#define FONTATTRIB_ALIGN_VBOTTOM (2<<2)
#define FONTATTRIB_ALIGN_VMASK   (3<<2)
#define FONTATTRIB_UNDERLINE     (1<<4)

#define COLOUR_NONE (0xFFFFFFFFlu)
#define COLOUR_RGB(r,g,b) ((((int)(r))<<8) | (((int)(g))<<16) | (((int)(b))<<24))

typedef struct coords_s {
  int32_t x;
  int32_t y;
} coords_t;

typedef struct bounds_s {
  int32_t width;
  int32_t height;
} bounds_t;

typedef struct bbox_s {
  int32_t x0, y0;
  int32_t x1, y1;
} bbox_t;

typedef struct stringbounds_s {
  int32_t ascent, descent;
  int32_t lbearing, rbearing;
  int32_t xoffset;
  int32_t charoffset;
} stringbounds_t;

typedef struct font_s *font_t;


typedef struct gcontext_s {
  void (*rectangle_fill)(uint32_t colour,
                         int32_t x0, int32_t y0, int32_t x1, int32_t y1);
  void (*rectangle_outline)(uint32_t colour,
                            int32_t x0, int32_t y0, int32_t x1, int32_t y1);

  void (*triangle_fill)(uint32_t colour,
                        int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

  void (*line_start)(uint32_t line_colour);
  void (*line_line)(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
  void (*line_end)(void);

  void (*fill_start)(uint32_t fill_colour);
  void (*fill_move)(int32_t x, int32_t y);
  void (*fill_line)(int32_t x, int32_t y);
  void (*fill_close)(void);
  void (*fill_end)(void);

  font_t (*text_findfont)(const char *name, int xsize, int ysize);
  void (*text_losefont)(font_t handle);

  void (*text_getstringsize)(font_t, stringbounds_t *bounds,
                             int32_t xlimit,
                             const char *str, int strsize, char split_char);

  bounds_t (*text_getemsize)(font_t);

  coords_t (*text_paint)(font_t handle,
                         int32_t xb, int32_t yb,
                         uint32_t bg, uint32_t fg,
                         const char *str, int strsize);

  coords_t (*text_paintattrib)(font_t handle,
                               int32_t x, int32_t y, int32_t x1, int32_t y1,
                               const char *str, int strsize,
                               uint32_t bg, uint32_t fg,
                               uint32_t attrib);
} gcontext_t;

extern gcontext_t gcontext;

#define rect_fillrgb(r,g,b, x0, y0, x1, y1) \
                  (gcontext.rectangle_fill)( COLOUR_RGB(r,g,b), \
                                             x0,y0,x1,y1)
#define rect_fill(col, x0, y0, x1, y1) \
                  (gcontext.rectangle_fill)(col,x0,y0,x1,y1)
#define rect_outline(col, x0, y0, x1, y1) \
                     (gcontext.rectangle_outline)(col,x0,y0,x1,y1)

#define tri_fill(col, x0, y0, x1, y1, x2, y2) \
                  (gcontext.triangle_fill)(col,x0,y0,x1,y1,x2,y2)

#define line_start(col) \
                  (gcontext.line_start)(col)
#define line(x0,y0,x1,y1) \
                  (gcontext.line_line)(x0,y0,x1,y1)
#define line_end() \
                  (gcontext.line_end)()

#define fill_start(col) \
                  gcontext.fill_start ? (gcontext.fill_start)(col),1 : 0
#define fill_move(x,y) \
                  (gcontext.fill_move)(x,y)
#define fill_line(x,y) \
                  (gcontext.fill_line)(x,y)
#define fill_close() \
                  (gcontext.fill_close)()
#define fill_end() \
                  (gcontext.fill_end)()

#define text_find(name,xsize,ysize) \
                  (gcontext.text_findfont)(name, xsize, ysize)
#define text_lose(handle) \
                  (gcontext.text_losefont)(handle)
#define text_getstringsize(handle, bounds, xlimit, str, strsize, split_char) \
                  (gcontext.text_getstringsize)(handle, bounds, xlimit, str, strsize, split_char)
#define text_getemsize(handle) \
                  (gcontext.text_getemsize)(handle)
#define text_paint(handle, xb, yb, bg, fg, str, strsize) \
                  (gcontext.text_paint)(handle,xb,yb,bg,fg,str,strsize)
#define text_paintattrib(handle, x, y, x1, y1, str, strsize, bg, fg, attr) \
                  (gcontext.text_paintattrib)(handle,x,y,x1,y1,str,strsize,bg,fg,attr)

/*************************************************** Gerph *********
 Function:     gcontext_initvdu
 Description:  Initialise the graphics context for the VDU system
 Parameters:   fonts = 1 if we should use fonts
 Returns:      none
 ******************************************************************/
void gcontext_initvdu(int fonts);

#endif
