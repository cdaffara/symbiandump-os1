
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
// TSI_MMFMIDICLNT.h
// Header file: Basic  tests.
// 
//

/**
 @file TSI_MMFVCLNT.h
*/

#ifndef TSU_MMFMIDICLNT_H__
#define TSU_MMFMIDICLNT_H__

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
#include <midiclientutility.h>
#include <mmf/common/mmfcontrollerframework.h>

// Uncomment this line for Jet-stream project
//#define JETSTREAM_TESTS  1 

class CTestSuite ;

// Flags for test step differentiation
enum TTestStepType
{
	ETestValid = 0,
	ETestNegative,
	ETestNullMessage,
	ETestInvalidMessage,
	ETestInvalidId,
	ETestInvalidIndex,
	ETestInvalidChannel,
	ETestAlreadyLoaded,
	ETestNotLoaded,
	ETestNotUnloadable,
	ETestOutOfRangeHigh,
	ETestOutOfRangeLow,
	ETestTimeOutOfRangeHigh,
	ETestTimeOutOfRangeLow,
	ETestUnsupported,
	ETestCorrupt,
	ETestOverflow,
	ETestNoPlugin,
	ETestInvalidState,
	ETestNoResource,
	ETestZeroDurationOutOfRange,
	ETestInvalidChannelOutOfRange,
	ETestNoteHighOutOfRange,
	ETestNoteLowOutOfRange,
	ETestAttackHighOutOfRange,
	ETestAttackLowOutOfRange,
	ETestReleaseHighOutOfRange,
	ETestReleaseLowOutOfRange,
	ETestNoNoteAvailable,
	ETestDifferentNoteVelocity,
	ETestVelocityHighOutOfRange,
	ETestVelocityLowOutOfRange,
	ETestBeatsHighOutOfRange,
	ETestBeatsLowOutOfRange,
	ETestInvalidTrack,
	ETestNoResourcePlaying,
	ETestMicrosecondsHighOutOfRange,
	ETestMicrosecondsLowOutOfRange,
	ETestMicrobeatsHighOutOfRange,
	ETestMicrobeatsLowOutOfRange,
	ETestMicrosecondsMicrobeatsZero,
	ETestVolumeHighOutOfRange,
	ETestVolumeLowOutOfRange,
	ETestRampDurationHighOutOfRange,
	ETestRampDurationLowOutOfRange,
	ETestBalanceHighOutOfRange,
	ETestBalanceLowOutOfRange,
	ETestPreferenceConflictsCannotBeResolved,
	ETestMetaDataIndexInvalid,
	ETestOutOfRange,
	ETestNoStandardBanks,
	ETestNoCustomBanks,
	ETestCheckInterface
};

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
	// From CActive, From Base Class
	void RunL() { CActiveScheduler::Stop(); }
	void DoCancel() {};
	};

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


enum TDevMidiByPass
	{
    EDevMidiOff = 0,
    EDevMidiOn = 1
	};

/**
 * It's a base class for all test steps.
 *
 * @class CTestMmfVclntStep 
 * @brief Test class that enables tests.
 *
 */
class CTestMmfMidiClntStep : public CTestStep, public MMidiClientUtilityObserver
	{
public:
	CTestMmfMidiClntStep(const TDesC& aTestName, const TTestStepType aTestType);
	CTestMmfMidiClntStep();
	~CTestMmfMidiClntStep();
	//void Close();
	static void CleanUp(TAny *aPtr) ;
	void SetTestSuite(const CTestSuite* aTestSuite ) { iTestSuite = aTestSuite; }  

	// MMidiClientUtilityObserver
	virtual void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError);
	virtual void MmcuoTempoChanged(TInt aMicroBeatsPerMinute);
	virtual void MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels);
	virtual void MmcuoMuteChanged(TInt aChannel,TBool aMuted);
	virtual void MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats);
	virtual void MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition);
	virtual void MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aEntry);
	virtual void MmcuoPolyphonyChanged(TInt aNewPolyphony);
	virtual void MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId);

protected:
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds);
	TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDivation);
	// tests may optionally implement pre- and post-ambles, From Base Class
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CMidiClientUtility* aMidi);
	void InitWservL();

protected:
	const CTestSuite* iTestSuite ;
	CActiveScheduler* iActiveScheduler;
	TInt iError;

	TInt iTestType;

	CWsScreenDevice* iScreen;
	RWindow* iWindow;
	RWsSession iWs;
	TMidiState iCurrentState;
	};

#endif




