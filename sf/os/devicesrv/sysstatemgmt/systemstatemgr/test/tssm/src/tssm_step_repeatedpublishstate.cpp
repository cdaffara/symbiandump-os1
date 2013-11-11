// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32property.h>
#include <ssm/ssmstates.hrh>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmdomaindefs.h>
#include "ssmserver.h"
#include "ssmclisrv.h"

#include "tssm_startserver.h"

#include "tssm_step_repeatedpublishstate.h"

#include "tssm_statepolicy_generic.h"
#include "tssm_statepolicy_generic.hrh"

#include "tssm_swppolicy_generic.h"
#include "tssm_swppolicy_generic.hrh"


//---------------- contants  ---------------------------------------

const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)


//----- Utility methods -----

void CSsmRepeatedPublishStateTest::RequestAndWaitForStateTransitionL(TSsmStateTransition& aState)
	{
	TEST(iClient.Handle() != KNullHandle);
	
	// Wait until we have reached ESsmTestConditionalCommandsState
	RSsmStateAwareSession sas;
	TInt err = sas.Connect(KSM2UiServicesDomain3);
	TRequestStatus status1;
	sas.RequestStateNotification(status1);
	
	err = iClient.RequestStateTransition(aState);

	INFO_PRINTF2(_L("  --completion codes:  err (%d)"), err);
	TEST(err == KErrNone);
	
	TSsmState current = sas.State();
	TEST(err == KErrNone);
	while(err == KErrNone && current.MainState() != aState.State().MainState() && current.SubState() != aState.State().SubState())
		{
		// Wait for next notification of state change
		User::WaitForRequest(status1);
		TEST(status1.Int() == KErrNone);
		sas.AcknowledgeAndRequestStateNotification(KErrNone, status1);
		current = sas.State();
		}
	// Clear the outstanding notification
	sas.RequestStateNotificationCancel();
	User::WaitForRequest(status1);
	TEST(status1.Int() == KErrNone || status1.Int() == KErrCancel);
	sas.Close();
	}

//---------------- Test Steps ---------------------------------------

void CSsmRepeatedPublishStateTest::DoRepeatedPublishStateL()
	{
	INFO_PRINTF1(_L("Doing a single sync RequestStateTransition then cancelling it with another..."));

	const TInt connect = iClient.Connect();
	TEST(KErrNone == connect);	
	iClient.HeapMark();
	
	INFO_PRINTF1(_L("Creating P+S value for custom command to wait on"));
	TInt err = RProperty::Define(KMySID, EPnSFnFCustomCommandValue, RProperty::EInt, EFalse);
	if(err != KErrNone && err != KErrAlreadyExists)
		{	
		INFO_PRINTF3(_L("Error when defining property 0x%x: %d"), EPnSFnFCustomCommandValue, err);
		User::Leave(err);
		}
	err = RProperty::Set(KMySID, EPnSFnFCustomCommandValue, 0);
	TEST(err == KErrNone);
	User::LeaveIfError(err);	
	
	// Request state change
	INFO_PRINTF1(_L("Request state transition to state EStateWithWaitingFnFPnSCustomCommand...")); 
	TSsmStateTransition state1(KSsmTestGenericState, EStateWithWaitingFnFPnSCustomCommand, 0);
	RequestAndWaitForStateTransitionL(state1);
	
	INFO_PRINTF1(_L("State transitions completed successfully"));
	INFO_PRINTF1(_L("Setting publish and subscribe value to complete FnF custom command"));
	err = RProperty::Set(KMySID, EPnSFnFCustomCommandValue, 1);
	TEST(err == KErrNone);
	User::LeaveIfError(err);		
	
	User::After(500000);	//Wait for 0.5sec to allow transitions to fully complete
	
	iClient.HeapMarkEnd(EDoCleanup);		//Pass EDoCleanup for cleaning up Transition Engine after the test case 	
	// clean-up
	iClient.Close();
	INFO_PRINTF1(_L("case completed\n"));
	}



//---------------- CSsmRepeatedPublishStateTest step ---------------------------------------


CSsmRepeatedPublishStateTest::~CSsmRepeatedPublishStateTest()
	{
	}

CSsmRepeatedPublishStateTest::CSsmRepeatedPublishStateTest()
	{
	SetTestStepName(KTSsmRepeatedPublishStateStep);
	}

TVerdict CSsmRepeatedPublishStateTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmRepeatedPublishStateTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0022
New Test CaseID 		DEVSRVS-SSMA-SSM-0022
 */

TVerdict CSsmRepeatedPublishStateTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmRepeatedPublishStateTest started...."));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating a test clone for SsmServer...\n"));
	RThread thread;
	TEST(KErrNone == StartServer(thread));	
	
	// Run the tests
	TRAPD(err, DoRepeatedPublishStateL());
	
	TEST(err == KErrNone);

	// clean-up
	INFO_PRINTF1(_L("cleaning up"));
	thread.Kill(KErrNone);
	thread.Close();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmRepeatedPublishStateTest completed!!"));
	return TestStepResult();
	}


