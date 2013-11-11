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
//

#ifndef OPENWFTEST_H
#define OPENWFTEST_H

// INCLUDES
#include <test/tefunit.h>
#include <w32std.h>
#include <graphics/symbianstream.h>
#include <graphics/surfacemanager.h>
#include <graphics/streammap.h>
#include <EGL/egl.h>
#include <graphics/eglsynchelper.h>
#include <WF/wfc.h>
#include "surfaceutility.h"

class CTestExecuteLogger;
class CSurfaceStream;

/* Color defines for openwf test.*/
#define KDarkRed		TRgb(127,0,0,255)
#define KRed			TRgb(255,0,0,255)
#define KGreen			TRgb(0,255,0,255)
#define KBlue			TRgb(0,0,255,255)
#define KCyan			TRgb(0,255,255,255)
#define KMagenta		TRgb(255,0,255,255)
#define KYellow			TRgb(255,255,0,255)
#define KBrown			TRgb(150,75,0,255)
#define KBlack			TRgb(0,0,0,255)
#define KBlackAlpha0	TRgb(0,0,0,0)
#define KWhite			TRgb(255,255,255,255)
#define KWhiteAlpha0	TRgb(255,255,255,0)
#define KGray			TRgb(127,127,127,255)
#define KGrayHalfAlpha	TRgb(127,127,127,127)

#define KMaxImagePathLength 100
#define KMaxTestNameLength 20

class CActiveNotification : public CActive
	{
public:
	static CActiveNotification* NewL(RSurfaceUpdateSession& aSurfaceUpdateSession,TBool aActivate);
	~CActiveNotification();
	void Activate();
	// From CActive:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
protected:
	CActiveNotification();
	void ConstructL(RSurfaceUpdateSession& aSurfaceUpdateSession);
private:
	RSurfaceUpdateSession iSurfaceUpdateSession;
	};

//Use instead of User::After
class CActiveWaiter : public CActive
	{
public:
	static CActiveWaiter* NewL();
	~CActiveWaiter();
	TInt Wait(TTimeIntervalMicroSeconds32 aDelay);
	void StartWait(TTimeIntervalMicroSeconds32 aDelay);
	// From CActive:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
protected:
	CActiveWaiter();
	void ConstructL();
public:
	TBool iRun;
protected:
	RTimer iTimer;
	};

/**
 *  COpenwfTest
 * 
 */
class COpenwfTest : public CTestFixture
	{
public:
	COpenwfTest();
	~COpenwfTest();

	virtual void SetupL();
	virtual void TearDownL();
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
    enum TCacheAttribute
        {
	    /** not CPU cached */
	    ENotCached = 0,
	    /** CPU cached */
	    ECached = 1,
	    /** Not listed */
	    ECacheNotlisted = 2
	    };

    enum TContiguousFlag
        {
        ENonContiguous = 0,
        EContiguous = 1,
        EFlagNotListed = 2
        };
    
    enum TTestMode
        {
        EAutomatic = 0,
        EManual = 1
        };
    
private:
    TBool AcquireOnScreenStream();
    void ReleaseOnScreenStream();
    TBool CheckOnScreenStreamRect(TRect aRect, const TRgb& aExpectedColor, TInt aTolerance);
    TBool CheckRect(const TSurfaceId& aSurface, TInt aNumOfBuffer, TRect aRect, 
    		const TRgb& aExpectedColor, TInt aTolerance);
    TBool CheckOnScreenReferenceImage();
    TBool SaveImage(const TSurfaceId& aSurfaceId, TInt aNumOfBuffer);
    TInt WaitL(TTimeIntervalMicroSeconds32 aDelay);	//aDelay in microseconds
    TBool WaitForNotifications(TTimeIntervalMicroSeconds32 aMaxDelay,TInt aNotificatons);
    void ReadIniData();
    void TearDown();
    void SetupEmptySceneL(TBool aAutonomous = ETrue);
    
	// Fastpath positive tests
	void GRAPHICS_OPENWFC_FASTPATH_0000L();
	void GRAPHICS_OPENWFC_FASTPATH_0001L();
	void GRAPHICS_OPENWFC_FASTPATH_0002L();
	void GRAPHICS_OPENWFC_FASTPATH_0003L();
    void GRAPHICS_OPENWFC_FASTPATH_0004L();
    void GRAPHICS_OPENWFC_FASTPATH_0005L();
    void GRAPHICS_OPENWFC_FASTPATH_0006L();
    void GRAPHICS_OPENWFC_FASTPATH_0007L();
	
	// Fastpath with notification tests
    void FastpathNotificationsTestL(TBool aAutonomous);
	void GRAPHICS_OPENWFC_FASTPATH_0010L();
	void GRAPHICS_OPENWFC_FASTPATH_0011L();
	void FastpathDispXNotificationsTestL(TBool aAutonomous);
	void GRAPHICS_OPENWFC_FASTPATH_0012L();
	void GRAPHICS_OPENWFC_FASTPATH_0013L();
	
	// Fastpath negative tests
	void GRAPHICS_OPENWFC_FASTPATH_0020L();
	void GRAPHICS_OPENWFC_FASTPATH_0021L();
	void GRAPHICS_OPENWFC_FASTPATH_0022L();
	void GRAPHICS_OPENWFC_FASTPATH_0023L();
	void GRAPHICS_OPENWFC_FASTPATH_0024L();
	void GRAPHICS_OPENWFC_FASTPATH_0025L();
	void GRAPHICS_OPENWFC_FASTPATH_0026L();
    void GRAPHICS_OPENWFC_FASTPATH_0027L();
			
private:
    TInt iFastpathableWidth;
    TInt iFastpathableHeight;
    TInt iStride;        
    TContiguousFlag iContiguous;
    TUidPixelFormat iFastpathablePixelFormat;
    TUidPixelFormat iNonFastpathablePixelFormat;
    TInt iOffsetToFirstBuffer; 
    TInt iAlignment;           
    TBool iMappable;
    TInt iMaxBuffers;              
    TInt iMinBuffers;
    TCacheAttribute iCacheAttrib;
    TInt iOffsetBetweenBuffers;
    TInt iTestMode;
    TBool iSaveImage;
    TTimeIntervalMicroSeconds32 iCompositionPause;	//delay in microseconds
    TTimeIntervalMicroSeconds32 iManualPause;	//delay for manual checking in microseconds
    TBool iAllowManualPause;  //some tests may not allow manual pauses
    TInt iTolerance;
    WFCint iDeviceId;
    EGLSyncKHR iSync;
    WFCDevice iDevice;
    WFCContext iContext;

    TRect iFullScreenTRect;
    WFCint iFullScreenRect[4];
    TRect iCenterTRect;
    WFCint iCenterRect[4];
    WFCint iHalfSizeRect[4];
    
    CActiveScheduler* iScheduler;
    EGLDisplay iEGLDisplay;
    CSurfaceUtility* iUtility;
    //TPtrC iDir;
    TPtrC iImageDirectory;
    TBufC<KMaxTestNameLength> iTestName;
	TBuf<KMaxImagePathLength> iImageAddress;
    TInt iImageCounter;
    SymbianStreamType iOnScreenStream;
    SymbianStreamBuffer iOnScreenBuffer;
	};

#endif // OPENWFTEST_H
