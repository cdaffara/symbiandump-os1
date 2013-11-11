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
// CPP file: Audio Resource Notification tests for Audio Output Stream.
// 
//

/**
 @file TestOutputStreamARN.cpp
*/

#include "TestAudioClientUtilitiesARN.h"

const TInt KSampleRate = 8000;
const TInt KBytesPerSample = 2; 
//------------------------------------------------------------------------------
//********************PREQ 797 - Audio Resource Notification********************
//------------------------------------------------------------------------------

//
//CTestMdaAudioOutputStreamCallbackSupport
//

/**
 * NewL
 */
CTestMdaAudioOutputStreamCallbackSupport* CTestMdaAudioOutputStreamCallbackSupport::NewL(MTestAudioOutputStreamCallback& aCallback)
	{
	return new(ELeave) CTestMdaAudioOutputStreamCallbackSupport(aCallback);
	}

/**
 * MaoscOpenComplete
 */
void CTestMdaAudioOutputStreamCallbackSupport::MaoscOpenComplete(TInt aError)
	{
	iCallback.MaoscOpenCompleteTest(aError, *this);
	}

/**
 * MaoscBufferCopied
 */
void CTestMdaAudioOutputStreamCallbackSupport::MaoscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{
	iCallback.MaoscBufferCopiedTest(aError, aBuffer, *this);
	}

/**
 * MaoscPlayComplete
 */
void CTestMdaAudioOutputStreamCallbackSupport::MaoscPlayComplete(TInt aError)
	{
	iCallback.MaoscPlayCompleteTest(aError, *this);
	}

/**
 * MarncResourceAvailable
 */
void CTestMdaAudioOutputStreamCallbackSupport::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	iCallback.MarncResourceAvailableTest(aNotificationEventId, aNotificationData, *this);
	}

/**
 * Constructor
 */
CTestMdaAudioOutputStreamCallbackSupport::CTestMdaAudioOutputStreamCallbackSupport(MTestAudioOutputStreamCallback& aCallback):iCallback(aCallback)
	{

	}

//
//CTestStepAudioOutputStreamARN
//

/**
 * Constructor
 */
CTestStepAudioOutputStreamARN::CTestStepAudioOutputStreamARN(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	iFileSize1 = 0;
	iFileSize2 = 0;
	iHeapSize = 3600000;
	}

/**
 * MMdaAudioOutputStreamCallback->MaoscOpenComplete
 */
void CTestStepAudioOutputStreamARN::MaoscOpenCompleteTest(TInt aError, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport)
	{
	if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport1)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream1::MMdaAudioOutputStreamCallback->MaoscOpenComplete (%d)"),aError);
		TRAP(iError, FsmL(EMaoscOpenComplete1, aError));
		}
	else if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport2)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream2::MMdaAudioOutputStreamCallback->MaoscOpenComplete (%d)"),aError);
		TRAP(iError, FsmL(EMaoscOpenComplete2, aError));
		}
	else if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport3)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream3::MMdaAudioOutputStreamCallback->MaoscOpenComplete (%d)"),aError);
		TRAP(iError, FsmL(EMaoscOpenComplete3, aError));
		}
	}

/**
 * MMdaAudioOutputStreamCallback->MaoscBufferCopied
 */
void CTestStepAudioOutputStreamARN::MaoscBufferCopiedTest(TInt aError, const TDesC8& /*aBuffer*/, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport)
	{
	if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport1)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream1::MMdaAudioOutputStreamCallback->MaoscBufferCopied (%d)"),aError);
		TRAP(iError, FsmL(EMaoscBufferCopied1, aError));
		}
	else if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport2)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream2::MMdaAudioOutputStreamCallback->MaoscBufferCopied (%d)"),aError);
		TRAP(iError, FsmL(EMaoscBufferCopied2, aError));
		}
	else if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport3)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream3::MMdaAudioOutputStreamCallback->MaoscBufferCopied (%d)"),aError);
		TRAP(iError, FsmL(EMaoscBufferCopied3, aError));
		}
	}

/**
 * MMdaAudioOutputStreamCallback->MaoscPlayComplete
 */
void CTestStepAudioOutputStreamARN::MaoscPlayCompleteTest(TInt aError, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport)
	{
	if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport1)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream1::MMdaAudioOutputStreamCallback->MaoscPlayComplete (%d)"),aError);
		TRAP(iError, FsmL(EMaoscPlayComplete1, aError));
		}
	else if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport2)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream2::MMdaAudioOutputStreamCallback->MaoscPlayComplete (%d)"),aError);
		TRAP(iError, FsmL(EMaoscPlayComplete2, aError));
		}
	else if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport3)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream3::MMdaAudioOutputStreamCallback->MaoscPlayComplete (%d)"),aError);
		TRAP(iError, FsmL(EMaoscPlayComplete3, aError));
		}
	}

/**
 * MMMFAudioResourceNotificationCallback->MarncResourceAvailable
 */
void CTestStepAudioOutputStreamARN::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestMdaAudioOutputStreamCallbackSupport& aMdaAudioOutputStreamCallbackSupport)
	{
	if (aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		INFO_PRINTF1(_L("iAudioOutputStream::MMMFAudioResourceNotificationCallback -> KMMFEventCategoryAudioResourceAvailable"));

		//Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg timeIntvbuf;
		timeIntvbuf.Copy(aNotificationData);
		TTimeIntervalMicroSeconds intPos = timeIntvbuf();
		iPosition = intPos;

		if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport1)
			{
			INFO_PRINTF1(_L("iTestAudioOutputStream1 -> MMMFAudioResourceNotificationCallback NotificationReceived"));
			TRAP(iError, FsmL(EMarncResourceAvailableOS1, KErrNone));
			}
		else if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport2)
			{
			INFO_PRINTF1(_L("iTestAudioOutputStream2 -> MMMFAudioResourceNotificationCallback NotificationReceived"));
			TRAP(iError, FsmL(EMarncResourceAvailableOS2, KErrNone));
			}
		else if (&aMdaAudioOutputStreamCallbackSupport == iMdaAudioOutputStreamCallbackSupport3)
			{
			INFO_PRINTF1(_L("iTestAudioOutputStream3 -> MMMFAudioResourceNotificationCallback NotificationReceived"));
			TRAP(iError, FsmL(EMarncResourceAvailableOS3, KErrNone));
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFAudioResourceNotificationCallback, but NOT KMMFEventCategoryAudioResourceAvailable"));
		}
	}

/**
 * DoTestStepPreambleL
 */
TVerdict CTestStepAudioOutputStreamARN::DoTestStepPreambleL()
	{
	// Local Variable to hold verdict
	TVerdict verdict;
	
	// Install the scheduler
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();
	
	if (verdict == EFail)
		{
		return verdict;
		}

	// Connect RFs session
	TInt err = iFs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting to File session (RFs) = %d"), err);
		return EFail;
		}

	// Initialise iTestAudioOutputStream1
	INFO_PRINTF1(_L("Initialise CMdaAudioOutputStream1 : EMdaPriorityNormal"));
	iMdaAudioOutputStreamCallbackSupport1 = CTestMdaAudioOutputStreamCallbackSupport::NewL(*this);
	TRAP(err, iTestAudioOutputStream1 = CMdaAudioOutputStream::NewL(*iMdaAudioOutputStreamCallbackSupport1, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
	if (err != KErrNone || iTestAudioOutputStream1 == NULL )
		{
		INFO_PRINTF2(_L("Error Creating iTestAudioOutputStream1 = %d"), err);
		return EInconclusive;
		}

	// Open RFile#1
	err = iFile1.Open(iFs, KStreamRawFile1, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening iFile1 = %d"), err);
		return EFail;
		}
	
	// Get size of iFile1
	err = iFile1.Size(iFileSize1);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile1 = %d"), err);
		return EFail;
		}
	
	// Initialise iBuf1
	iBuf1.CreateMaxL(iFileSize1);
	
	// Initialise iTestAudioOutputStream2
	INFO_PRINTF1(_L("Initialise CMdaAudioOutputStream2 : EMdaPriorityMax"));
	iMdaAudioOutputStreamCallbackSupport2 = CTestMdaAudioOutputStreamCallbackSupport::NewL(*this);
	TRAP(err, iTestAudioOutputStream2 = CMdaAudioOutputStream::NewL(*iMdaAudioOutputStreamCallbackSupport2, EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality));
	if (err != KErrNone || iTestAudioOutputStream2 == NULL )
		{
		INFO_PRINTF2(_L("Error Creating iTestAudioOutputStream2 = %d"), err);
		return EInconclusive;
		}

	// Open RFile#2
	err = iFile2.Open(iFs, KStreamRawFile2, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening iFile2 = %d"), err);
		return EFail;
		}
	
	// Get size of iFile2
	err = iFile2.Size(iFileSize2);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile2 = %d"), err);
		return EFail;
		}
	
	// Initialise iBuf2
	iBuf2.CreateMaxL(iFileSize2);
	
	// Initialise iTestAudioOutputStream3
	INFO_PRINTF1(_L("Initialise CMdaAudioOutputStream3 : EMdaPriorityMin"));
	iMdaAudioOutputStreamCallbackSupport3 = CTestMdaAudioOutputStreamCallbackSupport::NewL(*this);
	TRAP(err, iTestAudioOutputStream3 = CMdaAudioOutputStream::NewL(*iMdaAudioOutputStreamCallbackSupport3, EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality));
	if (err != KErrNone || iTestAudioOutputStream3 == NULL )
		{
		INFO_PRINTF2(_L("Error Creating iTestAudioOutputStream3 = %d"), err);
		return EInconclusive;
		}

	// Open RFile#3
	err = iFile3.Open(iFs, KStreamRawFile3, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening iFile3 = %d"), err);
		return EFail;
		}
	
	// Get size of iFile3
	err = iFile3.Size(iFileSize3);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting size of iFile3 = %d"), err);
		return EFail;
		}
	
	// Initialise iBuf3
	iBuf3.CreateMaxL(iFileSize3);
	
	// Initialise the CCallBackTimer.
	iCallBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	
	return verdict;
	}

/**
 * DoTestStepPostambleL
 */
TVerdict CTestStepAudioOutputStreamARN::DoTestStepPostambleL()
	{
	// Delete all the iTestAudioOutputStream utilities.
	delete iTestAudioOutputStream1;
	iTestAudioOutputStream1=NULL;
	delete iTestAudioOutputStream2;
	iTestAudioOutputStream2=NULL;
	delete iTestAudioOutputStream3;
	iTestAudioOutputStream3=NULL;
	// Destroy File Handles
	iFile1.Close();
	iFile2.Close();
	iFile3.Close();
	iFs.Close();
	// Delete RBuf8
	iBuf1.Close();
	iBuf2.Close();
	iBuf3.Close();
	// Delete CTestMdaAudioOutputStreamCallbackSupport
	delete iMdaAudioOutputStreamCallbackSupport1;
	delete iMdaAudioOutputStreamCallbackSupport2;
	delete iMdaAudioOutputStreamCallbackSupport3;
	// Delete the CCallBackTimer.
	delete iCallBackTimer;
	iCallBackTimer=NULL;
	// Destroy the scheduler
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * DoTestStepL
 */
TVerdict CTestStepAudioOutputStreamARN::DoTestStepL()
	{
	return( DoTestL() );
	}

/**
 * StateInit
 */
void CTestStepAudioOutputStreamARN::StateInit()
	{
	iExpectedEvent=EIdleOutputStream;
	iExpectedError=KErrNone;
	iState=0;
	}

/**
 * StateChange
 */
void CTestStepAudioOutputStreamARN::StateChange(TMmfAudioOutputStreamEvents aExpectedEvent, TInt aExpectedError)
	{
	iExpectedEvent=aExpectedEvent;
	iExpectedError=aExpectedError;
	iState++;
	}
	
/**
 * FsmCheck
 */
TBool CTestStepAudioOutputStreamARN::FsmCheck(TMmfAudioOutputStreamEvents aEventCode, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if (iExpectedEvent != aEventCode)
		{
		ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedEvent, aEventCode);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else if (iExpectedError != aError)
		{
		ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, aError);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;	
	}

CTestStepAudioOutputStreamARN2::CTestStepAudioOutputStreamARN2(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	iHeapSize = 3600000;
	}
	
/**
 * DoTestStepPreambleL
 */
TVerdict CTestStepAudioOutputStreamARN2::DoTestStepPreambleL()
	{
	TVerdict verdict;
	verdict = CTestMmfAclntStep::DoTestStepPreambleL();
	if (verdict == EFail)
		{
		return verdict;
		}
		
	iClientList[1] = CFsmAudioOutputStreamClient::NewL(1, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality, this);
	iClientList[2] = CFsmAudioOutputStreamClient::NewL(2, EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality, this);
	iClientList[3] = CFsmAudioOutputStreamClient::NewL(3, EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality, this);

	// Connect RFs session
	TInt err = iFs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error connecting to File session (RFs) = %d"), err);
		return EFail;
		}

	err = iFileHandle[1].Open(iFs, KStreamRawFile1, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening iFile1 = %d"), err);
		return EFail;
		}
	
	err = iFileHandle[2].Open(iFs, KStreamRawFile2, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening iFile2 = %d"), err);
		return EFail;
		}	
		
	err = iFileHandle[3].Open(iFs, KStreamRawFile3, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening iFile2 = %d"), err);
		return EFail;
		}	
	
	for(TInt fileNumber=1; fileNumber<=3; fileNumber++)
		{
		err = iFileHandle[fileNumber].Size(iFileSize[fileNumber]);
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error getting size of iFile = %d"), err);
			return EFail;
			}
		iBuf[fileNumber].CreateMaxL(iFileSize[fileNumber]);
		}
	
	// Initialise the CCallBackTimer.
	iCallBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	
	return verdict;
	}
	
/**
 * DoTestStepPostambleL
 */
TVerdict CTestStepAudioOutputStreamARN2::DoTestStepPostambleL()
	{
	iClientList.DeleteAll();
	for(TInt fileNumber=1; fileNumber <=3; fileNumber++)
		{
		iFileHandle[fileNumber].Close();
		}
	
	for(TInt bufNumber=1 ; bufNumber <=3; bufNumber++)
		{
		iBuf[bufNumber].Close();
		}
	
	delete iCallBackTimer;
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}
	
	
/**
 * DoTestStepL
 */
TVerdict CTestStepAudioOutputStreamARN2::DoTestStepL()
	{
	return( DoTestL() );
	}

/**
 * StateChange
 */
void CTestStepAudioOutputStreamARN2::StateChange(TFsmAosState aState)
	{
	iState = aState;
	}
	
void CTestStepAudioOutputStreamARN2::StopTest()
	{
	//We can add other cleanup things here 
	CActiveScheduler::Stop();
	}

void CTestStepAudioOutputStreamARN2::SetExpectedEvent(TInt aClientNumber, TInt aExpectedEvent, TInt aExpectedError)
	{
	iClientEventsToMonitor[aClientNumber]= aExpectedEvent;
	iClientError[aClientNumber] = aExpectedError;
	}
	
/**
 * FsmCheck
 */
TBool CTestStepAudioOutputStreamARN2::FsmCheck(TInt aClientNumber, TInt aEventCode, TInt aError)
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
			ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedEvent, aEventCode);
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
	
void CTestStepAudioOutputStreamARN2::RegisterClientEventToMonitor(TInt aClientNumber, TInt aEventCode, TInt aError, TFsmAosState aFsmState)
	{
	iClientsRegistered++;
	iClientEventsToMonitor[aClientNumber] = aEventCode;
	iClientError[aClientNumber] = aError;
	iFsmStateforClient[aClientNumber] = aFsmState;
	}

TBool CTestStepAudioOutputStreamARN2::IsMonitoringFinished()
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
	
CFsmAudioOutputStreamClient::CFsmAudioOutputStreamClient(TInt aClientNumber, TInt aPriority, TMdaPriorityPreference aPref, CTestStepAudioOutputStreamARN2* aParent):iClientNumber(aClientNumber),iParent(aParent)
	{
	iPriority = aPriority;
	iPref = aPref;
	}
	
/**
 *
 * NewL
 * CFsmAudioOutputStreamClient*
 *
 */
CFsmAudioOutputStreamClient* CFsmAudioOutputStreamClient::NewL(TInt aClientNumber, TInt aPriority,TMdaPriorityPreference aPref, CTestStepAudioOutputStreamARN2* aParent)
	{
	CFsmAudioOutputStreamClient* s = new(ELeave) CFsmAudioOutputStreamClient(aClientNumber, aPriority, aPref, aParent);
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
void CFsmAudioOutputStreamClient::ConstructL()
	{
	iMmfAudioOutputStream = CMdaAudioOutputStream::NewL(*this, iPriority, iPref);
	}
	
CFsmAudioOutputStreamClient::~CFsmAudioOutputStreamClient()
	{
	delete iMmfAudioOutputStream;
	}
	
void CFsmAudioOutputStreamClient::MaoscOpenComplete(TInt aError)
	{
	iEvent = EAudioOuputStreamOpenComplete;
	iError = aError;
	iParent->Fsm(iClientNumber, iEvent, iError);
	}

void CFsmAudioOutputStreamClient::MaoscPlayComplete(TInt aError)
	{
	iEvent = EAudioOutputStreamPlayComplete;
	iError = aError;
	iParent->Fsm(iClientNumber, iEvent, iError);
	}

void CFsmAudioOutputStreamClient::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iEvent = EAudioOutputStreamBufferCopied;
	iError = aError;
	iParent->Fsm(iClientNumber, iEvent, iError);
	}

void CFsmAudioOutputStreamClient::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	if (aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		iEvent = EAudioOutputStreamResourceAvailable;
		iError = KErrNone;
		//Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg timeIntvbuf;
		timeIntvbuf.Copy(aNotificationData);
		iResumedPosition = timeIntvbuf();
		
		iParent->Fsm(iClientNumber, iEvent, iError);
		}	
	}
	
void CFsmAudioOutputStreamClient::TestOpen()
	{
	iMmfAudioOutputStream->Open(NULL);
	}
	
TInt CFsmAudioOutputStreamClient::TestRegisterAsClient()
	{
	return iMmfAudioOutputStream->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
	}

TInt CFsmAudioOutputStreamClient::TestWrite(const TDesC8 &aData)
	{
	TRAPD(err,iMmfAudioOutputStream->WriteL(aData));
	return err;
	}

const TTimeIntervalMicroSeconds&  CFsmAudioOutputStreamClient::GetCurrentPosition()
	{
	return iMmfAudioOutputStream->Position();
	}
	
TTimeIntervalMicroSeconds& CFsmAudioOutputStreamClient::GetResumedPosition()
	{
	return iResumedPosition;
	}

TInt  CFsmAudioOutputStreamClient::BytesPlayed()
	{
	return iMmfAudioOutputStream->GetBytes();
	}	
//
//POSITIVE TESTS//
//

//
//CTestMmfAclntARN8021
//

/**
 * Constructor
 */
CTestMmfAclntARN8021::CTestMmfAclntARN8021(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8021* CTestMmfAclntARN8021::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN8021* self = new (ELeave) CTestMmfAclntARN8021(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8021::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8021::FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioOutputStream1
				iTestAudioOutputStream1->Open(NULL);				
				INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete1);
				break;
			case 1:
				// Register iTestAudioOutputStream1 for Notification
				err = iTestAudioOutputStream1->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification()"));
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					User::Leave(err);
					break;
					}
				// Open iTestAudioOutputStream2
				iTestAudioOutputStream2->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream2->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete2);
				break;
			case 2:
				// iFile2->Read()
				err = iFile2.Read(iBuf2);
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading file %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream2->WriteL()
				iTestAudioOutputStream2->WriteL(iBuf2);
				INFO_PRINTF1(_L("iTestAudioOutputStream2->WriteL()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied2);
				break;
			case 3:
				// Wait for PlayComplete event for iTestAudioOutputStream2
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete2, KErrUnderflow);
				break;
			case 4:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				iTestStepResult = EPass;
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMarncResourceAvailableOS1);
				break;
			case 5:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioOutputStream1->NotificationReceived"));
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8021::PerformTestL()
	{
	INFO_PRINTF1(_L("Register for notification, but make no request to play audio"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleOutputStream);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8022
//

/**
 * Constructor
 */
CTestMmfAclntARN8022::CTestMmfAclntARN8022(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN2(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8022* CTestMmfAclntARN8022::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN8022* self = new (ELeave) CTestMmfAclntARN8022(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8022::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8022::Fsm(TInt aClientNumber, TInt aEventCode, TInt aError)
	{
	if (FsmCheck(aClientNumber, aEventCode, aError))
		{
		iTestStepResult = (this->*(iStateHandlers[iState]))();
 		if(iTestStepResult != EPass)
 			{
 			StopTest();
 			}
		}
	}


void CTestMmfAclntARN8022::StartTestL()
	{
	iClientList[1]->TestOpen();
	INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
	//Wait for EAudioOuputStreamOpenComplete event
	SetExpectedEvent(1,CFsmAudioOutputStreamClient::EAudioOuputStreamOpenComplete,KErrNone);
	StateChange(EStateOpenComplete1);
	}

void CTestMmfAclntARN8022::InitTest()
	{
	iStateHandlers[EStateOpenComplete1] = &CTestMmfAclntARN8022::HandleStateOpenComplete1;
	iStateHandlers[EStateOpenComplete2] = &CTestMmfAclntARN8022::HandleStateOpenComplete2;
	iStateHandlers[EStateStreaming2] = &CTestMmfAclntARN8022::HandleStateStreaming2;
	iStateHandlers[EStateStreamingComplete1ErrInUse] = &CTestMmfAclntARN8022::HandleStateStreamingComplete1ErrInUse;
	iStateHandlers[EStateResourceAvailableOS1] = &CTestMmfAclntARN8022::HandleStateResourceAvailableOS1;
	iStateHandlers[EStateStreamingComplete2ErrUnderflow] = &CTestMmfAclntARN8022::HandleStateStreamingComplete2ErrUnderflow;
	}

TVerdict CTestMmfAclntARN8022::HandleStateOpenComplete1()
	{
	// Register Client1 for Notification
	TInt err = KErrNone;
	err = iClientList[1]->TestRegisterAsClient();
	INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification()"));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		User::Leave(err);
		}
	iClientList[2]->TestOpen();
	//Wait for EAudioOuputStreamOpenComplete event
	SetExpectedEvent(2,CFsmAudioOutputStreamClient::EAudioOuputStreamOpenComplete,KErrNone);
	StateChange(EStateOpenComplete2);
	return EPass;
	}

TVerdict CTestMmfAclntARN8022::HandleStateOpenComplete2()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream2->Open()"));
	TInt err = iFileHandle[2].Read(iBuf[2]);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading file %d"), err);
		User::Leave(err);
		}
	INFO_PRINTF1(_L("Pre iTestAudioOutputStream2->WriteL()"));
	err = iClientList[2]->TestWrite(iBuf[2]);
	if( err != KErrNone)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream2->WriteL failed %d"),err);
		User::Leave(err);
		}
	INFO_PRINTF1(_L("iTestAudioOutputStream1->WriteL()"));
	SetExpectedEvent(2,CFsmAudioOutputStreamClient::EAudioOutputStreamBufferCopied,KErrNone);
	StateChange(EStateStreaming2);
	return EPass;
	}
	
TVerdict CTestMmfAclntARN8022::HandleStateStreaming2()
	{
	TInt err = KErrNone;
	err = iFileHandle[1].Read(iBuf[1]); //-synchronous
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile1 %d"), err);
		return EInconclusive;
		}
	INFO_PRINTF1(_L("iTestAudioOutputStream1->WriteL()"));
	err = iClientList[1]->TestWrite(iBuf[1]); //-Should get Play Error.
	if( err != KErrNone)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream1->WriteL failed %d"),err);
		return EInconclusive;
		}
	// Set ExpectedEvent & ExpectedError and change the State
	SetExpectedEvent(1,CFsmAudioOutputStreamClient::EAudioOutputStreamPlayComplete, KErrInUse);
	StateChange(EStateStreamingComplete1ErrInUse);
	return EPass;
	}

TVerdict CTestMmfAclntARN8022::HandleStateStreamingComplete1ErrInUse()
	{
	RegisterClientEventToMonitor(1, CFsmAudioOutputStreamClient::EAudioOutputStreamResourceAvailable, KErrNone, EStateResourceAvailableOS1);
	RegisterClientEventToMonitor(2, CFsmAudioOutputStreamClient::EAudioOutputStreamPlayComplete, KErrUnderflow, EStateStreamingComplete2ErrUnderflow);
	StateChange(EStateMonitoringEvents);
	return EPass;
	}

TVerdict CTestMmfAclntARN8022::HandleStateResourceAvailableOS1()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream1->NotificationReceived"));
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
	
TVerdict CTestMmfAclntARN8022::HandleStateStreamingComplete2ErrUnderflow()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream2->PlayComplete"));
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
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8022::PerformTestL()
	{
	INFO_PRINTF1(_L("Register (b4 play, after Open) and Receive Notification of a Resource Available Event."));
	INFO_PRINTF1(_L("(when audio resource is currently unavailable and then available)"));
	iTestStepResult = EFail;
	
	InitTest();
	StartTestL();
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8023
//

/**
 * Constructor
 */
CTestMmfAclntARN8023::CTestMmfAclntARN8023(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN2(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8023* CTestMmfAclntARN8023::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN8023* self = new (ELeave) CTestMmfAclntARN8023(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8023::DoTestL()
	{
	return( PerformTestL() );
	}
	

/**
 * FsmL
 */
void CTestMmfAclntARN8023::Fsm(TInt aClientNumber, TInt aEventCode, TInt aError)
	{
	if(FsmCheck(aClientNumber, aEventCode, aError))
		{
		iTestStepResult = (this->*(iStateHandlers[iState]))();
		if(iTestStepResult != EPass)
			{
			StopTest();
			}
		}
	}

void CTestMmfAclntARN8023::InitTest()
	{
	iStateHandlers[EStateOpenComplete1] = &CTestMmfAclntARN8023::HandleStateOpenComplete1;
	iStateHandlers[EStateOpenComplete2] = &CTestMmfAclntARN8023::HandleStateOpenComplete2;
	iStateHandlers[EStateStreaming1] = &CTestMmfAclntARN8023::HandleStateStreaming1;
	iStateHandlers[EStateStreaming2] = &CTestMmfAclntARN8023::HandleStateStreaming2;
	iStateHandlers[EStateStreamingComplete1ErrInUse] = &CTestMmfAclntARN8023::HandleStateStreamingComplete1ErrInUse;
	iStateHandlers[EStateResourceAvailableOS1] = &CTestMmfAclntARN8023::HandleStateResourceAvailableOS1;
	iStateHandlers[EStateStreamingComplete2ErrUnderflow] = &CTestMmfAclntARN8023::HandleStateStreamingComplete2ErrUnderflow;
	}

void CTestMmfAclntARN8023::StartTestL()
	{
	//Open Client1
	iClientList[1]->TestOpen();
	INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
	//Wait for EAudioOuputStreamOpenComplete event
	SetExpectedEvent(1,CFsmAudioOutputStreamClient::EAudioOuputStreamOpenComplete,KErrNone);
	StateChange(EStateOpenComplete1);
	}

TVerdict CTestMmfAclntARN8023::HandleStateOpenComplete1()
	{
	TInt err = KErrNone;
	err = iFileHandle[1].Read(iBuf[1]);
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile1 %d"), err);
		User::Leave(err);
		}
	err = iClientList[1]->TestWrite(iBuf[1]);
	if( err != KErrNone)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream1->WriteL failed %d"),err);
		User::Leave(err);
		}
	INFO_PRINTF1(_L("iTestAudioOutputStream1->WriteL()"));
	SetExpectedEvent(1,CFsmAudioOutputStreamClient::EAudioOutputStreamBufferCopied,KErrNone);
	StateChange(EStateStreaming1);
	return EPass;
	}

TVerdict CTestMmfAclntARN8023::HandleStateStreaming1()
	{
	TInt err = KErrNone;
	err = iClientList[1]->TestRegisterAsClient();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		return EInconclusive;
		}
	
	iClientList[2]->TestOpen();
	//Wait for EAudioOuputStreamOpenComplete event
	SetExpectedEvent(2,CFsmAudioOutputStreamClient::EAudioOuputStreamOpenComplete,KErrNone);
	StateChange(EStateOpenComplete2);
	return EPass;
	}

TVerdict CTestMmfAclntARN8023::HandleStateOpenComplete2()
	{
	TInt err = iFileHandle[2].Read(iBuf[2]);
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile2 %d"), err);
		return EInconclusive;
		}
	err = iClientList[2]->TestWrite(iBuf[2]);
	if( err != KErrNone)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream2->WriteL failed %d"),err);
		return EInconclusive;
		}
	INFO_PRINTF1(_L("iTestAudioOutputStream2->WriteL()"));
	SetExpectedEvent(1,CFsmAudioOutputStreamClient::EAudioOutputStreamPlayComplete, KErrInUse);
	StateChange(EStateStreamingComplete1ErrInUse);
	return EPass;
	}

TVerdict CTestMmfAclntARN8023::HandleStateStreamingComplete1ErrInUse()
	{
	SetExpectedEvent(2,CFsmAudioOutputStreamClient::EAudioOutputStreamBufferCopied,KErrNone);
	StateChange(EStateStreaming2);
	return EPass;
	}

TVerdict CTestMmfAclntARN8023::HandleStateStreaming2()
	{
	RegisterClientEventToMonitor(1,CFsmAudioOutputStreamClient::EAudioOutputStreamResourceAvailable,KErrNone,EStateResourceAvailableOS1);
	RegisterClientEventToMonitor(2,CFsmAudioOutputStreamClient::EAudioOutputStreamPlayComplete,KErrUnderflow,EStateStreamingComplete2ErrUnderflow);
	StateChange(EStateMonitoringEvents);
	return EPass;
	}

TVerdict CTestMmfAclntARN8023::HandleStateResourceAvailableOS1()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream1->NotificationReceived"));
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

TVerdict CTestMmfAclntARN8023::HandleStateStreamingComplete2ErrUnderflow()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream2->PlayComplete"));
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Didn't finish monitoring
		}
	return EPass;
	}
	
/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8023::PerformTestL()
	{
	INFO_PRINTF1(_L("Register (at the time of Play) and Receive Notification of a Resource Available Event."));
	INFO_PRINTF1(_L("(when audio resource is currently available and subsequently unavailable, and then available)"));
	iTestStepResult = EFail;
		
	InitTest();
	
	StartTestL();

	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8024
//

/**
 * Constructor
 */
CTestMmfAclntARN8024::CTestMmfAclntARN8024(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8024* CTestMmfAclntARN8024::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN8024* self = new (ELeave) CTestMmfAclntARN8024(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8024::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8024::FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioOutputStream1
				iTestAudioOutputStream1->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete1);
				break;
			case 1:
				// iFile1->Read()
				err = iFile1.Read(iBuf1); //-synchronous
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading iFile1 %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream1->WriteL()
				INFO_PRINTF1(_L("iTestAudioOutputStream1->WriteL()"));
				iTestAudioOutputStream1->WriteL(iBuf1); //-Should get Play Error.
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied1);
				break;
			case 2:
				// Open iTestAudioOutputStream2
				iTestAudioOutputStream2->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream2->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete2);
				break;
			case 3:
				// iFile2->Read()
				err = iFile2.Read(iBuf2);
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading file %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream2->WriteL()
				iTestAudioOutputStream2->WriteL(iBuf2);
				INFO_PRINTF1(_L("iTestAudioOutputStream2->WriteL()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete1, KErrInUse);
				break;
			case 4:
				// Wait for BufferCopied event for iTestAudioOutputStream2
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied2);
				break;
			case 5:
				// Cancellation of notification request for iTestAudioOutputStream1
				err = iTestAudioOutputStream1->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF2(_L("iTestAudioOutputStream1->CancelRegisterAudioResourceNotification : Return Code = %d "), err);
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->CancelRegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;					
					CActiveScheduler::Stop();
					User::Leave(err);
					break;
					}
				// Wait for PlayComplete event for iTestAudioOutputStream2
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete2, KErrUnderflow);
				break;
			case 6:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				iTestStepResult = EPass;
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMarncResourceAvailableOS1);
				break;
			case 7:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioOutputStream1->NotificationReceived"));
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8024::PerformTestL()
	{
	INFO_PRINTF1(_L("Registration (b4 File Open) and Cancellation of  the notification request"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Register iTestAudioOutputStream1 for Notification
	TInt err = iTestAudioOutputStream1->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
	INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification()"));
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		return EFail;
		}
	
	// Begin the process
	FsmL(EIdleOutputStream);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8025
//

/**
 * Constructor
 */
CTestMmfAclntARN8025::CTestMmfAclntARN8025(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN2(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8025* CTestMmfAclntARN8025::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN8025* self = new (ELeave) CTestMmfAclntARN8025(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8025::DoTestL()
	{
	return( PerformTestL() );
	}
	

void CTestMmfAclntARN8025::InitTest()
	{
	iStateHandlers[EStateOpenComplete1] = &CTestMmfAclntARN8025::HandleStateOpenComplete1;
	iStateHandlers[EStateOpenComplete2] = &CTestMmfAclntARN8025::HandleStateOpenComplete2;
	iStateHandlers[EStateStreaming1] = &CTestMmfAclntARN8025::HandleStateStreaming1;
	iStateHandlers[EStateStreaming2] = &CTestMmfAclntARN8025::HandleStateStreaming2;
	iStateHandlers[EStateStreamingComplete1ErrInUse] = &CTestMmfAclntARN8025::HandleStateStreamingComplete1ErrInUse;
	iStateHandlers[EStateResourceAvailableOS1] = &CTestMmfAclntARN8025::HandleStateResourceAvailableOS1;
	iStateHandlers[EStateStreamingComplete2ErrUnderflow] = &CTestMmfAclntARN8025::HandleStateStreamingComplete2ErrUnderflow;
	}
	
/**
 * FsmL
 */
void CTestMmfAclntARN8025::Fsm(TInt aClientNumber, TInt aEventCode, TInt aError)
	{
	if(FsmCheck(aClientNumber, aEventCode, aError))
		{
		iTestStepResult = (this->*(iStateHandlers[iState]))();
		if(iTestStepResult != EPass)
			{
			StopTest();
			}
		}	
	}

void CTestMmfAclntARN8025::StartTestL()
	{
	//Open Client1
	iClientList[1]->TestOpen();
	INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
	//Wait for EAudioOuputStreamOpenComplete event
	SetExpectedEvent(1,CFsmAudioOutputStreamClient::EAudioOuputStreamOpenComplete,KErrNone);
	StateChange(EStateOpenComplete1);
	}

TVerdict CTestMmfAclntARN8025::HandleStateOpenComplete1()
	{
	TInt err = KErrNone;
	err = iFileHandle[1].Read(iBuf[1]);
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile1 %d"), err);
		User::Leave(err);
		}
	err = iClientList[1]->TestWrite(iBuf[1]);
	if( err != KErrNone)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream1->WriteL failed %d"),err);
		User::Leave(err);
		}
	INFO_PRINTF1(_L("iTestAudioOutputStream1->WriteL()"));
	SetExpectedEvent(1,CFsmAudioOutputStreamClient::EAudioOutputStreamBufferCopied,KErrNone);
	StateChange(EStateStreaming1);
	return EPass;
	}

TVerdict CTestMmfAclntARN8025::HandleStateStreaming1()
	{
	TInt err = KErrNone;
	err = iClientList[1]->TestRegisterAsClient();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification - Failed with Error : %d"), err);
		return EInconclusive;
		}
	
	iClientList[2]->TestOpen();
	INFO_PRINTF1(_L("iTestAudioOutputStream2->Open()"));
	//Wait for EAudioOuputStreamOpenComplete event
	SetExpectedEvent(2,CFsmAudioOutputStreamClient::EAudioOuputStreamOpenComplete,KErrNone);
	StateChange(EStateOpenComplete2);
	return EPass;
	}

TVerdict CTestMmfAclntARN8025::HandleStateOpenComplete2()
	{
	TInt err = iFileHandle[2].Read(iBuf[2]);
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Error reading iFile2 %d"), err);
		return EInconclusive;
		}
	err = iClientList[2]->TestWrite(iBuf[2]);
	if( err != KErrNone)
		{
		INFO_PRINTF2(_L("iTestAudioOutputStream1->WriteL failed %d"),err);
		return EInconclusive;
		}
	INFO_PRINTF1(_L("iTestAudioOutputStream2->WriteL()"));
	SetExpectedEvent(1,CFsmAudioOutputStreamClient::EAudioOutputStreamPlayComplete, KErrInUse);
	StateChange(EStateStreamingComplete1ErrInUse);
	return EPass;
	}

TVerdict CTestMmfAclntARN8025::HandleStateStreamingComplete1ErrInUse()
	{
	iPositionStop = (static_cast<TInt64>(iClientList[1]->BytesPlayed())* KMicroSecsInOneSec)/(KSampleRate*KBytesPerSample);
	SetExpectedEvent(2,CFsmAudioOutputStreamClient::EAudioOutputStreamBufferCopied,KErrNone);
	StateChange(EStateStreaming2);
	return EPass;
	}

TVerdict CTestMmfAclntARN8025::HandleStateStreaming2()
	{
	RegisterClientEventToMonitor(1,CFsmAudioOutputStreamClient::EAudioOutputStreamResourceAvailable,KErrNone,EStateResourceAvailableOS1);
	RegisterClientEventToMonitor(2,CFsmAudioOutputStreamClient::EAudioOutputStreamPlayComplete,KErrUnderflow,EStateStreamingComplete2ErrUnderflow);
	StateChange(EStateMonitoringEvents);
	return EPass;
	}

TVerdict CTestMmfAclntARN8025::HandleStateResourceAvailableOS1()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream1->NotificationReceived"));
	
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents);
		}
	TTimeIntervalMicroSeconds positionResume = iClientList[1]->GetResumedPosition();
	iPositionResume = ((positionResume.Int64()) * KMicroSecsInOneSec / KSampleRate8000Hz);
	if(iPositionResume == iPositionStop)
		{
		INFO_PRINTF3(_L("iTestAudioOutputStream1->Audio Resource Position Matches, Expected=%d, Actual=%d"), I64LOW(iPositionStop.Int64()), I64LOW(iPositionResume.Int64()));
		iTestStepResult = EPass;
		}
	else
		{
		ERR_PRINTF3(_L("iTestAudioOutputStream1->Audio Resource Position does NOT Match, Expected=%d, Actual=%d"), I64LOW(iPositionStop.Int64()), I64LOW(iPositionResume.Int64()));
		iTestStepResult=EFail;
		}
	return iTestStepResult;
	}

TVerdict CTestMmfAclntARN8025::HandleStateStreamingComplete2ErrUnderflow()
	{
	INFO_PRINTF1(_L("iTestAudioOutputStream2->PlayComplete"));
	if(IsMonitoringFinished())
		{
		iTestStepResult = EPass;
		StopTest();
		}
	else
		{
		StateChange(EStateMonitoringEvents); //Didn't finish monitoring
		}
	return EPass;
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8025::PerformTestL()
	{
	INFO_PRINTF1( _L("Get the actual position from where the paused audio can be resumed."));
	iTestStepResult = EFail;
	
	InitTest();
	
	StartTestL();
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN8026
//

/**
 * Constructor
 */
CTestMmfAclntARN8026::CTestMmfAclntARN8026(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN8026* CTestMmfAclntARN8026::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN8026* self = new (ELeave) CTestMmfAclntARN8026(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN8026::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmCheck
 */
TBool CTestMmfAclntARN8026::FsmCheck(TMmfAudioOutputStreamEvents aEventCode, TInt aError)
	{
	TBool retFsmCheck = EFalse;
	if (iExpectedEvent != aEventCode)
		{
		if ((aEventCode == EMaoscPlayComplete2))
	        {
	        retFsmCheck = ETrue;
	        if (iEventOccured == EMarncResourceAvailableOS1)
	            {
	            return retFsmCheck;
	            }
            else
                {
                iEventOccured = EMaoscPlayComplete2;
                }
	        }
        else if (aEventCode == EMarncResourceAvailableOS1)
            {
            retFsmCheck = ETrue;
            if (iEventOccured == EMaoscPlayComplete2)
                {
                return retFsmCheck;
                }
            else
                {
                iEventOccured = EMarncResourceAvailableOS1;
                }
            }
        else
            {
       		ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedEvent, aEventCode);
    		iTestStepResult = EFail;
    		CActiveScheduler::Stop();
            }
		}
	else if (iExpectedError != aError)
		{
		ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, aError);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else
		{
		retFsmCheck = ETrue;
		}
	return retFsmCheck;	
	}

/**
 * FsmL
 */
void CTestMmfAclntARN8026::FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioOutputStream1
				iTestAudioOutputStream1->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete1);
				break;
			case 1:
				// iFile1->Read()
				err = iFile1.Read(iBuf1); //-synchronous
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading iFile1 %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream1->WriteL()
				INFO_PRINTF1(_L("iTestAudioOutputStream1->WriteL()"));
				iTestAudioOutputStream1->WriteL(iBuf1); //-Should get Play Error.
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied1);
				break;
			case 2:
				// Register iTestAudioOutputStream1 for Notification
				err = iTestAudioOutputStream1->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification()"));
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;					
					CActiveScheduler::Stop();
					User::Leave(err);
					break;
					}
				// Register iTestAudioOutputStream3 for Notification
				err = iTestAudioOutputStream3->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport3, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioOutputStream3->RegisterAudioResourceNotification()"));
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream3->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;					
					CActiveScheduler::Stop();
					User::Leave(err);
					break;
					}
				// Open iTestAudioOutputStream3
				iTestAudioOutputStream3->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream3->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete3);
				break;
			case 3:
				// Open iTestAudioOutputStream2
				iTestAudioOutputStream2->Open(NULL);
				INFO_PRINTF1(_L("iTestAudioOutputStream2->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete2);
				break;
			case 4:
				// iFile2->Read()
				err = iFile2.Read(iBuf2);
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading file %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream2->WriteL()
				iTestAudioOutputStream2->WriteL(iBuf2);
				INFO_PRINTF1(_L("iTestAudioOutputStream2->WriteL()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete1, KErrInUse);
				break;
			case 5:
				// Wait for BufferCopied event for iTestAudioOutputStream2
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied2);
				break;
			case 6:
				// iFile3->Read()
				err = iFile3.Read(iBuf3); //-synchronous
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Error reading iFile2 %d"), err);
					User::Leave(err);
					}
				// iTestAudioOutputStream3->WriteL()
				INFO_PRINTF1(_L("iTestAudioOutputStream3->WriteL()"));
				iTestAudioOutputStream3->WriteL(iBuf3);
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscBufferCopied3, KErrInUse);
				break;
			case 7:
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete3, KErrInUse);
				break;
			case 8:
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete2, KErrUnderflow);
				break;
			case 9:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMarncResourceAvailableOS1);
				break;
			case 10:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioOutputStream1->NotificationReceived"));
				err = iTestAudioOutputStream1->WillResumePlay();
				INFO_PRINTF1(_L("iTestAudioOutputStream1->WillResumePlay()"));
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->WillResumePlay() - Failed with Error : %d"), err);
					iTestStepResult = EFail;					
					CActiveScheduler::Stop();
					break;
					}
				// Set the resume position.. or may be not, since its an offset.
				// iTestAudioOutputStream1->SetPosition(iPosition);
				// iTestAudioOutputStream1->WriteL()
				iTestAudioOutputStream1->WriteL(iBuf1);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->WriteL()"));
				StateChange(EMaoscBufferCopied1);
				break;
			case 11:
				// Wait for PlayComplete event for iTestAudioOutputStream1
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscPlayComplete1, KErrUnderflow);
				break;
			case 12:
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMarncResourceAvailableOS3);
				break;
			case 13:
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestAudioOutputStream3->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN8026::PerformTestL()
	{
	INFO_PRINTF1( _L("Call WillResumePlay()"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleOutputStream);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}


//
//NEGATIVE TESTS//
//

//
//CTestMmfAclntARN1321
//

/**
 * Constructor
 */
CTestMmfAclntARN1321::CTestMmfAclntARN1321(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1321* CTestMmfAclntARN1321::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN1321* self = new (ELeave) CTestMmfAclntARN1321(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1321::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1321::FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioOutputStream1
				iTestAudioOutputStream1->Open(NULL);				
				INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete1);
				break;
			case 1:
				// Register iTestAudioOutputStream1 for Notification
				err = iTestAudioOutputStream1->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport1, KMMFEventCategoryPlaybackComplete);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport1, KMMFEventCategoryPlaybackComplete)"));
				if( err == KErrNotSupported )
					{
					INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification - Returned KErrNotSupported ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN1321::PerformTestL()
	{
	INFO_PRINTF1( _L("Register for notification with an unsupported event type."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleOutputStream);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN1322
//

/**
 * Constructor
 */
CTestMmfAclntARN1322::CTestMmfAclntARN1322(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1322* CTestMmfAclntARN1322::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN1322* self = new (ELeave) CTestMmfAclntARN1322(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1322::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1322::FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioOutputStream1
				iTestAudioOutputStream1->Open(NULL);				
				INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete1);
				break;
			case 1:
				// Register iTestAudioOutputStream1 for Notification
				err = iTestAudioOutputStream1->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification()"));
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;					
					CActiveScheduler::Stop();
					User::Leave(err);
					break;
					}
				// Cancellation of notification request for iTestAudioOutputStream1
				err = iTestAudioOutputStream1->CancelRegisterAudioResourceNotification(KMMFEventCategoryPlaybackComplete);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->CancelRegisterAudioResourceNotification(KMMFEventCategoryPlaybackComplete)"));
				if( err == KErrNotSupported )
					{
					INFO_PRINTF1(_L("iTestAudioOutputStream1->CancelRegisterAudioResourceNotification - Returned KErrNotSupported ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->CancelRegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN1322::PerformTestL()
	{
	INFO_PRINTF1( _L("Cancel the request for notification with an unsupported event type."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleOutputStream);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN1323
//

/**
 * Constructor
 */
CTestMmfAclntARN1323::CTestMmfAclntARN1323(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1323* CTestMmfAclntARN1323::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN1323* self = new (ELeave) CTestMmfAclntARN1323(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1323::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1323::FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioOutputStream1
				iTestAudioOutputStream1->Open(NULL);				
				INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete1);
				break;
			case 1:
				// Cancellation of notification request for iTestAudioOutputStream1
				err = iTestAudioOutputStream1->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->CancelRegisterAudioResourceNotification()"));
				if( err == KErrCancel )
					{
					INFO_PRINTF1(_L("iTestAudioOutputStream1->CancelRegisterAudioResourceNotification - Returned KErrCancel ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->CancelRegisterAudioResourceNotification - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN1323::PerformTestL()
	{
	INFO_PRINTF1(_L("Cancel when no resource Notification is requested."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleOutputStream);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

//
//CTestMmfAclntARN1324
//

/**
 * Constructor
 */
CTestMmfAclntARN1324::CTestMmfAclntARN1324(const TDesC& aTestName)
	: CTestStepAudioOutputStreamARN(aTestName)
	{}

/**
 * NewL
 */
CTestMmfAclntARN1324* CTestMmfAclntARN1324::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntARN1324* self = new (ELeave) CTestMmfAclntARN1324(aTestName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict CTestMmfAclntARN1324::DoTestL()
	{
	return( PerformTestL() );
	}

/**
 * FsmL
 */
void CTestMmfAclntARN1324::FsmL(TMmfAudioOutputStreamEvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt err = KErrNone;
		switch (iState)
			{
			case 0:
				// Open iTestAudioOutputStream1
				iTestAudioOutputStream1->Open(NULL);				
				INFO_PRINTF1(_L("iTestAudioOutputStream1->Open()"));
				// Set ExpectedEvent & ExpectedError and change the State
				StateChange(EMaoscOpenComplete1);
				break;
			case 1:
				// Register iTestAudioOutputStream1 for Notification
				err = iTestAudioOutputStream1->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification()#1"));
				if( err != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification#1 - Failed with Error : %d"), err);
					iTestStepResult = EFail;					
					CActiveScheduler::Stop();
					User::Leave(err);
					break;
					}
				// Register iTestAudioOutputStream1 for Notification (again)
				err = iTestAudioOutputStream1->RegisterAudioResourceNotification(*iMdaAudioOutputStreamCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification()#2"));
				if( err == KErrAlreadyExists )
					{
					INFO_PRINTF1(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification#2 - Returned KErrAlreadyExists ") );
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestAudioOutputStream1->RegisterAudioResourceNotification#2 - Failed with Error : %d"), err);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
			}
		}
	}

/**
 * PerformTestL
 */
TVerdict CTestMmfAclntARN1324::PerformTestL()
	{
	INFO_PRINTF1(_L("Multiple Registration for notification for the same event type."));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	FsmL(EIdleOutputStream);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}
