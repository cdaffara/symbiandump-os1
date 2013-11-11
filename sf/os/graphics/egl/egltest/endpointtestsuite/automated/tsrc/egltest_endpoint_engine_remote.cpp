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

#include <e32std.h>
#include <e32math.h>
#include <e32atomics.h> 
#include "egltest_endpoint_engine.h"
#include "egltest_endpoint_images.h"
#include "egltest_surface.h"
#include "egltest_parameters.h"


const TInt KMemStatsReserve = 3;

CEgltest_Remote_Engine::CEgltest_Remote_Engine()
    : CRemoteTestStepBase(ETestUidEndpointEngine), iTestVerdict(ERtvPass), iLogging(EFalse), iSurface(0)
    {
    iMainThreadHeap = &User::Heap();   // ?? Is this the right heap ??
    for (TInt i = 0; i < KMaxEndpoints; i++)
        {
        iEndpoints[i] = EGL_NO_ENDPOINT_NOK;
        iEglImage[i] = EGL_NO_IMAGE_KHR;
        iVgImage[i] = NULL;
        iRequestStatus[i] = KRequestPending;
        }
    ipfnEglQueryProfilingDataNOK = reinterpret_cast<PFNEGLQUERYPROFILINGDATANOKPROC>(eglGetProcAddress("eglQueryProfilingDataNOK"));
    if (ipfnEglQueryProfilingDataNOK)
        {
        RDebug::Printf("%s:%d: found eglQueryProfilingDataNOK function");
        }
    }

CEgltest_Remote_Engine::~CEgltest_Remote_Engine()
    {
    // Note: This is run from a different thread an on a different heap than the one
    // used during the execution of DoRunRemoteTestCaseL(). So any allocation done
    // during DoRunRemoteTestCaseL must be de-allocated in DoEndRemoteTestStepL()
    }

TRemoteTestVerdict CEgltest_Remote_Engine::DoStartRemoteTestStepL(
        const TRemoteTestParams& aParams)
    {
    iMainThreadHeap = &User::Heap();   
    
    iMemoryStats.ReserveL(KMemStatsReserve);
    iLogging = EFalse;
    iLogErrors = aParams.iEndpointEngineConfig.iLogErrors;
    return ERtvPass;
    }

TRemoteTestVerdict CEgltest_Remote_Engine::DoEndRemoteTestStepL(
        const TRemoteTestParams& /* aParams */)
    {
    iMemoryStats.Close();
    delete iSurface;
    return ERtvPass;
    }

void CEgltest_Remote_Engine::CheckReturn(TInt aRetval,
        const TEngineTestCase& aEngineTestCase, TInt aFailValue,
        const TText* aFailSymbol, const TText* aFunction)
    {
    TBool isEqual = (aRetval == aFailValue);
    TBool expectFail = (aEngineTestCase.iFlags & EExpectFailureMask) != 0;

    EGLint err = eglGetError();
    if (err != aEngineTestCase.iErrorExpected)
        {
        if (iLogErrors)
            {
            REMOTE_ERR_PRINTF3(_L("testcase failed: expected %04x, got %04x"), aEngineTestCase.iErrorExpected, err);
            }
        iTestVerdict = ERtvFail;
        }

    if (!isEqual && expectFail)
        {
        if (iLogErrors)
            {
            REMOTE_ERR_PRINTF5(
                    _L("return value when failing from %s is not expected fail value %s (%d). Value returned is %d"),
                    aFunction, aFailSymbol, aFailValue, aRetval);
            }
        iTestVerdict = ERtvFail;
        }
    else if (isEqual && !expectFail)
        {
        if (iLogErrors)
            {
            REMOTE_ERR_PRINTF5(
                    _L("return value when succeeding from %s is equal to expected fail value %s (%d). Value returned is %d"),
                    aFunction, aFailSymbol, aFailValue, aRetval);
            }
        iTestVerdict = ERtvFail;
        }
    if (isEqual != expectFail)
        {
        if (iLogErrors)
            {
            REMOTE_ERR_PRINTF4(_L("Unexpected result for %s, failvalue is %s, flags = %d"),
                    aFunction, aFailSymbol,
                    aEngineTestCase.iFlags);
            }
        iTestVerdict = ERtvFail;
        }
    // Now check
    if (expectFail && err == EGL_SUCCESS)
        {
        if (iLogErrors)
            {
            REMOTE_ERR_PRINTF2(_L("Got EGL_SUCCESS in error when calling %s, when we expected an error"),
                    aFunction);
            }
        iTestVerdict = ERtvFail;
        }
    // Didn't expect to fail, so we
    else if (!expectFail && err != EGL_SUCCESS)
        {
        if (iLogErrors)
            {
            REMOTE_ERR_PRINTF3(_L("Got an error (%x) on successful call to %s, when expecting EGL_SUCCESS"),
                    err, aFunction);
            }
        iTestVerdict = ERtvFail;
        }
    }

#define CHECK_RETURN(retval, failval, func) \
    CheckReturn((retval), si, (failval), _S(#failval), func)

#define CHECK_RETURN_CAST(retval, failval, func) \
    CheckReturn(reinterpret_cast<int>(retval), si, reinterpret_cast<int>(failval), _S(#failval), func)

#define CHECK_BOOL_RET(func, funcName) \
{  \
    EGLBoolean ret = EglEndpoint().func(dpy, endpoint);     \
    CheckReturn(ret, si, EGL_FALSE, _S("EGL_FALSE"), _S(funcName));  \
}

void CEgltest_Remote_Engine::LogDump(const TEngineTestCase& aCase)
    {
    const TText *caseName = EngineCaseName(aCase.iCase);

    Log(((TText8*)__FILE__), __LINE__, ESevrInfo,
                    _L("Performing subcase %d (%s), with flags=%d, err=%04x endpointidx=%d, image=%d, args=(%d, %d)"),
                    aCase.iCase,
                    caseName,
                    aCase.iFlags,
                    aCase.iErrorExpected,
                    aCase.iEndpointIndex,
                    aCase.iImageIndex,
                    aCase.iArg1, aCase.iArg2);
    }

TRemoteTestVerdict CEgltest_Remote_Engine::DoRunRemoteTestCaseL(
        TInt aTestCase, const TRemoteTestParams &aParams)
    {
    TRemoteTestArgs args;
    iTestVerdict = ERtvPass;
    const TEngineTestCase &si = aParams.iEndpointEngine.iEngineTestCase;

    CDisplayParams* displayParams = CDisplayParams::NewLC(!!(si.iFlags & EUseBadDisplay), eglGetDisplay(EGL_DEFAULT_DISPLAY));
    TInt dpyParamsCount = displayParams->Count();
    CEndpointParams* endpointParams = CEndpointParams::NewLC(!!(si.iFlags & EUseBadEndpoint), iEndpoints, KMaxEndpoints, si.iEndpointIndex);
    TInt endpointCount  = endpointParams->Count();
    CImageParams* imageParams = CImageParams::NewLC(!!(si.iFlags & EUseBadEglImage), iEglImage, KMaxEndpoints, si.iImageIndex);
    TInt imageCount = imageParams->Count();

    for(TInt dpyIter = 0; dpyIter < dpyParamsCount; dpyIter++)
        {
        args.iDisplay = (*displayParams)[dpyIter];
        for(TInt epIter = 0; epIter < endpointCount; epIter++)
            {
            args.iEndpoint = (*endpointParams)[epIter];

            for(TInt imageIter = 0; imageIter < imageCount; imageIter++)
                {
                args.iImage = (*imageParams)[imageIter];

                RunCaseL(aTestCase, aParams, args);
                if (iLogErrors && iTestVerdict != ERtvPass || iLogging)
                    {
                    if (iTestVerdict != ERtvPass)
                        {
                        REMOTE_INFO_PRINTF1(_L("Test failed:"));
                        }
                    LogDump(si);
                    REMOTE_INFO_PRINTF4(_L("Using values: display: %d, endpoint: %d, image: %d"),
                            args.iDisplay, args.iEndpoint, args.iImage);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(3);
    return iTestVerdict;
    }


void CEgltest_Remote_Engine::ActivateVgContextL()
    {
    if (!iSurface)
        {
        iSurface = CEglWindowSurface::NewL();
        iSurface->CreateL(EStandardSurface, TPoint(0, 110));
        }
    iSurface->ActivateL();
    }


TInt CEgltest_Remote_Engine::FillGpuMemory()
    {
    TSurfaceIndex table[] = 
            {
            ELargeSurface,
            EStandard128sqSurface,
            ESmallSurface,
            ETinySurface
            };
    const TInt KNumSurfaceTypes = sizeof(table) / sizeof(table[0]);
    
    TInt nSurfaces = 0;
    const TInt KMaxSurfaceAllocs = 1000;
    CSurface **surfaces = new CSurface*[KMaxSurfaceAllocs];
    TInt size = 0;
    ENGINE_ASSERT(surfaces);
    for(TInt i = 0; i < KNumSurfaceTypes; i++)
        {
        TInt err = KErrNone;
        while(err == KErrNone)     
            {
            ENGINE_ASSERT(nSurfaces < KMaxSurfaceAllocs);
            CSurface* s = CSurface::SurfaceFactoryL(ESurfTypePBuffer);
            if (s)
                {
                TRAP(err, s->CreateL(table[i]));
                if (err == KErrNone)
                    {
                    surfaces[nSurfaces++] = s;
//                    s->DrawContentL(TRgb(0x10, 0x20, 0xB0));
                    size += s->SizeInBytes();
                    }
                }
            }
        }
    RDebug::Printf("nSurfaces=%d", nSurfaces);
    while(nSurfaces)
        {
        delete surfaces[--nSurfaces];
        }
    delete [] surfaces;
    return size;
    }

TInt CEgltest_Remote_Engine::CalculateAvailableGPUMemory()
    {
    TInt result = 0;
    if (ipfnEglQueryProfilingDataNOK)
        {
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        ENGINE_ASSERT(display != EGL_NO_DISPLAY);
        TInt count;
        ipfnEglQueryProfilingDataNOK(
                display, EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK, 
                NULL, 0, &count);
        ENGINE_ASSERT(count);
        TInt *mem = new TInt[count * 2];
        ENGINE_ASSERT(mem);
        TInt newCount;
        ipfnEglQueryProfilingDataNOK(
                display, EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK, 
                mem, count, &newCount);
        ENGINE_ASSERT(newCount == count);
        for(TInt i = 0; i < count; i ++)
            {
            switch(mem[i*2])
                {
            case EGL_PROF_USED_MEMORY_NOK:
                // Assert that we only have one entry - if there are
                // more than one, we can't really know what is the "right" one.
                ENGINE_ASSERT(!result);
                result = mem[i*2+1];
                break;
                }
            }
        delete [] mem;
        }
    else
        {
#if 1
        result = 1000;
#else
        // This code currently causes a memory leak to be detected when the
        // remote thread is destroyed. This causes further tests to be skipped.
        // We disable this function at the moment, to allow other tests to run.
        result = FillGpuMemory();
#endif
        }
    return result;
    }


TInt CEgltest_Remote_Engine::CalculateAvailableHeapMemory()
    {
    TInt biggest = 0;
    return User::Heap().Available(biggest);
    }


void CEgltest_Remote_Engine::CheckForMemoryLeaks()
    {
    TAvailableMemory mem;
    mem.iGpuMemAvailable = CalculateAvailableGPUMemory();
    mem.iHeapMemAvailable = CalculateAvailableHeapMemory();
    
    REMOTE_INFO_PRINTF3(_L("GPU memory available: %d, heapmemory available: %d"), 
            mem.iGpuMemAvailable, mem.iHeapMemAvailable);
    if (iMemoryStats.Count() ==  KMemStatsReserve)
        {
        REMOTE_INFO_PRINTF2(_L("false positive HEAP leak possible, as reserved memory is exhausted... (%d)"), KMemStatsReserve);
        }
    TInt err = iMemoryStats.Append(mem); 
    if (err)
        {
        REMOTE_ERR_PRINTF2(_L("CheckForMemoryLeaks could not append to iMemoryStats. err=%d"), err);
        }
    }

void CEgltest_Remote_Engine::CheckForMemoryLeaksFinish()
    {
    TInt count = iMemoryStats.Count();
    if (count)
        {
        TReal sumGpu = 0.0;
        TReal sumHeap = 0.0;
        
        for(TInt i = 0; i < count; i++)
            {
            sumGpu += iMemoryStats[i].iGpuMemAvailable;
            sumHeap += iMemoryStats[i].iHeapMemAvailable;
            }
        REMOTE_INFO_PRINTF2(_L("CheckMemoryLeaksFinish - average = %6.2f"), sumGpu / count);
        REMOTE_INFO_PRINTF2(_L("CheckMemoryLeaksFinish - average = %6.2f"), sumHeap / count);
        }
    else
        {
        REMOTE_INFO_PRINTF1(_L("CheckMemoryLeaksFinish - no data collected"));
        }
    iMemoryStats.Close();
    }

TRemoteTestVerdict ConvertToLocalVerdict(TInt aVerdict)
    {
    switch(aVerdict)
        {
        case EPass:
            return ERtvPass;
        case EFail:
            return ERtvFail;
        }
        return ERtvInconclusive;
    }

void CEgltest_Remote_Engine::RunCaseL(TInt aTestCase, const TRemoteTestParams &aParams, const TRemoteTestArgs& aArgs)
    {
    const TEngineTestCase &si = aParams.iEndpointEngine.iEngineTestCase;
    EGLDisplay dpy = aArgs.iDisplay;
    EGLImageKHR image = aArgs.iImage;
    EGLEndpointNOK endpoint = aArgs.iEndpoint;

    switch (aTestCase)
        {
        case EInitializeCase:
            REMOTE_INFO_PRINTF1(_L("calling EglStartL()"));
            EglStartL();
            break;

        case ECreateEndpointCase:
            CreateEndpointCaseL(aParams, aArgs);
            break;

        case EAcquireImageCase:
            image = EglEndpoint().AcquireImage(dpy, endpoint);
            CHECK_RETURN(image, EGL_NO_IMAGE_KHR, _S("eglAcquireImage"));
            if (image != EGL_NO_IMAGE_KHR)
                {
                iEglImage[si.iEndpointIndex] = image;
                }
            break;

        case ECompareImageCase:
            if (iEglImage[si.iEndpointIndex] == EGL_NO_IMAGE_KHR)
                {
                REMOTE_ERR_PRINTF2(_L("Image at index %d is not a valid eglImage"), si.iEndpointIndex);
                iTestVerdict = ERtvFail;
                }
            else
                {
                ActivateVgContextL();

                CTestCFbsImage *image = CTestCFbsImage::NewL(si.iImageIndex);
                CleanupStack::PushL(image);
                CTestVgEglImage *vgImage = CTestVgEglImage::New(iEglImage[si.iEndpointIndex]);
                if (!vgImage)
                    {
                    REMOTE_INFO_PRINTF2(_L("Could not create vgimage from eglimage: endpointindex=%d"), 
                            si.iEndpointIndex);
                    }
                else
                    {
                    CleanupStack::PushL(vgImage);
                    TBool res = vgImage->CompareImageL(image, !!(si.iFlags & EExpectError));
                    if (res != !(si.iFlags & EExpectError))
                        {
                        if (iLogErrors)
                            {
                        REMOTE_ERR_PRINTF1(_L("Pixel comparison failed...."));
                            }
                        iTestVerdict = ERtvFail;
                        }
                    else if (!(si.iFlags & EExpectError))
                        {
                        // Extra check that ANOTHER image doesn't match the image
                        // we compared with.
                        // This would detect when images have incorrect content or
                        // the code for comparing images have been broken. 
                        TInt imageIndex2 = (si.iImageIndex + 1) % CTestImage::KImageCount;
                        CTestCFbsImage *image2 = CTestCFbsImage::NewL(imageIndex2);
                        CleanupStack::PushL(image2);
                        res = vgImage->CompareImageL(image2, ETrue);
                        if (res)
                            {
                            REMOTE_ERR_PRINTF1(_L("Pixel comparison didn't fail - two images the same?...."));
                            iTestVerdict = ERtvFail;
                            }
                        CleanupStack::PopAndDestroy(image2);
                        }
                    CleanupStack::PopAndDestroy(vgImage);
                    }
                CleanupStack::PopAndDestroy(image);
                }
            break;

        case EReleaseImageCase:
            ReleaseImageCaseL(aParams, aArgs);
            break;

        case EBeginStreamingCase:
            CHECK_BOOL_RET(EndpointBeginStreaming, "eglEndpointBeginStreaming");
            break;

        case EEndStreamingCase:
            CHECK_BOOL_RET(EndpointEndStreaming, "eglEndpointEndStreaming");
            break;

        case EDestroyEndpointCase:
            CHECK_BOOL_RET(DestroyEndpoint, "eglDestroyEndpoint");
            break;

        case EGetAttribCase:
            {
            TInt value = EglEndpoint().GetEndpointAttrib(dpy, endpoint, si.iArg1);
            // We can't use the macro CHECK_RETURN here, as the return value for
            // "success" can be any integer value, including "EGL_FALSE". So we can
            // only check when we expect failure.
            if (si.iFlags & EExpectFailureMask)
                {
                CheckReturn(value, si, EGL_FALSE,_S("EGL_FALSE") ,_S("eglGetEndpointAttrib"));
                }
            else
                {
                EGLint err = eglGetError();
                if (err != EGL_SUCCESS)
                    {
                    REMOTE_ERR_PRINTF2(_L("Got an error (%x) on successful call to eglGetEndpointAttrib, when expecting EGL_SUCCESS in error"),
                            err);
                    iTestVerdict = ERtvFail;
                    }
                }
            if (value != si.iArg2)
                {
                REMOTE_ERR_PRINTF4(_L("GetEndpointAttrib(%04x), got %d, expected %d"), si.iArg1, value, si.iArg2);
                iTestVerdict = ERtvFail;
                }
            }
            break;

        case ESetAttribCase:
            {
            EGLBoolean ret = EglEndpoint().SetEndpointAttrib(dpy, endpoint, si.iArg1, si.iArg2);
            CHECK_RETURN(ret, EGL_FALSE, _S("eglSetEndpointAttrib"));
            }
            break;

        case EDestroyEglImageCase:
            {
            EGLBoolean ret = EglEndpoint().DestroyImage(dpy, image);
            CHECK_RETURN(ret, EGL_FALSE, _S("eglDestroyImageKHR"));
            }
			break;

        case ECreateVgImageCase:
            {
            // For a VgImage to be possible to create, we need to have a EglSurface.
            ActivateVgContextL();

            TRAPD(err, iVgImage[si.iImageIndex] = CTestVgEglImage::NewL(iEglImage[si.iEndpointIndex]));
            if ((si.iFlags & EExpectFailureMask) && err == KErrNone)
                {
                REMOTE_ERR_PRINTF1(_L("Successfully created VGImage when we expected an error"));
                iTestVerdict = ERtvFail;
                delete iVgImage[si.iImageIndex];
                iVgImage[si.iImageIndex] = NULL;
                }
            else if (!(si.iFlags & EExpectFailureMask) && err != KErrNone)
                {
                REMOTE_ERR_PRINTF1(_L("Failed to create VGImage when we expected to succeed"));
                iTestVerdict = ERtvFail;
                }
            }
            break;

        // Test that a vgImage can be used. We do NOT test the content for anything in particular, since
        // the current usage of this is to do checking on a vgImage after endpoint is destroyed, and the
        // specification is that the vgImage is undefined under this condition.
        case ETestVgImageValidCase:
            {
            ActivateVgContextL();
            CTestVgEglImage *vgImage = iVgImage[si.iImageIndex];
            if (!vgImage)
                {
                // Image not usable!
                REMOTE_ERR_PRINTF1(_L("VGImage is not present"));
                iTestVerdict = ERtvFail;
                }
            else
                {
                vgDrawImage(vgImage->VGImage());
                VGint err = vgGetError();
                if (err != VG_NO_ERROR)
                    {
                    iTestVerdict = ERtvFail;
                    }
#if 0
                TSize size = vgImage->Size();
                // Now read the pixels in four corners and the middle to check if the image is still "working".
                vgImage->Pixel(0, 0);
                vgImage->Pixel(size.iWidth-1, size.iHeight-1);
                vgImage->Pixel(0, size.iHeight-1);
                vgImage->Pixel(size.iWidth-1, 0);
                vgImage->Pixel(size.iWidth >> 1, size.iHeight >> 1);
#endif
                }
            // If we get here, the image is "working" - we expect to panic or crash if it's not...
            }
            break;

        case EDestroyVgImageCase:
            delete iVgImage[si.iImageIndex];
            iVgImage[si.iImageIndex] = NULL;
            break;

        case ERequestNotificationCase:
            RequestNotificationL(aParams, aArgs);
            break;

        case ECancelNotificationCase:
            CHECK_BOOL_RET(EndpointCancelNotification, "eglEndpointCancelNotification");
            break;

        case EWaitForNotificationCase:
            {
            RTimer timer;
            TInt err = timer.CreateLocal();
            if (err != KErrNone)
                {
                REMOTE_INFO_PRINTF2(_L("Could not create timer. Error=%d"), err);
                iTestVerdict = ERtvFail;
                }
            else
                {
                TRequestStatus timerStatus = KRequestPending;
                timer.HighRes(timerStatus, si.iArg1);
                // Note that the requeststatus is set to KRequestPending by
                // eglEndpointRequestNotificationNOK(), so we don't do that
                // before waiting. See below for more comments.
                TRequestStatus *requestStatus = &iRequestStatus[si.iEndpointIndex];
                User::WaitForRequest(timerStatus, *requestStatus);
                TInt result = KErrNotReady;  // Give it some ERROR value that is unlikely to happen later.
                timer.Cancel();
                timer.Close();
                if (timerStatus == KErrNone && *requestStatus == KRequestPending)
                    {
                    result = KErrTimedOut;
                    }
                else
                    {
                    result = requestStatus->Int();
                    }
                // Reset the request - this allows us to (ab-)use this request to
                // wait for things that aren't completing, etc.
                *requestStatus = KRequestPending;
                if ((si.iFlags & EExpectError) && result >= KErrNone)
                    {
                    iTestVerdict = ERtvFail;
                    REMOTE_INFO_PRINTF1(_L("Expected failure, but result was a success"));
                    }
                else if (!(si.iFlags & EExpectError) && result < KErrNone)
                    {
                    iTestVerdict = ERtvFail;
                    REMOTE_INFO_PRINTF1(_L("Expected success, but result was a failure"));
                    }
                if (result != si.iErrorExpected)
                    {
                    iTestVerdict = ERtvFail;
                    REMOTE_INFO_PRINTF3(_L("EWaitForNotificationCase: Expected error %d, got %d"), si.iErrorExpected, result);
                    }
                }
            }
            break;

        case EGetEndpointDirtyAreaCase:
            GetEndpointDirtyAreaL(aParams, aArgs);
            break;

        case ETerminateCase:
            REMOTE_INFO_PRINTF1(_L("calling EglEndL()"));
            EglEndL();
            break;
            
        // Memory leak checking functions.
        case ECheckForMemoryLeaks:
            CheckForMemoryLeaks();
            break;
            
        case ECheckForMemoryLeaksFinish:
            CheckForMemoryLeaksFinish();
            break;
            
            
        case EStartLoadThreadCase:
            StartThreadL(si.iEndpointIndex);
            break;
            
        case EEndLoadThreadCase:
            EndThread(si.iEndpointIndex);
            break;
            
        case ESetVerdictCase:
            iTestVerdict = ConvertToLocalVerdict(si.iEndpointIndex);
            break;
            

        /*
         * Debug cases
         */

        case EBreakPointCase:
            __BREAKPOINT();
            break;

        case ELogEnableCase:
            iLogging = ETrue;
            break;
            
        case EPanicCase:
            User::Panic(_L("EPanicCase"), -1);
            break;

        default:
            REMOTE_ERR_PRINTF2(_L("Invalid testcase %d"), aTestCase);
            User::Invariant();
            break;
        }
    }


// Create thread that consumes some sort of resource (e.g. Heap or GPU memory)
// @param aThreadNumber indicates "which" 
void CEgltest_Remote_Engine::StartThreadL(TInt aThreadNumber)
    {
    const TInt KStackSize = 12000;
    const TInt KHeapMinSize = 16000;
    const TInt KHeapMaxSize = 1000000;

    if (aThreadNumber >= KMaxLoadThreads)
        {
        User::Panic(_L("StartThreadL"), __LINE__);
        }
    
    __e32_atomic_store_rel32(&iStopThreadFlag[aThreadNumber], EFalse);
    
    TUint32 random = Math::Random();
    TName threadName;
    _LIT(KThreadNameFormat, "%S-%u");

    // Create a load-thread.
    _LIT(KThreadName, "EpTestLoadThread");
    threadName.Format(KThreadNameFormat, &KThreadName, random);
    TThreadFunction threadFunc = GetThreadFunction(aThreadNumber);
    if (threadFunc == NULL)
        {
        REMOTE_ERR_PRINTF2(_L("Requested thread function %d, got NULL pointer back!"), aThreadNumber);
        User::Leave(KErrArgument);
        }
    TInt err = iLoadThread[aThreadNumber].Create(threadName, threadFunc, 
                    KStackSize, KHeapMinSize, KHeapMaxSize, this, EOwnerThread);
    if(err != KErrNone)
        {
        REMOTE_ERR_PRINTF2(_L("Could not create load thread - err=%d"), err);
        User::Leave(err);
        }
    iLoadThread[aThreadNumber].Resume();
    }


void CEgltest_Remote_Engine::EndThread(TInt aThreadNumber)
    {
    if (aThreadNumber >= KMaxLoadThreads)
        {
        User::Panic(_L("StartThreadL"), __LINE__);
        }

    TRequestStatus status;
    iLoadThread[aThreadNumber].Logon(status);
    // Tell thread to go away. 
    __e32_atomic_store_rel32(&iStopThreadFlag[aThreadNumber], ETrue);
    User::WaitForRequest(status);
    iLoadThread[aThreadNumber].Close();
    }


void CEgltest_Remote_Engine::CreateEndpointCaseL(const TRemoteTestParams &aParams, const TRemoteTestArgs& aArgs)
    {
    const TEngineTestCase &si = aParams.iEndpointEngine.iEngineTestCase;
    EGLDisplay dpy = aArgs.iDisplay;
    EGLEndpointNOK endpoint = aArgs.iEndpoint;
    const TSurfaceParamsRemote& cp = aParams.iEndpointEngine.iSurfaceParams;

    CEnumParams* endpointTypeParams = CEnumParams::NewLC(!!(si.iFlags & EUseBadEndpointType),
                                                            EGL_ENDPOINT_TYPE_CONSUMER_NOK);
    CEnumParams* sourceParams = CEnumParams::NewLC(!!(si.iFlags & EUseBadSourceType),
                                                   EGL_TSURFACEID_NOK);
    CSurfaceIdParams *surfParams = CSurfaceIdParams::NewLC(!!(si.iFlags & EUseBadSurfaceId),
                                                           cp.iSurfaceId);

    TInt endpointTypeCount = endpointTypeParams->Count();
    TInt sourceCount = sourceParams->Count();
    TInt surfCount = surfParams->Count();

    for(TInt typeIter = 0; typeIter < endpointTypeCount; typeIter++)
        {
        EGLenum type = (*endpointTypeParams)[typeIter];
        for(TInt sourceIter = 0; sourceIter < sourceCount; sourceIter++)
            {
            EGLenum source_type = (*sourceParams)[sourceIter];

            for(TInt surfIter = 0; surfIter < surfCount; surfIter++)
                {
                EGLEndpointSourceNOK source = (EGLEndpointSourceNOK)(&(*surfParams)[surfIter]);
                EGLint *attrib_list = cp.iCommonParams.iUseAttribList?
                    const_cast<EGLint *>(cp.iCommonParams.iAttribs):NULL;

                endpoint = EglEndpoint().CreateEndpoint(dpy, type, source_type, source, attrib_list);
                CHECK_RETURN_CAST(endpoint, EGL_NO_ENDPOINT_NOK, _S("eglCreateEndpoint"));
                if (endpoint != EGL_NO_ENDPOINT_NOK)
                    {
                    iEndpoints[si.iEndpointIndex] = endpoint;
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(3);
    }


void CEgltest_Remote_Engine::ReleaseImageCaseL(const TRemoteTestParams& aParams, const TRemoteTestArgs& aArgs)
    {
    const TEngineTestCase &si = aParams.iEndpointEngine.iEngineTestCase;
    EGLDisplay dpy = aArgs.iDisplay;
    EGLImageKHR image = aArgs.iImage;
    EGLEndpointNOK endpoint = aArgs.iEndpoint;

    static const EGLenum validAPIs[] = { EGL_OPENVG_API, EGL_OPENGL_API, EGL_OPENGL_ES_API };
    const TInt validAPIcount = sizeof(validAPIs) / sizeof(validAPIs[0]);

    CEnumParams* enumParams = CEnumParams::NewLC(!!(si.iFlags & EUseBadApi),
            validAPIs, validAPIcount, 0);
    for(TInt enumIter = 0; enumIter < enumParams->Count(); enumIter++)
        {
        EGLenum api = (*enumParams)[enumIter];
        EGLBoolean ret = EglEndpoint().ReleaseImage(dpy, endpoint, image, api);
        CHECK_RETURN(ret, EGL_FALSE, _S("eglReleaseImage"));
        }
    CleanupStack::PopAndDestroy(enumParams);
    }

void CEgltest_Remote_Engine::RequestNotificationL(const TRemoteTestParams& aParams, const TRemoteTestArgs &aArgs)
    {
    const TEngineTestCase &si = aParams.iEndpointEngine.iEngineTestCase;
    EGLDisplay dpy = aArgs.iDisplay;
    EGLEndpointNOK endpoint = aArgs.iEndpoint;

    CSyncParams* enumParams = CSyncParams::NewLC(!!(si.iFlags & EUseBadSync), &iRequestStatus[si.iEndpointIndex]);
    for(TInt enumIter = 0; enumIter < enumParams->Count(); enumIter++)
        {
        TRequestStatus* sync = (*enumParams)[enumIter];
        EGLBoolean ret = EglEndpoint().EndpointRequestNotification(dpy, endpoint, sync);
        CHECK_RETURN(ret, EGL_FALSE, _S("eglEndpointRequestNotification"));
        }
    CleanupStack::PopAndDestroy(enumParams);
    }

// Mark either side of "rects" with something that we can detect.
// Must not be a valid rect coordinate - which is unlikely for this
// number (regardless of endianness), since it's roughly 0x40000000.
static const EGLint KMarker = 'NTCH';
// Allow space for this number of rectangles either side of the actual buffer.
static const TInt KBufferArea = 2;

void CEgltest_Remote_Engine::DoCheckRectsL(EGLint *aRectsBuffer, EGLint aRectCount, EGLint aMaxRects,
                                           TInt aRectsIndex, const TRect aSurfaceRect)
    {
    // Right now, this testing only supports "full surface" rectangles.
    ASSERT(aRectsIndex == 0);
    EGLint *rects = aRectsBuffer+KBufferArea * 4;

    // First, check the rects returned by the call. Should not be equal to KMarker.
    // For example, if we ask for 4 rects, and only two rects are filled in, index
    // 0 and 1 are checked that they are properly filled in.
    for (TInt i = 0; i < aRectCount * 4; i++)
        {
        if (rects[i] == KMarker)
            {
            iTestVerdict = ERtvFail;
            REMOTE_INFO_PRINTF3(_L("Seems the dirty area wasn't filled in properly! Got 0x%08x at %d"), rects[i], i);
            }
        }
    // Check the area not supposed to be filled in! All this should contain KMArker!
    // Check that the dirty area get call didn't fill in any memory
    // beyond the rectangles returned. Say we asked for 4 rectangles,
    // and two were returned, this will check that index 2 & 3 were
    // not modified. If we ask for 4 rects and get 4 rects back, nothing
    // is done here.
    for(TInt i = aRectCount * 4; i < aMaxRects * 4; i++)
        {
        if (rects[i] != KMarker)
            {
            iTestVerdict = ERtvFail;
            REMOTE_INFO_PRINTF3(_L("Seems the dirty area filled beyond the number of rects that it returned! Got 0x%08x at %d"), rects[i], i);
            }
        }
    // Check the "bufferaea" before the actual rects - MUST not be touched.
    for(TInt i = 0; i < KBufferArea * 4; i++)
        {
        if (aRectsBuffer[i] != KMarker)
            {
            iTestVerdict = ERtvFail;
            REMOTE_INFO_PRINTF3(_L("Seems the dirty area walked outside it's allowed memory! Got 0x%08x at %d"), rects[i], i);
            }
        }
    // Check the buffer area AFTER the buffer we gave - again, the
    // production code should ABSOLUTELY not write here.
    for(TInt i = (aMaxRects + KBufferArea) * 4; i < (aMaxRects + KBufferArea * 2) * 4; i++)
        {
        if (aRectsBuffer[i] != KMarker)
            {
            iTestVerdict = ERtvFail;
            REMOTE_INFO_PRINTF3(_L("Seems the dirty area walked outside it's allowed memory! Got 0x%08x at %d"), rects[i], i);
            }
        }
    if (aRectsIndex == 0)
        {
        // Check that rectangle matches the full surface extent.
        // We should only have ONE rectangle in this case!
        if (aRectCount != 1)
            {
            REMOTE_INFO_PRINTF2(_L("Expected 1 rectangle returned - got %d"), aRectCount);
            iTestVerdict = ERtvFail;
            }
        else
            {
            TRect returnedRect = TRect(rects[0], rects[1], rects[2], rects[3]);
            if (returnedRect != aSurfaceRect)
                {
                REMOTE_INFO_PRINTF1(_L("rectangles do not match!"));
                }
            }
        }
    // TODO: To support flexible sets of dirty area we need an else on the
    // above if-statement. However, with the current reference and the planned
    // third party known at this point, only "full surface" will ever be
    // returned.
    }

void CEgltest_Remote_Engine::GetEndpointDirtyAreaL(const TRemoteTestParams& aParams, const TRemoteTestArgs& aArgs)
    {
    const TEngineTestCase &si = aParams.iEndpointEngine.iEngineTestCase;
    EGLDisplay dpy = aArgs.iDisplay;
    EGLEndpointNOK endpoint = aArgs.iEndpoint;

    EGLint *rects;
    EGLint *rectsBuffer = NULL;
    const TInt actualRectsSize = (si.iArg2 + KBufferArea * 2) * 4;
    // We don't use the "parameter expansion" for bad rects value.
    // This is because it's so easy to just add it here, and there is only one bad
    // value that is recognisable.
    if (si.iFlags & (EUseNullRects | EUseBadRects))
        {
        rects = NULL;
        }
    else
        {
        rectsBuffer = new EGLint[actualRectsSize];
        CleanupStack::PushL(rectsBuffer);
        rects = rectsBuffer + (KBufferArea * 4);
        for(TInt i = 0; i < actualRectsSize; i++)
            {
            rectsBuffer[i] = KMarker;
            }
        }
    EGLBoolean collapse = (si.iFlags & EUseCollapseArea)?EGL_TRUE:EGL_FALSE;
    EGLint ret = EglEndpoint().GetEndpointDirtyArea(dpy, endpoint, rects, si.iArg1, si.iArg2, collapse);
    if (!(si.iFlags & EExpectFailureMask))
        {
        if (rectsBuffer)
            {
            TInt rectsIndex = si.iImageIndex;  // ImageIndex is used for rects!
            const TSurfaceParamsRemote &surfParams = aParams.iEndpointEngine.iSurfaceParams;
            // TODO: If the surface has been downscaled, we need to modify this rectangle.
            // We can only know if it's downsampled by getting the image, converting to a VGImage,
            // and getting the size of the VGImage. It can be done, but we would need to make
            // sure the imageindex matches the endpointindex, as imageindex is used by the
            // rectsindex (above).
            TRect surfaceRect = TRect(0, 0, surfParams.iCommonParams.iXSize-1, surfParams.iCommonParams.iYSize-1);
            DoCheckRectsL(rectsBuffer, ret, si.iArg2, rectsIndex, surfaceRect);
            }
        }
    if (rectsBuffer)
        {
        CleanupStack::PopAndDestroy(rectsBuffer);
        }
    if (ret != 0 || (si.iFlags & EExpectFailureMask))
        {
        CHECK_RETURN(ret, EGL_FALSE, _S("eglGetEndpointDirtyArea"));
        }
    }


TInt CEgltest_Remote_Engine::LoadHeapMemory(TAny *aSelf)
    {
    CEgltest_Remote_Engine* self = reinterpret_cast<CEgltest_Remote_Engine*>(aSelf);
    User::SwitchHeap(self->iMainThreadHeap);
    CTrapCleanup *cleanUpStack = CTrapCleanup::New();
    if (!cleanUpStack)
       {
       // Can't use INFO_PRINTF here, as we have not yet
       // created the logger object - nor can we until we have
       // a working cleanupstack, so we just do our best at a 
       // reasonable error message.
       RDebug::Printf("Could not allocate memory for cleanupStack!");
       User::Panic(_L("LoadThread"), __LINE__);
       return KErrNoMemory;
       }

    TRAPD(err, self->LoadHeapMemoryL());
    delete cleanUpStack;
    if (err != KErrNone)
        {
        RDebug::Printf("LoadThreadL left with %d", err);
        User::Panic(_L("LoadThread"), __LINE__);
        }
    return err;    
    }


void CEgltest_Remote_Engine::LoadHeapMemoryL()
    {
    const TInt KMaxAllocs = 40000;
    char **ptr = new char*[KMaxAllocs];
    TInt nAllocs = 0;
    while(!__e32_atomic_load_acq32(&iStopThreadFlag[EThreadLoadHeapMemory]))
        {
        char *p = new char[1000];
        if (p)
            {
            if (nAllocs >= KMaxAllocs)
                {
                User::Panic(_L("KMaxAllocs"), -3);
                }
            ptr[nAllocs++] = p;
            }
        else
            {
            RDebug::Printf("Memory full after %d allocations - freeing some", nAllocs);
            // Now release 1/4 of the allocations...
            TInt nRelease = nAllocs / 4;
            for(int i = 0; i < nRelease; i++)
                {
                // Decrement first, then use as index.
                delete [] ptr[--nAllocs];
                }
            User::After(10 * 1000);   // Let others run for a bit
            }
        }
    // Done - let's deallocate.
    while(nAllocs)
        {
        delete [] ptr[--nAllocs];
        }
    delete [] ptr;
    eglReleaseThread();
    }


TInt CEgltest_Remote_Engine::LoadGpuMemory(TAny* aSelf)
    { 
    CEgltest_Remote_Engine* self = reinterpret_cast<CEgltest_Remote_Engine*>(aSelf);
    CTrapCleanup *cleanUpStack = CTrapCleanup::New();
    if (!cleanUpStack)
       {
       // Can't use INFO_PRINTF here, as we have not yet
       // created the logger object - nor can we until we have
       // a working cleanupstack, so we just do our best at a 
       // reasonable error message.
       RDebug::Printf("Could not allocate memory for cleanupStack!");
       User::Panic(_L("LoadThread"), __LINE__);
       return KErrNoMemory;
       }

    TRAPD(err, self->LoadGpuMemoryL());
    delete cleanUpStack;
    if (err != KErrNone)
        {
        RDebug::Printf("LoadThreadL left with %d", err);
        User::Panic(_L("LoadThread"), __LINE__);
        }
    return err;
    }


void CEgltest_Remote_Engine::LoadGpuMemoryL()
    {
    const TInt KMaxSurfaceAllocs = 1000;
    CSurface **surfaces = new CSurface*[KMaxSurfaceAllocs];
    ENGINE_ASSERT(surfaces);
    TInt nSurfaces = 0;
    while(!__e32_atomic_load_acq32(&iStopThreadFlag[EThreadLoadGpuMemory]))     
        {
        ENGINE_ASSERT(nSurfaces < KMaxSurfaceAllocs);
        CSurface* s = CSurface::SurfaceFactoryL(ESurfTypePBuffer);
        if (s)
            {
            TRAPD(err, s->CreateL(ELargeSurface));
            if (err == KErrNone)
                {
                if (nSurfaces >= KMaxSurfaceAllocs)
                    {
                    User::Panic(_L("KMaxAllocs"), -3);
                    }
                surfaces[nSurfaces++] = s;
                s->DrawContentL(TRgb(0x10, 0x20, 0xB0));
                }
            else
                {
                delete s;
                s = NULL;
                }
            }
        if (!s)
            {
            User::After(100 * 1000);
            TInt nRelease = nSurfaces / 4;
            for(TInt i = 0; i < nRelease; i++)
                {
                delete surfaces[--nSurfaces];
                surfaces[nSurfaces] = NULL;
                }
            User::After(100 * 1000); // 100 ms. 
            }
        }
    while(nSurfaces)
        {
        delete surfaces[--nSurfaces];
        }
    delete [] surfaces;
    eglReleaseThread();
    }



TThreadFunction CEgltest_Remote_Engine::GetThreadFunction(TInt aThreadNumber)
    {
    switch(aThreadNumber)
        {
        case EThreadLoadHeapMemory:
            return LoadHeapMemory;
        case EThreadLoadGpuMemory:
            return LoadGpuMemory;
        }
    RDebug::Printf("%s:%d: Unknown thread function %d", __FILE__, __LINE__, aThreadNumber);
    return NULL;
    }
