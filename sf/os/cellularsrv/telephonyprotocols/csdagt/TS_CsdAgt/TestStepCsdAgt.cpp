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
// This contains CTestCase which is the base class for all the TestCase DLLs
// 
//

// EPOC includes
#include <e32base.h>
#include <dummyetel.h> // include here to avoid picking up the real etel.h
#include <etelmm.h> 

// Test system includes
#include "log.h"
#include "teststep.h"
#include "TestStepCsdAgt.h"
#include "TestSuiteCsdAgt.h"

// EConnectionOpen
#include <csdprog.h>

// Set CDMA or GPRS mode
#include "DbUndo.h"

// CTestStepCsdAgt

/**
 * Constructor.
 *
 */
CTestStepCsdAgt::CTestStepCsdAgt(TPtrC aName) 
	{
	iTestStepName=aName;
	// Always start with test stage zero
	iStage = 0;
	}

/**
 * Second phase constructor.
 *
 */
void CTestStepCsdAgt::ConstructL()
	{
	iDlgSvr = new(ELeave)CDialogServer();
	}

/**
 * Destructor.
 *
 */
CTestStepCsdAgt::~CTestStepCsdAgt()
	{
	delete iDlgSvr;
	}

/**
 * doTestStep is called by the framework to execute a test step
 * It performs some initialisation steps common to all CSD.Agt tests
 * before calling the doCsdAgtTestStep() function overridden in
 * the derived class.
 *
 */
enum TVerdict CTestStepCsdAgt::doTestStepL( void )
	{
	TInt processHandleCountBefore;
	TInt threadHandleCountBefore;
	TInt reqsBefore;
	
	TInt processHandleCountAfter;
	TInt threadHandleCountAfter;
	TInt reqsAfter;
	TBool bResult;

	reqsBefore= RThread().RequestCount();
	RThread().HandleCount(processHandleCountBefore, threadHandleCountBefore);

	// Create an active scheduler on the heap before the test
	CActiveScheduler *activeScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(activeScheduler);
	CActiveScheduler::Install(activeScheduler);

	if(!iDlgSvr)
			ConstructL();

	__UHEAP_MARK;
	
	bResult = iDlgSvr->Open();
	if (bResult == EFalse)
		{
		Log(_L("Cannot connect to Dialog Server"));
		return EFail;
		}

	iDlgSvr->RequestDialogAppearanceNotification(this);

	// Do the test
	iResult = doCsdAgtTestStepL();
	iDlgSvr->Close();

	DelayL(2000000);

	__UHEAP_MARKEND;

	// delete the active scheduler
	CleanupStack::PopAndDestroy(activeScheduler);

	reqsAfter= RThread().RequestCount();
	RThread().HandleCount(processHandleCountAfter, threadHandleCountAfter);
	

	TESTL(reqsAfter == reqsBefore);
	TESTL(threadHandleCountAfter == threadHandleCountBefore);
	//TESTL(processHandleCountAfter == processHandleCountBefore);

	return iResult;
	}

/**
 * Notification is called by PSDAGT when it needs to pass an event to NIFMAN
 * The call is passed up to here by our CNifAgentRefN1.
 *
 */
TInt CTestStepCsdAgt::Notification(TAgentToNifEventType aEvent, TAny* /*aInfo*/)
	{
	// This calls the appropriate handler function
	if(aEvent == EAgentToNifEventTypeDisableConnection)
		{
		CsdAgtDisconnectRequest();	
		}

	return 0;
	}

/**
 * Called by Notification function when a EAgentToNifEventTypeDisableConnection
 * event is received. Should override in derived classes when this request is 
 * expected in the test scenario
 *
 */
void CTestStepCsdAgt::CsdAgtDisconnectRequest()
	{
	// We're not expecting receive this unless the test case calls for it in 
	// which case the derived class should override this function
	Log(_L("Unexpected Disconnect Request from CSDAGT"));
	TESTE(FALSE,KErrNone);
	}

/**
 * DialogHasAppeared is called by the dummy dialog server
 * to inform the test case that a dialog has been presented
 */
void CTestStepCsdAgt::DialogHasAppeared()
	{
	// Should not get here in the default implementation of this function
	Log(_L("Unexpected dialog box"));
	TEST(FALSE);
	}


void CTestStepCsdAgt::AgentProgress(TInt , TInt )
	{
	// Normally don't care about AgentProgress messages, override when we do care
	}

/**
 * Allows other active objects to run while waiting for the specified time.
 *
 */
void CTestStepCsdAgt::DelayL(TInt aMicroseconds)
	{
	// Construct and start the timer
	TCallBack callbackfn(CTestStepCsdAgt::TimerCallback, this);
	CPeriodic *regularUpdater = CPeriodic::NewL(CActive::EPriorityStandard);
	CleanupStack::PushL(regularUpdater);
	regularUpdater->Start(aMicroseconds,aMicroseconds,callbackfn);

	// Block until timer complete
	CActiveScheduler::Start();

	// Stop and delete the timer
	regularUpdater->Cancel();
	CleanupStack::PopAndDestroy();
	}

/**
 * The callback for the DelayL function.
 *
 */
TInt CTestStepCsdAgt::TimerCallback(TAny *)
	{
	// Unblock the TAgt::DelayL() function
	CActiveScheduler::Stop();
	return 0;
	}

/**
 * Set GSM mode.
 *
 */
void CTestStepCsdAgt::SetGSMModeL(MAgtNotify &aAgtNotify)
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
	CCDConnectionPrefsRecord* connpref = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(connpref);

	connpref->iDirection = ECommDbConnectionDirectionOutgoing;
	if(!connpref->FindL(*session))
		{
		// ignore result as original code, couple of lines above table->GotoFirstRecord();
		}

	// Change IAP to GSM
	
	CCDIAPRecord* iap = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iap);

	iap->SetRecordId(KDialOutIAPIndex);
	if(!iap->FindL(*session))
		{
		// ignore result as original code, couple of lines above table->GotoFirstRecord();
		}

	connpref->iDefaultIAP = iap->RecordId();
	connpref->ModifyL(*session);

	// Cleanup
	CleanupStack::PopAndDestroy(iap);
	CleanupStack::PopAndDestroy(connpref);
	CleanupStack::PopAndDestroy(session);
	}

/**
 * Set GPRS mode.
 *
 */
void CTestStepCsdAgt::SetGPRSModeL(MAgtNotify &aAgtNotify)
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
	CCDConnectionPrefsRecord* connpref = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(connpref);

	connpref->iDirection = ECommDbConnectionDirectionOutgoing;
	if(connpref->FindL(*session))
		{
		User::Leave(KErrNotFound);
		}

	// Change IAP to GPRS
	
	CCDIAPRecord* iap = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(iap);

	iap->SetRecordId(KGprsIAPIndex);
	if(iap->FindL(*session))
		{
		User::Leave(KErrNotFound);
		}

	connpref->iDefaultIAP = iap->RecordId();
	connpref->ModifyL(*session);

	// Cleanup
	CleanupStack::PopAndDestroy(iap);
	CleanupStack::PopAndDestroy(connpref);
	CleanupStack::PopAndDestroy(session);
	}


/**
 * NormalConnect is a utility function to connect and disconnect using an Agent
 *
 * The preconditions for this function are assumed to be set so that 
 * a connection attempt will be successful. Any error will be reported to
 * the test framework as a failed test case
 *
 */
void CTestStepCsdAgt::NormalConnectL()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));

	agent = CNifAgentRefN1::NewL(this);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
		
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==ECsdConnectionOpen,stage);

	Log(_L("Connect Succeeded"));

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

/**
 * ReConnect is a utility function to connect and disconnect using an Agt.
 *
 * The preconditions for this function are assumed to be set so that 
 * a connection attempt will be successful. Any error will be reported to
 * the test framework as a failed test case
 * 
 */
void CTestStepCsdAgt::ReConnectL()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==ECsdConnectionOpen,stage);

	Log(_L("Connect Succeeded"));

	DelayL(2000000);

	// Attempt connection
	Log(_L("Reconnecting..."));
	agent->ReConnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==ECsdConnectionOpen,stage);

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

/**
 * ConnectExpectError is a utility function to connect and disconnect using an Agt
 *
 * The preconditions for this function are assumed to be set so that 
 * a connection attempt will fail with the error passed to the function and 
 * at the stage passed to the function.  
 *
 */
void CTestStepCsdAgt::ConnectExpectErrorL(TInt aError, TInt aStage)
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();

	// Check result codes
	agent->GetCompletionCode(err);
	// If the connection (wrongly) goes ahead without detecting an error, we have to leave,
	// but if we just return the unexpected KErrNone, the caller will think everything's
	// OK. Make sure that if we have got a KErrNone, we return something else, so that it
	// gets spotted as a test failure.
	if (err == KErrNone)
	   TESTEL(err==aError,KErrGeneral);
	else
	   TESTEL(err==aError,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==aStage,stage);

	Log(_L("Connect failed as expected"));

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

/**
 * Connect, and block execution, ie. wait for an asyncronous request
 *
 * The response function for the asyncronous callback should unblock
 *
 */
void CTestStepCsdAgt::ConnectAndWaitForAsyncronousRequestL()
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();

	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==ECsdConnectionOpen,stage);

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

/**
 * NormalConnectDisconnectExpectsError is a utility function to connect and disconnect using an Agt.
 *
 * The preconditions for this function are assumed to be set so that a connection 
 * attempt will succeed, but disconnect will fail with the error passed to the 
 * method and at the stage passed to the method.
 *
 */
void CTestStepCsdAgt::NormalConnectDisconnectExpectsErrorL(TInt aError, TInt aStage)
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();

	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==ECsdConnectionOpen,stage);

	// Disconnect
	DelayL(2000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err!=KErrNone,aError);
	Log(_L("Disconnect Successful"));

	// Check the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage!=ECsdConnectionOpen,aStage);

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	}
