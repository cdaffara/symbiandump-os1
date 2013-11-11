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

// System includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_DEVSOUND_NOTIFICATION_STEP.h"
#include "TSU_MMF_DEVSOUND_NOTIFICATION_SUITE.h"

#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>

/**
 * stopActiveScheduler
 */
static TInt stopActiveScheduler(TAny*)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}
	
/**
* Async callback timer
*/	
static TInt AsyncCallBackTimer(TAny *aPtr)
	{
	CEventNotification  *eventNotifier = static_cast<CEventNotification*>(aPtr);
	eventNotifier->TimerComplete();
	return KErrNone;	
	}
	

/**
 * Standard static NewL() taking a callback function
 */
CCallBackTimer* CCallBackTimer::NewL(TCallBack aCallBack, TPriority aPriority)
	{
	CCallBackTimer* self = new(ELeave) CCallBackTimer(aCallBack, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

/**
 * Private c'tor
 */
CCallBackTimer::CCallBackTimer(TCallBack aCallBack, TPriority aPriority)
: CTimer(aPriority),
iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

/*
 * Callback on timer complete
 */
void CCallBackTimer::RunL()
	{
	iCallBack.CallBack();
	}

/**
 *
 * SetTestSuite
 * @param aTestSuite
 *				
 */
void CTestStepDevSoundNotification::SetTestSuite(const CTestSuiteDevSoundNotification* aTestSuite)
	{
	iDevSoundNotificationTestSuite = aTestSuite;
	}
	
/**
 *
 * DoTestStepPreambleL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSoundNotification::DoTestStepPreambleL()
	{
	iDevSoundClient1 = CDevSoundClient1::NewL(this);
	iDevSoundClient2 = CDevSoundClient2::NewL(this);
	iDevSoundClient3 = CDevSoundClient3::NewL(this);
	//Install the Active Scheduler
	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);
	iAL= new(ELeave)CActiveListener;
	
	return EPass;
	}

/**
 *
 * DoTestStepPostambleL
 * @result TVerdict
 *
 */	
TVerdict CTestStepDevSoundNotification::DoTestStepPostambleL()
	{
	if(iDevSoundClient1)
		{
		delete iDevSoundClient1;	
		iDevSoundClient1 = NULL;
		}
	if(iDevSoundClient2)
		{
		delete iDevSoundClient2;	
		iDevSoundClient2 = NULL;
		}
	if(iDevSoundClient3)
		{
		delete iDevSoundClient3;	
		iDevSoundClient3 = NULL;
		}
	if (iAS)
		{
		delete iAS;
		iAS = NULL;
		}
	if (iAL)
		{
		delete iAL;
		iAL = NULL;
		}		
	return EPass;	
	}
	
/**
 *
 * CTestStepDevSoundNotification
 * Constructor
 *
 */	
CTestStepDevSoundNotification::CTestStepDevSoundNotification()
	{
	}

/**
 *
 * CTestStepDevSoundNotification
 * Destructor
 *
 */	
CTestStepDevSoundNotification::~CTestStepDevSoundNotification()
	{
	}
	
/**
 *
 * GetStatus - get the status of the 
 * @result TRequestStatus*
 *
 */	
TRequestStatus* CTestStepDevSoundNotification::GetStatus()
	{
	return &(iAL->iStatus); 
	}

/**
 *
 * InitialiseActiveListener
 * @result TVerdict
 *
 */	
TVerdict CTestStepDevSoundNotification::InitialiseActiveListener()	
	{
	iAL->InitialiseActiveListener();
	TInt err = iAL->iStatus.Int();
	if (err != KErrNone)
		{
		return EFail;	
		}
	return EPass;
	}

/**
 *
 * Cancel
 * @result TVerdict
 *
 */	
TVerdict CTestStepDevSoundNotification::Cancel()	
	{
	iAL->DoCancel();
	TInt err = iAL->iStatus.Int();
	if (err != KErrNone)
		{
		return EFail;	
		}
	return EPass;
	}

/**
 *
 * SetTestSuite
 * @param aTestSuite
 *				
 */
void CTestStepFsmDevSoundNotification::SetTestSuite(const CTestSuiteDevSoundNotification* aTestSuite)
	{
	iDevSoundNotificationTestSuite = aTestSuite;
	}
	
/**
 *
 * DoTestStepPreambleL
 * @result TVerdict
 *
 */
TVerdict CTestStepFsmDevSoundNotification::DoTestStepPreambleL()
	{
	for(TInt i=1; i<=3; i++)
		{
		iClientList[i] = CFsmDevSoundClient::NewL(i,this);
		}
	//Install the Active Scheduler
	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);	
	iCallBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	return EPass;
	}

/**
 *
 * DoTestStepPostambleL
 * @result TVerdict
 *
 */	
TVerdict CTestStepFsmDevSoundNotification::DoTestStepPostambleL()
	{
	iClientList.DeleteAll();
	if (iAS)
		{
		delete iAS;
		iAS = NULL;
		}	
	if(iCallBackTimer)
		{
		delete iCallBackTimer;
		iCallBackTimer = NULL;
		}
	return EPass;	
	}
	
/**
 *
 * CTestStepDevSoundNotification
 * Constructor
 *
 */	
CTestStepFsmDevSoundNotification::CTestStepFsmDevSoundNotification()
	{
	}

/**
 *
 * CTestStepDevSoundNotification
 * Destructor
 *
 */	
CTestStepFsmDevSoundNotification::~CTestStepFsmDevSoundNotification()
	{
	}
	
void CTestStepFsmDevSoundNotification::StateChange(TFsmDevSoundState aState)
	{
	iState = aState;
	}

void CTestStepFsmDevSoundNotification::StopTest()
	{
	CActiveScheduler::Stop();
	}
	
void CTestStepFsmDevSoundNotification::SetExpectedEvent(TInt aClientNumber, TInt aExpectedEvent, TInt aExpectedError)
	{
	iClientEventsToMonitor[aClientNumber]= aExpectedEvent;
	iClientError[aClientNumber] = aExpectedError;
	}
	
void CTestStepFsmDevSoundNotification::RegisterClientEventToMonitor(TInt aClientNumber, TInt aEventCode, TInt aError, TFsmDevSoundState aFsmState)
	{
	iClientsRegistered++;
	iClientEventsToMonitor[aClientNumber] = aEventCode;
	iClientError[aClientNumber] = aError;
	iFsmStateforClient[aClientNumber] = aFsmState;
	}

TBool CTestStepFsmDevSoundNotification::IsMonitoringFinished()
	{
	 if(iClientsRegistered == 0)
	 	{
	 	return ETrue;
	 	}
	 else
	 	{
	 	return EFalse;
	 	}
	}
	
/**
 * FsmCheck
 *
 */
TBool CTestStepFsmDevSoundNotification::FsmCheck(TInt aClientNumber, TInt aEventCode, TInt aError)
	{
	TBool retFsmCheck = ETrue;
	if (iState == EStateMonitoringEvents) /*special state to make decisions depending on clients events */
		{
		if(iClientEventsToMonitor[aClientNumber] == aEventCode && iClientError[aClientNumber] == aError )
			{
			StateChange(iFsmStateforClient[aClientNumber]);
			}
		else
			{
			retFsmCheck = EFalse;
			}
		if(retFsmCheck)
			{
			//Client event has finished
			iClientsRegistered--;
			}
		}
	else
		{
		if(iClientEventsToMonitor[aClientNumber] != aEventCode)
			{
			ERR_PRINTF4(_L("Client : %d, ExpectedEvent : %d, ActualEvent : %d"), aClientNumber, iExpectedEvent, aEventCode);
			retFsmCheck = EFalse;
			iTestStepResult = EFail;
			}
		else if(iClientError[aClientNumber] != aError)
			{
			ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, aError);
			retFsmCheck = EFalse;
			iTestStepResult = EFail;
			}
		}
	return retFsmCheck;
	}

/**
 * FsmL
 * Test steps should implement this
 */
 void CTestStepFsmDevSoundNotification::Fsm(TInt /*aClientNumber*/, TInt /*aEventCode*/, TInt /*aError*/)
 	{
 	
 	}
 	
 CFsmDevSoundClient::CFsmDevSoundClient(TInt aClientNumber,CTestStepFsmDevSoundNotification* aParent):iParent(aParent)
	{
	iClientNumber = aClientNumber;	
	}
	
/**
 *
 * NewL
 * CFsmDevSoundClient1*
 *
 */
CFsmDevSoundClient* CFsmDevSoundClient::NewL(TInt aClientNumber, CTestStepFsmDevSoundNotification* aParent)
	{
	CFsmDevSoundClient* s = new(ELeave) CFsmDevSoundClient(aClientNumber,aParent);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}
	
/**
 *
 * ConstructL
 * @result void
 *
 */
void CFsmDevSoundClient::ConstructL()
	{
	iMMFDevSound = CMMFDevSound::NewL();
	}
	
/**
 *
 * CFsmDevSoundClient
 * destructor
 *
 */
CFsmDevSoundClient::~CFsmDevSoundClient()
	{
	delete iMMFDevSound;
	iFileHandle.Close();
	iFileServer.Close();
	}

/**
 *
 * InitializeComplete  - Get the callback and update Fsm
 * @param aError
 *
 */
void CFsmDevSoundClient::InitializeComplete (TInt aError)
	{
	if( aError != KErrNone)
		{
		//Stop receiving call backs
		CActiveScheduler::Stop();
		}
	iEvent = EInitComplete;
	iError = KErrNone;
	iParent->Fsm(iClientNumber,iEvent,iError);

	}

/**
 *
 * ToneFinished  - Get the callback and update Fsm
 * @param aError
 *
 */
void CFsmDevSoundClient::ToneFinished(TInt /*aError*/)
	{
	//Implement this when required
	}

/**
 *
 * BufferToBeFilled  - Get the call back and update Fsm
 * @param aBuffer
 *
 */
void CFsmDevSoundClient::BufferToBeFilled (CMMFBuffer* aBuffer)
	{
	iEvent = EBufferToBeFilled;
	iError = KErrNone;
	iBuffer = aBuffer;
	iParent->Fsm(iClientNumber,iEvent,iError);
	}
	
/**
 *
 * PlayError  - Get the call back ,and update Fsm
 * @param aError
 *
 */
void CFsmDevSoundClient::PlayError(TInt aError)
	{
	iEvent = EPlayError;
	iError = aError;	
	iParent->Fsm(iClientNumber,iEvent,iError);
	}
	
/**
 * 
 * BufferToBeEmptied  - Get the call back and update Fsm
 * @param aBuffer
 *
 */
void CFsmDevSoundClient::BufferToBeEmptied (CMMFBuffer* aBuffer)
	{
	iEvent = EBufferToBeEmptied;
	iError = KErrNone;
	iParent->Fsm(iClientNumber,iEvent,iError);
	iBuffer = aBuffer;
	}
	
/**
 *
 * RecordError -  - Get the call back and update the Fsm 
 * @param aError
 *
 */
void CFsmDevSoundClient::RecordError (TInt aError)
	{
	iEvent = ERecordError;
	iError = aError;
	iParent->Fsm(iClientNumber,iEvent,iError);
	}
	
/**
 *
 * ConvertError  - Get the call back ,not used 
 *
 */
void CFsmDevSoundClient::ConvertError (TInt /*aError*/)
	{
	}

/**
 *
 * DeviceMessage -  - Get the call back ,not used 
 *
 */
void CFsmDevSoundClient::DeviceMessage (TUid /*aUid*/, const TDesC8& /*aMsg*/)
	{
	}

/**
 *
 * SendEventToClient -  Get the call back ,update the internal array and 
 *                      what event It got.
 */
void CFsmDevSoundClient::SendEventToClient (const TMMFEvent& aEvent)
	{	
	if(aEvent.iEventType == KMMFEventCategoryAudioResourceAvailable)
		{
		iEvent = EResourceAvailable;
		iParent->StateChange(EStateMonitoringEvents);
		}
	else
		{
		iEvent = ESendMsg;
		}
	iError = aEvent.iErrorCode;
	iParent->Fsm(iClientNumber,iEvent,iError);
	}
	
/**
 *
 * SetFileName  - set the file to play
 * @result TInt
 *
 */
TInt CFsmDevSoundClient::SetFileName(TDesC& aFileName)
	{
	TInt err = iFileServer.Connect();
	if (err != KErrNone)
		{
		return err;
		}
	err = iFileHandle.Open(iFileServer, aFileName, EFileRead);
	return err;
	}
	
/**
 *
 * SetPrioritySettings - Sets the Priority of the DevSound
 * @result void
 *
 */
void CFsmDevSoundClient::SetPrioritySettings(TMMFPrioritySettings aPrioritySettings)
	{
	iMMFDevSound->SetPrioritySettings(aPrioritySettings);
	}

/**
 *
 * TestPlayInit
 * @result TVerdict
 *
 */
TInt CFsmDevSoundClient::TestPlayInit()
	{
	TRAPD(err, iMMFDevSound->PlayInitL());
	return err;
	}
	
/**
 *
 * TestPlayData
 * @result TVerdict
 *
 */
void CFsmDevSoundClient::TestPlayData(TInt aClient)
	{
	//Manage internal buffers of the client
	CMMFDataBuffer*  buffer = static_cast<CMMFDataBuffer*>(iBuffer);
	iFileHandle.Read(buffer->Data());
	if(buffer->Data().Length()!= buffer->RequestSize())
		{
		switch (aClient)
			{
			case 1:
			iParent->SetExpectedEvent(1, CFsmDevSoundClient::EPlayError, KErrUnderflow);
			iParent->StateChange(EStatePlayError1ErrUnderflow);
			break;
			case 2:
			iParent->SetExpectedEvent(2, CFsmDevSoundClient::EPlayError, KErrUnderflow);
			iParent->StateChange(EStatePlayError2ErrUnderflow);
			break;
			case 3:
			iParent->SetExpectedEvent(3, CFsmDevSoundClient::EPlayError, KErrUnderflow);
			iParent->StateChange(EStatePlayError3ErrUnderflow);
			break;
			}
		iBuffer->SetLastBuffer(ETrue);
		}
	iMMFDevSound->PlayData();
	}

/**
 *
 * TestPlayData
 * @result TVerdict
 *
 */
void CFsmDevSoundClient::TestPlayStop()
	{
	iMMFDevSound->Stop();
	}

/**
 *
 * TestInitialize
 * @param aMode
 * @result TVerdict
 *
 */
TInt CFsmDevSoundClient::TestInitialize(TMMFState aMode)
	{
	TRAPD(err,iMMFDevSound->InitializeL(*this, aMode));
	return err;
	}

/**
 *
 * TestInitialize
 * @param aHwDeviceUid
 * @param aMode
 * @result TVerdict
 *
 */
TInt CFsmDevSoundClient::TestInitialize(TUid aHwDeviceUid, TMMFState aMode)
	{	
	TRAPD(err, iMMFDevSound->InitializeL(*this,aHwDeviceUid, aMode));
	return err;
	}

/**
 *
 * TestInitialize
 * @param aDataType
 * @param aMode
 * @result TVerdict
 *
 */
TInt CFsmDevSoundClient::TestInitialize(TFourCC& aDataType, TMMFState aMode)
	{	
	TRAPD(err, iMMFDevSound->InitializeL(*this, aDataType, aMode));
	return err;
	}

/**
 *
 * TestRegisterAsClient
 * @param aEventType
 * @result TVerdict
 *
 */
TInt CFsmDevSoundClient::TestRegisterAsClient(TUid aEventType)
	{
	return iMMFDevSound->RegisterAsClient(aEventType);
	}

/**
 *
 * TestCancelRegister
 * @param aEventType
 * @result TVerdict
 *
 */
TInt CFsmDevSoundClient::TestCancelRegister(TUid aEventType)
	{
	return iMMFDevSound->CancelRegisterAsClient(aEventType);
	}
	
TInt CFsmDevSoundClient::TestGetResourceNotification(TUid aEventType,TDes8& aNotificationData)	
	{
	return iMMFDevSound->GetResourceNotificationData(aEventType,aNotificationData);
	}
/**
 *
 * CloseClient
 * @result void
 *
 */
void CFsmDevSoundClient::CloseClient()
	{
	delete iMMFDevSound;
	}
	
/**
 *
 * TestWillResumePlay
 * @result TVerdict
 *
 */
TInt CFsmDevSoundClient::TestWillResumePlay()	
	{
	return iMMFDevSound->WillResumePlay();
	}

/**
 *
 * CDevSoundClient1
 * constructor
 *
 */
CDevSoundClient1::CDevSoundClient1(CTestStepDevSoundNotification* aParent):iParent(aParent)
	{
	iExpectedValue = KErrNone;
	ResetCallbacks();	
	iErrInUse = EFalse;
	}
	
/**
 *
 * CDevSoundClient1
 * constructor
 *
 */
CDevSoundClient1::CDevSoundClient1(CEventNotification* aParent):iOtherParent(aParent)
	{
	iExpectedValue = KErrNone;
	ResetCallbacks();	
	iErrInUse = EFalse;
	iSpecialCase = ETrue;
	}
	
/**
 *
 * NewL
 * CDevSoundClient1*
 *
 */
CDevSoundClient1* CDevSoundClient1::NewL(CTestStepDevSoundNotification* aParent)
	{
	CDevSoundClient1* s = new(ELeave) CDevSoundClient1(aParent);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

/**
 *
 * NewL
 * CDevSoundClient1*
 *
 */
CDevSoundClient1* CDevSoundClient1::NewL(CEventNotification* aParent)
	{
	CDevSoundClient1* s = new(ELeave) CDevSoundClient1(aParent);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

/**
 *
 * CDevSoundClient1
 * destructor
 *
 */
CDevSoundClient1::~CDevSoundClient1()
	{
	if (iMMFDevSound)
		{
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	}

/**
 *
 * ConstructL
 * @result void
 *
 */
void CDevSoundClient1::ConstructL()
	{
	iMMFDevSound = CMMFDevSound::NewL();
	}

/**
 *
 * InitializeComplete  - Get the call back and update the internal array 
 * @param aError
 *
 */
void CDevSoundClient1::InitializeComplete (TInt aError)
	{
	if (!iSpecialCase)
		{
		TRequestStatus* stat = iParent->GetStatus();
		User::RequestComplete(stat, aError);
		}
	iCallbackArray[EInitComplete] ++;
	iCallbackError = aError;
	}

/**
 *
 * ToneFinished   - Get the call back and update the internal array 
 * @param aError
 *
 */
void CDevSoundClient1::ToneFinished (TInt aError)
	{
	if (!iSpecialCase)
		{
		TRequestStatus* stat = iParent->GetStatus();
		User::RequestComplete(stat, aError);
		}
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	}

/**
 *
 * BufferToBeFilled  - Get the call back ,update the internal array and fill the buffer
 * @param aBuffer
 *
 */
void CDevSoundClient1::BufferToBeFilled (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		if (aBuffer != NULL) 
			{
			User::RequestComplete(stat, KErrNone);
			iCallbackError = KErrNone;
			}
		else 
			{
			User::RequestComplete(stat, KErrNotFound);
			iCallbackError = KErrNotFound;
			}
		}
	iCallbackArray[EBuffToFill] ++;
	if(!iErrInUse)
		{
		CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
		iFileHandle.Read(buffer->Data());
		if (buffer->Data().Length()!= buffer->RequestSize())
			{
			iBuffer->SetLastBuffer(ETrue);
			}
		}
	}

/**
 *
 * PlayError  - Get the call back ,update the internal array and check the error cause
 * @param aError
 *
 */
void CDevSoundClient1::PlayError (TInt aError)
	{
 	iCallbackError = aError;
	iCallbackArray[EPlayError] ++;
	if(aError == KErrInUse)
		{
		iErrInUse = ETrue;
		}
	if(aError == KErrUnderflow)
		{
		CActiveScheduler::Stop();
		}
	}

/**
 * 
 * BufferToBeEmptied  - Get the call back and update the internal array 
 * @param aBuffer
 *
 */
void CDevSoundClient1::BufferToBeEmptied (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		if (aBuffer != NULL)
			{
			User::RequestComplete(stat, KErrNone);
			iCallbackError = KErrNone;
			}
		else
			{
			User::RequestComplete(stat, KErrNotFound);
			iCallbackError = KErrNotFound;
			}
		}
	iCallbackArray[EBuffToEmpty] ++;
	}

/**
 *
 * RecordError -  - Get the call back and update the internal array 
 * @param aError
 *
 */
void CDevSoundClient1::RecordError (TInt aError)
	{
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		User::RequestComplete (stat, aError);
		}
	iCallbackError = aError;
	iCallbackArray[ERecError] ++;
	}

/**
 *
 * ConvertError  - Get the call back ,not used 
 *
 */
void CDevSoundClient1::ConvertError (TInt /*aError*/)
	{
	}

/**
 *
 * DeviceMessage -  - Get the call back ,not used 
 *
 */
void CDevSoundClient1::DeviceMessage (TUid, const TDesC8& /*aMsg*/)
	{
	}

/**
 *
 * SendEventToClient -  Get the call back ,update the internal array and 
 *                      what event It got.
 */
void CDevSoundClient1::SendEventToClient (const TMMFEvent& aEvent)
	{
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		User::RequestComplete (stat, aEvent.iErrorCode);
		}
	if(aEvent.iEventType == KMMFEventCategoryAudioResourceAvailable)
		{
		iCallbackArray[ESendMsg] ++;
		}
	}

/**
 *
 * ResetCallbacks - reset all the element in call back array to 0;
 *
 */
void CDevSoundClient1::ResetCallbacks()
	{
	//Initialize callback array
	iCallbackArray.Reset();
	}

/**
 *
 * GetCallbackTotal - gets the total call back
 * @result TInt
 *
 */
TInt CDevSoundClient1::GetCallbackTotal()
	{
	TInt total = 0;
	for (TInt i = EInitComplete; i < EDeviceMsg; i++)
		{
		total += iCallbackArray[i];
		}
	return total;
	}

/**
 *
 * InitialiseActiveListener - Initialize the parent Active Listener
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient1::InitialiseActiveListener()	
	{
	return iParent->InitialiseActiveListener();
	}

/**
 *
 * GetErrorStatus - Gets the Error Status
 * @result TInt
 *
 */
TInt CDevSoundClient1::GetErrorStatus(TInt aCallBack)
	{
	switch(aCallBack)
		{
	case EInitComplete:
		return iCallbackArray[EInitComplete];
	case EToneFinished:
		return iCallbackArray[EToneFinished];
	case EBuffToFill:
		return iCallbackArray[EBuffToFill];
	case EPlayError:
		return iCallbackArray[EPlayError];
	case EBuffToEmpty:
		return iCallbackArray[EBuffToEmpty];
	case EConvertMsg:
		return iCallbackArray[EConvertMsg];
	case EDeviceMsg:
		return iCallbackArray[EDeviceMsg];
	case ESendMsg:
		return iCallbackArray[ESendMsg];
	default:
		return KErrNotFound;
		}
	}
	
/**
 *
 * SetPrioritySettings - Sets the Priority of the DevSound
 * @result void
 *
 */
void CDevSoundClient1::SetPrioritySettings(TMMFPrioritySettings aPrioritySettings)
	{
	iMMFDevSound->SetPrioritySettings(aPrioritySettings);
	}

/**
 *
 * TestVolume  - Get the Volume
 * @result TInt
 *
 */
TInt CDevSoundClient1::TestVolume()
	{
	return iMMFDevSound->Volume();
	}

/**
 *
 * SetFileName  - set the file to play
 * @result TInt
 *
 */
TVerdict CDevSoundClient1::SetFileName(TDesC& aFileName)
	{
	TInt err = iFileServer.Connect();
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	err = iFileHandle.Open(iFileServer, aFileName, EFileRead);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * TestSetVolume
 * @param aVol
 *
 */
void CDevSoundClient1::TestSetVolume(TInt aVol)
	{
	iMMFDevSound->SetVolume(aVol);
	}

/**
 *
 * TestMaxVolume
 * @result TInt
 *
 */
TInt CDevSoundClient1::TestMaxVolume()
	{
	return iMMFDevSound->MaxVolume();
	}

/**
 *
 * TestPlayInit
 * @result TVerdict
 *
 */
TVerdict  CDevSoundClient1::TestPlayInit()
	{
	ResetCallbacks();
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();
		}
	//get buffer from devsound
	TRAPD(err, iMMFDevSound->PlayInitL());
	if (err)
		{
		return EFail;
		}
	else
		{
		if (!iSpecialCase)
			{
			// Start the active scheduler and catch the callback
 			CActiveScheduler::Start();
			if (iCallbackArray[EBuffToFill] != 1)
				{
				return EFail;
				}
			TInt tot = GetCallbackTotal();
			if (tot > 1)
				{
				return EFail;
				}
			}
		}
	return EPass;
	}

/**
 *
 * TestPlayData
 * @result TVerdict
 *
 */
void CDevSoundClient1::TestPlayData()
	{
	ResetCallbacks();
	iMMFDevSound->PlayData();
	}

/**
 *
 * TestDigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient1::TestDigitalPlayback()
	{
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type
	return TestDigitalPlayback(pcm16);
	}

/**
 *
 * TestDigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient1::TestDigitalPlayback(TFourCC& aDataType)
	{
	//Initialize
	TVerdict initOK = TestInitialize(aDataType, EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	//SetVolume
	TestSetVolume(iMMFDevSound->MaxVolume());
	//Get a buffer to fill
	initOK = TestPlayInit();
	if (initOK != KErrNone)
		{
		return EFail;
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient1::TestInitialize(TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	ResetCallbacks();
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();	
		}
		
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		if (!iSpecialCase)
			{
			CActiveScheduler::Start();
			if (iCallbackError != iExpectedValue)
				{
				return EFail;
				}
			if (iCallbackArray[EInitComplete] != 1)
				{
				return EFail;
				}				
			}
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aHwDeviceUid
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient1::TestInitialize(TUid aHwDeviceUid, TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	ResetCallbacks();
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();
		}
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aHwDeviceUid, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		if (!iSpecialCase)
			{
			CActiveScheduler::Start();
			if (iCallbackError != iExpectedValue)
				{
				return EFail;
				}
			if (iCallbackArray[EInitComplete] != 1)
				{
				return EFail;
				}
			}
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aDataType
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient1::TestInitialize(TFourCC& aDataType, TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	ResetCallbacks();
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();
		}
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aDataType, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		if (!iSpecialCase)
			{
			CActiveScheduler::Start();
			if (iCallbackError != iExpectedValue)
				{
				return EFail;
				}
			if (iCallbackArray[EInitComplete] != 1)
				{
				return EFail;
				}
			}
		}
	return EPass;
	}

/**
 *
 * TestRegisterAsClient
 * @param aEventType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient1::TestRegisterAsClient(TUid aEventType)
	{
	TInt err = iMMFDevSound->RegisterAsClient(aEventType);
	if(err)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * TestCancelRegister
 * @param aEventType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient1::TestCancelRegister(TUid aEventType)
	{
	TInt err = iMMFDevSound->CancelRegisterAsClient(aEventType);
	if(err)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * CDevSoundClient2
 * constructor
 *
 */
CDevSoundClient2::CDevSoundClient2(CTestStepDevSoundNotification* aParent):iParent(aParent)
	{
	iExpectedValue = KErrNone;
	ResetCallbacks();	
	}
	
/**
 *
 * CDevSoundClient2
 * constructor
 *
 */
CDevSoundClient2::CDevSoundClient2(CEventNotification* aParent):iOtherParent(aParent)
	{
	iExpectedValue = KErrNone;
	iSpecialCase =ETrue;
	ResetCallbacks();	
	}
	
/**
 *
 * NewL
 * Test step constructor
 *
 */

CDevSoundClient2* CDevSoundClient2::NewL(CTestStepDevSoundNotification* aParent)
	{
	CDevSoundClient2* s = new(ELeave) CDevSoundClient2(aParent);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

/**
 *
 * NewL
 * Test step constructor
 *
 */

CDevSoundClient2* CDevSoundClient2::NewL(CEventNotification* aParent)
	{
	CDevSoundClient2* s = new(ELeave) CDevSoundClient2(aParent);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}
	
/**
 *
 * ~CTestStepDevSound
 *	 Test step destructor
 *
 */
CDevSoundClient2::~CDevSoundClient2()
	{
	if (iMMFDevSound)
		{
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	}

/**
 *
 * ConstructL
 * @result void
 *
 */
void CDevSoundClient2::ConstructL()
	{
	iMMFDevSound = CMMFDevSound::NewL();
	}

/**
 *
 * InitializeComplete
 * @param aError
 *
 */
void CDevSoundClient2::InitializeComplete (TInt aError)
	{
	if (!iSpecialCase)
		{
		TRequestStatus* stat = iParent->GetStatus();
		User::RequestComplete(stat, aError);
		}
	iCallbackArray[EInitComplete] ++;
	iCallbackError = aError;
	}

/**
 *
 * ToneFinished
 * @param aError
 *
 */
void CDevSoundClient2::ToneFinished (TInt aError)
	{
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		User::RequestComplete(stat, aError);
		}
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	}

/**
 *
 * BufferToBeFilled
 * @param aBuffer
 *
 */
void CDevSoundClient2::BufferToBeFilled (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		if (aBuffer != NULL) 
			{
			User::RequestComplete(stat, KErrNone);
			iCallbackError = KErrNone;
			}
		else 
			{
			User::RequestComplete(stat, KErrNotFound);
			iCallbackError = KErrNotFound;
			}
		}
	iCallbackArray[EBuffToFill] ++;
	CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
	iFileHandle.Read(buffer->Data());
	if (buffer->Data().Length()!= buffer->RequestSize())
		{
		iBuffer->SetLastBuffer(ETrue);
		}
	}

/**
 *
 * PlayError
 * @param aError
 *
 */
void CDevSoundClient2::PlayError (TInt aError)
	{
	iCallbackError = aError;
	iCallbackArray[EPlayError] ++;
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		User::RequestComplete (stat, aError);
		}
	}

/**
 * 
 * BufferToBeEmptied
 * @param aBuffer
 *
 */
void CDevSoundClient2::BufferToBeEmptied (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		if (aBuffer != NULL)
			{
			User::RequestComplete(stat, KErrNone);
			iCallbackError = KErrNone;
			}
		else
			{
			User::RequestComplete(stat, KErrNotFound);
			iCallbackError = KErrNotFound;
			}
		}
	iCallbackArray[EBuffToEmpty] ++;
	}

/**
 *
 * RecordError
 * @param aError
 *
 */
void CDevSoundClient2::RecordError (TInt aError)
	{
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		User::RequestComplete (stat, aError);
		}
	iCallbackError = aError;
	iCallbackArray[ERecError] ++;
	}

/**
 *
 * ConvertError
 *
 */
void CDevSoundClient2::ConvertError (TInt /*aError*/)
	{}

/**
 *
 * DeviceMessage
 *
 */
void CDevSoundClient2::DeviceMessage (TUid, const TDesC8& /*aMsg*/)
	{
	}

/**
 *
 * SendEventToClient -  Get the call back ,update the internal array and 
 *                      what event It got.
 */
void CDevSoundClient2::SendEventToClient (const TMMFEvent& aEvent)
	{
	if (!iSpecialCase)
		{
		TRequestStatus* stat =iParent->GetStatus();
		User::RequestComplete (stat, aEvent.iErrorCode);
		}
	iCallbackArray[ESendMsg] ++;
	}
	
/**
 *
 * SetFileName  - set the file to play
 * @result TInt
 *
 */
TVerdict CDevSoundClient2::SetFileName(TDesC& aFileName)
	{
	TInt err = iFileServer.Connect();
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	err = iFileHandle.Open(iFileServer, aFileName, EFileRead);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	return EPass;
	}
	
void CDevSoundClient2::SetPrioritySettings(TMMFPrioritySettings aPrioritySettings)
	{
	iMMFDevSound->SetPrioritySettings(aPrioritySettings);
	}

/**
 *
 * ResetCallbacks
 * @result void
 *
 */
void CDevSoundClient2::ResetCallbacks()
	{
	//Initialize callback array
	iCallbackArray.Reset();
	}

/**
 *
 * GetCallbackTotal
 * @result TInt
 *
 */
TInt CDevSoundClient2::GetCallbackTotal()
	{
	TInt total = 0;
	for (TInt i = EInitComplete; i < EDeviceMsg; i++)
		{
		total += iCallbackArray[i];
		}
	return total;
	}

/**
 *
 * CloseClient
 * @result void
 *
 */
void CDevSoundClient2::CloseClient()
	{
	delete iMMFDevSound;
	iMMFDevSound = NULL;
	}
	
/**
 *
 * TestVolume
 * @result TInt
 *
 */
TInt CDevSoundClient2::TestVolume()
	{
	return iMMFDevSound->Volume();
	}
	
/**
 *
 * GetErrorStatus
 * @result TInt
 *
 */
TInt CDevSoundClient2::GetErrorStatus(TInt aCallBack)
	{
	switch(aCallBack)
		{
	case EInitComplete:
		return iCallbackArray[EInitComplete];
	case EToneFinished:
		return iCallbackArray[EToneFinished];
	case EBuffToFill:
		return iCallbackArray[EBuffToFill];
	case EPlayError:
		return iCallbackArray[EPlayError];
	case EBuffToEmpty:
		return iCallbackArray[EBuffToEmpty];
	case EConvertMsg:
		return iCallbackArray[EConvertMsg];
	case EDeviceMsg:
		return iCallbackArray[EDeviceMsg];
	case ESendMsg:
		return iCallbackArray[ESendMsg];
	default:
		return KErrNotFound;
		}
	}
	
/**
 *
 * TestStop
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestStop()
	{
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();
		}
	iMMFDevSound->Stop();
	CActiveScheduler::Start();
	TInt err = iParent->GetStatus()->Int();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	return EPass;	
	}

/**
 *
 * TestSetVolume
 * @param aVol
 *
 */
void CDevSoundClient2::TestSetVolume(TInt aVol)
	{
	iMMFDevSound->SetVolume(aVol);
	}

/**
 *
 * TestSetGain
 * @result TInt
 *
 */
TInt CDevSoundClient2::TestMaxVolume()
	{
	return iMMFDevSound->MaxVolume();
	}

/**
 *
 * TestPlayInit
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestPlayInit()
	{
	ResetCallbacks();
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();
		}
	//get buffer from devsound
	TRAPD(err, iMMFDevSound->PlayInitL());
	if (err)
		{
		return EFail;
		}
	else
		{
		if (!iSpecialCase)
			{
			CActiveScheduler::Start();
			if (iCallbackArray[EBuffToFill] != 1)
				{
				return EFail;
				}
			TInt tot = GetCallbackTotal();
			if (tot > 1)
				{
				return EFail;
				}
			}
		}
	return EPass;
	}

/**
 *
 * TestPlayData
 * @result TVerdict
 *
 */
void CDevSoundClient2::TestPlayData()
	{
	ResetCallbacks();
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();
		}
	iMMFDevSound->PlayData();
	if (!iSpecialCase)
		{
		CActiveScheduler::Start();
		}
	}

/**
 *
 * GetCallbackTotal
 * @param aNumSamples
 * @param aFilename
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestDigitalPlayback()
	{
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type
	return TestDigitalPlayback(pcm16);
	}

/**
 *
 * TestDigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestDigitalPlayback(TFourCC& aDataType)
	{
	TVerdict initOK = TestInitialize(aDataType, EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	//SetVolume
	TestSetVolume(iMMFDevSound->MaxVolume());
	//Get a buffer to fill
	initOK = TestPlayInit();
	if (initOK != KErrNone)
		{
		return EFail;
		}
	return EPass;
	}

/**
 *
 * TestDigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestInitRegAndPlayBack(TFourCC& aDataType,TUid aEventType)
	{
	//Initialize
	TVerdict initOK = TestInitialize(aDataType, EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	//SetVolume
	TestSetVolume(iMMFDevSound->MaxVolume());
	//Get a buffer to fill
	initOK = TestPlayInit();
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	initOK = TestRegisterAsClient(aEventType);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestInitialize(TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	ResetCallbacks();
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();
		}
		
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		if (!iSpecialCase)
			{
			CActiveScheduler::Start();
			if (iCallbackError != iExpectedValue)
				{
				return EFail;
				}
			if (iCallbackArray[EInitComplete] != 1)
				{
				return EFail;
				}
			}
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aHwDeviceUid
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestInitialize(TUid aHwDeviceUid, TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	ResetCallbacks();
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();
		}
		
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aHwDeviceUid, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aDataType
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestInitialize(TFourCC& aDataType, TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	ResetCallbacks();
	if (!iSpecialCase)
		{
		iParent->InitialiseActiveListener();
		}
		
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aDataType, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		if (!iSpecialCase)
			{
			CActiveScheduler::Start();
			if (iCallbackError != iExpectedValue)
				{
				return EFail;
				}
			if (iCallbackArray[EInitComplete] != 1)
				{
				return EFail;
				}
			}
		}
	return EPass;
	}

/**
 *
 * TestRegisterAsClient
 * @param aEventType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestRegisterAsClient(TUid aEventType)
	{
	TInt err = iMMFDevSound->RegisterAsClient(aEventType);
	if(err)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * TestCancelRegister
 * @param aEventType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient2::TestCancelRegister(TUid aEventType)
	{
	TInt err = iMMFDevSound->CancelRegisterAsClient(aEventType);
	if(err)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * CTestStepDevSound
 * Test step constructor
 *
 */
CDevSoundClient3::CDevSoundClient3(CTestStepDevSoundNotification* aParent):iParent(aParent)
	{
	iExpectedValue = KErrNone;
	ResetCallbacks();	
	}

/**
 *
 * NewL
 * Test step constructor
 *
 */
CDevSoundClient3* CDevSoundClient3::NewL(CTestStepDevSoundNotification* aParent)
	{
	CDevSoundClient3* s = new(ELeave) CDevSoundClient3(aParent);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

/**
 *
 * ~CTestStepDevSound
 *	 Test step destructor
 *
 */
CDevSoundClient3::~CDevSoundClient3()
	{
	if (iMMFDevSound)
		{
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	}

/**
 *
 * ConstructL
 * @result void
 *
 */
void CDevSoundClient3::ConstructL()
	{
	iMMFDevSound = CMMFDevSound::NewL();
	}

/**
 *
 * InitializeComplete
 * @param aError
 *
 */
void CDevSoundClient3::InitializeComplete (TInt aError)
	{
	TRequestStatus* stat=NULL;
	if(iParent)
		{
		stat = iParent->GetStatus();
		User::RequestComplete(stat, aError);
		}
	iCallbackArray[EInitComplete] ++;
	iCallbackError = aError;
	}

/**
 *
 * ToneFinished
 * @param aError
 *
 */
void CDevSoundClient3::ToneFinished (TInt aError)
	{
	TRequestStatus* stat =NULL;
	if(iParent)
		{
		stat =iParent->GetStatus();
		}
	User::RequestComplete(stat, aError);
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	}

/**
 *
 * BufferToBeFilled
 * @param aBuffer
 *
 */
void CDevSoundClient3::BufferToBeFilled (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	TRequestStatus* stat=NULL;
	if(iParent)
		{
		stat =iParent->GetStatus();
		}

	if (aBuffer != NULL) 
		{
		User::RequestComplete(stat, KErrNone);
		iCallbackError = KErrNone;
		}
	else 
		{
		User::RequestComplete(stat, KErrNotFound);
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToFill] ++;
	CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
	iFileHandle.Read(buffer->Data());
	if (buffer->Data().Length()!= buffer->RequestSize())
		{
		iBuffer->SetLastBuffer(ETrue);
		}
	}

/**
 *
 * PlayError
 * @param aError
 *
 */
void CDevSoundClient3::PlayError (TInt aError)
	{
	iCallbackError = aError;
	iCallbackArray[EPlayError] ++;
	}

/**
 * 
 * BufferToBeEmptied
 * @param aBuffer
 *
 */
void CDevSoundClient3::BufferToBeEmptied (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	TRequestStatus* stat =NULL;
	if(iParent)
		{
		stat =iParent->GetStatus();
		}
		
	if (aBuffer != NULL)
		{
		User::RequestComplete(stat, KErrNone);
		iCallbackError = KErrNone;
		}
	else
		{
		User::RequestComplete(stat, KErrNotFound);
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToEmpty] ++;
	}

/**
 *
 * RecordError
 * @param aError
 *
 */
void CDevSoundClient3::RecordError (TInt aError)
	{
	TRequestStatus* stat =NULL;
	if(iParent)
		{
		stat =iParent->GetStatus();
		}
		
	User::RequestComplete (stat, aError);
	iCallbackError = aError;
	iCallbackArray[ERecError] ++;
	}

/**
 *
 * ConvertError
 *
 */
void CDevSoundClient3::ConvertError (TInt /*aError*/)
	{
	}

/**
 *
 * DeviceMessage
 *
 */
void CDevSoundClient3::DeviceMessage (TUid, const TDesC8& /*aMsg*/)
	{
	}

/**
 *
 * SendEventToClient -  Get the call back ,update the internal array and 
 *                      what event It got.
 */
void CDevSoundClient3::SendEventToClient (const TMMFEvent& aEvent)
	{
	TRequestStatus* stat =iParent->GetStatus();
	User::RequestComplete (stat, aEvent.iErrorCode);
	if(aEvent.iEventType == KMMFEventCategoryAudioResourceAvailable)
		{
		iCallbackArray[ESendMsg] ++;	
		}
	}
	
/**
 *
 * SetFileName  - set the file to play
 * @result TInt
 *
 */
TVerdict CDevSoundClient3::SetFileName(TDesC& aFileName)
	{
	TInt err = iFileServer.Connect();
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	err = iFileHandle.Open(iFileServer, aFileName, EFileRead);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * SetPrioritySettings - Sets the Priority of the DevSound
 * @result void
 *
 */
void CDevSoundClient3::SetPrioritySettings(TMMFPrioritySettings aPrioritySettings)
	{
	iMMFDevSound->SetPrioritySettings(aPrioritySettings);
	}

/**
 *
 * ResetCallbacks
 *
 */
void CDevSoundClient3::ResetCallbacks()
	{
	//Initialize callback array
	iCallbackArray.Reset();
	}

/**
 *
 * GetCallbackTotal
 * @result TInt
 *
 */
TInt CDevSoundClient3::GetCallbackTotal()
	{
	TInt total = 0;
	for (TInt i = EInitComplete; i < EDeviceMsg; i++)
		{
		total += iCallbackArray[i];
		}
	return total;
	}

/**
 *
 * CloseClient
 * @result void
 *
 */
void CDevSoundClient3::CloseClient()
	{
	iParent->InitialiseActiveListener();
	delete iMMFDevSound;
	iMMFDevSound = NULL;
	CActiveScheduler::Start();
	}
	
/**
 *
 * TestVolume
 * @result TInt
 *
 */
TInt CDevSoundClient3::TestVolume()
	{
	return iMMFDevSound->Volume();
	}
	
/**
 *
 * TestWillResumePlay
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestWillResumePlay()	
	{
	TInt err = iMMFDevSound->WillResumePlay();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	return EPass;	
	}
	
/**
 *
 * GetErrorStatus
 * @result TInt
 *
 */
TInt CDevSoundClient3::GetErrorStatus(TInt aCallBack)
	{
	switch(aCallBack)
		{
	case EInitComplete:
		return iCallbackArray[EInitComplete];
	case EToneFinished:
		return iCallbackArray[EToneFinished];
	case EBuffToFill:
		return iCallbackArray[EBuffToFill];
	case EPlayError:
		return iCallbackArray[EPlayError];
	case EBuffToEmpty:
		return iCallbackArray[EBuffToEmpty];
	case EConvertMsg:
		return iCallbackArray[EConvertMsg];
	case EDeviceMsg:
		return iCallbackArray[EDeviceMsg];
	case ESendMsg:
		return iCallbackArray[ESendMsg];
	default:
		return KErrNotFound;
		}
	}
	
/**
 *
 * TestStop
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestStop()
	{
	
	iParent->InitialiseActiveListener();	
	iMMFDevSound->Stop();
	CActiveScheduler::Start();
	TInt err = iParent->GetStatus()->Int();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	return EPass;	
	}

/**
 *
 * TestSetVolume
 * @param aVol
 *
 */
void CDevSoundClient3::TestSetVolume(TInt aVol)
	{
	iMMFDevSound->SetVolume(aVol);
	}

/**
 *
 * TestSetGain
 * @result TInt
 *
 */
TInt CDevSoundClient3::TestMaxVolume()
	{
	return iMMFDevSound->MaxVolume();
	}

/**
 *
 * TestPlayInit
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestPlayInit()
	{
	ResetCallbacks();
	iParent->InitialiseActiveListener();
	//get buffer from devsound
	TRAPD(err, iMMFDevSound->PlayInitL());
	CActiveScheduler::Start();
	if (err)
		{
		return EFail;
		}
	else
		{
		if (iCallbackArray[EBuffToFill] != 1)
			{
			return EFail;
			}
		TInt tot = GetCallbackTotal();
		if (tot > 1)
			{
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestPlayData
 * @result TVerdict
 *
 */
void CDevSoundClient3::TestPlayData()
	{
	ResetCallbacks();
	iParent->InitialiseActiveListener();
	iMMFDevSound->PlayData();
	CActiveScheduler::Start();
	}

/**
 *
 * GetCallbackTotal
 * @param aNumSamples
 * @param aFilename
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestDigitalPlayback()
	{
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type
	return TestDigitalPlayback(pcm16);
	}

/**
 *
 * TestDigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestDigitalPlayback(TFourCC& aDataType)
	{
	TVerdict initOK = TestInitialize(aDataType, EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	//SetVolume
	TestSetVolume(iMMFDevSound->MaxVolume());
	//Get a buffer to fill
	initOK = TestPlayInit();
	if (initOK != KErrNone)
		{
		return EFail;
		}
	return EPass;
	}

/**
 *
 * TestDigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestInitRegAndPlayBack(TDesC& aFilename, TFourCC& aDataType,TUid aEventType)
	{
	//Initialize
	TVerdict initOK = TestInitialize(aDataType, EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	//SetVolume
	TestSetVolume(iMMFDevSound->MaxVolume());
	//Get a buffer to fill
	initOK = TestPlayInit();
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	initOK = TestRegisterAsClient(aEventType);
	if (initOK != EPass)
		{
		return EInconclusive;
		}
	//open file
	RFs fs;
	RFile file;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		return EInconclusive;
		}

	err = file.Open(fs, aFilename, EFileRead);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestInitialize(TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	ResetCallbacks();
	iParent->InitialiseActiveListener();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aHwDeviceUid
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestInitialize(TUid aHwDeviceUid, TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	ResetCallbacks();
	iParent->InitialiseActiveListener();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aHwDeviceUid, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aDataType
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestInitialize(TFourCC& aDataType, TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	ResetCallbacks();
	iParent->InitialiseActiveListener();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aDataType, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestRegisterAsClient
 * @param aEventType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestRegisterAsClient(TUid aEventType)
	{
	TInt err = iMMFDevSound->RegisterAsClient(aEventType);
	if(err)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * TestCancelRegister
 * @param aEventType
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestCancelRegister(TUid aEventType)
	{
	TInt err = iMMFDevSound->CancelRegisterAsClient(aEventType);
	if(err)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * TestGetResourceNotification
 * @param aEventType
 * @param aNotificationData
 * @result TVerdict
 *
 */
TVerdict CDevSoundClient3::TestGetResourceNotification(TUid aEventType,TDes8& aNotificationData)	
	{
	TInt err = iMMFDevSound->GetResourceNotificationData(aEventType,aNotificationData);
	if(err)
		{
		return EInconclusive;
		}
	return EPass;
	}

/**
 *
 * CTestStepDevSound0040
 *
 */
CTestStepDevSound0040::CTestStepDevSound0040()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0040-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0040::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Register for notification"));
	TVerdict result = EFail;
	//Low Priority
	TMMFPrioritySettings lowPriority;
	lowPriority.iPref = EMdaPriorityPreferenceNone;
	lowPriority.iPriority = 0;
	lowPriority.iState = EMMFStatePlaying;
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iDevSoundClient1->SetPrioritySettings(lowPriority);
	
	//High Priority
	TMMFPrioritySettings highPriority;
	highPriority.iPref = EMdaPriorityPreferenceNone;
	highPriority.iPriority = 20;
	highPriority.iState = EMMFStatePlaying;
	INFO_PRINTF1(_L("Client 2 with high priority"));
	iDevSoundClient2->SetPrioritySettings(highPriority);
	
	TFileName filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	result = iDevSoundClient1->SetFileName(filename);
	TFourCC pcm8(' ','P','U','8');
	// Initialize and play client 1
	INFO_PRINTF1(_L("Client 1 Initialize and play"));
	result =  iDevSoundClient1->TestDigitalPlayback(pcm8);
	if (result != EPass)
		{
		return EInconclusive;
		}
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	result = iDevSoundClient1->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (result != EPass)
		{
		return EInconclusive;
		}
	// Initialize and play client 2
	iDevSoundClient1->ResetCallbacks();	
	TFileName filename2 = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	result = iDevSoundClient2->SetFileName(filename2);
	TFourCC pcm16(KMMFFourCCCodePCM16);
	if (result != EPass)
		{
		return EInconclusive;
		}	
	INFO_PRINTF1(_L("Client 2 Initialize and play"));	
	result = iDevSoundClient2->TestDigitalPlayback(pcm16);
	if (result != EPass)
		{
		return EInconclusive;
		}

	// check sendeventtoclient is called from devsoundobserver
	iDevSoundClient2->ResetCallbacks();
	INFO_PRINTF1(_L("Client 2 playing ..."));	
	iDevSoundClient2->TestPlayData();
	INFO_PRINTF1(_L("Client 2 stopped"));	
	result = iDevSoundClient2->TestStop();
	if (result != EPass)
		{
		return EInconclusive;
		}
	INFO_PRINTF1(_L("Getting status of Client 1"));	
	if(!iDevSoundClient1->GetErrorStatus(ESendMsg))
		{
		INFO_PRINTF1(_L("Client 1 not received any notification"));
		return EFail;
		}
	return result;
	}

/**
 *
 * CTestStepDevSound0041
 *
 */
CTestStepDevSound0041::CTestStepDevSound0041()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0041-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0041::DoTestStepL(void)
	{
 	INFO_PRINTF1(_L("Testing Register for notification"));
	TVerdict result = EFail;
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	iEventHandler = CEventNotification::NewL();
	iEventHandler->Start();
 	CActiveScheduler::Start();
 	result = iEventHandler->GetResult();
 	delete iEventHandler;
 	iEventHandler=NULL;
	#else
	INFO_PRINTF1(_L("Test disabled from this suite for a3f. Implemented in A3F-DEVSOUND-CHRTZ-PLAY-RAW-0039 instead"));
	#endif
 	return result;
	}

void CTestStepDevSound0041::Close()
	{
	delete iEventHandler;	
	}
	
/**
 *
 * CTestStepDevSound0042
 *
 */
CTestStepDevSound0042::CTestStepDevSound0042()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0042-HP");
		
	//Low Priority DevSound client1
	iLowPriority.iPref = EMdaPriorityPreferenceNone;
	iLowPriority.iPriority = 0;
	iLowPriority.iState = EMMFStatePlaying;
	
	//High Priority DevSoundClient2
	iHighPriority.iPref = EMdaPriorityPreferenceNone;
	iHighPriority.iPriority = 20;
	iHighPriority.iState = EMMFStatePlaying;
	}

void CTestStepDevSound0042::InitializeTest()
	{
	iStateHandlers[EStateInitComplete1] = &CTestStepDevSound0042::HandleStateInitComplete1;
	iStateHandlers[EStateInitComplete2] = &CTestStepDevSound0042::HandleStateInitComplete2;
	iStateHandlers[EStateInitCompletePlaying1] = &CTestStepDevSound0042::HandleStateInitCompletePlaying1;
	iStateHandlers[EStateInitCompletePlaying2] = &CTestStepDevSound0042::HandleStateInitCompletePlaying2;
	iStateHandlers[EStatePlayError2ErrUnderflow] = &CTestStepDevSound0042::HandleStatePlayError2ErrUnderflow;
	iStateHandlers[EStateResourceAvailable1] = &CTestStepDevSound0042::HandleStateResourceAvailable1;
	iStateHandlers[EStatePlayError1ErrInUse] = &CTestStepDevSound0042::HandleStatePlayError1ErrInUse;
	}

/**
 * FsmL
 */
 void CTestStepDevSound0042::Fsm(TInt aClientNumber, TInt aEvent, TInt aError)
 	{	
 	if(FsmCheck(aClientNumber,aEvent,aError))
 		{ 		
 		iTestStepResult = (this->*(iStateHandlers[iState]))();
 		if(iTestStepResult != EPass)
 			{
 			StopTest();
 			}
 		}
 	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0042::DoTestStepL(void)
	{	
	iTestStepResult = EFail;
	InitializeTest();
	StartTestL();
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	return iTestStepResult;
	}
 
void CTestStepDevSound0042::StartTestL()
	{
	TFileName filename;
	TFourCC pcm8(' ','P','U','8');
	TInt err = KErrNone;
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iClientList[1]->SetPrioritySettings(iLowPriority);
	
	INFO_PRINTF1(_L("Client 2 with high priority"));
	iClientList[2]->SetPrioritySettings(iHighPriority);

	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	err = iClientList[1]->SetFileName(filename);			
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	err = iClientList[1]->TestInitialize(pcm8, EMMFStatePlaying);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete1);
	}

TVerdict CTestStepDevSound0042::HandleStateInitComplete1()
	{
	TInt err = KErrNone;
	err = iClientList[1]->TestPlayInit();
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying1);
	return EPass;
	}
	
TVerdict CTestStepDevSound0042::HandleStateInitCompletePlaying1()
	{
	TInt err = KErrNone;
	TFileName filename;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[1]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (err != KErrNone)
		{
		return EInconclusive;	
		}
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	err = iClientList[2]->SetFileName(filename);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	err = iClientList[2]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(2, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete2);
	return EPass;
	}

TVerdict CTestStepDevSound0042::HandleStateInitComplete2()
	{
	TInt err = KErrNone;
	err = iClientList[2]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError1ErrInUse);
	return EPass;
	}

TVerdict CTestStepDevSound0042::HandleStatePlayError1ErrInUse()
	{
	SetExpectedEvent(2, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying2);
	RegisterClientEventToMonitor(1, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable1);	
	return EPass;
	}

TVerdict CTestStepDevSound0042::HandleStateInitCompletePlaying2()
	{
	iClientList[2]->TestPlayData(2);
	return EPass;
	}

TVerdict CTestStepDevSound0042::HandleStatePlayError2ErrUnderflow()
	{
	INFO_PRINTF1(_L("DevSoundClient2->PlayComplete"));
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Still monitor
		}
	return EPass;
	}

TVerdict CTestStepDevSound0042::HandleStateResourceAvailable1()
	{
	INFO_PRINTF1(_L("DevSoundClient1->Notification Received"));
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents);
		}
	return EPass;
	}

/**
 *
 * CTestStepDevSound0043
 *
 */
CTestStepDevSound0043::CTestStepDevSound0043()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0043-HP");
	//Low
	iLowPriority.iPref = EMdaPriorityPreferenceNone;
	iLowPriority.iPriority = -20;
	iLowPriority.iState = EMMFStatePlaying;
	//Medium
	iMediumPriority.iPref = EMdaPriorityPreferenceNone;
	iMediumPriority.iPriority = 0;
	iMediumPriority.iState = EMMFStatePlaying;
	//High
	iHighPriority.iPref = EMdaPriorityPreferenceNone;
	iHighPriority.iPriority = 20;
	iHighPriority.iState = EMMFStatePlaying;
	}

void CTestStepDevSound0043::InitializeTest()
	{
	iStateHandlers[EStateInitComplete1] = &CTestStepDevSound0043::HandleStateInitComplete1;
	iStateHandlers[EStateInitCompletePlaying1] = &CTestStepDevSound0043::HandleStateInitCompletePlaying1;
	iStateHandlers[EStatePlayError1ErrInUse] = &CTestStepDevSound0043::HandleStatePlayError1ErrInUse;
	iStateHandlers[EStateInitComplete3] = &CTestStepDevSound0043::HandleStateInitComplete3;
	iStateHandlers[EStateInitCompletePlaying3] = &CTestStepDevSound0043::HandleStateInitCompletePlaying3;
	iStateHandlers[EStatePlayError3ErrInUse] = &CTestStepDevSound0043::HandleStatePlayError3ErrInUse;
	iStateHandlers[EStateInitComplete2] = &CTestStepDevSound0043::HandleStateInitComplete2;
	iStateHandlers[EStateInitCompletePlaying2] = &CTestStepDevSound0043::HandleStateInitCompletePlaying2;
	iStateHandlers[EStatePlayError2ErrUnderflow] = &CTestStepDevSound0043::HandleStatePlayError2ErrUnderflow;
	iStateHandlers[EStateResourceAvailable1] = &CTestStepDevSound0043::HandleStateResourceAvailable1;
	iStateHandlers[EStateResourceAvailable3] = &CTestStepDevSound0043::HandleStateResourceAvailable3;
	}
	
/**
 * FsmL
 */
 void CTestStepDevSound0043::Fsm(TInt aClientNumber, TInt aEvent, TInt aError)
 	{	
 	if(FsmCheck(aClientNumber,aEvent,aError))
 		{ 		
 		iTestStepResult = (this->*(iStateHandlers[iState]))();
 		if(iTestStepResult != EPass)
 			{
 			StopTest();
 			}
 		}
 	}
 	
void CTestStepDevSound0043::StartTestL()
	{
	TFileName filename;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	TInt err = KErrNone;
	
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iClientList[1]->SetPrioritySettings(iLowPriority);

	INFO_PRINTF1(_L("Client 1 with medium priority"));
	iClientList[3]->SetPrioritySettings(iMediumPriority);

	INFO_PRINTF1(_L("Client 2 with high priority"));
	iClientList[2]->SetPrioritySettings(iHighPriority);
	
	INFO_PRINTF1(_L("Client 1 Initialize and register"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\garbage.raw");
	err = iClientList[1]->SetFileName(filename);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
		
	err = iClientList[1]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete1);
	}

TVerdict CTestStepDevSound0043::HandleStateInitComplete1()
	{
	TInt err = KErrNone;
	err = iClientList[1]->TestPlayInit();
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying1);
	return EPass;
	}

TVerdict CTestStepDevSound0043::HandleStateInitCompletePlaying1()
	{
	TFileName filename;
	TFourCC pcm8(' ','P','U','8');
	TInt err = KErrNone;
	
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[1]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	
	INFO_PRINTF1(_L("Client 3 Initialize and register"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	err = iClientList[3]->SetFileName(filename);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
		
	err = iClientList[3]->TestInitialize(pcm8, EMMFStatePlaying);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(3, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete3);
	return EPass;
	}

TVerdict CTestStepDevSound0043::HandleStateInitComplete3()
	{
	TInt err = KErrNone;
	err = iClientList[3]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError1ErrInUse);
	return EPass;
	}

TVerdict CTestStepDevSound0043::HandleStatePlayError1ErrInUse()
	{
	RegisterClientEventToMonitor(1, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable1);
	SetExpectedEvent(3, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying3);
	return EPass;
	}
	
TVerdict CTestStepDevSound0043::HandleStateInitCompletePlaying3()
	{
	TFileName filename;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	TInt err = KErrNone;
	
	INFO_PRINTF1(_L("Client 3 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[3]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
		
	INFO_PRINTF1(_L("Client 2 Initialize, register and play"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	err = iClientList[2]->SetFileName(filename);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	err = iClientList[2]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(2, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete2);
	return EPass;
	}

TVerdict CTestStepDevSound0043::HandleStateInitComplete2()
	{
	TInt err = KErrNone;
	err = iClientList[2]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(3, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError3ErrInUse);
	return EPass;
	}
	
TVerdict CTestStepDevSound0043::HandleStatePlayError3ErrInUse()
	{
	RegisterClientEventToMonitor(3, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable3);
	SetExpectedEvent(2, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying2);
	return EPass;
	}

TVerdict CTestStepDevSound0043::HandleStateInitCompletePlaying2()
	{
	iClientList[2]->TestPlayData(2);
	//There is an issue in non a3f devsound where the notification is received before the PlayError. Therefore playback is stopped
	//to avoid getting a KErrUnderflow. This was not noticed before as the PlayData-BufferToBeFilled cycle was not correctly implemented
	//so KErrUnderflow was never received even when it was supposed to be received
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	iClientList[2]->TestPlayStop();
	#endif
	return EPass;
	}
	
TVerdict CTestStepDevSound0043::HandleStatePlayError2ErrUnderflow()
	{
	INFO_PRINTF1(_L("DevSoundClient2->PlayComplete"));
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Still monitor
		}
	return EPass;
	}

TVerdict CTestStepDevSound0043::HandleStateResourceAvailable1()
	{
	INFO_PRINTF1(_L("DevSoundClient1->Received Notification"));
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Still monitor
		}
	return EPass;
	}

TVerdict CTestStepDevSound0043::HandleStateResourceAvailable3()
	{
	INFO_PRINTF1(_L("DevSoundClient3->Received Notification"));
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Still monitor
		}
	return EPass;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0043::DoTestStepL(void)
	{
	iTestStepResult = EFail;
	InitializeTest();
	StartTestL();
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	return iTestStepResult;
	}
	
/**
 *
 * CTestStepDevSound0044
 *
 */
CTestStepDevSound0044::CTestStepDevSound0044()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0044-HP");
	//Low Priority
	iLowPriority.iPref = EMdaPriorityPreferenceNone;
	iLowPriority.iPriority = -20;
	iLowPriority.iState = EMMFStatePlaying;
	
	iMediumPriority.iPref = EMdaPriorityPreferenceNone;
	iMediumPriority.iPriority = 0;
	iMediumPriority.iState = EMMFStatePlaying;
	
	iHighPriority.iPref = EMdaPriorityPreferenceNone;
	iHighPriority.iPriority = 20;
	iHighPriority.iState = EMMFStatePlaying;
	}

void CTestStepDevSound0044::InitializeTest()
	{
	iStateHandlers[EStateInitComplete1] = &CTestStepDevSound0044::HandleStateInitComplete1;
	iStateHandlers[EStateInitComplete2] = &CTestStepDevSound0044::HandleStateInitComplete2;
	iStateHandlers[EStateInitComplete3] = &CTestStepDevSound0044::HandleStateInitComplete3;
	iStateHandlers[EStateInitCompletePlaying1] = &CTestStepDevSound0044::HandleStateInitCompletePlaying1;
	iStateHandlers[EStateInitCompletePlaying2] = &CTestStepDevSound0044::HandleStateInitCompletePlaying2;
	iStateHandlers[EStateInitCompletePlaying3] = &CTestStepDevSound0044::HandleStateInitCompletePlaying3;
	iStateHandlers[EStatePlayError2ErrUnderflow] = &CTestStepDevSound0044::HandleStatePlayError2ErrUnderflow;
	iStateHandlers[EStatePlayError1ErrInUse] = &CTestStepDevSound0044::HandleStatePlayError1ErrInUse;
	iStateHandlers[EStatePlayError3ErrInUse] = &CTestStepDevSound0044::HandleStatePlayError3ErrInUse;	
	iStateHandlers[EStateResourceAvailable3] = &CTestStepDevSound0044::HandleStateResourceAvailable3;
	iStateHandlers[EStateResourceAvailable1] = &CTestStepDevSound0044::HandleStateResourceAvailable1;
	}
	
/**
 * FsmL
 */
 void CTestStepDevSound0044::Fsm(TInt aClientNumber, TInt aEvent, TInt aError)
 	{	
 	if(FsmCheck(aClientNumber,aEvent,aError))
 		{ 		
 		iTestStepResult = (this->*(iStateHandlers[iState]))();
 		if(iTestStepResult != EPass)
 			{
 			StopTest();
 			}
 		}
 	}

void CTestStepDevSound0044::StartTestL()
	{
	TFileName filename;
	TInt err = KErrNone;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	
	INFO_PRINTF1(_L("Testing Register for notification"));			
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iClientList[1]->SetPrioritySettings(iLowPriority);
					
	INFO_PRINTF1(_L("Client 3 with medium priority"));
	iClientList[3]->SetPrioritySettings(iMediumPriority);

	INFO_PRINTF1(_L("Client 2 with high priority"));
	iClientList[2]->SetPrioritySettings(iHighPriority);
	
	INFO_PRINTF1(_L("Client 1 Initialize and register"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\garbage.raw");
	err = iClientList[1]->SetFileName(filename);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	
	err = iClientList[1]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete1);
	}

TVerdict CTestStepDevSound0044::HandleStateInitComplete1()
	{
	TInt err = KErrNone;
	err = iClientList[1]->TestPlayInit();
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying1);
	return EPass;
	}

TVerdict CTestStepDevSound0044::HandleStateInitCompletePlaying1()
	{
	TFileName filename;
	TInt err = KErrNone;
	TFourCC pcm8(' ','P','U','8');
	
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[1]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	
	INFO_PRINTF1(_L("Client 3 Initialize and register"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	
	err = iClientList[3]->SetFileName(filename);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
		
	// Initialize and play client 3
	err =  iClientList[3]->TestInitialize(pcm8, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(3, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete3);
	return EPass;
	}

TVerdict CTestStepDevSound0044::HandleStateInitComplete3()
	{
	TInt err = KErrNone;
	err = iClientList[3]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError1ErrInUse);
	return EPass;
	}

TVerdict CTestStepDevSound0044::HandleStatePlayError1ErrInUse()
	{
	RegisterClientEventToMonitor(1, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable1);
	SetExpectedEvent(3, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying3);
	return EPass;
	}

TVerdict CTestStepDevSound0044::HandleStateInitCompletePlaying3()
	{
	TInt err = KErrNone;
	TFileName filename;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	
	INFO_PRINTF1(_L("Client 3 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[3]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	INFO_PRINTF1(_L("Client 2 Initialize ,register and play"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	err = iClientList[2]->SetFileName(filename);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	err = iClientList[2]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(2, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete2);
	return EPass;
	}

TVerdict CTestStepDevSound0044::HandleStateInitComplete2()
	{
	TInt err = KErrNone;
	err = iClientList[2]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(3, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError3ErrInUse);
	return EPass;
	}
	
TVerdict CTestStepDevSound0044::HandleStatePlayError3ErrInUse()
	{
	RegisterClientEventToMonitor(3, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable3);
	SetExpectedEvent(2, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying2);
	return EPass;
	}

TVerdict CTestStepDevSound0044::HandleStateInitCompletePlaying2()
	{
	iClientList[2]->TestPlayData(2);
	//There is an issue in non a3f devsound where the notification is received before the PlayError. Therefore playback is stopped
	//to avoid getting a KErrUnderflow. This was not noticed before as the PlayData-BufferToBeFilled cycle was not correctly implemented
	//so KErrUnderflow was never received even when it was supposed to be received
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	iClientList[2]->TestPlayStop();
	#endif
	return EPass;
	}

TVerdict CTestStepDevSound0044::HandleStatePlayError2ErrUnderflow()
	{
	TInt err = KErrNone;
	INFO_PRINTF1(_L("iDevSoundClient2 -> PlayComplete"));
	if(!IsMonitoringFinished())
		{
		StateChange(EStateMonitoringEvents);
		}
	else
		{
		if(err != KErrNone)
			{
			iTestStepResult = EFail;
			return EFail;
			}
		}
	return EPass;
	}

TVerdict CTestStepDevSound0044::HandleStateResourceAvailable3()
	{
	INFO_PRINTF1(_L("iDevSoundClient3->Notification Received"));
	TInt err = KErrNone;
	err = iClientList[3]->TestWillResumePlay();
	if(err != KErrNone)
		{
		iTestStepResult = EFail;
		return EFail;
		}
	iCallBackTimer->After(10000000);
	if(!IsMonitoringFinished())
		{
		StateChange(EStateMonitoringEvents);
		}
	return EPass;
	}
	
TVerdict CTestStepDevSound0044::HandleStateResourceAvailable1()
	{
	if(IsMonitoringFinished())
		{
		INFO_PRINTF1(_L("DevSoundClient1->Notification is not Received"));
		iTestStepResult = EFail;
		}
	return iTestStepResult;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0044::DoTestStepL(void)
	{
	iTestStepResult = EFail;
	InitializeTest();
	StartTestL();
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
 *
 * CTestStepDevSound0045
 *
 */
CTestStepDevSound0045::CTestStepDevSound0045()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0045-HP");
	//Low Priority
	iLowPriority.iPref = EMdaPriorityPreferenceNone;
	iLowPriority.iPriority = -20;
	iLowPriority.iState = EMMFStatePlaying;
	
	iMediumPriority.iPref = EMdaPriorityPreferenceNone;
	iMediumPriority.iPriority = 0;
	iMediumPriority.iState = EMMFStatePlaying;
	
	iHighPriority.iPref = EMdaPriorityPreferenceNone;
	iHighPriority.iPriority = 20;
	iHighPriority.iState = EMMFStatePlaying;
	}

/**
 * FsmL
 */
 void CTestStepDevSound0045::Fsm(TInt aClientNumber, TInt aEvent, TInt aError)
 	{	
 	if(FsmCheck(aClientNumber,aEvent,aError))
 		{ 		
 		iTestStepResult = (this->*(iStateHandlers[iState]))();
 		if(iTestStepResult != EPass)
 			{
 			StopTest();
 			}
 		}
 	}
 
void CTestStepDevSound0045::InitializeTest()
	{
	iStateHandlers[EStateInitComplete1] = &CTestStepDevSound0045::HandleStateInitComplete1;
	iStateHandlers[EStateInitComplete2] = &CTestStepDevSound0045::HandleStateInitComplete2;
	iStateHandlers[EStateInitComplete3] = &CTestStepDevSound0045::HandleStateInitComplete3;
	iStateHandlers[EStateInitCompletePlaying1] = &CTestStepDevSound0045::HandleStateInitCompletePlaying1;
	iStateHandlers[EStateInitCompletePlaying2] = &CTestStepDevSound0045::HandleStateInitCompletePlaying2;
	iStateHandlers[EStateInitCompletePlaying3] = &CTestStepDevSound0045::HandleStateInitCompletePlaying3;
	iStateHandlers[EStatePlayError2ErrUnderflow] = &CTestStepDevSound0045::HandleStatePlayError2ErrUnderflow;
	iStateHandlers[EStatePlayError3ErrUnderflow] = &CTestStepDevSound0045::HandleStatePlayError3ErrUnderflow;
	iStateHandlers[EStatePlayError1ErrInUse] = &CTestStepDevSound0045::HandleStatePlayError1ErrInUse;
	iStateHandlers[EStatePlayError3ErrInUse] = &CTestStepDevSound0045::HandleStatePlayError3ErrInUse;	
	iStateHandlers[EStateResourceAvailable3] = &CTestStepDevSound0045::HandleStateResourceAvailable3;
	iStateHandlers[EStateResourceAvailable1] = &CTestStepDevSound0045::HandleStateResourceAvailable1;
	iStateHandlers[EStatePlayResumed3] = &CTestStepDevSound0045::HandleStatePlayResumed3;
	}
 	
void CTestStepDevSound0045::StartTestL()
	{
	TFileName filename;
	TInt result;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	
	INFO_PRINTF1(_L("Testing Register for notification"));			
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iClientList[1]->SetPrioritySettings(iLowPriority);
					
	INFO_PRINTF1(_L("Client 1 with medium priority"));
	iClientList[3]->SetPrioritySettings(iMediumPriority);

	INFO_PRINTF1(_L("Client 2 with high priority"));
	iClientList[2]->SetPrioritySettings(iHighPriority);
	
	INFO_PRINTF1(_L("Client 1 Initialize and register"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\garbage.raw");
	result = iClientList[1]->SetFileName(filename);
	if(result != KErrNone)
		{
		User::Leave(result);
		}
	
	result = iClientList[1]->TestInitialize(pcm16, EMMFStatePlaying);
	if(result != KErrNone)
		{
		User::Leave(result);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete1);
	}

TVerdict CTestStepDevSound0045::HandleStateInitComplete1()
	{
	TInt err = KErrNone;
	err = iClientList[1]->TestPlayInit();
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying1);
	return EPass;
	}
	
TVerdict CTestStepDevSound0045::HandleStateInitCompletePlaying1()
	{
	TFileName filename;
	TInt err;
	TFourCC pcm8(' ','P','U','8');
	
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[1]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	
	INFO_PRINTF1(_L("Client 3 Initialize and register"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	err = iClientList[3]->SetFileName(filename);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
		
	// Initialize and play client 1
	err =  iClientList[3]->TestInitialize(pcm8, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(3, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete3);
	return EPass;
	}

TVerdict CTestStepDevSound0045::HandleStateInitComplete3()
	{
	TInt err = KErrNone;
	err = iClientList[3]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError1ErrInUse);
	return EPass;
	}
	
TVerdict CTestStepDevSound0045::HandleStatePlayError1ErrInUse()
	{
	SetExpectedEvent(3, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying3);
	return EPass;
	}
	
TVerdict CTestStepDevSound0045::HandleStateInitCompletePlaying3()
	{
	TInt err = KErrNone;
	TFileName filename;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	
	INFO_PRINTF1(_L("Client 3 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[3]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	INFO_PRINTF1(_L("Client 2 Initialize ,register and play"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	err = iClientList[2]->SetFileName(filename);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	err = iClientList[2]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(2, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete2);
	return EPass;
	}

TVerdict CTestStepDevSound0045::HandleStateInitComplete2()
	{
	TInt err = KErrNone;
	err = iClientList[2]->TestPlayInit();
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(3, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError3ErrInUse);
	return EPass;
	}
	
TVerdict CTestStepDevSound0045::HandleStatePlayError3ErrInUse()
	{
	RegisterClientEventToMonitor(3, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable3);
	SetExpectedEvent(2, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying2);
	return EPass;
	}

TVerdict CTestStepDevSound0045::HandleStateInitCompletePlaying2()
	{
	iClientList[2]->TestPlayData(2);

	return EPass;
	}

TVerdict CTestStepDevSound0045::HandleStatePlayError2ErrUnderflow()
	{
	TInt err = KErrNone;
	INFO_PRINTF1(_L("iDevSoundClient2 -> PlayComplete"));
	if(!IsMonitoringFinished())
		{
		StateChange(EStateMonitoringEvents);
		}
	else
		{
		err = iClientList[3]->TestPlayInit();
		if(err != KErrNone)
			{
			iTestStepResult = EFail;
			return EFail;
			}
		SetExpectedEvent(3, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
		StateChange(EStatePlayResumed3);
		}
	return EPass;
	}

TVerdict CTestStepDevSound0045::HandleStateResourceAvailable3()
	{
	
	TInt err = KErrNone;
	INFO_PRINTF1(_L("iDevSoundclient3->Notification Received"));
	INFO_PRINTF1(_L("DevSoundClient3 Will resume Play"));
	err = iClientList[3]->TestWillResumePlay();
	if(err != KErrNone)
		{
		iTestStepResult = EFail;
		return EFail;
		}
	TBuf8<20> dataPlayed;
	err = iClientList[3]->TestGetResourceNotification(KMMFEventCategoryAudioResourceAvailable,dataPlayed);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	if(!IsMonitoringFinished())
		{
		StateChange(EStateMonitoringEvents);
		}
	else
		{
		err = iClientList[3]->TestPlayInit();
		if(err != KErrNone)
			{
			iTestStepResult = EFail;
			return EFail;
			}
		SetExpectedEvent(3, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
		StateChange(EStatePlayResumed3);
		}
	RegisterClientEventToMonitor(1, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable1);
	return EPass;
	}
	
TVerdict CTestStepDevSound0045::HandleStatePlayResumed3()
	{
	INFO_PRINTF1(_L("Client 3 is playing till eof"));
	iClientList[3]->TestPlayData(3);
	
	return EPass;
	}

TVerdict CTestStepDevSound0045::HandleStatePlayError3ErrUnderflow()
	{
	INFO_PRINTF1(_L("DevSoundClient3->PlayComplete"));
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Still monitor
		}
	return EPass;
	}

TVerdict CTestStepDevSound0045::HandleStateResourceAvailable1()
	{
	INFO_PRINTF1(_L("DevSoundClient1->Notification Received"));
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents);
		}
	return EPass;
	}
	
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0045::DoTestStepL(void)
	{
	iTestStepResult = EFail;
	InitializeTest();
	StartTestL();
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
 *
 * CTestStepDevSound0046
 *
 */
CTestStepDevSound0046::CTestStepDevSound0046()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0046-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0046::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Register for notification"));
	TVerdict result = EFail;
	//Low Priority
	TMMFPrioritySettings lowPriority;
	lowPriority.iPref = EMdaPriorityPreferenceNone;
	lowPriority.iPriority = 0;
	lowPriority.iState = EMMFStatePlaying;
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iDevSoundClient1->SetPrioritySettings(lowPriority);
	
	//High Priority
	TMMFPrioritySettings highPriority;
	highPriority.iPref = EMdaPriorityPreferenceNone;
	highPriority.iPriority = 20;
	highPriority.iState = EMMFStatePlaying;
	INFO_PRINTF1(_L("Client 2 with high priority"));
	iDevSoundClient2->SetPrioritySettings(highPriority);
	
	TFileName filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	result = iDevSoundClient1->SetFileName(filename);
	TFourCC pcm8(' ','P','U','8');
	// Initialize and play client 1
	INFO_PRINTF1(_L("Client 1 Initialize and play"));
	result =  iDevSoundClient1->TestDigitalPlayback(pcm8);
	if (result != EPass)
		{
		return EInconclusive;
		}
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	result = iDevSoundClient1->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (result != EPass)
		{
		return EInconclusive;
		}
	// Initialize and play client 2
	iDevSoundClient1->ResetCallbacks();	
	TFileName filename2 = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	result = iDevSoundClient2->SetFileName(filename2);
	TFourCC pcm16(KMMFFourCCCodePCM16);
	if (result != EPass)
		{
		return EInconclusive;
		}	
	INFO_PRINTF1(_L("Client 2 Initialize and play"));	
	result = iDevSoundClient2->TestDigitalPlayback(pcm16);
	if (result != EPass)
		{
		return EInconclusive;
		}

	INFO_PRINTF1(_L("Cancel registration of client 1 "));	
	result  = iDevSoundClient1->TestCancelRegister(KMMFEventCategoryAudioResourceAvailable);
	if (result != EPass)
		{
		return EInconclusive;
		}

	// check sendeventtoclient is called from devsoundobserver
	iDevSoundClient2->ResetCallbacks();
	INFO_PRINTF1(_L("Client 2 playing ..."));	
	
	while(!iDevSoundClient2->GetErrorStatus(EPlayError))
		{
		iDevSoundClient2->TestPlayData();
		}
	
	INFO_PRINTF1(_L("Getting status of Client 1"));	
	if(iDevSoundClient1->GetErrorStatus(ESendMsg))
		{
		INFO_PRINTF1(_L("Client 1 not received any notification"));
		return EFail;
		}
	return result;
	}

/**
 *
 * CTestStepDevSound0047
 *
 */
CTestStepDevSound0047::CTestStepDevSound0047()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0047-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0047::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Register for notification"));
	TVerdict result = EFail;
	//Low Priority
	TMMFPrioritySettings lowPriority;
	lowPriority.iPref = EMdaPriorityPreferenceNone;
	lowPriority.iPriority = 0;
	lowPriority.iState = EMMFStatePlaying;
	
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iDevSoundClient1->SetPrioritySettings(lowPriority);
	
	//High Priority
	TMMFPrioritySettings highPriority;
	highPriority.iPref = EMdaPriorityPreferenceNone;
	highPriority.iPriority = 20;
	highPriority.iState = EMMFStatePlaying;
	
	INFO_PRINTF1(_L("Client 2 with high priority"));
	iDevSoundClient2->SetPrioritySettings(highPriority);
	
	TFileName filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	result = iDevSoundClient1->SetFileName(filename);
	TFourCC pcm8(' ','P','U','8');
	// Initialize and play client 1
	INFO_PRINTF1(_L("Client 1 Initialize and play"));
	result =  iDevSoundClient1->TestDigitalPlayback(pcm8);
	if (result != EPass)
		{
		return EInconclusive;
		}
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	result = iDevSoundClient2->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (result != EPass)
		{
		return EInconclusive;
		}
	// Initialize and play client 2
	iDevSoundClient1->ResetCallbacks();	
	TFileName filename2 = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	result = iDevSoundClient2->SetFileName(filename2);
	TFourCC pcm16(KMMFFourCCCodePCM16);
	if (result != EPass)
		{
		return EInconclusive;
		}	
	INFO_PRINTF1(_L("Client 2 Initialize and play"));	
	result = iDevSoundClient2->TestDigitalPlayback(pcm16);
	if (result != EPass)
		{
		return EInconclusive;
		}
	// check sendeventtoclient is called from devsoundobserver
	iDevSoundClient2->ResetCallbacks();
	INFO_PRINTF1(_L("Client 2 playing ..."));	
	iDevSoundClient2->TestPlayData();
	while(!iDevSoundClient2->GetErrorStatus(EPlayError))
		{
		iDevSoundClient2->TestPlayData();
		}
	INFO_PRINTF1(_L("Getting status of Client 1"));	
	if(iDevSoundClient1->GetErrorStatus(ESendMsg))
		{
		INFO_PRINTF1(_L("Client 1 receieved any notification"));
		return EFail;
		}
	return result;
	}

/**
 *
 * CTestStepDevSound0048
 *
 */
CTestStepDevSound0048::CTestStepDevSound0048()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0048-HP");
	//Low Priority DevSound client1
	iLowPriority.iPref = EMdaPriorityPreferenceNone;
	iLowPriority.iPriority = 0;
	iLowPriority.iState = EMMFStatePlaying;
	//High Priority DevSoundClient2
	iHighPriority.iPref = EMdaPriorityPreferenceNone;
	iHighPriority.iPriority = 20;
	iHighPriority.iState = EMMFStatePlaying;
	}

void CTestStepDevSound0048::InitializeTest()
	{
	iStateHandlers[EStateInitComplete1] = &CTestStepDevSound0048::HandleStateInitComplete1;
	iStateHandlers[EStateInitComplete2] = &CTestStepDevSound0048::HandleStateInitComplete2;
	iStateHandlers[EStateInitCompletePlaying1] = &CTestStepDevSound0048::HandleStateInitCompletePlaying1;
	iStateHandlers[EStateInitCompletePlaying2] = &CTestStepDevSound0048::HandleStateInitCompletePlaying2;
	iStateHandlers[EStatePlayError2ErrUnderflow] = &CTestStepDevSound0048::HandleStatePlayError2ErrUnderflow;
	iStateHandlers[EStateResourceAvailable1] = &CTestStepDevSound0048::HandleStateResourceAvailable1;
	iStateHandlers[EStatePlayError1ErrInUse] = &CTestStepDevSound0048::HandleStatePlayError1ErrInUse;
	}

/**
 * FsmL
 */
 void CTestStepDevSound0048::Fsm(TInt aClientNumber, TInt aEvent, TInt aError)
 	{	
 	if(FsmCheck(aClientNumber,aEvent,aError))
 		{ 		
 		iTestStepResult = (this->*(iStateHandlers[iState]))();
 		if(iTestStepResult != EPass)
 			{
 			StopTest();
 			}
 		}
 	}

void CTestStepDevSound0048::StartTestL()
	{
	TFileName filename;
	TInt err;
	TFourCC pcm8(' ','P','U','8');
	//Low Priority DevSound client1
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iClientList[1]->SetPrioritySettings(iLowPriority);
	
	//High Priority DevSoundClient2
	INFO_PRINTF1(_L("Client 2 with high priority"));
	iClientList[2]->SetPrioritySettings(iHighPriority);
	
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	err = iClientList[1]->SetFileName(filename);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	err = iClientList[1]->TestInitialize(pcm8, EMMFStatePlaying);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete1);
	}

TVerdict CTestStepDevSound0048::HandleStateInitComplete1()
	{
	TInt err = KErrNone;
	err = iClientList[1]->TestPlayInit();
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying1);
	return EPass;
	}

TVerdict CTestStepDevSound0048::HandleStateInitCompletePlaying1()
	{
	TFileName filename;
	TInt err;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[1]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	err = iClientList[2]->SetFileName(filename);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	err = iClientList[2]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(2, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete2);
	return EPass;
	}

TVerdict CTestStepDevSound0048::HandleStateInitComplete2()
	{
	TInt err = KErrNone;
	err = iClientList[2]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError1ErrInUse);
	return EPass;
	}

TVerdict CTestStepDevSound0048::HandleStatePlayError1ErrInUse()
	{
	SetExpectedEvent(2, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
 	StateChange(EStateInitCompletePlaying2);
	RegisterClientEventToMonitor(1, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable1);
 	return EPass;
	}

TVerdict CTestStepDevSound0048::HandleStateInitCompletePlaying2()
	{
	iClientList[2]->TestPlayData(2);
	return EPass;
	}
	
TVerdict CTestStepDevSound0048::HandleStatePlayError2ErrUnderflow()
	{
	INFO_PRINTF1(_L("DevSoundClient2->PlayComplete"));
	TInt err = KErrNone;
	if(IsMonitoringFinished())
		{
		INFO_PRINTF1(_L("cancel registration of client 1 "));	
		err = iClientList[1]->TestCancelRegister(KMMFEventCategoryAudioResourceAvailable);
		if( err != KErrNone)
			{
			iTestStepResult = EFail;
			return EFail;
			}
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Still monitor
		}
	return EPass;
	}
	
TVerdict CTestStepDevSound0048::HandleStateResourceAvailable1()
	{
	INFO_PRINTF1(_L("DevSoundClient1->Notification Received"));
	TInt err = KErrNone;
	if(IsMonitoringFinished())
		{
		INFO_PRINTF1(_L("cancel registration of client 1 "));	
		err = iClientList[1]->TestCancelRegister(KMMFEventCategoryAudioResourceAvailable);
		if(err != KErrNone)
			{
			iTestStepResult = EFail;
			return EFail;
			}
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents);
		}
	return EPass;
	}
	
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0048::DoTestStepL(void)
	{
	iTestStepResult = EFail;
	InitializeTest();
	StartTestL();
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
 *
 * CTestStepDevSound0049
 *
 */
CTestStepDevSound0049::CTestStepDevSound0049()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0049-HP");
	//Low Priority DevSound client1
	iLowPriority.iPref = EMdaPriorityPreferenceNone;
	iLowPriority.iPriority = 0;
	iLowPriority.iState = EMMFStatePlaying;
	//High Priority DevSoundClient2
	iHighPriority.iPref = EMdaPriorityPreferenceNone;
	iHighPriority.iPriority = 20;
	iHighPriority.iState = EMMFStatePlaying;
	}

void CTestStepDevSound0049::InitializeTest()
	{
	iStateHandlers[EStateInitComplete1] = &CTestStepDevSound0049::HandleStateInitComplete1;
	iStateHandlers[EStateInitComplete2] = &CTestStepDevSound0049::HandleStateInitComplete2;
	iStateHandlers[EStateInitCompletePlaying1] = &CTestStepDevSound0049::HandleStateInitCompletePlaying1;
	iStateHandlers[EStateInitCompletePlaying2] = &CTestStepDevSound0049::HandleStateInitCompletePlaying2;
	iStateHandlers[EStatePlayError2ErrUnderflow] = &CTestStepDevSound0049::HandleStatePlayError2ErrUnderflow;
	iStateHandlers[EStateResourceAvailable1] = &CTestStepDevSound0049::HandleStateResourceAvailable1;
	iStateHandlers[EStatePlayError1ErrInUse] = &CTestStepDevSound0049::HandleStatePlayError1ErrInUse;
	}
	
/**
 * FsmL
 */
 void CTestStepDevSound0049::Fsm(TInt aClientNumber, TInt aEvent, TInt aError)
 	{	
 	if(FsmCheck(aClientNumber,aEvent,aError))
 		{ 		
 		iTestStepResult = (this->*(iStateHandlers[iState]))();
 		if(iTestStepResult != EPass)
 			{
 			StopTest();
 			}
 		}
 	}

void CTestStepDevSound0049::StartTestL()
	{
	TFileName filename;
	TInt err = KErrNone;
	TFourCC pcm8(' ','P','U','8');
	INFO_PRINTF1(_L("Testing Register for notification"));
	
	//Low Priority DevSound client1
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iClientList[1]->SetPrioritySettings(iLowPriority);
	
	//High Priority DevSoundClient2
	INFO_PRINTF1(_L("Client 2 with high priority"));
	iClientList[2]->SetPrioritySettings(iHighPriority);
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	err = iClientList[1]->SetFileName(filename);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	err = iClientList[1]->TestInitialize(pcm8, EMMFStatePlaying);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete1);
	}

TVerdict CTestStepDevSound0049::HandleStateInitComplete1()
	{
	TInt err = KErrNone;
	err = iClientList[1]->TestPlayInit();
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying1);
	return EPass;
	}

TVerdict CTestStepDevSound0049::HandleStateInitCompletePlaying1()
	{
	TFileName filename;
	TInt err = KErrNone;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[1]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	err = iClientList[2]->SetFileName(filename);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	err = iClientList[2]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(2, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete2);
	return EPass;
	}

TVerdict CTestStepDevSound0049::HandleStateInitComplete2()
	{
	TInt err = KErrNone;
	err = iClientList[2]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError1ErrInUse);
	return EPass;
	}
	
TVerdict CTestStepDevSound0049::HandleStatePlayError1ErrInUse()
	{
	SetExpectedEvent(2, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying2);
	RegisterClientEventToMonitor(1, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable1);
	return EPass;
	}

TVerdict CTestStepDevSound0049::HandleStateInitCompletePlaying2()
	{
	iClientList[2]->TestPlayData(2);
	return EPass;
	}
	
TVerdict CTestStepDevSound0049::HandleStatePlayError2ErrUnderflow()
	{
	INFO_PRINTF1(_L("DevSoundClient2->PlayComplete"));
	TInt err = KErrNone;
	if(IsMonitoringFinished())
		{
		INFO_PRINTF1(_L("cancel registration of client 1 "));	
		err = iClientList[1]->TestCancelRegister(KMMFEventCategoryAudioResourceAvailable);
		if(err != KErrNone)
			{
			iTestStepResult = EFail;
			return EFail;
			}
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Still monitor
		}
	return EPass;
	}
	
TVerdict CTestStepDevSound0049::HandleStateResourceAvailable1()
	{
	INFO_PRINTF1(_L("DevSoundClient1->Notification Received"));
	TInt err = KErrNone;
	if(IsMonitoringFinished())
		{
		INFO_PRINTF1(_L("cancel registration of client 1 "));	
		err = iClientList[1]->TestCancelRegister(KMMFEventCategoryAudioResourceAvailable);
		if(err != KErrNone)
			{
			iTestStepResult = EFail;
			return EFail;
			}
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents);
		}
	return EPass;
	}
	
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0049::DoTestStepL(void)
	{
	iTestStepResult = EFail;
	InitializeTest();
	StartTestL();
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	return iTestStepResult;
	}
	
/**
 *
 * CTestStepDevSound0044
 *
 */
CTestStepDevSound0050::CTestStepDevSound0050()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0050-HP");
	//Low Priority
	iLowPriority.iPref = EMdaPriorityPreferenceNone;
	iLowPriority.iPriority = -20;
	iLowPriority.iState = EMMFStatePlaying;
	
	iMediumPriority.iPref = EMdaPriorityPreferenceNone;
	iMediumPriority.iPriority = 0;
	iMediumPriority.iState = EMMFStatePlaying;
	
	iHighPriority.iPref = EMdaPriorityPreferenceNone;
	iHighPriority.iPriority = 20;
	iHighPriority.iState = EMMFStatePlaying;
	}

void CTestStepDevSound0050::InitializeTest()
	{
	iStateHandlers[EStateInitComplete1] = &CTestStepDevSound0050::HandleStateInitComplete1;
	iStateHandlers[EStateInitComplete2] = &CTestStepDevSound0050::HandleStateInitComplete2;
	iStateHandlers[EStateInitComplete3] = &CTestStepDevSound0050::HandleStateInitComplete3;
	iStateHandlers[EStateInitCompletePlaying1] = &CTestStepDevSound0050::HandleStateInitCompletePlaying1;
	iStateHandlers[EStateInitCompletePlaying2] = &CTestStepDevSound0050::HandleStateInitCompletePlaying2;
	iStateHandlers[EStateInitCompletePlaying3] = &CTestStepDevSound0050::HandleStateInitCompletePlaying3;
	iStateHandlers[EStatePlayError1ErrInUse] = &CTestStepDevSound0050::HandleStatePlayError1ErrInUse;
	iStateHandlers[EStatePlayError3ErrInUse] = &CTestStepDevSound0050::HandleStatePlayError3ErrInUse;
	iStateHandlers[EStatePlayError2ErrUnderflow] = &CTestStepDevSound0050::HandleStatePlayError2ErrUnderflow;
	iStateHandlers[EStateResourceAvailable3] = &CTestStepDevSound0050::HandleStateResourceAvailable3;
	iStateHandlers[EStateResourceAvailable1] = &CTestStepDevSound0050::HandleStateResourceAvailable1;
	}
	
/**
 * FsmL
 */
 void CTestStepDevSound0050::Fsm(TInt aClientNumber, TInt aEvent, TInt aError)
 	{	
 	if(FsmCheck(aClientNumber,aEvent,aError))
 		{ 		
 		iTestStepResult = (this->*(iStateHandlers[iState]))();
 		if(iTestStepResult != EPass)
 			{
 			StopTest();
 			}
 		}
 	}

void CTestStepDevSound0050::StartTestL()
	{
	TFileName filename;
	TInt err = KErrNone;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	
	INFO_PRINTF1(_L("Testing Register for notification"));			
	INFO_PRINTF1(_L("Client 1 with low priority"));
	iClientList[1]->SetPrioritySettings(iLowPriority);
					
	INFO_PRINTF1(_L("Client 3 with medium priority"));
	iClientList[3]->SetPrioritySettings(iMediumPriority);

	INFO_PRINTF1(_L("Client 2 with high priority"));
	iClientList[2]->SetPrioritySettings(iHighPriority);
	
	INFO_PRINTF1(_L("Client 1 Initialize and register"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\garbage.raw");
	err = iClientList[1]->SetFileName(filename);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	
	err = iClientList[1]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete1);
	}

TVerdict CTestStepDevSound0050::HandleStateInitComplete1()
	{
	TInt err = KErrNone;
	err = iClientList[1]->TestPlayInit();
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	SetExpectedEvent(1,CFsmDevSoundClient::EBufferToBeFilled,KErrNone);
	StateChange(EStateInitCompletePlaying1);
	return EPass;
	}

TVerdict CTestStepDevSound0050::HandleStateInitCompletePlaying1()
	{
	TFileName filename;
	TInt err = KErrNone;
	TFourCC pcm8(' ','P','U','8');
	
	INFO_PRINTF1(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[1]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	
	INFO_PRINTF1(_L("Client 3 Initialize and register"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
	
	err = iClientList[3]->SetFileName(filename);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
		
	// Initialize and play client 3
	err =  iClientList[3]->TestInitialize(pcm8, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(3, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete3);
	return EPass;
	}

TVerdict CTestStepDevSound0050::HandleStateInitComplete3()
	{
	TInt err = KErrNone;
	err = iClientList[3]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(1, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError1ErrInUse);
	return EPass;
	}

TVerdict CTestStepDevSound0050::HandleStatePlayError1ErrInUse()
	{
	SetExpectedEvent(3, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying3);
	return EPass;
	}

TVerdict CTestStepDevSound0050::HandleStateInitCompletePlaying3()
	{
	TInt err = KErrNone;
	TFileName filename;
	TFourCC pcm16(KMMFFourCCCodePCM16);
	
	INFO_PRINTF1(_L("Client 3 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
	err = iClientList[3]->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	INFO_PRINTF1(_L("Client 2 Initialize ,register and play"));
	filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
	err = iClientList[2]->SetFileName(filename);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
	err = iClientList[2]->TestInitialize(pcm16, EMMFStatePlaying);
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(2, CFsmDevSoundClient::EInitComplete, KErrNone);
	StateChange(EStateInitComplete2);
	return EPass;
	}

TVerdict CTestStepDevSound0050::HandleStateInitComplete2()
	{
	TInt err = KErrNone;
	err = iClientList[2]->TestPlayInit();
	if(err != KErrNone)
		{
		return EInconclusive;
		}
	SetExpectedEvent(3, CFsmDevSoundClient::EPlayError, KErrInUse);
	StateChange(EStatePlayError3ErrInUse);
	return EPass;
	}
	
TVerdict CTestStepDevSound0050::HandleStatePlayError3ErrInUse()
	{
	RegisterClientEventToMonitor(3, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable3);
	RegisterClientEventToMonitor(1, CFsmDevSoundClient::EResourceAvailable, KErrNone, EStateResourceAvailable1);
	SetExpectedEvent(2, CFsmDevSoundClient::EBufferToBeFilled, KErrNone);
	StateChange(EStateInitCompletePlaying2);
	return EPass;
	}

TVerdict CTestStepDevSound0050::HandleStateInitCompletePlaying2()
	{
	iClientList[2]->TestPlayData(2);
	//There is an issue in non a3f devsound where the notification is received before the PlayError. Therefore playback is stopped
	//to avoid getting a KErrUnderflow. This was not noticed before as the PlayData-BufferToBeFilled cycle was not correctly implemented
	//so KErrUnderflow was never received even when it was supposed to be received
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	iClientList[2]->TestPlayStop();
	#endif
	return EPass;
	}

TVerdict CTestStepDevSound0050::HandleStatePlayError2ErrUnderflow()
	{
	INFO_PRINTF1(_L("DevSoundClient2->PlayComplete"));
	TInt err = KErrNone;
	if(IsMonitoringFinished())
		{
		if(err != KErrNone)
			{
			iTestStepResult = EFail;
			return EFail;
			}
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Still monitor
		}
	return EPass;
	}

TVerdict CTestStepDevSound0050::HandleStateResourceAvailable3()
	{
	INFO_PRINTF1(_L("iDevSoundClient3->Notification Received"));
	TInt err = KErrNone;
	err = iClientList[3]->TestWillResumePlay();
	if(err != KErrNone)
		{
		iTestStepResult = EFail;
		return EFail;
		}
	err = iClientList[3]->TestCancelRegister(KMMFEventCategoryAudioResourceAvailable);
	if(err != KErrNone)
		{
		iTestStepResult = EFail;
		return EFail;
		}
	iCallBackTimer->After(10000000);
	if(!IsMonitoringFinished())
		{
		StateChange(EStateMonitoringEvents);
		}
	return EPass;
	}
	
TVerdict CTestStepDevSound0050::HandleStateResourceAvailable1()
	{
	if(IsMonitoringFinished())
		{
		INFO_PRINTF1(_L("DevSoundClient1->Notification is not Received"));
		iTestStepResult = EFail;
		}
	return iTestStepResult;
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0050::DoTestStepL(void)
	{
	iTestStepResult = EFail;
	InitializeTest();
	StartTestL();
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
 *
 * CTestStepDevSound0051
 *
 */
CTestStepDevSound0051::CTestStepDevSound0051()
	{
	iTestStepName = _L("MM-MMF-DEVSOUNDNOTIFICATION-U-0051-HP");
	}

/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */
TVerdict CTestStepDevSound0051::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("Testing Register for notification on Nokia adaption, KErrNotSupported will be returned"));
	TVerdict result = EFail;
	result = iDevSoundClient1->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
	if (result == EPass)
		{
		return EFail;
		}
	return EPass;
	}

/**
 *
 * NewL
 * CEventNotification*
 *
 */
CEventNotification* CEventNotification::NewL()
	{
	CEventNotification* self = new(ELeave) CEventNotification();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
 	}
 
/**
 *
 * CEventNotification
 * Initilaization List
 *
 */	
CEventNotification::CEventNotification():CActive(EPriorityIdle)
	{
	CActiveScheduler::Add(this);
 	}
 	
/**
 *
 * CEventNotification
 * Destructor
 *
 */	
CEventNotification::~CEventNotification()
	{
	delete iLog;
	if(iAsyncCallBackTimer)
		{
		delete iAsyncCallBackTimer;
		iAsyncCallBackTimer = NULL;
		}
 	}

/**
 *
 * DoCancel
 * @result void
 *
 */ 	
void CEventNotification::DoCancel()
 	{
 	}
 	
/**
 *
 * Start
 * @result void
 *
 */ 	
void CEventNotification::Start()
	{
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS, KErrNone);
	SetActive();
	}
/**
 * TimerComplete
 * @result void
 */

void CEventNotification::TimerComplete()
	{
	if(iEventStatus == EClient2Delete)
		{
		iEventStatus = EClient1Status;
		}
	else
		{
		iEventStatus = EClient2Delete;
		}		
	Start();
	}
/**
 *
 * ConstructL
 * @result void
 *
 */ 		
void CEventNotification::ConstructL()
	{
	iDevSoundClient1 = CDevSoundClient1::NewL(this);
	iDevSoundClient2 = CDevSoundClient2::NewL(this);
	iLog = CLog::NewL();
	iLog->OpenLogFileL();
	iTestResult = EFail;
	iAsyncCallBackTimer = CCallBackTimer::NewL(TCallBack(AsyncCallBackTimer, this));	
	}

/**
 *
 * RunL
 * @result void
 *
 */ 	
void CEventNotification::RunL()
	{
	switch(iEventStatus)
		{
		case EClient1Init:
			{
			TMMFPrioritySettings lowPriority;
			lowPriority.iPref = EMdaPriorityPreferenceNone;
			lowPriority.iPriority = 0;
			lowPriority.iState = EMMFStatePlaying;
			iLog->Log(_L("Client 1 with low priority"));
			iDevSoundClient1->SetPrioritySettings(lowPriority);
			TFileName filename = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
			iTestResult = iDevSoundClient1->SetFileName(filename);
			TFourCC pcm8(' ','P','U','8');
			iLog->Log(_L("Client 1 Initialize and play"));
			iTestResult =  iDevSoundClient1->TestDigitalPlayback(pcm8);
			if (iTestResult != EPass)
				{
				return ;
				}
			iLog->Log(_L("Client 1 Register for notification for the event - KMMFEventCategoryAudioResourceAvailable"));
			iTestResult = iDevSoundClient1->TestRegisterAsClient(KMMFEventCategoryAudioResourceAvailable);
			if (iTestResult != EPass)
				{
				return;
				}
			iEventStatus = EClient2Init;
			Start();
			break;
			}
		case EClient2Init:
			{
			//High Priority
			TMMFPrioritySettings highPriority;
			highPriority.iPref = EMdaPriorityPreferenceNone;
			highPriority.iPriority = 20;
			highPriority.iState = EMMFStatePlaying;
			iLog->Log(_L("Client 2 with high priority"));
			iDevSoundClient2->SetPrioritySettings(highPriority);
			// Initialize and play client 2
			iDevSoundClient1->ResetCallbacks();	
			TFileName filename2 = _L("C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
			iTestResult = iDevSoundClient2->SetFileName(filename2);
			if (iTestResult != EPass)
				{
				return ;
				}
			TFourCC pcm16(KMMFFourCCCodePCM16);
			iTestResult = iDevSoundClient2->TestDigitalPlayback(pcm16);
			if (iTestResult != EPass)
				{
				return ;
				}
			iEventStatus =	EClient2Idle;
			Start();
			break;
			}
		case EClient2Playing:
			{
			iLog->Log(_L("Client 2 Initialize and play"));	
			TFourCC pcm16(KMMFFourCCCodePCM16);
			iDevSoundClient2->TestPlayData();
			iEventStatus = EAddDelayClient2;
			Start();
			break;
			}
		case EClient2Delete:
			{
			iLog->Log(_L("Client 2 deleted "));	
			iDevSoundClient2->CloseClient();
			iAsyncCallBackTimer->After(1000000);
			break;
			}	
		case EClient1Status:
			{
			iLog->Log(_L("Getting status of client 1"));	
			if(iDevSoundClient1->GetErrorStatus(ESendMsg))			
				{
				iLog->Log(_L("Client 1 received notification"));
				iTestResult = EPass;
				}
			else
				{
			 	iLog->Log(_L("Client 1 NOT received any notification"));		
			 	iTestResult = EFail;
			 	}
			CActiveScheduler::Stop();
			break;
			}
		case EClient2Idle:
			{
			iEventStatus = EClient2Idle;	
			if(iDevSoundClient2->GetErrorStatus(EBuffToFill))
				{
				iEventStatus = EClient2Playing;	
				}
			Start();
			break;
			}
		case EAddDelayClient2:
			{
			iLog->Log(_L("Delay of 1 second"));	
			iAsyncCallBackTimer->After(1000000);
			break;
			}
		default:
			break;
		}
	}
	
/**
 *
 * GetStatus
 * @result TRequestStatus
 *
 */ 	
TRequestStatus* CEventNotification::GetStatus()
	{
	return &(this->iStatus); 
	}

/**
 *
 * GetResult
 * @result TVerdict
 *
 */ 
TVerdict CEventNotification::GetResult()
	{
	return iTestResult;
	}
