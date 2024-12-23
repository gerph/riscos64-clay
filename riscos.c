#include <stdio.h>
#include <stdlib.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"


#ifdef __riscos
#include <time.h>
#include "gcontext.h"
#include "border.h"
#include "font.h"
#include "screen.h"
#include "swis.h"
#endif

//#define DEBUG

#ifdef DEBUG
#define dprintf if (1) printf
#else
#define dprintf if (0) printf
#endif


#define screenWidth ((21.0 / 2.54) * 72)
#define screenHeight ((29.7 / 2.54) * 72)
bounds_t screen;

#define X(_p) ((int)( ((_p) * (screen.height - 4) / screenHeight) ))
#define Y(_p) ((int)( screen.height - 4 - ((_p) * (screen.height - 4) / screenHeight) ))
#define W(_p) ((int)( ((_p) * (screen.height - 4) / screenHeight) ))
#define H(_p) ((int)( ((_p) * (screen.height - 4) / screenHeight) ))
#define IW(_p) ((int)( ((_p) * screenHeight / (screen.height - 4)) ))
#define IH(_p) ((int)( ((_p) * screenHeight / (screen.height - 4)) ))

#define FONT_NAME "Homerton.Medium"

// Example measure text function
static inline Clay_Dimensions MeasureText(Clay_String *text, Clay_TextElementConfig *config) {
    // Clay_TextElementConfig contains members such as fontId, fontSize, letterSpacing etc
    // Note: Clay_String->chars is not guaranteed to be null terminated
    Clay_Dimensions dim = {0};
    stringbounds_t bounds = {0};
    /* The screenWidth / screenHeight is in points (72ths of an inch)
     * The screen is generally 180 OS units per inch.
     * Which means that each OS unit is 2.5 points.
     * and Font_FindFont wants the value in 16ths of a point.
     */
    int32_t xsize = W(config->fontSize / 2.5 * 16);
    int32_t ysize = H(config->fontSize / 2.5 * 16);

    font_t font = font_findfont(FONT_NAME, xsize, ysize);
    text_getstringsize(font, &bounds, -1, text->chars, text->length, -1);
    font_losefont(font);

    dim.width = IW(bounds.xoffset);
    dim.height = IH(bounds.ascent + bounds.descent);

    //printf("Measure text '%.*s' %i pt => %i, %i : size = %i x %i\n", text->length, text->chars, (int)config->fontSize, xsize, ysize, (int)dim.width, (int)dim.height);
    //printf("lbearing = %i, rbearing = %i, xoffset = %i\n", bounds.lbearing, bounds.rbearing, bounds.xoffset);

    return dim;
}

static void PlotText(Clay_RenderCommand *command) {
    Clay_TextElementConfig *config = command->config.textElementConfig;
    Clay_Color color = config->textColor;
    Clay_BoundingBox bb = command->boundingBox;
    Clay_String *text = &command->text;

    int32_t xsize = W(config->fontSize / 2.5 * 16);
    int32_t ysize = H(config->fontSize / 2.5 * 16);

    font_t font = font_findfont(FONT_NAME, xsize, ysize);
    //printf("Plot text '%.*s' %i pt @ %i, %i\n", text->length, text->chars, config->fontSize, (int)bb.x, (int)bb.y);
    text_paint(font, X(bb.x), Y(bb.y + bb.height),
               0, COLOUR_RGB(color.r, color.g, color.b),
               text->chars,
               text->length);
    font_losefont(font);
}

// Layout the first page.
void Layout() {
    static Clay_Color PRIMARY = { 0xa8, 0x42, 0x1c, 255 };
    static Clay_Color BACKGROUND = { 0xF4, 0xEB, 0xE6, 255 };
    static Clay_Color ACCENT = { 0xFA, 0xE0, 0xD4, 255 };

    CLAY(CLAY_LAYOUT({ .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                       .layoutDirection = CLAY_TOP_TO_BOTTOM }),
        CLAY_RECTANGLE({ .color = BACKGROUND })) {
        CLAY(CLAY_ID("PageMargins"),
             CLAY_LAYOUT({ .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                     .padding = { 70., 50. }, // Some nice looking page margins
                     .layoutDirection = CLAY_TOP_TO_BOTTOM,
                     .childGap = 10})) {

            // Section Title
            CLAY(CLAY_TEXT(
                     CLAY_STRING("Features Overview"),
                     CLAY_TEXT_CONFIG({
                             //.fontFamily = CLAY_STRING("Calistoga"),
                             .textColor = PRIMARY,
                             .fontSize = 24
                         })
                     ));

            // Feature Box
            CLAY(CLAY_LAYOUT({ .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIT() }, .childGap = 10 })) {
                CLAY(CLAY_LAYOUT({ .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIT() }}), CLAY_RECTANGLE({
                            .color = ACCENT,
                            .cornerRadius = CLAY_CORNER_RADIUS(12),
                        })) {
                    CLAY(CLAY_LAYOUT({.padding = { 20., 20. }, .childGap = 4, .layoutDirection = CLAY_TOP_TO_BOTTOM })) {
                        CLAY_TEXT(CLAY_STRING("- High performance"),
                                  CLAY_TEXT_CONFIG({ .textColor = PRIMARY, .fontSize = 14, /*.fontFamily = CLAY_STRING("Quicksand SemiBold")*/ }));
                        CLAY_TEXT(CLAY_STRING("- Declarative syntax"),
                                  CLAY_TEXT_CONFIG({ .textColor = PRIMARY, .fontSize = 14, /*.fontFamily = CLAY_STRING("Quicksand SemiBold")*/ }));
                        CLAY_TEXT(CLAY_STRING("- Flexbox-style responsive layout"),
                                  CLAY_TEXT_CONFIG({ .textColor = PRIMARY, .fontSize = 14, /*.fontFamily = CLAY_STRING("Quicksand SemiBold")*/ }));
                        CLAY_TEXT(CLAY_STRING("- Single .h file for C/C++"),
                                  CLAY_TEXT_CONFIG({ .textColor = PRIMARY, .fontSize = 14, /*.fontFamily = CLAY_STRING("Quicksand SemiBold")*/ }));
                        CLAY_TEXT(CLAY_STRING("- And now rendering in RISC OS 64!"),
                                  CLAY_TEXT_CONFIG({ .textColor = PRIMARY, .fontSize = 14, /*.fontFamily = CLAY_STRING("Quicksand SemiBold")*/ }));
                    }
                }
                CLAY(CLAY_LAYOUT({
                            .sizing = {CLAY_SIZING_FIT(), CLAY_SIZING_GROW()},
                            .padding = { 10, 10 },
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
                            .childGap = 4
                        }), CLAY_RECTANGLE({ .color = ACCENT, .cornerRadius = CLAY_CORNER_RADIUS(8) })) {
                    // Profile picture
                    CLAY(CLAY_LAYOUT({
                                .sizing = {CLAY_SIZING_FIT(), CLAY_SIZING_GROW()},
                                .padding = { 30, 0 },
                                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }}), CLAY_BORDER_OUTSIDE_RADIUS(2, PRIMARY, 10)) {
                        CLAY(CLAY_LAYOUT({ .sizing = { CLAY_SIZING_FIXED(32), CLAY_SIZING_FIXED(32) } }), CLAY_IMAGE({ .sourceDimensions = { 32, 32 },/* .path = CLAY_STRING("resources/check.png")*/ }));
                    }
                }
            }

            CLAY(CLAY_LAYOUT({ .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(16) } }));

            CLAY(CLAY_LAYOUT({ .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() }, .childGap = 10, .layoutDirection = CLAY_TOP_TO_BOTTOM })) {
                CLAY_TEXT(CLAY_STRING("Cairo"), CLAY_TEXT_CONFIG({ /*.fontFamily = CLAY_STRING("Calistoga"), */.fontSize = 24, .textColor = PRIMARY }));
                CLAY(CLAY_LAYOUT({ .padding = { 10, 10 } }), CLAY_RECTANGLE({ .color = ACCENT, .cornerRadius = CLAY_CORNER_RADIUS(10) })) {
                    CLAY_TEXT(CLAY_STRING("Officiis quia quia qui inventore ratione voluptas et. Quidem sunt unde similique. Qui est et exercitationem cumque harum illum. Numquam placeat aliquid quo voluptatem. "
                                          "Deleniti saepe nihil exercitationem nemo illo. Consequatur beatae repellat provident similique. Provident qui exercitationem deserunt sapiente. Quam qui dolor corporis odit. "
                                          "Assumenda corrupti sunt culpa pariatur. Vero sit ut minima. In est consequatur minus et cum sint illum aperiam. Qui ipsa quas nisi omnis aut quia nobis. "
                                          "Corporis deserunt eum mollitia modi rerum voluptas. Expedita non ab esse. Sit voluptates eos voluptatem labore aspernatur quia eum. Modi cumque atque non. Sunt officiis corrupti neque ut inventore excepturi rem minima. Possimus sed soluta qui ea aut ipsum laborum fugit. "
                                          "Voluptate eum consectetur non. Quo autem voluptate soluta atque dolorum maxime. Officiis inventore omnis eveniet beatae ipsa optio. Unde voluptatum ut autem quia sit sit et. Ut inventore qui quia totam consequatur. Sit ea consequatur omnis rerum nulla aspernatur deleniti."), CLAY_TEXT_CONFIG({ /*.fontFamily = CLAY_STRING("Quicksand SemiBold"),*/ .fontSize = 16, .textColor = PRIMARY, .lineHeight = 16 }));
                }
            }
        }
    }
}


int main(int argc, char *argv[])
{

#ifdef __riscos
    gcontext_initvdu(1);
    {
        int32_t mode_selector[] = {
            1, /* Flags */
            800,
            1024,
            5,
            -1,
            -1,
        };
        _kernel_oserror * err = _swix(OS_ScreenMode, _INR(0, 1), 0, &mode_selector);
        if (err)
        {
            printf("Couldn't change mode: %s\n", err->errmess);
            abort();
        }
    }
    screen_get_size(&screen);
#endif

    printf("1\n");
    uint64_t totalMemorySize = Clay_MinMemorySize();
    printf("2\n");
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
    printf("3\n");
    Clay_Initialize(arena, (Clay_Dimensions) { screenWidth, screenHeight });
    printf("4\n");
    Clay_SetMeasureTextFunction(MeasureText);
    printf("5\n");
    Clay_BeginLayout();
    printf("6\n");

    Layout();
    printf("7\n");

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    for (int i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommand *command = &renderCommands.internalArray[i];

        switch (command->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_NONE:
                dprintf("None\n");
                break;

            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                    dprintf("Rectangle\n");
                    Clay_RectangleElementConfig *config = command->config.rectangleElementConfig;
                    Clay_Color color = config->color;
                    Clay_BoundingBox bb = command->boundingBox;
                    dprintf("colour = %i, %i, %i\n", (int)color.r, (int)color.g, (int)color.b);
                    dprintf("colour = %08x\n", COLOUR_RGB(color.r, color.g, color.b));
                    dprintf("bb.x = %i\n", (int)bb.x);
                    dprintf("bb.y = %i\n", (int)bb.y);
                    dprintf("bb.width = %i\n", (int)bb.width);
                    dprintf("bb.height = %i\n", (int)bb.height);
#ifdef __riscos
                    rect_fillrgb(color.r, color.g, color.b,
                                 X(bb.x), Y(bb.y),
                                 X(bb.x + bb.width),
                                 Y(bb.y + bb.height));
                    dprintf("  %i, %i, %i, %i\n",
                                 X(bb.x), Y(bb.y),
                                 X(bb.x + bb.width),
                                 Y(bb.y + bb.height));
#endif
                }
                break;

            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                    dprintf("Border\n");
                    Clay_BorderElementConfig *config = command->config.borderElementConfig;
                    Clay_BoundingBox bb = command->boundingBox;

                    borderedges_t outer;
                    borderedges_t inner;
                    outer.top = Y(bb.y + bb.height);
                    outer.bottom = Y(bb.y);
                    outer.left = X(bb.x);
                    outer.right = X(bb.x + bb.width);
                    inner.top = Y(bb.y + bb.height - config->top.width);
                    inner.right = X(bb.x + bb.width - config->right.width);
                    inner.bottom = Y(bb.y + config->bottom.width);
                    inner.left = X(bb.x + config->bottom.width);


                    /* Shapes are in the form:
                        a b
                        c d
                     */
                    if (config->top.width > 0)
                    {
                        uint32_t col = COLOUR_RGB(config->top.color.r, config->top.color.g, config->top.color.b);
                        int xa, ya;
                        int xb, yb;
                        int xc, yc;
                        int xd, yd;
                        xa = outer.left;    ya = outer.top;
                        xb = outer.right;   yb = outer.top;
                        if (config->left.width)
                        {
                            xc = inner.left;    yc = inner.top;
                        }
                        else
                        {
                            xc = outer.left;    yc = inner.top;
                        }
                        if (config->right.width)
                        {
                            xd = inner.right;   yd = inner.top;
                        }
                        else
                        {
                            xd = outer.right;   yd = inner.top;
                        }
                        tri_fill(col, xa, ya,
                                      xc, yc,
                                      xb, yb);
                        tri_fill(col, xc, yc,
                                      xb, yb,
                                      xd, yd);
                    }

                    if (config->right.width > 0)
                    {
                        uint32_t col = COLOUR_RGB(config->right.color.r, config->right.color.g, config->right.color.b);
                        int xa, ya;
                        int xb, yb;
                        int xc, yc;
                        int xd, yd;
                        xb = outer.right;   yb = outer.top;
                        xd = outer.right;   yd = outer.bottom;
                        if (config->top.width)
                        {
                            xa = inner.right;   ya = inner.top;
                        }
                        else
                        {
                            xa = inner.right;   ya = outer.top;
                        }
                        if (config->bottom.width)
                        {
                            xc = inner.right;   yc = inner.bottom;
                        }
                        else
                        {
                            xc = inner.right;   yc = outer.bottom;
                        }
                        tri_fill(col, xa, ya,
                                      xc, yc,
                                      xb, yb);
                        tri_fill(col, xc, yc,
                                      xb, yb,
                                      xd, yd);
                    }

                    if (config->bottom.width > 0)
                    {
                        uint32_t col = COLOUR_RGB(config->bottom.color.r, config->bottom.color.g, config->bottom.color.b);
                        int xa, ya;
                        int xb, yb;
                        int xc, yc;
                        int xd, yd;
                        xc = outer.left;    yc = outer.bottom;
                        xd = outer.right;   yd = outer.bottom;
                        if (config->left.width)
                        {
                            xa = inner.left;    ya = inner.bottom;
                        }
                        else
                        {
                            xa = outer.left;    ya = inner.bottom;
                        }
                        if (config->right.width)
                        {
                            xb = inner.right;   yb = inner.bottom;
                        }
                        else
                        {
                            xb = outer.right;   yb = inner.bottom;
                        }
                        tri_fill(col, xa, ya,
                                      xc, yc,
                                      xb, yb);
                        tri_fill(col, xc, yc,
                                      xb, yb,
                                      xd, yd);
                    }

                    if (config->left.width > 0)
                    {
                        uint32_t col = COLOUR_RGB(config->left.color.r, config->left.color.g, config->left.color.b);
                        int xa, ya;
                        int xb, yb;
                        int xc, yc;
                        int xd, yd;
                        xa = outer.left;    ya = outer.top;
                        xc = outer.left;    yc = outer.bottom;
                        if (config->top.width)
                        {
                            xb = inner.left;    yb = inner.top;
                        }
                        else
                        {
                            xb = inner.left;    yb = outer.top;
                        }
                        if (config->bottom.width)
                        {
                            xd = inner.left;    yd = inner.bottom;
                        }
                        else
                        {
                            xd = inner.left;    yd = outer.bottom;
                        }
                        tri_fill(col, xa, ya,
                                      xc, yc,
                                      xb, yb);
                        tri_fill(col, xc, yc,
                                      xb, yb,
                                      xd, yd);
                    }
                }
                break;

            case CLAY_RENDER_COMMAND_TYPE_TEXT:
                dprintf("Text\n");
                PlotText(command);
                break;

            case CLAY_RENDER_COMMAND_TYPE_IMAGE:
                dprintf("Image\n");
                break;

            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
                dprintf("Scissor_start\n");
                break;

            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
                dprintf("Scissor_end\n");
                break;

            case CLAY_RENDER_COMMAND_TYPE_CUSTOM:
                dprintf("Custom\n");
                break;

            default:
                dprintf("Command type %i\n", command->commandType);
                break;
        }
    }
    //printf("8\n");

#ifdef __riscos
    //_swix(OS_ReadC, 0);
    time_t start = time(NULL);
    while (time(NULL) - start < 3)
    {
        _swix(OS_Byte, _IN(0), 19);
    }
#endif

    return 0;
}
