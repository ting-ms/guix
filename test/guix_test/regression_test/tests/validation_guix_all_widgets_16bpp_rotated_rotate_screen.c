/* This is a small demo of the high-performance GUIX graphics framework. */


#include <stdio.h>
#include "tx_api.h"
#include "gx_api.h"
#include "gx_validation_utility.h"

TEST_PARAM test_parameter = {
    "guix_all_widgets_16bpp_rotated_rotate_screen", /* Test name */
    7, 10, 471, 355  /* Define the coordinates of the capture area.
                         In this test, we only need to capture the pixelmap
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
#ifdef win32_graphics_driver_setup_565rgb_rotated
#undef win32_graphics_driver_setup_565rgb_rotated
#endif
#define win32_graphics_driver_setup_565rgb_rotated  gx_validation_graphics_driver_setup_565rgb_rotated


#ifdef WIN32
#undef WIN32
#endif

#include "gx_validation_wrapper.h"

#include "demo_guix_all_widgets_16bpp_rotated.c"

/*NOTICE: Number of xpos_value and ypos_valus should be same.*/
GX_RESOURCE_ID pixelmap_id_list[]={
    GX_PIXELMAP_ID_ROTATE_APPLE,
    GX_PIXELMAP_ID_ROTATE_FISH
};

extern INT     angle;
extern GX_BOOL direct;
extern INT     rotate_pixelmap_id;

static char test_comment[256];

VOID test_pixelmap_draw(GX_WINDOW *window)
{
INT  xpos;
INT  ypos;
GX_PIXELMAP *pixelmap;
GX_RECTANGLE *size;
GX_PIXELMAP destination;
INT x_axis;
INT y_axis;

    gx_window_background_draw((GX_WINDOW*)window);

    gx_context_pixelmap_get(rotate_pixelmap_id, &pixelmap);

    size = &pRotateWin->gx_widget_size;

    x_axis = 0;
    y_axis = 0;

    if (gx_utility_pixelmap_rotate(pixelmap, 30, &destination, GX_NULL, &y_axis) == GX_SUCCESS)
    {
        xpos = size->gx_rectangle_left;
        ypos = size->gx_rectangle_top;

        /* Draw rotated pixelmap.  */
        gx_canvas_pixelmap_draw(xpos, ypos, &destination);

        /* free memory of the rotated pixelmap */
        if (destination.gx_pixelmap_data)
        {
            dynamic_memory_free((VOID *)destination.gx_pixelmap_data);
        }
        if (destination.gx_pixelmap_aux_data)
        {
            dynamic_memory_free((VOID *)destination.gx_pixelmap_aux_data);
        }
    }

    if (gx_utility_pixelmap_rotate(pixelmap, 30, &destination, &x_axis, GX_NULL) == GX_SUCCESS)
    {
        xpos = size->gx_rectangle_right - destination.gx_pixelmap_width;
        ypos = size->gx_rectangle_bottom - destination.gx_pixelmap_height;

        /* Draw rotated pixelmap.  */
        gx_canvas_pixelmap_draw(xpos, ypos, &destination);

        /* free memory of the rotated pixelmap */
        if (destination.gx_pixelmap_data)
        {
            dynamic_memory_free((VOID *)destination.gx_pixelmap_data);
        }
        if (destination.gx_pixelmap_aux_data)
        {
            dynamic_memory_free((VOID *)destination.gx_pixelmap_aux_data);
        }
    }
}

/* This thread simulates user input.  Its priority is lower
   than the GUIX thread, so that GUIX finishes an operation 
   before this thread is able to issue the next command. */
static VOID control_thread_entry(ULONG input)
{
int frame_id = 1;
int id, index;

    ToggleScreen((GX_WINDOW *)&rotate_screen, pButtonScreen);
    for(id = 0; id < 2; id++)
    {
        rotate_pixelmap_id = pixelmap_id_list[id];

        for(angle = 1; angle < 360; angle++)
        {
            /* Inform the validation system 
            (1) Frame ID, which identifies a specific test configuration;
            (2) Start recording frame on the next toggle operation.
             */
            for(index = 0; index < 2; index++)
            {
                if(index)
                {
                    direct = GX_TRUE;
                }
                else
                {
                    direct = GX_FALSE;
                }
                gx_validation_set_frame_id(frame_id++);
                sprintf(test_comment, "canvas rotate: %d, angle: %d, pixelmap_id: %d. ", direct, angle, rotate_pixelmap_id);
                gx_validation_set_frame_comment(test_comment);

                gx_system_dirty_mark((GX_WIDGET *)&rotate_screen.rotate_screen_pixelmap_window);

                /* Force a screen refresh. */
                gx_validation_screen_refresh();
            }
        }
    }

    gx_widget_draw_set((GX_WIDGET *)&rotate_screen, test_pixelmap_draw);
    gx_validation_set_frame_id(frame_id++);
    gx_validation_set_frame_comment("Test utility pixelmap rotate with different rotation center.");
    gx_validation_screen_refresh();

    direct = GX_TRUE;
    angle = 30;

    for(index = 0; index < 3; index++)
    {
        switch(index)
        {
        case 0:
            rotate_pixelmap_id = GX_PIXELMAP_ID_BLACK_PAUSE_ALPHAMAP;
            sprintf(test_comment, "set alphamap");
            break;

        case 1:
            root->gx_window_root_canvas->gx_canvas_display->gx_display_driver_pixel_blend = GX_NULL;
            rotate_pixelmap_id = pixelmap_id_list[0];
            sprintf(test_comment, "set pixelmap to apple, set driver pixel blend null");
            break;

        case 2:
            rotate_pixelmap_id = pixelmap_id_list[1];
            sprintf(test_comment, "set pixelmap to fish, set driver pixel blend null");
            break;
        }

        gx_validation_set_frame_id(frame_id++);
        gx_validation_set_frame_comment(test_comment);
        gx_validation_screen_refresh();
    }
   
    /* Signal the end of the test case.  Verify the output. */
    gx_validation_end();

    exit(0);
}





