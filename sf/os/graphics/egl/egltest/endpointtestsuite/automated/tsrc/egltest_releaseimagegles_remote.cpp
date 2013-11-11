#include "egltest_releaseimagegles.h"
#include "eglendpointwrap.h"
#include "egltest_surface.h"


//Remote test step----------------------------------------------------------------

CEglTest_RemoteTestStep_EndpointReleaseImageGles::CEglTest_RemoteTestStep_EndpointReleaseImageGles() :
    CRemoteTestStepBase(ETestUidEndpointReleaseImageGles)
    {
    }


CEglTest_RemoteTestStep_EndpointReleaseImageGles::~CEglTest_RemoteTestStep_EndpointReleaseImageGles()
    {
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointReleaseImageGles::DoStartRemoteTestStepL(const TRemoteTestParams& /*aMessageIn*/)
    {
    REMOTE_INFO_PRINTF1(_L("Starting Remote Test Step."));
    EglStartL();
    
    iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    
    //Create a raw surface that will be encapsulated by the endpoint.
    iSurface = CRawSurface::NewL();
    iSurface->CreateL(EStandardSurface, TPoint(0, 0));

    TSurfaceId id = iSurface->SurfaceId();
    iEndpoint = EglEndpoint().CreateEndpoint(iDisplay, EGL_ENDPOINT_TYPE_CONSUMER_NOK, EGL_TSURFACEID_NOK, &id, NULL);
    if(iEndpoint == EGL_NO_ENDPOINT_NOK)
        {
        REMOTE_ERR_PRINTF1(_L("Failed to create endpoint."));
        User::Leave(KErrUnknown);
        }
    
    iSurface->DrawContentL(TRgb(128,128,128));
    User::LeaveIfError(iSurface->SubmitContent(EFalse));
    EGLBoolean ret = EglEndpoint().EndpointBeginStreaming(iDisplay, iEndpoint);
    if(!ret)
        {
        REMOTE_ERR_PRINTF1(_L("Failed to begin streaming from endpoint."));
        User::Leave(KErrUnknown);
        }
    
    return ERtvPass;
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointReleaseImageGles::DoEndRemoteTestStepL(const TRemoteTestParams& /*aMessageIn*/)
    {
    REMOTE_INFO_PRINTF1(_L("Ending Remote Test Step."));
    if(iEndpoint != EGL_NO_ENDPOINT_NOK)
        {
        EglEndpoint().EndpointEndStreaming(iDisplay, iEndpoint);
        EglEndpoint().DestroyEndpoint(iDisplay, iEndpoint);
        }
    delete iSurface;
    EglEndL();
    return ERtvPass;
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointReleaseImageGles::DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aParams)
    {
    switch(aTestCase)
        {
        case 0:     return GlesContextTestCaseL(aParams);
        default:    return ERtvAbort;
        }
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointReleaseImageGles::GlesContextTestCaseL(const TRemoteTestParams& aParams)
    {
    //This test case acquires an image from an endpoint then attempts to 
    //release the image, passing in EGL_OPENGL_ES_API as the API flag.
    //There are two configurations for the test, that is controlled by
    //a flag in aParams. The options are to have a valid GLES context 
    //set current, or to have nothing current.
    
    EGLContext glesContext = EGL_NO_CONTEXT;
    EGLSurface glesSurface = EGL_NO_SURFACE;
    
    //Create GLES state and set it current if the test requires it.
    if(aParams.iEndpointReleaseImageGles.iUseValidGlesContext)
        {
        EGLConfig config;
        EGLint nConfigs = 0;
        
        static const EGLint KConfigAttribs[] =
        {
            EGL_BUFFER_SIZE,    32,
            EGL_RED_SIZE,       8,
            EGL_GREEN_SIZE,     8,
            EGL_BLUE_SIZE,      8,
            EGL_ALPHA_SIZE,     8,
            EGL_SURFACE_TYPE,   EGL_PBUFFER_BIT,
            EGL_RENDERABLE_TYPE,EGL_OPENGL_ES_BIT,
            EGL_NONE
        };

        eglChooseConfig(iDisplay, KConfigAttribs, &config, 1, &nConfigs);
        if (!nConfigs)
            {
            RDebug::Printf("%s:%d: err = %d", __FILE__, __LINE__, eglGetError());
            User::Leave(KErrNotSupported);
            }
    
        if (!eglBindAPI(EGL_OPENGL_ES_API))
            {
            RDebug::Printf("%s:%d: err = %d", __FILE__, __LINE__, eglGetError());
            User::Leave(KErrNotSupported);
            }
        
        glesContext = eglCreateContext(iDisplay, config, 0, NULL);
        if (glesContext == EGL_NO_CONTEXT)
            {
            RDebug::Printf("%s:%d: err = %d", __FILE__, __LINE__, eglGetError());
            User::Leave(KErrNotSupported);
            }
        
        static const EGLint KPbufferAttribs[] =
            {
            EGL_WIDTH, 100,
            EGL_HEIGHT, 100,
            EGL_NONE,
            };
        
        glesSurface = eglCreatePbufferSurface(iDisplay, config, KPbufferAttribs);
        if (glesSurface == EGL_NO_SURFACE)
            {
            RDebug::Printf("%s:%d: err = %x", __FILE__, __LINE__, eglGetError());
            User::Leave(KErrNotSupported);
            }
        
        eglMakeCurrent(iDisplay, glesSurface, glesSurface, glesContext);
        }
    
    //Run the test case ansd trap to give us an opportunity to destroy the GLES state if it exists.
    TRemoteTestVerdict verdict = ERtvPass;
    TRAPD(err, verdict = DoGlesContextTestCaseL(aParams));
    
    //Destroy the GLES state if it exists.
    if(aParams.iEndpointReleaseImageGles.iUseValidGlesContext)
        {
        eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(iDisplay, glesSurface);
        eglDestroyContext(iDisplay, glesContext);
        }
    
    //Releave if the test left or return the verdict.
    User::LeaveIfError(err);
    return verdict;
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointReleaseImageGles::DoGlesContextTestCaseL(const TRemoteTestParams& /*aParams*/)
    {
    //Draw to the surface and submit the update.
    iSurface->DrawContentL(TRgb(128,128,128));
    User::LeaveIfError(iSurface->SubmitContent(ETrue));
    
    //Acquire an image and check that it was successful.
    EGLImageKHR image = EglEndpoint().AcquireImage(iDisplay, iEndpoint);
    if(image == EGL_NO_IMAGE_KHR)
        {
        REMOTE_ERR_PRINTF2(_L("Failed to acquire image from endpoint with error = %d"), eglGetError());
        return ERtvFail;
        }
    
    //Attempt to release the image. This must always succeed regardless of whether there is a valid GLES context.
    EGLBoolean ret = EglEndpoint().ReleaseImage(iDisplay, iEndpoint, image, EGL_OPENGL_ES_API);
    if(!ret)
        {
        REMOTE_ERR_PRINTF2(_L("Failed to release image from endpoint with error = %d"), eglGetError());
        return ERtvFail;
        }
    
    return ERtvPass;
    }

//--------------------------------------------------------------------------------
