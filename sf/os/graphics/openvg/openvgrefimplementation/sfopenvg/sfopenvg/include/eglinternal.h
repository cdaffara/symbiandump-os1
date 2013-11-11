/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Declaration of EGL like interface . Basically every EGL API names but prefixed with do_ e.g. do_eglInitialize
*/

#ifndef EGLINTERNAL_H
#define EGLINTERNAL_H

#include <egl.h>


/*
 * defines set of internal egl functiosn that are 1:1 with the published egl functions.
 * E.g. eglInitialize -> do_eglInitialize
 * 
 * These do_XXX fucntions are called from the libegl.dll (which is a shim) to be processed here
 */

IMPORT_C EGLint do_eglGetError(void);
IMPORT_C EGLDisplay do_eglGetDisplay(EGLNativeDisplayType display_id);
IMPORT_C EGLBoolean do_eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor);
IMPORT_C EGLBoolean do_eglTerminate(EGLDisplay dpy);
IMPORT_C const char* do_eglQueryString(EGLDisplay dpy, EGLint name);
IMPORT_C EGLBoolean do_eglGetConfigs(EGLDisplay dpy, EGLConfig *configs,EGLint config_size, EGLint *num_config);
IMPORT_C EGLBoolean do_eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list,EGLConfig *configs, EGLint config_size,EGLint *num_config);
IMPORT_C EGLBoolean do_eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config,EGLint attribute, EGLint *value);
IMPORT_C EGLSurface do_eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,EGLNativeWindowType win,const EGLint *attrib_list);
IMPORT_C EGLSurface do_eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config,const EGLint *attrib_list);
IMPORT_C EGLSurface do_eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config,EGLNativePixmapType pixmap,const EGLint *attrib_list);
IMPORT_C EGLBoolean do_eglDestroySurface(EGLDisplay dpy, EGLSurface surface);
IMPORT_C EGLBoolean do_eglQuerySurface(EGLDisplay dpy, EGLSurface surface,EGLint attribute, EGLint *value);
IMPORT_C EGLBoolean do_eglBindAPI(EGLenum api);
IMPORT_C EGLenum do_eglQueryAPI(void);
IMPORT_C EGLBoolean do_eglWaitClient(void);
IMPORT_C EGLBoolean do_eglReleaseThread(void);
IMPORT_C EGLSurface do_eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer,EGLConfig config, const EGLint *attrib_list);
IMPORT_C EGLBoolean do_eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface,EGLint attribute, EGLint value);
IMPORT_C EGLBoolean do_eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
IMPORT_C EGLBoolean do_eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
IMPORT_C EGLBoolean do_eglSwapInterval(EGLDisplay dpy, EGLint interval);
IMPORT_C EGLContext do_eglCreateContext(EGLDisplay dpy, EGLConfig config,EGLContext share_context,const EGLint *attrib_list);
IMPORT_C EGLBoolean do_eglDestroyContext(EGLDisplay dpy, EGLContext ctx);
IMPORT_C EGLBoolean do_eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,EGLSurface read, EGLContext ctx);
IMPORT_C EGLContext do_eglGetCurrentContext(void);
IMPORT_C EGLSurface do_eglGetCurrentSurface(EGLint readdraw);
IMPORT_C EGLDisplay do_eglGetCurrentDisplay(void);
IMPORT_C EGLBoolean do_eglQueryContext(EGLDisplay dpy, EGLContext ctx,EGLint attribute, EGLint *value);
IMPORT_C EGLBoolean do_eglWaitGL(void);
IMPORT_C EGLBoolean do_eglWaitNative(EGLint engine);
IMPORT_C EGLBoolean do_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface);
IMPORT_C EGLBoolean do_eglCopyBuffers(EGLDisplay dpy, EGLSurface surface,EGLNativePixmapType target);
IMPORT_C void (* EGLAPIENTRY do_eglGetProcAddress (const char *procname))(...);




#endif
