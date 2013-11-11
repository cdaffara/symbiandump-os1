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
//
// Description:
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32debug.h>
#include "eglendpointwrap.h"
#include "egltest_commscommon.h"


/*
 * TEglEndpointWrap is a simple class that presents all of the EGL endpoint
 * extension functions without the user needing to perform an eglGetProcAddress()
 * to obtain the function pointer. Each endpoint member function takes the same
 * arguments as the EGL functions and returns the same types. After construction
 * you should check the Error() function to ensure that all function pointers
 * were resolved. Trying to use one of the endpoint functions in the event of a
 * construction error will result in a panic.
 */


TEglEndpointWrap::TEglEndpointWrap()
    {
    //Save all the endpoint function pointers. If an error occurs, log it to iError.
    TRAP(iError,
        ipfnEglCreateEndpointNOK = reinterpret_cast<PFNEGLCREATEENDPOINTNOKPROC>(ProcAddressL("eglCreateEndpointNOK"));
        ipfnEglDestroyEndpointNOK = reinterpret_cast<PFNEGLDESTROYENDPOINTNOKPROC>(ProcAddressL("eglDestroyEndpointNOK"));
        ipfnEglGetEndpointAttribNOK = reinterpret_cast<PFNEGLGETENDPOINTATTRIBNOKPROC>(ProcAddressL("eglGetEndpointAttribNOK"));
        ipfnEglSetEndpointAttribNOK = reinterpret_cast<PFNEGLSETENDPOINTATTRIBNOKPROC>(ProcAddressL("eglSetEndpointAttribNOK"));
        ipfnEglEndpointBeginStreamingNOK = reinterpret_cast<PFNEGLENDPOINTBEGINSTREAMINGNOKPROC>(ProcAddressL("eglEndpointBeginStreamingNOK"));
        ipfnEglEndpointEndStreamingNOK = reinterpret_cast<PFNEGLENDPOINTENDSTREAMINGNOKPROC>(ProcAddressL("eglEndpointEndStreamingNOK"));
        ipfnEglAcquireImageNOK = reinterpret_cast<PFNEGLACQUIREIMAGENOKPROC>(ProcAddressL("eglAcquireImageNOK"));
        ipfnEglReleaseImageNOK = reinterpret_cast<PFNEGLRELEASEIMAGENOKPROC>(ProcAddressL("eglReleaseImageNOK"));
        ipfnEglGetEndpointDirtyAreaNOK = reinterpret_cast<PFNEGLGETENDPOINTDIRTYAREANOKPROC>(ProcAddressL("eglGetEndpointDirtyAreaNOK"));
        ipfnEglEndpointRequestNotificationNOK = reinterpret_cast<PFNEGLENDPOINTREQUESTNOTIFICATIONNOKPROC>(ProcAddressL("eglEndpointRequestNotificationNOK"));
        ipfnEglEndpointCancelNotificationNOK = reinterpret_cast<PFNEGLENDPOINTCANCELNOTIFICATIONNOKPROC>(ProcAddressL("eglEndpointCancelNotificationNOK"));
        ipfnEglDestroyImageKHR = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(ProcAddressL("eglDestroyImageKHR"));
        );
    }


TInt TEglEndpointWrap::Error() const
    {
    return iError;
    }


TAnyFuncPtr TEglEndpointWrap::ProcAddressL(const char* aProcName) const
    {
    //get the function pointer and check for errors
    TAnyFuncPtr func = reinterpret_cast<TAnyFuncPtr>(eglGetProcAddress(aProcName));
    if(!func)
        {
        User::Leave(KErrNotFound);
        }
    return func;
    }


EGLEndpointNOK TEglEndpointWrap::CreateEndpoint(EGLDisplay dpy, EGLenum type, EGLenum source_type, 
                                                EGLEndpointSourceNOK source, const EGLint *attrib_list) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglCreateEndpointNOK(dpy, type, source_type, source, attrib_list);
    }


EGLBoolean TEglEndpointWrap::DestroyEndpoint(EGLDisplay dpy, EGLEndpointNOK endpoint) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglDestroyEndpointNOK(dpy, endpoint);
    }


EGLint TEglEndpointWrap::GetEndpointAttrib(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglGetEndpointAttribNOK(dpy, endpoint, attrib);
    }


EGLBoolean TEglEndpointWrap::SetEndpointAttrib(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib, EGLint value) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglSetEndpointAttribNOK(dpy, endpoint, attrib, value);
    }


EGLBoolean TEglEndpointWrap::EndpointBeginStreaming(EGLDisplay dpy, EGLEndpointNOK endpoint) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglEndpointBeginStreamingNOK(dpy, endpoint);
    }


EGLBoolean TEglEndpointWrap::EndpointEndStreaming(EGLDisplay dpy, EGLEndpointNOK endpoint) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglEndpointEndStreamingNOK(dpy, endpoint);
    }


EGLImageKHR TEglEndpointWrap::AcquireImage(EGLDisplay dpy, EGLEndpointNOK endpoint) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglAcquireImageNOK(dpy, endpoint);
    }


EGLBoolean TEglEndpointWrap::ReleaseImage(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLImageKHR image, EGLenum api) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglReleaseImageNOK(dpy, endpoint, image, api);
    }


EGLint TEglEndpointWrap::GetEndpointDirtyArea(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint* rects, 
                                              EGLint start_rect, EGLint max_rects, EGLBoolean collapse) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglGetEndpointDirtyAreaNOK(dpy, endpoint, rects, start_rect, max_rects, collapse);
    }


EGLBoolean TEglEndpointWrap::EndpointRequestNotification(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLTRequestStatusNOK sync) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglEndpointRequestNotificationNOK(dpy, endpoint, sync);
    }


EGLBoolean TEglEndpointWrap::EndpointCancelNotification(EGLDisplay dpy, EGLEndpointNOK endpoint) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglEndpointCancelNotificationNOK(dpy, endpoint);
    }


EGLBoolean TEglEndpointWrap::DestroyImage(EGLDisplay dpy, EGLImageKHR image) const
    {
    ENDPOINT_ASSERT_DEBUG(iError == KErrNone, User::Invariant());
    return ipfnEglDestroyImageKHR(dpy, image);
    }
