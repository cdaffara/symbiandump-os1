/* Copyright (c) 2009 The Khronos Group Inc.
 * Portions copyright (c) 2009-2010  Nokia Corporation and/or its subsidiary(-ies)
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
/*! \ingroup wfc
 *  \file wfcimageprovider.h
 *
 *  \brief Routines for image providers
 */

#ifndef WFCIMAGEPROVIDER_H_
#define WFCIMAGEPROVIDER_H_

#include <stdio.h>
#include <stdlib.h>

#include "WF/wfc.h"
#include "owfobject.h"
#include "owfimage.h"
#include "owfarray.h"
#include "owfmemory.h"
#include "wfcstructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

OWF_API_CALL WFC_IMAGE_PROVIDER*
WFC_ImageProvider_Create(void* owner, /*WFC_CONTEXT* context,*/
                         OWFNativeStreamType stream,
                         WFC_IMAGE_PROVIDER_TYPE type);

OWF_API_CALL void
WFC_ImageProvider_LockForReading(WFC_IMAGE_PROVIDER* provider);

OWF_API_CALL void
WFC_ImageProvider_Unlock(WFC_IMAGE_PROVIDER* provider);

#ifdef __cplusplus
}
#endif



#endif /* WFCIMAGEPROVIDER_H_ */
