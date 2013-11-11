// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "egltest_threadedstress.h"
#include "eglendpointwrap.h"
#include "egltest_endpoint_images.h"
#include "egltest_threadmonitor.h"
#include <e32atomics.h>
#include "egltest_endpoint_images.h"
#include <e32math.h>


//Private Helper Class Declarations-----------------------------------------------

class CTightLoopThread : public CBase, public MLog
    {
public:
    ~CTightLoopThread();
    
    //Control the loop from the controlling thread. 
    //Calling Start() more than once causes panic.
    void Start();
    TRemoteTestVerdict Stop();
    TThreadId ThreadId() const;
    
protected:
    CTightLoopThread();
    void ConstructL(TBool aSharedHeap);
    MLog& Logger() const;
    void Log(const TText8* aFile, TInt aLine, TInt aSeverity, TRefByValue<const TDesC> aFmt, ...);
    
    //To be implemented by derived class.
    virtual void SetupInThreadContextL() = 0;
    virtual void TeardownInThreadContextL() = 0;
    virtual TBool ExecuteInnerLoopBody() = 0;
    
private:
    static TInt ThreadEntryPoint(TAny* aSelf);
    void EnterThreadLoopL();

private:
    RThread iThread;
    TRequestStatus iNotifyStart;
    volatile TBool iNotifyStop;
    TBool iHasBeenStarted;
    TBool iHasBeenStopped;
    };


class CEndpointExercise : public CTightLoopThread
    {
public:
    static CEndpointExercise* NewL(TBool aSharedHeap);
    ~CEndpointExercise();
    
    void SetupInThreadContextL();
    void TeardownInThreadContextL();
    TBool ExecuteInnerLoopBody();
    
private:
    CEndpointExercise();
    void ConstructL(TBool aSharedHeap);
    void ExecuteInnerLoopBodyL();
    TInt CheckImage(EGLImageKHR aEglImage);
    
    //Logging helpers.
    void PanicIfError(TInt aError, const TText8* aFile, TInt aLine) const;
    void PanicIfFalse(TBool aBool, const TText8* aFile, TInt aLine) const;
    void LogAndLeaveIfErrorL(TInt aError, const TText8* aFile, TInt aLine) const;
    void LogAndLeaveIfFalseL(TBool aBool, const TText8* aFile, TInt aLine) const;
    #define PANIC_IF_ERROR(ERROR)           PanicIfError((ERROR), (TText8*)__FILE__, __LINE__)
    #define PANIC_IF_FALSE(BOOL)            PanicIfFalse((BOOL), (TText8*)__FILE__, __LINE__)
    #define LOG_AND_LEAVE_IF_ERROR_L(ERROR) LogAndLeaveIfErrorL((ERROR), (TText8*)__FILE__, __LINE__)
    #define LOG_AND_LEAVE_IF_FALSE_L(BOOL)  LogAndLeaveIfFalseL((BOOL), (TText8*)__FILE__, __LINE__)

private:
    TInt iIteration;
    TInt iCurrentColour;
    RSurfaceManager iSurfaceManager;
    RSurfaceUpdateSession iSurfaceUpdate;
    RSurfaceManager::TSurfaceCreationAttributesBuf iSurfaceAttribs;
    EGLDisplay iDisplay;
    TEglEndpointWrap iEglEp;
    CEglWindowSurface* iDummyWindowSurface;
    };

//--------------------------------------------------------------------------------


//Cleanup Items used through out tests--------------------------------------------

struct TCleanupSurface
    {
    RSurfaceManager* iSurfaceManager;
    TSurfaceId iSurfaceId;
    };
static void CleanupSurface(TAny* aCleanupSurface)
    {
    TCleanupSurface* surface = static_cast<TCleanupSurface*>(aCleanupSurface);
    TInt err = surface->iSurfaceManager->CloseSurface(surface->iSurfaceId);
    ASSERT(err == KErrNone);
    }


struct TCleanupEndpoint
    {
    EGLDisplay iDisplay;
    EGLEndpointNOK iEndpoint;
    };
static void CleanupEndpoint(TAny* aCleanupEndpoint)
    {
    TCleanupEndpoint* endpoint = static_cast<TCleanupEndpoint*>(aCleanupEndpoint);
    TEglEndpointWrap ep;
    ASSERT(ep.Error() == KErrNone);
    EGLBoolean err = ep.DestroyEndpoint(endpoint->iDisplay, endpoint->iEndpoint);
    ASSERT(err);
    }


struct TCleanupImage
    {
    EGLDisplay iDisplay;
    EGLEndpointNOK iEndpoint;
    EGLImageKHR iImage;
    };
static void CleanupImage(TAny* aCleanupImage)
    {
    TCleanupImage* image = static_cast<TCleanupImage*>(aCleanupImage);
    TEglEndpointWrap ep;
    ASSERT(ep.Error() == KErrNone);
    EGLBoolean err = ep.ReleaseImage(image->iDisplay, image->iEndpoint, image->iImage, EGL_NONE);
    ASSERT(err);
    }


static void CleanupPointerArray(TAny* aPointerArray)
    {
    RPointerArray<CEndpointExercise>* array = static_cast<RPointerArray<CEndpointExercise>*>(aPointerArray);
    array->ResetAndDestroy();
    }

//--------------------------------------------------------------------------------


//Utility Functions---------------------------------------------------------------

inline TInt RandomNumberInRange(TInt aMin, TInt aMax)
    {
    if(aMin > aMax)
        {
        TInt temp = aMin;
        aMin = aMax;
        aMax = temp;
        }
    
    //Scale and offset to put random into the range inclusively.
    TUint range = aMax - aMin;
    TUint random  = Math::Random() % (range + 1);
    return (TInt)random + aMin;
    }


inline TReal Square(TReal aNumber)
    {
    return aNumber * aNumber;
    }


static TBool SamplesAreIncreasing(TInt* aSampledData, TInt aNumSamples)
    {
    //Naive linear least squares to get gradient of fit line and correlation coefficient.
    //Using TReal to avoid worrying about wrap.
    
    TReal n = aNumSamples;
    TReal sumX = 0.0;
    TReal sumXSq = 0.0;
    TReal sumY = 0.0;
    TReal sumYSq = 0.0;
    TReal sumXTimesY = 0.0;
    
    for(TInt i=0; i < aNumSamples; i++)
        {
        TReal x = (TReal)(i + 1);
        TReal y = (TReal)aSampledData[i];
        sumX += x;
        sumXSq += Square(x);
        sumY += y;
        sumYSq += Square(y);
        sumXTimesY += x * y;
        }
    
    TReal xBar = sumX / n;
    TReal yBar = sumY / n;
    
    TReal gradient = (sumXTimesY - (n * xBar * yBar)) / (sumXSq - (n * Square(xBar)));
    TReal correlation = Square(sumXTimesY - (n * xBar * yBar)) / ((sumXSq - (n * Square(xBar))) * (sumYSq - (n * Square(yBar))));
    
    //If the gradient is positive and the correlation coefficient is high, the samples are increasing.
    return (correlation > 0.8) && (gradient > 0.0);
    }

//--------------------------------------------------------------------------------


//CTightLoopThread----------------------------------------------------------------

CTightLoopThread::CTightLoopThread() :
    iNotifyStop(EFalse),
    iHasBeenStarted(EFalse),
    iHasBeenStopped(EFalse)
    {
    }


void CTightLoopThread::ConstructL(TBool aSharedHeap)
    {
    //Stack and heap sizes.
    static const TInt KStackSize =   0x2000;      //  8KB
    static const TInt KHeapMinSize = 0x1000;      //  4KB
    static const TInt KHeapMaxSize = 0x1000000;   // 16MB
    
    //The new thread either has its own heap or shares ours.
    if(aSharedHeap)
        {
        User::LeaveIfError(iThread.Create(KNullDesC, ThreadEntryPoint, KStackSize, NULL, this, EOwnerThread));
        }
    else
        {
        User::LeaveIfError(iThread.Create(KNullDesC, ThreadEntryPoint, KStackSize, KHeapMinSize, KHeapMaxSize, this, EOwnerThread));
        }
    
    //Resume and rendezvous.
    iThread.Resume();
    TRequestStatus rendezvous;
    iThread.Rendezvous(rendezvous);
    User::WaitForRequest(rendezvous);
    User::LeaveIfError(rendezvous.Int());
    }


MLog& CTightLoopThread::Logger() const
    {
    return *const_cast<CTightLoopThread*>(this);
    }


class TOverflowTruncate : public TDesOverflow
    {
public:
    virtual void Overflow(TDes& /*aDes*/)
        {
        //Do nothing - just let it truncate.
        }
    };


void CTightLoopThread::Log(const TText8* aFile, TInt aLine, TInt aSeverity, TRefByValue<const TDesC> aFmt, ...)
    {
    TOverflowTruncate overflow;
    VA_LIST list;
    VA_START(list, aFmt);
    TBuf<0x100> buf;
    buf.AppendFormatList(aFmt, list, &overflow);
    TPtrC8 file8(aFile);
    TBuf<0x100> file16;
    file16.Copy(file8);
    //Lots of effort is required to pump this into the TEF log file, so for now we just print to debug.
    RDebug::Print(_L("CTightLoopThread: %S:%d, Severity=%d, Message=\"%S\""), &file16, aLine, aSeverity, &buf);
    }


CTightLoopThread::~CTightLoopThread()
    {
    //Shutdown the thread according to the state it is in.
    if(!iHasBeenStarted)
        {
        TRequestStatus* notifyStart = &iNotifyStart;
        iThread.RequestComplete(notifyStart, KErrAbort);
        }
    if(iHasBeenStarted && !iHasBeenStopped)
        {
        Stop();
        }
    iThread.Close();
    }


void CTightLoopThread::Start()
    {
    ASSERT(!iHasBeenStarted);
    TRequestStatus* notifyStart = &iNotifyStart;
    iThread.RequestComplete(notifyStart, KErrNone);
    iHasBeenStarted = ETrue;
    }


TRemoteTestVerdict CTightLoopThread::Stop()
    {
    ASSERT(iHasBeenStarted);
    ASSERT(!iHasBeenStopped);
    
    TRequestStatus logon;
    iThread.Logon(logon);
    __e32_atomic_store_rel32(&iNotifyStop, ETrue);
    User::WaitForRequest(logon);
    
    TExitType exitType = iThread.ExitType();
    iThread.Close();
    iHasBeenStopped = ETrue;
    
    switch(exitType)
        {
        case EExitKill:
            //Terminated normally (since we never call kill).
            return ERtvPass; 
        
        case EExitPanic:
            //Thread panicked.
            return ERtvPanic;
            
        default:
            //Any other option should be impossible in our use case.
            ASSERT(0);
        }
    return ERtvAbort;
    }


TThreadId CTightLoopThread::ThreadId() const
    {
    return iThread.Id();
    }


TInt CTightLoopThread::ThreadEntryPoint(TAny* aSelf)
    {
    CTightLoopThread* self = static_cast<CTightLoopThread*>(aSelf);
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    TRAPD(err,
        //Create active scheduler.
        CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
        CleanupStack::PushL(scheduler);
        CActiveScheduler::Install(scheduler);

        //Setup the draw loop.
        self->EnterThreadLoopL();

        //Clean up.
        CleanupStack::PopAndDestroy(scheduler);
        );
    
    __ASSERT_ALWAYS(err == KErrNone, User::PanicUnexpectedLeave());
    delete cleanup;
    
    return KErrNone;
    }


void CTightLoopThread::EnterThreadLoopL()
    {
    //Setup the derived class in this thread context.
    TRAPD(err, SetupInThreadContextL());
    
    //Set the request to pending, rendezvous with parent and wait for start signal.
    iNotifyStart = KRequestPending;
    RThread().Rendezvous(err);
    User::WaitForRequest(iNotifyStart);
    
    //Exit immediately if the KErrAbort signal was received.
    TBool keepGoing = ETrue;
    if(iNotifyStart == KErrAbort)
        {
        keepGoing = EFalse;
        }
    else
        {
        ASSERT(iNotifyStart == KErrNone);
        }
    
    //Loop until we are told to stop.
    while(!__e32_atomic_load_acq32(&iNotifyStop) && keepGoing)
        {
        keepGoing = ExecuteInnerLoopBody();
        }
    
    //Teardown the derived class in this thread context.
    TeardownInThreadContextL();
    }

//--------------------------------------------------------------------------------


//CEndpointExercise---------------------------------------------------------------

CEndpointExercise* CEndpointExercise::NewL(TBool aSharedHeap)
    {
    CEndpointExercise* self = new (ELeave) CEndpointExercise();
    CleanupStack::PushL(self);
    self->ConstructL(aSharedHeap);
    CleanupStack::Pop(self);
    return self;
    }


CEndpointExercise::CEndpointExercise()
    {
    }


void CEndpointExercise::ConstructL(TBool aSharedHeap)
    {
    CTightLoopThread::ConstructL(aSharedHeap);
    User::LeaveIfError(iEglEp.Error());
    }


CEndpointExercise::~CEndpointExercise()
    {
    }


void CEndpointExercise::PanicIfError(TInt aError, const TText8* aFile, TInt aLine) const
    {
    if(aError != KErrNone)
        {
        Logger().Log(aFile, aLine, ESevrErr, _L("Panicking due to error %d"), aError);
        User::Panic(_L("EPTHREADEDSTRESS"), aLine);
        }
    }


void CEndpointExercise::PanicIfFalse(TBool aBool, const TText8* aFile, TInt aLine) const
    {
    if(!aBool)
        {
        Logger().Log(aFile, aLine, ESevrErr, _L("Panicking due to failing invariant test"));
        User::Panic(_L("EPTHREADEDSTRESS"), aLine);
        }
    }


void CEndpointExercise::LogAndLeaveIfErrorL(TInt aError, const TText8* aFile, TInt aLine) const
    {
    if(aError != KErrNone)
        {
        Logger().Log(aFile, aLine, ESevrWarn, _L("Abandoning iteration due to error %d"), aError);
        User::Leave(aError);
        }
    }


void CEndpointExercise::LogAndLeaveIfFalseL(TBool aBool, const TText8* aFile, TInt aLine) const
    {
    if(!aBool)
        {
        Logger().Log(aFile, aLine, ESevrWarn, _L("Abandoning iteration due to failing invariant test"));
        User::Leave(KErrUnknown);
        }
    }


TInt CEndpointExercise::CheckImage(EGLImageKHR aEglImage)
    {
    TRAPD
        (err,
        //Convert the image to a CTestVgEglImage
        CTestVgEglImage* vgEglImage = CTestVgEglImage::NewL(aEglImage);
        CleanupStack::PushL(vgEglImage);
        
        //Check the corners and center pixel are the same colour.
        //Since this test is focussed on correct OOM behaviour, 
        //we panic if the functionality is incorrect.
        PANIC_IF_FALSE(vgEglImage->IsSolidColourL());
        
        CleanupStack::PopAndDestroy(vgEglImage);
        );
    return err;
    }


void CEndpointExercise::SetupInThreadContextL()
    {
    //Colour to fill surface with (this is incremented every frame).
    iCurrentColour = 0x88CC44;
    
    //Connections to SUS and surface manager.
    User::LeaveIfError(iSurfaceManager.Open());
    User::LeaveIfError(iSurfaceUpdate.Connect(5));
    
    //Surface attribs to create surface with.
    iSurfaceAttribs().iSize = TSize(100, 100);
    iSurfaceAttribs().iBuffers = 2;
    iSurfaceAttribs().iPixelFormat = EUidPixelFormatARGB_8888_PRE;
    iSurfaceAttribs().iStride = 100 * 4;
    iSurfaceAttribs().iOffsetToFirstBuffer = 0;
    iSurfaceAttribs().iAlignment = 32;
    iSurfaceAttribs().iContiguous = EFalse;
    iSurfaceAttribs().iCacheAttrib = RSurfaceManager::ECached;
    iSurfaceAttribs().iOffsetBetweenBuffers = 0;
    iSurfaceAttribs().iSurfaceHints = NULL;
    iSurfaceAttribs().iHintCount = 0;
    iSurfaceAttribs().iMappable = ETrue;
    
    iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    
    //Create an EglWindowSurface so we have a current context for vg operations.
    iDummyWindowSurface = CEglWindowSurface::NewL();
    iDummyWindowSurface->CreateL(EStandardSurface, TPoint(0, 0));
    iDummyWindowSurface->ActivateL();
    }


void CEndpointExercise::TeardownInThreadContextL()
    {
    delete iDummyWindowSurface;
    iSurfaceUpdate.Close();
    iSurfaceManager.Close();
    }


TBool CEndpointExercise::ExecuteInnerLoopBody()
    {
    TRAPD(err, ExecuteInnerLoopBodyL());
    if(err != KErrNone)
        {
        Logger().Log((TText8*)__FILE__, __LINE__, ESevrWarn, _L("Iteration %d did not run to completion, due to an acceptable error in low memory conditions"), iIteration);
        }
    iIteration++;
    return ETrue;
    }


void CEndpointExercise::ExecuteInnerLoopBodyL()
    {
    //Create a surface.
    TCleanupSurface surface = {&iSurfaceManager, TSurfaceId::CreateNullId()};
    LOG_AND_LEAVE_IF_ERROR_L(iSurfaceManager.CreateSurface(iSurfaceAttribs, surface.iSurfaceId));
    CleanupStack::PushL(TCleanupItem(CleanupSurface, &surface));
    
    //Map surface and get pointer to buffer 0.
    RChunk surfaceChunk;
    TInt offset;
    PANIC_IF_ERROR(iSurfaceManager.MapSurface(surface.iSurfaceId, surfaceChunk));
    CleanupClosePushL(surfaceChunk);
    PANIC_IF_ERROR(iSurfaceManager.GetBufferOffset(surface.iSurfaceId, 0, offset));
    TUint32* buffer = (TUint32*)(surfaceChunk.Base() + offset);
    
    //Fill surface with current colour. This could
    //be much faster but its good enough for testing.
    TUint32 fillColour = TRgb(iCurrentColour, 255)._Color16MAP();
    for(TInt y=0; y < iSurfaceAttribs().iSize.iHeight; ++y)
        {
        for(TInt x=0; x < iSurfaceAttribs().iSize.iWidth; ++x)
            {
            buffer[x] = fillColour;
            }
        buffer += iSurfaceAttribs().iStride >> 2;
        }
    
    //Create an endpoint for the surface.
    TCleanupEndpoint endpoint = {iDisplay, EGL_NO_ENDPOINT_NOK};
    endpoint.iEndpoint = iEglEp.CreateEndpoint(iDisplay, EGL_ENDPOINT_TYPE_CONSUMER_NOK, EGL_TSURFACEID_NOK, &surface.iSurfaceId, NULL);
    LOG_AND_LEAVE_IF_FALSE_L(endpoint.iEndpoint != EGL_NO_ENDPOINT_NOK);
    CleanupStack::PushL(TCleanupItem(CleanupEndpoint, &endpoint));
    
    //Submit buffer 0 to surface update server.
    TRequestStatus displayed;
    iSurfaceUpdate.NotifyWhenDisplayedXTimes(1, displayed);
    LOG_AND_LEAVE_IF_ERROR_L(iSurfaceUpdate.SubmitUpdate(KAllScreens, surface.iSurfaceId, 0, NULL));
    User::WaitForRequest(displayed);
    
    //Begin streaming. Should not fail since we have submitted a buffer since creating ep.
    LOG_AND_LEAVE_IF_FALSE_L(iEglEp.EndpointBeginStreaming(iDisplay, endpoint.iEndpoint));
    
    //Acquire an image from the endpoint.
    TCleanupImage image = {iDisplay, endpoint.iEndpoint, EGL_NO_IMAGE_KHR};
    image.iImage = iEglEp.AcquireImage(iDisplay, endpoint.iEndpoint);
    LOG_AND_LEAVE_IF_FALSE_L(image.iImage != EGL_NO_IMAGE_KHR);
    CleanupStack::PushL(TCleanupItem(CleanupImage, &image));
    
    //Check that the image we acquired is coherrent.
    LOG_AND_LEAVE_IF_ERROR_L(CheckImage(image.iImage));
    
    //Release image, destroy endpoint, close chunk and close surface.
    CleanupStack::PopAndDestroy(4);
    
    //Modify the colour that we draw.
    iCurrentColour += 16;
    }

//--------------------------------------------------------------------------------


//Remote test step----------------------------------------------------------------

CEglTest_RemoteTestStep_EndpointThreadStress::CEglTest_RemoteTestStep_EndpointThreadStress() :
    CRemoteTestStepBase(ETestUidEndpointThreadStress)
    {
    }


CEglTest_RemoteTestStep_EndpointThreadStress::~CEglTest_RemoteTestStep_EndpointThreadStress()
    {
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointThreadStress::DoStartRemoteTestStepL(const TRemoteTestParams& /*aMessageIn*/)
    {
    REMOTE_INFO_PRINTF1(_L("Starting Remote Test Step."));
    EglStartL();
    return ERtvPass;
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointThreadStress::DoEndRemoteTestStepL(const TRemoteTestParams& /*aMessageIn*/)
    {
    REMOTE_INFO_PRINTF1(_L("Ending Remote Test Step."));
    EglEndL();
    return ERtvPass;
    }


TInt CEglTest_RemoteTestStep_EndpointThreadStress::Timeout() const
    {
    return 120 * 1000000; //2 min.
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointThreadStress::DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aParams)
    {
    switch(aTestCase)
        {
        case 0:     return CrazyThreadingTestCaseL(aParams);
        case 1:     return OutOfHeapMemoryTestCaseL(aParams);
        default:    return ERtvAbort;
        }
    }


//For a detailed description of this test case (GRAPHICS-EGL-594), see the local side cpp file.
TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointThreadStress::CrazyThreadingTestCaseL(const TRemoteTestParams& /*aParams*/)
    {
    //Create the exercises. These run an endpoint exercise in a tight loop in a private thread.
    CEndpointExercise* exercise1 = CEndpointExercise::NewL(EFalse);
    CleanupStack::PushL(exercise1);
    CEndpointExercise* exercise2 = CEndpointExercise::NewL(EFalse);
    CleanupStack::PushL(exercise2);
    
    //Create a monitor to cleanup if any of the threads panic. The controller thread 
    //must be at index zero in the array. This will even work if a deadlock occurs 
    //between the  exercise threads, since the call to stop the exercise will never 
    //return and the framework will eventually time us out. The monitor will notice
    //that the controller thread has panicked and will forward the panic to the exercises.
    RArray<TThreadId> threads;
    CleanupClosePushL(threads);
    threads.AppendL(RThread().Id());
    threads.AppendL(exercise1->ThreadId());
    threads.AppendL(exercise2->ThreadId());
    CThreadMonitor* monitor = CThreadMonitor::NewL(threads);
    CleanupStack::PushL(monitor);
    
    //Start the exercises.
    exercise1->Start();
    exercise2->Start();
    
    //Let the exercises run for 20 seconds.
    User::After(20 * 1000000);
    
    //Stop the exercises and record the results.
    TRemoteTestVerdict result1 = exercise1->Stop();
    TRemoteTestVerdict result2 = exercise2->Stop();
    
    CleanupStack::PopAndDestroy(4, exercise1);
    return (result1 != ERtvPass) ? result1 : result2;
    }


class THeapGobbler
    {
public:
    static THeapGobbler* New(TInt aSize)
        {
        THeapGobbler* self = (THeapGobbler*)new TUint8[sizeof(THeapGobbler) - sizeof(TUint8) + aSize];
        if(!self)
            {
            return NULL;
            }
        self->iSize = aSize;
        self->iNext = NULL;
        return self;
        }
    
public:
    THeapGobbler* iNext;
    TInt iSize;
    TUint8 iMemory[1];
    };


//For a detailed description of this test case (GRAPHICS-EGL-601), see the local side cpp file.
TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointThreadStress::OutOfHeapMemoryTestCaseL(const TRemoteTestParams& aParams)
    {
    const TInt KHeapSizeMin = 0x100000;   //1MB.
    const TInt KHeapSizeMax = 0x10000000; //256MB.

    RHeap* testHeap = User::ChunkHeap(NULL, KHeapSizeMin, KHeapSizeMax, KMinHeapGrowBy, 4);
    if(!testHeap)
        {
        REMOTE_ERR_PRINTF1(_L("Failed to create chunk heap. Aborting."));
        return ERtvAbort;
        }
    RHeap* oldHeap = User::SwitchHeap(testHeap);
    
    CTrapCleanup *cleanUpStack = CTrapCleanup::New();
    if (!cleanUpStack)
        {
        User::SwitchHeap(oldHeap);
        testHeap->Close();
        User::Leave(KErrNoMemory);
        }

    TRemoteTestVerdict verdict = ERtvPass;
    TRAPD(err, verdict = DoOutOfHeapMemoryTestCaseL(aParams));
    
    delete cleanUpStack;
    User::SwitchHeap(oldHeap);
    testHeap->Close();
    
    User::LeaveIfError(err);
    return verdict;
    }

 
TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointThreadStress::DoOutOfHeapMemoryTestCaseL(const TRemoteTestParams& aParams)
    {
    const TInt numExercises = aParams.iEndpointThreadStress.iNumThreads;
    
    const TInt KMinCellSize = 500;
    const TInt KMaxCellSize = 2000;
    const TInt KNumIterations = 20;
    TInt heapAllocSize[KNumIterations];
    TRemoteTestVerdict exerciseResult = ERtvPass;
    
    //One iteration of the outer loop results in one data point for deciding if the heap is leaking or not. 
    for(TInt x=0; x < KNumIterations; x++)
        {
        //Reserving space in these arrays ahead of time to 
        //make cleanup stack manipulation more staightforward.
        RPointerArray<CEndpointExercise> exercises;
        CleanupStack::PushL(TCleanupItem(CleanupPointerArray, &exercises));
        exercises.ReserveL(numExercises);
        RArray<TThreadId> threads;
        CleanupClosePushL(threads);
        threads.ReserveL(numExercises + 1);
        
        //Save the controller thread id for the monitor.
        threads.Append(RThread().Id());
        
        //Create endpoint exercise threads and save the thread Ids for the monitor.
        for(TInt j=0; j < numExercises; j++)
            {
            //Appends can't fail since we have already reserved space.
            //Note that the exercises all share the same heap as this thread.
            exercises.Append(CEndpointExercise::NewL(ETrue));
            threads.Append(exercises[j]->ThreadId());
            }

        //Create a monitor to handle thread cleanup if something panics or deadlocks.
        CThreadMonitor* monitor = CThreadMonitor::NewL(threads);
        
        //Nothing can leave after this.
        CleanupStack::Pop(2);
        
        //Start the exercises.
        for(TInt j=0; j < numExercises; j++)
            {
            exercises[j]->Start();
            }
        
        THeapGobbler* firstCell = NULL;
        THeapGobbler* lastCell = NULL;
        TInt numberOfCells = 0;
        
        for(TInt i=0; i < 2; i++)
            {
            //Allocate random sizes until full.
            THeapGobbler* newCell = THeapGobbler::New(RandomNumberInRange(KMinCellSize, KMaxCellSize));
            while(newCell)
                {
                if(lastCell)
                    lastCell->iNext = newCell;
                if(!firstCell)
                    firstCell = newCell;
                lastCell = newCell;
                numberOfCells++;
                newCell = THeapGobbler::New(RandomNumberInRange(KMinCellSize, KMaxCellSize));
                }
            
            //Let exercise run while heap is full.
            User::After(1 * 1000);
            
            //Deallocate n/4 cells.
            for(TInt n = numberOfCells / 4; n >= 1; --n)
                {
                THeapGobbler* oldCell = firstCell;
                firstCell = oldCell->iNext;
                delete oldCell;
                numberOfCells--;
                if(!firstCell)
                    {
                    lastCell = NULL;
                    ASSERT(numberOfCells == 0);
                    break;
                    }
                }
            
            //Let exercise run while heap is not full.
            User::After(1 * 1000);
            }
        
        //Deallocate all cells.
        while(firstCell)
            {
            THeapGobbler* oldCell = firstCell;
            firstCell = oldCell->iNext;
            delete oldCell;
            }
        lastCell = NULL;
        numberOfCells = 0;
        
        //Stop the exercises and save the result.
        for(TInt j=0; j < numExercises; j++)
            {
            TRemoteTestVerdict ret = exercises[j]->Stop();
            exerciseResult = (exerciseResult == ERtvPass) ? ret : exerciseResult;
            }

        delete monitor;
        threads.Close();
        exercises.ResetAndDestroy();
        
        if(exerciseResult != ERtvPass)
            {
            REMOTE_ERR_PRINTF2(_L("Aborting because the endpoint exercise failed for iteration %d"), x);
            return exerciseResult;
            }
        
        //Save the heap size.
        User::Heap().AllocSize(heapAllocSize[x]);
        }
    
    //Work out if any memory has leaked and return a verdict.
    TBool memoryIsLeaking = SamplesAreIncreasing(heapAllocSize, KNumIterations);
    if(memoryIsLeaking)
        {
        REMOTE_ERR_PRINTF1(_L("Heap memory is increasing over time with high certainty, there is probably a memory leak."));
        }
    else
        {
        REMOTE_INFO_PRINTF1(_L("No heap memory leak detected."));
        }
    return memoryIsLeaking ? ERtvFail : ERtvPass;
    }

//--------------------------------------------------------------------------------
