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

#include "tcmd_step_coopsysstatechange.h"
#include "ssmcustomcmdfactory.h"
#include <ssm/ssmstate.h>
#include <e32property.h>
#include "cmdcoopsysstatechange.h"
const TUint32 KStateAdaptationPluginPropertyKey = 0x2000D76A;
const TUid KPropertyCategory={0x2000D75B};

CCustomCmdTestCoopSysStateChange::~CCustomCmdTestCoopSysStateChange()
	{
	}

CCustomCmdTestCoopSysStateChange::CCustomCmdTestCoopSysStateChange()
	{
	SetTestStepName(KTCCustomCmdTestCoopSysStateChange);
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0001
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0007
 */

void CCustomCmdTestCoopSysStateChange::TestCustomCmdCoopSysStateChangeL()
	{
	// Setting the P and S key will route the request to the reference plugins instead of the actual plugins
	TInt err = RProperty::Define(KPropertyCategory, KStateAdaptationPluginPropertyKey, RProperty::EInt);
	TEST(KErrNone == err || KErrAlreadyExists == err);
	err = RProperty::Set(KPropertyCategory, KStateAdaptationPluginPropertyKey, 1);
	TEST(KErrNone == err);
	
	_LIT(KTESTLOG, "TestCustomCmdCoopSysStateChangeL");
	INFO_PRINTF1(KTESTLOG);

	MSsmCustomCommand* customCmdCoopSysStateChange = SsmCustomCmdFactory::CmdCoopSysStateChangeNewL();
	CSsmCustomCommandEnv* cmdEnv = NULL;
	//Initialise the command. CSsmCustomCommandEnv is not used inside the Initialize so passing
	//null should be fine.
	customCmdCoopSysStateChange->Initialize(cmdEnv);
	TRequestStatus status;

	TSsmState ssmState(2,3);
	TPckgC<TSsmState> pckgSsmState(ssmState);
	
	//Simulate as if data has been read and passed by custom command executor
	//Execute the command
	customCmdCoopSysStateChange->Execute(pckgSsmState, status);
	
	//Wait for the command to be completed
	User::WaitForRequest(status);
	TEST(KErrNone == status.Int());

	//Execute the command once again
	customCmdCoopSysStateChange->Execute(pckgSsmState, status);
	TEST(KRequestPending == status.Int());
	
	//Cancel the commmand execution.
	customCmdCoopSysStateChange->ExecuteCancel();
	
	//Refernce plugins doesnt have Cancel implementation and also by the time ExecuteCancel()
	// is called it might have completed executing Execute() function. Thats is the reason for 
	//checking the status to KErrNone instead of KErrCancel.
	User::WaitForRequest(status);
	TEST(KErrNone == status.Int());

	//Close the command
	customCmdCoopSysStateChange->Close();
	
	//Releasing the comand will delete itself.
	customCmdCoopSysStateChange->Release();
	err = RProperty::Delete(KPropertyCategory, KStateAdaptationPluginPropertyKey);
	TEST(KErrNone == err);
	}

TVerdict CCustomCmdTestCoopSysStateChange::doTestStepL()
	{
	__UHEAP_MARK;
	TestCustomCmdCoopSysStateChangeL();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
