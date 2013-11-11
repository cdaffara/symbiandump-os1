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

#ifndef OWFUTILS_H_
#define OWFUTILS_H_

#include "owftypes.h"

#ifdef __cplusplus
extern "C" {
#endif

OWF_API_CALL OWFint min(OWFint aLhs, OWFint aRhs);

OWF_API_CALL void OWF_Rect_Set(OWF_RECTANGLE* rect,
                         OWFint left,
                         OWFint top,
                         OWFint width,
                         OWFint height);

OWF_API_CALL OWFboolean OWF_Rect_Clip(OWF_RECTANGLE* clipped,
                                      OWF_RECTANGLE* rect,
                                      OWF_RECTANGLE* bounds);


#ifdef __cplusplus
}
#endif

#endif /* UTILS_H_ */
