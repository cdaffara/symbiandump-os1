// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Provide dummy implementations for functions of the variety of OpenGL ES versions.
// Currently supporting OpenGL ES version 1.0 and 1.1 (Common Profile & Common Profile v1)
// 
//

#include <EGL/egl.h>
#include <EGL/egltypes.h>
#include <GLES/gl.h>

#include <e32def.h>
#include <e32uid.h>
    
#ifdef __cplusplus
extern "C" {
#endif


// --------------------------------------------------------------------------------------------------------------------------------------------
// START: OpenGLES Version 1.0 Base set
// --------------------------------------------------------------------------------------------------------------------------------------------

#if	!defined(__SYMBIAN_USING_OPENGL_ES_1_1_v1__)

// OpenGLES EGL functions
EXPORT_C EGLint      eglGetError (void){ return 0; }
EXPORT_C EGLDisplay  eglGetDisplay (NativeDisplayType display){ return 0; }
EXPORT_C EGLBoolean  eglInitialize (EGLDisplay dpy, EGLint *major, EGLint *minor){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglTerminate (EGLDisplay dpy){ return GL_FALSE; }
EXPORT_C const char *  eglQueryString (EGLDisplay dpy, EGLint name){ return NULL; }

#ifdef __cplusplus
EXPORT_C void (* eglGetProcAddress (const char *procname))(...){ return NULL; }
#else
EXPORT_C void (* eglGetProcAddress (const char *procname))(){ return NULL; }
#endif

EXPORT_C EGLBoolean  eglGetConfigs (EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglChooseConfig (EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglGetConfigAttrib (EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value){ return GL_FALSE; }

EXPORT_C EGLSurface  eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint *attrib_list){ return 0; }
EXPORT_C EGLSurface  eglCreatePixmapSurface (EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint *attrib_list){ return 0; }
EXPORT_C EGLSurface  eglCreatePbufferSurface (EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list){ return 0; }
EXPORT_C EGLBoolean  eglDestroySurface (EGLDisplay dpy, EGLSurface surface){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglQuerySurface (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value){ return GL_FALSE; }

EXPORT_C EGLContext  eglCreateContext (EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint *attrib_list){ return 0; }
EXPORT_C EGLBoolean  eglDestroyContext (EGLDisplay dpy, EGLContext ctx){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglMakeCurrent (EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx){ return GL_FALSE; }
EXPORT_C EGLContext  eglGetCurrentContext (void){ return 0; }
EXPORT_C EGLSurface  eglGetCurrentSurface (EGLint readdraw){ return 0; }
EXPORT_C EGLDisplay  eglGetCurrentDisplay (void){ return 0; }
EXPORT_C EGLBoolean  eglQueryContext (EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value){ return GL_FALSE; }

EXPORT_C EGLBoolean  eglWaitGL (void){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglWaitNative (EGLint engine){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglSwapBuffers (EGLDisplay dpy, EGLSurface draw){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglCopyBuffers (EGLDisplay dpy, EGLSurface surface, NativePixmapType target){ return GL_FALSE; }

#endif // !defined(__SYMBIAN_USING_OPENGL_ES_1_1_v1__)

// --------------------------------------------------------------------------------------------------------------------------------------------
// OpenGLES GL functions
EXPORT_C void  glActiveTexture (GLenum texture){ }
EXPORT_C void  glAlphaFunc (GLenum func, GLclampf ref){ }
EXPORT_C void  glAlphaFuncx (GLenum func, GLclampx ref){ }
EXPORT_C void  glBindTexture (GLenum target, GLuint texture){ }
EXPORT_C void  glBlendFunc (GLenum sfactor, GLenum dfactor){ }
EXPORT_C void  glClear (GLbitfield mask){ }
EXPORT_C void  glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha){ }
EXPORT_C void  glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha){ }
EXPORT_C void  glClearDepthf (GLclampf depth){ }
EXPORT_C void  glClearDepthx (GLclampx depth){ }
EXPORT_C void  glClearStencil (GLint s){ }
EXPORT_C void  glClientActiveTexture (GLenum texture){ }
EXPORT_C void  glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){ }
EXPORT_C void  glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha){ }
EXPORT_C void  glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha){ }
EXPORT_C void  glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer){ }
EXPORT_C void  glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data){ }
EXPORT_C void  glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data){ }
EXPORT_C void  glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border){ }
EXPORT_C void  glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height){ }
EXPORT_C void  glCullFace (GLenum mode){ }
EXPORT_C void  glDeleteTextures (GLsizei n, const GLuint *textures){ }
EXPORT_C void  glDepthFunc (GLenum func){ }
EXPORT_C void  glDepthMask (GLboolean flag){ }
EXPORT_C void  glDepthRangef (GLclampf zNear, GLclampf zFar){ }
EXPORT_C void  glDepthRangex (GLclampx zNear, GLclampx zFar){ }
EXPORT_C void  glDisable (GLenum cap){ }
EXPORT_C void  glDisableClientState (GLenum array){ }
EXPORT_C void  glDrawArrays (GLenum mode, GLint first, GLsizei count){ }
EXPORT_C void  glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices){ }
EXPORT_C void  glEnable (GLenum cap){ }
EXPORT_C void  glEnableClientState (GLenum array){ }
EXPORT_C void  glFinish (void){ }
EXPORT_C void  glFlush (void){ }
EXPORT_C void  glFogf (GLenum pname, GLfloat param){ }
EXPORT_C void  glFogfv (GLenum pname, const GLfloat *params){ }
EXPORT_C void  glFogx (GLenum pname, GLfixed param){ }
EXPORT_C void  glFogxv (GLenum pname, const GLfixed *params){ }
EXPORT_C void  glFrontFace (GLenum mode){ }
EXPORT_C void  glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar){ }
EXPORT_C void  glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar){ }
EXPORT_C void  glGenTextures (GLsizei n, GLuint *textures){ }
EXPORT_C GLenum  glGetError (void){ return 0; }
EXPORT_C void  glGetIntegerv (GLenum pname, GLint *params){ }
EXPORT_C const GLubyte *  glGetString (GLenum name){ return 0; }
EXPORT_C void  glHint (GLenum target, GLenum mode){ }
EXPORT_C void  glLightModelf (GLenum pname, GLfloat param){ }
EXPORT_C void  glLightModelfv (GLenum pname, const GLfloat *params){ }
EXPORT_C void  glLightModelx (GLenum pname, GLfixed param){ }
EXPORT_C void  glLightModelxv (GLenum pname, const GLfixed *params){ }
EXPORT_C void  glLightf (GLenum light, GLenum pname, GLfloat param){ }
EXPORT_C void  glLightfv (GLenum light, GLenum pname, const GLfloat *params){ }
EXPORT_C void  glLightx (GLenum light, GLenum pname, GLfixed param){ }
EXPORT_C void  glLightxv (GLenum light, GLenum pname, const GLfixed *params){ }
EXPORT_C void  glLineWidth (GLfloat width){ }
EXPORT_C void  glLineWidthx (GLfixed width){ }
EXPORT_C void  glLoadIdentity (void){ }
EXPORT_C void  glLoadMatrixf (const GLfloat *m){ }
EXPORT_C void  glLoadMatrixx (const GLfixed *m){ }
EXPORT_C void  glLogicOp (GLenum opcode){ }
EXPORT_C void  glMaterialf (GLenum face, GLenum pname, GLfloat param){ }
EXPORT_C void  glMaterialfv (GLenum face, GLenum pname, const GLfloat *params){ }
EXPORT_C void  glMaterialx (GLenum face, GLenum pname, GLfixed param){ }
EXPORT_C void  glMaterialxv (GLenum face, GLenum pname, const GLfixed *params){ }
EXPORT_C void  glMatrixMode (GLenum mode){ }
EXPORT_C void  glMultMatrixf (const GLfloat *m){ }
EXPORT_C void  glMultMatrixx (const GLfixed *m){ }
EXPORT_C void  glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q){ }
EXPORT_C void  glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q){ }
EXPORT_C void  glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz){ }
EXPORT_C void  glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz){ }
EXPORT_C void  glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer){ }
EXPORT_C void  glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar){ }
EXPORT_C void  glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar){ }
EXPORT_C void  glPixelStorei (GLenum pname, GLint param){ }
EXPORT_C void  glPointSize (GLfloat size){ }
EXPORT_C void  glPointSizex (GLfixed size){ }
EXPORT_C void  glPolygonOffset (GLfloat factor, GLfloat units){ }
EXPORT_C void  glPolygonOffsetx (GLfixed factor, GLfixed units){ }
EXPORT_C void  glPopMatrix (void){ }
EXPORT_C void  glPushMatrix (void){ }
EXPORT_C void  glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels){ }
EXPORT_C void  glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z){ }
EXPORT_C void  glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z){ }
EXPORT_C void  glSampleCoverage (GLclampf value, GLboolean invert){ }
EXPORT_C void  glSampleCoveragex (GLclampx value, GLboolean invert){ }
EXPORT_C void  glScalef (GLfloat x, GLfloat y, GLfloat z){ }
EXPORT_C void  glScalex (GLfixed x, GLfixed y, GLfixed z){ }
EXPORT_C void  glScissor (GLint x, GLint y, GLsizei width, GLsizei height){ }
EXPORT_C void  glShadeModel (GLenum mode){ }
EXPORT_C void  glStencilFunc (GLenum func, GLint ref, GLuint mask){ }
EXPORT_C void  glStencilMask (GLuint mask){ }
EXPORT_C void  glStencilOp (GLenum fail, GLenum zfail, GLenum zpass){ }
EXPORT_C void  glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer){ }
EXPORT_C void  glTexEnvf (GLenum target, GLenum pname, GLfloat param){ }
EXPORT_C void  glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params){ }
EXPORT_C void  glTexEnvx (GLenum target, GLenum pname, GLfixed param){ }
EXPORT_C void  glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params){ }
EXPORT_C void  glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels){ }
EXPORT_C void  glTexParameterf (GLenum target, GLenum pname, GLfloat param){ }
EXPORT_C void  glTexParameterx (GLenum target, GLenum pname, GLfixed param){ }
EXPORT_C void  glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels){ }
EXPORT_C void  glTranslatef (GLfloat x, GLfloat y, GLfloat z){ }
EXPORT_C void  glTranslatex (GLfixed x, GLfixed y, GLfixed z){ }
EXPORT_C void  glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer){ }
EXPORT_C void  glViewport (GLint x, GLint y, GLsizei width, GLsizei height){ }
EXPORT_C GLbitfield  glQueryMatrixxOES (GLfixed mantissa[16], GLint exponent[16]){ return 0; }

// --------------------------------------------------------------------------------------------------------------------------------------------
// END: OpenGLES Version 1.0
// --------------------------------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------------------------------
// START: OpenGLES Version 1.1 Super set
// --------------------------------------------------------------------------------------------------------------------------------------------

#if defined(__SYMBIAN_USING_OPENGL_ES_1_1__) && !defined(__SYMBIAN_USING_OPENGL_ES_1_1_v1__) 

// OpenGLES EGL functions
/* EGL 1.1 swap control API */
EXPORT_C EGLBoolean  eglSwapInterval(EGLDisplay dpy, EGLint interval){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglSurfaceAttrib (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer){ return GL_FALSE; }
EXPORT_C EGLBoolean  eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer){ return GL_FALSE; }

#endif // defined(__SYMBIAN_USING_OPENGL_ES_1_1__) && !defined(__SYMBIAN_USING_OPENGL_ES_1_1_v1__) 

#if defined(__SYMBIAN_USING_OPENGL_ES_1_1__) || defined(__SYMBIAN_USING_OPENGL_ES_1_1_v1__) 

// --------------------------------------------------------------------------------------------------------------------------------------------
// OpenGLES GL functions
EXPORT_C void glBindBuffer (GLenum target, GLuint buffer){ }
EXPORT_C void glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage){ }
EXPORT_C void glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data){ }
EXPORT_C void glClipPlanex (GLenum plane, const GLfixed *equation){ }
EXPORT_C void glClipPlanef (GLenum plane, const GLfloat *equation){ }
EXPORT_C void glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha){ }
EXPORT_C void glDeleteBuffers (GLsizei n, const GLuint *buffers){ }
EXPORT_C void glGenBuffers (GLsizei n, GLuint *buffers){ }
EXPORT_C void glGetBooleanv (GLenum pname, GLboolean *params){ }
EXPORT_C void glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params){ }
EXPORT_C void glGetClipPlanef (GLenum pname, GLfloat eqn[4]){ }
EXPORT_C void glGetClipPlanex (GLenum pname, GLfixed eqn[4]){ }
EXPORT_C void glGetFixedv (GLenum pname, GLfixed *params){ }
EXPORT_C void glGetFloatv (GLenum pname, GLfloat *params){ }
EXPORT_C void glGetPointerv (GLenum pname, void **params){ }
EXPORT_C void glTexEnviv (GLenum target, GLenum pname, const GLint *params){ }
EXPORT_C void glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params){ }
EXPORT_C void glTexParameteriv (GLenum target, GLenum pname, const GLint *params){ }
EXPORT_C void glTexParameterxv (GLenum target, GLenum pname, const GLfixed *params){ }
EXPORT_C GLboolean  glIsBuffer (GLuint buffer){ return GL_FALSE; }
EXPORT_C GLboolean  glIsEnabled (GLenum cap){ return GL_FALSE; }
EXPORT_C GLboolean  glIsTexture (GLuint texture){ return GL_FALSE; }
EXPORT_C void glPointParameterf (GLenum pname, GLfloat param){ }
EXPORT_C void glPointParameterfv (GLenum pname, const GLfloat *params){ }
EXPORT_C void glPointParameterx (GLenum pname, GLfixed param){ }
EXPORT_C void glPointParameterxv (GLenum pname, const GLfixed *params){ }
EXPORT_C void glPointSizePointerOES (GLenum type, GLsizei stride, const GLvoid *pointer){ }
EXPORT_C void glTexEnvi (GLenum target, GLenum pname, GLint param){ }
EXPORT_C void glTexParameteri (GLenum target, GLenum pname, GLint param){ }
EXPORT_C void glGetLightfv (GLenum light, GLenum pname, GLfloat *params){ }
EXPORT_C void glGetLightxv (GLenum light, GLenum pname, GLfixed *params){ }
EXPORT_C void glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params){ }
EXPORT_C void glGetMaterialxv (GLenum face, GLenum pname, GLfixed *params){ }
EXPORT_C void glGetTexEnviv (GLenum env, GLenum pname, GLint *params){ }
EXPORT_C void glGetTexEnvfv (GLenum env, GLenum pname, GLfloat *params){ }
EXPORT_C void glGetTexEnvxv (GLenum env, GLenum pname, GLfixed *params){ }
EXPORT_C void glGetTexParameteriv (GLenum target, GLenum pname, GLint *params){ }
EXPORT_C void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params){ }
EXPORT_C void glGetTexParameterxv (GLenum target, GLenum pname, GLfixed *params){ }

#endif // defined(__SYMBIAN_USING_OPENGL_ES_1_1__) || defined(__SYMBIAN_USING_OPENGL_ES_1_1_v1__) 

// --------------------------------------------------------------------------------------------------------------------------------------------
// END: OpenGLES Version 1.1
// --------------------------------------------------------------------------------------------------------------------------------------------




#ifdef __cplusplus
}
#endif

