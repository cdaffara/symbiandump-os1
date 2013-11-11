// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tsus_step_platsec.h"
#include "tsus_startserver.h"
#include "tsus_ssmsuscli.h"


CSusPlatsecTest::~CSusPlatsecTest()
	{
	}

CSusPlatsecTest::CSusPlatsecTest()
	{
	SetTestStepName(KTSusPlatsecStep);
	}


/**
 Old Test CaseID 		APPFWK-SUS-0008
 New Test CaseID 		DEVSRVS-SSMA-SUS-0013
 New Test CaseID 		DEVSRVS-SSMA-SUS-0014
 New Test CaseID 		DEVSRVS-SSMA-SUS-0015
 New Test CaseID 		DEVSRVS-SSMA-SUS-0016
 New Test CaseID 		DEVSRVS-SSMA-SUS-0017
*/
TVerdict CSusPlatsecTest::doTestStepL()
	{
	INFO_PRINTF1(_L("Test to make sure UtilityServer refuse connection for clients with incorrect capabilities"));
	
	__UHEAP_MARK;
	
	RProcess process;
	const TUint32 sid = process.SecureId();
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread, sid));
	
	//Test the RSession
	RSsmSusCliTest client;
	TInt connect = client.Connect(KTestServerName);
	TEST(KErrPermissionDenied == connect);	
	
	client.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
