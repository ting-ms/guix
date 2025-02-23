/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** GUIX Component                                                        */
/**                                                                       */
/**   Horizontal List (List)                                              */
/**                                                                       */
/**************************************************************************/

#define GX_SOURCE_CODE


/* Include necessary system files.  */

#include "gx_api.h"
#include "gx_widget.h"
#include "gx_window.h"
#include "gx_system.h"
#include "gx_utility.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_horizontal_list_right_wrap                      PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function scrolls down the horizontal list.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    list                                  horizontal list widget control*/
/*                                            block                       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_last_client_child_get             Get the last client child     */
/*    _gx_first_client_child_get            Get the first client child    */
/*    _gx_widget_back_attach                Attach a widget to the back   */
/*    _gx_widget_shift                      Relocate a widget             */
/*    [gx_horizontal_list_callback]         Horizontal list callback      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _gx_horizontal_list_scroll                                          */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Kenneth Maxwell          Initial Version 6.0           */
/*  09-30-2020     Kenneth Maxwell          Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
VOID _gx_horizontal_list_right_wrap(GX_HORIZONTAL_LIST *list)
{
GX_WIDGET   *test;
GX_WIDGET   *check;
GX_BOOL      moving = GX_TRUE;
GX_RECTANGLE newpos;

    while (moving)
    {
        moving = GX_FALSE;

        if ((list -> gx_horizontal_list_top_index > 0) ||
            (list -> gx_widget_style & GX_STYLE_WRAP))
        {
            /* scrolling down, see if my bottom widget can be moved to the top: */
            test = _gx_widget_last_client_child_get((GX_WIDGET *)list);

            if (test)
            {
                if (test -> gx_widget_size.gx_rectangle_left > list -> gx_widget_size.gx_rectangle_right)
                {
                    /* right widget is to the right of my client area, move it to the top: */
                    moving = GX_TRUE;
                    list -> gx_horizontal_list_top_index--;

                    /* Wrap index. */
                    if (list -> gx_horizontal_list_top_index < 0)
                    {
                        list -> gx_horizontal_list_top_index = list -> gx_horizontal_list_total_columns - 1;
                    }

                    check = _gx_widget_first_client_child_get((GX_WIDGET *)list);

                    if (check)
                    {
                        _gx_widget_detach(test);
                        newpos = test -> gx_widget_size;
                        _gx_utility_rectangle_shift(&newpos,
                                                   (GX_VALUE)(-(newpos.gx_rectangle_right -
                                                   check -> gx_widget_size.gx_rectangle_left + 1)), 0);
                        _gx_widget_resize(test, &newpos);

                        list -> gx_horizontal_list_callback(list, test, list -> gx_horizontal_list_top_index);
                        if (list -> gx_horizontal_list_selected == list -> gx_horizontal_list_top_index)
                        {
                            test -> gx_widget_style |=  GX_STYLE_DRAW_SELECTED;
                        }
                        else
                        {
                            test -> gx_widget_style &= ~GX_STYLE_DRAW_SELECTED;
                        }
                        _gx_widget_back_attach((GX_WIDGET *)list, test);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}

