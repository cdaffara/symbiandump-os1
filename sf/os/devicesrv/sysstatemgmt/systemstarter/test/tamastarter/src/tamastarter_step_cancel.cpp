// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <amastart.h>
#include <s32file.h>

#include "appfwk_test_utils.h"
#include "tamastarter_constants.h"
#include "tamastarter_database_utils.h"
#include "tamastarter_step_cancel.h"

_LIT(KLogFileName, "C:\\testprocslow.log");

CAppfwkAmaStarterTestStepCancel::CAppfwkAmaStarterTestStepCancel(const TDesC& aStepName)
	{	
	SetTestStepName(aStepName);
	}


	
CAppfwkAmaStarterTestStepCancel::~CAppfwkAmaStarterTestStepCancel()
	{	
	}

/**
Old Test CaseID 		APPFWK-AMA-0001
New Test CaseID 		DEVSRVS-SYSSTART-AMA-0001
 */

void CAppfwkAmaStarterTestStepCancel::TestCase1L()
	{
	// Launch the appropriate AMAStarter(s) via CAmaStart::StartL()
	
	TRequestStatus myRequestStatus;

	CAmaStart* amaStart = CAmaStart::NewL();
	CleanupStack::PushL(amaStart);
	
	amaStart->Start(TUid::Uid(0x10000017), myRequestStatus);
	TEST(myRequestStatus == KRequestPending);
	User::After(100000); // Wait for a tenth of a second to give the starter a chance to start something - if the cancel is issued without waiting, nothing is started.
	amaStart->CancelStart();	
	User::WaitForRequest(myRequestStatus);
	TEST(myRequestStatus == KErrCancel);
	INFO_PRINTF3(_L("CAmaStart->Start(TUid::Uid(0x10000017), &myRequestStatus) finished with request status '%d' (expected '%d')"), myRequestStatus.Int(), KErrCancel);
	
	//Check that the last item in the DSC didn't run
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFileReadStream rs;
	TInt ret = rs.Open(fs, KLogFileName, EFileShareExclusive|EFileRead);
	if (KErrNone != ret)
		{
		ERR_PRINTF2(_L("Could not open log file. Leave code: '%d'"), ret);
		User::LeaveIfError(ret);
		}
	rs.PushL();
	const TInt KMaxCommandLength = 256;
	TBuf<KMaxCommandLength> commandLine;
	rs.ReadL(commandLine,1);
	TEST(commandLine.Compare(_L("9")) != 0);
	INFO_PRINTF2(_L("Checking log result: Expected not '9', Actual '%s'"), &commandLine);
	CleanupStack::PopAndDestroy(); //rs
	
	ret = fs.Delete(KLogFileName);
	if (KErrNone != ret)
		{
		ERR_PRINTF2(_L("Could not delete log file. Leave code: '%d'"), ret);
		User::LeaveIfError(ret);
		}

	// clean-up
	CleanupStack::PopAndDestroy(2,amaStart); // fs, amaStart
	}

/**
Old Test CaseID 		APPFWK-AMA-0002
New Test CaseID 		DEVSRVS-SYSSTART-AMA-0001
 */	

void CAppfwkAmaStarterTestStepCancel::TestCase2L()
	{	
	INFO_PRINTF1(_L("AMAStarter Cancel test started...."));

	CAmaStart* amaStart = CAmaStart::NewL();
	CleanupStack::PushL(amaStart);

	amaStart->CancelStart();

	// clean-up
	CleanupStack::PopAndDestroy(amaStart);

	INFO_PRINTF1(_L("...AMAStarter Cancel test finished!!"));
	}
	
TVerdict CAppfwkAmaStarterTestStepCancel::doTestStepL()
	{	
	INFO_PRINTF1(_L("AMAStarter Cancel starting DSC tests started...."));

 	__UHEAP_MARK;
 	
	// Create DBMS / SQL data store backups and delete original data stores
	TRAPD(ret, DscDatabaseBackupL());
	TESTE(ret == KErrNone, ret);

	INFO_PRINTF1(_L("Create appropriate data store for the tests"));
	TRAP(ret, CreateTestDatabaseL());
	TESTE(ret == KErrNone, ret);

	INFO_PRINTF1(_L("Run the test cases"));
	TRAP(ret, TestCase1L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase1L() finished with code '%d'\n"), ret);
	
	TRAP(ret, TestCase2L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase1L() finished with code '%d'\n"), ret);

	INFO_PRINTF1(_L("Restore DBMS / SQL data store backups available prior to running the test"));
	TRAP(ret, DscDatabaseRestoreL());
	TESTE(ret == KErrNone, ret);

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("...AMAStarter Cancel starting DSC tests finished!!"));

	return TestStepResult();
	}
