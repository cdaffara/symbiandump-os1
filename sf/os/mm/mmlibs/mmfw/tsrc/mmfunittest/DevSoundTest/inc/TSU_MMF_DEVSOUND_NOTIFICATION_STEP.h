// TSU_MMF_DEVSOUND_NOTIFICATION_STEP.h

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
//

#ifndef __TSU_MMF_DEVSOUND_NOTIFICATION_STEP_H__
#define __TSU_MMF_DEVSOUND_NOTIFICATION_STEP_H__

#include <testframework.h>
#include <mmf/common/mmfstandardcustomcommands.h>

#include "TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.h"

class CTestSuite;
class CTestStepDevSoundNotification;
class CDevSoundClient1;
class CDevSoundClient2;
/**
 * TCallback provides possible call back flag
 * of MDevSoundObserver
 */
 enum TFsmDevSoundState
	{
	EStateIdle = 0,
	EStateInitComplete1,//Initialize
	EStateInitComplete2,//Initialize
	EStateInitComplete3,//Initialize
	EStateInitCompletePlaying1,//playinit
	EStateInitCompletePlaying2,//playinit
	EStateInitCompletePlaying3,//playinit
	EStatePlayError1ErrInUse,
	EStatePlayError2ErrInUse,
	EStatePlayError3ErrInUse,
	EStatePlayError1ErrUnderflow,
	EStatePlayError2ErrUnderflow,
	EStatePlayError3ErrUnderflow,
	EStateResourceAvailable1,
	EStateResourceAvailable2,
	EStateResourceAvailable3,
	EStateMonitoringEvents,
	EStatePlayResumed1,
	EStatePlayResumed2,
	EStatePlayResumed3
	};

typedef enum TCallback 
	{
	EInitComplete,
	EToneFinished,
	EBuffToFill,
	EPlayError,
	EBuffToEmpty,
	ERecError,
	EConvertMsg,
	EDeviceMsg,
	ESendMsg
	}CallbackMsg;
	
class CCallBackTimer : public CTimer
	{
public:
	static CCallBackTimer* NewL(TCallBack aCallBack, TPriority aPriority = EPriorityStandard);
private:
	virtual void RunL();
private:
	CCallBackTimer(TCallBack aCallBack, TPriority aPriority);
private:
	TCallBack iCallBack;
	};
	
/**
 *
 * CActiveListener provides the asynchronous operation
 * of an active object
 *
 * @lib "TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.lib"
 *
 *
 */
class CActiveListener : public CActive
	{
public:
	CActiveListener() : CActive(CActive::EPriorityStandard) 
		{ 
		CActiveScheduler::Add(this);
		}
	void InitialiseActiveListener()
		{
		if(!IsActive())
			{
			iStatus = KRequestPending; 
			SetActive();	
			}
		}
	// From CActive
	virtual void RunL()
		{
		CActiveScheduler::Stop();
		}
	virtual void DoCancel()
		{
		CActiveScheduler::Stop();
		};
	};

class CEventNotification : public CActive
	{
public:
	static CEventNotification* NewL();
	~CEventNotification();
	void Start();
	TRequestStatus* GetStatus();
	TVerdict GetResult();
	void TimerComplete();
private:
	CEventNotification();
	void ConstructL();
private://From CActive
	void RunL();
	void DoCancel();
private:
enum TEventStatus
	{
	EClient1Init,
	EClient2Init,
	EClient1Playing,
	EClient2Playing,
	EClient1Idle,
	EClient2Idle,
	EClient1Error,	
	EClient2Error,
	EClient2Delete,
	EClient1Status,
	EAddDelayClient2
	};
private:
	RFs iFileSession;
	RFile* iFile;
	TEventStatus iEventStatus;
	CDevSoundClient1* iDevSoundClient1; // client 1
	CDevSoundClient2* iDevSoundClient2; // client 2
	TVerdict iTestResult;
	TEventStatus iClientStatus;
	TRequestStatus iRequestStatus 	;
	CLog* iLog;
	CCallBackTimer* iAsyncCallBackTimer;
	};
	

/**
 *
 * CDevSoundClient1 client 1 of the sound device 
 *
 * @lib "TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.lib"
 *
 *
 */
class CDevSoundClient1 : public CBase,public MDevSoundObserver
	{
public:
	CDevSoundClient1(CTestStepDevSoundNotification* aParent);
	CDevSoundClient1(CEventNotification* aParent);
	~CDevSoundClient1();
	static CDevSoundClient1* NewL(CTestStepDevSoundNotification* aParent);
	static CDevSoundClient1* NewL(CEventNotification* aParent);
	static void CleanUp(TAny *aPtr);
	// Callback methods from MDevSoundObserver mixin
	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);
	void ConvertError (TInt aError);
	void DeviceMessage (TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	
	// Utility Methods
	void ResetCallbacks();
	TInt GetCallbackTotal();
	
	// Error status methods.
	TInt GetErrorStatus(TInt aCallBack);
	TVerdict InitialiseActiveListener();

	// Test methods and helper methods (called by test cases)
	TInt TestVolume();
	TInt TestMaxGain();
	TInt TestMaxVolume();
	void TestSetVolume(TInt aVol);	
	TVerdict TestPlayInit();
	void TestPlayData();
	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestRegisterAsClient(TUid aEventType);
	TVerdict TestCancelRegister(TUid aEventType);
	TVerdict TestInitialize(TFourCC& aDataType, TMMFState aMode);
	TVerdict TestInitialize(TUid aHwDeviceUid, TMMFState aMode);
	TVerdict TestDigitalPlayback();
	TVerdict TestDigitalPlayback(TFourCC& aDataType);
	void SetPrioritySettings(TMMFPrioritySettings aPrioritySettings);
	TVerdict SetFileName(TDesC& aFileName);
private:
	void ConstructL();
public:
	RFs iFileServer;
	RFile iFileHandle;
protected:
	TFixedArray<TInt, 9> iCallbackArray;
	CMMFDevSound* iMMFDevSound;
	TInt iExpectedValue;
	TInt iCallbackError;
	CMMFBuffer* iBuffer;
	CTestStepDevSoundNotification* iParent; // parent class 
	TBool iErrInUse;
	CEventNotification* iOtherParent;
	TBool  iSpecialCase;
	}; 
 
/**
 *
 * CDevSoundClient2 client 2 of the sound device 
 *
 * @lib "TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.lib"
 *
 *
 */
class CDevSoundClient2 : public CBase,public MDevSoundObserver
	{
public:
	CDevSoundClient2(CTestStepDevSoundNotification* aParent);
	CDevSoundClient2(CEventNotification* aParent);
	~CDevSoundClient2();
	static void CleanUp(TAny *aPtr);
	static CDevSoundClient2* NewL(CTestStepDevSoundNotification* aParent);
	static CDevSoundClient2* NewL(CEventNotification* aParent);
	// Callback methods from MDevSoundObserver mixin
	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);
	void ConvertError (TInt aError);
	void DeviceMessage (TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);

	// Utility Methods
	void ResetCallbacks();
	TInt GetCallbackTotal();
	void CloseClient();
	
	// Error status methods.
	TInt GetErrorStatus(TInt aCallBack);
	TVerdict TestStop();
	
	// Test methods and helper methods(called by test cases)
	TInt TestVolume();
	TInt TestMaxGain();
	TInt TestMaxVolume();
	TVerdict TestPlayInit();
	void TestPlayData();
	void TestSetVolume(TInt aVol);	
	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestRegisterAsClient(TUid aEventType);
	TVerdict TestCancelRegister(TUid aEventType);
	TVerdict TestInitialize(TFourCC& aDataType, TMMFState aMode);
	TVerdict TestInitialize(TUid aHwDeviceUid, TMMFState aMode);
	TVerdict TestDigitalPlayback();
	TVerdict TestDigitalPlayback(TFourCC& aDataType);
	TVerdict TestInitRegAndPlayBack(TFourCC& aDataType,TUid aEventType);
	void SetPrioritySettings(TMMFPrioritySettings aPrioritySettings);
	TVerdict SetFileName(TDesC& aFileName);
private:
	void ConstructL();
public:
	RFs iFileServer;
	RFile iFileHandle;
protected:
	TFixedArray<TInt, 9> iCallbackArray;
	TInt iCallbackError;
	CMMFDevSound* iMMFDevSound;
	TInt iExpectedValue;
	CMMFBuffer* iBuffer;
	CTestStepDevSoundNotification* iParent;
	CEventNotification* iOtherParent;
	TBool  iSpecialCase;
private:
};	

/**
 *
 * CDevSoundClient3 client 3 of the sound device 
 *
 * @lib "TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.lib"
 *
 *
 */
class CDevSoundClient3 : public CBase,public MDevSoundObserver
	{
public:
	CDevSoundClient3(CTestStepDevSoundNotification* aParent);
	~CDevSoundClient3();
	static void CleanUp(TAny *aPtr);
	static CDevSoundClient3* NewL(CTestStepDevSoundNotification* aParent);

	// Callback methods from MDevSoundObserver mixin
	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);
	void ConvertError (TInt aError);
	void DeviceMessage (TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);

	// Utility Methods
	void ResetCallbacks();
	TInt GetCallbackTotal();
	void CloseClient();
	
	// Error status methods.
	TInt GetErrorStatus(TInt aCallBack);
	TVerdict TestStop();
	
	// Test methods and helper methods(called by test cases)
	TInt TestVolume();
	TInt TestMaxGain();
	TInt TestMaxVolume();
	TVerdict TestPlayInit();
	void TestPlayData();
	void TestSetVolume(TInt aVol);	
	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestRegisterAsClient(TUid aEventType);
	TVerdict TestCancelRegister(TUid aEventType);
	TVerdict TestGetResourceNotification(TUid aEventType,TDes8& aNotificationData);
	TVerdict TestInitialize(TFourCC& aDataType, TMMFState aMode);
	TVerdict TestInitialize(TUid aHwDeviceUid, TMMFState aMode);
	TVerdict TestDigitalPlayback();
	TVerdict TestDigitalPlayback(TFourCC& aDataType);
	TVerdict TestInitRegAndPlayBack(TDesC& aFilename,TFourCC& aDataType,TUid aEventType);
	void SetPrioritySettings(TMMFPrioritySettings aPrioritySettings);
	TVerdict SetFileName(TDesC& aFileName);
	TVerdict TestWillResumePlay();
private:
	void ConstructL();
public:
	RFs iFileServer;
	RFile iFileHandle;
protected:
	TFixedArray<TInt, 9> iCallbackArray;
	TInt iCallbackError;
	CMMFDevSound* iMMFDevSound;
	TInt iExpectedValue;
	CMMFBuffer* iBuffer;
	CTestStepDevSoundNotification* iParent;
private:
};	

/**
 *
 * CTestStepDevSoundNotification - Test class that enables all clients tests.
 * It's a base class for all test steps.
 *
 * @lib "TSU_MMF_DEVSOUND_SUITE.lib"
 *
 *
 */
class CTestStepDevSoundNotification: public CTestStep
 	{
 public:
	void SetTestSuite(const CTestSuiteDevSoundNotification* aTestSuite);
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	CTestStepDevSoundNotification();
	~CTestStepDevSoundNotification();
	TRequestStatus* GetStatus();
	TVerdict InitialiseActiveListener();
	TVerdict Cancel();
public:
	CDevSoundClient1* iDevSoundClient1; // client 1
	CDevSoundClient2* iDevSoundClient2; // client 2
	CDevSoundClient3* iDevSoundClient3; // client 3
private:
	CActiveScheduler* iAS;
	CActiveListener* iAL;
	const CTestSuiteDevSoundNotification* iDevSoundNotificationTestSuite;
 	};

class CFsmDevSoundClient; //forward declaration

/**
 *
 * CTestStepFsmDevSoundNotification - Test class that enables all clients tests.
 * It's a base class for all test steps.
 *
 * @lib "TSU_MMF_DEVSOUND_SUITE.lib"
 *
 *
 */
class CTestStepFsmDevSoundNotification: public CTestStep
 	{
 public:
	void SetTestSuite(const CTestSuiteDevSoundNotification* aTestSuite);
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	CTestStepFsmDevSoundNotification();
	~CTestStepFsmDevSoundNotification();
	virtual void Fsm(TInt aClientNumber, TInt aEventCode, TInt aError);//Derived classes will implement this method
	TBool FsmCheck(TInt aClientNumber, TInt aEventCode, TInt aError);
	void StateChange(TFsmDevSoundState aFsmstate);
	void SetExpectedEvent(TInt aClientNumber, TInt aExpectedEvent, TInt aExpectedError=KErrNone);
	//Register client events to monitor
	void RegisterClientEventToMonitor(TInt aClientnumber, TInt aEventCode, TInt aError, TFsmDevSoundState aFsmState);
	TBool IsMonitoringFinished();
	void StopTest();
	CCallBackTimer* iCallBackTimer;
protected:
	TInt iExpectedEvent;
	TInt iExpectedError;
	TInt iState;
	TFixedArray<CFsmDevSoundClient*, 4> iClientList;
	
	//Event,error and corresponding state of Fsm for event monitoring
	TFixedArray<TInt, 4> iClientEventsToMonitor;
	TFixedArray<TInt, 4> iClientError;
	TFixedArray<TFsmDevSoundState, 4> iFsmStateforClient;
	//Number of clients registered for monitoring
	TInt			   iClientsRegistered;
private:
	CActiveScheduler* iAS;
	const CTestSuiteDevSoundNotification* iDevSoundNotificationTestSuite;
 	};

class CFsmDevSoundClient : public CBase,public MDevSoundObserver
 	{
public:
	enum TFsmDevSoundEvents
		{
		EIdleDevSoundStream=0,
		EInitComplete,
		EBufferToBeFilled,
		EPlayError,
		EResourceAvailable,
		EBufferToBeEmptied,
		ERecordError,
		ESendMsg
		};
public:
	CFsmDevSoundClient(TInt aClientNumber,CTestStepFsmDevSoundNotification* aParent);
	~CFsmDevSoundClient();
	static CFsmDevSoundClient* NewL(TInt aClientNumber,CTestStepFsmDevSoundNotification* aParent);
	
	// Callback methods from MDevSoundObserver mixin
	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);
	void ConvertError (TInt aError);
	void DeviceMessage (TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	void CloseClient();
	
	TInt TestInitialize(TMMFState aMode);
	TInt TestInitialize(TFourCC& aDataType, TMMFState aMode);
	TInt TestInitialize(TUid aHwDeviceUid, TMMFState aMode);
	TInt TestPlayInit();
	void TestPlayData(TInt aClient);
	void TestPlayStop();
	TInt TestWillResumePlay();
	TInt TestRegisterAsClient(TUid aEventType);
	TInt TestGetResourceNotification(TUid aEventType,TDes8& aNotificationData);
	TInt TestCancelRegister(TUid aEventType);
	void SetPrioritySettings(TMMFPrioritySettings aPrioritySettings);
	TInt SetFileName(TDesC& aFileName);
private:
	void ConstructL();
public:
	RFs iFileServer;
	RFile iFileHandle;
protected:
	TFsmDevSoundEvents iEvent;
	TInt iError;
	CMMFBuffer* iBuffer;
	CMMFDevSound* iMMFDevSound;
	CTestStepFsmDevSoundNotification* iParent;
	TInt iClientNumber;
	};


class CTestStepDevSound0040:public CTestStepDevSoundNotification
	{
public:
	CTestStepDevSound0040();
	TVerdict DoTestStepL();
	};

class CTestStepDevSound0041:public CTestStepDevSoundNotification
	{
public:
	CEventNotification* iEventHandler;
	CTestStepDevSound0041();
	TVerdict DoTestStepL();
	virtual void Close();
	};

class CTestStepDevSound0042:public CTestStepFsmDevSoundNotification
	{
public:
	CTestStepDevSound0042();
	TVerdict DoTestStepL();
	void Fsm(TInt aClientNumber, TInt aEvent, TInt aError);
private:
	void InitializeTest();
	void StartTestL();
	TVerdict HandleStateInitComplete1();
	TVerdict HandleStateInitComplete2();
	TVerdict HandleStateInitCompletePlaying1();
	TVerdict HandleStateInitCompletePlaying2();
	TVerdict HandleStatePlayError1ErrInUse();
	TVerdict HandleStatePlayError2ErrUnderflow();
	TVerdict HandleStateResourceAvailable1();
private:
	TMMFPrioritySettings iLowPriority;
	TMMFPrioritySettings iHighPriority;
	typedef TVerdict (CTestStepDevSound0042::*fp)();//Function pointer for a StateHandler
	TFixedArray<fp,20> iStateHandlers;
	};

class CTestStepDevSound0043:public CTestStepFsmDevSoundNotification
	{
public:
	CTestStepDevSound0043();
	TVerdict DoTestStepL();
	void Fsm(TInt aClientNumber, TInt aEvent, TInt aError);
private:
	void InitializeTest();
	void StartTestL();
	TVerdict HandleStateInitComplete1();
	TVerdict HandleStateInitComplete2();
	TVerdict HandleStateInitComplete3();
	TVerdict HandleStateInitCompletePlaying1();
	TVerdict HandleStateInitCompletePlaying2();
	TVerdict HandleStateInitCompletePlaying3();
	TVerdict HandleStatePlayError1ErrInUse();
	TVerdict HandleStatePlayError3ErrInUse();
	TVerdict HandleStatePlayError2ErrUnderflow();
	TVerdict HandleStateResourceAvailable1();
	TVerdict HandleStateResourceAvailable3();
private:
	TMMFPrioritySettings iLowPriority;
	TMMFPrioritySettings iHighPriority;
	TMMFPrioritySettings iMediumPriority;
	typedef TVerdict (CTestStepDevSound0043::*fp)();
	TFixedArray<fp,20> iStateHandlers;
	};

class CTestStepDevSound0044:public CTestStepFsmDevSoundNotification
	{
public:
	CTestStepDevSound0044();
	TVerdict DoTestStepL();
	void Fsm(TInt aClientNumber, TInt aEvent, TInt aError);
private:
	void InitializeTest();
	void StartTestL();
	TVerdict HandleStateInitComplete1();
	TVerdict HandleStateInitComplete2();
	TVerdict HandleStateInitComplete3();
	TVerdict HandleStateInitCompletePlaying1();
	TVerdict HandleStateInitCompletePlaying2();
	TVerdict HandleStateInitCompletePlaying3();
	TVerdict HandleStatePlayError2ErrUnderflow();
	TVerdict HandleStatePlayError1ErrInUse();
	TVerdict HandleStatePlayError3ErrInUse();
	TVerdict HandleStateResourceAvailable3();
	TVerdict HandleStateResourceAvailable1();
	
private:
	TMMFPrioritySettings iLowPriority;
	TMMFPrioritySettings iHighPriority;
	TMMFPrioritySettings iMediumPriority;
	typedef TVerdict (CTestStepDevSound0044::*fp)();
	TFixedArray<fp,20> iStateHandlers;
	};
	
class CTestStepDevSound0045:public CTestStepFsmDevSoundNotification
	{
public:
	CTestStepDevSound0045();
	TVerdict DoTestStepL();
	void Fsm(TInt aClientNumber, TInt aEvent, TInt aError);
private:
	void InitializeTest();
	void StartTestL();
	TVerdict HandleStateInitComplete1();
	TVerdict HandleStateInitComplete2();
	TVerdict HandleStateInitComplete3();
	TVerdict HandleStateInitCompletePlaying1();
	TVerdict HandleStatePlayError1ErrInUse();
	TVerdict HandleStateInitCompletePlaying3();
	TVerdict HandleStatePlayError3ErrInUse();
	TVerdict HandleStateInitCompletePlaying2();
	TVerdict HandleStatePlayError2ErrUnderflow();
	TVerdict HandleStateResourceAvailable3();
	TVerdict HandleStatePlayResumed3();
	TVerdict HandleStatePlayError3ErrUnderflow();
	TVerdict HandleStateResourceAvailable1();
private:
	TMMFPrioritySettings iLowPriority;
	TMMFPrioritySettings iHighPriority;
	TMMFPrioritySettings iMediumPriority;
	typedef TVerdict (CTestStepDevSound0045::*fp)();
	TFixedArray<fp,20> iStateHandlers;
	};
	
class CTestStepDevSound0046:public CTestStepDevSoundNotification
	{
public:
	CTestStepDevSound0046();
	TVerdict DoTestStepL();
	};
	
class CTestStepDevSound0047:public CTestStepDevSoundNotification
	{
public:
	CTestStepDevSound0047();
	TVerdict DoTestStepL();
	};
	
class CTestStepDevSound0048:public CTestStepFsmDevSoundNotification
	{
public:
	CTestStepDevSound0048();
	TVerdict DoTestStepL();
	void Fsm(TInt aClientNumber, TInt aEvent, TInt aError);
private:
	void InitializeTest();
	void StartTestL();
	TVerdict HandleStateInitComplete1();
	TVerdict HandleStateInitComplete2();
	TVerdict HandleStateInitCompletePlaying1();
	TVerdict HandleStatePlayError1ErrInUse();
	TVerdict HandleStateInitCompletePlaying2();
	TVerdict HandleStatePlayError2ErrUnderflow();
	TVerdict HandleStateResourceAvailable1();
private:
	TMMFPrioritySettings iLowPriority;
	TMMFPrioritySettings iHighPriority;
	typedef TVerdict (CTestStepDevSound0048::*fp)();
	TFixedArray<fp,20> iStateHandlers;
	};
	
class CTestStepDevSound0049:public CTestStepFsmDevSoundNotification
	{
public:
	CTestStepDevSound0049();
	TVerdict DoTestStepL();
	void Fsm(TInt aClientNumber, TInt aEvent, TInt aError);
private:
	void InitializeTest();
	void StartTestL();
	TVerdict HandleStateInitComplete1();
	TVerdict HandleStateInitComplete2();
	TVerdict HandleStateInitCompletePlaying1();
	TVerdict HandleStatePlayError1ErrInUse();
	TVerdict HandleStateInitCompletePlaying2();
	TVerdict HandleStatePlayError2ErrUnderflow();
	TVerdict HandleStateResourceAvailable1();
private:
	TMMFPrioritySettings iLowPriority;
	TMMFPrioritySettings iHighPriority;
	typedef TVerdict (CTestStepDevSound0049::*fp)();
	TFixedArray<fp,20> iStateHandlers;
	};
	
class CTestStepDevSound0050:public CTestStepFsmDevSoundNotification
	{
public:
	CTestStepDevSound0050();
	TVerdict DoTestStepL();
	void Fsm(TInt aClientNumber, TInt aEvent, TInt aError);
private:
	void InitializeTest();
	void StartTestL();
	TVerdict HandleStateInitComplete1();
	TVerdict HandleStateInitComplete2();
	TVerdict HandleStateInitComplete3();
	TVerdict HandleStateInitCompletePlaying1();
	TVerdict HandleStateInitCompletePlaying2();
	TVerdict HandleStateInitCompletePlaying3();
	TVerdict HandleStatePlayError1ErrInUse();
	TVerdict HandleStatePlayError3ErrInUse();
	TVerdict HandleStatePlayError2ErrUnderflow();
	TVerdict HandleStateResourceAvailable3();
	TVerdict HandleStateResourceAvailable1();
	
private:
	TMMFPrioritySettings iLowPriority;
	TMMFPrioritySettings iHighPriority;
	TMMFPrioritySettings iMediumPriority;
	typedef TVerdict (CTestStepDevSound0050::*fp)();
	TFixedArray<fp,20> iStateHandlers;
	};

class CTestStepDevSound0051:public CTestStepDevSoundNotification
	{
public:
	CTestStepDevSound0051();
	TVerdict DoTestStepL();
	};

#endif// __TSU_MMF_DEVSOUND_NOTIFICATION_STEP_H__
