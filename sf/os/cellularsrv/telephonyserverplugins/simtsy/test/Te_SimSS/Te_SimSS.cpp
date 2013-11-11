// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This tests the GSM Supplementary services functions including
// Call Barring,  call forwarding and identity services
// 
//

/**
 @file
*/


#include "Te_SimSS.h"

/**************************************************************/
//
// Testing asynchronous retrieve of Call Barring list
//
/**************************************************************/

CTestGetCBList* CTestGetCBList::NewLC(RMobilePhone& aPhone, CTestCallBarring* aTest)
	{
    CTestGetCBList* r=new(ELeave) CTestGetCBList(aPhone, aTest);
    CleanupStack::PushL(r);
    r->ConstructL();
	CleanupStack::Pop();
    return r;
    }
      
CTestGetCBList::CTestGetCBList(RMobilePhone& aPhone, CTestCallBarring* aTest)
    : CActive(EPriorityNormal),
	  iPhone(aPhone),
	  iTest(aTest)
    {}

void CTestGetCBList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieveCB=CRetrieveMobilePhoneCBList::NewL(iPhone);
	
    }

void CTestGetCBList::Start(RMobilePhone::TMobilePhoneCBCondition aCondition)
    {
	iRetrieveCB->Start(iStatus, aCondition);
    SetActive();
    }       

void CTestGetCBList::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResultsL();
	User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}

void CTestGetCBList::DoCancel()
    {
    iRetrieveCB->Cancel();
    }       

TInt CTestGetCBList::SetExpectedResultsL(RMobilePhone::TMobilePhoneCBInfoEntryV1 aExpectedEntry)
	{
	if (!iChecked)
		iExpectedcbList=CMobilePhoneCBList::NewL();
	TRAPD(ret,iExpectedcbList->AddEntryL(aExpectedEntry););
	if(ret!=KErrNone)
		return ret;
	iChecked = ETrue;
	return KErrNone;
	}

void CTestGetCBList::PrintEntry(RMobilePhone::TMobilePhoneCBInfoEntryV1 aCfEntry, TInt aEntryNumber)
	{
	TBuf<200> EntryCondition;
	TBuf<200> EntryServiceGroup;
	TBuf<200> EntryStatus;
	
	switch (aCfEntry.iCondition)
		{
		case RMobilePhone::EBarUnspecified:
			EntryCondition = _L("EBarUnspecified");
			break;
		case RMobilePhone::EBarAllIncoming:
			EntryCondition = _L("EBarAllIncoming");
			break;
		case RMobilePhone::EBarIncomingRoaming:
			EntryCondition = _L("EBarIncomingRoaming");
			break;
		case RMobilePhone::EBarAllOutgoing:
			EntryCondition = _L("EBarAllOutgoing");
			break;
		case RMobilePhone::EBarOutgoingInternational:
			EntryCondition = _L("EBarOutgoingInternational");
			break;
		case RMobilePhone::EBarOutgoingInternationalExHC:
			EntryCondition = _L("EBarOutgoingInternationalExHC");
			break;
		case RMobilePhone::EBarAllCases:
			EntryCondition = _L("EBarAllCases");
			break; 
		case RMobilePhone::EBarAllOutgoingServices:
			EntryCondition = _L("EBarAllOutgoingServices");
			break;
		case RMobilePhone::EBarAllIncomingServices:
			EntryCondition = _L("EBarAllIncomingServices");
			break;
		default:
			break;
		}
	
	switch (aCfEntry.iServiceGroup)
		{
		case RMobilePhone::EServiceUnspecified:
			EntryServiceGroup = _L("EServiceUnspecified");
			break;
		case RMobilePhone::EVoiceService:
			EntryServiceGroup = _L("EVoiceService");
			break;
		case RMobilePhone::EAuxVoiceService:
			EntryServiceGroup = _L("EAuxVoiceService");
			break;
		case RMobilePhone::ECircuitDataService:
			EntryServiceGroup = _L("ECircuitDataService");
			break;
		case RMobilePhone::EPacketDataService:
			EntryServiceGroup = _L("EPacketDataService");
			break;
		case RMobilePhone::EFaxService:
			EntryServiceGroup = _L("EFaxService");
			break;
		case RMobilePhone::EShortMessageService:
			EntryServiceGroup = _L("EShortMessageService");
			break;
		case RMobilePhone::EAllServices:
			EntryServiceGroup = _L("EAllServices");
			break;
		case RMobilePhone::EAllTele:
			EntryServiceGroup = _L("EAllTele");
			break;
		case RMobilePhone::ETelephony:
			EntryServiceGroup = _L("ETelephony");
			break;
		case RMobilePhone::EAllDataTele:
			EntryServiceGroup = _L("EAllDataTele");
			break;
		case RMobilePhone::EAllDataExSms:
			EntryServiceGroup = _L("EBarUnspecified");
			break;
		case RMobilePhone::EAllTeleExcSms:
			EntryServiceGroup = _L("EAllTeleExcSms");
			break;
		case RMobilePhone::EAllPlmnTele:
			EntryServiceGroup = _L("EAllPlmnTele");
			break;
		case RMobilePhone::EPlmnTele1:
			EntryServiceGroup = _L("EPlmnTele1");
			break;
		case RMobilePhone::EPlmnTele2:
			EntryServiceGroup = _L("EPlmnTele2");
			break;
		case RMobilePhone::EPlmnTele3:
			EntryServiceGroup = _L("EPlmnTele3");
			break;
		case RMobilePhone::EPlmnTele4:
			EntryServiceGroup = _L("EPlmnTele4");
			break;
		case RMobilePhone::EPlmnTele5:
			EntryServiceGroup = _L("EPlmnTele5");
			break;
		case RMobilePhone::EPlmnTele6:
			EntryServiceGroup = _L("EPlmnTele6");
			break;
		case RMobilePhone::EPlmnTele7:
			EntryServiceGroup = _L("EPlmnTele7");
			break;
		case RMobilePhone::EPlmnTele8:
			EntryServiceGroup = _L("EPlmnTele8");
			break;
		case RMobilePhone::EPlmnTele9:
			EntryServiceGroup = _L("EPlmnTele9");
			break;
		case RMobilePhone::EPlmnTeleA:
			EntryServiceGroup = _L("EPlmnTeleA");
			break;
		case RMobilePhone::EPlmnTeleB:
			EntryServiceGroup = _L("EPlmnTeleB");
			break;
		case RMobilePhone::EPlmnTeleC:
			EntryServiceGroup = _L("EPlmnTeleC");
			break;
		case RMobilePhone::EPlmnTeleD:
			EntryServiceGroup = _L("EPlmnTeleD");
			break;
		case RMobilePhone::EPlmnTeleE:
			EntryServiceGroup = _L("EPlmnTeleE");
			break;
		case RMobilePhone::EPlmnTeleF:
			EntryServiceGroup = _L("EPlmnTeleF");
			break;
		case RMobilePhone::EAllBearer:
			EntryServiceGroup = _L("EAllBearer");
			break;
		case RMobilePhone::EAllAsync:
			EntryServiceGroup = _L("EAllAsync");
			break;
		case RMobilePhone::EAllSync:
			EntryServiceGroup = _L("EAllSync");
			break;
		case RMobilePhone::ESyncData:
			EntryServiceGroup = _L("ESyncData");
			break;
		case RMobilePhone::EAsyncData:
			EntryServiceGroup = _L("EAsyncData");
			break;
		case RMobilePhone::EPacketData:
			EntryServiceGroup = _L("EPacketData");
			break;
		case RMobilePhone::EPadAccess:
			EntryServiceGroup = _L("EPadAccess");
			break;
		case RMobilePhone::EAllPlmnBearer:
			EntryServiceGroup = _L("EAllPlmnBearer");
			break;
		case RMobilePhone::EPlmnBearerServ1:
			EntryServiceGroup = _L("EPlmnBearerServ1");
			break;
		case RMobilePhone::EPlmnBearerServ2:
			EntryServiceGroup = _L("EPlmnBearerServ2");
			break;
		case RMobilePhone::EPlmnBearerServ3:
			EntryServiceGroup = _L("EPlmnBearerServ3");
			break;
		case RMobilePhone::EPlmnBearerServ4:
			EntryServiceGroup = _L("EPlmnBearerServ4");
			break;
		case RMobilePhone::EPlmnBearerServ5:
			EntryServiceGroup = _L("EPlmnBearerServ5");
			break;
		case RMobilePhone::EPlmnBearerServ6:
			EntryServiceGroup = _L("EPlmnBearerServ6");
			break;
		case RMobilePhone::EPlmnBearerServ7:
			EntryServiceGroup = _L("EPlmnBearerServ7");
			break;
		case RMobilePhone::EPlmnBearerServ8:
			EntryServiceGroup = _L("EPlmnBearerServ8");
			break;
		case RMobilePhone::EPlmnBearerServ9:
			EntryServiceGroup = _L("EPlmnBearerServ9");
			break;
		case RMobilePhone::EPlmnBearerServA:
			EntryServiceGroup = _L("EPlmnBearerServA");
			break;
		case RMobilePhone::EPlmnBearerServB:
			EntryServiceGroup = _L("EPlmnBearerServB");
			break;
		case RMobilePhone::EPlmnBearerServC:
			EntryServiceGroup = _L("EPlmnBearerServC");
			break;
		case RMobilePhone::EPlmnBearerServD:
			EntryServiceGroup = _L("EPlmnBearerServD");
			break;
		case RMobilePhone::EPlmnBearerServE:
			EntryServiceGroup = _L("EPlmnBearerServE");
			break;
		case RMobilePhone::EPlmnBearerServF:
			EntryServiceGroup = _L("EPlmnBearerServF");
			break;
		case RMobilePhone::EAltTele:
			EntryServiceGroup = _L("EAltTele");
			break;
		case RMobilePhone::EVoiceGroupCall:
			EntryServiceGroup = _L("EVoiceGroupCall");
			break;
		case RMobilePhone::EVoiceBroadcast:
			EntryServiceGroup = _L("EVoiceBroadcast");
			break;
		case RMobilePhone::EAllGprsBearer:
			EntryServiceGroup = _L("EAllGprsBearer");
			break;
		default:
			break;
		}

	switch (aCfEntry.iStatus)
		{
		case RMobilePhone::ECallBarringStatusActive:
			EntryStatus = _L("ECallBarringStatusActive");
			break;
		case RMobilePhone::ECallBarringStatusNotActive:
			EntryStatus = _L("ECallBarringStatusNotActive");
			break;
		case RMobilePhone::ECallBarringStatusNotProvisioned:
			EntryStatus = _L("ECallBarringStatusNotProvisioned");
			break;
		case RMobilePhone::ECallBarringStatusNotAvailable:
			EntryStatus = _L("ECallBarringStatusNotAvailable");
			break;
		case RMobilePhone::ECallBarringStatusUnknown:
			EntryStatus = _L("ECallBarringStatusUnknown");
			break;
		default:
			break;
		}
	iTest->INFO_PRINTF3(_L("Entry %d: condition = >%S<"),aEntryNumber,&EntryCondition);
	iTest->INFO_PRINTF3(_L("Entry %d: service group = >%S<"),aEntryNumber,&EntryServiceGroup);
	iTest->INFO_PRINTF3(_L("Entry %d: status = >%S<"),aEntryNumber,&EntryStatus);

	}

void CTestGetCBList::PrintCondition(RMobilePhone::TMobilePhoneCBCondition aCondition)
	{
	TBuf<200> EntryCondition;
	
	switch (aCondition)
		{
		case RMobilePhone::EBarUnspecified:
			EntryCondition = _L("EBarUnspecified");
			break;
		case RMobilePhone::EBarAllIncoming:
			EntryCondition = _L("EBarAllIncoming");
			break;
		case RMobilePhone::EBarIncomingRoaming:
			EntryCondition = _L("EBarIncomingRoaming");
			break;
		case RMobilePhone::EBarAllOutgoing:
			EntryCondition = _L("EBarAllOutgoing");
			break;
		case RMobilePhone::EBarOutgoingInternational:
			EntryCondition = _L("EBarOutgoingInternational");
			break;
		case RMobilePhone::EBarOutgoingInternationalExHC:
			EntryCondition = _L("EBarOutgoingInternationalExHC");
			break;
		case RMobilePhone::EBarAllCases:
			EntryCondition = _L("EBarAllCases");
			break;
		case RMobilePhone::EBarAllOutgoingServices:
			EntryCondition = _L("EBarAllOutgoingServices");
			break;
		case RMobilePhone::EBarAllIncomingServices:
			EntryCondition = _L("EBarAllIncomingServices");
			break;
		default:
			break;
		}
	iTest->INFO_PRINTF2(_L("Retrive for condition = >%S<"),&EntryCondition);

	}

TInt CTestGetCBList::CheckResultsL()
	{
	CMobilePhoneCBList* cbList=NULL;
	TRAPD(ret,cbList=iRetrieveCB->RetrieveListL());
	if(ret!=KErrNone)
		{
		iTest->INFO_PRINTF2( _L("CMobilePhoneCBList::RetrieveListL returned with %d"),ret); 
		// see if we know of such errors
		// if so then they are expected by the test cases
		if ( (ret==KErrNotFound)
		     || (ret==KErrAccessDenied)
		     || (ret==KErrArgument))
		    {
			return KErrNone;
			}
		else
			{
			return ret;
			}
		}

	TInt cnt=cbList->Enumerate();
	TInt expctcnt(0);
	if (iChecked)
		{
		expctcnt=iExpectedcbList->Enumerate();
		if	(expctcnt != cnt)
			{
			iTest->INFO_PRINTF3(_L("The number of entries %d doesnt match with the number of the expected ones %d"),cnt, expctcnt);
			iTest->TEST(0);
			}
		}
		
	for(TInt i=0;i<cnt;i++)
		{
		RMobilePhone::TMobilePhoneCBInfoEntryV1 entry=cbList->GetEntryL(i);
		if (expctcnt == cnt)
			{
			if (iChecked)
				{
				for(TInt j=0;j<expctcnt;j++) 
					{
					RMobilePhone::TMobilePhoneCBInfoEntryV1 ExpectedEntry=iExpectedcbList->GetEntryL(j);
					if (entry.iCondition == ExpectedEntry.iCondition)
						break;
					if (j == (expctcnt-1))
						iTest->TEST(entry.iCondition == ExpectedEntry.iCondition);
					}
				for(TInt k=0;k<expctcnt;k++) 
					{
					RMobilePhone::TMobilePhoneCBInfoEntryV1 ExpectedEntry=iExpectedcbList->GetEntryL(k);
					if (entry.iServiceGroup == ExpectedEntry.iServiceGroup)
						break;
					if (k == (expctcnt-1))
						iTest->TEST(entry.iServiceGroup == ExpectedEntry.iServiceGroup);
					}
				for(TInt l=0;l<expctcnt;l++) 
					{
					RMobilePhone::TMobilePhoneCBInfoEntryV1 ExpectedEntry=iExpectedcbList->GetEntryL(l);
					if (entry.iStatus == ExpectedEntry.iStatus)
						break;
					if (l == (expctcnt-1))
						iTest->TEST(entry.iStatus == ExpectedEntry.iStatus);
					}
				}
			}
			PrintEntry(entry,i);
			iTest->INFO_PRINTF1(_L("Opened Call object"));
		}
				
	if (iChecked)
		{
			delete iExpectedcbList;
			iChecked = EFalse;
		}

	delete cbList;
	return KErrNone;
	}

CTestGetCBList::~CTestGetCBList()
    {
	delete iRetrieveCB;
	}
	
/**************************************************************/
//
// Testing asynchronous retrieve of Call Forwarding list
//
/**************************************************************/

CTestGetCFList* CTestGetCFList::NewLC(RMobilePhone& aPhone, CTestCallForwarding* aTest)
	{
    CTestGetCFList* r=new(ELeave) CTestGetCFList(aPhone, aTest);
    CleanupStack::PushL(r);
    r->ConstructL();
	CleanupStack::Pop();
    return r;
    }
      
CTestGetCFList::CTestGetCFList(RMobilePhone& aPhone, CTestCallForwarding* aTest)
    : CActive(EPriorityNormal),
	  iPhone(aPhone),
	  iTest(aTest)
    {}

void CTestGetCFList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieveCF=CRetrieveMobilePhoneCFList::NewL(iPhone);
    }

void CTestGetCFList::Start(RMobilePhone::TMobilePhoneCFCondition aCondition)
    {
	iRetrieveCF->Start(iStatus, aCondition);
    SetActive();
    }       

void CTestGetCFList::Start(RMobilePhone::TMobilePhoneCFCondition aCondition,
		           			RMobilePhone::TMobileService aServiceGroup)
	{
	iRetrieveCF->Start(iStatus, aCondition, aServiceGroup);
	SetActive();
	}												


void CTestGetCFList::RunL()
    {
    TInt ret=iStatus.Int();
	// we expect some error codes - KErrArgument
    if (!(ret == KErrArgument))
	    User::LeaveIfError(ret);

	ret=CheckResultsL();
	User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}

void CTestGetCFList::DoCancel()
    {
    iRetrieveCF->Cancel();
    }       

TInt CTestGetCFList::SetExpectedResultsL(RMobilePhone::TMobilePhoneCFInfoEntryV1 aExpectedEntry)
	{
	if (!iChecked)
		iExpectedCfList=CMobilePhoneCFList::NewL();
	TRAPD(ret,iExpectedCfList->AddEntryL(aExpectedEntry););
	if(ret!=KErrNone)
		return ret;
	iChecked = ETrue;
	return KErrNone;
	}

void CTestGetCFList::PrintEntry(RMobilePhone::TMobilePhoneCFInfoEntryV1 aCfEntry, TInt aEntryNumber)
	{
	TBuf<200> EntryCondition;
	TBuf<200> EntryServiceGroup;
	TBuf<200> EntryStatus;
	
	switch (aCfEntry.iCondition)
		{
		case RMobilePhone::ECallForwardingUnspecified:
			EntryCondition = _L("ECallForwardingUnspecified");
			break;
		case RMobilePhone::ECallForwardingUnconditional:
			EntryCondition = _L("ECallForwardingUnconditional");
			break;
		case RMobilePhone::ECallForwardingBusy:
			EntryCondition = _L("ECallForwardingBusy");
			break;
		case RMobilePhone::ECallForwardingNoReply:
			EntryCondition = _L("ECallForwardingNoReply");
			break;
		case RMobilePhone::ECallForwardingNotReachable:
			EntryCondition = _L("ECallForwardingNotReachable");
			break;
		case RMobilePhone::ECallForwardingAllCases:
			EntryCondition = _L("ECallForwardingAllCases");
			break;
		case RMobilePhone::ECallForwardingAllConditionalCases:
			EntryCondition = _L("ECallForwardingAllConditionalCases");
			break;
		default:
			break;
		}
	
	switch (aCfEntry.iServiceGroup)
		{
		case RMobilePhone::EServiceUnspecified:
			EntryServiceGroup = _L("EServiceUnspecified");
			break;
		case RMobilePhone::EVoiceService:
			EntryServiceGroup = _L("EVoiceService");
			break;
		case RMobilePhone::EAuxVoiceService:
			EntryServiceGroup = _L("EAuxVoiceService");
			break;
		case RMobilePhone::ECircuitDataService:
			EntryServiceGroup = _L("ECircuitDataService");
			break;
		case RMobilePhone::EPacketDataService:
			EntryServiceGroup = _L("EPacketDataService");
			break;
		case RMobilePhone::EFaxService:
			EntryServiceGroup = _L("EFaxService");
			break;
		case RMobilePhone::EShortMessageService:
			EntryServiceGroup = _L("EShortMessageService");
			break;
		case RMobilePhone::EAllServices:
			EntryServiceGroup = _L("EAllServices");
			break;
		case RMobilePhone::EAllTele:
			EntryServiceGroup = _L("EAllTele");
			break;
		case RMobilePhone::ETelephony:
			EntryServiceGroup = _L("ETelephony");
			break;
		case RMobilePhone::EAllDataTele:
			EntryServiceGroup = _L("EAllDataTele");
			break;
		case RMobilePhone::EAllDataExSms:
			EntryServiceGroup = _L("EBarUnspecified");
			break;
		case RMobilePhone::EAllTeleExcSms:
			EntryServiceGroup = _L("EAllTeleExcSms");
			break;
		case RMobilePhone::EAllPlmnTele:
			EntryServiceGroup = _L("EAllPlmnTele");
			break;
		case RMobilePhone::EPlmnTele1:
			EntryServiceGroup = _L("EPlmnTele1");
			break;
		case RMobilePhone::EPlmnTele2:
			EntryServiceGroup = _L("EPlmnTele2");
			break;
		case RMobilePhone::EPlmnTele3:
			EntryServiceGroup = _L("EPlmnTele3");
			break;
		case RMobilePhone::EPlmnTele4:
			EntryServiceGroup = _L("EPlmnTele4");
			break;
		case RMobilePhone::EPlmnTele5:
			EntryServiceGroup = _L("EPlmnTele5");
			break;
		case RMobilePhone::EPlmnTele6:
			EntryServiceGroup = _L("EPlmnTele6");
			break;
		case RMobilePhone::EPlmnTele7:
			EntryServiceGroup = _L("EPlmnTele7");
			break;
		case RMobilePhone::EPlmnTele8:
			EntryServiceGroup = _L("EPlmnTele8");
			break;
		case RMobilePhone::EPlmnTele9:
			EntryServiceGroup = _L("EPlmnTele9");
			break;
		case RMobilePhone::EPlmnTeleA:
			EntryServiceGroup = _L("EPlmnTeleA");
			break;
		case RMobilePhone::EPlmnTeleB:
			EntryServiceGroup = _L("EPlmnTeleB");
			break;
		case RMobilePhone::EPlmnTeleC:
			EntryServiceGroup = _L("EPlmnTeleC");
			break;
		case RMobilePhone::EPlmnTeleD:
			EntryServiceGroup = _L("EPlmnTeleD");
			break;
		case RMobilePhone::EPlmnTeleE:
			EntryServiceGroup = _L("EPlmnTeleE");
			break;
		case RMobilePhone::EPlmnTeleF:
			EntryServiceGroup = _L("EPlmnTeleF");
			break;
		case RMobilePhone::EAllBearer:
			EntryServiceGroup = _L("EAllBearer");
			break;
		case RMobilePhone::EAllAsync:
			EntryServiceGroup = _L("EAllAsync");
			break;
		case RMobilePhone::EAllSync:
			EntryServiceGroup = _L("EAllSync");
			break;
		case RMobilePhone::ESyncData:
			EntryServiceGroup = _L("ESyncData");
			break;
		case RMobilePhone::EAsyncData:
			EntryServiceGroup = _L("EAsyncData");
			break;
		case RMobilePhone::EPacketData:
			EntryServiceGroup = _L("EPacketData");
			break;
		case RMobilePhone::EPadAccess:
			EntryServiceGroup = _L("EPadAccess");
			break;
		case RMobilePhone::EAllPlmnBearer:
			EntryServiceGroup = _L("EAllPlmnBearer");
			break;
		case RMobilePhone::EPlmnBearerServ1:
			EntryServiceGroup = _L("EPlmnBearerServ1");
			break;
		case RMobilePhone::EPlmnBearerServ2:
			EntryServiceGroup = _L("EPlmnBearerServ2");
			break;
		case RMobilePhone::EPlmnBearerServ3:
			EntryServiceGroup = _L("EPlmnBearerServ3");
			break;
		case RMobilePhone::EPlmnBearerServ4:
			EntryServiceGroup = _L("EPlmnBearerServ4");
			break;
		case RMobilePhone::EPlmnBearerServ5:
			EntryServiceGroup = _L("EPlmnBearerServ5");
			break;
		case RMobilePhone::EPlmnBearerServ6:
			EntryServiceGroup = _L("EPlmnBearerServ6");
			break;
		case RMobilePhone::EPlmnBearerServ7:
			EntryServiceGroup = _L("EPlmnBearerServ7");
			break;
		case RMobilePhone::EPlmnBearerServ8:
			EntryServiceGroup = _L("EPlmnBearerServ8");
			break;
		case RMobilePhone::EPlmnBearerServ9:
			EntryServiceGroup = _L("EPlmnBearerServ9");
			break;
		case RMobilePhone::EPlmnBearerServA:
			EntryServiceGroup = _L("EPlmnBearerServA");
			break;
		case RMobilePhone::EPlmnBearerServB:
			EntryServiceGroup = _L("EPlmnBearerServB");
			break;
		case RMobilePhone::EPlmnBearerServC:
			EntryServiceGroup = _L("EPlmnBearerServC");
			break;
		case RMobilePhone::EPlmnBearerServD:
			EntryServiceGroup = _L("EPlmnBearerServD");
			break;
		case RMobilePhone::EPlmnBearerServE:
			EntryServiceGroup = _L("EPlmnBearerServE");
			break;
		case RMobilePhone::EPlmnBearerServF:
			EntryServiceGroup = _L("EPlmnBearerServF");
			break;
		case RMobilePhone::EAltTele:
			EntryServiceGroup = _L("EAltTele");
			break;
		case RMobilePhone::EVoiceGroupCall:
			EntryServiceGroup = _L("EVoiceGroupCall");
			break;
		case RMobilePhone::EVoiceBroadcast:
			EntryServiceGroup = _L("EVoiceBroadcast");
			break;
		case RMobilePhone::EAllGprsBearer:
			EntryServiceGroup = _L("EAllGprsBearer");
			break;
		default:
			break;
		}

	switch (aCfEntry.iStatus)
		{
		case RMobilePhone::ECallForwardingStatusActive:
			EntryStatus = _L("ECallForwardingStatusActive");
			break;
		case RMobilePhone::ECallForwardingStatusNotActive:
			EntryStatus = _L("ECallForwardingStatusNotActive");
			break;
		case RMobilePhone::ECallForwardingStatusNotRegistered:
			EntryStatus = _L("ECallForwardingStatusNotRegistered");
			break;
		case RMobilePhone::ECallForwardingStatusNotProvisioned:
			EntryStatus = _L("ECallForwardingStatusNotProvisioned");
			break;
		case RMobilePhone::ECallForwardingStatusNotAvailable:
			EntryStatus = _L("ECallForwardingStatusNotAvailable");
			break;
		case RMobilePhone::ECallForwardingStatusUnknown:
			EntryStatus = _L("ECallForwardingStatusUnknown");
			break;
		case RMobilePhone::ECallForwardingStatusActivePIAllowed:
			EntryStatus = _L("ECallForwardingStatusActivePIAllowed");
			break;
		case RMobilePhone::ECallForwardingStatusActivePINotAvailable:
			EntryStatus = _L("ECallForwardingStatusActivePINotAvailable");
			break;
		case RMobilePhone::ECallForwardingStatusActivePIClir:
			EntryStatus = _L("ECallForwardingStatusActivePIClir");
			break;
		case RMobilePhone::ECallForwardingStatusActivePIClirSIVerified:
			EntryStatus = _L("ECallForwardingStatusActivePIClirSIVerified");
			break;
		case RMobilePhone::ECallForwardingStatusActivePIClirSINotScreened:
			EntryStatus = _L("ECallForwardingStatusActivePIClirSINotScreened");
			break;
		case RMobilePhone::ECallForwardingStatusActivePIClirSINetwork:
			EntryStatus = _L("ECallForwardingStatusActivePIClirSINetwork");
			break;
		case RMobilePhone::ECallForwardingStatusQuiescent:
			EntryStatus = _L("ECallForwardingStatusQuiescent");
			break;
		default:
			break;
		}
	iTest->INFO_PRINTF3(_L("Entry %d: condition = >%S<"),aEntryNumber,&EntryCondition);
	iTest->INFO_PRINTF3(_L("Entry %d: service group = >%S<"),aEntryNumber,&EntryServiceGroup);
	iTest->INFO_PRINTF3(_L("Entry %d: status = >%S<"),aEntryNumber,&EntryStatus);
	iTest->INFO_PRINTF3(_L("Entry %d: Mobile address = >%S<"),aEntryNumber,&aCfEntry.iNumber.iTelNumber );
	iTest->INFO_PRINTF3(_L("Entry %d: Timeout = >%d<"),aEntryNumber,aCfEntry.iTimeout);
	}

void CTestGetCFList::PrintCondition(RMobilePhone::TMobilePhoneCFCondition aCondition)
	{
	TBuf<200> EntryCondition;
	
	switch (aCondition)
		{
		case RMobilePhone::ECallForwardingUnspecified:
			EntryCondition = _L("ECallForwardingUnspecified");
			break;
		case RMobilePhone::ECallForwardingUnconditional:
			EntryCondition = _L("ECallForwardingUnconditional");
			break;
		case RMobilePhone::ECallForwardingBusy:
			EntryCondition = _L("ECallForwardingBusy");
			break;
		case RMobilePhone::ECallForwardingNoReply:
			EntryCondition = _L("ECallForwardingNoReply");
			break;
		case RMobilePhone::ECallForwardingNotReachable:
			EntryCondition = _L("ECallForwardingNotReachable");
			break;
		case RMobilePhone::ECallForwardingAllCases:
			EntryCondition = _L("ECallForwardingAllCases");
			break;
		case RMobilePhone::ECallForwardingAllConditionalCases:
			EntryCondition = _L("ECallForwardingAllConditionalCases");
			break;
		default:
			break;
		}
	iTest->INFO_PRINTF2(_L("Retrieve for condition = >%S<"),&EntryCondition);

	}


TInt CTestGetCFList::CheckResultsL()
	{
	CMobilePhoneCFList* cfList=NULL;
	TRAPD(ret,cfList=iRetrieveCF->RetrieveListL());
	
	if(ret!=KErrNone)
		{
		iTest->INFO_PRINTF2( _L("CMobilePhoneCBList::RetrieveListL returned with %d"),ret); 
		// see if we know of such errors
		// if so then they are expected by the test cases
		if ((ret==KErrNotFound)
		     || (ret==KErrAccessDenied)
		     || (ret==KErrArgument))
		    {
			return KErrNone;
			}
		else 
			{
			return ret;
			}
		}

	TInt cnt=cfList->Enumerate();
	TInt expctcnt(0);
	if (iChecked)
		{
		expctcnt=iExpectedCfList->Enumerate();
		if	(expctcnt != cnt)
			{
			iTest->INFO_PRINTF1(_L("The number of entries doesnt match with the number of the expected ones"));
			iTest->TEST(0);
			}
		}
	for(TInt i=0;i<cnt;i++)
		{
		RMobilePhone::TMobilePhoneCFInfoEntryV1 entry=cfList->GetEntryL(i);
		if (expctcnt == cnt)
			{
			if (iChecked)
				{
				for(TInt j=0;j<expctcnt;j++) 
					{
					RMobilePhone::TMobilePhoneCFInfoEntryV1 ExpectedEntry=iExpectedCfList->GetEntryL(j);
					if (entry.iCondition == ExpectedEntry.iCondition)
						break;
					if (j == (expctcnt-1))
						iTest->TEST(entry.iCondition == ExpectedEntry.iCondition);
					}
				for(TInt k=0;k<expctcnt;k++) 
					{
					RMobilePhone::TMobilePhoneCFInfoEntryV1 ExpectedEntry=iExpectedCfList->GetEntryL(k);
					if (entry.iServiceGroup == ExpectedEntry.iServiceGroup)
						break;
					if (k == (expctcnt-1))
						iTest->TEST(entry.iServiceGroup == ExpectedEntry.iServiceGroup);
					}
				for(TInt l=0;l<expctcnt;l++) 
					{
					RMobilePhone::TMobilePhoneCFInfoEntryV1 ExpectedEntry=iExpectedCfList->GetEntryL(l);
					if (entry.iStatus == ExpectedEntry.iStatus)
						break;
					if (l == (expctcnt-1))
						iTest->TEST(entry.iStatus == ExpectedEntry.iStatus);
					}
				}
			}
		PrintEntry(entry,i);
		iTest->INFO_PRINTF1(_L("Opened Call object"));
	
		}
	if (iChecked)
		{
			delete iExpectedCfList;
			iChecked = EFalse;
		}
	delete cfList;
	return KErrNone;
	}

CTestGetCFList::~CTestGetCFList()
    {
	delete iRetrieveCF;
	}
	
	
/**************************************************************/
//
// Testing asynchronous retrieve of Call Waiting list
//
/**************************************************************/

CTestGetCWList* CTestGetCWList::NewLC(RMobilePhone& aPhone, CTestCallWaiting* aTest)
	{
    CTestGetCWList* r=new(ELeave) CTestGetCWList(aPhone, aTest);
    CleanupStack::PushL(r);
    r->ConstructL();
	CleanupStack::Pop();
    return r;
    }
      
CTestGetCWList::CTestGetCWList(RMobilePhone& aPhone, CTestCallWaiting* aTest)
    : CActive(EPriorityNormal),
	  iPhone(aPhone),
	  iTest(aTest)
    {}

void CTestGetCWList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieveCW=CRetrieveMobilePhoneCWList::NewL(iPhone);
	
    }

void CTestGetCWList::Start()
    {
	iRetrieveCW->Start(iStatus);
    SetActive();
    }       

void CTestGetCWList::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResultsL();
	User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}

void CTestGetCWList::DoCancel()
    {
    iRetrieveCW->Cancel();
    }       

TInt CTestGetCWList::SetExpectedResultsL(RMobilePhone::TMobilePhoneCWInfoEntryV1 aExpectedEntry)
	{
	if (!iChecked)
		iExpectedcwList=CMobilePhoneCWList::NewL();
	TRAPD(ret,iExpectedcwList->AddEntryL(aExpectedEntry););
	if(ret!=KErrNone)
		return ret;
	iChecked = ETrue;
	return KErrNone;
	}

void CTestGetCWList::PrintEntry(RMobilePhone::TMobilePhoneCWInfoEntryV1 aCwEntry, TInt aEntryNumber)
	{
	TBuf<200> EntryServiceGroup;
	TBuf<200> EntryStatus;
	
	switch (aCwEntry.iServiceGroup)
		{
		case RMobilePhone::EServiceUnspecified:
			EntryServiceGroup = _L("EServiceUnspecified");
			break;
		case RMobilePhone::EVoiceService:
			EntryServiceGroup = _L("EVoiceService");
			break;
		case RMobilePhone::EAuxVoiceService:
			EntryServiceGroup = _L("EAuxVoiceService");
			break;
		case RMobilePhone::ECircuitDataService:
			EntryServiceGroup = _L("ECircuitDataService");
			break;
		case RMobilePhone::EPacketDataService:
			EntryServiceGroup = _L("EPacketDataService");
			break;
		case RMobilePhone::EFaxService:
			EntryServiceGroup = _L("EFaxService");
			break;
		case RMobilePhone::EShortMessageService:
			EntryServiceGroup = _L("EShortMessageService");
			break;
		case RMobilePhone::EAllServices:
			EntryServiceGroup = _L("EAllServices");
			break;
		case RMobilePhone::EAllTele:
			EntryServiceGroup = _L("EAllTele");
			break;
		case RMobilePhone::ETelephony:
			EntryServiceGroup = _L("ETelephony");
			break;
		case RMobilePhone::EAllDataTele:
			EntryServiceGroup = _L("EAllDataTele");
			break;
		case RMobilePhone::EAllDataExSms:
			EntryServiceGroup = _L("EBarUnspecified");
			break;
		case RMobilePhone::EAllTeleExcSms:
			EntryServiceGroup = _L("EAllTeleExcSms");
			break;
		case RMobilePhone::EAllPlmnTele:
			EntryServiceGroup = _L("EAllPlmnTele");
			break;
		case RMobilePhone::EPlmnTele1:
			EntryServiceGroup = _L("EPlmnTele1");
			break;
		case RMobilePhone::EPlmnTele2:
			EntryServiceGroup = _L("EPlmnTele2");
			break;
		case RMobilePhone::EPlmnTele3:
			EntryServiceGroup = _L("EPlmnTele3");
			break;
		case RMobilePhone::EPlmnTele4:
			EntryServiceGroup = _L("EPlmnTele4");
			break;
		case RMobilePhone::EPlmnTele5:
			EntryServiceGroup = _L("EPlmnTele5");
			break;
		case RMobilePhone::EPlmnTele6:
			EntryServiceGroup = _L("EPlmnTele6");
			break;
		case RMobilePhone::EPlmnTele7:
			EntryServiceGroup = _L("EPlmnTele7");
			break;
		case RMobilePhone::EPlmnTele8:
			EntryServiceGroup = _L("EPlmnTele8");
			break;
		case RMobilePhone::EPlmnTele9:
			EntryServiceGroup = _L("EPlmnTele9");
			break;
		case RMobilePhone::EPlmnTeleA:
			EntryServiceGroup = _L("EPlmnTeleA");
			break;
		case RMobilePhone::EPlmnTeleB:
			EntryServiceGroup = _L("EPlmnTeleB");
			break;
		case RMobilePhone::EPlmnTeleC:
			EntryServiceGroup = _L("EPlmnTeleC");
			break;
		case RMobilePhone::EPlmnTeleD:
			EntryServiceGroup = _L("EPlmnTeleD");
			break;
		case RMobilePhone::EPlmnTeleE:
			EntryServiceGroup = _L("EPlmnTeleE");
			break;
		case RMobilePhone::EPlmnTeleF:
			EntryServiceGroup = _L("EPlmnTeleF");
			break;
		case RMobilePhone::EAllBearer:
			EntryServiceGroup = _L("EAllBearer");
			break;
		case RMobilePhone::EAllAsync:
			EntryServiceGroup = _L("EAllAsync");
			break;
		case RMobilePhone::EAllSync:
			EntryServiceGroup = _L("EAllSync");
			break;
		case RMobilePhone::ESyncData:
			EntryServiceGroup = _L("ESyncData");
			break;
		case RMobilePhone::EAsyncData:
			EntryServiceGroup = _L("EAsyncData");
			break;
		case RMobilePhone::EPacketData:
			EntryServiceGroup = _L("EPacketData");
			break;
		case RMobilePhone::EPadAccess:
			EntryServiceGroup = _L("EPadAccess");
			break;
		case RMobilePhone::EAllPlmnBearer:
			EntryServiceGroup = _L("EAllPlmnBearer");
			break;
		case RMobilePhone::EPlmnBearerServ1:
			EntryServiceGroup = _L("EPlmnBearerServ1");
			break;
		case RMobilePhone::EPlmnBearerServ2:
			EntryServiceGroup = _L("EPlmnBearerServ2");
			break;
		case RMobilePhone::EPlmnBearerServ3:
			EntryServiceGroup = _L("EPlmnBearerServ3");
			break;
		case RMobilePhone::EPlmnBearerServ4:
			EntryServiceGroup = _L("EPlmnBearerServ4");
			break;
		case RMobilePhone::EPlmnBearerServ5:
			EntryServiceGroup = _L("EPlmnBearerServ5");
			break;
		case RMobilePhone::EPlmnBearerServ6:
			EntryServiceGroup = _L("EPlmnBearerServ6");
			break;
		case RMobilePhone::EPlmnBearerServ7:
			EntryServiceGroup = _L("EPlmnBearerServ7");
			break;
		case RMobilePhone::EPlmnBearerServ8:
			EntryServiceGroup = _L("EPlmnBearerServ8");
			break;
		case RMobilePhone::EPlmnBearerServ9:
			EntryServiceGroup = _L("EPlmnBearerServ9");
			break;
		case RMobilePhone::EPlmnBearerServA:
			EntryServiceGroup = _L("EPlmnBearerServA");
			break;
		case RMobilePhone::EPlmnBearerServB:
			EntryServiceGroup = _L("EPlmnBearerServB");
			break;
		case RMobilePhone::EPlmnBearerServC:
			EntryServiceGroup = _L("EPlmnBearerServC");
			break;
		case RMobilePhone::EPlmnBearerServD:
			EntryServiceGroup = _L("EPlmnBearerServD");
			break;
		case RMobilePhone::EPlmnBearerServE:
			EntryServiceGroup = _L("EPlmnBearerServE");
			break;
		case RMobilePhone::EPlmnBearerServF:
			EntryServiceGroup = _L("EPlmnBearerServF");
			break;
		case RMobilePhone::EAltTele:
			EntryServiceGroup = _L("EAltTele");
			break;
		case RMobilePhone::EVoiceGroupCall:
			EntryServiceGroup = _L("EVoiceGroupCall");
			break;
		case RMobilePhone::EVoiceBroadcast:
			EntryServiceGroup = _L("EVoiceBroadcast");
			break;
		case RMobilePhone::EAllGprsBearer:
			EntryServiceGroup = _L("EAllGprsBearer");
			break;
		default:
			break;
		}

	switch (aCwEntry.iStatus)
		{
		case RMobilePhone::ECallWaitingStatusActive:
			EntryStatus = _L("ECallWaitingStatusActive");
			break;
		case RMobilePhone::ECallWaitingStatusNotActive:
			EntryStatus = _L("ECallWaitingStatusNotActive");
			break;
		case RMobilePhone::ECallWaitingStatusNotProvisioned:
			EntryStatus = _L("ECallWaitingStatusNotProvisioned");
			break;
		case RMobilePhone::ECallWaitingStatusNotAvailable:
			EntryStatus = _L("ECallBarringStatusNotAvailable");
			break;
		case RMobilePhone::ECallWaitingStatusUnknown:
			EntryStatus = _L("ECallWaitingStatusUnknown");
			break;
		default:
			break;
		}
	iTest->INFO_PRINTF3(_L("Entry %d: service group = >%S<"),aEntryNumber,&EntryServiceGroup);
	iTest->INFO_PRINTF3(_L("Entry %d: status = >%S<"),aEntryNumber,&EntryStatus);

	}


TInt CTestGetCWList::CheckResultsL()
	{
	CMobilePhoneCWList* cwList=NULL;
	TRAPD(ret,cwList=iRetrieveCW->RetrieveListL());
	if(ret!=KErrNone)
		{
		iTest->INFO_PRINTF2( _L("CMobilePhoneCWList::RetrieveListL left with %d"),ret); 
		if ( ret == KErrNotFound )
			return KErrNone;
		else 
			return ret;
		}

	TInt cnt=cwList->Enumerate();
	TInt expctcnt(0);
	if (iChecked)
		{
		expctcnt=iExpectedcwList->Enumerate();
		if	(expctcnt != cnt)
			{
			iTest->INFO_PRINTF3(_L("The number of entries %d doesnt match with the number of the expected ones %d "),cnt, expctcnt);
			iTest->TEST(0);
			}
		}
	for(TInt i=0;i<cnt;i++)
		{
		RMobilePhone::TMobilePhoneCWInfoEntryV1 entry=cwList->GetEntryL(i);
		if (expctcnt == cnt)
			{
			if (iChecked)
				{
				for(TInt k=0;k<expctcnt;k++) 
					{
					RMobilePhone::TMobilePhoneCWInfoEntryV1 ExpectedEntry=iExpectedcwList->GetEntryL(k);
					if (entry.iServiceGroup == ExpectedEntry.iServiceGroup)
						break;
					if (k == (expctcnt-1))
						iTest->TEST(entry.iServiceGroup == ExpectedEntry.iServiceGroup);
					}
				for(TInt l=0;l<expctcnt;l++) 
					{
					RMobilePhone::TMobilePhoneCWInfoEntryV1 ExpectedEntry=iExpectedcwList->GetEntryL(l);
					if (entry.iStatus == ExpectedEntry.iStatus)
						break;
					if (l == (expctcnt-1))
						iTest->TEST(entry.iStatus == ExpectedEntry.iStatus);
					}
				}
			}
			PrintEntry(entry,i);
		}
				
	if (iChecked)
		{
			delete iExpectedcwList;
			iChecked = EFalse;
		}

	delete cwList;
	return KErrNone;
	}

CTestGetCWList::~CTestGetCWList()
    {
	delete iRetrieveCW;
	}
	


//------------------------------------------------------------------
//						** TEST STEPS ***
//------------------------------------------------------------------

CTestCallBarring::CTestCallBarring()
	{
	SetTestStepName(_L("TestCallBarring"));
	}

enum TVerdict CTestCallBarring::doTestStepL()
/**
 * Local function that creates a CTestData object. 
 * Connects to the ETel Server, loads the phone module (SIM TSY), calls the BeginTest() 
 * function (which runs the tests) and then disconnects from the ETel Server at  
 * the end of the tests.
 */
	{
	CActiveScheduler* actSched=new(ELeave) CActiveScheduler();
	CleanupStack::PushL(actSched);
	CActiveScheduler::Install(actSched);

	SetTestNumberL(0);

	INFO_PRINTF1(_L("CTestCallBarring"));
	TRequestStatus reqStat=KErrNone;	
	TRequestStatus reqStat2=KErrNone;
	_LIT8(pswrd1, "1111");
	_LIT8(pswrd2, "2222");
	RMobilePhone::TMobilePhoneCBCondition condition=RMobilePhone::EBarAllOutgoing;
	RMobilePhone::TMobilePhoneCBChangeV1 changeInfo; 	
		
	CTestGetCBList* retrieveCBList=NULL;
	
	retrieveCBList = CTestGetCBList::NewLC(iPhone, this);
	
	RMobilePhone::TMobilePhoneCBInfoEntryV1 ExpectedEntry1;
	
	// position is important!
	// Test case: K.17 - Cancel notification with no previous reposts
	INFO_PRINTF1(_L("Test case: K.17 - Cancel notification with no previous reposts"));
	//register notification
	iPhone.NotifyCallBarringStatusChange(reqStat2,condition);
	
	// disable EMobilePhoneNotifyCallBarringStatusChange
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallBarringStatusChange);
	
	User::WaitForRequest(reqStat2);
	TEST(reqStat2==KErrCancel); 
	// because it is assumed no previous CTestGetCBList
	INFO_PRINTF2(_L("The notify change returned %d"), reqStat2.Int());


	// Test case: K.1 - Retrieve the status - preset
	INFO_PRINTF1(_L("Test case: K.1 - Retrieve the status from config"));
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	condition = RMobilePhone::EBarUnspecified;
	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();

	// Test case: K.2 - Disable Call barring for Voice :EBarAllOutgoingServices
	INFO_PRINTF1(_L("Test case: K.2 - De-activate Call Barring for all outgoing voice services"));
	condition = RMobilePhone::EBarAllOutgoingServices;
	changeInfo.iServiceGroup=RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionDeactivate;
	changeInfo.iPassword.Copy(pswrd1);
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
		
	// should not have anything here - empty list
    // as we are asking for a specific condition -EBarAllOutgoing
    condition = RMobilePhone::EBarAllOutgoing;
	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();
	INFO_PRINTF1(_L(""));
	
    // Test case: K.3 - Erase all services and request info for all services
	INFO_PRINTF1(_L("Test case: K.3 - Erase all services and request info for all services"));
	condition=RMobilePhone::EBarAllCases;
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionErase;
	changeInfo.iPassword.Copy(pswrd1);
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	condition=RMobilePhone::EBarUnspecified;
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllCases;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusNotActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	
	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: K.4 - Enable Call barring for Voice :EBarAllOutgoing
	INFO_PRINTF1(_L("Test case: K.4 - Activate Call Barring for all outgoing voice services"));
	condition = RMobilePhone::EBarAllOutgoing;
	changeInfo.iServiceGroup=RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd1);
	iPhone.SetCallBarringStatus(reqStat,condition,changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
		
	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();

    // Test case: K.5 - Enable Call barring for Voice but :EBarOutgoingInternational
	INFO_PRINTF1(_L("Test case: K.5 - Activate Call Barring (intl only) for outgoing Voice services"));
	condition = RMobilePhone::EBarOutgoingInternational;
	changeInfo.iServiceGroup=RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd1);
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
		
	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::EBarOutgoingInternational;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();

    // Test case: K.5 - Enable Call barring for Voice but :EBarOutgoingInternational again
	INFO_PRINTF1(_L("Test case: K.5 - Activate Call Barring (intl only) for outgoing Voice services again"));
	condition = RMobilePhone::EBarOutgoingInternational;
	changeInfo.iServiceGroup=RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd1);
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
		
	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::EBarOutgoingInternational;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();

	// Test case: K.6 - Set Call Barring Status for all outgoing services
	INFO_PRINTF1(_L("Activate Call Barring for all outgoing services"));
	condition = RMobilePhone::EBarAllOutgoing;
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd1);
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: K.6 - Set Call Barring Status for all outgoing services again
	INFO_PRINTF1(_L("Test case: K.6 - Activate Call Barring for all outgoing services again"));
	condition = RMobilePhone::EBarAllOutgoing;
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd1);
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: K.7 - deactivate for voice service & notification with different condition
	INFO_PRINTF1(_L("Test case: K.7 - Deactivate for outgoing voice service & notification with diferent condition"));
	condition = RMobilePhone::EBarAllOutgoing;
	iPhone.NotifyCallBarringStatusChange(reqStat2,condition);
	User::After(2000000);
	changeInfo.iServiceGroup=RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionDeactivate;
	changeInfo.iPassword.Copy(pswrd1);
	condition=RMobilePhone::EBarAllOutgoingServices;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	User::After(2000000);
	User::WaitForRequest(reqStat2);
	TEST(reqStat2==KErrNone);//notification did not complete(wrong condition)
	retrieveCBList->PrintCondition(condition);
	TEST(condition==RMobilePhone::EBarAllOutgoingServices);
	
	//Retrieve again 
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	condition=RMobilePhone::EBarAllOutgoing;
	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();

	// Test case: K.7 - deactivate for voice service & notification with different condition
	INFO_PRINTF1(_L("Test case: K.7 - Deactivate for outgoing voice service & notification with diferent condition"));
	condition = RMobilePhone::EBarAllOutgoing;
	iPhone.NotifyCallBarringStatusChange(reqStat2,condition);
	User::After(2000000);
	changeInfo.iServiceGroup=RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionDeactivate;
	changeInfo.iPassword.Copy(pswrd1);
	condition=RMobilePhone::EBarAllOutgoingServices;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	User::After(2000000);
	User::WaitForRequest(reqStat2);
	TEST(reqStat2==KErrNone);//notification did not complete(wrong condition)
	retrieveCBList->PrintCondition(condition);
	TEST(condition==RMobilePhone::EBarAllOutgoingServices);
	
	//Retrieve again 
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	condition=RMobilePhone::EBarAllOutgoing;
	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: K.8 - Activate EAuxVoiceService Service for outgoing international condition
	INFO_PRINTF1(_L("Test case: K.8 - Activate EAuxVoiceService Service for outgoing international condition"));
	changeInfo.iServiceGroup=RMobilePhone::EAuxVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd1);
	condition=RMobilePhone::EBarOutgoingInternational;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	condition=RMobilePhone::EBarUnspecified;	 
	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();
	
	//Retrieve again 
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

  	condition=RMobilePhone::EBarAllOutgoing;	 
	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();

	// Test case: K.9 - Activate for ECircuitDataService service, different conditions :EBarIncomingRoaming & notify
	INFO_PRINTF1(_L("Test case: K.9 - Activate for ECircuitDataService service, different conditions :EBarIncomingRoaming & notify"));
	changeInfo.iServiceGroup=RMobilePhone::ECircuitDataService;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	condition=RMobilePhone::EBarIncomingRoaming;
	changeInfo.iPassword.Copy(pswrd1);
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve for this condition
	ExpectedEntry1.iCondition=RMobilePhone::EBarIncomingRoaming;
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: K.10 - Retrieve for all conditions
	INFO_PRINTF1(_L("Test case: K.10 - Now retrieve for all conditions"));
	condition=RMobilePhone::EBarUnspecified;

	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iCondition=RMobilePhone::EBarIncomingRoaming;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	ExpectedEntry1.iCondition=RMobilePhone::EBarOutgoingInternational;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: K.11 - Deactivate EBarAllIncomingServices general condition and retrieve for all
	INFO_PRINTF1(_L("Test case: K.11 - Deactivate EBarAllIncomingServices general condition and retrieve  for all"));
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionDeactivate;
	changeInfo.iPassword.Copy(pswrd1);
	condition=RMobilePhone::EBarAllIncomingServices;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	condition=RMobilePhone::EBarUnspecified;
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	ExpectedEntry1.iCondition=RMobilePhone::EBarOutgoingInternational;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: K.11 - Deactivate EBarAllIncomingServices general condition and retrieve for all
	INFO_PRINTF1(_L("Test case: K.11 - Deactivate EBarAllIncomingServices general condition and retrieve all"));
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionDeactivate;
	changeInfo.iPassword.Copy(pswrd1);
	condition=RMobilePhone::EBarAllIncomingServices;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	condition=RMobilePhone::EBarUnspecified;
	ExpectedEntry1.iCondition=RMobilePhone::EBarAllOutgoing;
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallBarringStatusActive;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	ExpectedEntry1.iCondition=RMobilePhone::EBarOutgoingInternational;
	retrieveCBList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCBList->PrintCondition(condition);
	retrieveCBList->Start(condition);
	CActiveScheduler::Start();

	// Test case: K.12 - Set Call Barring Status for all services with wrong password
	INFO_PRINTF1(_L("Test case: K.12 - Set Call Barring Status for all services with wrong password"));
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd2);
	condition=RMobilePhone::EBarAllOutgoing;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrAccessDenied);

	// Test case: K.13 - Change Call Barrring Password
	INFO_PRINTF1(_L("Test case: K.13 - Change Call Barrring Password"));
	RMobilePhone::TMobilePhonePasswordChangeV1 passChange;
	passChange.iOldPassword.Copy(pswrd1);
	passChange.iNewPassword.Copy(pswrd2);
	iPhone.SetCallBarringPassword(reqStat,passChange);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	// Test case: K.12 - Set Call Barring Status for all services with wrong (original) password
	INFO_PRINTF1(_L("Test case: K.12 - Set Call Barring Status for all services with wrong (original) password"));
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd1);
	condition=RMobilePhone::EBarAllOutgoing;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrAccessDenied);

    // Test case: K.14 - Set Call Barring Status for all services with correct password
	INFO_PRINTF1(_L("Test case: K.14 - Set Call Barring Status for all services with correct password"));
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd2);
	condition=RMobilePhone::EBarAllOutgoing;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	// Test case: K.15 - Set wrong parameter - EBarAllCases - for activation
	INFO_PRINTF1(_L("Test case: K.15 - Set wrong parameters - EBarAllCases - for activation"));
	reqStat = KErrNone;
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd2);
	condition=RMobilePhone::EBarAllCases;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrArgument);

	// Test case: K.16 - Cancel notification
	INFO_PRINTF1(_L("Test case: K.16 - Cancel notification"));
	
	//register notification
	iPhone.NotifyCallBarringStatusChange(reqStat2,condition);

	// do a set 
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iPassword.Copy(pswrd2);
	condition=RMobilePhone::EBarAllOutgoing;
	iPhone.SetCallBarringStatus(reqStat,condition, changeInfo);
	
	// disable EMobilePhoneNotifyCallBarringStatusChange
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallBarringStatusChange);

	User::WaitForRequest(reqStat2);
	TEST((reqStat2==KErrCancel)||(reqStat2==KErrNone));//notification did not complete(wrong condition)
	INFO_PRINTF2(_L("The notify change returned %d"), reqStat2.Int());
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
    // cleanup phase; remove 
	CActiveScheduler::Install(NULL);	
	CleanupStack::PopAndDestroy();
	delete retrieveCBList;
	
	INFO_PRINTF1(_L("End CB test"));
	return TestStepResult();
	}
	
	
CTestCallForwarding::CTestCallForwarding()
	{
	SetTestStepName(_L("TestCallForwarding"));
	}

enum TVerdict CTestCallForwarding::doTestStepL()
/**
 * Local function that creates a CTestData object. 
 * Connects to the ETel Server, loads the phone module (SIM TSY), calls the BeginTest() 
 * function (which runs the tests) and then disconnects from the ETel Server at  
 * the end of the tests.
 */
	{
	CActiveScheduler* actSched = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(actSched);
	CActiveScheduler::Install(actSched);

 	SetTestNumberL(0);	
 	INFO_PRINTF1(_L("CTestCallForwarding"));
	
	TRequestStatus reqStat = KErrNone;	
	TRequestStatus reqStat2 = KErrNone;
 	
 	_LIT8(newCFNumber, "01632960000");
 	_LIT8(newCFNumber1, "01632960000");
	
	CTestGetCFList* retrieveCFList = NULL;
	retrieveCFList = CTestGetCFList::NewLC(iPhone, this);

 	RMobilePhone::TMobilePhoneCFCondition condition = RMobilePhone::ECallForwardingUnconditional;

	// ******************* NOTE: Position is important! **************************************
	
	// Test case: L.1 - Cancel notification with no previous reposts
	INFO_PRINTF1(_L("Test case: L.1 - Cancel notification with no previous reposts"));
	
	//register notification
	iPhone.NotifyCallForwardingStatusChange(reqStat2, condition);
	
	// disable EMobilePhoneNotifyCallForwardingStatusChange
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallForwardingStatusChange);
	User::WaitForRequest(reqStat2);
	TEST(reqStat2 == KErrCancel); 
	
	// because it is assumed no previous CTestGetCFList
	INFO_PRINTF2(_L("The notify change returned %d"), reqStat2.Int());

	// Test case: L.2 -  Retrieve the status for all basic services for a specific condition
	INFO_PRINTF1(_L("Test case: L.2 -  Retrieve the status for all basic services for a specific condition"));
	condition = RMobilePhone::ECallForwardingUnconditional;

	RMobilePhone::TMobilePhoneCFInfoEntryV1 ExpectedEntry1;

	// there should be only one entry
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingUnconditional;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();
		
	// Test case: L.3 -  Retrieve the status for all basic services for a specific condition
	INFO_PRINTF1(_L("Test case: L.3 -  Retrieve the status for all basic services for a specific condition"));
	condition = RMobilePhone::ECallForwardingNotReachable;
	
	// there should be only one entry
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingNotReachable;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EPacketDataService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

	// Test case: L.4 - Retrieve the configuration for erroneous condition
	//there should be no entries
	INFO_PRINTF1(_L("Test case: L.4 - Retrieve the configuration for erroneous  condition"));
	condition = RMobilePhone::ECallForwardingAllCases;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();
   
   	// Test case: L.5 - Retrieve the configuration for correct condition but for which there are no registered services
	INFO_PRINTF1(_L("Test case: L.5 - Retrieve the configuration for correct condition but for which there are no  registered services"));
	condition = RMobilePhone::ECallForwardingNoReply;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();
	INFO_PRINTF1(_L(""));

	RMobilePhone::TMobileService serviceGroup = RMobilePhone::EVoiceService;
	
   	// Test case: L.6 - Retrieve the configuration for correct condition but for specific registered group
	INFO_PRINTF1(_L("Test case: L.6 - Retrieve the configuration for correct condition but for specific registered group"));
	condition = RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition, serviceGroup);
	CActiveScheduler::Start();

   	// Test case: L.7 - Retrieve the configuration for correct condition but for specific non registered group
	INFO_PRINTF1(_L("Test case: L.7 - Retrieve the configuration for correct condition but for specific non registered group"));
	serviceGroup = RMobilePhone::EAuxVoiceService;
	condition=RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition, serviceGroup);
	CActiveScheduler::Start();
	INFO_PRINTF1(_L(""));
	
	// *********** Now we have retrieved our conditions list. Now modify the list. *************
	
	RMobilePhone::TMobilePhoneCFChangeV1 changeInfo;
	
	// Test case: L.8 - Register a previously not registered CF condition for all services to registered
	INFO_PRINTF1(_L("Test case: L.8 - Register a previously CF condition for all services to registered"));
	condition = RMobilePhone::ECallForwardingNoReply;
	changeInfo.iServiceGroup = RMobilePhone::EAllServices;
	changeInfo.iAction = RMobilePhone::EServiceActionRegister;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber);
	changeInfo.iTimeout=30;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat == KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingNoReply;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	condition = RMobilePhone::ECallForwardingNoReply;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

	// Test case: L.9 - Activate a previously not active CF condition for all services
	INFO_PRINTF1(_L("Test case: L.9 - De-activate a previously active CF condition for all services"));
	condition = RMobilePhone::ECallForwardingNoReply;
	changeInfo.iServiceGroup = RMobilePhone::EAllServices;
	changeInfo.iAction = RMobilePhone::EServiceActionActivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingNoReply;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::EAllServices;
	condition = RMobilePhone::ECallForwardingNoReply;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.10 - De-activate a previously active CF condition for all services
	INFO_PRINTF1(_L("Test case: L.10 - De-activate a previously active CF condition for all services"));
	condition = RMobilePhone::ECallForwardingNoReply;
	changeInfo.iServiceGroup = RMobilePhone::EAllServices;
	changeInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingNoReply;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::EAllServices;
	condition = RMobilePhone::ECallForwardingNoReply;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.11 - Erase a previously non active CF condition for all services
	INFO_PRINTF1(_L("Test case: L.11 - Erase a previously non active CF condition for all services"));
	condition = RMobilePhone::ECallForwardingNoReply;
	changeInfo.iServiceGroup = RMobilePhone::EAllServices;
	changeInfo.iAction = RMobilePhone::EServiceActionErase;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	serviceGroup = RMobilePhone::EAllServices;
	condition = RMobilePhone::ECallForwardingNoReply;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.12 - Register a previously not registered CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.12 - Register a previously not registered CF condition for a single service"));
	condition=RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup = RMobilePhone::EAuxVoiceService;
	changeInfo.iAction = RMobilePhone::EServiceActionRegister;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber);
	changeInfo.iTimeout = 30;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat == KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingUnconditional;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EAuxVoiceService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::EAuxVoiceService;
	condition = RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.13 - Activate a previously not registered CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.13 - Activate a previously not registered CF condition for a single service"));
	condition = RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup = RMobilePhone::EFaxService;
	changeInfo.iAction = RMobilePhone::EServiceActionActivate;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber);
	changeInfo.iTimeout = 25;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
    
	serviceGroup = RMobilePhone::EFaxService;
	condition = RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
    // should not return anything as cannot activate  
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();
	INFO_PRINTF1(_L(""));
	
	// Test case: L.14 - Register a previously not registered CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.14 - Register a previously not registered CF condition for a single service"));
	condition = RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup=RMobilePhone::EFaxService;
	changeInfo.iAction=RMobilePhone::EServiceActionRegister;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber);
	changeInfo.iTimeout=30;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingUnconditional;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EFaxService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::EFaxService;
	condition = RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.15 - Activate a previously registered CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.15 - Activate a previously not registered CF condition for a single service"));
	condition=RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup = RMobilePhone::EFaxService;
	changeInfo.iAction = RMobilePhone::EServiceActionActivate;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber);
	changeInfo.iTimeout = 25;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
    
	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingUnconditional;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EFaxService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
		
	serviceGroup = RMobilePhone::EFaxService;
	condition = RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.16 - De-activate a previously active CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.16 - De-activate a previously active CF condition for a single service"));
	condition=RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup = RMobilePhone::EFaxService;
	changeInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingUnconditional;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EFaxService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::EFaxService;
	condition = RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.17 - Erase a previously active CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.17 - Erase a previously active CF condition for a single service"));
	condition=RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup=RMobilePhone::EAuxVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionErase;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	serviceGroup = RMobilePhone::EAuxVoiceService;
	condition=RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();	

	// Test case: L.18 - Erase a previously not active CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.18 - Erase a previously not active CF condition for a single service"));
	condition = RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup = RMobilePhone::EFaxService;
	changeInfo.iAction = RMobilePhone::EServiceActionErase;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	serviceGroup = RMobilePhone::EFaxService;
	condition=RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();
	//there should be no entries printed as the service is deactivated
	INFO_PRINTF1(_L(""));
		
// Tests on Quiescent state	//
	
	// Test case: L.19 - De-activate a previously active CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.19 - De-activate a previously active CF condition for a single service"));
	condition=RMobilePhone::ECallForwardingBusy;
	changeInfo.iServiceGroup = RMobilePhone::ECircuitDataService;
	changeInfo.iAction = RMobilePhone::EServiceActionRegister;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingBusy;
	ExpectedEntry1.iServiceGroup = RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::ECircuitDataService;
	condition = RMobilePhone::ECallForwardingBusy;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.20 - Try and activate a single CF service
	// is active
	INFO_PRINTF1(_L("Test case: L.20 - Activate a single CF service"));
	condition=RMobilePhone::ECallForwardingBusy;
	changeInfo.iServiceGroup = RMobilePhone::ECircuitDataService;
	changeInfo.iAction = RMobilePhone::EServiceActionActivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingBusy;
	ExpectedEntry1.iServiceGroup = RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::ECircuitDataService;
	condition = RMobilePhone::ECallForwardingBusy;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.21 - De-activate active call forwarding unconditional service
	// is active
	INFO_PRINTF1(_L("Test case: L.21 - De-activate a previously active CF condition for a single service"));
	condition=RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup = RMobilePhone::EVoiceService;
	changeInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingUnconditional;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::EVoiceService;
	condition = RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	// Test case: L.22 - Try and activate call forwarding busy when call forwarding unconditional not active
	INFO_PRINTF1(_L("Test case: L.22 - Activate a previously active CF condition for a single service"));
	condition=RMobilePhone::ECallForwardingBusy;
	changeInfo.iServiceGroup = RMobilePhone::ECircuitDataService;
	changeInfo.iAction = RMobilePhone::EServiceActionActivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingBusy;
	ExpectedEntry1.iServiceGroup = RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::ECircuitDataService;
	condition = RMobilePhone::ECallForwardingBusy;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();
	
	// Test case: L.23 - Activate call forwarding unconditional when call forwarding busy active
	INFO_PRINTF1(_L("Test case: L.23 - Activate a previously active CF condition for a single service"));
	condition = RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup = RMobilePhone::EVoiceService;
	changeInfo.iAction = RMobilePhone::EServiceActionActivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingUnconditional;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::EVoiceService;
	condition = RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingBusy;
	ExpectedEntry1.iServiceGroup = RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusQuiescent;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::ECircuitDataService;
	condition = RMobilePhone::ECallForwardingBusy;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();
	
	// Test case: L.24 - De-activate call forwarding unconditional 
	INFO_PRINTF1(_L("Test case: L.24 - De-activate a previously active CF condition for a single service"));
	condition = RMobilePhone::ECallForwardingUnconditional;
	changeInfo.iServiceGroup = RMobilePhone::EVoiceService;
	changeInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingUnconditional;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::EVoiceService;
	condition = RMobilePhone::ECallForwardingUnconditional;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();
	
		//Retrieve again
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingBusy;
	ExpectedEntry1.iServiceGroup = RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	serviceGroup = RMobilePhone::ECircuitDataService;
	condition = RMobilePhone::ECallForwardingBusy;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition,serviceGroup);
	CActiveScheduler::Start();
	
//Tests on Notifications//

	INFO_PRINTF1(_L("Change simtsy config number"));
	SetTestNumberL(1);

	// Test case: L.25 - Register all services for a CF condition, whilst there are some previously reistered/non-active
	INFO_PRINTF1(_L("Test case: L.25 - Register all services for a CF condition, whilst there are some previously reistered/non-active"));
	changeInfo.iServiceGroup = RMobilePhone::EAllServices;
	changeInfo.iAction = RMobilePhone::EServiceActionRegister;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber1);
	condition = RMobilePhone::ECallForwardingNoReply;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	//Retrieve again  
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingNoReply;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	condition = RMobilePhone::ECallForwardingNoReply;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

	// Test case: L.26 - Activate all services for a CF condition, whilst there are some previously reistered/non-active
	INFO_PRINTF1(_L("Test case: L.26 - Activate all services for a CF condition, whilst there are some previously reistered/non-active"));
	changeInfo.iServiceGroup = RMobilePhone::EAllServices;
	changeInfo.iAction = RMobilePhone::EServiceActionActivate;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber1);
	condition = RMobilePhone::ECallForwardingNoReply;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	//Retrieve again  
	ExpectedEntry1.iCondition = RMobilePhone::ECallForwardingNoReply;
	ExpectedEntry1.iServiceGroup = RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus = RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	condition = RMobilePhone::ECallForwardingNoReply;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: L.27 - Enable notification whilst disable a service enabled for that condition
	INFO_PRINTF1(_L("Test case: L.27 - Enable notification whilst disable a service enabled for that condition"));
	condition = RMobilePhone::ECallForwardingNoReply;
	iPhone.NotifyCallForwardingStatusChange(reqStat2,condition);
	User::After(2000000);
	changeInfo.iServiceGroup = RMobilePhone::ECircuitDataService;
	changeInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	condition = RMobilePhone::ECallForwardingNoReply;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	
	condition = RMobilePhone::ECallForwardingUnconditional;
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	User::WaitForRequest(reqStat2);
	TEST(reqStat2==KErrNone);
	//notification should be 
	INFO_PRINTF1(_L("Notification condition"));
	retrieveCFList->PrintCondition(condition);
	
	//Retrieve again 
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingNoReply;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	condition=RMobilePhone::ECallForwardingNoReply;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

	// activate for 1 service but different condition
	
	// Test case: L.28 - Enable notification whilst disable a service enabled for other condition
	INFO_PRINTF1(_L("Test case: L.28 - Enable notification whilst disable a service enabled for other condition"));
	
	iPhone.NotifyCallForwardingStatusChange(reqStat2,condition);
	User::After(2000000);
	
	changeInfo.iServiceGroup=RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionRegister;
	condition=RMobilePhone::ECallForwardingBusy;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	User::WaitForRequest(reqStat2);
	TEST(reqStat2==KErrNone);
	//notification should be 
	INFO_PRINTF1(_L("Notification condition"));
	retrieveCFList->PrintCondition(condition);

	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingBusy;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

	// Test case: L.29 - Cancel notification with  previous reposts
	INFO_PRINTF1(_L("Test case: L.29 - Cancel notification with previous reposts"));
	//register notification
	iPhone.NotifyCallForwardingStatusChange(reqStat2,condition);

	//set	
	changeInfo.iServiceGroup=RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionRegister;
	condition=RMobilePhone::ECallForwardingBusy;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);

	// disable EMobilePhoneNotifyCallForwardingStatusChange
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallForwardingStatusChange);
	
	User::WaitForRequest(reqStat2);
	TEST((reqStat2==KErrCancel)||(reqStat2==KErrNone)); 
	// because it is NOT assumed no previous CTestGetCFList
	INFO_PRINTF2(_L("The notify change returned %d"), reqStat2.Int());
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

//Tests on Specific Actions//
	
	INFO_PRINTF1(_L("Change simtsy config number"));
	SetTestNumberL(2);
	// there is just a single entry

	// Test case: L.30 -  Retrieve the status for all basic services for a specific condition
	INFO_PRINTF1(_L("Test case: L.30 -  Retrieve the status for all basic services for a specific condition"));
	condition=RMobilePhone::ECallForwardingBusy;
	// there should be only one entry
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingBusy;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;//EAllServices;//ECircuitDataService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: L.31 - Register a CF condition for all services to registered, whilst there is an active service for that condition
	INFO_PRINTF1(_L("Test case: L.31 - Register a  CF condition for all services to registered, whilst there is an active service for that condition"));
	condition=RMobilePhone::ECallForwardingBusy;
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionRegister;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber);
	changeInfo.iTimeout=30;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingBusy;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	condition=RMobilePhone::ECallForwardingBusy;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();
	
	// Test case: L.32 - Register a CF condition for one services, whilst all services are registered for that condition
	INFO_PRINTF1(_L("Test case: L.32 -Register a CF condition for one services, whilst all services are registered for that condition"));
	condition=RMobilePhone::ECallForwardingBusy;
	changeInfo.iServiceGroup=RMobilePhone::EFaxService;
	changeInfo.iAction=RMobilePhone::EServiceActionRegister;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber1);
	changeInfo.iTimeout=20;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again for the specific 
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingBusy;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	//retrieve for all - in fact here should return all as enabled
	condition=RMobilePhone::ECallForwardingBusy;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

    // Test case: L.33 - incorporates L.6 and L.9 with the outcome that there 
    // are N-1 active service entries in the list and all with the same settings
    // as those for all groups.
    INFO_PRINTF1(_L("Test case:L.33 - incorporates L.6 and L.9"));

	// Test case: L.34 - Register a previously not registered CF condition for all services to registered
	INFO_PRINTF1(_L("Test case: L.34 - Register a previously not registered CF condition for all services to registered"));
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionRegister;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber);
	condition=RMobilePhone::ECallForwardingNotReachable;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingNotReachable;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

	condition=RMobilePhone::ECallForwardingNotReachable;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

	
	// Test case: L.35 - Register a previously not registered CF condition for all services to registered
	INFO_PRINTF1(_L("Test case: L.35 - Register a previously not registered CF condition for all services to registered"));
	changeInfo.iServiceGroup = RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionActivate;
	changeInfo.iNumber.iTelNumber.Copy(newCFNumber);
	condition=RMobilePhone::ECallForwardingNotReachable;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingNotReachable;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	
	condition=RMobilePhone::ECallForwardingNotReachable;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();
	
	//Deactivate call forwarding not reachable active for voice service
	// Test case: L.36 - De-activate a previously active CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.36 - De-activate a previously active CF condition for a single service"));
	changeInfo.iServiceGroup=RMobilePhone::EVoiceService;
	changeInfo.iAction=RMobilePhone::EServiceActionDeactivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	
	// Test case Final Result Retrieve again - in this case the output is different - all services are active bar one 
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingNotReachable;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	
	condition=RMobilePhone::ECallForwardingNotReachable;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

    // just to show that we can deactivate other services
	// Test case: L.37 - De-activate a previously active CF condition for a single service
	INFO_PRINTF1(_L("Test case: L.37 - De-activate a previously active CF condition for a single service"));
	//Deactivate call forwarding not reachable actif for data service
	changeInfo.iServiceGroup=RMobilePhone::ECircuitDataService;
	changeInfo.iAction=RMobilePhone::EServiceActionDeactivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingNotReachable;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

    condition=RMobilePhone::ECallForwardingNotReachable;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

    // just to show that we can deactivate all services
	// Test case: L.38 - De-activate  previously active CF condition for a set of single services
	INFO_PRINTF1(_L("Test case: L.38 - De-activate  previously active CF condition for a set of single services"));
	condition=RMobilePhone::ECallForwardingNotReachable;
	 			
	changeInfo.iServiceGroup=RMobilePhone::EAllServices;
	changeInfo.iAction=RMobilePhone::EServiceActionDeactivate;
	iPhone.SetCallForwardingStatus(reqStat,condition, changeInfo);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);

	//Retrieve again
	ExpectedEntry1.iCondition=RMobilePhone::ECallForwardingNotReachable;
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallForwardingStatusNotActive;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EPacketDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EShortMessageService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	retrieveCFList->SetExpectedResultsL(ExpectedEntry1);

    condition=RMobilePhone::ECallForwardingNotReachable;
	retrieveCFList->PrintCondition(condition);
	retrieveCFList->Start(condition);
	CActiveScheduler::Start();

    // cleanup phase; remove 
	CActiveScheduler::Install(NULL);	
	CleanupStack::PopAndDestroy();
	delete retrieveCFList;
	
	INFO_PRINTF1(_L("End CF test"));
	return TestStepResult();	
	}


CTestCallWaiting::CTestCallWaiting()
	{
	SetTestStepName(_L("TestCallWaiting"));
	}

enum TVerdict CTestCallWaiting::doTestStepL()
/**
 * Local function that creates a CTestData object. 
 * Connects to the ETel Server, loads the phone module (SIM TSY), calls the BeginTest() 
 * function (which runs the tests) and then disconnects from the ETel Server at  
 * the end of the tests.
 */
	{
	CActiveScheduler* actSched=new(ELeave) CActiveScheduler();
	CleanupStack::PushL(actSched);
	CActiveScheduler::Install(actSched);

	INFO_PRINTF1(_L("CTestCallWaiting"));
	
	SetTestNumberL(0);

	TRequestStatus reqStat=KErrNone;	
	TRequestStatus reqStat2=KErrNone;
	
	
	// Test case: M.1 -retrieve CW status
	//expect 1 entry: active for voice as defined in config file
	INFO_PRINTF1(_L("Test case: M.1 - Retrieve CW status from config file"));

	CTestGetCWList* retrieveCWList=NULL;
	INFO_PRINTF1(_L("Step 1"));
	
	retrieveCWList = CTestGetCWList::NewLC(iPhone, this);

	RMobilePhone::TMobilePhoneCWInfoEntryV1 ExpectedEntry1;
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallWaitingStatusActive;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCWList->Start();
	CActiveScheduler::Start();


	// Test case: M.2 activate data, notify
	//and retrieve list with 2 entries
	INFO_PRINTF1(_L("Test case: M.2 - Activate data, notify and retrieve list with 2 entries"));
	
	RMobilePhone::TMobilePhoneCWInfoEntryV1 aCWInfo; 
	RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg aCWInfoBufPckg(aCWInfo); 
	iPhone.NotifyCallWaitingStatusChange(reqStat2,aCWInfoBufPckg);
	
	iPhone.SetCallWaitingStatus(reqStat,RMobilePhone::ECircuitDataService,RMobilePhone::EServiceActionActivate );
	
	User::WaitForRequest(reqStat);
	User::WaitForRequest(reqStat2);
	TEST(reqStat==KErrNone);
	TEST(reqStat2==KErrNone);
	TEST(aCWInfo.iServiceGroup == RMobilePhone::ECircuitDataService);
	TEST(aCWInfo.iStatus == RMobilePhone::ECallWaitingStatusActive);
	
	iPhone.NotifyCallWaitingStatusChange(reqStat2,aCWInfoBufPckg);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallWaitingStatusActive;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallWaitingStatusActive;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);
	
	retrieveCWList->Start();
	CActiveScheduler::Start();
	
	// Test case: M.3 - - cancel notification, deactivate voice,
	//and retrieve list that contains 1 entry
	INFO_PRINTF1(_L("Test case: M.3 - Cancel notification, deactivate voice,and retrieve list that contains 1 entry"));
	
	//deregister the notification reposted previously
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCallWaitingStatusChange);
	User::WaitForRequest(reqStat2);
	TEST(reqStat2==KErrCancel);
	
	iPhone.SetCallWaitingStatus(reqStat,RMobilePhone::EVoiceService,RMobilePhone::EServiceActionDeactivate );
	User::WaitForRequest(reqStat);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallWaitingStatusActive;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);
	
	retrieveCWList->Start();
	CActiveScheduler::Start();
	
	
	// Test case: M.4 - activate all and retrieve list that contains 1 entry
	INFO_PRINTF1(_L("Test case: M.4 - activate all and retrieve list that contains 1 entry"));
	
	iPhone.SetCallWaitingStatus(reqStat,RMobilePhone::EAllServices,RMobilePhone::EServiceActionActivate );
	User::WaitForRequest(reqStat);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus=RMobilePhone::ECallWaitingStatusActive;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);
	
	retrieveCWList->Start();
	CActiveScheduler::Start();
	
	
	// Test case: M.5 - Deactivate 1 and retrieve list that active entries for all but this one
	INFO_PRINTF1(_L("Test case: M.5 - Deactivate 1 and retrieve list that active entries for all but this oney"));
	
	iPhone.SetCallWaitingStatus(reqStat,RMobilePhone::EPacketDataService,RMobilePhone::EServiceActionDeactivate );
	User::WaitForRequest(reqStat);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::EVoiceService;
	ExpectedEntry1.iStatus=RMobilePhone::ECallWaitingStatusActive;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAuxVoiceService;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::ECircuitDataService;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::EFaxService;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);

	retrieveCWList->Start();
	CActiveScheduler::Start();
	
	
	// Test case: M.6 - deactivate all and retrieve list that contains 1 entries
	INFO_PRINTF1(_L("Test case: M.6 - Deactivate all and retrieve list that contains 1 entries"));
	
	iPhone.SetCallWaitingStatus(reqStat,RMobilePhone::EAllServices,RMobilePhone::EServiceActionDeactivate );
	User::WaitForRequest(reqStat);
	
	ExpectedEntry1.iServiceGroup=RMobilePhone::EAllServices;
	ExpectedEntry1.iStatus=RMobilePhone::ECallWaitingStatusNotActive;
	retrieveCWList->SetExpectedResultsL(ExpectedEntry1);
	
	retrieveCWList->Start();
	CActiveScheduler::Start();
	
	// cleanup phase; remove 
	CActiveScheduler::Install(NULL);	
	CleanupStack::PopAndDestroy();
	delete retrieveCWList;
	
	INFO_PRINTF1(_L("End CW test"));
	return TestStepResult();
	}

CTestIdentityService::CTestIdentityService()
	{
	SetTestStepName(_L("TestIdentityService"));
	}

enum TVerdict CTestIdentityService::doTestStepL()
/**
 * Local function that creates a CTestData object. 
 * Connects to the ETel Server, loads the phone module (SIM TSY), calls the BeginTest() 
 * function (which runs the tests) and then disconnects from the ETel Server at  
 * the end of the tests.
 */
	{
	CActiveScheduler* actSched=new(ELeave) CActiveScheduler();
	CleanupStack::PushL(actSched);
	CActiveScheduler::Install(actSched);
		
	INFO_PRINTF1(_L("CTestIdentityService"));
	INFO_PRINTF1(_L("Change simtsy config number"));

	SetTestNumberL(3);
	TRequestStatus reqStat=KErrNone;	
	RMobilePhone::TMobilePhoneIdServiceStatus IdServiceStat;
	RMobilePhone::TMobilePhoneIdService IdService=RMobilePhone::EIdServiceCallerPresentation;
 	
 	// Test case: N.1 - Get Caller presentation status
	INFO_PRINTF1(_L("Test case: N.1 - Get Caller presentation status"));
	iPhone.GetIdentityServiceStatus(reqStat, IdService, IdServiceStat, RMobilePhone::EInfoLocationNetwork);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	TEST(IdServiceStat==RMobilePhone::EIdServiceActivePermanent);
	PrintIdServiceStatus(IdServiceStat, IdService);

 	// Test case: N.2 - Get Caller restriction status
	INFO_PRINTF1(_L("Test case: N.2 - Get Caller restriction status"));
	IdService=RMobilePhone::EIdServiceCallerRestriction;
	iPhone.GetIdentityServiceStatus(reqStat, IdService, IdServiceStat, RMobilePhone::EInfoLocationNetwork);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	TEST(IdServiceStat==RMobilePhone::EIdServiceActiveDefaultRestricted);
	PrintIdServiceStatus(IdServiceStat, IdService);

 	// Test case: N.3 - Get connected presentation status
	INFO_PRINTF1(_L("Test case: N.3 - Get connected presentation status"));
	IdService=RMobilePhone::EIdServiceConnectedPresentation;
	iPhone.GetIdentityServiceStatus(reqStat, IdService, IdServiceStat, RMobilePhone::EInfoLocationNetwork);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	TEST(IdServiceStat==RMobilePhone::EIdServiceActiveDefaultAllowed);
	PrintIdServiceStatus(IdServiceStat, IdService);

 	// Test case: N.4 - Get connected restriction status
	INFO_PRINTF1(_L("Test case: N.4 - Get connected restriction status"));
	IdService=RMobilePhone::EIdServiceConnectedRestriction;
	iPhone.GetIdentityServiceStatus(reqStat, IdService, IdServiceStat, RMobilePhone::EInfoLocationNetwork);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	TEST(IdServiceStat==RMobilePhone::EIdServiceNotProvisioned);
	PrintIdServiceStatus(IdServiceStat, IdService);

 	// Test case: N.5 - Get caller name status
	INFO_PRINTF1(_L("Test case: N.5 - Get caller name status"));
	IdService=RMobilePhone::EIdServiceCallerName;
	iPhone.GetIdentityServiceStatus(reqStat, IdService, IdServiceStat, RMobilePhone::EInfoLocationNetwork);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	TEST(IdServiceStat==RMobilePhone::EIdServiceActivePermanent);
	PrintIdServiceStatus(IdServiceStat, IdService);

 	// Test case: N.6 - Get called presentation status
	INFO_PRINTF1(_L("Test case: N.6 - Get called presentation status"));
	IdService=RMobilePhone::EIdServiceCalledPresentation;
	iPhone.GetIdentityServiceStatus(reqStat, IdService, IdServiceStat, RMobilePhone::EInfoLocationNetwork);
	User::WaitForRequest(reqStat);
	TEST(reqStat==KErrNone);
	TEST(IdServiceStat==RMobilePhone::EIdServiceActiveDefaultRestricted);
	PrintIdServiceStatus(IdServiceStat, IdService);
	INFO_PRINTF1(_L(""));

    CActiveScheduler::Install(NULL);	
	CleanupStack::PopAndDestroy(actSched);	
	//Retrieve the status
	return TestStepResult();
	
	}
	
void CTestIdentityService::PrintIdServiceStatus(RMobilePhone::TMobilePhoneIdServiceStatus aIdServiceStat, RMobilePhone::TMobilePhoneIdService aIdService)
	{
	TBuf<200> IdServiceStatus;
	TBuf<200> IdService;
	
	switch (aIdServiceStat)
		{
		case RMobilePhone::EIdServiceActivePermanent:
			IdServiceStatus = _L("EIdServiceActivePermanent");
			break;
		case RMobilePhone::EIdServiceActiveDefaultRestricted:
			IdServiceStatus = _L("EIdServiceActiveDefaultRestricted");
			break;
		case RMobilePhone::EIdServiceActiveDefaultAllowed:
			IdServiceStatus = _L("EIdServiceActiveDefaultAllowed");
			break;
		case RMobilePhone::EIdServiceNotProvisioned:
			IdServiceStatus = _L("EIdServiceNotProvisioned");
			break;
		case RMobilePhone::EIdServiceUnknown:
			IdServiceStatus = _L("EIdServiceUnknown");
			break;
		default:
			break;
		}

	switch (aIdService)
		{
		case RMobilePhone::EIdServiceUnspecified:
			IdService = _L("EIdServiceUnspecified");
			break;
		case RMobilePhone::EIdServiceCallerPresentation:
			IdService = _L("EIdServiceCallerPresentation");
			break;
		case RMobilePhone::EIdServiceCallerRestriction:
			IdService = _L("EIdServiceCallerRestriction");
			break;
		case RMobilePhone::EIdServiceConnectedPresentation:
			IdService = _L("EIdServiceConnectedPresentation");
			break;
		case RMobilePhone::EIdServiceConnectedRestriction:
			IdService = _L("EIdServiceConnectedRestriction");
			break;
		case RMobilePhone::EIdServiceCallerName:
			IdService = _L("EIdServiceCallerName");
			break;
		case RMobilePhone::EIdServiceCalledPresentation:
			IdService = _L("EIdServiceCalledPresentation");
			break;
		default:
			break;
		}

	INFO_PRINTF3(_L("Phone Identity Status for >%S< = >%S<"),&IdService,&IdServiceStatus);
	}
