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

#ifndef OWFIMAGE_H_
#define OWFIMAGE_H_

#include "owftypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void*   OWF_DISPCTX;


#undef USE_FLOAT_PIXEL

/*
 * This is and always should be the only place where USE_FLOAT_PIXEL is 
 * defined so if #define USE_FLOAT_PIXEL is absent in owfimage.h then it
 * can be assumed it is not defined elsewhere. 
 */ 
//#define USE_FLOAT_PIXEL

/* --
* internal pixel format
*/
#ifdef USE_FLOAT_PIXEL
#define OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT

typedef OWFfloat            OWFsubpixel; /* subpixel representation */

#define OWF_RED_MIN_VALUE   0.0f
#define OWF_RED_MAX_VALUE   1.0f
#define OWF_GREEN_MIN_VALUE 0.0f
#define OWF_GREEN_MAX_VALUE 1.0f
#define OWF_BLUE_MIN_VALUE  0.0f
#define OWF_BLUE_MAX_VALUE  1.0f
#define OWF_ALPHA_MIN_VALUE 0.0f
#define OWF_ALPHA_MAX_VALUE 1.0f

#define OWF_FULLY_OPAQUE        OWF_ALPHA_MAX_VALUE
#define OWF_FULLY_TRANSPARENT   OWF_ALPHA_MIN_VALUE

#define OWF_BYTE_MAX_VALUE  255.0f
#define OWF_BILINEAR_ROUNDING_VALUE 0.0f
#define OWF_BLEND_ROUNDING_VALUE 0.0f
#define OWF_PREMUL_ROUNDING_FACTOR  0.0f
#define OWF_SOURCE_CONVERSION_ROUNDING_VALUE 0.0f

#else
#undef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT

typedef OWFuint8            OWFsubpixel; /* subpixel representation */

#define OWF_RED_MIN_VALUE         0
#define OWF_RED_MAX_VALUE       255
#define OWF_GREEN_MIN_VALUE       0
#define OWF_GREEN_MAX_VALUE     255
#define OWF_BLUE_MIN_VALUE        0
#define OWF_BLUE_MAX_VALUE      255
#define OWF_ALPHA_MIN_VALUE       0
#define OWF_ALPHA_MAX_VALUE     255
#define OWF_FULLY_OPAQUE        OWF_ALPHA_MAX_VALUE
#define OWF_FULLY_TRANSPARENT   OWF_ALPHA_MIN_VALUE

#define OWF_BYTE_MAX_VALUE      255

#define OWF_BILINEAR_ROUNDING_VALUE 0.5f
#define OWF_SOURCE_CONVERSION_ROUNDING_VALUE 0.5f
#define OWF_BLEND_ROUNDING_VALUE (OWF_ALPHA_MAX_VALUE/2)
#define OWF_PREMUL_ROUNDING_FACTOR (OWF_ALPHA_MAX_VALUE/2)

#endif

/*
 * Byte order of different color formats
 * these are used when converting from/to
 * internal format
 */

#define ARGB8888_ALPHA_MASK         0xFF000000
#define ARGB8888_RED_MASK           0x00FF0000
#define ARGB8888_GREEN_MASK         0x0000FF00
#define ARGB8888_BLUE_MASK          0x000000FF
#define ARGB8888_ALPHA_SHIFT        24
#define ARGB8888_RED_SHIFT          16
#define ARGB8888_GREEN_SHIFT        8
#define ARGB8888_BLUE_SHIFT         0

#define RGB565_ALPHA_MASK   0xFFFF
#define RGB565_RED_MASK     0xF800
#define RGB565_GREEN_MASK   0x07E0
#define RGB565_BLUE_MASK    0x001F

/* These are used when converting from RGB565 to ARGB8888. */
#define RGB565_ALPHA_SHIFT  16
#define RGB565_RED_SHIFT    11
#define RGB565_GREEN_SHIFT  5

/* subpixels per pixel */
#define OWF_PIXEL_SIZE      4

/* subpixel in bytes */
#define OWF_SUBPIXEL_SIZE   sizeof(OWFsubpixel)
#define OWF_BYTES_PER_PIXEL (OWF_PIXEL_SIZE * OWF_SUBPIXEL_SIZE)

#pragma pack(push, 1)
typedef union {
    struct {
        OWFsubpixel         blue;
        OWFsubpixel         green;
        OWFsubpixel         red;
        OWFsubpixel         alpha;
    } color;
    OWFsubpixel             subpixel[OWF_PIXEL_SIZE];
    OWFuint8                pixelbytes[OWF_BYTES_PER_PIXEL];
} OWFpixel;
#pragma pack(pop)

/* -- */

/* filters used in OWF_Image_Stretch */
typedef enum {
    OWF_FILTER_POINT_SAMPLING,  /* nearest pixel */
    OWF_FILTER_BILINEAR        /* nearest 4 */
} OWF_FILTERING;

typedef struct {
    OWFint                  width;
    OWFint                  height;
    OWFint                  stride; /* number of bytes per line */
    OWFint                  pixelSize; /* pixel size in bytes */
    OWF_IMAGE_FORMAT        format;
    OWFboolean              foreign;
    OWFint                  dataMax; /* data buffer max size */
    void*                   data;
} OWF_IMAGE;

/* This typedef denotes an owned OWF_IMAGE, as opposed to a temporary association.
 * Owned instances must be destroyed when the containing object is destroyed.
 */
typedef OWF_IMAGE* OWF_IMAGE_INST;

typedef enum {
    OWF_FLIP_NONE,
    OWF_FLIP_VERTICALLY     = 1,
    OWF_FLIP_HORIZONTALLY   = 2
} OWF_FLIP_DIRECTION;

typedef enum {
    OWF_ROTATION_0          = 0,
    OWF_ROTATION_90         = 90,
    OWF_ROTATION_180        = 180,
    OWF_ROTATION_270        = 270
} OWF_ROTATION;

typedef enum {
    OWF_TRANSPARENCY_NONE,
    OWF_TRANSPARENCY_GLOBAL_ALPHA   = (1 << 0),
    OWF_TRANSPARENCY_SOURCE_ALPHA   = (1 << 1),
    OWF_TRANSPARENCY_MASK           = (1 << 2),
    OWF_TRANSPARENCY_COLOR_KEY      = (1 << 3)
} OWF_TRANSPARENCY;

typedef struct _OWF_BLEND_INFO {
    struct {
        OWF_IMAGE*          image;
        OWF_RECTANGLE*      rectangle;
    } destination;

    struct {
        OWF_IMAGE*          image;
        OWF_RECTANGLE*      rectangle;
    } source;

    OWF_IMAGE*              mask;
    OWFsubpixel             globalAlpha;
} OWF_BLEND_INFO;


/*!---------------------------------------------------------------------------
 *  \brief Initialize image object
 *
 *  \param image            Image object to initialize
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Init(OWF_IMAGE* image);

/*!---------------------------------------------------------------------------
 *  \brief Creates new reference counted image object
 *
 *  \param width            Image width (in pixels)
 *  \param height           Image height (in pixels)
 *  \param format           Image format (\see OWF_IMAGE_FORMAT)
 *  \param buffer           Pointer to image pixel data. If NULL, then a new
 *                          buffer is allocated for storing image pixel data.
 *                          Allocated buffer is owned by the image. If non-NULL,
 *                          then the image will be a "foreign" one, that doesn't
 *                          own the pixel data but merely uses it.
 *  \param minimumStride    Minimum number of bytes per scanline (may be zero)
 *
 *
 *  \return New image object or NULL if error occured.
 *----------------------------------------------------------------------------*/
OWF_PUBLIC OWF_IMAGE*
OWF_Image_Create(OWFint width,
                 OWFint height,
                 const OWF_IMAGE_FORMAT* format,
                 void* buffer,
                 OWFint minimumStride);

/*!---------------------------------------------------------------------------
 *  \brief Destroy image object. Rather than actually destroying the image
 *  immediately, this decrements image's reference count by one and once the
 *  counter reaches zero, the actual deletion will occur.
 *
 *  \param image            Image to destroy
 *----------------------------------------------------------------------------*/
OWF_PUBLIC void
OWF_Image_Destroy(OWF_IMAGE* image);

/*!---------------------------------------------------------------------------
 *  \brief Create a pixel-perfect copy (clone) of the image. The copy will be
 *  totally independent from the original image, i.e. doesn't share pixel
 *  buffers or anything.
 *
 *  \param image            Image to copy
 *
 *  \param Copy of the image or NULL if error occured.
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWF_IMAGE*
OWF_Image_Copy(const OWF_IMAGE* image);

/*!---------------------------------------------------------------------------
 *  \brief Set image size. This doesn't modify the pixel data in anyway,
 *  merely just changes the image header. Mostly used for (recycling) foreign
 *  images that point to external pixel buffers.
 *
 *  If the new pixel count (width * height) exceeds
 *  current values, the pixel buffer will NOT be resized, but the call will FAIL.
 *
 *  \param image            Image to resize
 *  \param width            New width of the image
 *  \param height           New height of the image
 *
 *
 *  \return Boolean value indicating success of the operation. In case of
 *  failure, no modifications are done to original image.
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_SetSize(OWF_IMAGE* image,
                  OWFint width,
                  OWFint height);

/*!---------------------------------------------------------------------------
 *  \brief Set internal mode flags. This doesn't modify the pixel data in anyway,
 *  merely just changes the image header. Mostly used for (recycling) foreign
 *  images that point to external pixel buffers.
 *
 *
 *  \param image            Image to resize
 *  \param premultiply      Image data is premultiplied
 *  \param linear           Image colour data is linear
 *
 *
 *  \return Boolean value indicating success of the operation. In case of
 *  failure, no modifications are done to original image.
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_SetFlags(OWF_IMAGE* image,
                  OWFboolean premultiply,
                  OWFboolean linear);

/*!---------------------------------------------------------------------------
 *  \brief Set the pixel buffer to an alternate location.
 *  All other parameters remain the same.
 *
 *  \param image            Image to resize
 *  \param buffer           Image data source to start using
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_SetPixelBuffer(OWF_IMAGE* image,      void* buffer);


/*!---------------------------------------------------------------------------
 *  \brief Blit (1:1 copy) pixels from image to another w/ clipping.
 *
 *  \param dst              Destination image
 *  \param dstRect          Destination rectangle
 *  \param src              Source image
 *  \param srcRect          Source rectangle
 *
 *  \return Boolean value indicating whether pixels were copied or not. If not,
 *  it means that either of the rectangles is outside its respective image's
 *  bounds.
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_Blit(OWF_IMAGE* dst,
               OWF_RECTANGLE const* dstRect,
               OWF_IMAGE const* src,
               OWF_RECTANGLE const* srcRect);

/*!---------------------------------------------------------------------------
 *  \brief Stretch-blit (scaled copy) pixels from image to another w/ clipping.
 *
 *  \param dst              Destination image
 *  \param dstRect          Destination rectangle
 *  \param src              Source image
 *  \param srcRect          Source rectangle
 *
 *  \return Boolean value indicating whether pixels were copied or not. If not,
 *  it means that either of the rectangles is outside its respective image's
 *  bounds.
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_Stretch(OWF_IMAGE* dst,
                  OWF_RECTANGLE* dstRect,
                  OWF_IMAGE* src,
                  OWFfloat* srcRect,
                  OWF_FILTERING filter);

/*!---------------------------------------------------------------------------
 *  \brief Multiply pixels' alpha value into rgb-color components.
 *  Multiplies only if image source image is non-premultiplied.
 *  \param image            Image to convert to pre-multiplied domain.
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_PremultiplyAlpha(OWF_IMAGE* image);

/*!---------------------------------------------------------------------------
 *  \brief Divide pixels' rgb-color components by its alpha value.
 *
 *  \param image            Image to convert to nonpre-multiplied domain.
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_UnpremultiplyAlpha(OWF_IMAGE* image);

/*!---------------------------------------------------------------------------
 *  \brief Linearizes image pixel data
 *
 *  \param image            Image to convert to linear domain
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_LinearizeData(OWF_IMAGE* image);

/*!---------------------------------------------------------------------------
 *  \brief Non-linearizes image pixel data
 *
 *  \param image            Image to convert to non-linear domain
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_NonLinearizeData(OWF_IMAGE* image);

/*!---------------------------------------------------------------------------
 *  \brief Apply gamma correction to image pixel values
 *
 *  \param image            Image to operate on
 *  \param gamma            Gamma value
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Gamma(OWF_IMAGE* image, OWFfloat gamma);

/*!---------------------------------------------------------------------------
 *  \brief Flip (mirror) image about one or both of its center axes.
 *
 *  \param image            Image to flip
 *  \param dir              Flip direction. Valid values are
 *                          OWF_FLIP_NONE, OWF_FLIP_HORIZONTALLY,
 *                          OWF_FLIP_VERTICALLY or any bitwise-or combination
 *                          of the former.
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Flip(OWF_IMAGE* image,
               OWF_FLIP_DIRECTION dir);

/*!---------------------------------------------------------------------------
 *  \brief Rotate image n*90 degrees about its center
 *
 *  \param dst              Result (rotated) image.
 *  \param src              Source image.
 *  \param rotation         Rotation angle (OWF_ROTATION_0, OWF_ROTATION_90,
 *                          OWF_ROTATION_180, or OWF_ROTATION_270)
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Rotate(OWF_IMAGE* dst,
                 OWF_IMAGE* src,
                 OWF_ROTATION rotation);

/*!---------------------------------------------------------------------------
 *  \brief
 *
 *  \param
 *
 *  \return
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Blend(OWF_BLEND_INFO* blend,
                OWF_TRANSPARENCY transparency);

/*!---------------------------------------------------------------------------
 *  \brief
 *
 *  \param
 *
 *  \return
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Clear(OWF_IMAGE* image,
                OWFsubpixel red,
                OWFsubpixel green,
                OWFsubpixel blue,
                OWFsubpixel alpha);

/*!---------------------------------------------------------------------------
 *  \brief Convert image data from internal color format to destination format
 *
 *  \param dst
 *  \param src
 *
 *  \return
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_DestinationFormatConversion(OWF_IMAGE* dst,
                                      OWF_IMAGE* src);
/*!---------------------------------------------------------------------------
 *  \brief Test whether it will be possible to convert to destination format.
 *  Note, no IMAGE object yet extists. This is effectively a "static" member method.
 *  \param format proposed destination format
 *
 *  \return boolean true if image lib can convert image data from internal color format 
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_IsValidDestinationFormat(OWF_IMAGE_FORMAT* format);

/*!---------------------------------------------------------------------------
*  \brief Convert image data from source format to internal format
 *
 *  \param src
 *  \param dst
 *
 *  \return
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_SourceFormatConversion(OWF_IMAGE* dst,
                                 OWF_IMAGE* src);

/*!---------------------------------------------------------------------------
 *  \brief
 *
 *  \param
 *
 *  \return
 *----------------------------------------------------------------------------*/
OWF_API_CALL void*
OWF_Image_AllocData(OWF_DISPCTX dc, OWFint width,
                    OWFint height,
                    OWF_PIXEL_FORMAT format);


/*!---------------------------------------------------------------------------
 *  \brief
 *
 *  \param
 *
 *  \return
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_FreeData(OWF_DISPCTX dc, void** buffer);

/*!---------------------------------------------------------------------------
 *  \brief
 *
 *  \param
 *
 *  \return
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFint
OWF_Image_GetFormatPixelSize(OWF_PIXEL_FORMAT format);


/*!---------------------------------------------------------------------------
 * \brief Return stride (aligned row size in bytes) calculated from image
 * width and pixelSize. If pixelSize is negative image width must be divisible
 * by pixelSize.
 *
 * \param width             Width of image
 * \param pixelSize         Size of single pixel in bytes. Negative size
 *                          means value is a divisor (i.e. 1/pixelSize)
 * \param padding           Number of bits each row is padded to.

 * \return Row size in bytes.
 *----------------------------------------------------------------------------*/
OWF_PUBLIC OWFint
OWF_Image_GetStride(OWFint width,
                    const OWF_IMAGE_FORMAT* format,OWFint minimumStride);

/*!---------------------------------------------------------------------------
 *  \brief
 *
 *  \param
 *
 *  \return
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFint
OWF_Image_GetFormatPadding(OWF_PIXEL_FORMAT format);

/*!---------------------------------------------------------------------------
 *  \brief Swap image width & height values in image header. Doesn't modify
 *  image pixel data.
 *
 *  \param image            Image to operate on.
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_SwapWidthAndHeight(OWF_IMAGE* image);

/*!---------------------------------------------------------------------------
 *  \brief Convert mask from external format to internal 8bpp format.
 *
 *  \param output           Result (converted) mask image
 *  \param input            Input mask image to convert.
 *
 *  \return Boolean value indicating operation success. OWF_FALSE means that
 *  input mask is either invalid or unsupported, or degenerate in some other
 *  fascinating way.
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_ConvertMask(OWF_IMAGE* output,
                      OWF_IMAGE* input);

/*!---------------------------------------------------------------------------
 *  \brief Return pointer to given pixel in image.
 *
 *  \param image            Image
 *  \param x                X-coordinate of the pixel (0..width-1)
 *  \param y                Y-coordinate of the pixel (0..height-1)
 *
 *  The x & y coordinates will be clamped to [0..width-1, 0..height-1]
 *
 *  \return Pointer to given pixel
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWFpixel*
OWF_Image_GetPixelPtr(OWF_IMAGE* image,
                      OWFint x,
                      OWFint y);

/*!---------------------------------------------------------------------------
 *  \brief Read single pixel from image
 *
 *  \param image Image
 *  \param x Pixel x coordinate
 *  \param y Pixel y coordinate
 *  \param pixel Where to store the pixel color value
 *
 *  Coordinates are clamped to region (0..width-1, 0..height-1)
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_GetPixel(OWF_IMAGE* image,
                   OWFint x,
                   OWFint y,
                   OWFpixel* pixel);

/*!---------------------------------------------------------------------------
 *  \brief Write a pixel into image
 *
 *  \param image Image
 *  \param x Pixel x coordinate
 *  \param y Pixel y coordinate
 *  \param pixel Color of the pixel
 *
 *  Coordinates are clamped to region (0..width-1, 0..height-1)
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_SetPixel(OWF_IMAGE* image,
                   OWFint x,
                   OWFint y,
                   OWFpixel const* pixel);

#ifdef __cplusplus
}
#endif

#endif
