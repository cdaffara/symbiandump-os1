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
// TestStepEsoc.cpp
// This contains CTestCase which is the base class for all the TestCase DLLs
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "networking/log.h"
#include "networking/teststep.h"
#include "TestStepPsdAgt.h"
#include "TestSuitePsdAgt.h"

// RPacketContext
#include <etelpckt.h> 

// KConnectionOpen
#include <connectprog.h>

// Set CDMA or GPRS mode
#include "DbUndo.h"

// constructor
CTestStepPsdAgt::CTestStepPsdAgt(const TPtrC &aName)
	{
	iTestStepName=aName;

	// Always start with test stage zero
	iStage=0;
	}

enum TVerdict CTestStepPsdAgt::doTestStepPreambleL( void )
	{	
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	iDlgSvr = new(ELeave)CDialogServer();
	return EPass;
	}

enum TVerdict CTestStepPsdAgt::doTestStepPostambleL( void )
	{
	if(iDlgSvr)
		delete iDlgSvr;

	if(iActiveScheduler)
		delete iActiveScheduler;

	return EPass;
	}



/***
*   doTestStep is called by the framework to execute a test step
*   It performs some initialisation steps common to all PSD.AGT tests
*   before calling the doPsdAgtTestStepL() function overridden in
*   the derived class
***/
enum TVerdict CTestStepPsdAgt::doTestStepL(  )
	{
	TBool bResult;

	TInt processHandleCountBefore;
	TInt threadHandleCountBefore;
	TInt reqsBefore;
	
	TInt processHandleCountAfter;
	TInt threadHandleCountAfter;
	TInt reqsAfter;

	reqsBefore= RThread().RequestCount();
	RThread().HandleCount(processHandleCountBefore, threadHandleCountBefore);

	__UHEAP_MARK;

	bResult = iDlgSvr->Open();
	if (bResult == EFalse)
		{
		Log(_L("Cannot connect to Dialog Server"));
		return EFail;
		}

	iDlgSvr->RequestDialogAppearanceNotification(this);
	CleanupClosePushL(*iDlgSvr);

	// Do the test
	iResult = doPsdAgtTestStepL();
	CleanupStack::PopAndDestroy(); // iDlgSvr->Close();

	DelayL(1000000);
	
	__UHEAP_MARKEND;

	reqsAfter= RThread().RequestCount();
	RThread().HandleCount(processHandleCountAfter, threadHandleCountAfter);
	

	TESTL(reqsAfter == reqsBefore);
	TESTL(threadHandleCountAfter == threadHandleCountBefore);
	//TESTL(processHandleCountAfter == processHandleCountBefore);
	
	return iResult;
	}



/***
*   Notification is called by PSDAGT when it needs to pass an event to NIFMAN
*   The call is passed up to here by our CNifAgentRefN1
*   
***/
TInt CTestStepPsdAgt::Notification(TAgentToNifEventType aEvent, TAny* aInfo)
	{
	// This calls the appropriate handler function
	if(aEvent == EAgentToNifEventTypeDisableConnection)
		{
		PsdAgtDisconnectRequest();	
		}
	else if(aEvent == EAgentToNifEventTypeGetDataTransfer)
		{
		// Unpack data volume structure here instead of doing it in several derived classes
		TPckg<RPacketContext::TDataVolume>* dataPackage = (TPckg<RPacketContext::TDataVolume>*) aInfo;
		RPacketContext::TDataVolume& data = (*dataPackage)();
	
		data.iOverflowCounterSent = 0;
		data.iOverflowCounterReceived = 0;
		PsdAgtGetDataVolumeRequest(data.iBytesSent, data.iBytesReceived);
		}

	return 0;	
	}

/***
*   Called by Notification function when a EAgentToNifEventTypeGetDataTransfer
*   event is received. Should override in derived classes when interested in 
*   log data volume requests
*
***/
void CTestStepPsdAgt::PsdAgtGetDataVolumeRequest(TUint32 &aSent, TUint32 &aReceived)
	{
	aSent = 0;
	aReceived=0;
	}

/***
*   Called by Notification function when a EAgentToNifEventTypeDisableConnection
*   event is received. Should override in derived classes when this request is 
*   expected in the test scenario
*
***/
void CTestStepPsdAgt::PsdAgtDisconnectRequest()
	{
	// We're not expecting receive this unless the test case calls for it in 
	// which case the derived class should override this function
	Log(_L("Unexpected Disconnect Request from PSDAGT"));
	TESTE(FALSE,KErrNone);
	}


/***
*   DialogHasAppeared is called by the dummy dialog server
*   to inform the test case that a dialog has been presented
***/
void CTestStepPsdAgt::DialogHasAppeared()
	{
	// Should not get here in the default implementation of this function
	Log(_L("Unexpected QoS change dialog box"));
	TEST(FALSE);
	}


void CTestStepPsdAgt::AgentProgress(TInt , TInt )
	{
	// Normally don't care about AgentProgress messages, override when we do care

	}



// Allows other active objects to run while waiting for the specified time
void CTestStepPsdAgt::DelayL(TInt aMicroseconds)
	{
	// Construct and start the timer
	TCallBack callbackfn(CTestStepPsdAgt::TimerCallback, this);
	CPeriodic *regularUpdater = CPeriodic::NewL(CActive::EPriorityStandard);
	CleanupStack::PushL(regularUpdater);
	regularUpdater->Start(aMicroseconds,aMicroseconds,callbackfn);

	// Block until timer complete
	CActiveScheduler::Start();

	// Stop and delete the timer
	regularUpdater->Cancel();
	CleanupStack::PopAndDestroy();
	}


// The callback for the DelayL function
TInt CTestStepPsdAgt::TimerCallback(TAny *)
	{
	// Unblock the TAgt::DelayL() function
	CActiveScheduler::Stop();
	return 0;
	}

void CTestStepPsdAgt::SetGPRSModeL(MAgtNotify &aAgtNotify)
	{
	aAgtNotify.SetMode(RTelServer::ENetworkModeGsm);

	// Open database
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* session = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession* session = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(session);
	
	// Open outgoing Connection Prefs table
	CCDConnectionPrefsRecord* ptrConnectionPref = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(ptrConnectionPref);

	ptrConnectionPref->iDirection = static_cast<TCommDbConnectionDirection>(ECommDbConnectionDirectionOutgoing);
	ptrConnectionPref->FindL(*session);

	// Change IAP to GPRS
	ptrConnectionPref->iDefaultIAP = KGprsIAPIndex;
	ptrConnectionPref->ModifyL(*session);
	
	// Cleanup
	CleanupStack::PopAndDestroy(ptrConnectionPref);
	CleanupStack::PopAndDestroy(session);
	}



/************************************************************************************
*    NormalConnect is a utility function to connect and disconnect using an AGT
*
*    The preconditions for this function are assumed to be set so that 
*    a connection attempt will be successful. Any error will be reported to
*    the test framework as a failed test case
*    
*************************************************************************************/
void CTestStepPsdAgt::NormalConnectL()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	DelayL(2000000);

	// Disconnect
	Log(_L("Disconnecting"));
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);
	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}

/************************************************************************************
*    ReConnect is a utility function to connect and disconnect using an AGT
*
*    The preconditions for this function are assumed to be set so that 
*    a connection attempt will be successful. Any error will be reported to
*    the test framework as a failed test case
*    
*************************************************************************************/
void CTestStepPsdAgt::ReConnectL()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	DelayL(2000000);

	// Attempt connection
	Log(_L("Reconnecting..."));
	agent->ReConnect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Reconnect successful"));

	DelayL(2000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	Log(_L("Disconnect successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));

	}

/************************************************************************************
*    NormalConnectOOM is a utility function to connect and disconnect using an AGT
*	 for OOM test
*
*    The preconditions for this function are assumed to be set so that 
*    a connection attempt will be successful. Any error will be reported to
*    the test framework as a failed test case
*    
*************************************************************************************/
void CTestStepPsdAgt::NormalConnectOOML()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	
	// we dont want to log info about the error during OOM test using TESTEL
	if(err == KErrNoMemory)
		User::Leave(KErrNoMemory);
	
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	DelayL(2000000);

	// Disconnect
	Log(_L("Disconnecting"));
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	
		// we dont want to log info about the error during OOM test using TESTEL
	if(err == KErrNoMemory)
		User::Leave(KErrNoMemory);
	
	TESTEL(err==KErrNone,err);
	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}

/************************************************************************************
*    ReConnectOOM is a utility function to connect and disconnect using an AGT
*	 for OOM test
*	
*    The preconditions for this function are assumed to be set so that 
*    a connection attempt will be successful. Any error will be reported to
*    the test framework as a failed test case
*    
*************************************************************************************/
void CTestStepPsdAgt::ReConnectOOML()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	
	// we dont want to log info about the error during OOM test using TESTEL
	if(err == KErrNoMemory)
		User::Leave(KErrNoMemory);
	// report any other errors
	TESTEL(err==KErrNone, err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	DelayL(2000000);

	// Attempt connection
	Log(_L("Reconnecting..."));
	agent->ReConnect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	
	// we dont want to log info about the error during OOM test using TESTEL
	if(err == KErrNoMemory)
		User::Leave(KErrNoMemory);
	
	// report any other errors
	TESTEL(err==KErrNone, err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Reconnect successful"));

	DelayL(2000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	
	// we dont want to log info about the error during OOM test using TESTEL
	if(err == KErrNoMemory)
		User::Leave(KErrNoMemory);
	
	// report any other errors
	TESTEL(err==KErrNone ,err);

	Log(_L("Disconnect successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));

	}


/************************************************************************************
*    ConnectExpectError is a utility function to connect and disconnect using an AGT
*
*    The preconditions for this function are assumed to be set so that 
*    a connection attempt will fail with the error passed to the function and 
*    at the stage passed to the function.  
*    
*************************************************************************************/
void CTestStepPsdAgt::ConnectExpectErrorL(TInt aError, TInt aStage)
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==aError,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==aStage,stage);

	Log(_L("Connect failed as expected"));

	DelayL(3000000);
	
	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);
	Log(_L("Disconnect successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}

/************************************************************************************
*
*  Trigger a QoS change from the dummy ETEL layer and wait for the dialog box to
*  be presented. 
*
************************************************************************************/
void CTestStepPsdAgt::QoSChangeDisconnectL(MAgtNotify &aAgtNotify)
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect successful"));

	iDlgSvr->SetQoSWarnResponse(ETrue);

	DelayL(3000000);

	// Tell the ETEL layer to send QOS change to PSDAGT
	Log(_L("Notify PSDAGT of Context Config / QoS change"));
	aAgtNotify.NotifyAgt(EReduce);

	// Block until we receive dialog notification of status change
	CActiveScheduler::Start();

	DelayL(2000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	Log(_L("Disconnect successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}


/************************************************************************************
*
*  Trigger a QoS change from the dummy ETEL layer and wait for the dialog box to
*  be presented. 
*
*  Once dialog is dismissed, revert QoS to it's previous value and make sure
*  no extra dialog is displayed
*
************************************************************************************/
void CTestStepPsdAgt::QoSChangeNoDisconnectL(MAgtNotify & aAgtNotify)
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Request a QoS change and do not disconnect when the dialog box is presented


	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);
	
	Log(_L("Connect Successful"));

	DelayL(3000000);

	// Tell PSDAGT not to disconnect when dialog is presented
	iDlgSvr->SetQoSWarnResponse(EFalse);

	// Tell the ETEL layer to send a QOS change to PSDAGT
	Log(_L("Notify PSDAGT of Context Config / QoS change"));
	aAgtNotify.NotifyAgt(EReduce);

	// Block until we receive dialog notification of status change
	CActiveScheduler::Start();

	DelayL(2000000);

	// Tell the ETEL layer to restore QOS and notify PSDAGT
	Log(_L("Notify PSDAGT of restore Context Config / QoS change"));
	aAgtNotify.NotifyAgt(ERestore);

	// No dialog should be presented the second time, wait to make sure that no
	// dialog appears
	DelayL(3000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);
	
	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}


/************************************************************************************
*
*  Trigger a momentary QoS change from the dummy ETEL layer.
*  
*  The change should be within the timeout and no dialog box should be presented
*
************************************************************************************/
void CTestStepPsdAgt::QoSChangeWithinTimeoutL(MAgtNotify & aAgtNotify)
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Request a QoS change and revert back to the original value before the 
	// timeout expires

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));
	DelayL(3000000);

	// Tell PSDAGT not to disconnect if dialog is presented
	iDlgSvr->SetQoSWarnResponse(EFalse);

	// Tell the ETEL layer to send a QOS change to PSDAGT
	Log(_L("Notify PSDAGT of Context Config / QoS change"));
	aAgtNotify.NotifyAgt(EReduce);
	// Wait for a short time < timeout expires
	DelayL(5000);

	// Tell the ETEL layer to restore QOS and notify PSDAGT
	Log(_L("Notify PSDAGT of restore Context Config / QoS change"));
	aAgtNotify.NotifyAgt(ERestore);

	// Wait to make sure that no dialog appears
	DelayL(3000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}

/************************************************************************************
*
*  Connect, and block execution, ie. wait for an asyncronous request
*
*  The response function for the asyncronous callback should unblock
*
************************************************************************************/
void CTestStepPsdAgt::ConnectAndWaitForAsyncronousRequestL()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));
	Log(_L("Wait for EAgentToNifEventTypeGetDataTransfer event"));

	DelayL(2000000);

	iStage++;   // increment stage so we know that we are connected

	// Block until request arrives
	CActiveScheduler::Start();

	DelayL(2000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);
	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}


/************************************************************************************
*
*  Connect, and wait for 80 seconds before disconnecting
*
*
************************************************************************************/
void CTestStepPsdAgt::ConnectAndWaitForEightySecondsL()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
		
	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen, stage);
	
	Log(_L("Connect Successful"));
	Log(_L("Waiting 80 seconds for log request to ETEL"));

	// DelayL for the time it takes for one log event to occur
	DelayL(80000000); // 80 secs

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);
	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}

/************************************************************************************
*
*  Connect, and wait for 20 seconds before disconnecting
*
*
************************************************************************************/
void CTestStepPsdAgt::ConnectAndWaitForTwentySecondsL()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
		
	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen, stage);
	Log(_L("Connect Successful"));
	Log(_L("Wait for 20 seconds for NotifyStatusChange"));

	// DelayL for 20 seconds for the NotifyStatusChange in the PsdOpen state
	DelayL(2000000); // 2 secs

	iStage++;  // ie iStage = 1

	DelayL(18000000); // 18 secs

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);
	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}

