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

/* When no font can be returned */
#define FONT_NONE ((font_t)(NULL))

typedef struct gcontext_s {
    void (*rectangle_fill)(struct gcontext_s *gc,
                           uint32_t colour,
                           int x0, int y0, int x1, int y1);
    void (*rectangle_outline)(struct gcontext_s *gc,
                              uint32_t colour,
                              int32_t x0, int32_t y0, int32_t x1, int32_t y1);

    void (*triangle_fill)(struct gcontext_s *gc,
                          uint32_t colour,
                          int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    void (*line_start)(struct gcontext_s *gc,
                       uint32_t colour);
    void (*line_line)(struct gcontext_s *gc,
                      int32_t x0, int32_t y0, int32_t x1, int32_t y1);
    void (*line_dotted)(struct gcontext_s *gc,
                        int32_t x0, int32_t y0, int32_t x1, int32_t y1);
    void (*line_dashed)(struct gcontext_s *gc,
                        int32_t x0, int32_t y0, int32_t x1, int32_t y1);
    void (*line_end)(struct gcontext_s *gc);

    void (*fill_start)(struct gcontext_s *gc,
                       uint32_t fill_colour);
    void (*fill_move)(struct gcontext_s *gc,
                      int32_t x, int32_t y);
    void (*fill_line)(struct gcontext_s *gc,
                      int32_t x, int32_t y);
    void (*fill_close)(struct gcontext_s *gc);
    void (*fill_end)(struct gcontext_s *gc);

    font_t (*text_findfont)(struct gcontext_s *gc,
                            const char *name, int xsize, int ysize);
    void (*text_losefont)(struct gcontext_s *gc,
                          font_t handle);

    void (*text_getstringsize)(struct gcontext_s *gc,
                               font_t, stringbounds_t *bounds,
                               int32_t xlimit,
                               const char *str, int strsize, char split_char);

    bounds_t (*text_getemsize)(struct gcontext_s *gc,
                               font_t);

    coords_t (*text_paint)(struct gcontext_s *gc,
                           font_t handle,
                           int32_t xb, int32_t yb,
                           uint32_t bg, uint32_t fg,
                           const char *str, int strsize);

    coords_t (*text_paintattrib)(struct gcontext_s *gc,
                                 font_t handle,
                                 int32_t x, int32_t y, int32_t x1, int32_t y1,
                                 const char *str, int strsize,
                                 uint32_t bg, uint32_t fg,
                                 uint32_t attrib);
} gcontext_t;


/*************************************************** Gerph *********
 Function:     gcontext_initvdu
 Description:  Initialise the graphics context for the VDU system
 Parameters:   fonts = 1 if we should use fonts
 Returns:      gcontext_t to use for plotting, or NULL if unavailable
 ******************************************************************/
struct gcontext_s *gcontext_initvdu(int fonts);

#endif
