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
* Implementation of EGL interface
*/

#include <egl.h>
#include "eglinternal.h"
#include <stdarg.h>


extern "C" {

EXPORT_C EGLint eglGetError(void)
	{
	return do_eglGetError();
	}
EXPORT_C EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id)
	{
	return do_eglGetDisplay(display_id);
	}
EXPORT_C EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
	{
	return do_eglInitialize(dpy, major, minor);
	}
EXPORT_C EGLBoolean eglTerminate(EGLDisplay dpy)
	{
	return do_eglTerminate(dpy);
	}
EXPORT_C const char* eglQueryString(EGLDisplay dpy, EGLint name)
	{
	return do_eglQueryString(dpy, name);
	}
EXPORT_C EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig *configs,EGLint config_size, EGLint *num_config)
	{
	return do_eglGetConfigs(dpy, configs,config_size, num_config);
	}
EXPORT_C EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list,EGLConfig *configs, EGLint config_size,EGLint *num_config)
	{
	return do_eglChooseConfig(dpy, attrib_list,configs, config_size,num_config);
    }
EXPORT_C EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config,EGLint attribute, EGLint *value)
	{
	return do_eglGetConfigAttrib(dpy, config,attribute, value);
	}
EXPORT_C EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,EGLNativeWindowType win,const EGLint *attrib_list)
	{
	return do_eglCreateWindowSurface(dpy, config,win,attrib_list);
	}
EXPORT_C EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config,const EGLint *attrib_list)
	{
	return do_eglCreatePbufferSurface(dpy, config,attrib_list);
	}
EXPORT_C EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config,EGLNativePixmapType pixmap,const EGLint *attrib_list)
	{
	return do_eglCreatePixmapSurface(dpy, config,pixmap,attrib_list);
	}
EXPORT_C EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
	{
	return do_eglDestroySurface(dpy, surface);
	}
EXPORT_C EGLBoolean eglQuerySurface(EGLDisplay dpy, EGLSurface surface,EGLint attribute, EGLint *value)
	{
	return do_eglQuerySurface(dpy, surface,attribute, value);
	}
EXPORT_C EGLBoolean eglBindAPI(EGLenum api)
	{
	return do_eglBindAPI(api);
	}
EXPORT_C EGLenum eglQueryAPI(void)
	{
	return do_eglQueryAPI();
	}
EXPORT_C EGLBoolean eglWaitClient(void)
	{
	return do_eglWaitClient();
	}
EXPORT_C EGLBoolean eglReleaseThread(void)
	{
	return do_eglReleaseThread();
	}
EXPORT_C EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer,EGLConfig config, const EGLint *attrib_list)
	{
	return do_eglCreatePbufferFromClientBuffer(dpy, buftype, buffer,config, attrib_list);
	}
EXPORT_C EGLBoolean eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface,EGLint attribute, EGLint value)
	{
	return do_eglSurfaceAttrib(dpy, surface,attribute, value);
	}
EXPORT_C EGLBoolean eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
	{
	return do_eglBindTexImage(dpy, surface, buffer);
	}
EXPORT_C EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
	{
	return do_eglReleaseTexImage(dpy, surface, buffer);
	}
EXPORT_C EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval)
	{
	return do_eglSwapInterval(dpy, interval);
	}
EXPORT_C EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config,EGLContext share_context,const EGLint *attrib_list)
	{
	return do_eglCreateContext(dpy, config,share_context,attrib_list);
	}
EXPORT_C EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
	{
	return do_eglDestroyContext(dpy, ctx);
	}
EXPORT_C EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,EGLSurface read, EGLContext ctx)
	{
	return do_eglMakeCurrent(dpy, draw,read, ctx);
	}
EXPORT_C EGLContext eglGetCurrentContext(void)
	{
	return do_eglGetCurrentContext();
	}
EXPORT_C EGLSurface eglGetCurrentSurface(EGLint readdraw)
	{
	return do_eglGetCurrentSurface(readdraw);
	}
EXPORT_C EGLDisplay eglGetCurrentDisplay(void)
	{
	return do_eglGetCurrentDisplay();
	}
EXPORT_C EGLBoolean eglQueryContext(EGLDisplay dpy, EGLContext ctx,EGLint attribute, EGLint *value)
	{
	return do_eglQueryContext(dpy, ctx,attribute, value);
	}
EXPORT_C EGLBoolean eglWaitGL(void)
	{
	return do_eglWaitGL();
	}
EXPORT_C EGLBoolean eglWaitNative(EGLint engine)
	{
	return do_eglWaitNative(engine);
	}
EXPORT_C EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
	{
	return do_eglSwapBuffers(dpy, surface);
	}
EXPORT_C EGLBoolean eglCopyBuffers(EGLDisplay dpy, EGLSurface surface,EGLNativePixmapType target)
	{
	return do_eglCopyBuffers(dpy, surface,target);
	}

typedef void (*eglfPtr)(...);
EXPORT_C void (*eglGetProcAddress(const char *procname))(...)
	{
    eglfPtr pname = do_eglGetProcAddress(procname);
    return pname;
	}
}

