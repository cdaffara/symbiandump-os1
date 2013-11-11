// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#include <ssm/ssmstates.hrh>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmstatemanager.h>
#include <ssm/startupadaptation.h>
#include <ssm/startupadaptationcommands.h>
#include <ssm/startupadaptationevents.h>

// Include the header file for the class under test
#include "saastateadaptation.h"
#include "saartcadaptation.h"
#include "saasimadaptation.h"
#include "saaemergencycallrfadaptation.h"
#include "saamiscadaptation.h"
#include "startupadaptationadapter.h"


#include "ssmclayer.h"

#include "ssmdebug.h"

#include "tclayer_step_startupadaptationadapter.h"

//Include the test wrappers
#include "startupadaptationwrapper.h"

#include <s32mem.h>

/**
 * Helper class that stops the active scheduler when it's iStatus is complete
 * 
 * @internalComponent
 * @test
 */
class CRequestWaiter : public CActive
	{
public:
	/**
	 * Starts the activescheduler, stopping it when iStatus on this object is complete.
	 * Equivalent to User::WaitForRequest(iStatus) but uses the activescheduler.
	 */
	void StartWait()
		{
		if(!IsActive())
			{
			SetActive();
			}
		CActiveScheduler::Start();
		}

	// Make iStatus public for ease of use.
	using CActive::iStatus;
	// Make SetActive public for ease of use
	void SetActive()
		{
		CActive::SetActive();
		}
	
	CRequestWaiter() : CActive(EPriorityNormal)
		{
		CActiveScheduler::Add(this);
		}
protected:
	// From CActive
	void RunL()
		{
		// Ignore the completion value and just stop the active scheduler
		CActiveScheduler::Stop();
		}
	
	TInt RunError(TInt aError)
		{
		return aError;
		}
	
	void DoCancel()
		{
		// Do nothing
		}
	};

/**
 * Constructs a complete CCLayerTestStartupAdaptationAdapter object
 * @internalComponent
 */
CCLayerTestStartupAdaptationAdapter::CCLayerTestStartupAdaptationAdapter()
	{
	
	}

/**
 * Releases any resources associated with this test
 * @internalComponent
 */
CCLayerTestStartupAdaptationAdapter::~CCLayerTestStartupAdaptationAdapter()
	{
	
	}

/**
 * Test step preamble
 * @internalComponent
 */
TVerdict CCLayerTestStartupAdaptationAdapter::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/**
 * Test step - a single test step performs a series of tests.
 * 
 * See the individual doTest... methods on this class for details of the tests.
 * 
 * @internalComponent
 */
TVerdict CCLayerTestStartupAdaptationAdapter::doTestStepL()
	{
	INFO_PRINTF1(_L("Entering test for compatibility layer startup adaptation adapter"));
	__UHEAP_MARK;
	
	CActiveScheduler* as = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);
	
	TRAPD(err, doTestOnDemandL());
	TEST(err == KErrNone);
	
	TRAP(err, doTestStateAdaptationL());
	TEST(err == KErrNone);
	
	TRAP(err, doTestSimAdaptationL());
	TEST(err == KErrNone);
	
	TRAP(err, doTestRtcAdaptationL());
	TEST(err == KErrNone);
	
	TRAP(err, doTestEmergencyCallRfAdaptationL());
	TEST(err == KErrNone);
	
	TRAP(err, doTestMiscAdaptationL());
	TEST(err == KErrNone);
	
	TRAP(err, doTestConcurrentRequestsL());
	TEST(err == KErrNone);
	
	TRAP(err, doTestMultipleRequestsL());
	TEST(err == KErrNone);	
	
	TRAP(err, doTestCreationAndDestructionL());
	TEST(err == KErrNone);	
	
	CleanupStack::PopAndDestroy(as);
	
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for compatibility layer startup adaptation adapter"));
	return TestStepResult();
	}

/**
 * Test step postamble
 * @internalComponent
 */
TVerdict CCLayerTestStartupAdaptationAdapter::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}
	
/**
Old Test CaseID 		AFSS-CLAYER-0012
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0012
 */		

void CCLayerTestStartupAdaptationAdapter::doTestStateAdaptationL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Enterting doTestStateAdaptationL()"));
	
	INFO_PRINTF1(_L("Creating state adaptation"));
	MStateAdaptation* stateAdaptation = CStartupAdaptationAdapter::NewStateAdaptationL();
	TEST(stateAdaptation != NULL);
	if(stateAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create state adaptation"));
		return;
		}
	
	CRequestWaiter* request = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(request);
	
	INFO_PRINTF1(_L("Testing coop state change mapping"));
	doTestStateChangeMappingsL(stateAdaptation);
	
	INFO_PRINTF1(_L("Testing coop self test"));
	stateAdaptation->RequestCoopSysSelfTest(request->iStatus);
	StartupAdaptation::TResponsePckg okResponse(KErrNone); 
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteSelftests);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteSelftests, okResponse);	
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Testing coop self test fail"));
	stateAdaptation->RequestCoopSysSelfTest(request->iStatus);
	StartupAdaptation::TResponsePckg failResponse(KErrGeneral); 
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteSelftests);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteSelftests, failResponse);	
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	
	INFO_PRINTF1(_L("Testing coop self test cancel"));
	stateAdaptation->RequestCoopSysSelfTest(request->iStatus); 
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteSelftests);	
	TEST(request->iStatus == KRequestPending);
	stateAdaptation->RequestCancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);
	
	INFO_PRINTF1(_L("Testing coop perform shutdown actions"));
	TInt reason = 0;
	stateAdaptation->RequestCoopSysPerformShutdownActions(reason, request->iStatus);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteShutdown);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteShutdown, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Testing coop perform shutdown actions fail"));
	reason = 0;
	stateAdaptation->RequestCoopSysPerformShutdownActions(reason, request->iStatus);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteShutdown);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteShutdown, failResponse);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
				
	INFO_PRINTF1(_L("Testing coop perform RFS actions"));
	INFO_PRINTF1(_L("Testing shallow RFS"));
	stateAdaptation->RequestCoopSysPerformRfsActions(ESsmShallowRfs, request->iStatus);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteDOSRfs);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteDOSRfs, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TPckgBuf<StartupAdaptation::TRFSReason> rfsReasonBuf;
	rfsReasonBuf.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(rfsReasonBuf() == StartupAdaptation::ENormalRFS);
	
	INFO_PRINTF1(_L("Testing shallow RFS fail"));
	stateAdaptation->RequestCoopSysPerformRfsActions(ESsmShallowRfs, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteDOSRfs);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteDOSRfs, failResponse);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	rfsReasonBuf.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(rfsReasonBuf() == StartupAdaptation::ENormalRFS);
	
	INFO_PRINTF1(_L("Testing deep RFS"));
	stateAdaptation->RequestCoopSysPerformRfsActions(ESsmDeepRfs, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteDOSRfs);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteDOSRfs, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	rfsReasonBuf.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(rfsReasonBuf() == StartupAdaptation::EDeepRFS);	
	
	INFO_PRINTF1(_L("Testing deep RFS failure"));
	stateAdaptation->RequestCoopSysPerformRfsActions(ESsmDeepRfs, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteDOSRfs);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteDOSRfs, failResponse);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	rfsReasonBuf.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(rfsReasonBuf() == StartupAdaptation::EDeepRFS);
	
	INFO_PRINTF1(_L("Testing coop perform restart actions"));
	reason = StartupAdaptation::ELanguageSwitchReset;
	stateAdaptation->RequestCoopSysPerformRestartActions(reason, request->iStatus);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteReset);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteReset, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TPckgBuf<StartupAdaptation::TResetReason> restartReasonBuf;
	restartReasonBuf.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(restartReasonBuf() == reason);
	
	INFO_PRINTF1(_L("Testing coop perform restart actions fail"));
	reason = StartupAdaptation::ELanguageSwitchReset;
	stateAdaptation->RequestCoopSysPerformRestartActions(reason, request->iStatus);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteReset);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteReset, failResponse);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	restartReasonBuf.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(restartReasonBuf() == reason);
	
	INFO_PRINTF1(_L("Testing alternative coop perform restart actions"));
	reason = StartupAdaptation::ESIMStatusChangeReset;
	stateAdaptation->RequestCoopSysPerformRestartActions(reason, request->iStatus);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteReset);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteReset, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	restartReasonBuf.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(restartReasonBuf() == reason);
			
	INFO_PRINTF1(_L("Testing coop notify sys event fatal event"));
	TPckgBuf<TSsmCoopSysEventType> eventBuf;
	stateAdaptation->NotifyCoopSysEvent(eventBuf, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	StartupAdaptation::TFatalErrorTypePckg fatalErrorPckg(StartupAdaptation::EUnclassifiedError);
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EFatalError, fatalErrorPckg);
	TEST(request->iStatus == KErrNone);
	TEST(eventBuf() == ESsmFatalCoopSysError);
	
	INFO_PRINTF1(_L("Testing coop notify sys event non-fatal event"));
	stateAdaptation->NotifyCoopSysEvent(eventBuf, request->iStatus);
	TEST(request->iStatus == KRequestPending);
    fatalErrorPckg() = StartupAdaptation::ESimRemoved;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EFatalError, fatalErrorPckg);
	//Should still be pending as SIM removed goes via the SIM adaptation
	TEST(request->iStatus == KRequestPending);
	
	INFO_PRINTF1(_L("Testing coop notify sys event cancel"));
	stateAdaptation->NotifyCancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);
	
	INFO_PRINTF1(_L("Testing coop notify sys shutdown event"));
	stateAdaptation->NotifyCoopSysEvent(eventBuf, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TBuf8<1> emptyBuf;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EDOSOriginatedShutdown, emptyBuf);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(eventBuf() == ESsmShutdownDevice);
	
	INFO_PRINTF1(_L("Testing coop notify sys restart event"));
	stateAdaptation->NotifyCoopSysEvent(eventBuf, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EDOSOriginatedReset, emptyBuf);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(eventBuf() == ESsmRestartDevice);
	
	INFO_PRINTF1(_L("Releasing state adaptation"));
	stateAdaptation->Release();
	stateAdaptation = NULL;

	CleanupStack::PopAndDestroy(request);
	
	INFO_PRINTF1(_L("Heap check macro end"));
	// Delete the test data buffer
	CStartupAdaptationWrapper::DeleteLastCommandData();
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("Returning from doTestStateAdaptationL()"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0013
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0012
 */		

void CCLayerTestStartupAdaptationAdapter::doTestSimAdaptationL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Enterting doTestSimAdaptationL()"));

	INFO_PRINTF1(_L("Creating SIM adaptation"));
	MSimAdaptation* simAdaptation = CStartupAdaptationAdapter::NewSimAdaptationL();
	TEST(simAdaptation != NULL);
	if(simAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create SIM adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Testing get SIM changed true"));
	CRequestWaiter* request = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(request);
	
	TPckgBuf<TBool> boolPckg;
	StartupAdaptation::TBooleanResponsePckg booleanResponsePckg;
	
	INFO_PRINTF1(_L("Testing get SIM owned true"));
	simAdaptation->GetSimOwned(boolPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSimOwned);
	booleanResponsePckg().iErrorCode = KErrNone;
	booleanResponsePckg().iValue = ETrue;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSimOwned, booleanResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(boolPckg());
		
	
	INFO_PRINTF1(_L("Testing get SIM owned false"));
	simAdaptation->GetSimOwned(boolPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSimOwned);
	booleanResponsePckg().iErrorCode = KErrNone;
	booleanResponsePckg().iValue = EFalse;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSimOwned, booleanResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(!boolPckg());
	
	INFO_PRINTF1(_L("Testing get SIM owned fail"));
	simAdaptation->GetSimOwned(boolPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSimOwned);
	booleanResponsePckg().iErrorCode = KErrGeneral;
	booleanResponsePckg().iValue = ETrue;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSimOwned, booleanResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	
	INFO_PRINTF1(_L("Testing get SIM owned cancel"));
	simAdaptation->GetSimOwned(boolPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSimOwned);
	// Cancel instead of responding
	simAdaptation->GetCancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);
		
		
	INFO_PRINTF1(_L("Testing notify SIM event: SIM removed remembered from state test"));
	TPckgBuf<TSsmSimEventType> simEventPckg;
	StartupAdaptation::TFatalErrorTypePckg fatalErrorTypePckg;
	simAdaptation->NotifySimEvent(simEventPckg, request->iStatus);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(simEventPckg() == ESsmSimRemoved);
	
	INFO_PRINTF1(_L("Testing notify SIM event: SIM removed"));
	simAdaptation->NotifySimEvent(simEventPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	fatalErrorTypePckg() = StartupAdaptation::ESimRemoved;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EFatalError, fatalErrorTypePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(simEventPckg() == ESsmSimRemoved);
	
	INFO_PRINTF1(_L("Testing notify SIM event: SIM usable"));
	StartupAdaptation::TSimEventTypePckg simEventTypePckg;
	simAdaptation->NotifySimEvent(simEventPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	simEventTypePckg() = StartupAdaptation::ESimUsable;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::ESimEvent, simEventTypePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(simEventPckg() == ESsmSimUsable);
	
	INFO_PRINTF1(_L("Testing notify SIM event: SIM not ready"));
	simAdaptation->NotifySimEvent(simEventPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	simEventTypePckg() = StartupAdaptation::ESimNotReady;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::ESimEvent, simEventTypePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(simEventPckg() == ESsmSimNotReady);
	
	INFO_PRINTF1(_L("Testing notify SIM event: SIM readable"));
	simAdaptation->NotifySimEvent(simEventPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	simEventTypePckg() = StartupAdaptation::ESimReadable;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::ESimEvent, simEventTypePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(simEventPckg() == ESsmSimReadable);
	
	INFO_PRINTF1(_L("Testing notify SIM event ignores state events"));
	simAdaptation->NotifySimEvent(simEventPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	fatalErrorTypePckg() = StartupAdaptation::EUnclassifiedError;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EFatalError, fatalErrorTypePckg);
	TEST(request->iStatus == KRequestPending);
	
	INFO_PRINTF1(_L("Testing notify SIM event cancel"));
	simAdaptation->NotifyCancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);

	CleanupStack::PopAndDestroy(request);
	
	INFO_PRINTF1(_L("Releasing SIM adaptation"));
	simAdaptation->Release();
	simAdaptation = NULL;

	INFO_PRINTF1(_L("Heap check macro end"));
	// Delete the test data buffer
	CStartupAdaptationWrapper::DeleteLastCommandData();
	__UHEAP_MARKEND;
		
	INFO_PRINTF1(_L("Returning from doTestSimAdaptationL()"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0014
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0012
 */
void CCLayerTestStartupAdaptationAdapter::doTestRtcAdaptationL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Enterting doTestRtcAdaptationL()"));

	INFO_PRINTF1(_L("Creating RTC adaptation"));
	MRtcAdaptation* rtcAdaptation = CStartupAdaptationAdapter::NewRtcAdaptationL();
	TEST(rtcAdaptation != NULL);
	if(rtcAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create RTC adaptation"));
		return;
		}
	
	CRequestWaiter* request = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(request);
	
	INFO_PRINTF1(_L("Testing RTC validity true"));
	TPckgBuf<TBool> boolPckg;
	StartupAdaptation::TResponsePckg responsePckg;
	rtcAdaptation->ValidateRtc(boolPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EValidateRTCTime);
	// KErrNone means the RTC is valid
	responsePckg() = KErrNone;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EValidateRTCTime, responsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(boolPckg());
	
	INFO_PRINTF1(_L("Testing RTC validity false/failure - KErrGeneral"));
	rtcAdaptation->ValidateRtc(boolPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EValidateRTCTime);
	// KErrGeneral means the RTC is not valid
	responsePckg() = KErrGeneral;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EValidateRTCTime, responsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(!boolPckg());
		
	
	INFO_PRINTF1(_L("Testing RTC validity false/failure - KErrNotFound"));
	rtcAdaptation->ValidateRtc(boolPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EValidateRTCTime);
	// KErrNotFound means the RTC is invalid
	responsePckg() = KErrNotFound;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EValidateRTCTime, responsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(!boolPckg());
		
	INFO_PRINTF1(_L("Testing RTC validity cancelling"));
	rtcAdaptation->ValidateRtc(boolPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EValidateRTCTime);
	rtcAdaptation->Cancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);
	
	INFO_PRINTF1(_L("Testing setting RTC wakeup"));
	TInt64 testTime64 = 0x6677445599226600LL;
	TTime testTime(testTime64);
	TPckgC<TTime> timePckg(testTime);
	rtcAdaptation->SetWakeupAlarm(timePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ESetWakeupAlarm);
	StartupAdaptation::TWakeupAlarmPckg alarmPckg;
	alarmPckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(alarmPckg().iTime.Int64() == timePckg().Int64());
	TEST(alarmPckg().iUniversalTimeOffset.Int() == 0);
	TEST(alarmPckg().iDaylightSavingOffset.Int() == 0);
	responsePckg() = KErrNone;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::ESetWakeupAlarm, responsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Testing setting RTC wakeup failure"));
	TInt64 test2Time64 = 0x5577112299338844LL;
	TTime test2Time(test2Time64);
	TPckgC<TTime> time2Pckg(test2Time);
	rtcAdaptation->SetWakeupAlarm(time2Pckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ESetWakeupAlarm);
	alarmPckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(alarmPckg().iTime.Int64() == time2Pckg().Int64());
	TEST(alarmPckg().iUniversalTimeOffset.Int() == 0);
	TEST(alarmPckg().iDaylightSavingOffset.Int() == 0);
	responsePckg() = KErrGeneral;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::ESetWakeupAlarm, responsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	
	INFO_PRINTF1(_L("Testing unsetting RTC wakeup"));
	rtcAdaptation->UnsetWakeupAlarm(request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ECancelWakeupAlarm);
	TBuf8<1> nullBuf;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::ECancelWakeupAlarm, nullBuf);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Testing unsetting RTC wakeup cancel"));
	rtcAdaptation->UnsetWakeupAlarm(request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ECancelWakeupAlarm);
	rtcAdaptation->Cancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);	
	
	INFO_PRINTF1(_L("Releasing RTC adaptation"));
	rtcAdaptation->Release();
	rtcAdaptation = NULL;

	CleanupStack::PopAndDestroy(request);
	
	INFO_PRINTF1(_L("Heap check macro end"));
	// Delete the test data buffer
	CStartupAdaptationWrapper::DeleteLastCommandData();
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("Returning from doTestRtcAdaptationL()"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0015
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0012
 */

void CCLayerTestStartupAdaptationAdapter::doTestEmergencyCallRfAdaptationL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Enterting doTestEmergencyCallRfAdaptationL()"));
	
	INFO_PRINTF1(_L("Creating emergency call rf adaptation"));
	MEmergencyCallRfAdaptation* ecrfAdaptation = CStartupAdaptationAdapter::NewEmergencyCallRfAdaptationL();
	TEST(ecrfAdaptation != NULL);
	if(ecrfAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create emergency call rf adaptation"));
		return;
		}
		
	CRequestWaiter* request = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(request);
	
	INFO_PRINTF1(_L("Test emergency RF activation success"));
	StartupAdaptation::TResponsePckg responsePckg;
	ecrfAdaptation->ActivateRfForEmergencyCall(request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EActivateRfForEmergencyCall);
	responsePckg() = KErrNone;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EActivateRfForEmergencyCall, responsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Test emergency RF activation failure"));
	ecrfAdaptation->ActivateRfForEmergencyCall(request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EActivateRfForEmergencyCall);
	responsePckg() = KErrGeneral;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EActivateRfForEmergencyCall, responsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	
	INFO_PRINTF1(_L("Test emergency RF deactivation success"));
	ecrfAdaptation->DeactivateRfForEmergencyCall(request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EDeactivateRfAfterEmergencyCall);
	responsePckg() = KErrNone;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EDeactivateRfAfterEmergencyCall, responsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Test emergency RF deactivation failure"));
	ecrfAdaptation->DeactivateRfForEmergencyCall(request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EDeactivateRfAfterEmergencyCall);
	responsePckg() = KErrGeneral;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EDeactivateRfAfterEmergencyCall, responsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	
	INFO_PRINTF1(_L("Test emergency RF activation cancel"));
	ecrfAdaptation->ActivateRfForEmergencyCall(request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EActivateRfForEmergencyCall);
	ecrfAdaptation->Cancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);
		
		
	INFO_PRINTF1(_L("Releasing emergency call rf adaptation"));
	ecrfAdaptation->Release();
	ecrfAdaptation = NULL;

	CleanupStack::PopAndDestroy(request);
	
	INFO_PRINTF1(_L("Heap check macro end"));
	// Delete the test data buffer
	CStartupAdaptationWrapper::DeleteLastCommandData();
	__UHEAP_MARKEND;
		
	INFO_PRINTF1(_L("Returning from doTestEmergencyCallRfAdaptationL()"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0016
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0012
 */


void CCLayerTestStartupAdaptationAdapter::doTestMiscAdaptationL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Enterting doTestMiscAdaptationL()"));

	INFO_PRINTF1(_L("Creating misc. adaptation"));
	MMiscAdaptation* miscAdaptation = CStartupAdaptationAdapter::NewMiscAdaptationL();
	TEST(miscAdaptation != NULL);
	if(miscAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create misc. adaptation"));
		return;
		}
	

	CRequestWaiter* request = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(request);
	
	INFO_PRINTF1(_L("Testing get global startup mode success"));
	TPckgBuf<TInt> startupModePckg;
	StartupAdaptation::TGlobalStartupModeResponsePckg startupModeResponsePckg; 
	miscAdaptation->GetGlobalStartupMode(startupModePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetGlobalStartupMode);
	startupModeResponsePckg().iErrorCode = KErrNone;
	startupModeResponsePckg().iValue = StartupAdaptation::EAlarm;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetGlobalStartupMode, startupModeResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(startupModePckg() == StartupAdaptation::EAlarm);
	
	INFO_PRINTF1(_L("Testing get global startup mode failure"));
	miscAdaptation->GetGlobalStartupMode(startupModePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetGlobalStartupMode);
	startupModeResponsePckg().iErrorCode = KErrGeneral;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetGlobalStartupMode, startupModeResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	
	INFO_PRINTF1(_L("Testing get global startup mode cancel"));
	miscAdaptation->GetGlobalStartupMode(startupModePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetGlobalStartupMode);
	miscAdaptation->Cancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);
		
	INFO_PRINTF1(_L("Testing get hidden reset success"));
	TPckgBuf <TBool> hiddenResetPckg;
	StartupAdaptation::TBooleanResponsePckg hiddenResetResponsePckg; 
	miscAdaptation->GetHiddenReset(hiddenResetPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetHiddenReset);
	hiddenResetResponsePckg().iErrorCode = KErrNone;
	hiddenResetResponsePckg().iValue = ETrue;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetHiddenReset, hiddenResetResponsePckg);
	request->StartWait();
	TInt expectedErr = KErrNone;
	TEST(request->iStatus == expectedErr);
	INFO_PRINTF3(_L("Get hidden reset success status : %d Expected Status : %d"), request->iStatus.Int(), expectedErr);
	TEST(hiddenResetPckg() == 1);
	INFO_PRINTF2(_L("Get hidden reset package : %d"), hiddenResetPckg());
	
	INFO_PRINTF1(_L("Testing get hidden reset failure"));
	miscAdaptation->GetHiddenReset(hiddenResetPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetHiddenReset);
	hiddenResetResponsePckg().iErrorCode = KErrGeneral;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetHiddenReset, hiddenResetResponsePckg);
	request->StartWait();
	expectedErr = KErrGeneral;
	TEST(request->iStatus == expectedErr);
	INFO_PRINTF3(_L("Get hidden reset failure status : %d Expected Status : %d"), request->iStatus.Int(), expectedErr);
	
	INFO_PRINTF1(_L("Testing get hidden reset cancel"));
	miscAdaptation->GetHiddenReset(hiddenResetPckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetHiddenReset);
	miscAdaptation->Cancel();
	request->StartWait();
	expectedErr = KErrCancel;
	TEST(request->iStatus == expectedErr);
	INFO_PRINTF3(_L("Get hidden reset cancel status : %d Expected Status : %d"), request->iStatus.Int(), expectedErr);
	
	INFO_PRINTF1(_L("Testing security state change success, EAskPIN"));
	TPckgBuf<TInt> clientResponsePckg;
	StartupAdaptation::TSecurityStateResponsePckg adaptationResponsePckg;
	StartupAdaptation::TSecurityStateChangePckg statePckg;
	miscAdaptation->SecurityStateChange(StartupAdaptation::EAskPIN, clientResponsePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ESecurityStateChange);
	statePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(statePckg() == StartupAdaptation::EAskPIN);
	adaptationResponsePckg().iErrorCode = KErrNone;
	adaptationResponsePckg().iValue = StartupAdaptation::ENoValue;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::ESecurityStateChange, adaptationResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(clientResponsePckg() == adaptationResponsePckg().iValue);
	
	INFO_PRINTF1(_L("Testing security state change success, ESIMRejected"));
	miscAdaptation->SecurityStateChange(StartupAdaptation::ESIMRejected, clientResponsePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ESecurityStateChange);
	statePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(statePckg() == StartupAdaptation::ESIMRejected);
	adaptationResponsePckg().iErrorCode = KErrNone;
	adaptationResponsePckg().iValue = StartupAdaptation::EYes;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::ESecurityStateChange, adaptationResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(clientResponsePckg() == adaptationResponsePckg().iValue);
	
	INFO_PRINTF1(_L("Testing security state change failure"));
	miscAdaptation->SecurityStateChange(StartupAdaptation::ESIMBlocked, clientResponsePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ESecurityStateChange);
	statePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(statePckg() == StartupAdaptation::ESIMBlocked);
	adaptationResponsePckg().iErrorCode = KErrGeneral;
	adaptationResponsePckg().iValue = StartupAdaptation::ENo;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::ESecurityStateChange, adaptationResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	
	INFO_PRINTF1(_L("Testing security state change cancel"));	
	miscAdaptation->SecurityStateChange(StartupAdaptation::ESIMLock, clientResponsePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ESecurityStateChange);
	statePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(statePckg() == StartupAdaptation::ESIMLock);
	miscAdaptation->Cancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);
		
		
	INFO_PRINTF1(_L("Testing prepare and get SIM languages success, size 2"));
	TPckgBuf<TInt> sizePckg;
	StartupAdaptation::TLanguageListPriorityPckg languagePriorityPckg;
	StartupAdaptation::RLanguageListResponse languageListResponse; 
	StartupAdaptation::TLanguageListResponsePckg languageResponsePckg(languageListResponse);
	miscAdaptation->PrepareSimLanguages(EPrimaryLanguages, sizePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSIMLanguages);
	languagePriorityPckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(languagePriorityPckg() == StartupAdaptation::EPrimaryLanguages);
	languageListResponse.iErrorCode = KErrNone;
	languageListResponse.iLanguages.AppendL(StartupAdaptation::EPrefLangEnglish);
	languageListResponse.iLanguages.AppendL(StartupAdaptation::EPrefLangFinnish);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSIMLanguages, languageResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(sizePckg() == 2);
	
	//Getting the Sim Languages using the flat storage dynamic buffer
	CBufFlat* writeBuffer=CBufFlat::NewL(2);
	CleanupStack::PushL(writeBuffer);
	writeBuffer->ExpandL(0,2 * sizeof(TInt32));
	TRAPD(res,miscAdaptation->GetSimLanguagesL(writeBuffer, 2));
	TEST(res == KErrNone);
	
	//Reading the languages using the read stream from the buffer
	RBufReadStream readBuffer;
	readBuffer.Open(*writeBuffer,0);
	CleanupClosePushL(readBuffer);
	TInt32 language = readBuffer.ReadInt32L();
	TEST(language == StartupAdaptation::EPrefLangEnglish);
	language = readBuffer.ReadInt32L();
	TEST(language == StartupAdaptation::EPrefLangFinnish);
	
	//Reset the write Buffer
	writeBuffer->Reset();
	
	INFO_PRINTF1(_L("Testing prepare and get SIM languages success, size 1"));
	miscAdaptation->PrepareSimLanguages(EPrimaryLanguages, sizePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSIMLanguages);
	languagePriorityPckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(languagePriorityPckg() == StartupAdaptation::EPrimaryLanguages);
	languageListResponse.iErrorCode = KErrNone;
	languageListResponse.iLanguages.Remove(1); // Remove EPrefLangFinnish
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSIMLanguages, languageResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(sizePckg() == 1);

	//Getting the Sim Languages using the flat storage dynamic buffer
	writeBuffer->ResizeL(sizeof(TInt32));
	TRAP(res,miscAdaptation->GetSimLanguagesL(writeBuffer, 1));
	TEST(res == KErrNone);
		
	//Reading the languages using the read stream from the buffer
	readBuffer.Open(*writeBuffer,0);
	language = readBuffer.ReadInt32L();
	TEST(language == StartupAdaptation::EPrefLangEnglish);
	
	INFO_PRINTF1(_L("Testing get SIM languages failure, prepared sim languages count 1, size 0"));
	miscAdaptation->PrepareSimLanguages(EPrimaryLanguages, sizePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSIMLanguages);
	languagePriorityPckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(languagePriorityPckg() == StartupAdaptation::EPrimaryLanguages);
	languageListResponse.iErrorCode = KErrNone;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSIMLanguages, languageResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(sizePckg() == 1);

	writeBuffer->ResizeL(0);
	TRAP(res,miscAdaptation->GetSimLanguagesL(writeBuffer, 0));
	TEST(res == KErrArgument);
		
	INFO_PRINTF1(_L("Testing prepare and get SIM languages success, size 0"));
	miscAdaptation->PrepareSimLanguages(EPrimaryLanguages, sizePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSIMLanguages);
	languagePriorityPckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(languagePriorityPckg() == StartupAdaptation::EPrimaryLanguages);
	languageListResponse.iErrorCode = KErrNone;
	languageListResponse.iLanguages.Remove(0); // Remove EPrefLangEnglish from the array
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSIMLanguages, languageResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(sizePckg() == 0);

	//Getting the Sim Languages using the flat storage dynamic buffer
	TRAP(res,miscAdaptation->GetSimLanguagesL(writeBuffer, 0));
	TEST(res == KErrNone);

	INFO_PRINTF1(_L("Testing prepare twice and get SIM languages success, size 0"));
	miscAdaptation->PrepareSimLanguages(EPrimaryLanguages, sizePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	request->SetActive();
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSIMLanguages);
	languagePriorityPckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(languagePriorityPckg() == StartupAdaptation::EPrimaryLanguages);
	//Try preparing the list again while outstanding request
	CRequestWaiter* request2 = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(request2);
	miscAdaptation->PrepareSimLanguages(EPrimaryLanguages, sizePckg, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	languageListResponse.iErrorCode = KErrNone;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSIMLanguages, languageResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	TEST(sizePckg() == 0);
	
	//Getting the Sim Languages using the flat storage dynamic buffer
	TRAP(res,miscAdaptation->GetSimLanguagesL(writeBuffer, 0));
	TEST(res == KErrNone);

	INFO_PRINTF1(_L("Testing prepare get SIM languages failure"));
	miscAdaptation->PrepareSimLanguages(EPrimaryLanguages, sizePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSIMLanguages);
	languagePriorityPckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(languagePriorityPckg() == StartupAdaptation::EPrimaryLanguages);
	languageListResponse.iErrorCode = KErrGeneral;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSIMLanguages, languageResponsePckg);
	request->StartWait();
	TEST(request->iStatus == KErrGeneral);
	
	INFO_PRINTF1(_L("Testing prepare SIM lanuages and then cancel"));
	miscAdaptation->PrepareSimLanguages(EPrimaryLanguages, sizePckg, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSIMLanguages);
	languagePriorityPckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(languagePriorityPckg() == StartupAdaptation::EPrimaryLanguages);
	languageListResponse.iErrorCode = KErrNone;
	// Try to cancel
	miscAdaptation->Cancel();
	request->StartWait();
	TEST(request->iStatus == KErrCancel);	
	
	INFO_PRINTF1(_L("Releasing misc. adaptation"));
	miscAdaptation->Release();
	miscAdaptation = NULL;
	
	CleanupStack::PopAndDestroy(request2);
	CleanupStack::PopAndDestroy(3); //readBuffer, writeBuffer, request
	
	INFO_PRINTF1(_L("Heap check macro end"));
	// Delete the test data buffer
	CStartupAdaptationWrapper::DeleteLastCommandData();
	// Close the adaptation wrapper language array
	languageListResponse.Close();
	__UHEAP_MARKEND;
		
	INFO_PRINTF1(_L("Returning from doTestMiscAdaptationL()"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0017
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0012
 */

void CCLayerTestStartupAdaptationAdapter::doTestConcurrentRequestsL()
	{
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Enterting doTestConcurrentRequestsL()"));
	
	INFO_PRINTF1(_L("Setting up adaptation classes"));
	
	INFO_PRINTF1(_L("Creating state adaptation"));
	MStateAdaptation* stateAdaptation = CStartupAdaptationAdapter::NewStateAdaptationL();
	TEST(stateAdaptation != NULL);
	if(stateAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create state adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Creating SIM adaptation"));
	MSimAdaptation* simAdaptation = CStartupAdaptationAdapter::NewSimAdaptationL();
	TEST(simAdaptation != NULL);
	if(simAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create SIM adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Creating RTC adaptation"));
	MRtcAdaptation* rtcAdaptation = CStartupAdaptationAdapter::NewRtcAdaptationL();
	TEST(rtcAdaptation != NULL);
	if(rtcAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create RTC adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Creating emergency call rf adaptation"));
	MEmergencyCallRfAdaptation* ecrfAdaptation = CStartupAdaptationAdapter::NewEmergencyCallRfAdaptationL();
	TEST(ecrfAdaptation != NULL);
	if(ecrfAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create emergency call rf adaptation"));
		return;
		}
		
	INFO_PRINTF1(_L("Creating misc. adaptation"));
	MMiscAdaptation* miscAdaptation = CStartupAdaptationAdapter::NewMiscAdaptationL();
	TEST(miscAdaptation != NULL);
	if(miscAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create misc. adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("End of set up of adaptation classes"));
	
	INFO_PRINTF1(_L("Starting concurrency tests"));
	
	INFO_PRINTF1(_L("Testing queued requests"));
	INFO_PRINTF1(_L("Testing non-queued self test"));
	CRequestWaiter* stateRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(stateRequest);
	stateAdaptation->RequestCoopSysSelfTest(stateRequest->iStatus);
	TEST(stateRequest->iStatus == KRequestPending);
	stateRequest->SetActive();
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteSelftests);
	
	INFO_PRINTF1(_L("Testing queueing unset wakeup alarm"));
	CRequestWaiter* rtcRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(rtcRequest);
	rtcAdaptation->UnsetWakeupAlarm(rtcRequest->iStatus);
	TEST(rtcRequest->iStatus == KRequestPending);
	rtcRequest->SetActive();
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteSelftests);
	
	CRequestWaiter* busyRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(busyRequest);
		
	INFO_PRINTF1(_L("Testing busy response on state adaptation APIs"));
	stateAdaptation->RequestCoopSysPerformRestartActions(0, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	stateAdaptation->RequestCoopSysPerformRfsActions(ESsmShallowRfs, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	stateAdaptation->RequestCoopSysSelfTest(busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	stateAdaptation->RequestCoopSysPerformShutdownActions(0, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	TSsmState state;
	stateAdaptation->RequestCoopSysStateChange(state, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Testing cancelling dispatched request with queue"));
	stateAdaptation->RequestCancel();
	stateRequest->StartWait();
	TEST(stateRequest->iStatus == KErrCancel);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ECancelWakeupAlarm);
	
	INFO_PRINTF1(_L("Testing queueing activate rf for emergency calls"));
	CRequestWaiter* ecrfRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(ecrfRequest);
	ecrfAdaptation->ActivateRfForEmergencyCall(ecrfRequest->iStatus);
	TEST(ecrfRequest->iStatus == KRequestPending);
	ecrfRequest->SetActive();
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ECancelWakeupAlarm);
	
	INFO_PRINTF1(_L("Testing queueing sim owned status"));
	CRequestWaiter* simRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(simRequest);
	TPckgBuf<TBool> simOwnedPckg;
	simAdaptation->GetSimOwned(simOwnedPckg, simRequest->iStatus);
	TEST(simRequest->iStatus == KRequestPending);
	simRequest->SetActive();
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ECancelWakeupAlarm);
	
	INFO_PRINTF1(_L("Testing queueing get startup mode"));
	TPckgBuf<TInt> startupModePckg;
	CRequestWaiter* miscRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(miscRequest);
	miscAdaptation->GetGlobalStartupMode(startupModePckg, miscRequest->iStatus);
	TEST(miscRequest->iStatus == KRequestPending);
	miscRequest->SetActive();
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ECancelWakeupAlarm);
	
	INFO_PRINTF1(_L("Testing cancelling queued request (get startup mode)"));
	miscAdaptation->Cancel();
	miscRequest->StartWait();
	TEST(miscRequest->iStatus == KErrCancel);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ECancelWakeupAlarm);
	
	INFO_PRINTF1(_L("Testing re-queueing get startup mode"));
	miscAdaptation->GetGlobalStartupMode(startupModePckg, miscRequest->iStatus);
	TEST(miscRequest->iStatus == KRequestPending);
	miscRequest->SetActive();
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ECancelWakeupAlarm);
	
	INFO_PRINTF1(_L("Testing busy response on non-state adaptation APIs"));
	TBuf8<1> nullBuf;
	
	INFO_PRINTF1(_L("Testing busy response on SIM adaptation APIs"));
	simAdaptation->GetSimOwned(nullBuf, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Testing busy response on RTC adaptation APIs"));
	rtcAdaptation->SetWakeupAlarm(nullBuf, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	rtcAdaptation->UnsetWakeupAlarm(busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	rtcAdaptation->ValidateRtc(nullBuf, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Testing busy response on emergency call RF adaptation APIs"));
	ecrfAdaptation->ActivateRfForEmergencyCall(busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	ecrfAdaptation->DeactivateRfForEmergencyCall(busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Testing busy response on misc adaptation APIs"));
	miscAdaptation->GetGlobalStartupMode(nullBuf, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	miscAdaptation->PrepareSimLanguages(EPrimaryLanguages, nullBuf, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	miscAdaptation->SecurityStateChange(0, nullBuf, busyRequest->iStatus);
	busyRequest->StartWait();
	TEST(busyRequest->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Finished busy response testing"));
	
	INFO_PRINTF1(_L("Completing wakeup alarm request"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::ECancelWakeupAlarm);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::ECancelWakeupAlarm, nullBuf);
	rtcRequest->StartWait();
	TEST(rtcRequest->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Completing active rf request"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EActivateRfForEmergencyCall);
	StartupAdaptation::TResponsePckg okResponse(KErrNone);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EActivateRfForEmergencyCall, okResponse);
	ecrfRequest->StartWait();
	TEST(ecrfRequest->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Testing queued self test"));
	stateAdaptation->RequestCoopSysSelfTest(stateRequest->iStatus);
	TEST(stateRequest->iStatus == KRequestPending);
	stateRequest->SetActive();
		
	INFO_PRINTF1(_L("Completing sim owned request"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSimOwned);
	StartupAdaptation::TBooleanResponsePckg booleanResponsePckg;	
	booleanResponsePckg().iErrorCode = KErrNone;
	booleanResponsePckg().iValue = ETrue;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSimOwned, booleanResponsePckg);
	simRequest->StartWait();
	TEST(simRequest->iStatus == KErrNone);
	TEST(simOwnedPckg());
	
	INFO_PRINTF1(_L("Completing get startup mode request"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetGlobalStartupMode);
	StartupAdaptation::TGlobalStartupModeResponsePckg startupModeResponsePckg; 
	startupModeResponsePckg().iErrorCode = KErrNone;
	startupModeResponsePckg().iValue = StartupAdaptation::ECharging;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetGlobalStartupMode, startupModeResponsePckg);
	miscRequest->StartWait();
	TEST(miscRequest->iStatus == KErrNone);
	TEST(startupModePckg() == StartupAdaptation::ECharging);
	
	INFO_PRINTF1(_L("Completing self test request"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteSelftests); 
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteSelftests, okResponse);
	stateRequest->StartWait();
	TEST(stateRequest->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Testing notify coop sys events, remembering fatal event from SIM test"));
	TPckgBuf<TSsmCoopSysEventType> eventBuf;
	stateAdaptation->NotifyCoopSysEvent(eventBuf, stateRequest->iStatus);
	stateRequest->StartWait();
	TEST(stateRequest->iStatus == KErrNone);
	TEST(eventBuf() == ESsmFatalCoopSysError);
	
	INFO_PRINTF1(_L("Testing queued events for state adaptation"));
	StartupAdaptation::TFatalErrorTypePckg fatalErrorPckg(StartupAdaptation::EUnclassifiedError);
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EFatalError, fatalErrorPckg);
	
	TBuf8<1> emptyBuf;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EDOSOriginatedShutdown, emptyBuf);
	
	stateAdaptation->NotifyCoopSysEvent(eventBuf, stateRequest->iStatus);
	stateRequest->StartWait();
	TEST(stateRequest->iStatus == KErrNone);
	TEST(eventBuf() == ESsmFatalCoopSysError);
	
	stateAdaptation->NotifyCoopSysEvent(eventBuf, stateRequest->iStatus);
	stateRequest->StartWait();
	TEST(stateRequest->iStatus == KErrNone);
	TEST(eventBuf() == ESsmShutdownDevice);
	
	INFO_PRINTF1(_L("Testing queued events for SIM adaptation"));
	
	fatalErrorPckg() = StartupAdaptation::ESimRemoved;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EFatalError, fatalErrorPckg);
	
	StartupAdaptation::TSimEventTypePckg simEventTypePckg;
	simEventTypePckg() = StartupAdaptation::ESimUsable;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::ESimEvent, simEventTypePckg);
		
	fatalErrorPckg() = StartupAdaptation::ESimRemoved;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::EFatalError, fatalErrorPckg);
	
	simEventTypePckg() = StartupAdaptation::ESimNotReady;
	CStartupAdaptationWrapper::TriggerObserverEventCallbackL(StartupAdaptation::ESimEvent, simEventTypePckg);
	
	INFO_PRINTF1(_L("Testing removing queued SIM events"));
	TPckgBuf<TSsmSimEventType> simEventPckg;
	simAdaptation->NotifySimEvent(simEventPckg, simRequest->iStatus);
	simRequest->StartWait();
	TEST(simRequest->iStatus == KErrNone);
	TEST(simEventPckg() == ESsmSimRemoved);
	
	simAdaptation->NotifySimEvent(simEventPckg, simRequest->iStatus);
	simRequest->StartWait();
	TEST(simRequest->iStatus == KErrNone);
	TEST(simEventPckg() == ESsmSimUsable);
	
	simAdaptation->NotifySimEvent(simEventPckg, simRequest->iStatus);
	simRequest->StartWait();
	TEST(simRequest->iStatus == KErrNone);
	TEST(simEventPckg() == ESsmSimRemoved);
	
	simAdaptation->NotifySimEvent(simEventPckg, simRequest->iStatus);
	simRequest->StartWait();
	TEST(simRequest->iStatus == KErrNone);
	TEST(simEventPckg() == ESsmSimNotReady);
		
	INFO_PRINTF1(_L("End of test, tearing down"));
	
	CleanupStack::PopAndDestroy(miscRequest);
	CleanupStack::PopAndDestroy(simRequest);
	CleanupStack::PopAndDestroy(ecrfRequest);
	CleanupStack::PopAndDestroy(busyRequest);
	CleanupStack::PopAndDestroy(rtcRequest);
	CleanupStack::PopAndDestroy(stateRequest);
	
	INFO_PRINTF1(_L("Releasing misc. adaptation"));
	miscAdaptation->Release();
	miscAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing emergency call rf adaptation"));
	ecrfAdaptation->Release();
	ecrfAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing RTC adaptation"));
	rtcAdaptation->Release();
	rtcAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing SIM adaptation"));
	simAdaptation->Release();
	simAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing state adaptation"));
	stateAdaptation->Release();
	stateAdaptation = NULL;
	
	INFO_PRINTF1(_L("Heap check macro end"));
	// Delete the test data buffer
	CStartupAdaptationWrapper::DeleteLastCommandData();
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("Returning from doTestConcurrentRequestsL()"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0018
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0012
 */

void CCLayerTestStartupAdaptationAdapter::doTestOnDemandL()
	{
	INFO_PRINTF1(_L("Enterting doTestOnDemandRequestsL()"));
	
	INFO_PRINTF1(_L("Setting up adaptation classes"));
		
	INFO_PRINTF1(_L("Creating state adaptation"));
	MStateAdaptation* stateAdaptation = CStartupAdaptationAdapter::NewStateAdaptationL();
	TEST(stateAdaptation != NULL);
	if(stateAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create state adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Creating SIM adaptation"));
	MSimAdaptation* simAdaptation = CStartupAdaptationAdapter::NewSimAdaptationL();
	TEST(simAdaptation != NULL);
	if(simAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create SIM adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Creating RTC adaptation"));
	MRtcAdaptation* rtcAdaptation = CStartupAdaptationAdapter::NewRtcAdaptationL();
	TEST(rtcAdaptation != NULL);
	if(rtcAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create RTC adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Creating emergency call rf adaptation"));
	MEmergencyCallRfAdaptation* ecrfAdaptation = CStartupAdaptationAdapter::NewEmergencyCallRfAdaptationL();
	TEST(ecrfAdaptation != NULL);
	if(ecrfAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create emergency call rf adaptation"));
		return;
		}
		
	INFO_PRINTF1(_L("Creating misc. adaptation"));
	MMiscAdaptation* miscAdaptation = CStartupAdaptationAdapter::NewMiscAdaptationL();
	TEST(miscAdaptation != NULL);
	if(miscAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create misc. adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("End of set up of adaptation classes"));
	
	INFO_PRINTF1(_L("Starting on demand tests"));
	
	INFO_PRINTF1(_L("Asseting adaptation isn't loaded"));
	TEST(!CStartupAdaptationWrapper::IsLoaded());
	TEST(CStartupAdaptationWrapper::LastCommandId() == 0);
	
	CRequestWaiter* request = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(request);
	
	INFO_PRINTF1(_L("Requesting self test"));
	stateAdaptation->RequestCoopSysSelfTest(request->iStatus);
	TEST(request->iStatus == KRequestPending);
	//Signal completion
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteSelftests);
	StartupAdaptation::TResponsePckg okResponse(KErrNone); 
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteSelftests, okResponse);
	request->StartWait();
	
	TEST(request->iStatus == KErrNone);
	
	CleanupStack::PopAndDestroy(request);
	
	INFO_PRINTF1(_L("Asseting adaptation is loaded"));
	TEST(CStartupAdaptationWrapper::IsLoaded());
	
	INFO_PRINTF1(_L("End of test, tearing down"));
	
	INFO_PRINTF1(_L("Releasing misc. adaptation"));
	miscAdaptation->Release();
	miscAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing emergency call rf adaptation"));
	ecrfAdaptation->Release();
	ecrfAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing RTC adaptation"));
	rtcAdaptation->Release();
	rtcAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing SIM adaptation"));
	simAdaptation->Release();
	simAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing state adaptation"));
	stateAdaptation->Release();
	stateAdaptation = NULL;
	
	// Delete the test data buffer
	CStartupAdaptationWrapper::DeleteLastCommandData();
	
	INFO_PRINTF1(_L("Returning from doTestOnDemandRequestsL()"));
	}

/**
 * Tests the state mappings are correct. Used by DoTestStateAdaptation().
 * 
 * @test
 * @internalComponent
 */
void CCLayerTestStartupAdaptationAdapter::doTestStateChangeMappingsL(MStateAdaptation* aStateAdapt)
	{
	/* We need to check the mappings for every member of StartupAdaptation::TGlobalState
	 * 
	 * The substate is represented by the enum state value with the following adjustments:
	 * * minus ESWStateStartingUiServices - to have values starting from 0
	 * * add ESsmStartupSubStateCriticalStatic - to base the state values from the critical static state
	 * 
	 * Mapping is as follows:
	 * 
	 * ESWStateStartingUiServices = TSsmState(ESsmStartup, ESWStateStartingUiServices - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical); 
	 * ESWStateStartingCriticalApps = TSsmState(ESsmStartup, ESWStateStartingCriticalApps - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);  
	 * ESWStateSelfTestOK = TSsmState(ESsmStartup, ESWStateSelfTestOK - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
	 * // States for the security check phase.
	 * ESWStateSecurityCheck = TSsmState(ESsmStartup, ESWStateSecurityCheck - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * ESWStateCriticalPhaseOK = TSsmState(ESsmStartup, ESWStateCriticalPhaseOK - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * ESWStateEmergencyCallsOnly = TSsmState(ESsmStartup, ESWStateEmergencyCallsOnly - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * // Terminal states defined by the boot mode (and some other variables such as offline mode).
     * ESWStateTest = TSsmState(ESsmStartup, ESWStateTest - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * ESWStateCharging = TSsmState(ESsmStartup, ESWStateCharging - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * ESWStateAlarm = TSsmState(ESsmStartup, ESWStateAlarm - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * ESWStateNormalRfOn = TSsmState(ESsmNormal, ESWStateNormalRfOn - ESWStateStartingUiServices + ESsmStartupSubStateCriticalStatic);
     * ESWStateNormalRfOff = TSsmState(ESsmNormal, ESWStateNormalRfOff - ESWStateStartingUiServices + ESsmStartupSubStateCriticalStatic);
     * ESWStateNormalBTSap = TSsmState(ESsmNormal, ESWStateNormalBTSap - ESWStateStartingUiServices + ESsmStartupSubStateCriticalStatic);
     * // States for notifying adaptation about a terminal state change.
     * ESWStateAlarmToCharging = TSsmState(ESsmStartup, ESWStateAlarmToCharging - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * ESWStateChargingToAlarm = TSsmState(ESsmStartup, ESWStateChargingToAlarm - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * ESWStateChargingToNormal = TSsmState(ESsmStartup, ESWStateChargingToNormal - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * ESWStateAlarmToNormal = TSsmState(ESsmStartup, ESWStateAlarmToNormal - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
     * // Shutdown-related states.
     * ESWStateShuttingDown = TSsmState(ESsmShutdown, KSsmAnySubState);
     * // Error states during critical phase.
     * ESWStateFatalStartupError = TSsmState(ESsmFail, KSsmAnySubState);
     */
	
	CRequestWaiter* request = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(request);
	
	TSsmState state;
	StartupAdaptation::TGlobalStateChangePckg globalStatePckg;
	StartupAdaptation::TResponsePckg okResponse(KErrNone);
	const TInt KStartupRebaseValue = static_cast<TInt>(StartupAdaptation::ESWStateStartingUiServices)
								- static_cast<TInt>(ESsmStartupSubStateNonCritical);
	const TInt KNormalRebaseValue = static_cast<TInt>(StartupAdaptation::ESWStateStartingUiServices)
									- static_cast<TInt>(ESsmStartupSubStateCriticalStatic);
	// Import startup adaptation namespace
	using namespace StartupAdaptation;
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateStartingUiServices"));
	state.Set(ESsmStartup, ESWStateStartingUiServices - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateStartingUiServices);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateStartingCriticalApps"));
	state.Set(ESsmStartup, ESWStateStartingCriticalApps - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateStartingCriticalApps);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);

	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateSelfTestOK"));
	state.Set(ESsmStartup, ESWStateSelfTestOK - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateSelfTestOK);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateSecurityCheck"));
	state.Set(ESsmStartup, ESWStateSecurityCheck - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateSecurityCheck);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateCriticalPhaseOK"));
	state.Set(ESsmStartup, ESWStateCriticalPhaseOK - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateCriticalPhaseOK);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateEmergencyCallsOnly"));
	state.Set(ESsmStartup, ESWStateEmergencyCallsOnly - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateEmergencyCallsOnly);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateTest"));
	state.Set(ESsmStartup, ESWStateTest - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateTest);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateCharging"));
	state.Set(ESsmStartup, ESWStateCharging - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateCharging);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateAlarm"));
	state.Set(ESsmStartup, ESWStateAlarm - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateAlarm);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateNormalRfOn"));
	state.Set(ESsmNormal, ESWStateNormalRfOn - KNormalRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateNormalRfOn);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateNormalRfOff"));
	state.Set(ESsmNormal, ESWStateNormalRfOff - KNormalRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateNormalRfOff);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateNormalBTSap"));
	state.Set(ESsmNormal, ESWStateNormalBTSap - KNormalRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateNormalBTSap);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateAlarmToNormal"));
	state.Set(ESsmStartup, ESWStateAlarmToNormal - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateAlarmToNormal);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateAlarmToCharging"));
	state.Set(ESsmStartup, ESWStateAlarmToCharging - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateAlarmToCharging);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateChargingToAlarm"));
	state.Set(ESsmStartup, ESWStateChargingToAlarm - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateChargingToAlarm);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateChargingToNormal"));
	state.Set(ESsmStartup, ESWStateChargingToNormal - KStartupRebaseValue);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateChargingToNormal);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateShuttingDown"));
	state.Set(ESsmShutdown, KSsmAnySubState);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateShuttingDown);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Testing state remapping for ESWStateFatalStartupError"));
	state.Set(ESsmFail, KSsmAnySubState);
	aStateAdapt->RequestCoopSysStateChange(state, request->iStatus);
	TEST(request->iStatus == KRequestPending);
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGlobalStateChange);
	globalStatePckg.Copy(*CStartupAdaptationWrapper::LastCommandData());
	TEST(globalStatePckg() == ESWStateFatalStartupError);
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGlobalStateChange, okResponse);
	request->StartWait();
	TEST(request->iStatus == KErrNone);
	
	CleanupStack::PopAndDestroy(request);
	}

// declare exported functions for test AFSS-CLAYER-0019
IMPORT_C MEmergencyCallRfAdaptation* CreateEmergencyCallRfAdaptationL(void);
IMPORT_C MMiscAdaptation* CreateMiscAdaptationL(void);
IMPORT_C MRtcAdaptation* CreateRtcAdaptationL(void);
IMPORT_C MSimAdaptation* CreateSimAdaptationL(void);
IMPORT_C MStateAdaptation* CreateStateAdaptationL(void);

/**
Old Test CaseID 		AFSS-CLAYER-0019
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0012
 */

void CCLayerTestStartupAdaptationAdapter::doTestCreationAndDestructionL()
	{
	INFO_PRINTF1(_L("Entering test for creation and destruction of adaptations"));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Requesting Emergency Call Rf Adaptation"));
	MEmergencyCallRfAdaptation* ecrfAdaptation = CreateEmergencyCallRfAdaptationL();
	TEST(ecrfAdaptation != NULL);
	
	INFO_PRINTF1(_L("Requesting Misc Adaptation"));
	MMiscAdaptation* miscAdaptation = CreateMiscAdaptationL();
	TEST(miscAdaptation != NULL);
	
	INFO_PRINTF1(_L("Requesting RTC Adaptation"));
	MRtcAdaptation* rtcAdaptation = CreateRtcAdaptationL();
	TEST(rtcAdaptation != NULL);
	
	INFO_PRINTF1(_L("Requesting SIM Adaptation"));
	MSimAdaptation* simAdaptation = CreateSimAdaptationL();
	TEST(simAdaptation != NULL);
	
	INFO_PRINTF1(_L("Requesting State Adaptation"));
	MStateAdaptation* stateAdaptation = CreateStateAdaptationL();
	TEST(stateAdaptation != NULL);
	
	INFO_PRINTF1(_L("Releasing Emergency Call Rf Adaptation"));
	ecrfAdaptation->Release();
	
	INFO_PRINTF1(_L("Releasing Misc Adaptation"));
	miscAdaptation->Release();
	
	INFO_PRINTF1(_L("Releasing RTC Adaptation"));
	rtcAdaptation->Release();
	
	INFO_PRINTF1(_L("Releasing SIM Adaptation"));
	simAdaptation->Release();
	
	INFO_PRINTF1(_L("Releasing State Adaptation"));
	stateAdaptation->Release();
	
	INFO_PRINTF1(_L("Destroying CStartupAdaptationAdapter"));
	CStartupAdaptationAdapter::Destroy();
	
	INFO_PRINTF1(_L("Checking for memory leaks"));
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("Leaving test for creation and destruction of adaptations"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0026
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0012
 */


void CCLayerTestStartupAdaptationAdapter::doTestMultipleRequestsL()
	{
	INFO_PRINTF1(_L("Enterting doTestOnDemandRequestsL()"));
	
	INFO_PRINTF1(_L("Setting up adaptation classes"));
		
	INFO_PRINTF1(_L("Creating state adaptation"));
	MStateAdaptation* stateAdaptation = CStartupAdaptationAdapter::NewStateAdaptationL();
	TEST(stateAdaptation != NULL);
	if(stateAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create state adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Creating SIM adaptation"));
	MSimAdaptation* simAdaptation = CStartupAdaptationAdapter::NewSimAdaptationL();
	TEST(simAdaptation != NULL);
	if(simAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create SIM adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Creating RTC adaptation"));
	MRtcAdaptation* rtcAdaptation = CStartupAdaptationAdapter::NewRtcAdaptationL();
	TEST(rtcAdaptation != NULL);
	if(rtcAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create RTC adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("Creating emergency call rf adaptation"));
	MEmergencyCallRfAdaptation* ecrfAdaptation = CStartupAdaptationAdapter::NewEmergencyCallRfAdaptationL();
	TEST(ecrfAdaptation != NULL);
	if(ecrfAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create emergency call rf adaptation"));
		return;
		}
		
	INFO_PRINTF1(_L("Creating misc. adaptation"));
	MMiscAdaptation* miscAdaptation = CStartupAdaptationAdapter::NewMiscAdaptationL();
	TEST(miscAdaptation != NULL);
	if(miscAdaptation == NULL)
		{
		INFO_PRINTF1(_L("Failed to create misc. adaptation"));
		return;
		}
	
	INFO_PRINTF1(_L("End of set up of adaptation classes"));
	
	
	// State
	INFO_PRINTF1(_L("Requesting first self test"));
	CRequestWaiter* stateRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(stateRequest);
	stateAdaptation->RequestCoopSysSelfTest(stateRequest->iStatus);
	TEST(stateRequest->iStatus == KRequestPending);
	stateRequest->SetActive();
	
	INFO_PRINTF1(_L("Requesting first event notification"));
	CRequestWaiter* eventStateRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(eventStateRequest);
	TBuf8<1> emptyBuf;
	stateAdaptation->NotifyCoopSysEvent(emptyBuf, eventStateRequest->iStatus);
	TEST(eventStateRequest->iStatus == KRequestPending);
	eventStateRequest->SetActive();
	
	INFO_PRINTF1(_L("Requesting coop sys change, should fail"));
	CRequestWaiter* request2 = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(request2);
	stateAdaptation->RequestCoopSysStateChange(TSsmState(0,0), request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Requesting coop sys perform restart actions, should fail"));
	stateAdaptation->RequestCoopSysPerformRestartActions(0, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Requesting coop sys perform shutdown actions, should fail"));
	stateAdaptation->RequestCoopSysPerformShutdownActions(0, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Requesting coop sys perform rfs actions, should fail"));
	stateAdaptation->RequestCoopSysPerformRfsActions(ESsmShallowRfs, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Requesting second self test, should fail"));
	stateAdaptation->RequestCoopSysSelfTest(request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	
	INFO_PRINTF1(_L("Requesting second event notification"));
	stateAdaptation->NotifyCoopSysEvent(emptyBuf, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	// SIM
	INFO_PRINTF1(_L("Requesting first sim owned"));
	CRequestWaiter* simRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(simRequest);
	TPckgBuf<TBool> simOwnedPckg;
	simAdaptation->GetSimOwned(simOwnedPckg, simRequest->iStatus);
	TEST(simRequest->iStatus == KRequestPending);
	simRequest->SetActive();
	
	INFO_PRINTF1(_L("Requesting first SIM event notification"));
	CRequestWaiter* eventSimRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(eventSimRequest);
	simAdaptation->NotifySimEvent(emptyBuf, eventSimRequest->iStatus);
	TEST(eventSimRequest->iStatus == KRequestPending);
	eventSimRequest->SetActive();
	
	INFO_PRINTF1(_L("Requesting second SIM event notification"));
	simAdaptation->NotifySimEvent(emptyBuf, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);

	INFO_PRINTF1(_L("Requesting second SIM owned"));
	simAdaptation->GetSimOwned(emptyBuf, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	// RTC
	INFO_PRINTF1(_L("Requesting first RTC validity"));
	CRequestWaiter* rtcRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(rtcRequest);
	TPckgBuf<TBool> rtcValidityPckg;
	rtcAdaptation->ValidateRtc(rtcValidityPckg, rtcRequest->iStatus);
	TEST(rtcRequest->iStatus == KRequestPending);
	rtcRequest->SetActive();
	
	INFO_PRINTF1(_L("Requesting set wakeup alarm"));
	rtcAdaptation->SetWakeupAlarm(emptyBuf, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Requesting unset wakeup alarm"));
	rtcAdaptation->UnsetWakeupAlarm(request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Requesting second RTC validity"));
	rtcAdaptation->ValidateRtc(emptyBuf, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
		
	// Emergency Call
	INFO_PRINTF1(_L("Requesting first Emergency Call RF on"));
	CRequestWaiter* ecrfRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(ecrfRequest);
	ecrfAdaptation->ActivateRfForEmergencyCall(ecrfRequest->iStatus);
	TEST(ecrfRequest->iStatus == KRequestPending);
	ecrfRequest->SetActive();
	
	INFO_PRINTF1(_L("Requesting Emergency Call RF off"));
	ecrfAdaptation->ActivateRfForEmergencyCall(request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Requesting second Emergency Call RF on"));
	ecrfAdaptation->ActivateRfForEmergencyCall(request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
		
	// MISC
	INFO_PRINTF1(_L("Requesting first get global startup mode"));
	CRequestWaiter* miscRequest = new (ELeave) CRequestWaiter();
	CleanupStack::PushL(miscRequest);
	TPckgBuf<TInt> startupModePckg;
	miscAdaptation->GetGlobalStartupMode(startupModePckg, miscRequest->iStatus);
	TEST(miscRequest->iStatus == KRequestPending);
	miscRequest->SetActive();
	
	INFO_PRINTF1(_L("Requesting security state change"));
	miscAdaptation->SecurityStateChange(0 , emptyBuf, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
		
	INFO_PRINTF1(_L("Requesting prepare sim languages"));
	miscAdaptation->PrepareSimLanguages(ESecondaryLanguages, emptyBuf, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	INFO_PRINTF1(_L("Requesting second get global startup mode"));
	miscAdaptation->GetGlobalStartupMode(emptyBuf, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	
	INFO_PRINTF1(_L("Requesting second get global startup mode"));
	miscAdaptation->GetGlobalStartupMode(emptyBuf, request2->iStatus);
	request2->StartWait();
	TEST(request2->iStatus == KErrInUse);
	
	// End of testing cycle... clean up
	
	INFO_PRINTF1(_L("Signal completion of self test"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EExecuteSelftests);
	StartupAdaptation::TResponsePckg okResponse(KErrNone); 
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EExecuteSelftests, okResponse);
	stateRequest->StartWait();
	TEST(stateRequest->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Signal completion of SIM owned test"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetSimOwned);
	StartupAdaptation::TBooleanResponsePckg booleanResponsePckg;
	booleanResponsePckg().iErrorCode = KErrNone;
	booleanResponsePckg().iValue = EFalse;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetSimOwned, booleanResponsePckg);		
	simRequest->StartWait();
	TEST(simRequest->iStatus == KErrNone);
	TEST(!simOwnedPckg());
	
	INFO_PRINTF1(_L("Signal completion of RTC validity test"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EValidateRTCTime);
	// KErrNotFound means the RTC is not valid
	StartupAdaptation::TResponsePckg responsePckg;
	responsePckg() = KErrNotFound;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EValidateRTCTime, responsePckg);
	rtcRequest->StartWait();
	TEST(rtcRequest->iStatus == KErrNone);
	TEST(!rtcValidityPckg());	
	
	INFO_PRINTF1(_L("Signal completion of activate RF"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EActivateRfForEmergencyCall);
	responsePckg() = KErrNone;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EActivateRfForEmergencyCall, responsePckg);
	ecrfRequest->StartWait();
	TEST(ecrfRequest->iStatus == KErrNone);
	
	INFO_PRINTF1(_L("Signal completion of get startup mode"));
	TEST(CStartupAdaptationWrapper::LastCommandId() == StartupAdaptation::EGetGlobalStartupMode);
	StartupAdaptation::TGlobalStartupModeResponsePckg startupModeResponsePckg;
	startupModeResponsePckg().iErrorCode = KErrNone;
	startupModeResponsePckg().iValue = StartupAdaptation::ECharging;
	CStartupAdaptationWrapper::TriggerResponseCallbackL(StartupAdaptation::EGetGlobalStartupMode, startupModeResponsePckg);
	miscRequest->StartWait();
	TEST(miscRequest->iStatus == KErrNone);
	TEST(startupModePckg() == StartupAdaptation::ECharging);
	 
	INFO_PRINTF1(_L("Cancel state notification"));
	stateAdaptation->NotifyCancel();
	eventStateRequest->StartWait();
	TEST(eventStateRequest->iStatus == KErrCancel);
	
	INFO_PRINTF1(_L("Cancel SIM notification"));
	simAdaptation->NotifyCancel();
	eventSimRequest->StartWait();
	TEST(eventSimRequest->iStatus == KErrCancel);
	
	INFO_PRINTF1(_L("End of test, tearing down"));
	
	CleanupStack::PopAndDestroy(miscRequest);
	CleanupStack::PopAndDestroy(ecrfRequest);
	CleanupStack::PopAndDestroy(rtcRequest);
	CleanupStack::PopAndDestroy(eventSimRequest);
	CleanupStack::PopAndDestroy(simRequest);
	CleanupStack::PopAndDestroy(request2);
	CleanupStack::PopAndDestroy(eventStateRequest);
	CleanupStack::PopAndDestroy(stateRequest);
	
	INFO_PRINTF1(_L("Releasing misc. adaptation"));
	miscAdaptation->Release();
	miscAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing emergency call rf adaptation"));
	ecrfAdaptation->Release();
	ecrfAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing RTC adaptation"));
	rtcAdaptation->Release();
	rtcAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing SIM adaptation"));
	simAdaptation->Release();
	simAdaptation = NULL;
	
	INFO_PRINTF1(_L("Releasing state adaptation"));
	stateAdaptation->Release();
	stateAdaptation = NULL;
	
	// Delete the test data buffer
	CStartupAdaptationWrapper::DeleteLastCommandData();
	
	INFO_PRINTF1(_L("Returning from doTestOnDemandRequestsL()"));
	}
