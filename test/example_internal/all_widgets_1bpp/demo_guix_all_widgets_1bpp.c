/* This is a small demo of the high-performance GUIX graphics framework. */

#include <stdio.h>
#include "gx_api.h"
#include "all_widgets_1bpp_resources.h"
#include "all_widgets_1bpp_specifications.h"

#define DROP_LIST_VISIBLE_ROWS 5

GX_WINDOW *pTextScreen;
GX_WINDOW *pWindowScreen;
GX_WINDOW *pButtonScreen;
GX_WINDOW *pIndicatorScreen;
GX_WINDOW *pShapesScreen;
GX_WINDOW *pGraphicsWin;
GX_WINDOW *pSpriteScreen;
GX_WINDOW *pRotateScreen;
GX_WINDOW *pPicScreen;
GX_WINDOW *pRotateWin;
GX_WINDOW *pScrollWheelScreen;
GX_SCROLLBAR list_scroll;

extern MENU_SCREEN_CONTROL_BLOCK menu_screen;

GX_BOOL            day_wheel_selected_row_changed = GX_FALSE;/* Used for testing*/
GX_BOOL            month_wheel_selected_row_changed = GX_FALSE;/* Used for testing*/

/* Define the ThreadX demo thread control block and stack.  */

TX_THREAD          demo_thread;
UCHAR              demo_thread_stack[4096];
TX_BYTE_POOL       rotate_pool;

GX_WINDOW          NestedWindow;
GX_WINDOW_ROOT     nested_root;

#define           NESTED_WINDOW_WIDTH  94
#define           NESTED_WINDOW_HEIGHT  73
#define           NUMBER_ANIMATION_TIMER 2

GX_WINDOW_ROOT    *root;

#define            SCRATCHPAD_PIXELS (MAIN_DISPLAY_X_RESOLUTION * MAIN_DISPLAY_X_RESOLUTION)
GX_UBYTE           scratchpad[SCRATCHPAD_PIXELS];
GX_UBYTE           composite_canvas_memory[SCRATCHPAD_PIXELS];
GX_UBYTE           animation_canvas_memory[NESTED_WINDOW_WIDTH * NESTED_WINDOW_HEIGHT];

GX_CANVAS          composite_canvas;
GX_CANVAS          animation_canvas;

GX_WINDOW         *pic_window;
INT                compress = 0;
INT                transparency = 0;
GX_RESOURCE_ID     pixelmap_id[4] = {GX_PIXELMAP_ID_RAW, GX_PIXELMAP_ID_COMPRESSED, GX_PIXELMAP_ID_TRANSPARENT, GX_PIXELMAP_ID_COMPRESSED_TRANSPARENT};
INT                numeric_prompt_value = 10000;


extern GX_STUDIO_DISPLAY_INFO all_widgets_1bpp_display_table[];

typedef struct {
    GX_PROMPT prompt;
    CHAR text[80];
} DROP_LIST_WIDGET;

DROP_LIST_WIDGET drop_list_widgets[DROP_LIST_VISIBLE_ROWS + 1];

/* Define prototypes.   */
VOID        PopulateDropList();
VOID        demo_thread_entry(ULONG thread_input);
VOID        memory_free(VOID *mem);
extern UINT win32_graphics_driver_setup_monochrome(GX_DISPLAY *display);

int main(int argc, char ** argv)
{
    tx_kernel_enter();
    return(0);
}

VOID *memory_allocate(ULONG size)
{
    VOID *memptr;

    if (tx_byte_allocate(&rotate_pool, &memptr, size, TX_NO_WAIT) == TX_SUCCESS)
    {
        return memptr;
    }

    return NULL;
}

VOID memory_free(VOID *mem)
{
    tx_byte_release(mem);
}

VOID tx_application_define(void *first_unused_memory)
{

    /* Create the main demo thread.  */
    tx_thread_create(&demo_thread, "GUIX Demo Thread", demo_thread_entry,
        0, demo_thread_stack, sizeof(demo_thread_stack),
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

}


VOID  demo_thread_entry(ULONG thread_input)
{

    GX_RECTANGLE winsize;
    tx_byte_pool_create(&rotate_pool, "scratchpad", scratchpad,
        SCRATCHPAD_PIXELS * sizeof(GX_UBYTE));
    /* Initialize GUIX.  */
    gx_system_initialize();

    gx_system_memory_allocator_set(memory_allocate, memory_free);

    gx_studio_display_configure(MAIN_DISPLAY, win32_graphics_driver_setup_monochrome,
        LANGUAGE_ENGLISH, MAIN_DISPLAY_THEME_1, &root);

    /* create the button screen */
    gx_studio_named_widget_create("button_screen", GX_NULL, (GX_WIDGET **)&pButtonScreen);

    /* create the text widget screen */
    gx_studio_named_widget_create("text_screen", GX_NULL, (GX_WIDGET **)&pTextScreen);

    /* create the windows screen */
    gx_studio_named_widget_create("window_screen", GX_NULL, (GX_WIDGET **)&pWindowScreen);
    PopulateDropList();

    /* create the indicator screen */
    gx_studio_named_widget_create("indicator_screen", GX_NULL, (GX_WIDGET **)&pIndicatorScreen);

    /* create the shapes screen */
    gx_studio_named_widget_create("shapes_screen", GX_NULL, (GX_WIDGET **)&pShapesScreen);

    /* create the pic screen */
    gx_studio_named_widget_create("pic_screen", GX_NULL, (GX_WIDGET **)&pPicScreen);
    pic_window = &(((PIC_SCREEN_CONTROL_BLOCK *)pPicScreen)->pic_screen_pic_window);

    /* create the sprite screen */
    gx_studio_named_widget_create("sprite_screen", (GX_WIDGET *)root, (GX_WIDGET **)&pSpriteScreen);

    /* create the rotate screen */
    gx_studio_named_widget_create("rotate_screen", GX_NULL, (GX_WIDGET **)&pRotateScreen);
    pRotateWin = &(((ROTATE_SCREEN_CONTROL_BLOCK *)pRotateScreen)->rotate_screen_pixelmap_window);

    /* create menu screen */
    gx_studio_named_widget_create("menu_screen", GX_NULL, GX_NULL);

    /* create image convert screen */
    gx_studio_named_widget_create("image_convert_screen", GX_NULL, GX_NULL);

    gx_studio_named_widget_create("resize_screen", GX_NULL, GX_NULL);

    /* Show the root window to make it and patients screen visible.  */
    gx_widget_show(root);

    /* create the composite canvas used during animations */
    gx_canvas_create(&composite_canvas, GX_NULL,
        all_widgets_1bpp_display_table[0].display,
        GX_CANVAS_COMPOSITE,
        root->gx_widget_size.gx_rectangle_right - root->gx_widget_size.gx_rectangle_left + 1,
        root->gx_widget_size.gx_rectangle_bottom - root->gx_widget_size.gx_rectangle_top + 1,
        (GX_COLOR *)composite_canvas_memory, SCRATCHPAD_PIXELS * sizeof(GX_UBYTE));
    gx_canvas_alpha_set(&composite_canvas, 255);

    /* create the nested window shown on the window screen */

    winsize.gx_rectangle_left = winsize.gx_rectangle_top = 0;
    winsize.gx_rectangle_right = NESTED_WINDOW_WIDTH - 1;
    winsize.gx_rectangle_bottom = NESTED_WINDOW_HEIGHT - 1;
    gx_window_create(&NestedWindow, "Nested_window", GX_NULL, GX_STYLE_BORDER_THIN, 0, &winsize);
    gx_widget_fill_color_set(&NestedWindow, GX_COLOR_ID_WHITE, GX_COLOR_ID_WHITE, GX_COLOR_ID_WHITE);

    /* create the root window used for window blend animation */
    gx_window_root_create(&nested_root, "nested_root", &animation_canvas, GX_STYLE_BORDER_NONE, GX_NULL, &NestedWindow.gx_widget_size);
    gx_widget_attach((GX_WIDGET *)&nested_root, (GX_WIDGET *)&NestedWindow);


    /* let GUIX run */
    gx_system_start();
}

void ToggleScreen(GX_WINDOW *new_win, GX_WINDOW *old_win)
{
    if (!new_win->gx_widget_parent)
    {
        gx_widget_attach(root, (GX_WIDGET *)new_win);
    }
    else
    {
        gx_widget_show((GX_WIDGET *)new_win);
    }
    gx_widget_hide((GX_WIDGET *)old_win);
}

UINT next_button_handler(GX_WINDOW *window, GX_EVENT *event_ptr)
{
    switch (event_ptr->gx_event_type)
    {
    case GX_EVENT_SHOW:
        gx_window_event_process(window, event_ptr);
        if (window->gx_widget_id == ID_WINDOW_SCREEN)
        {
            /* Create new canvas to save the semi-transparent window,
            Then set alpha to canvas and shift it.                      */
            gx_canvas_create(&animation_canvas, GX_NULL,
                all_widgets_1bpp_display_table[0].display,
                GX_CANVAS_MANAGED_VISIBLE,
                NESTED_WINDOW_WIDTH, NESTED_WINDOW_HEIGHT,
                (GX_COLOR *)animation_canvas_memory, NESTED_WINDOW_WIDTH * NESTED_WINDOW_HEIGHT * sizeof(GX_UBYTE));
            gx_canvas_offset_set(&animation_canvas, 90, 75);
            gx_widget_show((GX_WIDGET *)&nested_root);

            gx_window_client_scroll(&window_screen.window_screen_scroll_frame_1, 1, 1);
        }
        else if (window->gx_widget_id == ID_TEXT_SCREEN)
        {
            gx_system_timer_start(pTextScreen, NUMBER_ANIMATION_TIMER, 20, 20);
        }
        break;

    case GX_EVENT_HIDE:
        if (window->gx_widget_id == ID_WINDOW_SCREEN)
        {
            gx_widget_hide((GX_WIDGET *)&nested_root);
            gx_canvas_delete(&animation_canvas);
        }
        else if (window->gx_widget_id == ID_TEXT_SCREEN)
        {
            gx_system_timer_stop(window, NUMBER_ANIMATION_TIMER);
        }
        gx_window_event_process(window, event_ptr);
        break;

    case GX_SIGNAL(IDB_NEXT, GX_EVENT_CLICKED):
        switch (window->gx_widget_id)
        {
        case ID_BUTTON_SCREEN:
            /* we are on button window, switch to window */
            ToggleScreen(pWindowScreen, window);
            break;

        case ID_WINDOW_SCREEN:
            ToggleScreen(pTextScreen, window);
            break;

        case ID_TEXT_SCREEN:
            ToggleScreen(pIndicatorScreen, window);
            break;

        case ID_INDICATOR_SCREEN:
            ToggleScreen(pShapesScreen, window);
            break;

        case ID_SHAPES_SCREEN:
            ToggleScreen(pPicScreen, window);
            break;

        case ID_PIC_SCREEN:
            ToggleScreen(pSpriteScreen, window);
            break;

        case ID_SPRITE_SCREEN:
            gx_studio_named_widget_create("scroll_wheel_screen", GX_NULL, (GX_WIDGET **)&pScrollWheelScreen);
            ToggleScreen((GX_WINDOW *)pScrollWheelScreen, window);
            break;

        case ID_SCROLL_WHEEL_SCREEN:
            ToggleScreen((GX_WINDOW *)&menu_screen, window);
            gx_tree_view_selected_set(&menu_screen.menu_screen_tree_view, (GX_WIDGET *)&menu_screen.menu_screen_tree_menu_2_2_1);
            gx_widget_delete(window);
            break;

        case ID_MENU_SCREEN:
            ToggleScreen(pRotateScreen, window);
            break;

        case ID_ROTATE_SCREEN:
            ToggleScreen((GX_WINDOW *)&image_convert_screen, window);
            break;

        case ID_IMAGE_CONVERT_SCREEN:
            ToggleScreen((GX_WINDOW *)&resize_screen, window);
            break;

        case ID_RESIZE_SCREEN:
            ToggleScreen(pButtonScreen, window);
            break;
        }
        break;

    case GX_EVENT_TIMER:
        if (event_ptr->gx_event_payload.gx_event_timer_id == NUMBER_ANIMATION_TIMER)
        {
            numeric_prompt_value++;

            if (numeric_prompt_value > 19999)
            {
                numeric_prompt_value = 10000;
            }
            gx_numeric_prompt_value_set(&text_screen.text_screen_numeric_prompt, numeric_prompt_value);
            gx_numeric_pixelmap_prompt_value_set(&text_screen.text_screen_numeric_pixelmap_prompt, numeric_prompt_value);
        }
        break;

    default:
        return gx_window_event_process(window, event_ptr);
    }
    return 0;
}

UINT pic_screen_handler(GX_WINDOW *window, GX_EVENT *event_ptr)
{
    switch (event_ptr->gx_event_type)
    {
    case GX_SIGNAL(ID_TRANSPARENCY, GX_EVENT_TOGGLE_ON):
        transparency = 2;
        gx_system_dirty_mark(pic_window);
        break;

    case GX_SIGNAL(ID_TRANSPARENCY, GX_EVENT_TOGGLE_OFF):
        transparency = 0;
        gx_system_dirty_mark(pic_window);
        break;

    case GX_SIGNAL(ID_COMPRESS, GX_EVENT_TOGGLE_ON):
        compress = 1;
        gx_system_dirty_mark(pic_window);
        break;

    case GX_SIGNAL(ID_COMPRESS, GX_EVENT_TOGGLE_OFF):
        compress = 0;
        gx_system_dirty_mark(pic_window);
        break;

    default:
        return next_button_handler(window, event_ptr);
        break;
    }
    gx_window_wallpaper_set(pic_window, pixelmap_id[transparency + compress], GX_FALSE);
    return 0;
}

VOID drop_list_row_create(GX_VERTICAL_LIST *list, GX_WIDGET *widget, INT index)
{
    GX_BOOL created;
    GX_RECTANGLE size;
    GX_CHAR temp[10];
    GX_STRING string;

    DROP_LIST_WIDGET *entry = (DROP_LIST_WIDGET *)widget;

    GX_STRCPY(entry->text, "List Entry #");
    gx_utility_ltoa(index + 1, temp, 10);
    GX_STRCAT(entry->text, (char*)temp);

    gx_widget_created_test(widget, &created);

    if (!created)
    {
        gx_utility_rectangle_define(&size, 0, 0, 100, 30);
        gx_prompt_create(&entry->prompt, entry->text, list, 0, GX_STYLE_ENABLED | GX_STYLE_TEXT_LEFT | GX_STYLE_BORDER_NONE, 0, &size);
        gx_widget_fill_color_set(&entry->prompt, GX_COLOR_ID_BLACK, GX_COLOR_ID_BLUE, GX_COLOR_ID_BLACK);
        gx_prompt_text_color_set(&entry->prompt, GX_COLOR_ID_WHITE, GX_COLOR_ID_WHITE, GX_COLOR_ID_WHITE);
    }

    string.gx_string_ptr = entry->text;
    string.gx_string_length = strnlen(string.gx_string_ptr, sizeof(entry->text));
    gx_prompt_text_set_ext(&entry->prompt, &string);
}

VOID PopulateDropList(void)
{
    int index;
    GX_VERTICAL_LIST *list;

    WINDOW_SCREEN_CONTROL_BLOCK *control = (WINDOW_SCREEN_CONTROL_BLOCK *)pWindowScreen;
    GX_DROP_LIST *drop = &control->window_screen_drop_list;
    gx_drop_list_popup_get(drop, &list);

    for (index = 0; index < DROP_LIST_VISIBLE_ROWS; index++)
    {
        drop_list_row_create(list, (GX_WIDGET *)&drop_list_widgets[index], index);
    }

    gx_vertical_scrollbar_create(&list_scroll, "list_scroll", list, GX_NULL,
        GX_SCROLLBAR_VERTICAL | GX_SCROLLBAR_RELATIVE_THUMB | GX_SCROLLBAR_END_BUTTONS | GX_STYLE_ENABLED);

}

VOID format_func(GX_NUMERIC_PROMPT *prompt, INT value)
{
    gx_utility_ltoa(value % 1000, prompt->gx_numeric_prompt_buffer, GX_NUMERIC_PROMPT_BUFFER_SIZE);
}

VOID numeric_pixelmap_format_func(GX_NUMERIC_PIXELMAP_PROMPT *prompt, INT value)
{
    gx_utility_ltoa(value % 1000, prompt->gx_numeric_pixelmap_prompt_buffer, GX_NUMERIC_PROMPT_BUFFER_SIZE);
}