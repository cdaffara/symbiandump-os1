// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Base classes for Video Client tests
// 
//

#ifndef TSI_MMF_VCLNTAVI_STEP_H
#define TSI_MMF_VCLNTAVI_STEP_H

#include <e32base.h>
#include <badesca.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

#include <fbs.h>
#include <w32std.h>
#include <mmf/common/mmfstandardcustomcommands.h> 
#include <mda/common/video.h>

#include <videoplayer.h>
#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h> 
#include <mediaclientvideodisplay.h>
#endif
#include <videorecorder.h>
#include <ecam.h>

// Video Controller UIDs
const TUid KAviRecordFormatUid =    {0x102737E3};
const TUid KVideoStubControllerUid = {0x101F88D9};
const TUid KVideoInvalidControllerUid = {0xA0A0A0A0};
const TUid KVideoTestSCBControllerUid = {0x101FE477};

// Video Controller UIDs
const TUid KMmfTestAviPlayControllerUid = { 0x102737A5 };
const TUid KMmfTestAviRecordControllerUid = { 0x102737E1 };
const TUid KMmfTestAviNonGcePlayControllerUid = { 0x10283420 };
	

const TInt KMMFVideoRecordTimeAvailable = 10000000;
const TReal32 KDefaultFrameRate = 10.00;

_LIT(KECamTestCameraPluginName, "C:\\102070cc.txt");
_LIT(KBinaryCompatibilityKeyName, "binarycomp");

_LIT(KMMCameraPluginName, "C:\\mmcameraclientplugin.txt");

// Local function to get the drive name
TAny GetDriveName(TDes& aFileName);

class CTestSuite ;

/**
 * Global Data used within tests
 *
 */
const TInt KTwelfthOfSec        = 83333;
const TInt KTenthOfSec            = 100000 ;
const TInt KOneSecond            = 1000000;    // Used in a time out function, 1 second (in microSeconds)
const TInt KTenSeconds            = 10000000;    // Used in a time out function, 10 seconds (in microSeconds)
const TInt KFiveSeconds            = 5000000;    // 5 seconds (in microSeconds)
const TInt KFifteenSeconds        = 15000000;    // 15 seconds (in microSeconds)
const TInt KExpectedDeviation    = 1750000;    // 1.75 second tolerance for timed tests
const TInt KSixSeconds			 = 6000000;
const TInt KNameBufSize            = 25;
const TInt KMaxBalance            = 100;
const TInt KMinBalance          = -100;
const TInt KMaxVolume           = 65535;


// Enum to decide Agent has generated the event.
enum TVclntTestPlayEvents
    {
    EVPIdle = 0,
    EVPOpenComplete,
    EVPPrepareComplete,
    EVPFrameReady,
    EVPPlayComplete,
    EVPPlayStopped,
    EVPPlayPaused
    };

enum TVclntTestRecordEvents
    {
    EVRIdle = 0,
    EVROpenComplete,
    EVRRecordPaused,
    EVRRecordResumed,
    EVRRecordStopped,
    EVRRecordComplete,
    EVRPrepareComplete,
    EVRViewFinderReady,
    EVRImageBufferReady,
    EVRVideoBufferReady,
    };

// Global function to get the drive name
TAny GetDriveName(TDes& aFileName);
    
/**
 * It's a base class for all test steps.
 *
 * RTestMmfVclntAviStep 
 * Base Test step class that enables tests.
 *
 */
class RTestMmfVclntAviStep : public RTestStep
    {
public:
    RTestMmfVclntAviStep(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    void SetTestSuite(const CTestSuite* aTestSuite ) { iTestSuite = aTestSuite; }

protected:
    void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds);
    TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation);
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    virtual void InitWservL();
    virtual void DeInitWserv();
    // Function to delete a file
    TInt DeleteFileL(const TDesC &aFileName);
    TUid ControllerUid() const;
protected:
    const CTestSuite* iTestSuite ;
    CActiveScheduler* iActiveScheduler;
    
    // Window server related objects
    RWsSession iWs;
    CWsScreenDevice* iScreen;
    RWindow* iWindow;
    RWindowGroup iRootWindow;
    
    // Members to hold the ini file sections and the file name for each test.
    TBuf<KNameBufSize> iSectName;            // Section name for retrieving filename
    TBuf<KNameBufSize> iKeyName;            // Key name for retrieving filename
    TFileName iFilename;
    
    TInt iTestExpectedError;
    TInt iExpectedError;
    TInt iError;
    TBool iBinaryCompatibility;
#ifdef SYMBIAN_BUILD_GCE
    CMediaClientVideoDisplay* iMediaClientVideoDisplay;
#endif
    };

/**
 * It's a base class for all test steps using CVideoPlayerUtility for video playback
 *
 * RTestVclntAviPlayerStep
 * Base Test step class class for Video Player tests using CVideoPlayerUtility
 *
 */
class RTestVclntAviPlayerStep : public RTestMmfVclntAviStep, public MVideoPlayerUtilityObserver
    {
public:
    RTestVclntAviPlayerStep(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);

protected:
    // from RTestStep
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    // from MVideoPlayerUtilityObserver
    virtual void MvpuoOpenComplete(TInt aError);
    virtual void MvpuoPrepareComplete(TInt aError);
    virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
    virtual void MvpuoPlayComplete(TInt aError);
    virtual void MvpuoEvent(const TMMFEvent& aEvent);
    //State Machine
    virtual void FsmL(TVclntTestPlayEvents aEventCode) = 0;
    virtual TBool FsmCheck(TVclntTestPlayEvents aEventCode);
    void ResetState();
    void PrepareState(TVclntTestPlayEvents aExpectedEvent, TInt aExpectedError = KErrNone);
protected:
    CVideoPlayerUtility* iVideoPlayer;
    TVclntTestPlayEvents iExpectedEvent;
    };


#ifdef SYMBIAN_BUILD_GCE 
/**
 * It's a base class for all test steps using CVideoPlayerUtility2 for video playback
 *
 * RTestVclntAviPlayerStep2
 * Base Test step class class for Video Player tests using CVideoPlayerUtility2
 *
 */
class RTestVclnt2AviPlayerStep : public RTestVclntAviPlayerStep
    {
public:
    RTestVclnt2AviPlayerStep(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    
protected:
    // from RTestStep
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    virtual TVerdict DoTestStepL();    
    
    // state machine
    virtual TBool FsmCheck(TVclntTestPlayEvents aEventCode);    

    // for the setup of display window 
    virtual void InitWservL();
    virtual void DeInitWserv();

protected:
    CVideoPlayerUtility2* iVideoPlayer2;
    CWindowGc* iGc;    
    };
#endif

/**
 * It's a base class for all test steps.
 *
 * RTestVclntAviRecorderStep
 * Base Test step class class for Video Recorder tests.
 *
 */
class RTestVclntAviRecorderStep : public RTestMmfVclntAviStep, public MVideoRecorderUtilityObserver, public MCameraObserver2
    {
public:
    RTestVclntAviRecorderStep(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);

protected:
    // from RTestStep
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    // MVideoRecorderUtilityObserver
    virtual void MvruoOpenComplete(TInt aError);
    virtual void MvruoRecordComplete(TInt aError);
    virtual void MvruoPrepareComplete(TInt aError);
    virtual void MvruoEvent(const TMMFEvent& aEvent);
    // from MCameraObserver2
    virtual void HandleEvent(const TECAMEvent &aEvent);
    virtual void ViewFinderReady(MCameraBuffer &aCameraBuffer, TInt aError);
    virtual void ImageBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);
    virtual void VideoBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);
    //State Machine
    virtual void FsmL(TVclntTestRecordEvents aEventCode) = 0;
    TBool FsmCheck(TVclntTestRecordEvents aEventCode);
    void ResetState();
    void PrepareState(TVclntTestRecordEvents aExpectedEvent, TInt aExpectedError = KErrNone);
protected:
    CVideoRecorderUtility* iVideoRecorder;
    CCamera* iCamera;
    TInt iCameraHandle;
    TVclntTestRecordEvents iExpectedEvent;
    };

#endif// TSI_MMF_VCLNTAVI_STEP_H

