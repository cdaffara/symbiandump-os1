// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TSIMmfCtlfrmStep.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "TSI_MmfCtlfrmStep.h"
#include "TSI_MmfCtlfrmSuite.h"
#include "TSI_MmfCodes.h"
#include "TSI_MmfEventIds.h"
#include "ActrlTestUids.h"

#include <mmf/common/mmfcontroller.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>

const TUid KTestControllerUid = {KTSIMmfControllerUid};
const TUid KTestController2Uid = {KTSIMmfController2Uid};
//const TUid KTestDataSourceUid = {KTSIMmfDataSourceUid};	
//const TUid KTestDataSinkUid   = {KTSIMmfDataSinkUid};		

// audio controller, from mmfControllerImplementationUIDs.hrh
//const TUid KTestAudioControllerUid = {KMmfUidControllerAudio};	

// event values for audio controller testing
const TInt KDelay            = 1000000;
const TInt KMaxRetries       = 12;


// ---------------------------
// RTestStepMmfCtlfrm
// base class

RTestStepMmfCtlfrm::RTestStepMmfCtlfrm() 
	{
	}

// ---------------------------
// RTestStepMmfCtlfrmTest1
// base class with preamble / postamble to open and close the test controller

TVerdict RTestStepMmfCtlfrmTest1::OpenL() 
	{
	TInt error = KErrNone;

	iSettings.iPriority = ETSIMmfPriorityLow;
	iSettings.iPref = EMdaPriorityPreferenceTime;
	iSettings.iState = EMMFStateIdle;

	// Open a controller
	error = iController.Open(KTestControllerUid, iSettings);
	if (error)
		{
		ERR_PRINTF2(_L("controller failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	return iTestStepResult = EPass;
	}

void RTestStepMmfCtlfrmTest1::Close() 
	{
	iController.Close();

	// close controller delay - ensure it has time to finish
	// (so we don't get access violation from something trying to dequeue it before it's closed)
	const TTimeIntervalMicroSeconds32 KCloseControllerDelay = 500000L;
	User::After(KCloseControllerDelay);
	}

// ---------------------------
// RTestStepMmfCtlfrmTest2
// base class with preamble / postamble to open and close the test controller 2

TVerdict RTestStepMmfCtlfrmTest2::OpenL() 
	{
	TInt error = KErrNone;

	iSettings.iPriority = ETSIMmfPriorityLow;
	iSettings.iPref = EMdaPriorityPreferenceTime;
	iSettings.iState = EMMFStateIdle;

	// Open a controller
	error = iController.Open(KTestController2Uid, iSettings);
	if (error)
		{
		ERR_PRINTF2(_L("controller failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	return iTestStepResult = EPass;
	}

void RTestStepMmfCtlfrmTest2::Close() 
	{
	iController.Close();
	// close controller delay - ensure it has time to finish
	// (so we don't get access violation from something trying to dequeue it before it's closed)
	const TTimeIntervalMicroSeconds32 KCloseControllerDelay = 500000L;
	User::After(KCloseControllerDelay);
	}

// ---------------------------
// RTestStepMmfCtlfrmAudio
// base class with preamble / postamble to open and close the audio controller

TVerdict RTestStepMmfCtlfrmAudio::OpenL() 
	{
	TInt error = KErrNone;

	iSettings.iPriority = ETSIMmfPriorityHigh;
	iSettings.iPref = EMdaPriorityPreferenceQuality;
	iSettings.iState = EMMFStateIdle;

	// Open a controller
	error = iController.Open(KUidCustomTestAudioPlugin, iSettings);
//	error = iController.Open(KTestAudioControllerUid, iSettings);
	if (error)
		{
		ERR_PRINTF2(_L("controller failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF2(_L("Opened Custom Audio Controller, UID 0x%8x"), KUidCustomTestAudioPlugin);
	// Add a source and sink
	_LIT(KTestWavFile, "c:\\TsiMmfCtlfrmData\\test.wav");
	iFileConfig().iPath = KTestWavFile;
	error = iController.AddDataSource(KUidMmfFileSource, iFileConfig);
	if (error)
		{
		ERR_PRINTF2(_L("failed to add a data source, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}
	error = iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
	if (error)
		{
		ERR_PRINTF2(_L("failed to add a data sink, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}
	
	return iTestStepResult = EPass;
	}

void RTestStepMmfCtlfrmAudio::Close() 
	{
	iController.Stop();
	iController.Reset();
	iController.Close();
	// close controller delay - ensure it has time to finish
	// (so we don't get access violation from something trying to dequeue it before it's closed)
	const TTimeIntervalMicroSeconds32 KCloseControllerDelay = 500000L;
	User::After(KCloseControllerDelay);
	}

// Searches for an event
TVerdict RTestStepMmfCtlfrmAudio::SearchForEvent( TMMFEvent& aEvent )
	{
#ifdef EVENT_SEARCH_DISABLED
	WARN_PRINTF1(_L("Warning : SearchForEvent disabled"));
    TMMFEvent dummyEvent; // JS
	aEvent = dummyEvent;	// to get rid of compiler warning
	return EPass;
#else

	TVerdict result = EFail;
	
	// status of events
	TRequestStatus timerStatus;
	TRequestStatus eventStatus;

	// due to problems with the timer as a member variable 
	// added a local timer here
	RTimer  myTimer ;
	myTimer.CreateLocal();
	
    // create an event and initialise with unknown marker

    TMMFEventPckg receivedEvent;
	
	// for KMaxRetries attempt to find the event
	for( TInt retries = 0; retries < KMaxRetries; retries++ )
		{
		// post receive event to controller
		iController.ReceiveEvents( receivedEvent, eventStatus );
		// start breakout timer to escape
		myTimer.After(timerStatus, KDelay);
		
		// wait for an event to mature
		User::WaitForRequest( eventStatus, timerStatus );
		
		// check if the command is the event 
		// and that a timeout did not occur
        if( IsTimeOut( eventStatus ))
			{
			// cancel the receive events
			CancelReceivedEvents();
			}
		else if( IsSoughtEvent( aEvent, receivedEvent ) )
			{
			// cancel the outstanding timer
			User::Check();
			myTimer.Cancel();
			// set status to pass since we have found the event
			result = EPass;
			break;
			}
		else
			{
			// We rx'd an event other than expected
			myTimer.Cancel();
			}
		}
	return result;
#endif // EVENT_SEARCH_DISABLED
	}

TBool RTestStepMmfCtlfrmAudio::IsTimeOut( TRequestStatus& aEventStatus )
	{
    return ( aEventStatus == KRequestPending);
	}

TBool RTestStepMmfCtlfrmAudio::IsSoughtEvent( TMMFEvent& aExpectedEvent, TMMFEventPckg& aReceivedEvent )
	{
	
	TInt expectedUid = aExpectedEvent.iEventType.iUid;
	TInt receivedUid = aReceivedEvent().iEventType.iUid;

	// display any events we get
	INFO_PRINTF3(_L("Event received : 0x%08x %d"), 
					 aReceivedEvent().iEventType.iUid, aReceivedEvent().iErrorCode);

	return ((expectedUid == receivedUid) &&
	 	    (aExpectedEvent.iErrorCode == aReceivedEvent().iErrorCode));
	}

void RTestStepMmfCtlfrmAudio::CancelReceivedEvents()
	{
	iController.CancelReceiveEvents();
	}

// ---------------------------
// RTestStepMmfCtlfrmAudioPrimed
// as above, with source / sink handles, primed in preamble

TVerdict RTestStepMmfCtlfrmAudioPrimed::OpenL()
	{
	// initialise the controller, including adding source and sink; prime it.
	// retain handles to the source and sink so we can verify them after reset
	TInt error = KErrNone;
	TVerdict result = EPass;

	iSourceHandlePtr = NULL;
	iSinkHandlePtr = NULL;

	iSettings.iPriority = ETSIMmfPriorityHigh;
	iSettings.iPref = EMdaPriorityPreferenceQuality;
	iSettings.iState = EMMFStateIdle;

	// Open a controller
	error = iController.Open(KUidCustomTestAudioPlugin, iSettings);
	if (error)
		{
		ERR_PRINTF2(_L("controller failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF2(_L("Opened Custom Audio Controller, UID 0x%8x"), KUidCustomTestAudioPlugin);
	// Add a source and sink
	_LIT(KTestWavFile, "c:\\TsiMmfCtlfrmData\\test.wav");
	iFileConfig().iPath = KTestWavFile;

	// add a data source with a handle
	iSourceHandlePtr = new (ELeave) TMMFMessageDestination();
	TMMFMessageDestination& sourceHandle = *iSourceHandlePtr;
	error = iController.AddDataSource(KUidMmfFileSource, iFileConfig, sourceHandle);
	if (error)
		{
		ERR_PRINTF2(_L("AddDataSource failed, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}

	// add a data sink with a handle
	iSinkHandlePtr = new (ELeave) TMMFMessageDestination();
	TMMFMessageDestination& sinkHandle = *iSinkHandlePtr;

	error = iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8, sinkHandle);
	if (error)
		{
		ERR_PRINTF2(_L("AddDataSink failed, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}

	// prime it
	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}
	
	// wait for and process the return event from the custom audio controller
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a prime event"));
		Close();
		return iTestStepResult = EInconclusive;
		}

	return iTestStepResult = EPass;
	}

void RTestStepMmfCtlfrmAudioPrimed::Close() 
	{
	iController.Stop();
	iController.Reset();
	delete iSinkHandlePtr;
	delete iSourceHandlePtr;
	iController.Close();
	// close controller delay - ensure it has time to finish
	// (so we don't get access violation from something trying to dequeue it before it's closed)
	const TTimeIntervalMicroSeconds32 KCloseControllerDelay = 500000L;
	User::After(KCloseControllerDelay);
	}


// ---------------------------
// RTestStepMmfCtlfrmAudioNoSource
// same as RTestStepMmfCtlfrmAudio, but has no source or sink loaded yet
TVerdict RTestStepMmfCtlfrmAudioNoSource::OpenL()
	{
	// preamble - load a controller but give it no data source
	TInt error = KErrNone;

	iSettings.iPriority = ETSIMmfPriorityHigh;
	iSettings.iPref = EMdaPriorityPreferenceQuality;
	iSettings.iState = EMMFStateIdle;

	// Open a controller
	error = iController.Open(KUidCustomTestAudioPlugin, iSettings);
	if (error)
		{
		ERR_PRINTF2(_L("controller failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF2(_L("Opened Custom Audio Controller, UID 0x%8x"), KUidCustomTestAudioPlugin);
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestStepMmfCtlfrmDualAudio
// same as RTestStepMmfCtlfrmAudio, with 2 identical controllers

TVerdict RTestStepMmfCtlfrmDualAudio::OpenL() 
	{
	TInt error = KErrNone;

	iSettings.iPriority = ETSIMmfPriorityHigh;
	iSettings.iPref = EMdaPriorityPreferenceTime;
	iSettings.iState = EMMFStateIdle;

	// Open both controllers
	error = iController1.Open(KUidCustomTestAudioPlugin, iSettings);
	if (error)
		{
		ERR_PRINTF2(_L("iController1 failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	INFO_PRINTF2(_L("Controller 1 : Opened Custom Audio Controller, UID 0x%8x"), KUidCustomTestAudioPlugin);
	error = iController2.Open(KUidCustomTestAudioPlugin, iSettings);
	if (error)
		{
		ERR_PRINTF2(_L("iController2 failed to open, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}
	INFO_PRINTF2(_L("Controller 2 : Opened Custom Audio Controller, UID 0x%8x"), KUidCustomTestAudioPlugin);

	// Add source and sink
	// Use two different files to avoid conflict
	_LIT(KTestWavFile1, "c:\\TsiMmfCtlfrmData\\test.wav");
	_LIT(KTestWavFile2, "c:\\TsiMmfCtlfrmData\\test2.wav");
	iFileConfig1().iPath = KTestWavFile1;
	iFileConfig2().iPath = KTestWavFile2;
	error = iController1.AddDataSource(KUidMmfFileSource, iFileConfig1);
	if (error)
		{
		ERR_PRINTF2(_L("iController1 failed to add a data source, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	error = iController1.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
	if (error)
		{
		ERR_PRINTF2(_L("iController1 failed to add a data sink, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}
	error = iController2.AddDataSource(KUidMmfFileSource, iFileConfig2);
	if (error)
		{
		ERR_PRINTF2(_L("iController2 failed to add a data source, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}
	error = iController2.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
	if (error)
		{
		ERR_PRINTF2(_L("iController2 failed to add a data sink, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}
	
	return iTestStepResult = EPass;
	}

void RTestStepMmfCtlfrmDualAudio::Close() 
	{
	iController1.Stop();
	iController1.Reset();
	iController1.Close();

	iController2.Stop();
	iController2.Reset();
	iController2.Close();

	// close controller delay - ensure it has time to finish
	// (so we don't get access violation from something trying to dequeue it before it's closed)
	const TTimeIntervalMicroSeconds32 KCloseControllerDelay = 500000L;
	User::After(KCloseControllerDelay);
	}

// Searches for an event
TVerdict RTestStepMmfCtlfrmDualAudio::SearchForEvent( TMMFEvent& aEvent, TInt aWhichController )
	{
	// NB don't worry about EVENT_SEARCH_DISABLED here,
	// as generally we'll be looking for events from the Controller Framework here,
	// not from the custom Audio Controller

	TVerdict result = EFail;
	
	// status of events
	TRequestStatus timerStatus    ;
	TRequestStatus eventStatus    ;

	RMMFController* theController;
	// which controller are we searching on?
	if(aWhichController == 1)
		theController = &iController1;
	else if(aWhichController == 2)
		theController = &iController2;
	else 
		{
		ERR_PRINTF1(_L("SearchForEvent error : controller must be 1 or 2"));
		return result = EFail;
		}

	// due to problems with the timer as a member variable 
	// added a local timer here
	RTimer  myTimer ;
	myTimer.CreateLocal();
	
    // create an event and initialise with unknown marker

    TMMFEventPckg receivedEvent;
	
	// for KMaxRetries attempt to find the event
	for( TInt retries = 0; retries < KMaxRetries; retries++ )
		{
		// post receive event to controller
		(*theController).ReceiveEvents( receivedEvent, eventStatus );
		// start breakout timer to escape
		myTimer.After( timerStatus, KDelay );
		
		// wait for an event to mature
		User::WaitForRequest( eventStatus, timerStatus );
		
		// check if the command is the event 
		// and that a timeout did not occur
        if( IsTimeOut( eventStatus ))
			{
			// cancel the receive events
			CancelReceivedEvents(aWhichController);
			}
		else if( IsSoughtEvent( aEvent, receivedEvent ) )
			{
			// cancel the outstanding timer
			User::Check();
			myTimer.Cancel();
			// set status to pass since we have found the event
			result = EPass;
			break;
			}
		else
			{
			// We rx'd an event other than expected
			myTimer.Cancel();
			}
		}
    return result;
	}

TBool RTestStepMmfCtlfrmDualAudio::IsTimeOut( TRequestStatus& aEventStatus )
	{
    return (aEventStatus == KRequestPending);
	}

TBool RTestStepMmfCtlfrmDualAudio::IsSoughtEvent( TMMFEvent& aExpectedEvent, TMMFEventPckg& aReceivedEvent )
	{
	
	TInt expectedUid = aExpectedEvent.iEventType.iUid;
	TInt receivedUid = aReceivedEvent().iEventType.iUid;

	// display any events we get
	INFO_PRINTF3(_L("Event expected : 0x%08x %d"), 
				 expectedUid, aExpectedEvent.iErrorCode);
	INFO_PRINTF3(_L("Event received : 0x%08x %d"), 
				 aReceivedEvent().iEventType.iUid, aReceivedEvent().iErrorCode);

	return ((expectedUid == receivedUid) &&
	 	    (aExpectedEvent.iErrorCode == aReceivedEvent().iErrorCode));
	}

void RTestStepMmfCtlfrmDualAudio::CancelReceivedEvents(TInt aWhichController)
	{
	RMMFController* theController;
	// which controller are we searching on?
	if(aWhichController == 1)
		theController = &iController1;
	else if(aWhichController == 2)
		theController = &iController2;
	else 
		{
		ERR_PRINTF1(_L("CancelReceivedEvents error : controller must be 1 or 2"));
		return;
		}
	(*theController).CancelReceiveEvents();
	}

// ---------------------------
// RTestStepMmfCtlfrmVideo
// base class with preamble / postamble to open and close the video controller
// NB we're only actually using this to test custom commands

TVerdict RTestStepMmfCtlfrmVideo::OpenL() 
	{
	TInt error = KErrNone;

	iSettings.iPriority = ETSIMmfPriorityHigh;
	iSettings.iPref = EMdaPriorityPreferenceQuality;
	iSettings.iState = EMMFStateIdle;

	// Open a controller
	error = iController.Open(KMmfVideoTestControllerUid, iSettings);
	if (error)
		{
		ERR_PRINTF2(_L("controller failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF2(_L("Opened Custom Video Controller, UID 0x%8x"), KMmfVideoTestControllerUid);
	// Add a source and sink
	_LIT(KTestAviFile, "c:\\VclntITestData\\LongVideo.avi");
	iFileConfig().iPath = KTestAviFile;
	error = iController.AddDataSource(KUidMmfFileSource, iFileConfig);
	if (error)
		{
		ERR_PRINTF2(_L("failed to add a data source, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}
	error = iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
	if (error)
		{
		ERR_PRINTF2(_L("failed to add a data sink, error %d"), error);
		Close();
		return iTestStepResult = EInconclusive;
		}
	
	return iTestStepResult = EPass;
	}

void RTestStepMmfCtlfrmVideo::Close() 
	{
	iController.Stop();
	iController.Reset();
	iController.Close();
	// close controller delay - ensure it has time to finish
	// (so we don't get access violation from something trying to dequeue it before it's closed)
	const TTimeIntervalMicroSeconds32 KCloseControllerDelay = 500000L;
	User::After(KCloseControllerDelay);
	}

