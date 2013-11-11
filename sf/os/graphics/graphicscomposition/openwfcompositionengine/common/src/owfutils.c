/* Copyright (c) 2009 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

 #include "owftypes.h"

#ifdef __cplusplus
extern "C" {
#endif

OWFint min(OWFint aLhs, OWFint aRhs)
{
    if (aLhs < aRhs) {
        return aLhs;
    }
    return aRhs;
}

void OWF_Rect_Set(OWF_RECTANGLE* rect,
                  OWFint left,
                  OWFint top,
                  OWFint width,
                  OWFint height)
{
    if (!rect) {
        return;
    }

    rect->x = left;
    rect->y = top;
    rect->width = width;
    rect->height = height;
}

OWFboolean OWF_Rect_Clip(OWF_RECTANGLE* clipped,
                         OWF_RECTANGLE* rect,
                         OWF_RECTANGLE* bounds)
{
    OWFint dw, dh, dx0, dy0, dx1, dy1; /* destination image coordinates */
    OWFint bl, bt, br, bb;

    bl = bounds->x;
    bt = bounds->y;
    br = bl + bounds->width;
    bb = bt + bounds->height;

    dw = rect->width;
    dh = rect->height;
    dx0 = rect->x;
    dy0 = rect->y;

    /* Crop x coords to target image width */
    if (dx0 < bl) {
        dw = dw - (bl - dx0);
        dx0 = bl;
    }

    /* check if destination rectangle is outside target image */
    if (dx0 >= br) {
        return OWF_FALSE;
    }

    /* Crop y coord to target image height */
    if (dy0 < bt) {
        dh = dh - (bt - dy0);
        dy0 = bt;
    }

    /* check if destination rectangle is outside target image */
    if (dy0 >= bb) {
        return OWF_FALSE;
    }

    /* clamp right edge */
    dx1 = dx0 + dw;
    if (dx1 >= br) {
        dx1 = br;
    } else if (dx1 < bl) {
        return OWF_FALSE;
    }

    /* clamp bottom edge */
    dy1 = dy0 + dh;
    if (dy1 >= bb) {
        dy1 = bb;
    } else if (dy1 < bt) {
        return OWF_FALSE;
    }

    clipped->x = dx0;
    clipped->y = dy0;
    clipped->width = dx1 - dx0;
    clipped->height = dy1 - dy0;

    return OWF_TRUE;
}

#ifdef __cplusplus
}
#endif
