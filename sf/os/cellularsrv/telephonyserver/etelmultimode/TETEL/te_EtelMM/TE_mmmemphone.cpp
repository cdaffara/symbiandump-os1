// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmmemphone.h"
#include "testdef.h"
#include "TE_mmcall.h"


CTestMemPhone::CTestMemPhone()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMemPhone"));
	}

enum TVerdict CTestMemPhone::doTestStepL()
	{
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test how RMobilePhone handles OOM"));

	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	CMemTestGetDetectedNetworks* getNetworks=CMemTestGetDetectedNetworks::NewLC(mmPhone, this);
    getNetworks->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();

	CMemTestGetCFStatus* getCFStatus=CMemTestGetCFStatus::NewLC(mmPhone, this);
    getCFStatus->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();

	CMemTestGetCB* getCB=CMemTestGetCB::NewLC(mmPhone, this);
    getCB->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();

	CMemTestGetCW* getCW=CMemTestGetCW::NewLC(mmPhone,this);
    getCW->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();

	CMemTestGetCcbs* getCcbs=CMemTestGetCcbs::NewLC(mmPhone, this);
    getCcbs->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();
	
	CMemTestGetPreferredNetworks* getPrNtw=CMemTestGetPreferredNetworks::NewLC(mmPhone, this);
    getPrNtw->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();

	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

/**************************************************************/
//
// Testing how CRetrieveMobilePhoneDetectedNetworks handles OOM
//
/**************************************************************/

CTestMemPhone::CMemTestGetDetectedNetworks* CTestMemPhone::CMemTestGetDetectedNetworks::NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone)
	{
    CMemTestGetDetectedNetworks* r = new (ELeave) CMemTestGetDetectedNetworks(aPhone, aTestMemPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemPhone::CMemTestGetDetectedNetworks::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemPhone::CMemTestGetDetectedNetworks::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetDetectedNetworks::Start();
	}
	
CTestMemPhone::CMemTestGetDetectedNetworks::CMemTestGetDetectedNetworks(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone) :
	CTestGetDetectedNetworks(aPhone, aTestMemPhone), iTestMemPhone(aTestMemPhone)
	{

	}

void CTestMemPhone::CMemTestGetDetectedNetworks::RunL()
	{
	CTestGetDetectedNetworks::RunL();
	__UHEAP_RESET;
	iTestMemPhone->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneDetectedNetworks OOM Test required %d allocations to succeed"),iTestMemPhone->iTestCount++,iFailEvery);
	}

TInt CTestMemPhone::CMemTestGetDetectedNetworks::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		iFailEvery++;
		StartMemTest();
		return KErrNone;
		}
	else
		return aError;
	}

/**************************************************************/
//
// Testing how CRetrieveMobilePhoneCFList handles OOM
//
/**************************************************************/

CTestMemPhone::CMemTestGetCFStatus* CTestMemPhone::CMemTestGetCFStatus::NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone)
	{
    CMemTestGetCFStatus* r = new (ELeave) CMemTestGetCFStatus(aPhone, aTestMemPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemPhone::CMemTestGetCFStatus::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemPhone::CMemTestGetCFStatus::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetCFStatus::Start();
	}
	
CTestMemPhone::CMemTestGetCFStatus::CMemTestGetCFStatus(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone) :
	CTestGetCFStatus(aPhone, aTestMemPhone), iTestMemPhone(aTestMemPhone)
	{

	}

void CTestMemPhone::CMemTestGetCFStatus::RunL()
	{
	CTestGetCFStatus::RunL();
	__UHEAP_RESET;
	iTestMemPhone->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneCFList OOM Test required %d allocations to succeed"),iTestMemPhone->iTestCount++,iFailEvery);
	}

TInt CTestMemPhone::CMemTestGetCFStatus::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		iFailEvery++;
		StartMemTest();
		return KErrNone;
		}
	else
		return aError;
	}

/**************************************************************/
//
// Testing how CRetrieveMobilePhoneCBList handles OOM
//
/**************************************************************/

CTestMemPhone::CMemTestGetCB* CTestMemPhone::CMemTestGetCB::NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone)
	{
    CMemTestGetCB* r = new (ELeave) CMemTestGetCB(aPhone, aTestMemPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemPhone::CMemTestGetCB::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemPhone::CMemTestGetCB::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetCB::Start();
	}
	
CTestMemPhone::CMemTestGetCB::CMemTestGetCB(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone) :
	CTestGetCB(aPhone, aTestMemPhone), iTestMemPhone(aTestMemPhone)
	{

	}

void CTestMemPhone::CMemTestGetCB::RunL()
	{
	CTestGetCB::RunL();
	__UHEAP_RESET;
	iTestMemPhone->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneCBList OOM Test required %d allocations to succeed"),iTestMemPhone->iTestCount++,iFailEvery);
	}

TInt CTestMemPhone::CMemTestGetCB::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		iFailEvery++;
		StartMemTest();
		return KErrNone;
		}
	else
		return aError;
	}

/**************************************************************/
//
// Testing how CRetrieveMobilePhoneCWList handles OOM
//
/**************************************************************/

CTestMemPhone::CMemTestGetCW* CTestMemPhone::CMemTestGetCW::NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone)
	{
    CMemTestGetCW* r = new (ELeave) CMemTestGetCW(aPhone, aTestMemPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemPhone::CMemTestGetCW::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemPhone::CMemTestGetCW::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetCW::Start();
	}
	
CTestMemPhone::CMemTestGetCW::CMemTestGetCW(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone) :
	CTestGetCW(aPhone, aTestMemPhone), iTestMemPhone(aTestMemPhone)
	{

	}

void CTestMemPhone::CMemTestGetCW::RunL()
	{
	CTestGetCW::RunL();
	__UHEAP_RESET;
	iTestMemPhone->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneCWList OOM Test required %d allocations to succeed"),iTestMemPhone->iTestCount++,iFailEvery);
	}

TInt CTestMemPhone::CMemTestGetCW::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		iFailEvery++;
		StartMemTest();
		return KErrNone;
		}
	else
		return aError;
	}

/**************************************************************/
//
// Testing how CRetrieveMobilePhoneCcbsList handles OOM
//
/**************************************************************/

CTestMemPhone::CMemTestGetCcbs* CTestMemPhone::CMemTestGetCcbs::NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone)
	{
    CMemTestGetCcbs* r = new (ELeave) CMemTestGetCcbs(aPhone, aTestMemPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemPhone::CMemTestGetCcbs::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemPhone::CMemTestGetCcbs::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetCcbs::Start();
	}
	
CTestMemPhone::CMemTestGetCcbs::CMemTestGetCcbs(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone) :
	CTestGetCcbs(aPhone, aTestMemPhone), iTestMemPhone(aTestMemPhone)
	{

	}

void CTestMemPhone::CMemTestGetCcbs::RunL()
	{
	CTestGetCcbs::RunL();
	__UHEAP_RESET;
	iTestMemPhone->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneCcbsList OOMTest required %d allocations to succeed"),iTestMemPhone->iTestCount++,iFailEvery);
	}

TInt CTestMemPhone::CMemTestGetCcbs::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		iFailEvery++;
		StartMemTest();
		return KErrNone;
		}
	else
		return aError;
	}


/**************************************************************/
//
// Testing how CRetrieveMobilePhonePreferredNetworks handles OOM
//
/**************************************************************/

CTestMemPhone::CMemTestGetPreferredNetworks* CTestMemPhone::CMemTestGetPreferredNetworks::NewLC(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone)
	{
    CMemTestGetPreferredNetworks* r = new (ELeave) CMemTestGetPreferredNetworks(aPhone, aTestMemPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemPhone::CMemTestGetPreferredNetworks::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemPhone::CMemTestGetPreferredNetworks::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetPreferredNetworks::Start();
	}
	
CTestMemPhone::CMemTestGetPreferredNetworks::CMemTestGetPreferredNetworks(RMobilePhone& aPhone, CTestMemPhone* aTestMemPhone) :
	CTestGetPreferredNetworks(aPhone, aTestMemPhone), iTestMemPhone(aTestMemPhone)
	{

	}

void CTestMemPhone::CMemTestGetPreferredNetworks::RunL()
	{
	CTestGetPreferredNetworks::RunL();
	__UHEAP_RESET;
	iTestMemPhone->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhonePreferredNetworks OOM Test required %d allocations to succeed"),iTestMemPhone->iTestCount++,iFailEvery);
	}

TInt CTestMemPhone::CMemTestGetPreferredNetworks::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		iFailEvery++;
		StartMemTest();
		return KErrNone;
		}
	else
		return aError;
	}
