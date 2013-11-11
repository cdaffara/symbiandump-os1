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


#include "tsus_step_platsec2.h"
#include "tsus_startserver.h"
#include "tsus_ssmsuscli.h"


CSusPlatsecTestProtSrvBegin::~CSusPlatsecTestProtSrvBegin() {}

CSusPlatsecTestProtSrvBegin::CSusPlatsecTestProtSrvBegin()	
	{
	SetTestStepName(KTSusPlatsecStepProtSrvBegin);
	}

/** Start server */
TVerdict CSusPlatsecTestProtSrvBegin::doTestStepL()
	{
	RProcess process;
	const TUint32 sid = process.SecureId();
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread, sid));
	CleanupStack::PopAndDestroy(&thread);	
	return TestStepResult();
	}


CSusPlatsecTestProtSrv::~CSusPlatsecTestProtSrv() {}

CSusPlatsecTestProtSrv::CSusPlatsecTestProtSrv()
	{
	SetTestStepName(KTSusPlatsecStepProtSrv);
	}


/**
 Old Test CaseID 		APPFWK-SUS-0009
 New Test CaseID 		DEVSRVS-SSMA-SUS-0019
*/
TVerdict CSusPlatsecTestProtSrv::doTestStepL()
	{
	INFO_PRINTF1(_L("Test to make sure UtilityServer refuse connection for clients without ProtSrv"));
	
	__UHEAP_MARK;
		
	//Test the RSession
	RSsmSusCliTest client;
	TInt connect = client.Connect(KTestServerName);
	TEST(KErrPermissionDenied == connect);	
	client.Close();
	
	__UHEAP_MARKEND;
	return TestStepResult();
	}

CSusPlatsecTestProtSrvEnd::~CSusPlatsecTestProtSrvEnd() {}

CSusPlatsecTestProtSrvEnd::CSusPlatsecTestProtSrvEnd()
	{
	SetTestStepName(KTSusPlatsecStepProtSrvEnd);
	}

/** Kill server */
TVerdict CSusPlatsecTestProtSrvEnd::doTestStepL()
	{
	INFO_PRINTF1(_L("Closing test UtilityServer"));
	RSsmSusCliTest client;
	TInt err = client.Connect(KTestServerName);
	INFO_PRINTF2(_L("***** client.Connect() = %d *****"),err);
	TEST(KErrNone == err);	
	err = client.Shutdown();
	INFO_PRINTF2(_L("***** client.Shutdown() = %d *****"),err);
	TEST(KErrNone == err);	
	// Shutdown() kills the server so we don't want to call any functions on the server.
	// Only release the handle.
	client.Close(EFalse);
	return TestStepResult();
	}
