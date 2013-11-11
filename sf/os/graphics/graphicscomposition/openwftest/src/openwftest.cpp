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
//
// Description:
// OpenWF tests
//
#include <test/extendtef.h>
#include <hal.h>
#include "openwftest.h"

#define KCompositorVersion	0x01023456
#define KGrowCleanupStack	12
#define KMaxNotificationDelay   5000000 //5 seconds
#define KNotificationWarning    100000  //100 ms

_LIT(KTestSectionName, "TestInfo");
_LIT(KImageSectionName, "ImageInfo");
_LIT(KContiguousFlag, "ContiguousFlag");
_LIT(KTolerance, "Tolerance");
_LIT(KTestMode, "TestMode");
_LIT(KSaveImage, "SaveImage");
_LIT(KDir, "Dir");
_LIT(KDefaultDir, "c:\\openwftest\\img\\");
_LIT(KCompositionPause, "CompositionPause");
_LIT(KManualPause, "ManualPause");

//Uncomment this and rebuild if you wish to test if fastpath is enabled using breakpoints
//#define BREAKPOINT_FASTPATH

/*
 * COpenwfTest implementation
 */
COpenwfTest::COpenwfTest()
:iFastpathablePixelFormat(EUidPixelFormatXRGB_8888),
 iNonFastpathablePixelFormat(EUidPixelFormatRGB_565),
 iMappable(ETrue),
 iMaxBuffers(2),
 iMinBuffers(1),
 iCacheAttrib(ECacheNotlisted),
 iDeviceId(WFC_INVALID_HANDLE),
 iSync(EGL_NO_SYNC_KHR),
 iDevice(WFC_INVALID_HANDLE),
 iContext(WFC_INVALID_HANDLE)
	{
	}

COpenwfTest::~COpenwfTest()
	{
    TearDown();
	}

void COpenwfTest::SetupL()
    {    
    iScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iScheduler);

    ReadIniData();
    
    iEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    ASSERT_FALSE(iEGLDisplay == EGL_NO_DISPLAY);
    ASSERT_FALSE(iEGLDisplay == EGL_BAD_ALLOC);
    ASSERT_EQUALS(eglGetError(),EGL_SUCCESS);
    eglInitialize(iEGLDisplay, NULL, NULL);
    ASSERT_EQUALS(eglGetError(),EGL_SUCCESS);
    
    WFCint filterList[] = { WFC_DEVICE_FILTER_SCREEN_NUMBER, WFC_DEFAULT_SCREEN_NUMBER, WFC_NONE};
    ASSERT_TRUE(wfcEnumerateDevices(&iDeviceId, 1, filterList) == 1);
    ASSERT_TRUE(iDeviceId != WFC_INVALID_HANDLE);    

    iDevice = wfcCreateDevice(iDeviceId, NULL);
    ASSERT_TRUE(iDevice != WFC_INVALID_HANDLE);  
    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    iContext = wfcCreateOnScreenContext(iDevice, WFC_DEFAULT_SCREEN_NUMBER, NULL);
    ASSERT_TRUE(iContext != WFC_INVALID_HANDLE);  
    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);

    iFastpathableHeight = wfcGetContextAttribi(iDevice, iContext, WFC_CONTEXT_TARGET_HEIGHT);
    iFastpathableWidth = wfcGetContextAttribi(iDevice, iContext, WFC_CONTEXT_TARGET_WIDTH);
    iStride = iFastpathableWidth * 4;

    iFullScreenTRect = TRect(0,0,iFastpathableWidth,iFastpathableHeight);
    iFullScreenRect[0] = 0;
    iFullScreenRect[1] = 0;
    iFullScreenRect[2] = iFullScreenTRect.Width();
    iFullScreenRect[3] = iFullScreenTRect.Height();
    
    iCenterTRect = TRect(iFastpathableWidth/4,iFastpathableHeight/4,
            3*iFastpathableWidth/4,3*iFastpathableHeight/4);
    iCenterRect[0] = iCenterTRect.iTl.iX;
    iCenterRect[1] = iCenterTRect.iTl.iY;
    iCenterRect[2] = iCenterTRect.Width();
    iCenterRect[3] = iCenterTRect.Height();
    
    iHalfSizeRect[0] = 0;
    iHalfSizeRect[1] = 0;
    iHalfSizeRect[2] = iFastpathableWidth/2;
    iHalfSizeRect[3] = iFastpathableHeight/2;
    
    iUtility = CSurfaceUtility::NewL();

    if((iTestMode == EAutomatic) || iSaveImage)
        {
        // Need to initialise the on screen stream before composition
        // to ensure the content is available later 
        iUtility->GetOnScreenStream(iDevice, iContext);
        }
    if (iSaveImage)
        {   // Create a directory to save images
        iUtility->CreateImagePath(iImageDirectory);                
        }
    iAllowManualPause = ETrue; //tests can stop it (eg dispXtimes tests) 
    
    EGLint attrib_list[1] = {EGL_NONE};
    iSync = eglCreateSyncKHR(iEGLDisplay,EGL_SYNC_REUSABLE_KHR, attrib_list);
    ASSERT_NOT_EQUALS(iSync,EGL_NO_SYNC_KHR);
    ASSERT_EQUALS(eglGetError(),EGL_SUCCESS);
    }

/**
 * test Suite furniture 
 **/
void COpenwfTest::TearDownL()
    {
    // Nothing leaves in this function, 
    // but TearDownL() is needed for the CTestFixture (TEF) interface
    TearDown();
    }

void COpenwfTest::TearDown()
    {
    WFCErrorCode wfcError;
    if (iDevice != WFC_INVALID_HANDLE)
        {
        wfcError = wfcGetError(iDevice);
        if (wfcError != WFC_ERROR_NONE)
            {
            INFO_PRINTF2(_L("TearDown: There was a failure in the test. error=%d"),wfcError);
            }
        }
    if (iOnScreenStream != WFC_INVALID_HANDLE)
        {
        (void)SymbianStreamReleaseReadBuffer(iOnScreenStream, iOnScreenBuffer); //ignore the error
        iOnScreenStream = WFC_INVALID_HANDLE;
        }
    delete iUtility;
    iUtility = NULL;
    if (iContext != WFC_INVALID_HANDLE)
        {
        wfcDestroyContext(iDevice, iContext);
        iContext = WFC_INVALID_HANDLE;
        WFCErrorCode wfcError = wfcGetError(iDevice);
        if (wfcError != WFC_ERROR_NONE)
            {
            INFO_PRINTF2(_L("TearDown: wfcDestroyContext error=%d"),wfcError);
            }
        }
    if (iDevice != WFC_INVALID_HANDLE)
        {
        WFCErrorCode deviceError = wfcDestroyDevice(iDevice);
        iDevice = WFC_INVALID_HANDLE;
        if (deviceError != WFC_ERROR_NONE)
            {
            INFO_PRINTF2(_L("TearDown: wfcDestroyDevice error=%d"),deviceError);
            }
        }
    if (iSync != EGL_NO_SYNC_KHR)
        {
        EGLBoolean eglSyncError = eglDestroySyncKHR(iEGLDisplay, iSync);
        iSync = EGL_NO_SYNC_KHR;
        if (eglSyncError != EGL_TRUE)
            {
            INFO_PRINTF2(_L("TearDown: eglSyncError line %d"),__LINE__);
            }
        }
    if (iEGLDisplay != EGL_NO_DISPLAY)
        {
        EGLBoolean eglTerminateError = eglTerminate(iEGLDisplay);
        iEGLDisplay = EGL_NO_DISPLAY;
        if (eglTerminateError != EGL_TRUE)
            {
            INFO_PRINTF2(_L("TearDown: eglTerminateError line %d"),__LINE__);
            }
        }
    delete iScheduler;
    iScheduler = NULL;
    }

/**
 * To be called at the beginning of tests
 * aActivate default is ON
 */
void COpenwfTest::SetupEmptySceneL(TBool aActivate)
    {
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    wfcSetContextAttribi(iDevice, iContext, WFC_CONTEXT_ROTATION, WFC_ROTATION_0);
    wfcSetContextAttribi(iDevice, iContext, WFC_CONTEXT_BG_COLOR, 0xFFFFFFFF);
    if (aActivate)
        {
        LOG(("OpenWFTest: Running in autonomous composition mode"));
        wfcActivate(iDevice,iContext);
        }
    else
        {
        LOG(("OpenWFTest: Running in non autonomous composition mode"));
        wfcDeactivate(iDevice,iContext);
        }
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit"));
    wfcCommit(iDevice, iContext, WFC_TRUE);   // Compose the scene
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    LOG(("OpenWFTest: Fastpath - setting up scene - fastpath now off"));
    }

void COpenwfTest::ReadIniData()
    {
	//test mode
    TBool returnValue = iConfig.GetInt(KTestSectionName, KTestMode, iTestMode);
    ASSERT_TRUE(returnValue);
    
    //save images
	iSaveImage=EFalse;
    if (iTestMode == EAutomatic)
        {
        returnValue = iConfig.GetBool(KTestSectionName, KSaveImage, iSaveImage);
        }
    
    // Get path for the images to be saved to
    TPtrC dir;
    returnValue = iConfig.GetString(KTestSectionName, KDir, dir);
    if (returnValue)
        {
		iImageDirectory.Set(dir);
        }
    else
        {
		iImageDirectory.Set(KDefaultDir);
        }
    
    // Composition pause in microseconds
    TInt compositionPause;
    returnValue = iConfig.GetInt(KTestSectionName, KCompositionPause, compositionPause);
    ASSERT_TRUE(returnValue);
    iCompositionPause = compositionPause;
    
    // Manual pause in microseconds
    iManualPause = 0;
    if (iTestMode != EAutomatic)
        {
		TInt manualPause;
		returnValue = iConfig.GetInt(KTestSectionName, KManualPause, manualPause);
		ASSERT_TRUE(returnValue);
		iManualPause = manualPause;
        }
    
    // Force contiguous
    TBool contiguousFlag;
    returnValue = iConfig.GetBool(KImageSectionName, KContiguousFlag, contiguousFlag);
    if (returnValue && contiguousFlag)
        {
        iContiguous = EContiguous;
        }
    
    // Test tolerance
    TInt tolerance;
    returnValue = iConfig.GetInt(KImageSectionName, KTolerance, tolerance);
    if (returnValue)
        {
        iTolerance = tolerance;
        }
    }

/**
 * Acquires the on screen stream and buffer number
 * NOTE: this function should be paired with a ReleaseOnScreenStream call
 * Returns ETrue on success
 */
TBool COpenwfTest::AcquireOnScreenStream()
	{
	TInt error = KErrNone;
	ASSERT_TRUE(iOnScreenStream == WFC_INVALID_HANDLE);
    if(iTestMode == EAutomatic)
    	{	//need to get the on screen stream
		iOnScreenStream = iUtility->GetOnScreenStream(iDevice, iContext);
		if(iOnScreenStream == WFC_INVALID_HANDLE)
			{
			INFO_PRINTF1(_L("Failed getting on screen stream"));
			LOG(("OpenWFTest: *** Failed getting on screen stream"));
			return EFalse;
			}
		error = SymbianStreamAcquireReadBuffer(iOnScreenStream, &iOnScreenBuffer);
		if (error != KErrNone)
			{
			LOG(("OpenWFTest: *** Unable to acquire read buffer. error = %d",error));
			INFO_PRINTF2(_L("Unable to acquire read buffer. error = %d"),error);
			}
		return (error == KErrNone);
    	}
	else
		{
		INFO_PRINTF1(_L("No need to get the on screen stream, probably manual checking mode"));
		LOG(("OpenWFTest: *** No need to get the on screen stream, probably manual checking mode"));
		return ETrue;
		}
	}

/**
 * Releases the on screen stream
 */
void COpenwfTest::ReleaseOnScreenStream()
	{
	if (iOnScreenStream != WFC_INVALID_HANDLE)
		{
		(void)SymbianStreamReleaseReadBuffer(iOnScreenStream, iOnScreenBuffer);	//ignore the error
		iOnScreenStream = WFC_INVALID_HANDLE;
		}
	if(iTestMode != EAutomatic && iAllowManualPause)
		{
		INFO_PRINTF1(_L("Performing manual wait"));
		LOG(("OpenWFTest: *** Performing manual wait"));
		TRAP_IGNORE(WaitL(iManualPause));
		}
	if(iTestMode != EAutomatic && !iAllowManualPause)
	    {
        INFO_PRINTF1(_L("Test not allowing a manual wait"));
        LOG(("OpenWFTest: *** Test not allowing a manual wait"));
	    }
	}

/**
 * Checks the rect of the on screen stream
 * Returns ETrue on success
 */
TBool COpenwfTest::CheckOnScreenStreamRect(TRect aRect, const TRgb& aExpectedColor, TInt aTolerance)
	{
    if(iTestMode == EAutomatic)
    	{
		if (iOnScreenStream == WFC_INVALID_HANDLE)
			{
			LOG(("OpenWFTest: *** We don't have access to the on screen stream"));
			INFO_PRINTF1(_L("We don't have access to the on screen stream"));
			return EFalse;
			}
		const TSurfaceId* surfaceId = NULL;
		long bufferIndex;
		TInt error = SymbianStreamGetBufferId(iOnScreenStream,iOnScreenBuffer,&bufferIndex,&surfaceId);
		if (error == KErrNone)
			{
			return CheckRect(*surfaceId,bufferIndex,aRect,aExpectedColor,aTolerance);
			}
		else
			{
			LOG(("OpenWFTest: *** Unable to get stream buffer id. error = %d",error));
			INFO_PRINTF2(_L("Unable to get stream buffer id. error = %d"),error);
			return EFalse;
			}
    	}
    else
    	{	//print out what we're hoping to see for manual testing
		LOG(("OpenWFTest: For rect %d %d %d %d",aRect.iTl.iX,aRect.iTl.iY,aRect.iBr.iX,aRect.iBr.iY));
		LOG(("OpenWFTest: The expected color is r=%d g=%d b=%d",aExpectedColor.Red(),aExpectedColor.Green(),
				aExpectedColor.Blue()));
		INFO_PRINTF5(_L("For rect %d %d %d %d"),aRect.iTl.iX,aRect.iTl.iY,aRect.iBr.iX,aRect.iBr.iY);
		INFO_PRINTF4(_L("The expected color is r=%d g=%d b=%d"),aExpectedColor.Red(),aExpectedColor.Green(),
				aExpectedColor.Blue());
		return ETrue;
    	}
	}

/**
 * checks the color within the given rectangle of the given surface
 * returns ETrue on success
 */
TBool COpenwfTest::CheckRect(const TSurfaceId& aSurface, TInt aBuffer,  TRect aRect,
		const TRgb& aExpectedColor, TInt aTolerance)
	{
	INFO_PRINTF1(_L("Starting pixel checking"));
	LOG(("OpenWFTest: *** Starting pixel checking"));
	// Check pixel data
	
	TBool success = iUtility->CheckRectColor(aSurface, aRect, aBuffer, aExpectedColor, aTolerance);
	if (success)
		{
		INFO_PRINTF1(_L("Finished pixel checking - pass"));
		LOG(("OpenWFTest: *** Finished pixel checking - pass"));
		}
	else
		{
		INFO_PRINTF1(_L("Finished pixel checking - fail"));
		LOG(("OpenWFTest: *** Finished pixel checking - fail"));
		}
    return success;
	}

/**
 * If in automatic mode it will save a TGA image of the on screen stream.
 * It will also print the name of the reference image.
 * Returns ETrue on success
 */
TBool COpenwfTest::CheckOnScreenReferenceImage()
	{
	TInt error = KErrNone;
	//Create image file name
	_LIT(KSeparator, "_");
	_LIT(KTGAImgFormat, ".tga");
	iImageAddress.Zero();
	iImageAddress.Append(iTestName);
	iImageAddress.Append(KSeparator);
	iImageAddress.AppendNum(iImageCounter);
	iImageAddress.Append(KTGAImgFormat);
    
	LOGL((_L("Reference Image:%S"),&iImageAddress));
	INFO_PRINTF2(_L("Reference Image:%S"),&iImageAddress);
	//Add file path
	iImageAddress.Insert(0,iImageDirectory);
	iImageCounter++;
	
    if(iTestMode == EAutomatic && iSaveImage)
    	{
		if (iOnScreenStream == WFC_INVALID_HANDLE)
			{
			LOG(("OpenWFTest: *** We don't have access to the on screen stream",error));
			INFO_PRINTF1(_L("We don't have access to the on screen stream"));
			return EFalse;
			}
		const TSurfaceId* surfaceId = NULL;
		long bufferIndex;
		error = SymbianStreamGetBufferId(iOnScreenStream,iOnScreenBuffer,&bufferIndex,&surfaceId);
		if (error == KErrNone)
			{
			return SaveImage(*surfaceId,bufferIndex);
			}
		else
			{
			LOG(("OpenWFTest: *** Unable to get stream buffer id. error = %d",error));
			INFO_PRINTF2(_L("Unable to get stream buffer id. error = %d"),error);
			return EFalse;
			}
    	}
    return ETrue;
	}

/**
 *  helper function for saving images to files
 *  returns ETrue on success
 */
TBool COpenwfTest::SaveImage(const TSurfaceId& aSurface, TInt aNumOfBuffer)
    {
	if(iTestMode == EAutomatic && iSaveImage)
		{
		INFO_PRINTF1(_L("Start to save image"));
		LOG(("OpenWFTest: *** Start to save image"));
		TBool success = EFalse;
		TRAPD(error, success = iUtility->SaveResultImageTGAL(aSurface, aNumOfBuffer, iImageAddress));
		if(error != KErrNone || !success)
			{
			INFO_PRINTF1(_L("Failed saving image"));
			LOG(("OpenWFTest: *** Failed saving image"));
			return EFalse;
			}
		INFO_PRINTF1(_L("Saved image"));
		LOG(("OpenWFTest: *** Saved image"));
		}
    return ETrue;
    }

/**
 * Basic wait function call. Return amount of other active objects that stopped the scheduler
*/
TInt COpenwfTest::WaitL(TTimeIntervalMicroSeconds32 aDelay)	//aDelay in microseconds
	{
	CActiveWaiter* waiter = NULL;
	waiter = CActiveWaiter::NewL();
    TInt counter = waiter->Wait(aDelay);
	delete waiter;
	return counter;
	}

/**
 * aMaxDelay    The maximum time we are willing to wait in microseconds
 * aNotifications   The amount of notifications we expect to complete, must be >0
 * Returns ETrue on success
*/
TBool COpenwfTest::WaitForNotifications(TTimeIntervalMicroSeconds32 aMaxDelay,TInt aNotificatons)
    {
    if (aNotificatons == 0)
        {   //use WaitL if no notifications are expected
        return EFalse;
        }
    TTime before;
    TTime after;
    before.UniversalTime();
    TTimeIntervalMicroSeconds diff;
    TInt counter = aNotificatons;
    CActiveWaiter* waiter = NULL;
    TRAP_IGNORE(waiter = CActiveWaiter::NewL());
    if (!waiter)
        {
        INFO_PRINTF1(_L("Failed creating CActiveWaiter"));
        LOG(("OpenWFTest: *** Failed creating CActiveWaiter"));
        return EFalse;
        }
    waiter->StartWait(aMaxDelay);
    CActiveScheduler::Start();
    while(!waiter->iRun)
        {
        after.UniversalTime();
        diff = after.MicroSecondsFrom(before);
        if (diff > KNotificationWarning)
            {
            INFO_PRINTF2(_L("note: Notification took a long time to complete: %ld microseconds"),diff.Int64());
            LOG(("OpenWFTest: *** note: Notification took a long time to complete: %ld microseconds",diff.Int64()));
            }
        counter--;
        if (counter == 0)
            {   //all expected notifications were completed
            delete waiter;
            return ETrue;
            }
        CActiveScheduler::Start();
        }
    INFO_PRINTF2(_L("Not all notifications completed, counter=%d"),counter);
    LOG(("OpenWFTest: *** Not all notifications completed, counter=%d",counter));
    delete waiter;
    //the notifications didn't complete in time
    return EFalse;
    }

// Create a suite of all the tests
CTestSuite* COpenwfTest::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE_OPT(COpenwfTest,NULL);
		//positive fastpath tests
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0000L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0001L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0002L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0003L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0004L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0005L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0006L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0007L);
        //fastpath with notification tests
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0010L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0011L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0012L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0013L);
        //negative fastpath tests
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0020L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0021L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0022L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0023L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0024L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0025L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0026L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_FASTPATH_0027L);
		
	END_SUITE;
	}

void TefUnitFailLeaveL()
	{
	User::Leave(KErrTEFUnitFail);
	}

CActiveNotification* CActiveNotification::NewL(RSurfaceUpdateSession& aSurfaceUpdateSession,TBool aActivate)
	{
	CActiveNotification* self = new (ELeave) CActiveNotification;
	CleanupStack::PushL(self);
	self->ConstructL(aSurfaceUpdateSession);
	CleanupStack::Pop(self);
	if (aActivate)
		{
		self->SetActive();
		}
	return self;
	}

void CActiveNotification::ConstructL(RSurfaceUpdateSession& aSurfaceUpdateSession)
	{
	CActiveScheduler::Add(this);
	iSurfaceUpdateSession = aSurfaceUpdateSession;
	}
	
CActiveNotification::CActiveNotification() : CActive(EPriorityNormal)
	{}

CActiveNotification::~CActiveNotification()
	{
	Cancel();
	}

void CActiveNotification::DoCancel()
	{  //we need to cancel all notifications - this will impact other notifications in this sus session!
    iSurfaceUpdateSession.CancelAllUpdateNotifications();
	}

void CActiveNotification::RunL()
	{
	CActiveScheduler::Stop();
	}
	
TInt CActiveNotification::RunError(TInt aError)
	{
	return aError; // exists so a break point can be placed on it.
	}

void CActiveNotification::Activate()
	{
	SetActive();
	}

CActiveWaiter* CActiveWaiter::NewL()
    {
	CActiveWaiter* self = new (ELeave) CActiveWaiter;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
void CActiveWaiter::ConstructL()
    {
    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);
    }
    
CActiveWaiter::CActiveWaiter() : CActive(CActive::EPriorityStandard)
    {}

CActiveWaiter::~CActiveWaiter()
    {
    Cancel();
    iTimer.Close();
    }

void CActiveWaiter::DoCancel()
    {
    iTimer.Cancel();
    }

void CActiveWaiter::RunL()
    {
	iRun = ETrue;
    CActiveScheduler::Stop();
    }
    
TInt CActiveWaiter::RunError(TInt aError)
    {
    return aError; // exists so a break point can be placed on it.
    }

/**
 * Waits the set amount of time. Returns the amount of times the active scheduler 
 * was stopped (not including its own active scheduler stop)
 */
TInt CActiveWaiter::Wait(TTimeIntervalMicroSeconds32 aDelay)   //microseconds
    {
	TInt counter = 0;
    SetActive();
    iTimer.After(iStatus, aDelay);
    iRun = EFalse;
    CActiveScheduler::Start();
    while (!iRun)
    	{//may be another object that stops the scheduler running in these tests.
		counter++;
		CActiveScheduler::Start();
    	}
    return counter;
    }

/**
 * Starts an AO with the given delay. Remember the RunL will stop the active scheduler.
 */
void CActiveWaiter::StartWait(TTimeIntervalMicroSeconds32 aDelay)   //microseconds
    {
    SetActive();
    iTimer.After(iStatus, aDelay);
    }

/*
@SYMTestCaseID          GRAPHICS-OPENWFC-FASTPATH-0000
@SYMTestCaseDesc        Positive testing - Fastpath one-element scene
@SYMREQ                 
@SYMPREQ                PREQ417-54885
@SYMTestType            Unit Test
@SYMTestPriority        High
@SYMTestPurpose         Check a scene meeting fastpath criteria can be fastpathed
@SYMTestActions         
    All compositions are autonomous:
    Compose an empty scene
    Create a scene containing a screen sized element with no scaling, no alpha, no mask, no rotation
    Compose the scene
@SYMTestExpectedResults
    The final scene should be fastpathed    
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0000L()
	{
    iTestName = _L("FASTPATH_0000");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - Now ON"));
    
    iUtility->FillSurfaceL(surface, 0, KGreen);
    err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WaitL(iCompositionPause);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();

    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0001
@SYMTestCaseDesc		Positive testing - Fastpath one-element scene  
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High	
@SYMTestPurpose			Check a scene including a full screen element with global alpha at opaque level 
										can be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Compose an empty scene
	Create a scene containing a screen sized element with 2 buffers
	Enable WFC_TRANPARENCY_GLOBAL_ALPHA 
	Set WFC_ELEMENT_GLOBAL_ALPHA to be 255 (opaque)
	Compose the scene
@SYMTestExpectedResults
	The final scene should be fastpathed
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0001L()
	{
    iTestName = _L("FASTPATH_0001");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    //set element global alpha
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_TRANSPARENCY_TYPES, WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA);
    wfcSetElementAttribf(iDevice, fpElement, WFC_ELEMENT_GLOBAL_ALPHA, 1.0f);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit - using element alpha"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now ON - using element alpha"));
    
    iUtility->FillSurfaceL(surface, 0, KGreen);
    err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WaitL(iCompositionPause);	
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();

    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);  
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0002
@SYMTestCaseDesc		Positive testing - Fastpath one-element scene  
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High
@SYMTestPurpose			Check a scene including a full screen element with source alpha flagged but an 
										opaque data format can be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Compose an empty scene
	Create a scene containing a screen sized element 
	Enable WFC_TRANSPARENCY_SOURCE, but the source format has no alpha value 
	Compose the scene
@SYMTestExpectedResults
	The final scene should be fastpathed
*/	
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0002L()
	{
    iTestName = _L("FASTPATH_0002");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
												EUidPixelFormatXRGB_8888,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    //set pixel alpha on XRGB element
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_TRANSPARENCY_TYPES, WFC_TRANSPARENCY_SOURCE);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit - using pixel alpha"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now ON - using pixel alpha"));
    
    iUtility->FillSurfaceL(surface, 0, KGreen);
    err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WaitL(iCompositionPause);
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();

    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);  
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0003
@SYMTestCaseDesc		Positive testing - Fastpath scene with a double buffered element
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High
@SYMTestPurpose			Check if a scene is fastpathed using the second buffer
@SYMTestActions			
	All compositions are autonomous:
	Compose an empty scene
	A) Create a scene containing an element with 2 buffers
	Set the element screen size
	Set destination and source rectangle to screen size
	Compose the scene
	B) Send an update to buffer 1 with change of colour
	Wait for notification
@SYMTestExpectedResults
	The scene should be fastpathed for A and B
	Screen colour should change after B
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0003L()
	{
    iTestName = _L("FASTPATH_0003");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now ON"));
    
    iUtility->FillSurfaceL(surface, 0, KGreen);
    err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WaitL(iCompositionPause);	
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();
    
    //Fill surface is blue
    iUtility->FillSurfaceL(surface, 1, KBlue);
	//Do content update for surface buff1
    CActiveNotification* buff1disp = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(buff1disp);
    TTimeStamp buff1time;
    iUtility->NotifyWhenDisplayed(buff1disp->iStatus,buff1time);
    
    LOG(("OpenWFTest: Fastpath - staying ON..."));
    err = iUtility->SubmitUpdate(surface, 1, NULL);
	ASSERT_EQUALS(err,KErrNone);
    ASSERT_TRUE(WaitForNotifications(KMaxNotificationDelay,1));	//1 AO should have completed
    
    //create ref image/check on screen content
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KBlue,iTolerance));
    ReleaseOnScreenStream();

    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);
    CleanupStack::PopAndDestroy(buff1disp);
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0004
@SYMTestCaseDesc		Positive testing - Fastpath two-element scene 
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		Medium
@SYMTestPurpose			Check a scene including a full screen opaque element as the top layer can be 
										fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Compose an empty scene
	A) Create a scene containing a screen sized element with WFC_TRANSPARENCY_NONE enabled
	Compose the scene
  B) Create a small rectangle behind the first element
	Compose the scene
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0004L()
	{
    iTestName = _L("FASTPATH_0004");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
	
	TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
												iFastpathablePixelFormat,
												iStride, iContiguous, iMaxBuffers);
	ASSERT_FALSE(surface.IsNull());

	WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
	TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
	wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);	
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
	
    TSurfaceId surfaceB = iUtility->CreateSurfaceL(TSize(iFastpathableWidth/2,iFastpathableHeight/2),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surfaceB.IsNull());
    WFCSource sourceB = wfcCreateSourceFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceB), NULL);
    err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement elementB = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, elementB, WFC_ELEMENT_SOURCE, sourceB);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iHalfSizeRect);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iCenterRect);
	
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
	wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
	wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
	LOG(("OpenWFTest: Fastpath - now ON"));
	
	iUtility->FillSurfaceL(surface, 0, KGreen);
	err = iUtility->SubmitUpdate(surface, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WaitL(iCompositionPause);
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();
	
    LOG(("OpenWFTest: Fastpath - staying ON..."));
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
	wfcInsertElement(iDevice, elementB, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
	wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
	
	iUtility->FillSurfaceL(surfaceB, 0, KBlue);
	err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WaitL(iCompositionPause);
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();

    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
	wfcDestroySource(iDevice, fpSource);
	wfcDestroySource(iDevice, sourceB);  
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0005
@SYMTestCaseDesc		Positive testing - Fastpath two-element scene 
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		Medium	
@SYMTestPurpose			Check a scene including a full screen element with global alpha as the top layer 
										can be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Compose an empty scene
	A) Create a scene containing a screen sized element 
	Enable WFC_TRANPARENCY_GLOBAL_ALPHA and set WFC_ELEMENT_GLOBAL_ALPHA to be 255 (opaque)
	Compose the scene
	B) Create a smaller element behind the first element
	Compose the scene
@SYMTestExpectedResults
	Both compositions should trigger fastpathing.
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0005L()
	{
    iTestName = _L("FASTPATH_0005");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
	
	TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
												iFastpathablePixelFormat,
												iStride, iContiguous, iMaxBuffers);
	ASSERT_FALSE(surface.IsNull());

	WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
	TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
	wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);	
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
//set element global alpha
	wfcSetElementAttribf(iDevice, fpElement, WFC_ELEMENT_GLOBAL_ALPHA, 1.0f);
	wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_TRANSPARENCY_TYPES, WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA);
	
    TSurfaceId surfaceB = iUtility->CreateSurfaceL(TSize(iFastpathableWidth/2,iFastpathableHeight/2),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surfaceB.IsNull());
    WFCSource sourceB = wfcCreateSourceFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceB), NULL);
    err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement elementB = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, elementB, WFC_ELEMENT_SOURCE, sourceB);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iHalfSizeRect);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iCenterRect);
	
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
	wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit - using element global alpha"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
	wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
	LOG(("OpenWFTest: Fastpath - now ON... - using element global alpha"));
	
	iUtility->FillSurfaceL(surface, 0, KGreen);
	ASSERT_EQUALS(iUtility->SubmitUpdate(surface, 0, NULL),KErrNone);
	WaitL(iCompositionPause);
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();
	

#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
	wfcInsertElement(iDevice, elementB, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit - using element global alpha"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
	wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
	
	iUtility->FillSurfaceL(surfaceB, 0, KBlue);
	err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WaitL(iCompositionPause);	
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();

    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
	wfcDestroySource(iDevice, fpSource);
	wfcDestroySource(iDevice, sourceB);  
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0006
@SYMTestCaseDesc		Positive testing - Fastpath two-element scene 
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		Low
@SYMTestPurpose			Check a scene including a full screen opaque element behind a transparent element 
										can be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Compose an empty scene
	A) Create a scene containing an element of any size 
	Enable WFC_TRANPARENCY_GLOBAL_ALPHA and set WFC_ELEMENT_GLOBAL_ALPHA to 0 (transparent)
	Compose the scene
	B) Create another screen sized opaque element behind the frist 
	Compose the scene
@SYMTestExpectedResults
	The final scene should fastpath for B), not for A)	
	(The current implementation does not support this fastpath behaviour. This test can be ignored at 
	the moment.)
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0006L()
	{
    iTestName = _L("FASTPATH_0006");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
	
	TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth/2,iFastpathableHeight/2),
												iFastpathablePixelFormat,
												iStride, iContiguous, iMaxBuffers);
	ASSERT_FALSE(surface.IsNull());

	WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
	TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
	wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);	
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iHalfSizeRect);
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iCenterRect);
	//set element global alpha fully transparent
	wfcSetElementAttribf(iDevice, fpElement, WFC_ELEMENT_GLOBAL_ALPHA, 0.0f);
	wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_TRANSPARENCY_TYPES, WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA);
	
    TSurfaceId surfaceB = iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
													iFastpathablePixelFormat,
													iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surfaceB.IsNull());
    WFCSource sourceB = wfcCreateSourceFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceB), NULL);
    err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement elementB = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, elementB, WFC_ELEMENT_SOURCE, sourceB);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
	
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
	wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit -single fully transparent element using global alpha"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
	wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
	LOG(("OpenWFTest: Fastpath - now ON - single fully transparent element using global alpha"));
	
	iUtility->FillSurfaceL(surface, 0, KGreen);
	err = iUtility->SubmitUpdate(surface, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WaitL(iCompositionPause);	
 
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
	wfcInsertElement(iDevice, elementB, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit -fullscreen opaque element behind fully transparent element"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
	wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
	LOG(("OpenWFTest: Fastpath - now ON - fullscreen opaque element behind transparent element"));
	
	iUtility->FillSurfaceL(surfaceB, 0, KBlue);
	err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WaitL(iCompositionPause);	
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KBlue,iTolerance));
    ReleaseOnScreenStream();

    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
	wfcDestroySource(iDevice, fpSource);
	wfcDestroySource(iDevice, sourceB);  
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0007
@SYMTestCaseDesc		Positive testing - Fastpath one-element scene  
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High
@SYMTestPurpose			Check a scene including destination rectangle changes can trigger fastpath on and off
@SYMTestActions			
	All compositions are autonomous:
	Compose an empty scene
	Create a scene containing a screen sized element 
	Compose scene
	Set the destination rectangle smaller than screen size
	Compose the scene
	Set the destination rectangle back to screen size
	Compose the scene
@SYMTestExpectedResults
	The scenes with the element of full screen destination rectangle should be fastpathed	
	The scene with the element of smaller destination rectangle should not be fastpathed
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0007L()
	{
    iTestName = _L("FASTPATH_0007");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
	
	TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
												iFastpathablePixelFormat,
												iStride, iContiguous, iMaxBuffers);
	ASSERT_FALSE(surface.IsNull());

	WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
	TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
	wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);	
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
	//NOT full screen destination
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iCenterRect);
	
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
	wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
	wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
	LOG(("OpenWFTest: Fastpath - still OFF"));
	
	iUtility->FillSurfaceL(surface, 0, KGreen);
	err = iUtility->SubmitUpdate(surface, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WaitL(iCompositionPause);	
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iCenterTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();
	
	//NOW set full screen
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
	
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
	wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
	WaitL(iCompositionPause);
 
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
    ReleaseOnScreenStream();

    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
	wfcDestroySource(iDevice, fpSource);  
	}

//NOTIFICATION TESTS
void COpenwfTest::FastpathNotificationsTestL(TBool aAutonomous)
	{
    SetupEmptySceneL(aAutonomous);

    //Create 3 elements A+C fastpathable, B centered
    TSurfaceId surfaceA = iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surfaceA.IsNull());
    WFCSource sourceA = wfcCreateSourceFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceA), NULL);
    TInt err = iUtility->SubmitUpdate(surfaceA, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement elementA = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, elementA, WFC_ELEMENT_SOURCE, sourceA);
    wfcSetElementAttribiv(iDevice, elementA, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, elementA, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    
    TSurfaceId surfaceB = iUtility->CreateSurfaceL(TSize(iFastpathableWidth/2,iFastpathableHeight/2),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surfaceB.IsNull());
    WFCSource sourceB = wfcCreateSourceFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceB), NULL);
    err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement elementB = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, elementB, WFC_ELEMENT_SOURCE, sourceB);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iHalfSizeRect);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iCenterRect);
    
    TSurfaceId surfaceC = iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surfaceC.IsNull());
    WFCSource sourceC = wfcCreateSourceFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceC), NULL);
    err = iUtility->SubmitUpdate(surfaceC, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement elementC = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, elementC, WFC_ELEMENT_SOURCE, sourceC);
    wfcSetElementAttribiv(iDevice, elementC, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, elementC, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);

    //**********STARTING ACTUAL TEST!**********
    //**********SURFACE A FASTPATHABLE**********
	//Add A to scene
    wfcInsertElement(iDevice, elementA, WFC_INVALID_HANDLE);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now ON"));
	//Set A buffer0 red
    iUtility->FillSurfaceL(surfaceA, 0, KRed);
	//Do content update Abuff0 idisp iavail
    CActiveNotification* surfAbuff0disp = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfAbuff0disp);
    TTimeStamp surfAbuff0time;
    CActiveNotification* surfAbuff0avail = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfAbuff0avail);
    //iUtility->NotifyWhenDisplayed(surfAbuff0disp,surfAbuff0time);
    iUtility->NotifyWhenDisplayed(surfAbuff0disp->iStatus,surfAbuff0time);
    iUtility->NotifyWhenAvailable(surfAbuff0avail->iStatus); 
    err = iUtility->SubmitUpdate(surfaceA, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	
    if(!aAutonomous)
    	{
        WaitL(iCompositionPause);
		//Check A buff0 iDisp doesnt complete
		ASSERT_EQUALS(surfAbuff0disp->iStatus.Int(),KRequestPending);
		//Check A buff0 iAvail doesnt complete
		ASSERT_EQUALS(surfAbuff0avail->iStatus.Int(),KRequestPending);
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    ASSERT_TRUE(WaitForNotifications(KMaxNotificationDelay,1));
	
	//Check A buff0 iDisp completes
    ASSERT_EQUALS(surfAbuff0disp->iStatus.Int(),KErrNone);
	//Check A buff0 iAvail doesnt complete
    ASSERT_EQUALS(surfAbuff0avail->iStatus.Int(),KRequestPending);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KRed,iTolerance));
    ReleaseOnScreenStream();

	//Set A buffer1 Yellow
    iUtility->FillSurfaceL(surfaceA, 1, KYellow);
	
	//Check A buff0 iavail doesnt complete
    ASSERT_EQUALS(surfAbuff0avail->iStatus.Int(),KRequestPending);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KRed,iTolerance));
	ReleaseOnScreenStream();
	
	//Do content update Abuff1 idisp iavail
    CActiveNotification* surfAbuff1disp = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfAbuff1disp);
    TTimeStamp surfAbuff1time;
    CActiveNotification* surfAbuff1avail = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfAbuff1avail);
    iUtility->NotifyWhenDisplayed(surfAbuff1disp->iStatus,surfAbuff1time);
    iUtility->NotifyWhenAvailable(surfAbuff1avail->iStatus); 
    err = iUtility->SubmitUpdate(surfaceA, 1, NULL);
    ASSERT_EQUALS(err,KErrNone);
    
    if(!aAutonomous)
    	{
        WaitL(iCompositionPause);
		//Check A buff1 iDisp doesnt complete
		ASSERT_EQUALS(surfAbuff1disp->iStatus.Int(),KRequestPending);
		//Check A buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
		//Check A buff0 iAvail doesnt complete
	    ASSERT_EQUALS(surfAbuff0avail->iStatus.Int(),KRequestPending);
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    ASSERT_TRUE(WaitForNotifications(KMaxNotificationDelay,2));
	
	//Check A buff1 idisp completes
    ASSERT_EQUALS(surfAbuff1disp->iStatus.Int(),KErrNone);
	//Check A buff0 iAvail completes
    ASSERT_EQUALS(surfAbuff0avail->iStatus.Int(),KErrNone);
	//Check A buff1 iAvail doesnt complete
    ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KYellow,iTolerance));
	ReleaseOnScreenStream();
	
	//**********SURFACE B NOT VISIBLE**********
	//Add surfaceB to scene behind surfaceA
    wfcInsertElement(iDevice, elementB, WFC_INVALID_HANDLE);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - still ON"));
	//Set B buffer0 green
    iUtility->FillSurfaceL(surfaceB, 0, KGreen);
	//Do content update idisp iavail
    CActiveNotification* surfBbuff0disp = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfBbuff0disp);
    TTimeStamp surfBbuff0time;
    CActiveNotification* surfBbuff0avail = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfBbuff0avail);
    iUtility->NotifyWhenDisplayed(surfBbuff0disp->iStatus,surfBbuff0time);
    iUtility->NotifyWhenAvailable(surfBbuff0avail->iStatus); 
    err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	
    if(!aAutonomous)
    	{
        WaitL(iCompositionPause);
		//Check B buff0 iAvail doesnt complete
		ASSERT_EQUALS(surfBbuff0avail->iStatus.Int(),KRequestPending);
		//Check B buff0 iDisp doesnt complete
		ASSERT_EQUALS(surfBbuff0disp->iStatus.Int(),KRequestPending);
		//Check A buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    ASSERT_TRUE(WaitForNotifications(KMaxNotificationDelay,1));
    
	//Check B buff0 idisp completes - could be no error or not visible
    ASSERT_TRUE((surfBbuff0disp->iStatus.Int()==KErrNotVisible)||(surfBbuff0disp->iStatus.Int()==KErrNone));
	//Check B buff0 iavail doesnt complete
    ASSERT_EQUALS(surfBbuff0avail->iStatus.Int(),KRequestPending);
	//Check A buff1 iavail doesnt complete
    ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
	
	//Set B buffer1 blue
	iUtility->FillSurfaceL(surfaceB, 1, KBlue);
	//Do content update idisp iavail
    CActiveNotification* surfBbuff1disp = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfBbuff1disp);
    TTimeStamp surfBbuff1time;
    CActiveNotification* surfBbuff1avail = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfBbuff1avail);
    iUtility->NotifyWhenDisplayed(surfBbuff1disp->iStatus,surfBbuff1time);
    iUtility->NotifyWhenAvailable(surfBbuff1avail->iStatus); 
    err = iUtility->SubmitUpdate(surfaceB, 1, NULL);
    ASSERT_EQUALS(err,KErrNone);
    
    if(!aAutonomous)
    	{
        WaitL(iCompositionPause);
		//Check B buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KRequestPending);
		//Check B buff1 iDisp doesnt complete
		ASSERT_EQUALS(surfBbuff1disp->iStatus.Int(),KRequestPending);
		//Check B buff0 iAvail doesnt complete
		ASSERT_EQUALS(surfBbuff0avail->iStatus.Int(),KRequestPending);
		//Check A buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    ASSERT_TRUE(WaitForNotifications(KMaxNotificationDelay,2));
    
    //Check B buff1 idisp completes - could be no error or not visible
    ASSERT_TRUE((surfBbuff1disp->iStatus.Int()==KErrNotVisible)||(surfBbuff1disp->iStatus.Int()==KErrNone));
	//Check B buff1 iavail doesnt complete
    ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KRequestPending);
	//Check B buff0 iavail completes - could be no error or not visible
    ASSERT_TRUE((surfBbuff0avail->iStatus.Int()==KErrNotVisible)||(surfBbuff0avail->iStatus.Int()==KErrNone));
	//Check A buff1 iavail doesnt complete
    ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KYellow,iTolerance));
	ReleaseOnScreenStream();
	
	//**********SURFACE C ALSO FASTPATHABLE**********
	//Add C to scene in front of surfaceA
    wfcInsertElement(iDevice, elementC, elementA);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - still ON"));
	//Set C to brown
    iUtility->FillSurfaceL(surfaceC, 0, KBrown);
	//Do content update Cbuff0 idisp iavail
    CActiveNotification* surfCbuff0disp = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfCbuff0disp);
    TTimeStamp surfCbuff0time;
    CActiveNotification* surfCbuff0avail = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfCbuff0avail);
    iUtility->NotifyWhenDisplayed(surfCbuff0disp->iStatus,surfCbuff0time);
    iUtility->NotifyWhenAvailable(surfCbuff0avail->iStatus); 
    err = iUtility->SubmitUpdate(surfaceC, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	
    if(!aAutonomous)
    	{
        WaitL(iCompositionPause);
		//Check C buff0 iAvail doesnt complete
		ASSERT_EQUALS(surfCbuff0avail->iStatus.Int(),KRequestPending);
		//Check C buff0 iDisp doesnt complete
		ASSERT_EQUALS(surfCbuff0disp->iStatus.Int(),KRequestPending);
		//Check B buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KRequestPending);
		//Check A buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    ASSERT_TRUE(WaitForNotifications(KMaxNotificationDelay,1));
    
	//Check C buff0 idisp completes
    ASSERT_EQUALS(surfCbuff0disp->iStatus.Int(),KErrNone);
	//Check C buff0 iavail does not complete
    ASSERT_EQUALS(surfCbuff0avail->iStatus.Int(),KRequestPending);
	//Check B buff1 iavail doesnt complete
    ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KRequestPending);
	//Check A buff1 iavail doesnt complete
    ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KBrown,iTolerance));
	ReleaseOnScreenStream();
	
	//Set C buffer1 grey
    iUtility->FillSurfaceL(surfaceC, 1, KGray);
	//Do content update Cbuff1 idisp iavail
    CActiveNotification* surfCbuff1disp = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfCbuff1disp);
    TTimeStamp surfCbuff1time;
    CActiveNotification* surfCbuff1avail = CActiveNotification::NewL(iUtility->Session(),ETrue);
    CleanupStack::PushL(surfCbuff1avail);
    iUtility->NotifyWhenDisplayed(surfCbuff1disp->iStatus,surfCbuff1time);
    iUtility->NotifyWhenAvailable(surfCbuff1avail->iStatus); 
    err = iUtility->SubmitUpdate(surfaceC, 1, NULL);
    ASSERT_EQUALS(err,KErrNone);
    
    if(!aAutonomous)
    	{
        WaitL(iCompositionPause);
		//Check C buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfCbuff1avail->iStatus.Int(),KRequestPending);
		//Check C buff1 iDisp doesnt complete
		ASSERT_EQUALS(surfCbuff1disp->iStatus.Int(),KRequestPending);
		//Check C buff0 iAvail doesnt complete
		ASSERT_EQUALS(surfCbuff0avail->iStatus.Int(),KRequestPending);
		//Check B buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KRequestPending);
		//Check A buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    ASSERT_TRUE(WaitForNotifications(KMaxNotificationDelay,2));
    
	//Check C buff1 idisp completes
    ASSERT_EQUALS(surfCbuff1disp->iStatus.Int(),KErrNone);
	//Check C buff1 iavail does not complete
    ASSERT_EQUALS(surfCbuff1avail->iStatus.Int(),KRequestPending);
	//Check C buff0 iavail completes
    ASSERT_EQUALS(surfCbuff0avail->iStatus.Int(),KErrNone);
	//Check B buff1 iavail doesnt complete
    ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KRequestPending);
	//Check A buff1 iavail doesnt complete
    ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGray,iTolerance));
	ReleaseOnScreenStream();
	
    //**********SURFACE A BACK TO FRONT**********
	//Move A to the front
    wfcInsertElement(iDevice, elementA, elementC);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - still ON"));
	//Set A buffer0 magenta
    iUtility->FillSurfaceL(surfaceA, 0, KMagenta);
	//Do content update Abuff0 idisp iavail
    surfAbuff0disp->Activate();
    surfAbuff0avail->Activate();
    iUtility->NotifyWhenDisplayed(surfAbuff0disp->iStatus,surfAbuff0time);
    iUtility->NotifyWhenAvailable(surfAbuff0avail->iStatus); 
    err = iUtility->SubmitUpdate(surfaceA, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	
    if(!aAutonomous)
    	{
        WaitL(iCompositionPause);
		//Check A buff0 iAvail doesnt complete
		ASSERT_EQUALS(surfAbuff0avail->iStatus.Int(),KRequestPending);
		//Check A buff0 iDisp doesnt complete
		ASSERT_EQUALS(surfAbuff0disp->iStatus.Int(),KRequestPending);
		//Check C buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfCbuff1avail->iStatus.Int(),KRequestPending);
		//Check B buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KRequestPending);
		//Check A buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KRequestPending);
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    ASSERT_TRUE(WaitForNotifications(KMaxNotificationDelay,2));
    
	//Check A buff0 idisp completes
    ASSERT_EQUALS(surfAbuff0disp->iStatus.Int(),KErrNone);
	//Check A buff0 iavail does not complete
    ASSERT_EQUALS(surfAbuff0avail->iStatus.Int(),KRequestPending);
	//Check A buff1 iavail does complete
    ASSERT_EQUALS(surfAbuff1avail->iStatus.Int(),KErrNone);
	//Check C buff1 iavail does not complete
    ASSERT_EQUALS(surfCbuff1avail->iStatus.Int(),KRequestPending);
	//Check B buff1 iavail doesnt complete
    ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KRequestPending);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KMagenta,iTolerance));
	ReleaseOnScreenStream();
    
    //**********SURFACE B TO FRONT - STOPS FASTPATH HAPPENING**********
    //Move B to the front
	wfcInsertElement(iDevice, elementB, elementA);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now OFF..."));
	//Set B buffer0 dark red
	iUtility->FillSurfaceL(surfaceB, 0, KDarkRed);
	//Do content update Abuff0 idisp iavail
	surfBbuff0disp->Activate();
	surfBbuff0avail->Activate();
	iUtility->NotifyWhenDisplayed(surfBbuff0disp->iStatus,surfBbuff0time);
	iUtility->NotifyWhenAvailable(surfBbuff0avail->iStatus); 
	err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	
    if(!aAutonomous)
    	{
        WaitL(iCompositionPause);
		//Check B buff0 iAvail doesnt complete
		ASSERT_EQUALS(surfBbuff0avail->iStatus.Int(),KRequestPending);
		//Check B buff0 iDisp doesnt complete
		ASSERT_EQUALS(surfBbuff0disp->iStatus.Int(),KRequestPending);
		//Check A buff0 iAvail doesnt complete
		ASSERT_EQUALS(surfAbuff0avail->iStatus.Int(),KRequestPending);
		//Check C buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfCbuff1avail->iStatus.Int(),KRequestPending);
		//Check B buff1 iAvail doesnt complete
		ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KRequestPending);
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    ASSERT_TRUE(WaitForNotifications(KMaxNotificationDelay,2));
	
	//Check B buff0 iAvail doesnt complete
	ASSERT_EQUALS(surfBbuff0avail->iStatus.Int(),KRequestPending);
	//Check B buff0 iDisp completes
	ASSERT_EQUALS(surfBbuff0disp->iStatus.Int(),KErrNone);
	//Check A buff0 iAvail doesnt complete
	ASSERT_EQUALS(surfAbuff0avail->iStatus.Int(),KRequestPending);
	//Check C buff1 iAvail doesnt complete
	ASSERT_EQUALS(surfCbuff1avail->iStatus.Int(),KRequestPending);
	//Check B buff1 iAvail completes
	ASSERT_EQUALS(surfBbuff1avail->iStatus.Int(),KErrNone);
	
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	//Check dark red center of screen
	ASSERT_TRUE(CheckOnScreenStreamRect(iCenterTRect,KDarkRed,iTolerance));
	//Check magenta around the borders
	ASSERT_TRUE(CheckOnScreenStreamRect(TRect(0,0,iCenterTRect.iTl.iX,iCenterTRect.iTl.iY),KMagenta,iTolerance));
	ASSERT_TRUE(CheckOnScreenStreamRect(TRect(iCenterTRect.iBr.iX+1,iCenterTRect.iBr.iY+1,
			iFastpathableWidth,iFastpathableHeight),KMagenta,iTolerance));
	ReleaseOnScreenStream();
	
	ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    
    wfcDestroySource(iDevice, sourceA);
    wfcDestroySource(iDevice, sourceB);
    wfcDestroySource(iDevice, sourceC);
    
	CleanupStack::PopAndDestroy(12,surfAbuff0disp);
	}
/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0010
@SYMTestCaseDesc		Positive testing - Fastpath with notification checking for autonomous composition
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Integration Test
@SYMTestPriority		High
@SYMTestPurpose			Check fastpath does not cause any problem to notifications
@SYMTestActions			
	Autonomous composition
	All elements created are double buffered and opaque:
	Compose an empty scene
	
	A) Create a scene containing a screen sized element A
	Fill buffer 0 of element A with red colour 
	Submit the scene
	Do content update of buffer 0 for available (iAvailable) and display time (iDisplayed)
	Compose the scene, iDisplayed for buffer 0 should complete
	
	Fill buffer 1 of element A with yellow colour
	Do content update of buffer 1 
	Compose the scene, iDisplayed for buffer 1 should complete
	Wait for iAvailable of buffer 0 to complete	
	(iAvailable of buffer 1 will not complete until further content update)	
	
	B) Add a new element B of smaller non fastpathable size behind element A
	Fill buffer 0 of element B with green colour
	Submit the scene
	Do content update of buffer 0 of element B 
	Compose the scene, iDisplayed for buffer 0 of element B should not complete
	iAvailable for buffer 1 of element A should not complete
		
	Fill buffer 1 of element B with blue colour
	Do content update of buffer 1 of element B 
	Wait for iDisplay to complete (possibly with KErrNotVisible)
	Wait for iAvailable for buffer 0 of element B to complete (possibly with KErrNotVisible)
	
	C) Add a new screen size element C in front of A and B
	Fill buffer 0 of element C to be brown
	Do content update of buffer 0 of element C 
	Compose the scene, iDisplayed of buffer 0 of element C should complete
	iAvailable of buffer 1 of element B should not complete
	
	Fill buffer 1 of element C with grey colour
	Do content update of buffer 1 of element C 
	Compose the scene, iDisplayed of buffer 1 of element C should complete
	Wait for iAvailable of buffer 0 of element C to complete
		
	D) Move element A to the front
	Fill buffer 0 of element A with magenta colour
	Do content update of buffer 0 of element A 
	Compose the scene, iDisplayed for buffer 0 of element A should complete
	Wait for iAvailable of buffer 1 of element A to complete
	iAvailable of buffer 1 of element C should not complete
	
	E) Move element B to the front *NOW NOT FASTPATHING*
	Fill buffer 0 of element B with dark red colour
	Do content update of buffer 0 of element B
	Compose the scene, iDisplayed for buffer 0 of element B should complete
	Wait for iAvailable of buffer 1 of element B to complete
	iAvailable of buffer 1 of element C should not complete
	iAvailable of buffer 0 of element A should not complete
		
@SYMTestExpectedResults
	All composition should trigger fastpath 
	Request status checks iAvailable and iDisplayed should be set to KErrNone every time.	
	Screen colour should change in the sequence of red, yellow, brown, grey, magenta
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0010L()
	{
    iTestName = _L("FASTPATH_0010");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    FastpathNotificationsTestL(ETrue);
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0011
@SYMTestCaseDesc		Positive testing - Fastpath with notification checking for non-autonomous composition
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Integration Test
@SYMTestPriority		High
@SYMTestPurpose			Check fastpath does not cause any problem to notifications
@SYMTestActions			
	A repeat of GRAPHICS-OPENWFC-FASTPATH-0010 but with non autonomous composition
	
	Where possible TRequestStatuses are additionally checked after submit updates and before
	explicit wfcCompose calls are made.
	
@SYMTestExpectedResults
	Just commit should not trigger any changes to the scene or notifications to complete
	Just content updates should not trigger any changes to the scene or notifications to complete
	Only after Commit, Content update and Force compose should things happen.
	Request status checks iAvailable and iDisplayed should be completed.
	Screen colour should change in the sequence of red, yellow, green, blue, brown and grey
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0011L()
	{
    iTestName = _L("FASTPATH_0011");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    FastpathNotificationsTestL(EFalse);
	}

//NOTIFICATION TESTS
void COpenwfTest::FastpathDispXNotificationsTestL(TBool aAutonomous)
	{
    SetupEmptySceneL(aAutonomous);
    if (aAutonomous)
        {   //if we have manual pauses the test will complete early
        iAllowManualPause = EFalse;
        }
	
	//Create 2 fastpathable elements
    TSurfaceId surfaceA = iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);

    ASSERT_FALSE(surfaceA.IsNull());
    WFCSource sourceA = wfcCreateSourceFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceA), NULL);
    TInt err = iUtility->SubmitUpdate(surfaceA, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement elementA = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, elementA, WFC_ELEMENT_SOURCE, sourceA);
    wfcSetElementAttribiv(iDevice, elementA, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, elementA, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    
    TSurfaceId surfaceB = iUtility->CreateSurfaceL(TSize(iFastpathableWidth/2,iFastpathableHeight/2),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surfaceB.IsNull());
    WFCSource sourceB = wfcCreateSourceFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceB), NULL);
    err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement elementB = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, elementB, WFC_ELEMENT_SOURCE, sourceB);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iHalfSizeRect);
    wfcSetElementAttribiv(iDevice, elementB, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iCenterRect);
	
	//**********STARTING ACTUAL TEST!**********
	//Add A to scene
	wfcInsertElement(iDevice, elementA, WFC_INVALID_HANDLE);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now ON"));
	//Set A buffer0 red
	iUtility->FillSurfaceL(surfaceA, 0, KRed);
	err = iUtility->SubmitUpdate(surfaceA, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
    if(!aAutonomous)
    	{
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    WaitL(iCompositionPause);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KRed,iTolerance));
	ReleaseOnScreenStream();
	
	//Add surfaceB to scene infront of surfaceA
	wfcInsertElement(iDevice, elementB, elementA);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now OFF"));
	//Set B buffer0 green
	iUtility->FillSurfaceL(surfaceB, 0, KBlue);
	err = iUtility->SubmitUpdate(surfaceB, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
    if(!aAutonomous)
    	{
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
    WaitL(iCompositionPause);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	//Check blue center of screen
    ASSERT_TRUE(CheckOnScreenStreamRect(iCenterTRect,KBlue,iTolerance));
	//Check red around the borders
	ASSERT_TRUE(CheckOnScreenStreamRect(TRect(0,0,iCenterTRect.iTl.iX,iCenterTRect.iTl.iY),KRed,iTolerance));
	ASSERT_TRUE(CheckOnScreenStreamRect(TRect(iCenterTRect.iBr.iX+1,iCenterTRect.iBr.iY+1,
			iFastpathableWidth,iFastpathableHeight),KRed,iTolerance));
	ReleaseOnScreenStream();
	
	//Set A buffer1 yellow
	iUtility->FillSurfaceL(surfaceA, 1, KYellow);
	//Set B buffer1 KGreen
	iUtility->FillSurfaceL(surfaceB, 1, KGreen);
	
	//Do content updated surfaceA dispXtimes 10
	CActiveNotification* surfAbuff1dispX = CActiveNotification::NewL(iUtility->Session(),ETrue);
	CleanupStack::PushL(surfAbuff1dispX);
	iUtility->NotifyWhenDisplayedXTimes(10,surfAbuff1dispX->iStatus);
	err = iUtility->SubmitUpdate(surfaceA, 1, NULL);
	//Do content updated surfaceB dispXtimes 5
	CActiveNotification* surfBbuff1dispX = CActiveNotification::NewL(iUtility->Session(),ETrue);
	CleanupStack::PushL(surfBbuff1dispX);
	iUtility->NotifyWhenDisplayedXTimes(5,surfBbuff1dispX->iStatus);
	err = iUtility->SubmitUpdate(surfaceB, 1, NULL);
	
    if(!aAutonomous)
    	{
		for(TInt i=0;i<4;i++)
			{
			wfcCompose(iDevice, iContext, WFC_TRUE);
		    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
		    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
		    WaitL(iCompositionPause);
			//Check A has not completed yet
			ASSERT_EQUALS(surfAbuff1dispX->iStatus.Int(),KRequestPending);
			//Check B has not completed yet
			ASSERT_EQUALS(surfBbuff1dispX->iStatus.Int(),KRequestPending);
			}
		wfcCompose(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    	}
	//surfBbuff1dispX should complete, and its RunL will stop the scheduler
    CActiveScheduler::Start();
	//Check B has completed
    ASSERT_EQUALS(surfBbuff1dispX->iStatus.Int(),KErrNone);
	//Check A has not completed yet
	ASSERT_EQUALS(surfAbuff1dispX->iStatus.Int(),KRequestPending);
	
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	//Check green center of screen
    ASSERT_TRUE(CheckOnScreenStreamRect(iCenterTRect,KGreen,iTolerance));
	//Check yellow around the borders
	ASSERT_TRUE(CheckOnScreenStreamRect(TRect(0,0,iCenterTRect.iTl.iX,iCenterTRect.iTl.iY),KYellow,iTolerance));
	ASSERT_TRUE(CheckOnScreenStreamRect(TRect(iCenterTRect.iBr.iX+1,iCenterTRect.iBr.iY+1,
			iFastpathableWidth,iFastpathableHeight),KYellow,iTolerance));
	ReleaseOnScreenStream();
	
    //check it hasn't completed unexpectedly early while using on screen stream
    ASSERT_EQUALS(surfAbuff1dispX->iStatus.Int(),KRequestPending);
    
	//Remove surfaceB from the scene
	wfcRemoveElement(iDevice, elementB);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
    if(!aAutonomous)
    	{
		for(TInt i=0;i<4;i++)
			{
			wfcCompose(iDevice, iContext, WFC_TRUE);
		    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
		    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
		    LOG(("OpenWFTest: Fastpath - now ON"));
		    WaitL(iCompositionPause);
			//Check A has not completed yet
			ASSERT_EQUALS(surfAbuff1dispX->iStatus.Int(),KRequestPending);
			}
		wfcCompose(iDevice, iContext, WFC_TRUE);
    	}
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);

	//surfAbuff1dispX should complete, and its RunL will stop the scheduler
    CActiveScheduler::Start();
    ASSERT_EQUALS(surfAbuff1dispX->iStatus.Int(),KErrNone);

    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KYellow,iTolerance));
	ReleaseOnScreenStream();
	
	ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
	//Shutdown
    wfcDestroySource(iDevice, sourceA);
    wfcDestroySource(iDevice, sourceB);
    
	CleanupStack::PopAndDestroy(2,surfAbuff1dispX);
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0012
@SYMTestCaseDesc		Positive testing - Fastpath with notification checking DisplayXTime
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Integration Test
@SYMTestPriority		High
@SYMTestPurpose			Check fastpath does not cause any problem to notifications
@SYMTestActions			
	All compositions are autonomous:
	Compose an empty scene
	A) Create a scene containing a screen sized opaque element A
	Commit the scene
	Make a content update for available (iAvailable) and display time (aDisplayed)
	Wait for the request status objects to complete
	
	B) Add a small element B in front of A
	Compose the scene
	Content update for both A and B, set display 10 times to A, set display 5 times to B 
	Wait for the 5 times
	
	C) Remove element B
	Commit the scene
	Compose the scene
	Wait for the 10 times
@SYMTestExpectedResults
	Composition should fastpath in sections A and C
	Elements A's 10 times should not complete before element B's 5 times
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0012L()
	{
    iTestName = _L("FASTPATH_0012");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
	FastpathDispXNotificationsTestL(ETrue);
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0013
@SYMTestCaseDesc		Positive testing - Fastpath with notification checking DisplayXTime
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Integration Test
@SYMTestPriority		High
@SYMTestPurpose			Check fastpath does not cause any problem to notifications
@SYMTestActions			
	Repeats GRAPHICS-OPENWFC-FASTPATH-0013 in non autonomous mode.
	This allows gives the added benefit of accurate counting of wfcCompose calls, checking the displayXtimes
	notifications only complete after the correct number of composes.
@SYMTestExpectedResults
	Composition should fastpath in sections A and C
	Both should complete after their expect amount of compose calls with no errors
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0013L()
	{
    iTestName = _L("FASTPATH_0013");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    FastpathDispXNotificationsTestL(EFalse);
	}

/*
 *	NEGATIVE TESTING
 * */
/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0020
@SYMTestCaseDesc		Negative testing - Fastpath one-element scene
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High
@SYMTestPurpose			Check a scene including a full screen element with source alpha cannot be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Create a scene containing a screen sized element 
	Enable WFC_TRANSPARENCY_SOURCE  
	Set the value of WFC_ELEMENT_SOURCE between 0 and 255 (non-inclusive).
	Compose the scene
@SYMTestExpectedResults
	The scene cannot be fastpathed.
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0020L()
	{
    iTestName = _L("FASTPATH_0020");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
												EUidPixelFormatARGB_8888,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    //set per pixel alpha
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_TRANSPARENCY_TYPES, WFC_TRANSPARENCY_SOURCE);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit - using source alpha"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    
    for (TInt i=0;i<=256;i+=64)
    	{
		i=(i==256)?255:i;
		iUtility->FillSurfaceL(surface, 0, TRgb(0,255,0,i));
		err = iUtility->SubmitUpdate(surface, 0, NULL);
		ASSERT_EQUALS(err,KErrNone);
		WaitL(iCompositionPause);
    	}
    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);  
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0021
@SYMTestCaseDesc		Negative testing - Fastpath one-element scene
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High	
@SYMTestPurpose			Check a scene including a full screen element with global alpha cannot be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Create a scene containing a screen sized element 
	Enable WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA
	Set the value of WFC_ELEMENT_GLOBAL_ALPHA between 0 and 255 (non-inclusive)
	Compose the scene
@SYMTestExpectedResults
	The scene cannot be fastpathed.
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0021L()
	{
    iTestName = _L("FASTPATH_0021");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
												EUidPixelFormatXRGB_8888,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    //set element global alpha
	wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_TRANSPARENCY_TYPES, WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA);
	wfcSetElementAttribf(iDevice, fpElement, WFC_ELEMENT_GLOBAL_ALPHA, 0.0f);
	
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
	iUtility->FillSurfaceL(surface, 0, KGreen);
	err = iUtility->SubmitUpdate(surface, 0, NULL);
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit - using element global alpha not fully opaque"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    
    for (TInt i=0;i<=256;i+=64)
    	{
		i=(i==256)?255:i;
		wfcSetElementAttribf(iDevice, fpElement, WFC_ELEMENT_GLOBAL_ALPHA, (TReal32)i/255);
		if(i == 255)
			{
#ifdef BREAKPOINT_FASTPATH
			__BREAKPOINT();
#endif
			}
        if(i == 255)
            {
            LOG(("OpenWFTest: Expecting FASTPATH after next commit - using element global alpha fully opaque"));
            }
        else
            {
            LOG(("OpenWFTest: Expecting COMPOSITION after next commit - using element global alpha not fully opaque"));
            }
	    wfcCommit(iDevice, iContext, WFC_TRUE);
	    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
		WaitL(iCompositionPause);
    	}
    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);  
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0022
@SYMTestCaseDesc		Negative testing - Fastpath one-element scene
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High	
@SYMTestPurpose			Check a scene with RGB 565 format cannot be fastpathed
@SYMTestActions			
	Create a scene containing a screen sized element with an unsupported source format, such as 565
	Compose the scene
@SYMTestExpectedResults
	The scene cannot be fastpathed.	
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0022L()
	{
    iTestName = _L("FASTPATH_0022");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
    
    //NON fastpathable pixel format
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
												iNonFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit - using non fastpathable pixel format"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    
    iUtility->FillSurfaceL(surface, 0, KGreen);
    err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WaitL(iCompositionPause);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
	ReleaseOnScreenStream();

	ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);  
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0023
@SYMTestCaseDesc		Negative testing - Fastpath one-element scene
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High
@SYMTestPurpose			Check a scene with mask settings cannot be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Create a scene containing a screen sized element A
	Enable WFC_TRANSPARENCY_MASK
	Set a value to WFC_ELEMENT_MASK
	Create a mask element, set the size to be the same as element A's desitnation rectangle 
	Compose the scene
@SYMTestExpectedResults
	The scene cannot be fastpathed.
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0023L()
	{
    iTestName = _L("FASTPATH_0023");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();

    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
												iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    
    TSurfaceId surfaceMask = iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surfaceMask.IsNull());
    WFCMask mask = wfcCreateMaskFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceMask), NULL);
    err = iUtility->SubmitUpdate(surfaceMask, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    
    //set the mask to fpElement
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_TRANSPARENCY_TYPES, WFC_TRANSPARENCY_MASK);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_MASK, mask);
    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit - using a mask"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest:Fastpath - still off - using a mask"));
    
    iUtility->FillSurfaceL(surface, 0, KGreen);
    err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    iUtility->FillSurfaceL(surfaceMask, 0, KBlack);
    err = iUtility->SubmitUpdate(surfaceMask, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WaitL(iCompositionPause);	
 
    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0024
@SYMTestCaseDesc		Negative testing - Fastpath one-element scene
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High
@SYMTestPurpose			Check a scene with alpha and mask settings cannot be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Create a scene containing a screen sized element A
	Enable WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA
	Set the value of WFC_ELEMENT_GLOBAL_ALPHA to be 255 
	Enable WFC_TRANSPARENCY_MASK
	Set a value to WFC_ELEMENT_MASK 	
	Create a mask element, set it the same size of element A's destination rectangle
	Compose the scene
@SYMTestExpectedResults
	The scene cannot be fastpathed.
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0024L()
	{
    iTestName = _L("FASTPATH_0024");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();

    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
												iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    
    TSurfaceId surfaceMask = iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surfaceMask.IsNull());
    WFCMask mask = wfcCreateMaskFromStream(iDevice, iContext, reinterpret_cast<WFCNativeStreamType>(&surfaceMask), NULL);
    err = iUtility->SubmitUpdate(surfaceMask, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    
    //set the mask and global alpha for use with fpElement
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_MASK, mask);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_TRANSPARENCY_TYPES, 
    		WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA|WFC_TRANSPARENCY_MASK);
    //set global alpha to fully opaque
    wfcSetElementAttribf(iDevice, fpElement, WFC_ELEMENT_GLOBAL_ALPHA, 1.0f);
    
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit - using a mask and global alpha"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - still off - using a mask and global alpha"));
    
    iUtility->FillSurfaceL(surface, 0, KGreen);
    err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    iUtility->FillSurfaceL(surfaceMask, 0, KBlack);
    err = iUtility->SubmitUpdate(surfaceMask, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WaitL(iCompositionPause);	
 
    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0025
@SYMTestCaseDesc		Negative testing - Fastpath a scene with scaling element
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High	
@SYMTestPurpose			Check a scene with scaling cannot be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Create an element containing source size smaller than full screen destination size
	Add to scene and compose
@SYMTestExpectedResults
	The scene cannot be fastpathed.
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0025L()
	{
    iTestName = _L("FASTPATH_0025");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
	
	//create half screen size surface
	TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth/2,iFastpathableHeight/2),
												iFastpathablePixelFormat,
												iStride, iContiguous, iMaxBuffers);
	ASSERT_FALSE(surface.IsNull());

	WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
	TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
	wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
	
	//note the source is only half the size of the screen
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iHalfSizeRect);
	//set destination to fullscreen, causing it to stretch
	wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit - using a mask and global alpha"));
	wfcCommit(iDevice, iContext, WFC_TRUE);
	wfcFence(iDevice, iContext, iEGLDisplay, iSync);
	ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
	LOG(("OpenWFTest: Fastpath - still off... - source not equal to destination"));
	
	iUtility->FillSurfaceL(surface, 0, KGreen);
	err = iUtility->SubmitUpdate(surface, 0, NULL);
	ASSERT_EQUALS(err,KErrNone);
	WaitL(iCompositionPause);
	
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KGreen,iTolerance));
	ReleaseOnScreenStream();

	ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
	wfcDestroySource(iDevice, fpSource);  
	}

/*
@SYMTestCaseID			GRAPHICS-OPENWFC-FASTPATH-0026
@SYMTestCaseDesc		Negative testing - Fastpath a scene with rotation
@SYMREQ					
@SYMPREQ						PREQ417-54885
@SYMTestType				Unit Test
@SYMTestPriority		High	
@SYMTestPurpose			Check a scene with rotation cannot be fastpathed
@SYMTestActions			
	All compositions are autonomous:
	Create a scene containing a screen size element 
	Compose the scene
	Rotate the element with 90, 180 and 270 degrees
	Compose the scene
@SYMTestExpectedResults
	The scene can be fastpathed only for the first scene commited composition.
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0026L()
	{
    iTestName = _L("FASTPATH_0026");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    SetupEmptySceneL();
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(iDevice, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    TInt err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WFCElement fpElement = wfcCreateElement(iDevice, iContext, NULL);
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE_RECTANGLE, 4, iFullScreenRect);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_DESTINATION_RECTANGLE, 4, iFullScreenRect);
    
    wfcInsertElement(iDevice, fpElement, WFC_INVALID_HANDLE);
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now ON"));
    
    iUtility->FillSurfaceL(surface, 0, KGreen);
    err = iUtility->SubmitUpdate(surface, 0, NULL);
    ASSERT_EQUALS(err,KErrNone);
    WaitL(iCompositionPause);	
 
    //Set 90 degree rotation
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE_ROTATION, WFC_ROTATION_90);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    iUtility->FillSurfaceL(surface, 0, KBlue);
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now OFF"));
    WaitL(iCompositionPause);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenReferenceImage());
	ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KBlue,iTolerance));
	ReleaseOnScreenStream();

	
    //Set 180 degree rotation
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE_ROTATION, WFC_ROTATION_180);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    iUtility->FillSurfaceL(surface, 0, KMagenta);
    LOG(("OpenWFTest: Expecting FASTPATH after next commit"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now OFF"));
    WaitL(iCompositionPause);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KMagenta,iTolerance));
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ReleaseOnScreenStream();

    //Set 270 degree rotation
    wfcSetElementAttribi(iDevice, fpElement, WFC_ELEMENT_SOURCE_ROTATION, WFC_ROTATION_270);
    
#ifdef BREAKPOINT_FASTPATH
    __BREAKPOINT();
#endif
    iUtility->FillSurfaceL(surface, 0, KBrown);
    LOG(("OpenWFTest: Expecting COMPOSITION after next commit"));
    wfcCommit(iDevice, iContext, WFC_TRUE);
    wfcFence(iDevice, iContext, iEGLDisplay, iSync);
    ASSERT_EQUALS(eglClientWaitSyncKHR(iEGLDisplay,iSync,0,(EGLTimeKHR)EGL_FOREVER_KHR),EGL_CONDITION_SATISFIED_KHR);
    LOG(("OpenWFTest: Fastpath - now OFF"));
    WaitL(iCompositionPause);
    
    ASSERT_TRUE(AcquireOnScreenStream());
    ASSERT_TRUE(CheckOnScreenStreamRect(iFullScreenTRect,KBrown,iTolerance));
    ASSERT_TRUE(CheckOnScreenReferenceImage());
    ReleaseOnScreenStream();

    ASSERT_EQUALS(wfcGetError(iDevice),WFC_ERROR_NONE);
    wfcDestroySource(iDevice, fpSource);  
	}

/*
@SYMTestCaseID          GRAPHICS-OPENWFC-FASTPATH-0027
@SYMTestCaseDesc        Negative testing - Pass in bad parameter values 
@SYMREQ                 
@SYMPREQ                PREQ417-54885
@SYMTestType            Unit Test
@SYMTestPriority        High    
@SYMTestPurpose         Check invalid parameter values passed in OWF APIs
@SYMTestActions 
    Use invalid values to call OWF APIs
@SYMTestExpectedResults
    Test should pass without panic.
*/
void COpenwfTest::GRAPHICS_OPENWFC_FASTPATH_0027L()
    {
    iTestName = _L("FASTPATH_0027");
    INFO_PRINTF2(_L("GRAPHICS_OPENWFC_%SL()"),&iTestName);
    LOGL((_L("OpenWFTest: GRAPHICS_OPENWFC_%SL()"),&iTestName));
    
    WFCint deviceId;
    WFCint filterList[] = { WFC_DEVICE_FILTER_SCREEN_NUMBER, WFC_DEFAULT_SCREEN_NUMBER, WFC_NONE};
    ASSERT_TRUE(wfcEnumerateDevices(&deviceId, 1, filterList) == 1);

    WFCint attribList = 1;
    WFCDevice device = wfcCreateDevice(deviceId, &attribList);
    WFCContext onScreenContext = wfcCreateOnScreenContext(NULL, WFC_DEFAULT_SCREEN_NUMBER, NULL);
    WFCContext offScreenContext = wfcCreateOffScreenContext(NULL, WFC_DEFAULT_SCREEN_NUMBER, NULL);

    WFCfloat value = 1.0;
    wfcGetContextAttribi(NULL, iContext, WFC_CONTEXT_TARGET_HEIGHT);
    wfcSetContextAttribi(NULL, iContext, WFC_CONTEXT_ROTATION, WFC_ROTATION_0);
    wfcGetContextAttribfv(NULL, iContext, WFC_CONTEXT_ROTATION, NULL, &value);
    wfcSetContextAttribfv(NULL, iContext, WFC_CONTEXT_ROTATION, NULL, &value);
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(iFastpathableWidth,iFastpathableHeight),
                                                iFastpathablePixelFormat,
                                                iStride, iContiguous, iMaxBuffers);
    ASSERT_FALSE(surface.IsNull());

    WFCSource fpSource = wfcCreateSourceFromStream(NULL, iContext,reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    wfcCreateMaskFromStream(NULL, iContext, reinterpret_cast<WFCNativeStreamType>(&surface), NULL);
    WFCElement fpElement = wfcCreateElement(NULL, iContext, NULL);
    wfcSetElementAttribi(NULL, fpElement, WFC_ELEMENT_SOURCE, fpSource);
    wfcSetElementAttribf(NULL, fpElement, WFC_ELEMENT_SOURCE, NULL);
    wfcSetElementAttribiv(NULL, fpElement, WFC_ELEMENT_SOURCE, NULL, NULL);
    wfcSetElementAttribfv(NULL, fpElement, WFC_ELEMENT_SOURCE, NULL, NULL);
    wfcSetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE, NULL, NULL);
    wfcSetElementAttribfv(iDevice, fpElement, WFC_ELEMENT_SOURCE, NULL, NULL);
    
    wfcGetElementAttribi(NULL, fpElement, WFC_ELEMENT_SOURCE);
    wfcGetElementAttribf(NULL, fpElement, WFC_ELEMENT_SOURCE);
    wfcGetElementAttribiv(NULL, fpElement, WFC_ELEMENT_SOURCE, NULL, NULL);
    wfcGetElementAttribiv(iDevice, fpElement, WFC_ELEMENT_SOURCE, NULL, NULL);
    wfcGetElementAttribfv(NULL, fpElement, WFC_ELEMENT_SOURCE, NULL, NULL);
    wfcGetElementAttribfv(iDevice, fpElement, WFC_ELEMENT_SOURCE, NULL, NULL);
    
    wfcInsertElement(NULL, fpElement, NULL);
    wfcGetElementAbove(NULL, fpElement);
    wfcGetElementBelow(NULL, fpElement);
        
    WFCDeviceAttrib devAttrib = WFC_DEVICE_ID;
    wfcGetDeviceAttribi(NULL, devAttrib);

    wfcActivate(NULL, iContext);
    wfcActivate(iDevice, NULL);
    wfcDeactivate(NULL, iContext);
    wfcDeactivate(iDevice, NULL);
    wfcCompose(NULL, iContext, WFC_TRUE);
    wfcCompose(iDevice, NULL, WFC_TRUE);
    wfcCommit(NULL, iContext, WFC_TRUE);
    wfcCommit(iDevice, NULL, WFC_TRUE);
    wfcFence(NULL, iContext, iEGLDisplay, iSync);
    wfcFence(iDevice, NULL, iEGLDisplay, iSync);

    WFCMask mask = WFC_INVALID_HANDLE;
    wfcDestroyMask(NULL, mask);
    wfcRemoveElement(NULL, fpElement);
    wfcDestroyElement(NULL, fpElement);
    wfcDestroySource(NULL, fpSource);  
    wfcDestroyContext(NULL, onScreenContext);
    }

