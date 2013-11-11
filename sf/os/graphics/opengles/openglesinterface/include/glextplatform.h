/*
** Copyright (c) 2007-2009 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

#ifndef glextplatform_h
#define glextplatform_h

/**
@publishedAll
@released
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
** The following defines were in gl.h in OpenGL ES 1.0.  To keep backwards 
** compatibility they are defined in this file which is always included at
** the end of gl.h. Undefine guard definitions as required to enable 
** OpenGL ES 1.1 glext.h functionality.
**
** We have also added __SOFTFP decorations for call-by-value float functions
*/

/* Renamed for OpenGL ES 1.1 */

#define GL_WRITE_ONLY                               0x88B9
#define GL_BUFFER_ACCESS                            0x88BB


/* Removed for OpenGL ES 1.1 */

/* OpenGL ES core versions */
#define GL_OES_VERSION_1_0                          1
#define GL_OES_VERSION_1_1                          1

/* BeginMode */
#define GL_MAX_ELEMENTS_VERTICES                    0x80E8
#define GL_MAX_ELEMENTS_INDICES                     0x80E9

/* HintTarget */
#define GL_POLYGON_SMOOTH_HINT                      0x0C53


/* Moved from gl.h to glext.h (optional header) for OpenGL ES 1.1 */

/* GL_OES_byte_coordinates */
#define GL_OES_byte_coordinates	                    1

/* GL_OES_draw_texture */
#define GL_OES_draw_texture                         1
#define GL_TEXTURE_CROP_RECT_OES                    0x8B9D
GL_API void GL_APIENTRY glDrawTexsOES (GLshort x, GLshort y, GLshort z, GLshort width, GLshort height);
GL_API void GL_APIENTRY glDrawTexiOES (GLint x, GLint y, GLint z, GLint width, GLint height);
GL_API void GL_APIENTRY glDrawTexxOES (GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height);
GL_API void GL_APIENTRY glDrawTexsvOES (const GLshort *coords);
GL_API void GL_APIENTRY glDrawTexivOES (const GLint *coords);
GL_API void GL_APIENTRY glDrawTexxvOES (const GLfixed *coords);
GL_API void GL_APIENTRY glDrawTexfOES (GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height) __SOFTFP;
GL_API void GL_APIENTRY glDrawTexfvOES (const GLfloat *coords);

/* GL_OES_fixed_point */
#define GL_OES_fixed_point	                        1

/* GL_OES_single_precision */
#define GL_OES_single_precision                     1

/* OES_matrix_get */
#define GL_OES_matrix_get	                        1
#define GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES   0x898D
#define GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES  0x898E
#define GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES     0x898F

/* OES_matrix_palette */
#define GL_OES_matrix_palette	                    1
#define GL_MAX_VERTEX_UNITS_OES                     0x86A4
#define GL_MAX_PALETTE_MATRICES_OES                 0x8842
#define GL_MATRIX_PALETTE_OES                       0x8840
#define GL_MATRIX_INDEX_ARRAY_OES                   0x8844
#define GL_WEIGHT_ARRAY_OES                         0x86AD
#define GL_CURRENT_PALETTE_MATRIX_OES               0x8843
#define GL_MATRIX_INDEX_ARRAY_SIZE_OES              0x8846
#define GL_MATRIX_INDEX_ARRAY_TYPE_OES              0x8847
#define GL_MATRIX_INDEX_ARRAY_STRIDE_OES            0x8848
#define GL_MATRIX_INDEX_ARRAY_POINTER_OES           0x8849
#define GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES    0x8B9E
#define GL_WEIGHT_ARRAY_SIZE_OES                    0x86AB
#define GL_WEIGHT_ARRAY_TYPE_OES                    0x86A9
#define GL_WEIGHT_ARRAY_STRIDE_OES                  0x86AA
#define GL_WEIGHT_ARRAY_POINTER_OES                 0x86AC
#define GL_WEIGHT_ARRAY_BUFFER_BINDING_OES          0x889E
GL_API void GL_APIENTRY glCurrentPaletteMatrixOES (GLuint matrixpaletteindex);
GL_API void GL_APIENTRY glLoadPaletteFromModelViewMatrixOES (void);
GL_API void GL_APIENTRY glMatrixIndexPointerOES (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
GL_API void GL_APIENTRY glWeightPointerOES (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);


GL_API GLbitfield GL_APIENTRY glQueryMatrixxOES (GLfixed mantissa[16], GLint exponent[16]);

#ifdef __cplusplus
}
#endif

#endif /* glextplatform_h */