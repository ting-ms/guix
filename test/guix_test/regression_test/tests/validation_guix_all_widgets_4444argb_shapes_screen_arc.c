/* This is a small demo of the high-performance GUIX graphics framework. */

#include <stdio.h>
#include "tx_api.h"
#include "gx_api.h"
#include "gx_validation_utility.h"

TEST_PARAM test_parameter = {
    "guix_all_widgets_4444argb_shapes_screen_arc", /* Test name */
    10, 10, 400, 402  /* Define the coordinates of the capture area.
                         In this test, we only need to capture the graphics
                         drawing area.  */
};

static VOID      control_thread_entry(ULONG);
int main(int argc, char ** argv)
{
    /* Parse the command line argument. */
    gx_validation_setup(argc, argv);

    /* Start ThreadX system */
    tx_kernel_enter(); 
    return(0);
}

VOID tx_application_define(void *first_unused_memory)
{

    /* Create a dedicated thread to perform various operations
       on the pixelmap drawing example. These operations simulate 
       user input. */
    gx_validation_control_thread_create(control_thread_entry);

    /* Termiante the test if it runs for more than 100 ticks */
    /* This function is not implemented yet. */
    gx_validation_watchdog_create(100);

    /* Call the actual line example routine. */
    gx_validation_application_define(first_unused_memory);

}


/* Replace the default graphics driver with the validation driver. */
#ifdef win32_graphics_driver_setup_4444argb  
#undef win32_graphics_driver_setup_4444argb
#endif
#define win32_graphics_driver_setup_4444argb  gx_validation_graphics_driver_setup_4444argb


#ifdef WIN32
#undef WIN32
#endif

#include "gx_validation_wrapper.h"

#include "demo_guix_all_widgets_4444argb.c"

typedef struct ARC_PROPS_STRUCT{
INT start_angle;
INT end_angle;
INT brush_width;
INT solid_fill;
INT pixelmap_fill;
INT alpha_mode;
INT compress_mode;
}ARC_PROPS;

ARC_PROPS arc_props[]={
    {0, 90, 1, 0, 0, 0, 0},
    {10, 30, 2, 1, 0, 0, 0},
    {10, 65, 3, 0, 1, 0, 0},
    {100, 115, 5, 0, 1, 0, 1},
    {110, 175, 7, 0, 1, 2, 0},
    {180, 190, 9, 0, 1, 2, 1},
    {185, -115, 10, 1, 0, 0, 0},
    {-10, -55, 12, 1, 0, 0, 0},
    {15, 175, 13, 1, 0, 0, 0},
    {16, 260, 14, 1, 0, 0, 0},
    {25, 335, 15, 1, 0, 0, 0},
    {115, 200, 16, 1, 0, 0, 0},
    {160, 300, 17, 1, 0, 0, 0},
    {210, 355, 18, 1, 0, 0, 0}
};

static INT brush_alpha_value[] = {0, 155, 255};
extern INT draw_shape;
extern INT fill_pixelmap_id;
extern INT brush_width;
extern INT start_angle;
extern INT end_angle;
extern INT radius;
extern GX_UBYTE brush_alpha;
extern GX_BOOL pixelmap_fill;
extern GX_BOOL solid_fill;
char test_comment[256];


/* This thread simulates user input.  Its priority is lower
   than the GUIX thread, so that GUIX finishes an operation 
   before this thread is able to issue the next command. */
static VOID control_thread_entry(ULONG input)
{
INT index, brush_alpha_index;
INT frame_id = 1;

    /* Arc */
    draw_shape = 1;
    ToggleScreen(pShapesScreen, pRotateScreen);

    /* Loop radius from 5 to 200. */
    for(radius = 1; radius < 200; radius += 50)
    {
        for(brush_alpha_index = 0; brush_alpha_index < (INT)(sizeof(brush_alpha_value)/sizeof(INT)); brush_alpha_index++)
        {
           brush_alpha = brush_alpha_value[brush_alpha_index];
    
            for(index = 0; index < (INT)(sizeof(arc_props)/sizeof(ARC_PROPS)); index++)
            {
                start_angle = arc_props[index].start_angle;
                end_angle = arc_props[index].end_angle;

                if(radius < 30)
                {
                    brush_width = 1;
                }
                else
                {
                    brush_width = arc_props[index].brush_width;
                }

                solid_fill = arc_props[index].solid_fill;
                pixelmap_fill = arc_props[index].pixelmap_fill;
                fill_pixelmap_id =  arc_props[index].compress_mode + arc_props[index].alpha_mode;

                /* Set frame id. */
                gx_validation_set_frame_id(frame_id);

                frame_id++;

                /* Set test comments. */
                sprintf(test_comment, "radius:%d, start_angle:%d, end_angle:%d, brush_width %d, solid_fill:%d, pixelmap_fill:%d, alpha:%d, compress: %d, brush_alpha: %d", radius, start_angle, end_angle, brush_width, solid_fill, pixelmap_fill, arc_props[index].alpha_mode, arc_props[index].compress_mode ,brush_alpha);

                gx_validation_set_frame_comment(test_comment);

                /* Mark graphics window dirty. */
                gx_system_dirty_mark(pGraphicsWin);

                /* Force a screen refresh.  */
                gx_validation_screen_refresh();
            }
        }
    }

    /* Signal the end of the test case, Verify the outout.  */
    gx_validation_end();

    exit(0);
}
