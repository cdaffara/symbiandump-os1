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
// Telephony Watchers Integration Test main test code.
// 
//

/**
 @file
*/

#include <e32uid.h>
#include <f32file.h>
#include <bafindf.h>
#include <cdbcols.h>
#include <simtsy.h>
#include <ecom/ecom.h>

#include "indicatorwatcher.h"
#include "signalstrengthwatcher.h"

#include "TE_TelWatchersIntBase.h"
#include "TE_TelWatchersIntTest.h"


/**
 *  Each test step initialises it's own name
 */
CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe::CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestRetrieveSignalStrengthWithPublishSubscribe"));
	} // CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe::CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe::~CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe::~CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe


/**
 *  This test tests the retrieval of the signal strength.
 *
 *  @test GT83-SSW-Test01 Test retrieval of the Signal Strength setting.
 */	
enum TVerdict CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe::doTestStepL()
	{
	//
	// Make sure watchers are stopped and unloaded before running this test
	// This forces SIMTSY to be loaded again.
	//
	StopWatcher();
	SetSimTsyTestNumberL(0);
	StartWatcher();
	CheckSimTsyTestNumberL(0);

	TInt  state;
	TRequestStatus  localStatus;
	RProperty       localProperty;
		
	//-- attach and subscribe to KUidNetworkStrength property
	TESTCHECKL(localProperty.Attach(KUidSystemCategory,KUidNetworkStrength.iUid), KErrNone);    
	localProperty.Subscribe(localStatus);

	TInt ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Initial Signal Strength State is %d"), state);

	INFO_PRINTF1(_L("Commence Wait for Request local status..."));
	INFO_PRINTF2(_L("Prior to wait cycle, status is %d"), localStatus.Int());
	User::WaitForRequest(localStatus);			
	INFO_PRINTF1(_L("Complete Wait for Request local status."));
	localProperty.Subscribe(localStatus);
	ret = localProperty.Get(state);	
	TESTCHECKL(ret, KErrNone);
				
	INFO_PRINTF2(_L("Signal Strength State is %d"), state);
	TESTCHECK(state, ESANetworkStrengthNone);
		
	User::WaitForRequest(localStatus);		
	ret = localProperty.Get(state);
	localProperty.Subscribe(localStatus);
	TESTCHECKL(ret, KErrNone);
			
	INFO_PRINTF2(_L("Signal Strength State is %d"),state);
	TESTCHECK(state, ESANetworkStrengthLow);

	User::WaitForRequest(localStatus);		
	ret = localProperty.Get(state);
	localProperty.Subscribe(localStatus);
	TESTCHECKL(ret, KErrNone);

	INFO_PRINTF2(_L("Signal Strength State is %d"),state);
	TESTCHECK(state, ESANetworkStrengthMedium);

	User::WaitForRequest(localStatus);	
	ret = localProperty.Get(state);	
	localProperty.Subscribe(localStatus);
	TESTCHECKL(ret, KErrNone);

	INFO_PRINTF2(_L("Signal Strength State is %d"),state);
	TESTCHECK(state, ESANetworkStrengthHigh);

	User::WaitForRequest(localStatus);		
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);

	INFO_PRINTF2(_L("Signal Strength State is %d"),state);
	TESTCHECK(state, ESANetworkStrengthUnknown);

	return TestStepResult();
	} // CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe::doTestStepL

/**
 *  Each test step initialises it's own name
 */
CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe::CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestRetrieveChargerStatusWithPublishSubscribe"));
	} // CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe::CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe::~CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe::~CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe


/**
 *  This tests the retrieval of the charger Strength.  ESAChargerNotCharging
 *  does not appear to be used in the indicatorwatcher, apart from the
 *  #ifdef'ed test code that is not available to this "in situ" test.
 *
 *  @test GT83-SSW-Test02 Test retrieval of the Battery and Charger setting.
 */	
enum TVerdict CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe::doTestStepL()
	{
	//
	// Make sure watchers are stopped and unloaded before running this test.
	// This forces SIMTSY to be loaded again.
	//
	StopWatcher();
	SetSimTsyTestNumberL(1);
	StartWatcher();

	TInt  state;
	TRequestStatus  localStatus;
	RProperty       localProperty;

	//-- attach and subscribe to KUidChargerStatus property
	TInt ret = localProperty.Attach(KUidSystemCategory,KUidChargerStatus.iUid);
	TESTCHECKL(ret, KErrNone);
	localProperty.Subscribe(localStatus);  

	//
	// Get initial battery charge state
	//
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Initial Battery Charger State is %d"), state);

	
	User::WaitForRequest(localStatus);		
	localProperty.Subscribe(localStatus);
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Battery Charger State is %d"), state);
	TESTCHECK(state, ESAChargerConnected);

	User::WaitForRequest(localStatus);		
	localProperty.Subscribe(localStatus);
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Battery Charger State is %d"), state);
	TESTCHECK(state, ESAChargerDisconnected);

	User::WaitForRequest(localStatus);		
	localProperty.Subscribe(localStatus);
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Battery Charger State is %d"), state);
	TESTCHECK(state, KErrUnknown);

	User::WaitForRequest(localStatus);
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Battery Charger State is %d"), state);
	TESTCHECK(state, ESAChargerConnected);

	CheckSimTsyTestNumberL(1);

	return TestStepResult();
	} // CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe::doTestStepL

/**
 *  Each test step initialises it's own name
 */
CTelWatchersIntTestNetworkStatusWithPublishSubscribe::CTelWatchersIntTestNetworkStatusWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestNetworkStatusWithPublishSubscribe"));
	} // CTelWatchersIntTestNetworkStatusWithPublishSubscribe::CTelWatchersIntTestNetworkStatusWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersIntTestNetworkStatusWithPublishSubscribe::~CTelWatchersIntTestNetworkStatusWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersIntTestNetworkStatusWithPublishSubscribe::~CTelWatchersIntTestNetworkStatusWithPublishSubscribe


/**
 *  This tests the retrieval of the Network Status.
 *
 *  @test GT83-SSW-Test03 Test retrieval of the Network status information.
 */	
enum TVerdict CTelWatchersIntTestNetworkStatusWithPublishSubscribe::doTestStepL()
	{
	//
	// Make sure watchers are stopped and unloaded before running this test.
	// This forces SIMTSY to be loaded again.
	//
	StopWatcher();
	SetSimTsyTestNumberL(2);
	StartWatcher();

	TInt  state;
	TRequestStatus  localStatus;
	RProperty       localProperty;

	//
	// Get initial network available status...
	//
	TInt ret = RProperty::Get(KUidSystemCategory, KUidChargerStatus.iUid, state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Initial Network Available Status is %d"), state);
		
	//-- attach and subscribe to KUidNetworkStatus property
	ret = localProperty.Attach(KUidSystemCategory,KUidNetworkStatus.iUid);
	TESTCHECKL(ret, KErrNone);
	localProperty.Subscribe(localStatus);
	
	User::WaitForRequest(localStatus);		
	localProperty.Subscribe(localStatus);
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Network Available Status is %d"), state);
	TESTCHECK(state, ESANetworkAvailable);

	User::WaitForRequest(localStatus);		
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Network Available Status is %d"), state);
	TESTCHECK(state, ESANetworkUnAvailable);

	CheckSimTsyTestNumberL(2);

	return TestStepResult();
	} // CTelWatchersIntTestNetworkStatusWithPublishSubscribe::doTestStepL

/**
 *  Each test step initialises it's own name
 */
CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe::CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestOutgoingCallStatusWithPublishSubscribe"));
	} // CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe::CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe::~CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersIntTestOutgoingCallStatus::~CTelWatchersIntTestOutgoingCallStatus


/**
 *  "Outgoing Call" function.  This function will wait until the
 *  Watchers have started and then will make and terminate a call.
 */
void CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe::OutgoingCallL()
	{
	const TInt KStartupPollPeriod = 1*1000000;
	const TInt KCallActivePeriod  = 5*1000000;

	TInt val(KErrNotFound);

	while (val < KErrNone)
		{
		User::After(KStartupPollPeriod);
		User::LeaveIfError(RProperty::Get(KUidSystemCategory, KUidCurrentCall.iUid, val));
		}

	RTelServer  telServer;

	User::LeaveIfError(telServer.Connect());
	CleanupClosePushL(telServer);

	RPhone  phone;

	User::LeaveIfError(phone.Open(telServer, KPhoneName));
	CleanupClosePushL(phone);

	RLine  line;

	User::LeaveIfError(line.Open(phone, KVoiceLineName));
	CleanupClosePushL(line);

	RCall  call;
	TName  callName;

	User::LeaveIfError(call.OpenNewCall(line, callName));
	CleanupClosePushL(call);

	_LIT(KTelNumber,"1234");

	User::LeaveIfError(call.Dial(KTelNumber));

	User::After(KCallActivePeriod);
	call.HangUp();

	CleanupStack::PopAndDestroy(&call);
	CleanupStack::PopAndDestroy(&line);
	CleanupStack::PopAndDestroy(&phone);
	CleanupStack::PopAndDestroy(&telServer);
	} // CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe::OutgoingCallL


/**
 *  "Outgoing Call Thread" function.  This function just calls OutgoingCallL.
 *
 *  @param  aPtr  A pointer to a CTelWatchersIntTestOutgoingCallStatus object.
 *  @return TInt  Standard error code.
 */
TInt CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe::OutgoingCallThreadFunction(TAny* aPtr)
	{
  	CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe*  testStep =
  			REINTERPRET_CAST(CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe*, aPtr);

	//
	// Create a clean up stack...
	//
	CTrapCleanup*  cleanup;

	cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrGeneral;
		}

	//
	// Call the OutgoingCallL() function...
	//
	TRAPD(leaveCode, testStep->OutgoingCallL());

	//
	// Clean up the clean up stack.
	//
	delete cleanup;

	return leaveCode;
	} // CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe::OutgoingCallThreadFunction


/**
 *  This tests the retrieval of the Current MO Call State.
 */	
enum TVerdict CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe::doTestStepL()
	{
	const TInt  KStackSize   = 0x8000;
	const TInt  KHeapSize    = 0x8000;
	const TInt  KMaxHeapSize = 0x80000;
	_LIT(KCallThreadName, "OutgoingCallThreadWithPublishSubscribe");
	_LIT(KOutgoingThreadSuccess, "Kill");

	//
	// Make sure watchers are stopped and unloaded before running this test
	// This forces SIMTSY to be loaded again
	//
	StopWatcher();
	SetSimTsyTestNumberL(3);

	//
	// The following is required when running all the tests in "batch" mode.
	// Since the property persists and the "Call Thread" uses the
	// KUidCurrentCall>=KErrNone as an indicator that the Watchers are loaded,
	// its important to ensure that it starts out with an error condition.
	// This will be reset by the Indicator Watcher.
	//

	TInt  initialState;
	User::LeaveIfError(RProperty::Get(KUidSystemCategory,KUidCurrentCall.iUid,initialState));
	INFO_PRINTF2(_L("Initial Call Status is %d"), initialState);
	if (initialState >= KErrNone)
		{
		User::LeaveIfError(RProperty::Set(KUidSystemCategory,KUidCurrentCall.iUid,KErrUnknown));
		}

	StartWatcher();

	//
	// Start an outgoing call in a separate thread.
	//
	RThread  thread;
	TRequestStatus  status;
	TInt  res;
	
	res = thread.Create(KCallThreadName, OutgoingCallThreadFunction,
						KStackSize, KHeapSize, KMaxHeapSize, this);
	TESTCHECKL(res, KErrNone)

	thread.Logon(status);
	thread.Resume();

	//
	// Monitor the Current Call Status through each state...
	//
	TInt  state;
	TRequestStatus  localStatus;
	RProperty localProperty;

	//-- attach and subscribe to KUidCurrentCall property
	res = localProperty.Attach(KUidSystemCategory,KUidCurrentCall.iUid);
	TESTCHECKL(res, KErrNone);

	localProperty.Subscribe(localStatus);  
	User::WaitForRequest(localStatus);			
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallNone);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);	
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallDialling);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);	
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallAlerting);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);	
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallVoice);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);	
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallDisconnecting);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallNone);

	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNone);
	TESTCHECKSTR(thread.ExitCategory(), KOutgoingThreadSuccess);
	TESTCHECK(thread.ExitReason(), KErrNone);	
	TESTCHECK(thread.ExitType(), EExitKill);

	thread.Close();

	CheckSimTsyTestNumberL(3);

 	return TestStepResult();
	} // CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe::doTestStepL

/**
 *  Each test step initialises it's own name
 */
CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestIncomingCallStatusWithPublishSubscribe"));
	} // CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::~CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::~CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe


/**
 *  "Incoming Call" function.  This function will wait until the
 *  Watchers have started and then will receive an incoming data call.
 */
void CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::IncomingCallL()
	{
	const TInt KStartupPollPeriod = 1*1000000;
	const TInt KCallRingingPeriod = 5*1000000;
	const TInt KCallActivePeriod  = 5*1000000;

	TInt val(KErrNotFound);
	
	while (val < KErrNone)
		{
		User::After(KStartupPollPeriod);
		User::LeaveIfError(RProperty::Get(KUidSystemCategory, KUidCurrentCall.iUid, val));
		}

	RTelServer telServer;
	
	User::LeaveIfError(telServer.Connect());
	CleanupClosePushL(telServer);

	RPhone phone;
	
	User::LeaveIfError(phone.Open(telServer, KPhoneName));
	CleanupClosePushL(phone);

	RLine line;

	User::LeaveIfError(line.Open(phone, KVoiceLineName));
	CleanupClosePushL(line);

	TRequestStatus  reqStatus;
	TName  callName;

	line.NotifyIncomingCall(reqStatus, callName);

	//
	// Now initiate incoming call using published property.
	// Toggle a UID to opposite state to kick SIMTSY.
	//
	TInt state;
	User::LeaveIfError(RProperty::Get(KUidPSSimTsyCategory, KPSSimTsyIncomingVoiceCall, state));
	
	if (state == 0)
		{
		state = 1;
		}
	else
		{
		state = 0;
		}

	User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyIncomingVoiceCall, state));

	User::WaitForRequest(reqStatus);
	User::LeaveIfError(reqStatus.Int());

	RCall  call;

	User::LeaveIfError(call.OpenExistingCall(line, callName));
	CleanupClosePushL(call);

	// Delay answering to allow watcher to notice ringing state
	User::After(KCallRingingPeriod);

	TInt  ret;

	ret = call.AnswerIncomingCall();
	if (ret != KErrNone)
		{
		call.HangUp();
		User::Leave(ret);
		}

	User::After(KCallActivePeriod);
	call.HangUp();

	CleanupStack::PopAndDestroy(&call);
	CleanupStack::PopAndDestroy(&line);
	CleanupStack::PopAndDestroy(&phone);
	CleanupStack::PopAndDestroy(&telServer);
	} // CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::IncomingCallThreadFunction

/**
 *  "Incoming Call Thread" function.  This function just calls IncomingCallL.
 *
 *  @param  aPtr  A pointer to a CTelWatchersIntTestIncomingCallStatus object.
 *  @return TInt  Standard error code.
 */
TInt CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::IncomingCallThreadFunction(TAny* aPtr)
	{
  	CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe*  testStep =
  			REINTERPRET_CAST(CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe*, aPtr);

	//
	// Create a clean up stack...
	//
	CTrapCleanup*  cleanup;

	cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrGeneral;
		}

	//
	// Call the IncomingCallL() function...
	//
	TRAPD(leaveCode, testStep->IncomingCallL());

	//
	// Clean up the clean up stack.
	//
	delete cleanup;

	return leaveCode;
	} // CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::IncomingCallThreadFunction

/**
 *  This tests the retrieval of the Current MT Call State.
 *
 *  @test GT83-SSW-Test04 Test retrieval of the Call status information.
 */	
enum TVerdict CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::doTestStepL()
	{
	const TInt  KStackSize   = 0x8000;
	const TInt  KHeapSize    = 0x8000;
	const TInt  KMaxHeapSize = 0x80000;
	_LIT(KCallThreadName, "IncomingCallThreadWithPublishSubscribe");
	_LIT(KIncomingThreadSuccess, "Kill");

	//
	// Make sure watchers are stopped and unloaded before running this test.
	// This forces SIMTSY to be loaded again.
	//
	StopWatcher();
	SetSimTsyTestNumberL(4);

	//
	// The following is required when running all the tests in "batch" mode.
	// Since the property persists and the "Call Thread" uses the
	// KUidCurrentCall>=KErrNone as an indicator that the Watchers are loaded,
	// its important to ensure that it starts out with an error condition.
	// This will be reset by the Indicator Watcher.
	//
	TInt initialState;
	User::LeaveIfError(RProperty::Get(KUidSystemCategory,KUidCurrentCall.iUid,initialState));

	INFO_PRINTF2(_L("Initial Call Status is %d"), initialState);
	if (initialState >= KErrNone)
		{
		User::LeaveIfError(RProperty::Set(KUidSystemCategory,KUidCurrentCall.iUid,KErrUnknown));			
		}

	StartWatcher();

	RThread  thread;
	TRequestStatus  status;
	TInt  res;
	
	res = thread.Create(KCallThreadName, IncomingCallThreadFunction,
						KStackSize, KHeapSize, KMaxHeapSize, this);
	TESTCHECKL(res, KErrNone);

	thread.Logon(status);
	thread.Resume();

	TInt  state;
	TRequestStatus  localStatus;
	RProperty localProperty;
		
	//-- attach and subscribe to KUidCurrentCall property
	res = localProperty.Attach(KUidSystemCategory,KUidCurrentCall.iUid);
	TESTCHECKL(res, KErrNone);

	localProperty.Subscribe(localStatus);  
	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallNone);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);	
	TESTCHECKL(res, KErrNone);
	
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallRinging);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);	
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallAnswering);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);	
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallVoice);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	localProperty.Subscribe(localStatus);	
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallDisconnecting);

	User::WaitForRequest(localStatus);		
	res = localProperty.Get(state);
	TESTCHECKL(res, KErrNone);
	INFO_PRINTF2(_L("Current Call Status is %d"), state);
	TESTCHECK(state, ESACallNone);

	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNone);
	TESTCHECKSTR(thread.ExitCategory(), KIncomingThreadSuccess);
	TESTCHECK(thread.ExitReason(), KErrNone);	
	TESTCHECK(thread.ExitType(), EExitKill);
	thread.Close();

	CheckSimTsyTestNumberL(4);

	return TestStepResult();
	} // CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe::doTestStepL

/**
 *  Each test step initialises it's own name
 */ 
CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe::CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestIndicatorKErrNotSupportedScenarioWithPublishSubscribe"));
	} // CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe::CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe::~CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe::~CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe


/**
 *  This tests the Indicator Error response when facing with TSY errors that
 *  cause it to disable further requests.
 */	
enum TVerdict CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe::doTestStepL()
	{
	SetSimTsyTestNumberL(5);

	//
	// Make sure watchers are stopped and unloaded before running this test.
	// This forces SIMTSY to be loaded again.
	//
	StopWatcher();
	StartWatcher();

	TInt state;
	TRequestStatus localStatus;
	RProperty      localProperty;

	//-- attach and subscribe to KUidNetworkStrength property
	TInt ret = localProperty.Attach(KUidSystemCategory,KUidNetworkStrength.iUid);
	TESTCHECKL(ret, KErrNone);
	localProperty.Subscribe(localStatus);

	//
	// Get initial signal state...
	//
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone );
	INFO_PRINTF2(_L("Initial Signal Strength State is %d"), state);

	//
	// Wait for change to known state...
	//
	User::WaitForRequest(localStatus);		
	localProperty.Subscribe(localStatus);
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Signal Strength State is %d"), state);
	TESTCHECK(state, ESANetworkStrengthNone);

	//
	// Now wait for change back to unknown state.  It should stay unknown for
	// the next 15 seconds.
	//
	User::WaitForRequest(localStatus);		
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Signal Strength State is %d"), state);
	TESTCHECK(state, ESANetworkStrengthUnknown);

	RTimer  timer;
	TRequestStatus  timerRequestStatus;
	const TTimeIntervalMicroSeconds32  timerPeriod=15*1000000;
	TInt  err;

	err = timer.CreateLocal();
	TESTCHECKL(err, KErrNone);
	timer.After(timerRequestStatus, timerPeriod);

	localProperty.Subscribe(localStatus);
	
	User::WaitForRequest(timerRequestStatus, localStatus);

	TESTCHECKL(localStatus.Int(), KRequestPending);
	TESTCHECKL(timerRequestStatus.Int(), KErrNone);

	localProperty.Cancel();
	User::WaitForRequest(localStatus);

	TESTCHECKL(localStatus.Int(), KErrCancel);
	timer.Close();

	CheckSimTsyTestNumberL(5);

	return TestStepResult();
	} // CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe::doTestStepL

/**
 *  Each test step initialises it's own name
 */
CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe::CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe"));
	} // CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe::CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe


/**
 *  Simple destructor.
 */
CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe::~CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe()
	{
	// NOP
	} // CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe::~CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe


/**
 *  This tests the Indicator Error response when faced with a TSY failure -2017
 *	and the restart of the watchers.
 */	
enum TVerdict CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe::doTestStepL()
	{
	TInt ret = RProperty::Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff);
	TESTCHECKL(ret, KErrNone);
	
	SetSimTsyTestNumberL(6);

	TInt  state;
	TRequestStatus localStatus;
	RProperty      localProperty;

	//-- attach and subscribe to KUidNetworkStrength property
	ret = localProperty.Attach(KUidSystemCategory,KUidNetworkStrength.iUid);
	TESTCHECKL(ret, KErrNone);
	localProperty.Subscribe(localStatus);
	
	User::WaitForRequest(localStatus);		
	localProperty.Subscribe(localStatus);
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	TESTCHECK(state, ESANetworkStrengthNone);

	User::WaitForRequest(localStatus);
	localProperty.Subscribe(localStatus);
	ret = localProperty.Get(state);
	TESTCHECKL(ret, KErrNone);
	INFO_PRINTF2(_L("Signal Strength State is %d"), state);
	TESTCHECK(state, ESANetworkStrengthUnknown);

	User::After(1*1000000);
	
	ret = RProperty::Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn);
	TESTCHECKL(ret, KErrNone);

	TInt previousState = state;
	do
	{
		User::WaitForRequest(localStatus);	
		localProperty.Subscribe(localStatus);
		ret = localProperty.Get(state);
		TESTCHECKL(ret, KErrNone);		
	} while (state == previousState); // wait until state changes

	INFO_PRINTF2(_L("Signal Strength State is %d"), state);
	TESTCHECK(state, ESANetworkStrengthHigh);

	CheckSimTsyTestNumberL(6);

	return TestStepResult();
	} // CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe::doTestStepL
