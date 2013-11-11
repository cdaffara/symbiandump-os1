// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimSmsListTest.h"

CSimTestGetSmspList* CSimTestGetSmspList::NewL(RMobileSmsMessaging& aSmsMessaging,TBool aBatch,
								 TInt aStart,TInt aBatchSize,TInt aCount, CSimSmsListTest* aTestStep)
	{
    CSimTestGetSmspList* r=new(ELeave) CSimTestGetSmspList(aSmsMessaging,aBatch,aStart,aBatchSize,aCount,aTestStep);
    CleanupStack::PushL(r);
    r->ConstructL();
	CleanupStack::Pop();
    return r;
    }
      
CSimTestGetSmspList::CSimTestGetSmspList(RMobileSmsMessaging& aSmsMessaging,TBool aBatch,
								 TInt aStart,TInt aBatchSize,TInt aCount, CSimSmsListTest* aTestStep)
    : CActive(EPriorityNormal),
	  iSmsMessaging(aSmsMessaging),	
	  iBatch(aBatch),
	  iStart(aStart),
	  iBatchSize(aBatchSize),
	  iCount(aCount),
	  iTestStep(aTestStep)
    {}

void CSimTestGetSmspList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieveSmsp=CRetrieveMobilePhoneSmspList::NewL(iSmsMessaging/*,RMobileSmspStore::KETelMobileGsmSmsEntryV1*/);
    }

CSimTestGetSmspList::~CSimTestGetSmspList()
    {
    delete iRetrieveSmsp;
	}

void CSimTestGetSmspList::Start()
    {
	iRetrieveSmsp->Start(iStatus);
    SetActive();
    }       

void CSimTestGetSmspList::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckGsmResultsL();
	User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}

void CSimTestGetSmspList::DoCancel()
    {
    iRetrieveSmsp->Cancel();
    }       

TInt CSimTestGetSmspList::CheckGsmResultsL()
	{
	CMobilePhoneSmspList* smspList=NULL;
	TRAPD(ret,smspList=iRetrieveSmsp->RetrieveListL());
	if(ret!=KErrNone)
		return ret;

	TInt cnt=smspList->Enumerate();
	for(TInt i=0;i<cnt;i++)
		{
		RMobileSmsMessaging::TMobileSmspEntryV1 entry=smspList->GetEntryL(i);
		iTestStep->INFO_PRINTF3(_L("Entry %d: PID = >%d<"),i,entry.iProtocolId);
		iTestStep->INFO_PRINTF3(_L("Entry %d: Dcs = >%d<"),i,entry.iDcs);
		iTestStep->INFO_PRINTF3(_L("Entry %d: ValidityPeriod = >%d<"),i,entry.iValidityPeriod);
		iTestStep->INFO_PRINTF3(_L("Entry %d: Destination  = >%S<"),i,&entry.iDestination.iTelNumber);
		iTestStep->INFO_PRINTF3(_L("Entry %d: Service Centre = >%S<"),i,&entry.iServiceCentre.iTelNumber);
		iTestStep->INFO_PRINTF3(_L("Entry %d: Service Centre = >%S<"),i,&entry.iText);
		}
	delete smspList;
	return KErrNone;
	}

CSimSmsListTest::CSimSmsListTest() 
	{ 
	SetTestStepName(_L("SmsListTest"));
	}

TVerdict CSimSmsListTest::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Sms parameters"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(8);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSmsMessaging.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened SMS Messaging object"));

	//get smsp
	CSimTestGetSmspList* getSmspList0=CSimTestGetSmspList::NewL(iSmsMessaging,EFalse,1,2,1,this);
	getSmspList0->Start();
	CActiveScheduler::Start();
	
	//store smsp
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
	smspEntry.iValidityPeriod=18;
	smspEntry.iDcs=2;
	smspEntry.iProtocolId=0;
	smspEntry.iServiceCentre.iTelNumber=KVodafoneSca;
	smspEntry.iDestination.iTelNumber=KTelephoneNumber2;
	smspEntry.iText=_L("Vodafone");
	smspList->AddEntryL(smspEntry);
	
	smspEntry.iValidityPeriod=10;
	smspEntry.iDcs=240;
	smspEntry.iProtocolId=0;	
	smspEntry.iServiceCentre.iTelNumber=KOrangeSca;
	smspEntry.iDestination.iTelNumber=KTelephoneNumber2;
	smspEntry.iText=_L("Orange");
	smspList->AddEntryL(smspEntry);

	TRequestStatus reqStatus;
	iSmsMessaging.StoreSmspListL(reqStatus, smspList);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	//get smsp
	getSmspList0->Start();
	CActiveScheduler::Start();
	User::After(5000000L);
	
	//get smsp Cancel
	getSmspList0->Start();
	getSmspList0->Cancel();
		User::After(5000000L);
	delete getSmspList0;

	CleanupStack::PopAndDestroy(); // smspList
	iSmsMessaging.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

