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

#include "tsus_step_loadadaptationserver.h"

CTestSusLoadAdaptationServer::~CTestSusLoadAdaptationServer ()
	{
	}
CTestSusLoadAdaptationServer::CTestSusLoadAdaptationServer ()
	{
	SetTestStepName(KTSusLoadAdaptationServerStep);
	}

TVerdict CTestSusLoadAdaptationServer::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}


/**
 Old Test CaseID 		APPFWK-SUS-0012
 New Test CaseID 		DEVSRVS-SSMA-SUS-0012
*/


TVerdict CTestSusLoadAdaptationServer::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestSusLoadAdaptationServer tests started...."));
	INFO_PRINTF1(_L("Create active scheduler."));
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	TInt err = KErrNoMemory;
	__UHEAP_MARK;
	TRAP(err,doTestLoadStateAdaptationOOML());
	TEST(err == KErrNone);
	TRAP(err,doTestLoadStateAdaptationObserversOOML());
	TEST(err == KErrNone);
	TRAP(err,doTestLoadSimAdaptationOOML());
	TEST(err == KErrNone);
	TRAP(err,doTestLoadSimAdaptationObserversOOML());
	TEST(err == KErrNone);
	TRAP(err,doTestLoadRtcAdaptationOOML());
	TEST(err == KErrNone);
	TRAP(err,doTestLoadMiscAdaptationOOML());
	TEST(err == KErrNone);
	TRAP(err,doTestLoadEmergencyCallRfAdaptationOOML());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Destroy active scheduler."));
	CleanupStack::PopAndDestroy(sched);
	INFO_PRINTF1(_L("CTestSusLoadAdaptationServer tests completed!"));
	return TestStepResult();	
	}

TVerdict CTestSusLoadAdaptationServer::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

void CTestSusLoadAdaptationServer::doTestLoadStateAdaptationOOML()
	{
	INFO_PRINTF1(_L("doTestLoadStateAdaptationOOML tests started...."));
	TInt ret = KErrNoMemory;
	
	for(TInt allocFailRate=1; ;allocFailRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
		__UHEAP_MARK;
		TRAP(ret, TestLoadStateAdaptationL());
		TEST((ret == KErrNoMemory || ret == KErrNone));
		__UHEAP_MARKEND;
		if (ret == KErrNone)
			{
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF1(_L("doTestLoadStateAdaptationOOML tests completed."));
	}

void CTestSusLoadAdaptationServer::TestLoadStateAdaptationL()
	{
	CSsmAdaptationServer* ssmAdaptationServer = CSsmAdaptationServer::NewLC();
	ssmAdaptationServer->LoadStateAdaptationL();
	CleanupStack::PopAndDestroy(ssmAdaptationServer);
	}

void CTestSusLoadAdaptationServer::doTestLoadStateAdaptationObserversOOML()
	{
	INFO_PRINTF1(_L("doTestLoadStateAdaptationObserversOOML tests started...."));
	TInt ret = KErrNoMemory;
	for(TInt allocFailRate=1; ;allocFailRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
		__UHEAP_MARK;
		TRAP(ret, TestLoadStateAdaptationObserversL());
		TEST((ret == KErrNoMemory || ret == KErrNone));
		__UHEAP_MARKEND;
		if (ret == KErrNone)
			{
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF1(_L("doTestLoadStateAdaptationObserversOOML tests completed."));
	}

void CTestSusLoadAdaptationServer::TestLoadStateAdaptationObserversL()
	{
	CSsmAdaptationServer* ssmAdaptationServer = CSsmAdaptationServer::NewLC();
	ssmAdaptationServer->LoadStateAdaptationObserversL();
	CleanupStack::PopAndDestroy(ssmAdaptationServer);
	INFO_PRINTF1(_L("End LoadStateAdaptationObserversL"));
	}

void CTestSusLoadAdaptationServer::doTestLoadSimAdaptationOOML()
	{
	INFO_PRINTF1(_L("doTestLoadSimAdaptationOOML tests started...."));
	TInt ret = KErrNoMemory;
	for(TInt allocFailRate=1; ;allocFailRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
		__UHEAP_MARK;
		TRAP(ret, TestLoadSimAdaptationL());
		TEST((ret == KErrNoMemory || ret == KErrNone));
		__UHEAP_MARKEND;
		if (ret == KErrNone)
			{
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF1(_L("doTestLoadSimAdaptationOOML tests completed."));
	}

void CTestSusLoadAdaptationServer::TestLoadSimAdaptationL()
	{
	CSsmAdaptationServer* ssmAdaptationServer = CSsmAdaptationServer::NewLC();
	ssmAdaptationServer->LoadSimAdaptationL();
	CleanupStack::PopAndDestroy(ssmAdaptationServer);
	INFO_PRINTF1(_L("End LoadSimAdaptationL"));
	}

void CTestSusLoadAdaptationServer::doTestLoadSimAdaptationObserversOOML()
	{
	INFO_PRINTF1(_L("doTestLoadSimAdaptationObserversOOML tests started...."));
	TInt ret = KErrNoMemory;
	for(TInt allocFailRate=1; ;allocFailRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
		__UHEAP_MARK;
		TRAP(ret, TestLoadSimAdaptationObserversL());
		TEST((ret == KErrNoMemory || ret == KErrNone));
		__UHEAP_MARKEND;
		if (ret == KErrNone)
			{
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF1(_L("doTestLoadSimAdaptationObserversOOML tests completed."));
	}

void CTestSusLoadAdaptationServer::TestLoadSimAdaptationObserversL()
	{
	CSsmAdaptationServer* ssmAdaptationServer = CSsmAdaptationServer::NewLC();
	ssmAdaptationServer->LoadSimAdaptationObserversL();
	CleanupStack::PopAndDestroy(ssmAdaptationServer);
	INFO_PRINTF1(_L("End LoadSimAdaptationObserversL"));
	}

void CTestSusLoadAdaptationServer::doTestLoadRtcAdaptationOOML()
	{
	INFO_PRINTF1(_L("doTestLoadRtcAdaptationOOML tests started...."));
	TInt ret = KErrNoMemory;
	for(TInt allocFailRate=1; ;allocFailRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
		__UHEAP_MARK;
		TRAP(ret, TestLoadRtcAdaptationL());
		TEST((ret == KErrNoMemory || ret == KErrNone));
		__UHEAP_MARKEND;
		if (ret == KErrNone)
			{
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF1(_L("doTestLoadRtcAdaptationOOML tests completed."));
	}

void CTestSusLoadAdaptationServer::TestLoadRtcAdaptationL()
	{
	CSsmAdaptationServer* ssmAdaptationServer = CSsmAdaptationServer::NewLC();
	ssmAdaptationServer->LoadRtcAdaptationL();
	CleanupStack::PopAndDestroy(ssmAdaptationServer);
	INFO_PRINTF1(_L("End LoadRtcAdaptationL."));
	}

void CTestSusLoadAdaptationServer::doTestLoadMiscAdaptationOOML()
	{
	INFO_PRINTF1(_L("doTestLoadMiscAdaptationOOML tests started...."));
	TInt ret = KErrNoMemory;
	for(TInt allocFailRate=1; ;allocFailRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
		__UHEAP_MARK;
		TRAP(ret, TestLoadMiscAdaptationL());
		TEST((ret == KErrNoMemory || ret == KErrNone));
		__UHEAP_MARKEND;
		if (ret == KErrNone)
			{
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF1(_L("doTestLoadMiscAdaptationOOML tests completed."));
	}

void CTestSusLoadAdaptationServer::TestLoadMiscAdaptationL()
	{
	CSsmAdaptationServer* ssmAdaptationServer = CSsmAdaptationServer::NewLC();
	ssmAdaptationServer->LoadMiscAdaptationL();
	CleanupStack::PopAndDestroy(ssmAdaptationServer);
	INFO_PRINTF1(_L("End LoadMiscAdaptationL."));
	}

void CTestSusLoadAdaptationServer::doTestLoadEmergencyCallRfAdaptationOOML()
	{
	INFO_PRINTF1(_L("doTestLoadEmergencyCallRfAdaptationOOML tests started...."));
	TInt ret = KErrNoMemory;
	for(TInt allocFailRate=1; ;allocFailRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailRate);
		__UHEAP_MARK;
		TRAP(ret, TestLoadEmergencyCallRfAdaptationL());
		TEST((ret == KErrNoMemory || ret == KErrNone));
		__UHEAP_MARKEND;
		if (ret == KErrNone)
			{
			break;
			}
		}
	__UHEAP_RESET;
	INFO_PRINTF1(_L("doTestLoadEmergencyCallRfAdaptationOOML tests completed."));
	}

void CTestSusLoadAdaptationServer::TestLoadEmergencyCallRfAdaptationL()
	{
	CSsmAdaptationServer* ssmAdaptationServer = CSsmAdaptationServer::NewLC();
	ssmAdaptationServer->LoadEmergencyCallRfAdaptationL();
	CleanupStack::PopAndDestroy(ssmAdaptationServer);
	INFO_PRINTF1(_L("End LoadEmergencyCallRfAdaptationL."));
	}
