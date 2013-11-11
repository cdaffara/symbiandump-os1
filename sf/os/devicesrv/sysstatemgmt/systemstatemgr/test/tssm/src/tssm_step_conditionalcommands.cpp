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
// NOTE:
//

// ### The idea is that the test will request the following transitions:
// InitialState 
// |	(request transition to state ConditionalCommands.Any)
// ConditionalCommands ----> There are no further transitions in this test state, so the loop terminates!
// ### To verify that the conditional commands are executed correctly the command list will have 4 conditional commands
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
#include <centralrepository.h>  
#include <featmgr/featurecontrol.h> 
#include <featmgr/featmgr.h>

#include "ssmserver.h"
#include "ssmclisrv.h"

#include "tssm_ssmclient.h"
#include "tssm_startserver.h"
#include "tssm_statepolicy_conditionalcommands.h"
#include "tssm_statepolicy_conditionalcommands.hrh"
#include "ssmtestapps.h"

#include "tssm_step_conditionalcommands.h"


//---------------- contants  ---------------------------------------

const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)

// ----- Local helper function -----
void CSsmConditionalCommandsTest::DefinePropertyL(TUint aKey, TInt aValue)
	{
	TInt err = RProperty::Define(KMySID, aKey, RProperty::EInt, EFalse);
	if(err != KErrNone && err != KErrAlreadyExists)
		{	
		INFO_PRINTF3(_L("Error when defining property 0x%x: %d"), aKey, err);
		User::Leave(err);
		}
	User::LeaveIfError(RProperty::Set(KMySID, aKey, aValue));
	}

void CSsmConditionalCommandsTest::TestPropertyL(TUint aKey, TInt aValue)
	{
	TInt value = 0;
	TInt err = RProperty::Get(KMySID, aKey, value);
	if (err == KErrNotFound)
		{
		INFO_PRINTF2(_L("Testing property 0x%x did not exist, assuming false"), aKey);
		value = 0;
		err = KErrNone;
		}
	User::LeaveIfError(err);
	// Test value is the same truth value as aValue
	INFO_PRINTF4(_L("Testing Property 0x%x has value: %d, expected: %d"), aKey, value, aValue);
	TEST(aValue == value);
	}

/**
 Local helper function to define the Future manager property
 */
void CSsmConditionalCommandsTest::DefineFML()
	{
	//For Feature manager
	const TUid KTestFMUid = {0x2001D2A6};
	const TUint32 KValidFMData = 0;

	RFeatureControl feature;
	
	feature.Open();
	CleanupClosePushL(feature);
	TBitFlags32 flags;
	flags.Set( EFeatureSupported);
	flags.Set( EFeaturePersisted);

	TFeatureEntry fEntry(KTestFMUid, flags, KValidFMData);
	TInt err = feature.AddFeature(fEntry);
	#ifdef __WINS__
	TEST(KErrNone == err || KErrAlreadyExists == err);
	#endif //__WINS__

	CleanupStack::PopAndDestroy(&feature);
	}

/**
 Local helper function to set the central repository
 */	
void CSsmConditionalCommandsTest::DefineCRL(TInt value)
	{
	TInt KCentRepKey = 0x011; 
	TUid KCentRepId = {0x2001D2A8}; 
	CRepository* repository = CRepository::NewLC(KCentRepId);
	TInt ret = repository->Set(KCentRepKey,value); 
	value = 0;
	repository->Get(KCentRepKey,value);
	CleanupStack::PopAndDestroy(repository);
	}

/** 
 Old Test CaseID 		APPFWK-CONDEVAL-00011
 Old Test CaseID 		APPFWK-CONDEVAL-00012
 New Test CaseID 		DEVSRVS-SSMA-SSM-0003
 */
	

void CSsmConditionalCommandsTest::TestConditionEvaluatorL()
	{
	INFO_PRINTF1(_L("Doing a single sync RequestStateTransition..."));
	//For publish and subscribe condition check
	const TUint KPropertyKey = 0x0012AC;
	//PS and central repository value 
	TInt value = 0x1010;
	DefinePropertyL(KPropertyKey,value);
	DefineFML();
	DefineCRL(value);
	
	//set EPropertyCondition1 to EPropertyCondition45 keys to 0 before executing the condition
	TInt condValue = 0;
	//If the condition evaluates to TRUE then it will set the PSkeys(EPropertyCondition1 to EPropertyCondition45) to 1
	TInt resOfCond = 1;
	for(TUint pSkeyid = EPropertyCondition1; pSkeyid <= EPropertyCondition45 ; pSkeyid++ )
		{
		DefinePropertyL(pSkeyid,condValue);	
		}
	RequestStateTransition();
	
	for(TUint pSkeyid = EPropertyCondition1; pSkeyid <= EPropertyCondition36; pSkeyid++ )
		{
		TestPropertyL(pSkeyid,resOfCond);	
		}
	//conditions EPropertyCondition37 to EPropertyCondition42 are negative test cases
	for(TUint pSkeyid = EPropertyCondition37; pSkeyid <= EPropertyCondition42; pSkeyid++ )
		{
		TestPropertyL(pSkeyid,condValue);	
		}
		
	//Nested condition
	TestPropertyL(EPropertyCondition43,condValue);
	TestPropertyL(EPropertyCondition44,resOfCond);
	
	//Recursive condition
	TestPropertyL(EPropertyCondition45,condValue);
/*
	DefinePropertyL(EPropertyCondition4,condValue);
	RequestStateTransition();
	TestPropertyL(EPropertyCondition4,resOfCond);*/	
	INFO_PRINTF1(_L("case completed\n"));
	}

void CSsmConditionalCommandsTest::RequestStateTransition()
	{
	RSsmStateManagerTestClient client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);
	// Wait until we have reached ESsmTestConditionalCommandsState
	RSsmStateAwareSession sas;
	TInt err = sas.Connect(KSM2UiServicesDomain3);
	TRequestStatus status1;
	sas.RequestStateNotification(status1);
	// Now that we will be notified of state change, request state change
	INFO_PRINTF1(_L("Request state transition to state ConditionalCommands...")); 
	TSsmStateTransition state1(ESsmTestConditionalCommandsState, KSsmAnySubState, 00);
	err = client.RequestStateTransition(state1);

	INFO_PRINTF2(_L("  --completion codes:  err (%d)"), err);
	TEST(err == KErrNone);
	
	TSsmState current = sas.State();
	TEST(err == KErrNone);
	while(err == KErrNone && current.MainState() != ESsmTestConditionalCommandsState)
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
	
	// clean-up
	sas.Close();
	client.Close();
	}

CSsmConditionalCommandsTest::~CSsmConditionalCommandsTest()
	{
	}

CSsmConditionalCommandsTest::CSsmConditionalCommandsTest()
	{
	SetTestStepName(KTSsmConditionalCommandsStep);
	}

TVerdict CSsmConditionalCommandsTest::doTestStepPreambleL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TInt err = fs.MkDirAll(KTestAppLogFileLoc);
	if (KErrAlreadyExists == err)
		{
		//If the folder already exists, delete any old leftover files
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);
		TBuf<KTestCommandLineMaxLength> wildPath;
		wildPath.Append(KTestAppLogFileLoc);
		wildPath.Append(_L("*.*"));
		err = fm->Delete(wildPath);
		if (KErrNotFound != err)
			{
			ERR_PRINTF2(_L("Attempt to clear out results driectory resulted in an error (%d)"),err);
			User::LeaveIfError(err);
			}
		CleanupStack::PopAndDestroy(fm);
		}
	else
		{
		ERR_PRINTF2(_L("Attempt to create results driectory resulted in an error (%d)"),err);
		User::LeaveIfError(err);
		}
	
	CleanupStack::PopAndDestroy(&fs);
	
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmConditionalCommandsTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0020
New Test CaseID 		DEVSRVS-SSMA-SSM-0003
 */
 
 

TVerdict CSsmConditionalCommandsTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmConditionalCommandsTest started...."));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating a test clone for SsmServer...\n"));
	RThread thread;
	TEST(KErrNone == StartServer(thread));	

	
	
	// Run the tests
	TRAPD(err, TestConditionEvaluatorL());
	
	TEST(err == KErrNone);

	// clean-up
	INFO_PRINTF1(_L("cleaning up"));
	thread.Kill(KErrNone);
	thread.Close();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmConditionalCommandsTest completed!!"));
	return TestStepResult();
	}


