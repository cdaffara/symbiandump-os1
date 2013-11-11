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

#include "tcmd_step_coopsysselftest.h"
#include "ssmcustomcmdfactory.h"
#include "cmdcoopsysselftest.h"
#include <e32property.h>

const TUint32 KStateAdaptationPluginPropertyKey = 0x2000D76A;
const TUid KPropertyCategory={0x2000D75B};

CCustomCmdTestCoopSysSelfTest::~CCustomCmdTestCoopSysSelfTest()
	{
	}

CCustomCmdTestCoopSysSelfTest::CCustomCmdTestCoopSysSelfTest()
	{
	SetTestStepName(KTCCustomCmdTestCoopSysSelfTest);
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0005
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0005
 */

void CCustomCmdTestCoopSysSelfTest::TestCustomCmdCoopSysSelfTestL()
	{
	// Setting the P and S key will route the request to the reference plugins instead of the actual plugins
    TInt err = RProperty::Define(KPropertyCategory, KStateAdaptationPluginPropertyKey, RProperty::EInt);
    TEST(KErrNone == err || KErrAlreadyExists == err);
    err = RProperty::Set(KPropertyCategory, KStateAdaptationPluginPropertyKey, 1);
    TEST(KErrNone == err);
	_LIT(KTESTLOG, "TestCustomCmdCoopSysSelfTestL");
	INFO_PRINTF1(KTESTLOG);

	MSsmCustomCommand* customCmdCoopSysSelfTest = SsmCustomCmdFactory::CmdCoopSysSelfTestNewL();

	CSsmCustomCommandEnv* cmdEnv = NULL;
	//Initialise the command. CSsmCustomCommandEnv is not used inside the Initialize so passing
	//null should be fine.
	customCmdCoopSysSelfTest->Initialize(cmdEnv);
	
	TRequestStatus status;
	TBufC8<1> dummy;
	
	//Command parameter is not used inside the Execute. So passing any dummy data should be fine
	//Execute the command
	customCmdCoopSysSelfTest->Execute(dummy, status);
	
	//Wait for the command to be completed
	User::WaitForRequest(status);
	
	TEST(KErrNone == status.Int());
	
	//Execute the command once again
	customCmdCoopSysSelfTest->Execute(dummy, status);

	TEST(KRequestPending == status.Int());
	
	//Cancel the commmand execution.
	customCmdCoopSysSelfTest->ExecuteCancel();
	User::WaitForRequest(status);
	
	//Refernce plugins doesnt have Cancel implementation and also by the time ExecuteCancel()
	// is called it might have completed executing Execute() function. Thats is the reason for 
	//checking the status to KErrNone instead of KErrCancel.
	TEST(KErrNone == status.Int());
	
	//Close the command
	customCmdCoopSysSelfTest->Close();
	
	//Releasing the comand will delete itself.
	customCmdCoopSysSelfTest->Release();
	err = RProperty::Delete(KPropertyCategory, KStateAdaptationPluginPropertyKey);
	TEST(KErrNone == err);
	}

TVerdict CCustomCmdTestCoopSysSelfTest::doTestStepL()
	{
	__UHEAP_MARK;
	TestCustomCmdCoopSysSelfTestL();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
