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

#ifndef _owfscreen_h
#define _owfscreen_h

/*
 * owfscreen.h
 *
 * These functions connect composition/display to physical screen devices
 */


#include "owftypes.h"
#include "owfimage.h"
#include "owfextensions.h"


#ifdef __cplusplus
extern "C" {
#endif

#define OWF_INVALID_SCREEN_NUMBER   (0)

/*  
 * On-screen context size and color format
 * SEE: /graphics/graphicscomposition/graphicscompositionengine/openwfcmigraion/gceopenwfclayerimpl.h
 * */
#define OWF_SURFACE_WIDTH  (768) /* needs to be > 640 + 2 to allow S60 nHD config to work */
#define OWF_SURFACE_HEIGHT (768) /* needs to be > 640 + 2 to allow S60 nHD config to work */
#define OWF_SURFACE_PIXEL_FORMAT (OWF_IMAGE_XRGB8888)
#define OWF_SURFACE_LINEAR (OWF_FALSE)
#define OWF_SURFACE_ROWPADDING (4)
#define OWF_SURFACE_PREMULTIPLIED (OWF_FALSE)

typedef struct {
    OWFint width;
    OWFint height;
    OWFint stride;
} OWF_ORIENTATION_INFO;

typedef struct {
    OWFuint supportedRotations;
    OWF_ROTATION initialRotation;
    OWF_ROTATION currentRotation;
    OWF_PIXEL_FORMAT pixelFormat;
    OWF_ORIENTATION_INFO normal;
    OWF_ORIENTATION_INFO flipped;
} OWF_SCREEN;

/*!
 * \brief Get systems's default screen
 *
 * \return The screen number of systems default screen
 */
OWF_API_CALL OWFint
OWF_Screen_GetDefaultNumber();

/*!
 * \brief Get screen's dimensions
 *
 * \param dc display context
 * \param header returned screen attributes
 *
 * \return OWF_TRUE if screen exists
 * \return OWF_FALSE if screen does not exist
 */
OWF_API_CALL OWFboolean
OWF_Screen_GetHeader(OWF_DISPCTX dc, OWF_SCREEN* header);

/*!
 * \brief Validate a screen number
 *
 * \param screen screen number
 *
 * \return OWF_TRUE if screen exists
 * \return OWF_FALSE if screen does not exist
 */
OWF_API_CALL OWFboolean
OWF_Screen_Valid(OWFint screen);

/*!
 * \brief Checks if the screen rotation support is enabled
 *
 * \param dc display context
 *
 * \return OWF_TRUE if the screen rotation is enabled
 * \return OWF_FALSE if the screen rotation is enabled
 */
OWF_API_CALL OWFboolean
OWF_Screen_Rotation_Supported(OWF_DISPCTX dc);

/*!
 * \brief Create a new screen
 *
 * \param dc display context
 * \param screen screen number
 *
 * \return OWF_TRUE if screen exists
 * \return OWF_FALSE if screen does not exist
 */
OWF_API_CALL OWFboolean
OWF_Screen_Create(OWFint screen, OWF_DISPCTX dc);

/*!
 * \brief Destroy a screen
 *
 * \param dc display context
 *
 * \return OWF_TRUE if screen exists
 * \return OWF_FALSE if screen does not exist
 */
OWF_API_CALL void
OWF_Screen_Destroy(OWF_DISPCTX dc);

/*!
 * \brief Blit image to screen
 *
 * \param dc display context
 * \param dc stream the stream containing the buffer to be displayed
 * rotation the current context rotation
 *
 * \return OWF_TRUE if screen exists and blitting is  OK
 * \return OWF_FALSE if screen does not exist or blitting is not allowed.
 */
OWF_API_CALL OWFboolean
OWF_Screen_Post_Topmost_Layer(OWF_DISPCTX dc, OWFNativeStreamType stream, OWF_ROTATION rotation);

#ifdef __cplusplus
}
#endif


#endif
