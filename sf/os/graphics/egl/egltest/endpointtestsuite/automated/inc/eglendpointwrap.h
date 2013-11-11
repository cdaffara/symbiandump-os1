// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __EGLENDPOINTWRAP_H__
#define __EGLENDPOINTWRAP_H__


#include <e32base.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>


//generic function pointer
typedef void (*TAnyFuncPtr)(...);


/*
 * TEglEndpointWrap is a simple class that presents all of the EGL endpoint
 * extension functions without the user needing to perform an eglGetProcAddress()
 * to obtain the function pointer. Each endpoint member function takes the same
 * arguments as the EGL functions and returns the same types. After before first
 * use, you should check the Error() function to ensure that all function pointers
 * were resolved. Trying to use one of the endpoint functions in the event of a
 * construction error will result in a panic.
 */
class TEglEndpointWrap
    {
public:
    TEglEndpointWrap();
    TInt Error() const;

    //endpoint functions
    EGLEndpointNOK CreateEndpoint(EGLDisplay dpy, EGLenum type, EGLenum source_type, EGLEndpointSourceNOK source, const EGLint *attrib_list) const;
    EGLBoolean DestroyEndpoint(EGLDisplay dpy, EGLEndpointNOK endpoint) const;
    EGLint GetEndpointAttrib(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib) const;
    EGLBoolean SetEndpointAttrib(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib, EGLint value) const;
    EGLBoolean EndpointBeginStreaming(EGLDisplay dpy, EGLEndpointNOK endpoint) const;
    EGLBoolean EndpointEndStreaming(EGLDisplay dpy, EGLEndpointNOK endpoint) const;
    EGLImageKHR AcquireImage(EGLDisplay dpy, EGLEndpointNOK endpoint) const;
    EGLBoolean ReleaseImage(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLImageKHR image, EGLenum api) const;
    EGLint GetEndpointDirtyArea(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint* rects, EGLint start_rect, EGLint max_rects, EGLBoolean collapse) const;
    EGLBoolean EndpointRequestNotification(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLTRequestStatusNOK sync) const;
    EGLBoolean EndpointCancelNotification(EGLDisplay dpy, EGLEndpointNOK endpoint) const;
    EGLBoolean DestroyImage(EGLDisplay dpy, EGLImageKHR image) const;

private:
    TAnyFuncPtr ProcAddressL(const char* aProcName) const;

private:
    TInt iError;

    //enpoint function pointers
    PFNEGLCREATEENDPOINTNOKPROC ipfnEglCreateEndpointNOK;
    PFNEGLDESTROYENDPOINTNOKPROC ipfnEglDestroyEndpointNOK;
    PFNEGLGETENDPOINTATTRIBNOKPROC ipfnEglGetEndpointAttribNOK;
    PFNEGLSETENDPOINTATTRIBNOKPROC ipfnEglSetEndpointAttribNOK;
    PFNEGLENDPOINTBEGINSTREAMINGNOKPROC ipfnEglEndpointBeginStreamingNOK;
    PFNEGLENDPOINTENDSTREAMINGNOKPROC ipfnEglEndpointEndStreamingNOK;
    PFNEGLACQUIREIMAGENOKPROC ipfnEglAcquireImageNOK;
    PFNEGLRELEASEIMAGENOKPROC ipfnEglReleaseImageNOK;
    PFNEGLGETENDPOINTDIRTYAREANOKPROC ipfnEglGetEndpointDirtyAreaNOK;
    PFNEGLENDPOINTREQUESTNOTIFICATIONNOKPROC ipfnEglEndpointRequestNotificationNOK;
    PFNEGLENDPOINTCANCELNOTIFICATIONNOKPROC ipfnEglEndpointCancelNotificationNOK;
    PFNEGLDESTROYIMAGEKHRPROC ipfnEglDestroyImageKHR;
    };


#endif
