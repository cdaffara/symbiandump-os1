// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the cpp file for MM Authorization Data
// 
//

// Symbian OS includes
#include <e32base.h>

#include "ETELMM.H"

#include "testdef.h"

#include "TE_mmauthorization.h"


CEtelMMTestAuthorizationData::CEtelMMTestAuthorizationData()
{
	// store the name of this test case
	SetTestStepName(_L("TestAuthorizationData"));
}

// destructor
CEtelMMTestAuthorizationData::~CEtelMMTestAuthorizationData()
{
}

void CEtelMMTestAuthorizationData::Test( TBool aCondition)
{
	TEST(aCondition);
}

enum TVerdict CEtelMMTestAuthorizationData::doTestStepL( void )
/**
 * Test step AuthorizationData
 *
 */
{
__UHEAP_MARK;
	
	TInt ret(KErrNone);
	TRequestStatus reqStatus;
	
	// test serialization
	
	RMobilePhone::CImsAuthorizationInfoV5* authInfo=RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfo);
	authInfo->iIMPI = DMMTSY_PHONE1_IMPI;
	authInfo->iIMPUArray.Append(DMMTSY_PHONE1_IMPU0);
	authInfo->iHomeNetworkDomainName = DMMTSY_PHONE1_HNDN;
	authInfo->iAuthenticationDataSource=DMMTSY_PHONE1_DATASOURCE;
	HBufC8* hbuf(NULL);
	authInfo->ExternalizeL(hbuf);	
	CleanupStack::PushL(hbuf);
	
	RMobilePhone::CImsAuthorizationInfoV5* authInfoCopy=RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfoCopy);
	
	TBuf8<256> buffBig;
	buffBig=*hbuf;

	//OOM test for internalize
	TInt err=KErrNone;
	TInt len=0;
	do
		{
		buffBig.SetLength(len);
		TRAP(err, authInfoCopy->InternalizeL(buffBig));
		if(err == KErrOverflow)
			{
			len++;	
			}
		else
			{
			TEST(err == KErrNone);
			break;
			}		
		}
	while(len < 100); //just in case, set a limit to finish the loop.
	TEST(len<100);



	authInfoCopy->InternalizeL(buffBig);
	TRAP(ret,authInfoCopy->InternalizeL(buffBig));
	TEST(ret==KErrNone);
	TEST(authInfoCopy->iIMPI == authInfo->iIMPI);
	TEST(authInfoCopy->iIMPUArray.Count() == authInfo->iIMPUArray.Count());
	TEST(authInfoCopy->iIMPUArray[0] == authInfo->iIMPUArray[0]);
	TEST(authInfoCopy->iHomeNetworkDomainName == authInfo->iHomeNetworkDomainName);
	TEST(authInfoCopy->iAuthenticationDataSource == authInfo->iAuthenticationDataSource);
	
	CleanupStack::PopAndDestroy(authInfoCopy);
	
	CleanupStack::PopAndDestroy(hbuf);
	CleanupStack::PopAndDestroy(authInfo);
	
	
	// test IPC to ETEL/TSY
	
	RMobilePhone phone;
	ret=phone.Open(iTelServer,DMMTSY_PHONE_NAMEAUTH1);
	TEST(ret==KErrNone);
	
	RMobilePhone::CImsAuthorizationInfoV5* authInfoV5=RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfoV5);
	
	CTestAuthorizationData* testAuthData=CTestAuthorizationData::NewLC(phone,*authInfoV5);
	
	
	testAuthData->Start();
	
	testAuthData->iCanceller->Call();
    CActiveScheduler::Start();
    	
	testAuthData->Start();	
	CActiveScheduler::Start();

	TEST(testAuthData->iRetrieve != NULL);
	TEST(testAuthData->iRetrieve->AuthInfoValid());

	if(testAuthData->iRetrieve && testAuthData->iRetrieve->AuthInfoValid())
		{
		TEST(authInfoV5->iIMPI == DMMTSY_PHONE1_IMPI);
		TEST(authInfoV5->iIMPUArray.Count()==DMMTSY_PHONE1_COUNT);
		TEST(authInfoV5->iIMPUArray[0]==DMMTSY_PHONE1_IMPU0);
		TEST(authInfoV5->iHomeNetworkDomainName==DMMTSY_PHONE1_HNDN);
		TEST(authInfoV5->iAuthenticationDataSource==DMMTSY_PHONE1_DATASOURCE);
		}



	testAuthData->Start();
	CActiveScheduler::Start();

	TEST(testAuthData->iRetrieve != NULL);
	TEST(testAuthData->iRetrieve->AuthInfoValid());

	if(testAuthData->iRetrieve && testAuthData->iRetrieve->AuthInfoValid())
		{
		TEST(authInfoV5->iIMPI == DMMTSY_PHONE1_IMPI);
		TEST(authInfoV5->iIMPUArray.Count()==DMMTSY_PHONE1_COUNT);
		TEST(authInfoV5->iIMPUArray[0]==DMMTSY_PHONE1_IMPU0);
		TEST(authInfoV5->iHomeNetworkDomainName==DMMTSY_PHONE1_HNDN);
		TEST(authInfoV5->iAuthenticationDataSource==DMMTSY_PHONE1_DATASOURCE);
		}
	
	CleanupStack::PopAndDestroy(testAuthData);
	CleanupStack::PopAndDestroy(authInfoV5);
	phone.Close();



	ret=phone.Open(iTelServer,DMMTSY_PHONE_NAMEAUTH2);
	TEST(ret==KErrNone);

	authInfoV5=RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfoV5);

	testAuthData=CTestAuthorizationData::NewLC(phone,*authInfoV5);

	testAuthData->Start();
	CActiveScheduler::Start();

	if(testAuthData->iRetrieve && testAuthData->iRetrieve->AuthInfoValid())
		{				
		TEST(authInfoV5->iIMPI == DMMTSY_PHONE2_IMPI);
		TEST(authInfoV5->iIMPUArray.Count()==DMMTSY_PHONE2_COUNT);
		TEST(authInfoV5->iIMPUArray[0]==DMMTSY_PHONE2_IMPU1);
		TEST(authInfoV5->iIMPUArray[1]==DMMTSY_PHONE2_IMPU2);
		TEST(authInfoV5->iIMPUArray[2]==DMMTSY_PHONE2_IMPU3);
		TEST(authInfoV5->iIMPUArray[3]==DMMTSY_PHONE2_IMPU4);
		TEST(authInfoV5->iHomeNetworkDomainName==DMMTSY_PHONE2_HNDN);
		TEST(authInfoV5->iAuthenticationDataSource==DMMTSY_PHONE2_DATASOURCE);		
		}

	CleanupStack::PopAndDestroy(testAuthData);
	CleanupStack::PopAndDestroy(authInfoV5);
	phone.Close();
	

	ret=phone.Open(iTelServer,DMMTSY_PHONE_NAMEAUTH3);
	TEST(ret==KErrNone);

	authInfoV5=RMobilePhone::CImsAuthorizationInfoV5::NewL();
	CleanupStack::PushL(authInfoV5);

	testAuthData=CTestAuthorizationData::NewLC(phone,*authInfoV5);

	testAuthData->Start();
	CActiveScheduler::Start();

	if(testAuthData->iRetrieve && testAuthData->iRetrieve->AuthInfoValid())
		{		
		TEST(authInfoV5->iIMPI == DMMTSY_PHONE3_IMPI);
		TEST(authInfoV5->iIMPUArray.Count()==DMMTSY_PHONE3_COUNT);
		TEST(authInfoV5->iHomeNetworkDomainName==DMMTSY_PHONE3_HNDN);
		TEST(authInfoV5->iAuthenticationDataSource==DMMTSY_PHONE3_DATASOURCE);
		}

	CleanupStack::PopAndDestroy(testAuthData);
	CleanupStack::PopAndDestroy(authInfoV5);
	
	//Test asynchronous NotifyImsAuthorizationInfoChange - just returns KErrNone

	phone.NotifyImsAuthorizationInfoChange(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	//Test asynchronous NotifyImsAuthorizationInfoChange & Cancel	
	phone.NotifyImsAuthorizationInfoChange(reqStatus);
	phone.CancelAsyncRequest(EMobilePhoneNotifyImsAuthorizationInfoChanged);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrCancel);
	
	phone.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
}


CTestAuthorizationData* CTestAuthorizationData::NewLC(RMobilePhone& aPhone,RMobilePhone::CImsAuthorizationInfoV5& aAuthInfo)
	{
    CTestAuthorizationData* r=new(ELeave) CTestAuthorizationData(aPhone,aAuthInfo);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }

CTestAuthorizationData::CTestAuthorizationData(RMobilePhone& aPhone,RMobilePhone::CImsAuthorizationInfoV5& aAuthInfo)
    : CActive(EPriorityHigh), iAuthInfo(aAuthInfo),iPhone(aPhone)
    {
	}

TInt CTestAuthorizationData::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}


void CTestAuthorizationData::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CAsyncRetrieveAuthorizationInfo::NewL(iPhone,iAuthInfo);
	iCanceller=new (ELeave) CTestCanceller(this);
    }

CTestAuthorizationData::~CTestAuthorizationData()
    {
    delete iCanceller;
	delete iRetrieve;
	}

void CTestAuthorizationData::Start()
    {
    iRetrieve->Start(iStatus);
    SetActive();    
    }

void CTestAuthorizationData::RunL()
    {    
    CActiveScheduler::Stop();
	}


void CTestAuthorizationData::DoCancel()
    {
    iRetrieve->Cancel();    
    }
