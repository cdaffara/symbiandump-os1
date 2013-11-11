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

#include "Te_SimONStoreTest.h"

const TInt KONMaxNoOfNumbersToTest=5;

CSimONStoreTest::CSimONStoreTest() 
	{ 
	SetTestStepName(_L("ONStoreTest"));
	}

TVerdict CSimONStoreTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginONStoreTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(7);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	CleanupClosePushL(iPhone);
	
	TUint32 caps;
	INFO_PRINTF1(_L("Testing Get ICC Access Capabilites from Phone level."));
	iPhone.GetIccAccessCaps(caps);
	TESTL(caps==7);

	ret=iOwnNumberStore.Open(iPhone);
	TESTL(ret==KErrNone);
	INFO_PRINTF1(_L("Opened Mobile Station International Subscriber Directory Number (MSISDN) Store object"));
	CleanupClosePushL(iOwnNumberStore);
	
	INFO_PRINTF1(_L("Retrieve MSISDN Information"));
	RMobileONStore::TMobileONStoreInfoV1 info;
	RMobileONStore::TMobileONStoreInfoV1Pckg infoPckg(info);

	TRequestStatus accessStatus;
	iOwnNumberStore.GetInfo(accessStatus,infoPckg);
	User::WaitForRequest(accessStatus);
	TESTL(accessStatus==KErrNone);
	
	INFO_PRINTF1(_L("Testing MSISDN Read"));
	TESTL(info.iNumberLen==50);
	TESTL(info.iTextLen==60);
	TESTL(info.iTotalEntries==5);

	// Try reading ON store one entry at a time
	for (TInt i = 1; i <= info.iTotalEntries && i<=KONMaxNoOfNumbersToTest; i++)
		{
		RMobileONStore::TMobileONEntryV1 ownNumberEntry;
	
		ownNumberEntry.iIndex = i;
		RMobileONStore::TMobileONEntryV1Pckg ownNumberEntryPckg(ownNumberEntry);

		iOwnNumberStore.Read(accessStatus, ownNumberEntryPckg);
		User::WaitForRequest(accessStatus);
		
		if(accessStatus == KErrNone)
			{
			ownNumberEntry.iIndex = i;
			INFO_PRINTF5(_L("OWN number entry: index=%d, service=%d, number=%S, text=%S"), ownNumberEntry.iIndex, ownNumberEntry.iService, &ownNumberEntry.iNumber.iTelNumber, &ownNumberEntry.iText);
			}
		}	
	
	CleanupStack::Pop();
	iOwnNumberStore.Close();
	CleanupStack::Pop();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

