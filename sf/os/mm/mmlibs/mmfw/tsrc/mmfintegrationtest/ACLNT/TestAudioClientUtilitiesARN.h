// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Declaration for Various Classes used for Audio Resource Notification tests.
// 
//

/**
 @file TestAudioClientUtilitiesARN.h
*/

#ifndef TESTAUDIOCLIENTUTILITIESARN_H
#define TESTAUDIOCLIENTUTILITIESARN_H

#include "mdaaudiosampleplayer.h"
#include "mdaaudiosampleeditor.h"
#include "mdaaudiooutputstream.h"
#include "TestPlayerUtils.h"
#include "TestRecorder.h"
#include "TSI_MMFACLNT.h"
#include <mmf/common/mmfaudio.h>

//------------------------------------------------------------------------------
//********************PREQ 797 - Audio Resource Notification********************
//------------------------------------------------------------------------------

/**
 * stopActiveScheduler
 */
static TInt stopActiveScheduler(TAny*)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}

//
//Declarations//
//

//
//Custom Error Codes & CONST Declarations.
//
const TInt KErrUser = -1000;
const TInt KErrBadResult = (KErrUser - 1);
const TInt KMicroSecsInOneSec = 1000000; 
const TUint KSampleRate8000Hz  = 8000;

//
//Forward class declarations
//
class CCallbackTimer;
class CTestMdaAudioPlayerCallbackSupport;
class CTestMdaAudioRecorderCallbackSupport;
class CTestMdaAudioOutputStreamCallbackSupport;
class CTestMdaAudioMultiClientCallbackSupport;

//------------------------------------------------------------------------------
//******************PREQ 797 - Audio Player Test Declarations*******************
//------------------------------------------------------------------------------
class CMultiEventChecker: public CBase
  {	
public:
	// MultiEvent checker utility enums
	enum TClientUtilityType
		{
		EAudioPlayer = 0,
		EAudioRecorder,
		EAudioStream	
		};
public:
	static CMultiEventChecker* NewL();
    TBool CheckFsm(TInt aClientNumber, TClientUtilityType aClientType,TInt aEventCode, TInt aError=KErrNone,CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	void StopTest();
	void SetExpectedState(TInt aState);
	void SetExpectedEvent(TInt aClientNumber, TInt aEvent,TInt aError,CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	void RegisterClientEventToMonitor(TInt aClientNumber, TInt aEventCode, TInt aError, TInt aState,CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	void EnableMultipleEventMonitor()
		{
		iMonitorMultipleEvents = ETrue;	
		}
	void DisableMultipleEventMonitor()
		{
		iMonitorMultipleEvents = EFalse;		
		}
	TBool IsMutipleEventMonitorEnabled()
		{
		return iMonitorMultipleEvents;
		}
	TInt NextState()
		{
		return iNextState;			
		}
	TBool IsMonitoringFinished();
protected:
	void ConstructL();
	CMultiEventChecker();	
	//Register client events to monitor
	TInt iExpectedEvent;
	TInt iExpectedError;
	CMdaAudioClipUtility::TState iExpectedRecorderState;
	TInt iNextState;
	TFixedArray <TInt, 4> iClientEventsToMonitor;
	TFixedArray <TInt, 4> iClientError;
	TFixedArray <TInt, 4> iFsmStateforClient;
	TInt iClientsRegistered;
	TBool iMonitorMultipleEvents;
	};

//
//BASE CLASS DECLARATIONS//
//

/**
 * Observer class to identify and pass events to the test step class
 * 
 * @class MTestAudioPlayerCallback
 *
 */
class MTestAudioPlayerCallback
	{
public :
	virtual void MapcInitCompleteTest(TInt aError, const TTimeIntervalMicroSeconds& aDuration, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport) = 0;
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport) = 0;
	virtual void MapcPlayCompleteTest(TInt aError, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport) = 0;
	};

/**
 * Support class to pass the events to the test step class using the MTestAudioPlayerCallback
 * 
 * @class CTestMdaAudioPlayerCallbackSupport
 *
 */
class CTestMdaAudioPlayerCallbackSupport : public CBase, public MMdaAudioPlayerCallback, public MMMFAudioResourceNotificationCallback
	{
public :
	static CTestMdaAudioPlayerCallbackSupport* NewL(MTestAudioPlayerCallback& aCallback);
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);
	virtual void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
protected:
	CTestMdaAudioPlayerCallbackSupport(MTestAudioPlayerCallback& aCallback);
	void ConstructL();
private :
	MTestAudioPlayerCallback& iCallback;
	};


/**
 * Base Class for ARN functionality for most ARN audio player tests
 * 
 * @class CTestMmfAclntAudioPlayerARN
 *
 */
class CTestMmfAclntAudioPlayerARN : public CTestMmfAclntStep, public MTestAudioPlayerCallback
	{
public:
	// Enum to describe the current state.
	enum TMmfAudioPlayerState
		{
		EStateBegin = 0,
		EStateOpenPlayer1,
		EStateOpenPlayer2,
		EStateOpenPlayer3,
		EStatePlaying1,
		EStatePlaying2,
		EStatePlaying3,
		EStateEndPlay1,
		EStateEndPlay2,
		EStateEndPlay3,
		EStateErrInUsePlayer1,
		EStateErrInUsePlayer2,
		EStateErrInUsePlayer3,
		EStateNotifiedPlayer1,
		EStateNotifiedPlayer2,
		EStateNotifiedPlayer3,
		EStateErrInUsePlayer1A,
		EStateNotifiedPlayer1A,
		EStatePlaying2A,
		EStateEndPlay2A
		};

	// Enum to decide which Event has occured.
	enum TMmfAudioPlayerEvents
		{
		EIdlePlayer = 0,
		EMapcInitComplete1,
		EMapcInitComplete2,
		EMapcInitComplete3,
		EMapcPlayComplete1,
		EMapcPlayComplete2,
		EMapcPlayComplete3,
		EMarncResourceAvailable1,
		EMarncResourceAvailable2,
		EMarncResourceAvailable3
		};
public :
	// Constructor
	CTestMmfAclntAudioPlayerARN(const TDesC& aTestName, const TDesC& aSectName);
	// Virtual methods for Preamble and Postamble.
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// Virtual function to access test specific methods
	virtual TVerdict DoTestStepL();
	// Pure function to access test specific methods
	virtual TVerdict DoTestL()=0;
	// from MTestAudioPlayerCallback
	virtual void MapcInitCompleteTest(TInt aError, const TTimeIntervalMicroSeconds& aDuration, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport);
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport);
	virtual void MapcPlayCompleteTest(TInt aError, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport);
	// Centralised State Handler
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError)=0;
	virtual TBool FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError);
protected :	
		// StateInit Function
	void StateInit();
	// StateChange Function
	void StateChange(TMmfAudioPlayerState aState, TMmfAudioPlayerEvents aExpectedEvent, TInt aExpectedError=KErrNone);
protected :	
	// CMdaAudioPlayerUtility instances
	CMdaAudioPlayerUtility* iTestAudioPlayer1;
	CMdaAudioPlayerUtility* iTestAudioPlayer2;
	CMdaAudioPlayerUtility* iTestAudioPlayer3;
	// CTestMdaAudioPlayerCallbackSupport instances
	CTestMdaAudioPlayerCallbackSupport* iMdaAudioPlayerCallbackSupport1;
	CTestMdaAudioPlayerCallbackSupport* iMdaAudioPlayerCallbackSupport2;
	CTestMdaAudioPlayerCallbackSupport* iMdaAudioPlayerCallbackSupport3;
	// Other class variable declarations
	TBuf<KSizeBuf> iFilename1;
	TBuf<KSizeBuf> iFilename2;
	TBuf<KSizeBuf> iFilename3;
	TTimeIntervalMicroSeconds iPosition;
	TTimeIntervalMicroSeconds iDuration;
	TInt iError;
	TMmfAudioPlayerState iState;
	TRequestStatus iRequestStatus;
	CCallBackTimer* iCallBackTimer;
	TMmfAudioPlayerEvents iExpectedEvent;
	TInt iExpectedError;
	TMmfAudioPlayerEvents iEventOccured;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	CMultiEventChecker* iEventChecker;
	};

//
//POSITIVE TESTS//
//

/** 
 * Register for notification, but make no request to play audio.
 *
 * @class CTestMmfAclntARN8101
 * 
 */
class CTestMmfAclntARN8101 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN8101* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
	
protected:
	CTestMmfAclntARN8101( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Register (b4 play, after Open) and Receive Notification of a Resource Available Event.
 * (when audio resource is currently unavailable and then available)
 *
 * @class CTestMmfAclntARN8102
 * 
 */
class CTestMmfAclntARN8102 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN8102* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN8102( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Register (at the time of Play) and Receive Notification of a Resource Available Event.
 * (when audio resource is currently available and subsequently unavailable, and then available)
	
 *
 * @class CTestMmfAclntARN8103
 * 
 */
class CTestMmfAclntARN8103 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN8103* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
	virtual TBool FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN8103( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Registration (b4 File Open) and Cancellation of  the notification request
 *
 * @class CTestMmfAclntARN8104
 * 
 */
class CTestMmfAclntARN8104 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN8104* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN8104( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Register (after Play Error) and Get the actual position from where the paused audio can be resumed.
 * Low priority client is played first.
 *
 * @class CTestMmfAclntARN8105
 * 
 */
class CTestMmfAclntARN8105 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN8105* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
	virtual TBool FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN8105( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();

private:
	TTimeIntervalMicroSeconds iPositionStop;
	TTimeIntervalMicroSeconds iPositionResume;
	};

/** 
 * Register (after Play Error) and Get the actual position from where the paused audio can be resumed.
 * High priority client is played first.
 *
 * @class CTestMmfAclntARN8106
 * 
 */
class CTestMmfAclntARN8106 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN8106* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
	virtual TBool FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN8106( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();

private:
	TTimeIntervalMicroSeconds iPositionStop;
	TTimeIntervalMicroSeconds iPositionResume;
	};

/** 
 * Client does not need to Re-register for the same event when the event is received 
 * i.e API is not one shot.
 *
 * @class CTestMmfAclntARN8107
 * 
 */
class CTestMmfAclntARN8107 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN8107* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
	virtual TBool FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN8107( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * High priority client plays with repeats and low priority client doesn't get resources
 *
 * @class CTestMmfAclntARN8109
 * 
 */
class CTestMmfAclntARN8109 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN8109* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
	virtual TBool FsmCheck(TMmfAudioPlayerEvents aEventCode, TInt aError);
	
		// from MTestAudioPlayerCallback
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport);
	virtual void MapcPlayCompleteTest(TInt aError, CTestMdaAudioPlayerCallbackSupport& aMdaAudioPlayerCallbackSupport);

protected:
	CTestMmfAclntARN8109( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation);
private:
	TTime iStart;
	TTime iStop;
	TInt  iDuration;
	TUint iActualDuration;
	TBool iPlayComplete2;
	TBool iResourceAvailable1;
	};



/** 
 * Call WillResumePlay()
 *
 * @class CTestMmfAclntARN8108
 * 
 */
class CTestMmfAclntARN8108 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN8108* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
	virtual void DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError=KErrNone);
protected:
	TVerdict HandleStateOpenPlayer1();
	TVerdict HandleStateOpenPlayer3();
	TVerdict HandleStateOpenPlayer2();
	TVerdict HandleStateErrInUsePlayer1();
	TVerdict HandleStateErrInUsePlayer3();
	TVerdict HandleStateNotifiedPlayer1();
	TVerdict HandleStateNotifiedPlayer3();
	TVerdict HandleStatePlaying1();
	CTestMmfAclntARN8108( const TDesC& aTestName, const TDesC& aSectName );
	void InitTest();
	void StartTestL();
	TVerdict PerformTestL();
	typedef TVerdict (CTestMmfAclntARN8108::*fp)();
	TFixedArray<fp,30> iStateHandlers;
	};

/** 
 * Dont register for ARN, get pre-empted and then resume from same position.
 * Test case for DevCR KEVN-7T5EHA 
 * @class CTestMmfAclntARN8110
 * 
 */
class CTestMmfAclntARN8110 : public CTestMmfAclntAudioPlayerARN
    {
public:
    static CTestMmfAclntARN8110* NewL( const TDesC& aTestName, const TDesC& aSectName );
    virtual TVerdict DoTestL();
    virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
    
protected:
    CTestMmfAclntARN8110( const TDesC& aTestName, const TDesC& aSectName );
    TVerdict PerformTestL();
    TTimeIntervalMicroSeconds iPlayer1Pos;
    };

/** 
 * Dont register for ARN, get pre-empted, stop and chk if position is reset, start play again.
 * Test case for DevCR KEVN-7T5EHA 
 * @class CTestMmfAclntARN8111
 * 
 */
class CTestMmfAclntARN8111 : public CTestMmfAclntAudioPlayerARN
    {
public:
    static CTestMmfAclntARN8111* NewL( const TDesC& aTestName, const TDesC& aSectName );
    virtual TVerdict DoTestL();
    virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
    
protected:
    CTestMmfAclntARN8111( const TDesC& aTestName, const TDesC& aSectName );
    TVerdict PerformTestL();
    TTimeIntervalMicroSeconds iPlayer1Pos;
    };

/** 
 * Play higher priority first, then start playing low priority
 * Test case for DevCR KEVN-7T5EHA 
 * @class CTestMmfAclntARN8112
 * 
 */
class CTestMmfAclntARN8112 : public CTestMmfAclntAudioPlayerARN
    {
public:
    static CTestMmfAclntARN8112* NewL( const TDesC& aTestName, const TDesC& aSectName );
    virtual TVerdict DoTestL();
    virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);
    
protected:
    CTestMmfAclntARN8112( const TDesC& aTestName, const TDesC& aSectName );
    TVerdict PerformTestL();
    TTimeIntervalMicroSeconds iPlayer1Pos;
    };
//
//NEGATIVE TESTS//
//

/** 
 * Register for notification with an unsupported event type.
 *
 * @class CTestMmfAclntARN1301
 * 
 */
class CTestMmfAclntARN1301 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN1301* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN1301( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Cancel the request for notification with an unsupported event type.
 *
 * @class CTestMmfAclntARN1302
 * 
 */
class CTestMmfAclntARN1302 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN1302* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN1302( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Cancel when no resource Notification is requested.
 *
 * @class CTestMmfAclntARN1303
 * 
 */
class CTestMmfAclntARN1303 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN1303* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN1303( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Multiple Registration for notification for the same event type.
 *
 * @class CTestMmfAclntARN1304
 * 
 */
class CTestMmfAclntARN1304 : public CTestMmfAclntAudioPlayerARN
	{
public:
	static CTestMmfAclntARN1304* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioPlayerEvents aEventCode, TInt aError);

protected:
	CTestMmfAclntARN1304( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

//------------------------------------------------------------------------------
//*****************PREQ 797 - Audio Recorder Test Declarations******************
//------------------------------------------------------------------------------

//
//ENUM DECLARATIONS//
//

// Enum to decide which Event has occured.
enum TMmfAudioRecorderEvents
	{
	EIdleRecorder = 0,
	EMoscoStateChange1,
	EMoscoStateChange2,
	EMoscoStateChange3,
	EMarncResourceAvailableRec1,
	EMarncResourceAvailableRec2,
	EMarncResourceAvailableRec3
	};

//
//BASE CLASS DECLARATIONS//
//

/**
 * Observer class to identify and pass events to the test step class
 * 
 * @class MTestAudioRecorderCallback
 *
 */
class MTestAudioRecorderCallback
	{
public :
	virtual void MoscoStateChangeEventTest(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode, CTestMdaAudioRecorderCallbackSupport& aMdaAudioRecorderCallbackSupport) = 0;
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioRecorderCallbackSupport& aMdaAudioRecorderCallbackSupport) = 0;
	};

/**
 * Support class to pass the events to the test step class using the MTestAudioRecorderCallback
 * 
 * @class CTestMdaAudioRecorderCallbackSupport
 *
 */
class CTestMdaAudioRecorderCallbackSupport : public CBase, public MMdaObjectStateChangeObserver, public MMMFAudioResourceNotificationCallback
	{
public :
	static CTestMdaAudioRecorderCallbackSupport* NewL(MTestAudioRecorderCallback& aCallback);
	virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);
	virtual void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
protected:
	CTestMdaAudioRecorderCallbackSupport(MTestAudioRecorderCallback& aCallback);
	void ConstructL();
private :
	MTestAudioRecorderCallback& iCallback;
	};

/**
 * Base Class for ARN functionality for most ARN audio recorder tests
 * 
 * @class CTestMmfAclntRecordARN
 *
 */
class CTestMmfAclntRecordARN : public CTestMmfAclntStep, public MTestAudioRecorderCallback
	{
public:
	// Constructor
	CTestMmfAclntRecordARN(const TDesC& aTestName, const TDesC& aSectName);
	// Virtual methods for Preamble and Postamble.
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// Virtual function to access test specific methods
	virtual TVerdict DoTestStepL();
	// Pure function to access test specific methods
	virtual TVerdict DoTestL()=0;
	// from MTestAudioRecorderCallback
	void MoscoStateChangeEventTest(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode, CTestMdaAudioRecorderCallbackSupport& aMdaAudioRecorderCallbackSupport);
	void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioRecorderCallbackSupport& aMdaAudioRecorderCallbackSupport);
	// Centralised State Handler
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone)=0;
	virtual TBool FsmCheck(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);
protected:
	// StateInit Function
	void StateInit();
	// StateChange Function
	void StateChange(TMmfAudioRecorderEvents aExpectedEvent, CMdaAudioClipUtility::TState aExpectedRecorderState, TInt aExpectedError=KErrNone);
protected :	
	// CMdaAudioRecorderUtility instances
	CMdaAudioRecorderUtility* iTestAudioRecorder1;
	CMdaAudioRecorderUtility* iTestAudioRecorder2;
	CMdaAudioRecorderUtility* iTestAudioRecorder3;
	// CTestMdaAudioRecorderCallbackSupport instances
	CTestMdaAudioRecorderCallbackSupport* iMdaAudioRecorderCallbackSupport1;
	CTestMdaAudioRecorderCallbackSupport* iMdaAudioRecorderCallbackSupport2;
	CTestMdaAudioRecorderCallbackSupport* iMdaAudioRecorderCallbackSupport3;
	// Other class variable declarations
	TBuf<KSizeBuf>	iFilename1;
	TBuf<KSizeBuf>	iFilename2;
	TBuf<KSizeBuf>	iFilename3;
	TInt iError;
	TInt iState;
	TRequestStatus iRequestStatus;
	CCallBackTimer* iCallBackTimer;
	TMmfAudioRecorderEvents iExpectedEvent;
	CMdaAudioClipUtility::TState iExpectedRecorderState;
	TInt iExpectedError;
	TTimeIntervalMicroSeconds iDuration;
	TTimeIntervalMicroSeconds iPosition;
	TMmfAudioRecorderEvents iEventOccured;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	};

//
//POSITIVE TESTS//
//

/** 
 * Register for notification, but make no request to play audio.
 *
 * @class CTestMmfAclntARN8011
 * 
 */
class CTestMmfAclntARN8011 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN8011* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);
	
protected:
	CTestMmfAclntARN8011( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};


/** 
 * Register (b4 play, after Open) and Receive Notification of a Resource Available Event.
 * (when audio resource is currently unavailable and then available)	
 *
 * @class CTestMmfAclntARN8012
 * 
 */
class CTestMmfAclntARN8012 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN8012* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8012( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Register (at the time of Play) and Receive Notification of a Resource Available Event.
 * (when audio resource is currently available and subsequently unavailable, and then available)
 *
 * @class CTestMmfAclntARN8013
 * 
 */
class CTestMmfAclntARN8013 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN8013* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8013( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Registration (b4 File Open) and Cancellation of  the notification request
 *
 * @class CTestMmfAclntARN8014
 * 
 */
class CTestMmfAclntARN8014 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN8014* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8014( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Get the actual position from where the paused audio can be resumed.
 *
 * @class CTestMmfAclntARN8015
 * 
 */
class CTestMmfAclntARN8015 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN8015* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);
	virtual TBool FsmCheck(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8015( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();

private:
	TTimeIntervalMicroSeconds iPositionStop;
	TTimeIntervalMicroSeconds iPositionResume;
	};

/** 
 * Call WillResumePlay()
 *
 * @class CTestMmfAclntARN8016
 * 
 */
class CTestMmfAclntARN8016 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN8016* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);
	virtual TBool FsmCheck(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8016( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();

private:
	TTimeIntervalMicroSeconds iPositionStop;
	TTimeIntervalMicroSeconds iPositionResume;
	};


//
//NEGATIVE TESTS//
//

/** 
 * Register for notification with an unsupported event type.
 *
 * @class CTestMmfAclntARN1311
 * 
 */
class CTestMmfAclntARN1311 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN1311* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN1311( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Cancel the request for notification with an unsupported event type.
 *
 * @class CTestMmfAclntARN1312
 * 
 */
class CTestMmfAclntARN1312 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN1312* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN1312( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Cancel when no resource Notification is requested.
 *
 * @class CTestMmfAclntARN1313
 * 
 */
class CTestMmfAclntARN1313 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN1313* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN1313( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Multiple Registration for notification for the same event type.
 *
 * @class CTestMmfAclntARN1314
 * 
 */
class CTestMmfAclntARN1314 : public CTestMmfAclntRecordARN
	{
public:
	static CTestMmfAclntARN1314* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN1314( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};


//------------------------------------------------------------------------------
//***************PREQ 797 - Audio OutputStream Test Declarations****************
//------------------------------------------------------------------------------

//
//ENUM DECLARATIONS//
//

_LIT(KStreamRawFile1,"c:\\AclntITestData\\test.raw");
_LIT(KStreamRawFile2,"c:\\AclntITestData\\test2.raw");
_LIT(KStreamRawFile3,"c:\\AclntITestData\\test3.raw");

// Enum to decide which Event has occured.
enum TMmfAudioOutputStreamEvents
	{
	EIdleOutputStream = 0,
	EMaoscOpenComplete1,
	EMaoscOpenComplete2,
	EMaoscOpenComplete3,
	EMaoscBufferCopied1,
	EMaoscBufferCopied2,
	EMaoscBufferCopied3,
	EMaoscPlayComplete1,
	EMaoscPlayComplete2,
	EMaoscPlayComplete3,
	EMarncResourceAvailableOS1,
	EMarncResourceAvailableOS2,
	EMarncResourceAvailableOS3,
	};
	
enum TFsmOutputStreamState
	{
	EStateIdle = 0,
	EStateOpening1,
	EStateOpening2,
	EStateOpening3,
	EStateOpenComplete1,
	EStateOpenComplete2,
	EStateOpenComplete3,
	EStateStreaming1,
	EStateStreaming2,
	EStateStreaming3,
	EStateResumeStreaming1,
	EStateResumeStreaming2,
	EStateResumeStreaming3,
	EStateBufferCopied1,
	EStateBufferCopied2,
	EStateBufferCopied3,
	EStateStreamingComplete1,
	EStateStreamingComplete1ErrInUse,
	EStateStreamingComplete1ErrUnderflow,
	EStateStreamingComplete2,
	EStateStreamingComplete2ErrInUse,
	EStateStreamingComplete2ErrUnderflow,
	EStateStreamingComplete3,
	EStateStreamingComplete3ErrInUse,
	EStateStreamingComplete3ErrUnderflow,
	EStateResourceAvailableOS1,
	EStateResourceAvailableOS2,
	EStateResourceAvailableOS3,
	EStateMonitoringEvents,
	EStateFinished
	};

//
//BASE CLASS DECLARATIONS//
//

/**
 * Observer class to identify and pass events to the test step class
 * 
 * @class MTestAudioOutputStreamCallback
 *
 */
class MTestAudioOutputStreamCallback
	{
public :
	virtual void MaoscOpenCompleteTest(TInt aError, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport) = 0;
	virtual void MaoscBufferCopiedTest(TInt aError, const TDesC8& aBuffer, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport) = 0;
	virtual void MaoscPlayCompleteTest(TInt aError, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport) = 0; 
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport) = 0;
	};

/**
 * Support class to pass the events to the test step class using the MTestAudioOutputStreamCallback
 * 
 * @class CTestMdaAudioOutputStreamCallbackSupport
 *
 */
class CTestMdaAudioOutputStreamCallbackSupport : public CBase, public MMdaAudioOutputStreamCallback, public MMMFAudioResourceNotificationCallback
	{
public :
	static CTestMdaAudioOutputStreamCallbackSupport* NewL(MTestAudioOutputStreamCallback& aCallback);
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError); 
	virtual void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
protected:
	CTestMdaAudioOutputStreamCallbackSupport(MTestAudioOutputStreamCallback& aCallback);
	void ConstructL();
private :
	MTestAudioOutputStreamCallback& iCallback;
	};


/**
 * Base Class for ARN functionality for most ARN Audio Output Stream tests
 * 
 * @class CTestStepAudioOutputStreamARN
 *
 */
class CTestStepAudioOutputStreamARN : public CTestMmfAclntStep, public MTestAudioOutputStreamCallback
  {													           
public:
	// Virtual methods for Preamble and Postamble.
	virtual enum TVerdict DoTestStepPreambleL();
	virtual enum TVerdict DoTestStepPostambleL();
	// Virtual function to access test specific methods
	virtual TVerdict DoTestStepL();
	// Pure function to access test specific methods
	virtual TVerdict DoTestL()=0;
	// from MTestAudioOutputStreamCallback
	virtual void MaoscOpenCompleteTest(TInt aError, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport);
	virtual void MaoscBufferCopiedTest(TInt aError, const TDesC8& aBuffer, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport);
	virtual void MaoscPlayCompleteTest(TInt aError, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport);
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport);
	// Centralised State Handler
	virtual void FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone)=0;
	virtual TBool FsmCheck(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone);
protected:
	// Constructor
	CTestStepAudioOutputStreamARN(const TDesC& aTestName);
	// StateInit Function
	void StateInit();
	// StateChange Function
	void StateChange(TMmfAudioOutputStreamEvents aExpectedEvent, TInt aExpectedError=KErrNone);
	// CMdaAudioOutputStream instances
	CMdaAudioOutputStream*		iTestAudioOutputStream1;
	CMdaAudioOutputStream*		iTestAudioOutputStream2;
	CMdaAudioOutputStream*		iTestAudioOutputStream3;
	// CTestMdaAudioOutputStreamCallbackSupport instances
	CTestMdaAudioOutputStreamCallbackSupport* iMdaAudioOutputStreamCallbackSupport1;
	CTestMdaAudioOutputStreamCallbackSupport* iMdaAudioOutputStreamCallbackSupport2;
	CTestMdaAudioOutputStreamCallbackSupport* iMdaAudioOutputStreamCallbackSupport3;
	// Other class variable declarations
	RFs							iFs;
	RFile						iFile1;
	RFile						iFile2;
	RFile						iFile3;
	TInt						iFileSize1;
	TInt						iFileSize2;
	TInt						iFileSize3;
	RBuf8						iBuf1;
	RBuf8						iBuf2;
	RBuf8						iBuf3;
	TInt 						iError;
	TTimeIntervalMicroSeconds	iPosition;
	TInt 						iState;
	TRequestStatus 				iRequestStatus;
	CCallBackTimer* 			iCallBackTimer;
	TMmfAudioOutputStreamEvents iExpectedEvent;
	TInt 						iExpectedError;
	TMmfAudioOutputStreamEvents iEventOccured;
	};


class CFsmAudioOutputStreamClient;
/**
 * Base Class for ARN functionality for most ARN Audio Output Stream tests
 * 
 * @class CTestStepAudioOutputStreamARN2
 *
 */
class CTestStepAudioOutputStreamARN2 : public CTestMmfAclntStep
  {													           
public:
	enum TFsmAosState
	{
	EStateIdle = 0,
	EStateOpenComplete1,
	EStateOpenComplete2,
	EStateOpenComplete3,
	EStateStreaming1,
	EStateStreaming2,
	EStateStreaming3,
	EStateResumeStreaming1,
	EStateResumeStreaming2,
	EStateResumeStreaming3,
	EStateBufferCopied1,
	EStateBufferCopied2,
	EStateBufferCopied3,
	EStateStreamingComplete1,
	EStateStreamingComplete1ErrInUse,
	EStateStreamingComplete1ErrUnderflow,
	EStateStreamingComplete2,
	EStateStreamingComplete2ErrInUse,
	EStateStreamingComplete2ErrUnderflow,
	EStateStreamingComplete3,
	EStateStreamingComplete3ErrInUse,
	EStateStreamingComplete3ErrUnderflow,
	EStateResourceAvailableOS1,
	EStateResourceAvailableOS2,
	EStateResourceAvailableOS3,
	EStateMonitoringEvents,
	EStateFinished,
	};
public:
	virtual enum TVerdict DoTestStepPreambleL();
	virtual enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL()=0;
	virtual void Fsm(TInt aClientNumber,TInt aEventCode, TInt aError=KErrNone)=0;
	virtual TBool FsmCheck(TInt aClientNumber, TInt aEventCode, TInt aError);
	void StopTest();
protected:
	CTestStepAudioOutputStreamARN2(const TDesC& aTestName);	
	void StateChange(TFsmAosState aState);
	void SetExpectedEvent(TInt aClientNumber, TInt aEvent,TInt aError);	
	//Register client events to monitor
	void RegisterClientEventToMonitor(TInt aClientnumber, TInt aEventCode, TInt aError, TFsmAosState aFsmState);
	TBool IsMonitoringFinished();
protected:
	TInt iExpectedEvent;
	TInt iExpectedError;
	TFsmAosState iState;
	TFixedArray <CFsmAudioOutputStreamClient*, 4> iClientList;
	TFixedArray <TInt, 4> iClientEventsToMonitor;
	TFixedArray <TInt, 4> iClientError;
	TFixedArray <TFsmAosState, 4> iFsmStateforClient;
	TInt iClientsRegistered;
	RFs	 iFs;
	TFixedArray<RFile,4> iFileHandle;
	TFixedArray<TInt, 4> iFileSize;
	TFixedArray<RBuf8, 4> iBuf; 
	TInt 						iError;
	TTimeIntervalMicroSeconds	iPosition;
	TRequestStatus 				iRequestStatus;
	CCallBackTimer* 			iCallBackTimer;
	};

class CFsmAudioOutputStreamClient : public CBase,public MMdaAudioOutputStreamCallback, public MMMFAudioResourceNotificationCallback
	{
public:
	// Enum to decide which Event has occured.
	enum TMmfAosStreamEvents
		{
		EIdleOutputStream = 0,
		EAudioOuputStreamOpenComplete,
		EAudioOutputStreamBufferCopied,
		EAudioOutputStreamPlayComplete,
		EAudioOutputStreamResourceAvailable,
		};
public:
	CFsmAudioOutputStreamClient(TInt aClientNumber,TInt aPriority,TMdaPriorityPreference aPref,CTestStepAudioOutputStreamARN2* aParent );
	~CFsmAudioOutputStreamClient();
	static CFsmAudioOutputStreamClient* NewL(TInt aClientNumber, TInt aPriority,TMdaPriorityPreference aPref, CTestStepAudioOutputStreamARN2* aParent);
	//Callback handlers
	void MaoscOpenComplete(TInt aError);
	void MaoscPlayComplete(TInt aError);
	void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
	
	void TestOpen();
	TInt TestRegisterAsClient();
	TInt TestWrite(const TDesC8 &aData);
	const TTimeIntervalMicroSeconds& GetCurrentPosition();
	TTimeIntervalMicroSeconds& GetResumedPosition();
	TInt BytesPlayed();
private:
	void ConstructL();
private:
	TMmfAosStreamEvents iEvent;
	TInt iError;
	TInt iClientNumber;
	TInt iPriority;
	TMdaPriorityPreference iPref;
	CMdaAudioOutputStream* iMmfAudioOutputStream;
	CTestStepAudioOutputStreamARN2* iParent;
	TTimeIntervalMicroSeconds	iResumedPosition;
	};
//
//POSITIVE TESTS//
//

/** 
 * Register for notification, but make no request to play audio.
 *
 * @class CTestMmfAclntARN8021
 * 
 */
class CTestMmfAclntARN8021 : public CTestStepAudioOutputStreamARN
	{
public:
	static CTestMmfAclntARN8021* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone);
	
protected:
	CTestMmfAclntARN8021( const TDesC& aTestName );
	TVerdict PerformTestL();
	};


/** 
 * Register (b4 play, after Open) and Receive Notification of a Resource Available Event.
 * (when audio resource is currently unavailable and then available)
 *
 * @class CTestMmfAclntARN8022
 * 
 */
class CTestMmfAclntARN8022 : public CTestStepAudioOutputStreamARN2
	{
public:
	static CTestMmfAclntARN8022* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	void Fsm(TInt aClientNumber,TInt aEventCode, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8022( const TDesC& aTestName );
	TVerdict PerformTestL();
private:
	void InitTest();
	void StartTestL();
	TVerdict HandleStateStreaming2();
	TVerdict HandleStateStreamingComplete1ErrInUse();
	TVerdict HandleStateResourceAvailableOS1();
	TVerdict HandleStateStreamingComplete2ErrUnderflow();
	TVerdict HandleStateOpenComplete2();
	TVerdict HandleStateOpenComplete1();
private:
	typedef TVerdict (CTestMmfAclntARN8022::*fp)();//Function pointer for a StateHandler
	TFixedArray<fp,30> iStateHandlers;
	};

/** 
 * Register (at the time of Play) and Receive Notification of a Resource Available Event.
 * (when audio resource is currently available and subsequently unavailable, and then available)
 *
 * @class CTestMmfAclntARN8023
 * 
 */
class CTestMmfAclntARN8023 : public CTestStepAudioOutputStreamARN2
	{
public:
	static CTestMmfAclntARN8023* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	void Fsm(TInt aClientNumber,TInt aEventCode, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8023( const TDesC& aTestName );
	TVerdict PerformTestL();
private:
	void InitTest();
	void StartTestL();
	TVerdict HandleStateStreaming1();
	TVerdict HandleStateStreaming2();
	TVerdict HandleStateStreamingComplete1ErrInUse();
	TVerdict HandleStateResourceAvailableOS1();
	TVerdict HandleStateStreamingComplete2ErrUnderflow();
	TVerdict HandleStateOpenComplete2();
	TVerdict HandleStateOpenComplete1();
private:
	typedef TVerdict (CTestMmfAclntARN8023::*fp)();
	TFixedArray<fp,30> iStateHandlers;
	};

/** 
 * Registration (b4 File Open) and Cancellation of  the notification request
 *
 * @class CTestMmfAclntARN8024
 * 
 */
class CTestMmfAclntARN8024 : public CTestStepAudioOutputStreamARN
	{
public:
	static CTestMmfAclntARN8024* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8024( const TDesC& aTestName );
	TVerdict PerformTestL();
	};

/** 
 * Get the actual position from where the paused audio can be resumed.
 *
 * @class CTestMmfAclntARN8025
 * 
 */
class CTestMmfAclntARN8025 : public CTestStepAudioOutputStreamARN2
	{
public:
	static CTestMmfAclntARN8025* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	void Fsm(TInt aClientNumber,TInt aEventCode, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8025( const TDesC& aTestName );
	TVerdict PerformTestL();
private:
	void InitTest();
	void StartTestL();
	TVerdict HandleStateStreaming1();
	TVerdict HandleStateStreaming2();
	TVerdict HandleStateStreamingComplete1ErrInUse();
	TVerdict HandleStateResourceAvailableOS1();
	TVerdict HandleStateStreamingComplete2ErrUnderflow();
	TVerdict HandleStateOpenComplete2();
	TVerdict HandleStateOpenComplete1();
private:
	typedef TVerdict (CTestMmfAclntARN8025::*fp)();
	TFixedArray<fp,30> iStateHandlers;
	TTimeIntervalMicroSeconds iPositionStop;
	TTimeIntervalMicroSeconds iPositionResume;
	};

/** 
 * Call WillResumePlay()
 *
 * @class CTestMmfAclntARN8026
 * 
 */
class CTestMmfAclntARN8026 : public CTestStepAudioOutputStreamARN
	{
public:
	static CTestMmfAclntARN8026* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone);
	virtual TBool FsmCheck(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN8026( const TDesC& aTestName );
	TVerdict PerformTestL();
	};


//
//NEGATIVE TESTS//
//

/** 
 * Register for notification with an unsupported event type.
 *
 * @class CTestMmfAclntARN1321
 * 
 */
class CTestMmfAclntARN1321 : public CTestStepAudioOutputStreamARN
	{
public:
	static CTestMmfAclntARN1321* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN1321( const TDesC& aTestName );
	TVerdict PerformTestL();
	};

/** 
 * Cancel the request for notification with an unsupported event type.
 *
 * @class CTestMmfAclntARN1322
 * 
 */
class CTestMmfAclntARN1322 : public CTestStepAudioOutputStreamARN
	{
public:
	static CTestMmfAclntARN1322* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN1322( const TDesC& aTestName );
	TVerdict PerformTestL();
	};

/** 
 * Cancel when no resource Notification is requested.
 *
 * @class CTestMmfAclntARN1323
 * 
 */
class CTestMmfAclntARN1323 : public CTestStepAudioOutputStreamARN
	{
public:
	static CTestMmfAclntARN1323* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN1323( const TDesC& aTestName );
	TVerdict PerformTestL();
	};

/** 
 * Multiple Registration for notification for the same event type.
 *
 * @class CTestMmfAclntARN1324
 * 
 */
class CTestMmfAclntARN1324 : public CTestStepAudioOutputStreamARN
	{
public:
	static CTestMmfAclntARN1324* NewL( const TDesC& aTestName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError=KErrNone);

protected:
	CTestMmfAclntARN1324( const TDesC& aTestName );
	TVerdict PerformTestL();
	};


//------------------------------------------------------------------------------
//**************PREQ 797 - Multiple Audio Client Test Declarations**************
//------------------------------------------------------------------------------
	

//
//BASE CLASS DECLARATIONS//
//

/**
 * Observer class to identify and pass events to the test step class
 * 
 * @class MTestAudioResourceNotificationCallback
 *
 */
class MTestAudioResourceNotificationCallback
	{
public :
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioMultiClientCallbackSupport& aMdaAudioMultiClientCallbackSupport) = 0;
	};

/**
 * Support class to pass the events to the test step class using the MTestAudioResourceNotificationCallback
 * 
 * @class CTestMdaAudioMultiClientCallbackSupport
 *
 */
class CTestMdaAudioMultiClientCallbackSupport : public CBase, public MMMFAudioResourceNotificationCallback
	{
public :
	static CTestMdaAudioMultiClientCallbackSupport* NewL(MTestAudioResourceNotificationCallback& aCallback);
	virtual void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
protected:
	CTestMdaAudioMultiClientCallbackSupport(MTestAudioResourceNotificationCallback& aCallback);
	void ConstructL();
private :
	MTestAudioResourceNotificationCallback& iCallback;
	};

/**
 * Base Class for ARN functionality for most ARN multiple client tests
 * 
 * @class CTestMmfAclntMultiClientARN
 *
 */
class CTestMmfAclntMultiClientARN : public CTestMmfAclntStep, public MMdaAudioPlayerCallback, public MMdaObjectStateChangeObserver, public MMdaAudioOutputStreamCallback, public MTestAudioResourceNotificationCallback
	{
public:
// Enum to decide which Event has occured.
enum TMmfAudioMultiClientEvents
	{
	EIdleMulti = 0,
	EMapcInitComplete,
	EMapcPlayComplete,
	EMoscoStateChangeEvent,
	EMaoscOpenComplete,
	EMaoscBufferCopied,
	EMaoscPlayComplete,
	EMarncResourceAvailablePlayer,
	EMarncResourceAvailableRecorder,
	EMarncResourceAvailableOS,
	EImproperEvent
	};

// Enum to decide on state
enum TMmfAudioMultiClientStates
	{
	EStateApInitComplete,
	EStateApPlayComplete,
	EStateApPlayCompleteKErrInUse,
	EStateApResourceAvailable,
	EStateRecoderOpen,
	EStateRecoderPlaying,
	EStateRecorderResumePlay,
	EStateRecorderPlayComplete,
	EStateRecorderrelease,
	EStateAosOpenComplete,
	EStateAosBufferCopiedKErrInUse,
	EStateAosPlayCompleteKErrInUse,
	EStateAosResourceAvailable
	};
public:
	// Constructor
	CTestMmfAclntMultiClientARN(const TDesC& aTestName, const TDesC& aSectName);
	// Virtual methods for Preamble and Postamble.
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// Virtual function to access test specific methods
	virtual TVerdict DoTestStepL();
	// Pure function to access test specific methods
	virtual TVerdict DoTestL()=0;
	// from MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);
	// from MTestAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);
	// from MTestAudioResourceNotificationCallback
	virtual void MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioMultiClientCallbackSupport& aMdaAudioMultiClientCallbackSupport);
	// Centralised State Handler
	virtual void FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady)=0;
	virtual TBool FsmCheck(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
protected:
	// StateInit Function
	void StateInit();
	// StateChange Function
	void StateChange(TMmfAudioMultiClientEvents aExpectedEvent, TInt aExpectedError=KErrNone, CMdaAudioClipUtility::TState aExpectedRecorderState=CMdaAudioClipUtility::ENotReady);
	// Audio Client Utilities declaration
	CMdaAudioPlayerUtility* iTestAudioPlayer;
	CMdaAudioRecorderUtility* iTestAudioRecorder;
	CMdaAudioOutputStream* iTestAudioOutputStream;
	// CTestMdaAudioMultiClientCallbackSupport instances
	CTestMdaAudioMultiClientCallbackSupport* iMdaAudioPlayerCallbackSupport;
	CTestMdaAudioMultiClientCallbackSupport* iMdaAudioRecorderCallbackSupport;
	CTestMdaAudioMultiClientCallbackSupport* iMdaAudioOutputStreamCallbackSupport;
	// Variables to hold the file names
	TBuf<KSizeBuf>				iFilenamePlayer;
	TBuf<KSizeBuf>				iFilenameRecorder;
	TBuf<KSizeBuf>				iFilenameOutputStream;
	// Common Class Variables
	TInt 						iError;
	TInt 						iState;
	TRequestStatus 				iRequestStatus;
	CCallBackTimer* 			iCallBackTimer;
	// for checking the expected states
	TMmfAudioMultiClientEvents 		iExpectedEvent;
	CMdaAudioClipUtility::TState 	iExpectedRecorderState;
	TInt 							iExpectedError;
	TMmfAudioMultiClientEvents      iEventOccured;
	// for Checking the duration
	TTimeIntervalMicroSeconds 	iDuration;
	TTimeIntervalMicroSeconds 	iPosition;
	// for iAudioOutputStream
	RFs							iFs;
	RFile						iFile;
	TInt						iFileSize;
	RBuf8						iBuf;
	TBuf<KNameBufSize> 			iSectName;		// Section name for retrieving filename
	CMultiEventChecker*			iEventChecker;
	};

//
//POSITIVE TESTS//
//

/** 
 * Receipt of Notification (on deleting the Audio Play Client instance of a Notified Client) 
 * of a Resource Available Event.
 *
 * @class CTestMmfAclntARN9001
 * 
 */
class CTestMmfAclntARN9001 : public CTestMmfAclntMultiClientARN
	{
public:
	static CTestMmfAclntARN9001* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	virtual TBool FsmCheck(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);

protected:
	CTestMmfAclntARN9001( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};


/** 
 * Receipt of Notification (on completion of play of a Notifed Client) of a Resource Available Event.
 *
 * @class CTestMmfAclntARN9002
 * 
 */
class CTestMmfAclntARN9002 : public CTestMmfAclntMultiClientARN
	{
public:
	static CTestMmfAclntARN9002* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	virtual void DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError=KErrNone,CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);

protected:
	CTestMmfAclntARN9002( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
private:
	void InitTest();
	void StartTestL();
	TVerdict HandleStateApInitComplete();
	TVerdict HandleStateRecorderOpen();
	TVerdict HandleStateRecorderPlaying();
	TVerdict HandleApPlayCompleteKErrInUse();
	TVerdict HandleStateAosOpenComplete();
	TVerdict HandleStateAosBufferCopiedKErrInUse();
	TVerdict HandleStateAosPlayCompleteKErrInUse();
	TVerdict HandleStateRecorderrelease();
	TVerdict HandleStateApResourceAvailable();
	TVerdict HandleStateApPlayComplete();
	TVerdict HandleStateAosResourceAvailable();
	typedef TVerdict (CTestMmfAclntARN9002::*fp)();//Function pointer for a StateHandler
	TFixedArray<fp,30> iStateHandlers;
	};

/** 
 * Receipt of Notification (on stopping play of a Notitifed Client) of a Resource Available Event.
 *
 * @class CTestMmfAclntARN9003
 * 
 */
class CTestMmfAclntARN9003 : public CTestMmfAclntMultiClientARN
	{
public:
	static CTestMmfAclntARN9003* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
protected:
	CTestMmfAclntARN9003( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Receipt of Notification (chronological order for equal priority clients) of a Resource Available Event.
 *
 * @class CTestMmfAclntARN9004
 * 
 */
class CTestMmfAclntARN9004 : public CTestMmfAclntMultiClientARN
	{
public:
	static CTestMmfAclntARN9004* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	virtual TBool FsmCheck(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);

protected:
	CTestMmfAclntARN9004( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Non Receipt of Notification (on calling WillResumePlay() of a notified client) 
 * of a Resource Available Event. 
 *
 * @class CTestMmfAclntARN9005
 * 
 */
class CTestMmfAclntARN9005 : public CTestMmfAclntMultiClientARN
	{
public:
	static CTestMmfAclntARN9005* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	virtual void DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError=KErrNone,CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
protected:	
	CTestMmfAclntARN9005( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
private:
	void InitTest();
	void StartTestL();
	TVerdict HandleStateApInitComplete();
	TVerdict HandleStateRecorderOpen();
	TVerdict HandleStateRecorderPlaying();
	TVerdict HandleApPlayCompleteKErrInUse();
	TVerdict HandleStateAosOpenComplete();
	TVerdict HandleStateAosBufferCopiedKErrInUse();
	TVerdict HandleStateAosPlayCompleteKErrInUse();
	TVerdict HandleStateRecorderrelease();
	TVerdict HandleStateApResourceAvailable();
	TVerdict HandleStateApPlayComplete();
	TVerdict HandleStateAosResourceAvailable();
	typedef TVerdict (CTestMmfAclntARN9005::*fp)();//Function pointer for a StateHandler
	TFixedArray<fp,30> iStateHandlers;
	};

/** 
 * Receipt of Notification (on timeout of a notified client, 1 sec assumed, call 
 * WillResumePlay() after 1 second) of a Resource Available Event.
 *
 * @class CTestMmfAclntARN9006
 * 
 */
class CTestMmfAclntARN9006 : public CTestMmfAclntMultiClientARN
	{
public:
	static CTestMmfAclntARN9006* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	virtual void DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError=KErrNone,CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
protected:
	CTestMmfAclntARN9006( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
private:
	void InitTest();
	void StartTestL();
	TVerdict HandleStateApInitComplete();
	TVerdict HandleStateRecorderOpen();
	TVerdict HandleStateRecorderPlaying();
	TVerdict HandleApPlayCompleteKErrInUse();
	TVerdict HandleStateAosOpenComplete();
	TVerdict HandleStateAosBufferCopiedKErrInUse();
	TVerdict HandleStateAosPlayCompleteKErrInUse();
	TVerdict HandleStateRecorderrelease();
	TVerdict HandleStateApResourceAvailable();
	TVerdict HandleStateAosResourceAvailable();
	typedef TVerdict (CTestMmfAclntARN9006::*fp)();//Function pointer for a StateHandler
	TFixedArray<fp,30> iStateHandlers;
	};

/** 
 * Receipt of Notification (on cancellation of the notification of a client that has 
 * requested for notification) of a Resource Available Event.  [Cancel before receipt of Notification]
 *
 * @class CTestMmfAclntARN9007
 * 
 */
class CTestMmfAclntARN9007 : public CTestMmfAclntMultiClientARN
	{
public:
	static CTestMmfAclntARN9007* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	virtual TBool FsmCheck(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);

protected:
	CTestMmfAclntARN9007( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
	};

/** 
 * Receipt of Notification (on cancellation of notification of a client that has just 
 * received notification) of a Resource Available Event. [Cancel after receipt of Notification]
 *
 * @class CTestMmfAclntARN9008
 * 
 */
class CTestMmfAclntARN9008 : public CTestMmfAclntMultiClientARN
	{
public:
	static CTestMmfAclntARN9008* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL();
	virtual void FsmL(TMmfAudioMultiClientEvents aEventCode, TInt aError=KErrNone, CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);
	virtual void DecideFsmState(TInt aClientNumber,CMultiEventChecker::TClientUtilityType aClientType, TInt aEventCode, TInt aError=KErrNone,CMdaAudioClipUtility::TState aRecorderState=CMdaAudioClipUtility::ENotReady);

protected:
	CTestMmfAclntARN9008( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL();
private:
	void InitTest();
	void StartTestL();
	TVerdict HandleStateApInitComplete();
	TVerdict HandleStateApPlayComplete();
	TVerdict HandleStateRecorderOpen();
	TVerdict HandleStateRecorderPlaying();
	TVerdict HandleStateRecorderResumePlay();
	TVerdict HandleApPlayCompleteKErrInUse();
	TVerdict HandleStateAosOpenComplete();
	TVerdict HandleStateAosBufferCopiedKErrInUse();
	TVerdict HandleStateAosPlayCompleteKErrInUse();
	TVerdict HandleStateRecorderrelease();
	TVerdict HandleStateApResourceAvailable();
	TVerdict HandleStateAosResourceAvailable();
	TVerdict HandleStateRecorderPlayComplete();
	typedef TVerdict (CTestMmfAclntARN9008::*fp)();//Function pointer for a StateHandler
	TFixedArray<fp,30> iStateHandlers;
	};

#endif
