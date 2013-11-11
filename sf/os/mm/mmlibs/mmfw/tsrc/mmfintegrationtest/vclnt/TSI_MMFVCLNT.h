
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file TSI_MMFVCLNT.h
*/

#ifndef TSI_MMFVCLNT_H__
#define TSI_MMFVCLNT_H__

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
// *** Jim - added for getting dummy video format uid 
#include <mda/common/video.h>

#include <videoplayer.h>
#include <videorecorder.h>

// *** Jim - added for getting dummy video controller uid 
// stolen from CMmfVideoTestControllerUIDs.hrh
#define KMmfVideoTestControllerUid                   0x101F88D9

// Uncomment this line for Jetstream project
//#define JETSTREAM_TESTS  1 


class CTestSuite ;

/**
 * Global Data used within tests
 *
 */

const TInt KTwelfthOfSec		= 83333;
const TInt KTenthOfSec			= 100000 ;
const TInt KOneSecond			= 1000000;	// Used in a time out function, 1 second (in microSeconds)
const TInt KTenSeconds			= 10000000;	// Used in a time out function, 10 seconds (in microSeconds)
const TInt KFiveSeconds			= 5000000;	// 5 seconds (in microSeconds)
const TInt KFifteenSeconds		= 15000000;	// 15 seconds (in microSeconds)
const TInt KExpectedDeviation	= 1500000;	// 1.5 second tolerance for timed tests
const TInt KNameBufSize			= 25;


const TInt KMaxBalance	=  100;
const TInt KMinBalance  = -100;

const TInt KMaxVolume   =  100;

/**
 * @ MCallbackHandlerObserver, provides an Observer
 * @ for monitoring callbacks.
 */
class MCallbackHandlerObserver
	{
public:
	virtual void MchoComplete(TInt aID, TInt aError)=0;
	};


/**
 * CCallbackHandler provides an Observer for 
 * monitoring MVideoPlayerUtilityObserver callbacks. 
 *
 * @class CVideoCallbackHandler
 *
 */
class CVideoCallbackHandler : public MVideoPlayerUtilityObserver
	{
public:
	CVideoCallbackHandler(const TInt aID, MCallbackHandlerObserver* aMchObserver);
	TInt ID();
	// from MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame);
	virtual void MvpuoPlayComplete(TInt aError);

private:
	MCallbackHandlerObserver* iMchObserver;
	TInt iID;
	};


/**
 * CCallbackHandler provides an Observer for 
 * monitoring MVideoRecorderUtilityObserver callbacks. 
 *
 * @class CVideoRecorderCallbackHandler
 *
 */
class CVideoRecorderCallbackHandler : public MVideoRecorderUtilityObserver
	{
public:
	CVideoRecorderCallbackHandler(const TInt aID, MCallbackHandlerObserver* aMchObserver);
	TInt ID();
//	const TInt ID();
	// MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);
	virtual void MvruoRecordComplete(TInt aError);

private:
	MCallbackHandlerObserver* iMchObserver;
	TInt iID;
	};


/**
 *
 * CActiveListener provides the asynchronous operation
 * of an active object
 *
 * @class CActiveListener
 *
 */
class CActiveListener : public CActive
	{
public:
	CActiveListener() : CActive(CActive::EPriorityIdle) { CActiveScheduler::Add(this); }
	void InitialiseActiveListener() { iStatus = KRequestPending; SetActive(); }
	// From CActive
	virtual void RunL() { CActiveScheduler::Stop(); }
	virtual void DoCancel() {};
	};


/**
 * It's a base class for all test steps.
 *
 * @class CTestMmfVclntStep 
 * @brief Test class that enables tests.
 *
 */
class CTestMmfVclntStep : public CTestStep
	{
public:
	CTestMmfVclntStep();
	~CTestMmfVclntStep();
	static void CleanUp(TAny *aPtr) ;
	void SetTestSuite(const CTestSuite* aTestSuite ) { iTestSuite = aTestSuite; }  

protected:
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds);
	TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDivation);
	// tests may optionally implement pre- and post-ambles
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

	void InitWservL();
protected:
	const CTestSuite* iTestSuite ;
	CActiveScheduler* iActiveScheduler;

	CWsScreenDevice* iScreen;
	RWindow* iWindow;
	RWsSession iWs;
	};

#endif// TSI_MMFVCLNT_H__

