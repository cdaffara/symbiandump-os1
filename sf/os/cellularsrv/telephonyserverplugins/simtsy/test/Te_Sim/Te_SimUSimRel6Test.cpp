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
// Te_SimNetworkStatusTest.cpp
// 
//

#include "Te_SimUSimRel6Test.h"
#include <testconfigfileparser.h>
#include <etelmm.h>
#include "SimConstants.h"

/****************************************************************************\
 * 																			*
 * 			Videomail  Testcases							 				*	
 * 																			*	
 ****************************************************************************/

CSimVideomailTest1::CSimVideomailTest1() 
	{ 
	SetTestStepName(_L("VideomailTest1"));
	}
		
TVerdict CSimVideomailTest1::doTestStepL()
/**
TEL-SIMTSY-MM-8653.
Client retrieves mailbox dialling numbers successfully; writes a new phonebook entry successfully.
*/
	{  	
	INFO_PRINTF1(_L("Testing Videomail features"));
	TRequestStatus stat0,stat1;
	RMobilePhone::TMobilePhoneVoicemailIdsV8 voicemailId;
	RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg voicemailIdPckg(voicemailId);
	iPhone.NotifyMailboxNumbersChange(stat0,voicemailIdPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(voicemailId.iData==iVoiceMailIds->At(0).iData);
	TESTL(voicemailId.iFax==iVoiceMailIds->At(0).iFax);
	TESTL(voicemailId.iOther==iVoiceMailIds->At(0).iOther);
	TESTL(voicemailId.iVoice==iVoiceMailIds->At(0).iVoice);
	TESTL(voicemailId.iVideo==iVoiceMailIds->At(0).iVideo);
	
	iPhone.GetMailboxNumbers(stat1,voicemailIdPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);	 
	TESTL(voicemailId.iData==iVoiceMailIds->At(0).iData);
	TESTL(voicemailId.iFax==iVoiceMailIds->At(0).iFax);
	TESTL(voicemailId.iOther==iVoiceMailIds->At(0).iOther);
	TESTL(voicemailId.iVoice==iVoiceMailIds->At(0).iVoice);
	TESTL(voicemailId.iVideo==iVoiceMailIds->At(0).iVideo);
			
  	INFO_PRINTF1(_L("Testing Get Phonebook store info from Phone level."));
 	TESTL(iPhonebook.Open(iPhone,KETelIccMbdnPhoneBook,KEtelUSimPhoneBookType)==KErrNone);
  	INFO_PRINTF1(_L("Opened Phonebook object"));
    INFO_PRINTF1(_L("Testing Phonebook Write"));
	
	TUint32 notifyEvent;
	TInt notifyIndex;
	iPhonebook.NotifyStoreEvent(stat1,notifyEvent,notifyIndex);
	TInt index=7;
	TBuf8<100> phBkBuf;
	phBkBuf.Copy(KTestPhonebookB,sizeof(KTestPhonebookB));
	iPhonebook.Write(stat0,phBkBuf,index);
	User::WaitForRequest(stat0);
	User::WaitForRequest(stat1);
	TESTL(notifyEvent==(RMobilePhoneStore::KStoreHasSpace |
						RMobilePhoneStore::KStoreEntryAdded));
	TESTL(notifyIndex==7);
	TInt numEntries=1;
 	iPhonebook.Read(stat0,notifyIndex,numEntries,phBkBuf);
  	User::WaitForRequest(stat0);
  	TESTL(stat0==KErrNone);
	iPhonebook.Close(); 	
	return TestStepResult();
	}

CSimVideomailTest2::CSimVideomailTest2() 
	{ 
	SetTestStepName(_L("VideomailTest2"));
	}

		
TVerdict CSimVideomailTest2::doTestStepL()
/**
TEL-SIMTSY-MM-8654.
Client is notified of the change in videomail mailbox number successfully.
*/
	{ 
  	TRequestStatus stat0;
  	RMobilePhone::TMobilePhoneVoicemailIdsV8 voicemailId;
	RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg voicemailIdPckg(voicemailId);
  	iPhone.NotifyMailboxNumbersChange(stat0,voicemailIdPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(voicemailId.iData==iVoiceMailIds->At(0).iData);
	TESTL(voicemailId.iFax==iVoiceMailIds->At(0).iFax);
	TESTL(voicemailId.iOther==iVoiceMailIds->At(0).iOther);
	TESTL(voicemailId.iVoice==iVoiceMailIds->At(0).iVoice);
	TESTL(voicemailId.iVideo==iVoiceMailIds->At(0).iVideo);
	return TestStepResult();
	}

CSimVideomailTest3::CSimVideomailTest3() 
	{ 
	SetTestStepName(_L("VideomailTest3"));
	}
		
TVerdict CSimVideomailTest3::doTestStepL()
/**
TEL-SIMTSY-MM-8655.
Client retrieves message waiting indicators and sets one indicator to off successfully.
*/
	{
	TRequestStatus stat0,stat1;
	RMobilePhone::TMobilePhoneMessageWaitingV8 messageWaiting;
	RMobilePhone::TMobilePhoneMessageWaitingV8Pckg messageWaitingPckg(messageWaiting);
	
	TInt messageWaitCount;
	iPhone.NotifyMessageWaiting(stat0,messageWaitCount);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(messageWaitCount==6);	
	
	iPhone.NotifyIccMessageWaitingIndicatorsChange(stat1,messageWaitingPckg);		
	
	messageWaiting.iDisplayStatus=iMessageWaiting->At(0).iDisplayStatus;
	messageWaiting.iVoiceMsgs=iMessageWaiting->At(0).iVoiceMsgs;
	messageWaiting.iAuxVoiceMsgs=iMessageWaiting->At(0).iAuxVoiceMsgs;
	messageWaiting.iDataMsgs=iMessageWaiting->At(0).iDataMsgs;
	messageWaiting.iFaxMsgs=iMessageWaiting->At(0).iFaxMsgs;
	messageWaiting.iEmailMsgs=iMessageWaiting->At(0).iEmailMsgs;
	messageWaiting.iOtherMsgs=iMessageWaiting->At(0).iOtherMsgs;
	messageWaiting.iVideoMsgs=iMessageWaiting->At(0).iVideoMsgs;
	
	iPhone.SetIccMessageWaitingIndicators(stat0,messageWaitingPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);	
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);
	TESTL(messageWaiting.iDisplayStatus==iMessageWaiting->At(0).iDisplayStatus);
	TESTL(messageWaiting.iVoiceMsgs==iMessageWaiting->At(0).iVoiceMsgs);
	TESTL(messageWaiting.iAuxVoiceMsgs==iMessageWaiting->At(0).iAuxVoiceMsgs);
	TESTL(messageWaiting.iDataMsgs==iMessageWaiting->At(0).iDataMsgs);
	TESTL(messageWaiting.iFaxMsgs==iMessageWaiting->At(0).iFaxMsgs);
	TESTL(messageWaiting.iEmailMsgs==iMessageWaiting->At(0).iEmailMsgs);
	TESTL(messageWaiting.iOtherMsgs==iMessageWaiting->At(0).iOtherMsgs);
	TESTL(messageWaiting.iVideoMsgs==iMessageWaiting->At(0).iVideoMsgs);
	
	iPhone.GetIccMessageWaitingIndicators(stat1,messageWaitingPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone); 
	TESTL(messageWaiting.iDisplayStatus==iMessageWaiting->At(0).iDisplayStatus);
	TESTL(messageWaiting.iVoiceMsgs==iMessageWaiting->At(0).iVoiceMsgs);
	TESTL(messageWaiting.iAuxVoiceMsgs==iMessageWaiting->At(0).iAuxVoiceMsgs);
	TESTL(messageWaiting.iDataMsgs==iMessageWaiting->At(0).iDataMsgs);
	TESTL(messageWaiting.iFaxMsgs==iMessageWaiting->At(0).iFaxMsgs);
	TESTL(messageWaiting.iEmailMsgs==iMessageWaiting->At(0).iEmailMsgs);
	TESTL(messageWaiting.iOtherMsgs==iMessageWaiting->At(0).iOtherMsgs);
	TESTL(messageWaiting.iVideoMsgs==iMessageWaiting->At(0).iVideoMsgs);
		
	return TestStepResult();
	}

CSimVideomailTest4::CSimVideomailTest4() 
	{ 
	SetTestStepName(_L("VideomailTest4"));
	}
		
TVerdict CSimVideomailTest4::doTestStepL()
/**
TEL-SIMTSY-MM-8656.
Client is unable to set message waiting indicators due to absence of indicators on the UICC.
*/
	{ 
	TRequestStatus stat0,stat1;
	RMobilePhone::TMobilePhoneMessageWaitingV8 messageWaiting;
	RMobilePhone::TMobilePhoneMessageWaitingV8Pckg messageWaitingPckg(messageWaiting);
	RMobilePhone::TMobilePhoneMessageWaitingV8 messageWaitingV8;
	RMobilePhone::TMobilePhoneMessageWaitingV8Pckg messageWaitingPckgV8(messageWaitingV8);
	
	TInt messageWaitCount;
	iPhone.NotifyMessageWaiting(stat0,messageWaitCount);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(messageWaitCount==6);
	
	iPhone.NotifyIccMessageWaitingIndicatorsChange(stat1,messageWaitingPckgV8);
		
	iPhone.GetIccMessageWaitingIndicators(stat0,messageWaitingPckgV8);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone); 
	TESTL(messageWaitingV8.iDisplayStatus==iMessageWaiting->At(0).iDisplayStatus);
	TESTL(messageWaitingV8.iVoiceMsgs==iMessageWaiting->At(0).iVoiceMsgs);
	TESTL(messageWaitingV8.iAuxVoiceMsgs==iMessageWaiting->At(0).iAuxVoiceMsgs);
	TESTL(messageWaitingV8.iDataMsgs==iMessageWaiting->At(0).iDataMsgs);
	TESTL(messageWaitingV8.iFaxMsgs==iMessageWaiting->At(0).iFaxMsgs);
	TESTL(messageWaitingV8.iEmailMsgs==iMessageWaiting->At(0).iEmailMsgs);
	TESTL(messageWaitingV8.iOtherMsgs==iMessageWaiting->At(0).iOtherMsgs);
	TESTL(messageWaitingV8.iVideoMsgs==iMessageWaiting->At(0).iVideoMsgs);
		
	messageWaiting.iDisplayStatus=iMessageWaiting->At(0).iDisplayStatus;
	messageWaiting.iVoiceMsgs=iMessageWaiting->At(0).iVoiceMsgs;
	messageWaiting.iAuxVoiceMsgs=iMessageWaiting->At(0).iAuxVoiceMsgs;
	messageWaiting.iDataMsgs=iMessageWaiting->At(0).iDataMsgs;
	messageWaiting.iFaxMsgs=iMessageWaiting->At(0).iFaxMsgs;
	messageWaiting.iEmailMsgs=iMessageWaiting->At(0).iEmailMsgs;
	messageWaiting.iOtherMsgs=iMessageWaiting->At(0).iOtherMsgs;
	iPhone.SetIccMessageWaitingIndicators(stat0,messageWaitingPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrCorrupt);
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);
	TESTL(messageWaitingV8.iDisplayStatus==iMessageWaiting->At(0).iDisplayStatus);
	TESTL(messageWaitingV8.iVoiceMsgs==iMessageWaiting->At(0).iVoiceMsgs);
	TESTL(messageWaitingV8.iAuxVoiceMsgs==iMessageWaiting->At(0).iAuxVoiceMsgs);
	TESTL(messageWaitingV8.iDataMsgs==iMessageWaiting->At(0).iDataMsgs);
	TESTL(messageWaitingV8.iFaxMsgs==iMessageWaiting->At(0).iFaxMsgs);
	TESTL(messageWaitingV8.iEmailMsgs==iMessageWaiting->At(0).iEmailMsgs);
	TESTL(messageWaitingV8.iOtherMsgs==iMessageWaiting->At(0).iOtherMsgs);
	TESTL(messageWaitingV8.iVideoMsgs==iMessageWaiting->At(0).iVideoMsgs);
		
	return TestStepResult();
	}



CSimVideomailTest5::CSimVideomailTest5() 
	{ 
	SetTestStepName(_L("VideomailTest5"));
	}
		
TVerdict CSimVideomailTest5::doTestStepL()
/**
TEL-SIMTSY-MM-8657.
Cancel Message waiting and MBDN requests. (All APIs)
*/
	{ 
	TRequestStatus stat0,stat1;
	RMobilePhone::TMobilePhoneMessageWaitingV8 messageWaiting;
	RMobilePhone::TMobilePhoneMessageWaitingV8Pckg messageWaitingPckg(messageWaiting);
	
	TInt messageWaitCount;
	iPhone.NotifyMessageWaiting(stat0,messageWaitCount);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyMessageWaiting);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrCancel);
		
	iPhone.NotifyIccMessageWaitingIndicatorsChange(stat1,messageWaitingPckg);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange);
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrCancel);
	
  	RMobilePhone::TMobilePhoneVoicemailIdsV8 voicemailId;
	RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg voicemailIdPckg(voicemailId);
  	iPhone.NotifyMailboxNumbersChange(stat0,voicemailIdPckg);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyMailboxNumbersChange);	
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrCancel);
	
	return TestStepResult();
	}

/****************************************************************************\
 * 																			*
 * 			Wireless Local Area Network  Testcases	 						*	
 * 																			*	
 ****************************************************************************/

CSimWlanTest1::CSimWlanTest1() 
	{ 
	SetTestStepName(_L("Wlan Test1"));
	}
		
TVerdict CSimWlanTest1::doTestStepL()
/**
TEL-SIMTSY-MM-8663.
Client stores the Wlan parameters on USIM successfully.
*/
	{ 
	// local variables used throughout the tests
	TRequestStatus reqStatus,aReqStatus,stat0,reqStatus1;
	RMobilePhone::TMobilePhoneSecurityCode whichCode = RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8);
	
	RMobilePhone::TMobilePassword code = _L("1111");
	RMobilePhone::TMobilePassword unblockCode = _L("1111");
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	iPhone.NotifySecurityEvent(aReqStatus,securityEvent);
	usimWlanDataV8.iFieldsUsed = iWlanData->At(0).iFieldsUsed;
	iPhone.NotifyWlanDataChange(reqStatus1,usimWlanDataV8Pckgd);
	
	TestSmartCardAuthenticate();

  	usimWlanDataV8.iPseudonym = iWlanData->At(0).iPseudonym ;
  	usimWlanDataV8.iReauthenticationId = iWlanData->At(0).iReauthenticationId;	 
  	iPhone.SetWlanData(reqStatus,usimWlanDataV8Pckgd);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinRequired);
	
	iPhone.NotifySecurityEvent(stat0,securityEvent);
	iPhone.VerifySecurityCode(aReqStatus,whichCode,code,unblockCode);
	User::WaitForRequest(aReqStatus);
	User::WaitForRequest(stat0);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinVerified);
  	User::WaitForRequest(reqStatus);
  	User::WaitForRequest(reqStatus1);
  	TEST(reqStatus.Int() == KErrNone);
  	TEST(reqStatus1.Int() == KErrNone);
  	TEST(usimWlanDataV8.iFieldsUsed == iWlanData->At(0).iFieldsUsed);
  	TEST(usimWlanDataV8.iPseudonym == iWlanData->At(0).iPseudonym);
  	TEST(usimWlanDataV8.iReauthenticationId == iWlanData->At(0).iReauthenticationId);
	return TestStepResult();
	}

CSimWlanTest2::CSimWlanTest2() 
	{ 
	SetTestStepName(_L("Wlan Test2"));
	}
		
TVerdict CSimWlanTest2::doTestStepL()
/**
TEL-SIMTSY-MM-8664.
Client is unable to store the Wlan parameters on USIM due to PIN verification failure.
*/
	{
	TRequestStatus reqStatus,aReqStatus,stat0;
	RMobilePhone::TMobilePhoneSecurityCode whichCode = RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8); 
	
	RMobilePhone::TMobilePassword code = _L("2222");
	RMobilePhone::TMobilePassword unblockCode = _L("1111");
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	iPhone.NotifySecurityEvent(stat0,securityEvent);
	
	TestSmartCardAuthenticate();
	
  	usimWlanDataV8.iFieldsUsed = iWlanData->At(1).iFieldsUsed;
  	usimWlanDataV8.iPseudonym = SIMTSY_PHONE_WLAN_ACESS_DENIED;
  	usimWlanDataV8.iReauthenticationId = iWlanData->At(1).iReauthenticationId;
 	iPhone.SetWlanData(reqStatus,usimWlanDataV8Pckgd);
  	User::WaitForRequest(stat0);
	TEST(stat0.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinRequired);
	iPhone.VerifySecurityCode(aReqStatus,whichCode,code,unblockCode);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrAccessDenied);	
  	User::WaitForRequest(reqStatus);
  	TEST(reqStatus.Int() == KErrAccessDenied); 
	return TestStepResult();
	}

CSimWlanTest3::CSimWlanTest3() 
	{ 
	SetTestStepName(_L("Wlan Test3"));
	}
		
TVerdict CSimWlanTest3::doTestStepL()
/**
TEL-SIMTSY-MM-8665.
Client, at its own will, retrieves WLAN parameters from USIM successfully.
*/
	{
 	TRequestStatus reqStatus,aReqStatus,stat0;
	RMobilePhone::TMobilePhoneSecurityCode whichCode = RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8);
 
	RMobilePhone::TMobilePassword code = _L("1111");
	RMobilePhone::TMobilePassword unblockCode = _L("1111");
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	iPhone.NotifySecurityEvent(stat0,securityEvent);
  	
  	usimWlanDataV8.iFieldsUsed = iWlanData->At(1).iFieldsUsed;
  	iPhone.GetWlanData(reqStatus,usimWlanDataV8Pckgd);
  	User::WaitForRequest(stat0);
	TEST(stat0.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinRequired);
	
	iPhone.NotifySecurityEvent(stat0,securityEvent);
	iPhone.VerifySecurityCode(aReqStatus,whichCode,code,unblockCode);
	User::WaitForRequest(aReqStatus);
	User::WaitForRequest(stat0);
	User::WaitForRequest(reqStatus);
	TEST(stat0.Int() == KErrNone);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(reqStatus.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinVerified);
	TEST(usimWlanDataV8.iFieldsUsed == iWlanData->At(1).iFieldsUsed);
  	TEST(usimWlanDataV8.iPseudonym == iWlanData->At(1).iPseudonym);
  	TEST(usimWlanDataV8.iReauthenticationId == iWlanData->At(1).iReauthenticationId);
  	TEST(usimWlanDataV8.iMasterKey == iWlanData->At(1).iMasterKey);
  	TEST(usimWlanDataV8.iCounter == iWlanData->At(1).iCounter);
	return TestStepResult();
	}

CSimWlanTest4::CSimWlanTest4() 
	{ 
	SetTestStepName(_L("Wlan Test4"));
	}
		
TVerdict CSimWlanTest4::doTestStepL()
/**
TEL-SIMTSY-MM-8667.
Client is unable to retrieve WLAN parameters from USIM due to lack of support in USIM service table
*/
	{ 
	TRequestStatus reqStatus;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8);
 
 	usimWlanDataV8.iCounter = iWlanData->At(2).iCounter;
 	iPhone.GetWlanData(reqStatus,usimWlanDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotSupported);		
	return TestStepResult();
	}
	
CSimWlanTest5::CSimWlanTest5() 
	{ 
	SetTestStepName(_L("Wlan Test5"));
	}
		
TVerdict CSimWlanTest5::doTestStepL()
/**
TEL-SIMTSY-MM-8670.
Client is unable to retrieve WLAN parameters from USIM due to PIN verification failure.
*/
	{ 
	TRequestStatus reqStatus,aReqStatus,stat0,reqStatus1;
	RMobilePhone::TMobilePhoneSecurityCode whichCode = RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8);
	
	RMobilePhone::TMobilePassword code = _L("1111");
	RMobilePhone::TMobilePassword unblockCode = _L("1111");
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	iPhone.NotifySecurityEvent(aReqStatus,securityEvent);
	usimWlanDataV8.iFieldsUsed = iWlanData->At(0).iFieldsUsed;
	iPhone.NotifyWlanDataChange(reqStatus1,usimWlanDataV8Pckgd);
	
	TestSmartCardAuthenticate();

  	usimWlanDataV8.iPseudonym = iWlanData->At(0).iPseudonym ;
  	usimWlanDataV8.iReauthenticationId = iWlanData->At(0).iReauthenticationId;	 
  	iPhone.SetWlanData(reqStatus,usimWlanDataV8Pckgd);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinRequired);
	
	iPhone.NotifySecurityEvent(stat0,securityEvent);
	iPhone.VerifySecurityCode(aReqStatus,whichCode,code,unblockCode);
	User::WaitForRequest(aReqStatus);
	User::WaitForRequest(stat0);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinVerified);
  	User::WaitForRequest(reqStatus);
  	User::WaitForRequest(reqStatus1);
  	TEST(reqStatus.Int() == KErrNone);
  	TEST(reqStatus1.Int() == KErrNone);
  	TEST(usimWlanDataV8.iFieldsUsed == iWlanData->At(0).iFieldsUsed);
  	TEST(usimWlanDataV8.iPseudonym == iWlanData->At(0).iPseudonym);
  	TEST(usimWlanDataV8.iReauthenticationId == iWlanData->At(0).iReauthenticationId);
	
	code = _L("2222");
	iPhone.NotifySecurityEvent(stat0,securityEvent);
	usimWlanDataV8.iPseudonym = SIMTSY_PHONE_WLAN_ACESS_DENIED;
 	iPhone.GetWlanData(reqStatus,usimWlanDataV8Pckgd);
 	User::WaitForRequest(stat0);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinRequired);
	
	iPhone.VerifySecurityCode(aReqStatus,whichCode,code,unblockCode);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrAccessDenied);	
  	User::WaitForRequest(reqStatus);
  	TEST(reqStatus.Int() == KErrAccessDenied); 
	return TestStepResult();
	}
	
CSimWlanTest6::CSimWlanTest6() 
	{ 
	SetTestStepName(_L("Wlan Test6"));
	}
		
TVerdict CSimWlanTest6::doTestStepL()
/**
TEL-SIMTSY-MM-8669.
Client is unable to retrieve WLAN parameters from USIM due to due to inactive USIM application.
*/
	{
	TRequestStatus reqStatus,aReqStatus;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8);
  	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;
	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);
	authenticateData.iAUTN = KConfigScWlanAuthAUTN();
	authenticateData.iRAND = KConfigScWlanAuthRAND();
	authenticateData.iCK = KConfigScWlan();
	
	RMobilePhone::TAID appAID;
	iPhone.SmartCardAuthenticate(aReqStatus,appAID,authenticateDataPckg);
	usimWlanDataV8.iPseudonym = SIMTSY_PHONE_WLAN_ACESS_DENIED;
 	iPhone.GetWlanData(reqStatus,usimWlanDataV8Pckgd);
 	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus==KErrMMEtelScApplicationNotActive);
  	User::WaitForRequest(reqStatus);
  	TEST(reqStatus.Int() == KErrAccessDenied); 
	return TestStepResult();
	}
	
CSimWlanTest7::CSimWlanTest7() 
	{ 
	SetTestStepName(_L("Wlan Test7"));
	}
		
TVerdict CSimWlanTest7::doTestStepL()
/**
TEL-SIMTSY-MM-8671.
Client retrieves the preferred networks list present on the UICC successfully.
*/
	{ 
	CTestGetPreferredNetworks* preferredNetworks=CTestGetPreferredNetworks::NewLC(iPhone);
    preferredNetworks->Start();
    CActiveScheduler::Start();
    delete preferredNetworks->iTestPhone->iNetworkList;
	delete preferredNetworks->iTestPhone->iWlanList;
    delete preferredNetworks;
	return TestStepResult();
	}

CSimWlanTest8::CSimWlanTest8() 
	{ 
	SetTestStepName(_L("Wlan Test8"));
	}
		
TVerdict CSimWlanTest8::doTestStepL()
/**
TEL-SIMTSY-MM-8674.
Client retrieves the WLAN SID list present on the UICC successfully.
*/
	{ 
	CTestRetrieveMobilePhonePreferredWlanSIDs* preferredWlanSidsList=CTestRetrieveMobilePhonePreferredWlanSIDs::NewLC(iPhone);
	preferredWlanSidsList->Start();
	CActiveScheduler::Start();
	delete preferredWlanSidsList->iTestWLAN->iNetworkList;
	delete preferredWlanSidsList->iTestWLAN->iWlanList;
    delete preferredWlanSidsList;
	return TestStepResult();
	}

CSimWlanTest9::CSimWlanTest9() 
	{ 
	SetTestStepName(_L("Wlan Test9"));
	}
		
TVerdict CSimWlanTest9::doTestStepL()
/**
TEL-SIMTSY-MM-8675.
Client edits the WLAN SID list and stores back on UICC successfully.
*/
	{
	TRequestStatus reqStatus,aReqStatus;
	iPhone.NotifyPreferredWlanSIDListChange(aReqStatus);
	CTestRetrieveMobilePhonePreferredWlanSIDs* preferredWlanSidsList=CTestRetrieveMobilePhonePreferredWlanSIDs::NewLC(iPhone);
	preferredWlanSidsList->Start();
	CActiveScheduler::Start();	
    RMobilePhone::TWlanSIDV8 wlanSIDV8_1;
    wlanSIDV8_1.iWSID = iWlanSid->At(0).iWSID;
    wlanSIDV8_1.iUserDefined = iWlanSid->At(0).iUserDefined;
    preferredWlanSidsList->iTestWLAN->iWlanList->ChangeEntryL(0,wlanSIDV8_1);
    preferredWlanSidsList->iTestWLAN->iWlanList->DeleteEntryL(1);
    preferredWlanSidsList->iTestWLAN->iWlanList->AddEntryL(wlanSIDV8_1);     
    iPhone.StorePreferredWlanSIDListL(reqStatus,preferredWlanSidsList->iTestWLAN->iWlanList);
    User::WaitForRequest(reqStatus);   
    TEST(reqStatus.Int() == KErrNone);
    User::WaitForRequest(aReqStatus);
    TEST(aReqStatus.Int() == KErrNone);
    delete preferredWlanSidsList->iTestWLAN->iNetworkList; 
    delete preferredWlanSidsList->iTestWLAN->iWlanList; 
    delete preferredWlanSidsList;
	return TestStepResult();
	}
	
CSimWlanTest10::CSimWlanTest10() 
	{ 
	SetTestStepName(_L("Wlan Test10"));
	}
		
TVerdict CSimWlanTest10::doTestStepL()
/**
TEL-SIMTSY-MM-8684
Test to cancel all the WLAN APIs successfully.
*/
	{ 
	TRequestStatus reqStatus,aReqStatus,stat0;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8);
	iPhone.NotifyPreferredWlanSIDListChange(aReqStatus);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyPreferredWlanSIDListChange);
    User::WaitForRequest(aReqStatus);
    TEST(aReqStatus.Int() == KErrCancel);
    
    iPhone.NotifyPreferredNetworksListChange(reqStatus);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyStorePreferredNetworksListChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrCancel);
	
	usimWlanDataV8.iFieldsUsed = iWlanData->At(1).iFieldsUsed;
	iPhone.NotifyWlanDataChange(reqStatus,usimWlanDataV8Pckgd);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyWlanDataChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrCancel);
	
	//preferredWlanSids Cancel
	CTestRetrieveMobilePhonePreferredWlanSIDs* preferredWlanSidsList=CTestRetrieveMobilePhonePreferredWlanSIDs::NewLC(iPhone);
	preferredWlanSidsList->Start();
	preferredWlanSidsList->Cancel();
	User::After(1500000L);
	
	CTestGetPreferredNetworks* preferredNetworks=CTestGetPreferredNetworks::NewLC(iPhone);
    preferredNetworks->Start();
    preferredNetworks->Cancel();
    User::After(1500000L);
    
    delete preferredWlanSidsList;
    delete preferredNetworks;
	return TestStepResult();
	}
	
CSimWlanTest11::CSimWlanTest11() 
	{ 
	SetTestStepName(_L("Wlan Test11"));
	}
		
TVerdict CSimWlanTest11::doTestStepL()
/**
TEL-SIMTSY-MM-8676.
Client is unable to store the WLAN SID list back on the UICC because the submitted list is larger than the expected.
*/
	{ 
	TRequestStatus reqStatus,aReqStatus,stat0;
	iPhone.NotifyPreferredWlanSIDListChange(aReqStatus);
	CTestRetrieveMobilePhonePreferredWlanSIDs* preferredWlanSidsList=CTestRetrieveMobilePhonePreferredWlanSIDs::NewLC(iPhone);
	preferredWlanSidsList->Start();
	CActiveScheduler::Start();
    RMobilePhone::TWlanSIDV8 wlanSIDV8_1;
    wlanSIDV8_1.iWSID = iWlanSid->At(0).iWSID;
    wlanSIDV8_1.iUserDefined = iWlanSid->At(0).iUserDefined;
    preferredWlanSidsList->iTestWLAN->iWlanList->AddEntryL(wlanSIDV8_1);
      
    iPhone.StorePreferredWlanSIDListL(reqStatus,preferredWlanSidsList->iTestWLAN->iWlanList);
    User::WaitForRequest(reqStatus);
    
    TEST(reqStatus.Int() == KErrNoMemory);
    User::WaitForRequest(aReqStatus);
    TEST(aReqStatus.Int() == KErrNone); 
    delete preferredWlanSidsList->iTestWLAN->iWlanList;
    delete preferredWlanSidsList->iTestWLAN->iNetworkList;
    delete preferredWlanSidsList;
	return TestStepResult();
	}
	
CSimWlanTest12::CSimWlanTest12() 
	{ 
	SetTestStepName(_L("Wlan Test12"));
	}
		
TVerdict CSimWlanTest12::doTestStepL()
/**
TEL-SIMTSY-MM-8677.
Test SetWlanData with different capability
*/
	{ 
	TRequestStatus reqStatus,aReqStatus,stat0;
	TInt ret;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8);
	usimWlanDataV8.iFieldsUsed = iWlanData->At(1).iFieldsUsed;
  	usimWlanDataV8.iPseudonym = iWlanData->At(1).iPseudonym ;
  	usimWlanDataV8.iReauthenticationId = iWlanData->At(1).iReauthenticationId;
  	usimWlanDataV8.iMasterKey = iWlanData->At(1).iMasterKey;
  	usimWlanDataV8.iCounter = iWlanData->At(1).iCounter;
	iPhone.SetWlanData(reqStatus,usimWlanDataV8Pckgd);
  	User::WaitForRequest(reqStatus);
  	ret=reqStatus.Int();
 	INFO_PRINTF2(_L("ret=%d"),ret);
	TEST(reqStatus.Int() == KErrPermissionDenied);
	return TestStepResult();
	}
	
CSimWlanTest13::CSimWlanTest13() 
	{ 
	SetTestStepName(_L("Wlan Test13"));
	}
		
TVerdict CSimWlanTest13::doTestStepL()
/**
TEL-SIMTSY-MM-8678.
Test GetWlanData with different capability
*/
	{
	TRequestStatus reqStatus,aReqStatus,stat0;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8); 
	usimWlanDataV8.iFieldsUsed = iWlanData->At(0).iFieldsUsed;
	iPhone.GetWlanData(reqStatus,usimWlanDataV8Pckgd);
  	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrPermissionDenied);
	return TestStepResult();
	}
	
CSimWlanTest14::CSimWlanTest14() 
	{ 
	SetTestStepName(_L("Wlan Test14"));
	}
		
TVerdict CSimWlanTest14::doTestStepL()
/**
TEL-SIMTSY-MM-8679.
Test NotifyWlanDataChange with different capability
*/
	{
	TRequestStatus reqStatus,aReqStatus,stat0;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8); 	
  
    usimWlanDataV8.iFieldsUsed = iWlanData->At(1).iFieldsUsed;
	iPhone.NotifyWlanDataChange(reqStatus,usimWlanDataV8Pckgd);
	User::WaitForRequest(reqStatus);
    TEST(reqStatus.Int()==KErrPermissionDenied);
     
	return TestStepResult();
	}

CSimWlanTest15::CSimWlanTest15() 
	{ 
	SetTestStepName(_L("Wlan Test15"));
	}
		
TVerdict CSimWlanTest15::doTestStepL()
/**
TEL-SIMTSY-MM-8681.Test 
StorePreferredWlanSIDListL with different capability
*/
	{ 
	TRequestStatus reqStatus,aReqStatus,stat0;
	RMobilePhone::TWlanSIDV8 wlanSIDV8_1;
  	RMobilePhone::TWlanSIDV8 wlanSIDV8_2;
  	RMobilePhone::TWlanSIDV8 wlanSIDV8_3;
  	
  	CMobilePhoneStoredWlanSIDList* wlanSIDlist;
  	wlanSIDlist =  CMobilePhoneStoredWlanSIDList::NewL();
    CleanupStack::PushL(wlanSIDlist);
      
    wlanSIDV8_1.iWSID = iWlanSid->At(0).iWSID;
  	wlanSIDV8_2.iWSID = iWlanSid->At(1).iWSID;
  	wlanSIDV8_3.iWSID = iWlanSid->At(2).iWSID;
  	
  	wlanSIDV8_1.iUserDefined = iWlanSid->At(0).iUserDefined;
  	wlanSIDV8_2.iUserDefined = iWlanSid->At(1).iUserDefined;
  	wlanSIDV8_3.iUserDefined = iWlanSid->At(2).iUserDefined;
  		
    wlanSIDlist->AddEntryL(wlanSIDV8_1);
    wlanSIDlist->AddEntryL(wlanSIDV8_2);
    wlanSIDlist->AddEntryL(wlanSIDV8_3);
      
    iPhone.StorePreferredWlanSIDListL(reqStatus,wlanSIDlist);
    User::WaitForRequest(reqStatus);
    
    TEST(reqStatus.Int()==KErrPermissionDenied);
    CleanupStack::PopAndDestroy();
	return TestStepResult();
	}

CSimWlanTest16::CSimWlanTest16() 
	{ 
	SetTestStepName(_L("Wlan Test16"));
	}
		
TVerdict CSimWlanTest16::doTestStepL()
/**
TEL-SIMTSY-MM-8682
Test StorePreferredNetworksListL() with different capability
*/
	{
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry1;
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry2;
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry3;
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry4; //- FOR WLAN
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntryFetch;
	
	preferredNetworkEntry4.iAccess = SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_WLAN;
	preferredNetworkEntry4.iUserDefined = iPreferredNetworks->At(0).iUserDefined;
	preferredNetworkEntry4.iCountryCode = iPreferredNetworks->At(0).iCountryCode;
	preferredNetworkEntry4.iNetworkId = iPreferredNetworks->At(0).iNetworkId;
	
	preferredNetworkEntry3.iAccess = SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_UTRAN;
	preferredNetworkEntry3.iUserDefined = iPreferredNetworks->At(1).iUserDefined;
	preferredNetworkEntry3.iCountryCode = iPreferredNetworks->At(1).iCountryCode;
	preferredNetworkEntry3.iNetworkId = iPreferredNetworks->At(1).iNetworkId;
	
	preferredNetworkEntry2.iAccess = SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSMC;
	preferredNetworkEntry2.iUserDefined = iPreferredNetworks->At(2).iUserDefined;
	preferredNetworkEntry2.iCountryCode = iPreferredNetworks->At(2).iCountryCode;
	preferredNetworkEntry2.iNetworkId = iPreferredNetworks->At(2).iNetworkId;
	
	preferredNetworkEntry1.iAccess = SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM;
	preferredNetworkEntry1.iUserDefined = iPreferredNetworks->At(3).iUserDefined;
	preferredNetworkEntry1.iCountryCode = iPreferredNetworks->At(3).iCountryCode;
	preferredNetworkEntry1.iNetworkId = iPreferredNetworks->At(3).iNetworkId;
	
	CMobilePhoneStoredNetworkList* networklist =  CMobilePhoneStoredNetworkList::NewL();
	CleanupStack::PushL(networklist);
	
	//add at the end

	networklist->AddEntryL(preferredNetworkEntry4);	 //- FOR WLAN
	networklist->AddEntryL(preferredNetworkEntry3);
	networklist->AddEntryL(preferredNetworkEntry2);
	networklist->AddEntryL(preferredNetworkEntry1);
	
	iPhone.StorePreferredNetworksListL(reqStatus, networklist);
	User::WaitForRequest(reqStatus); 
    TEST(reqStatus.Int()==KErrPermissionDenied);
    CleanupStack::PopAndDestroy();  // networklist	 
	return TestStepResult();
	}

CSimWlanTest17::CSimWlanTest17() 
	{ 
	SetTestStepName(_L("Wlan Test17"));
	}
		
TVerdict CSimWlanTest17::doTestStepL()
/**
TEL-SIMTSY-MM-8680.
Test CRetrieveMobilePhonePreferredWlanSIDs::Start with different capability
*/
	{
	CTestRetrieveMobilePhonePreferredWlanSIDs* preferredWlanSidsList=CTestRetrieveMobilePhonePreferredWlanSIDs::NewLC(iPhone);
	preferredWlanSidsList->Start();
	CActiveScheduler::Start();
	
    delete preferredWlanSidsList; 
	return TestStepResult();
	}

CSimWlanTest18::CSimWlanTest18() 
	{ 
	SetTestStepName(_L("Wlan Test18"));
	}
		
TVerdict CSimWlanTest18::doTestStepL()
/**
TEL-SIMTSY-MM-8683.
Test CRetrieveMobilePhonePreferredNetworks::Start with different capability.
*/
	{
	CTestGetPreferredNetworks* preferredNetworks=CTestGetPreferredNetworks::NewLC(iPhone);
    preferredNetworks->Start();
    CActiveScheduler::Start();
    delete preferredNetworks->iTestPhone->iNetworkList;
	delete preferredNetworks->iTestPhone->iWlanList;
    delete preferredNetworks;
	return TestStepResult();
	}
CSimWlanTest19::CSimWlanTest19() 
	{ 
	SetTestStepName(_L("Wlan Test19"));
	}
		
TVerdict CSimWlanTest19::doTestStepL()
/**
TEL-SIMTSY-MM-8673.
Client is unable to store the list back on the UICC because the submitted list is larger than the expected.
*/
	{ 
	TRequestStatus reqStatus,aReqStatus;
	INFO_PRINTF1(_L("Test - RMobilePhone::NotifyPreferredWlanSIDListChange async"));    
    iPhone.NotifyPreferredNetworksListChange(aReqStatus);
    
	CTestGetPreferredNetworks* preferredNetworks=CTestGetPreferredNetworks::NewLC(iPhone);
    preferredNetworks->Start();
    CActiveScheduler::Start();
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry1;
	
	preferredNetworkEntry1.iAccess = SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM;
	preferredNetworkEntry1.iUserDefined = iPreferredNetworks->At(3).iUserDefined;
	preferredNetworkEntry1.iCountryCode = iPreferredNetworks->At(3).iCountryCode;
	preferredNetworkEntry1.iNetworkId = iPreferredNetworks->At(3).iNetworkId;	
	
	preferredNetworks->iTestPhone->iNetworkList->AddEntryL(preferredNetworkEntry1);

	iPhone.StorePreferredNetworksListL(reqStatus,preferredNetworks->iTestPhone->iNetworkList);
	User::WaitForRequest(reqStatus); 
    User::WaitForRequest(aReqStatus);
    TEST(reqStatus.Int() == KErrNoMemory);
    TEST(aReqStatus.Int() == KErrNone);    

	delete preferredNetworks->iTestPhone->iNetworkList;
	delete preferredNetworks->iTestPhone->iWlanList;
    delete preferredNetworks;
	return TestStepResult();
	}

CSimWlanTest20::CSimWlanTest20() 
	{ 
	SetTestStepName(_L("Wlan Test20"));
	}
		
TVerdict CSimWlanTest20::doTestStepL()
/**
TEL-SIMTSY-MM-8668.
Client confirms on the support of WLAN functionality by the USIM.
*/
	{ 
	//Verify the support of WLAN in USIM Service Table
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneServiceTableV8 asyncSstV8File;
	RMobilePhone::TMobilePhoneServiceTableV8Pckg asyncSstV8FilePckg(asyncSstV8File);
	RMobilePhone::TMobilePhoneServiceTable serviceTableType=RMobilePhone::EUSIMServiceTable;
	
	iPhone.GetServiceTable(reqStatus, serviceTableType, asyncSstV8FilePckg);
	
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	if(reqStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for Wlan passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for Wlan failed"));
		}

	TEST((asyncSstV8File.iServices65To72) == RMobilePhone::KUstWLANReAuthenId);	
	return TestStepResult();
	}
	
CSimWlanTest21::CSimWlanTest21() 
	{ 
	SetTestStepName(_L("Wlan Test21"));
	}
		
TVerdict CSimWlanTest21::doTestStepL()
/**
TEL-SIMTSY-MM-8672.
Client edits the preferred networks list and stores back on UICC successfully.
*/
	{
	TRequestStatus reqStatus,aReqStatus;
	INFO_PRINTF1(_L("Test - RMobilePhone::NotifyPreferredWlanSIDListChange async"));    
    iPhone.NotifyPreferredNetworksListChange(aReqStatus);
    
	CTestGetPreferredNetworks* preferredNetworks=CTestGetPreferredNetworks::NewLC(iPhone);
    preferredNetworks->Start();
    CActiveScheduler::Start();
    
	RMobilePhone::TMobilePreferredNetworkEntryV3 preferredNetworkEntry1;
	
	preferredNetworkEntry1.iAccess = SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM;
	preferredNetworkEntry1.iUserDefined = iPreferredNetworks->At(3).iUserDefined;
	preferredNetworkEntry1.iCountryCode = iPreferredNetworks->At(3).iCountryCode;
	preferredNetworkEntry1.iNetworkId = iPreferredNetworks->At(3).iNetworkId;
	preferredNetworks->iTestPhone->iNetworkList->ChangeEntryL(0,preferredNetworkEntry1);
	preferredNetworks->iTestPhone->iNetworkList->DeleteEntryL(1);		
	preferredNetworks->iTestPhone->iNetworkList->AddEntryL(preferredNetworkEntry1);
	iPhone.StorePreferredNetworksListL(reqStatus,preferredNetworks->iTestPhone->iNetworkList);
	User::WaitForRequest(reqStatus); 
    User::WaitForRequest(aReqStatus);
    TEST(reqStatus.Int() == KErrNone);
    TEST(aReqStatus.Int() == KErrNone);    

	delete preferredNetworks->iTestPhone->iNetworkList;
	delete preferredNetworks->iTestPhone->iWlanList;
    delete preferredNetworks;
	return TestStepResult();
	}
	
CSimWlanTest22::CSimWlanTest22() 
	{ 
	SetTestStepName(_L("Wlan Test22"));
	}
		
TVerdict CSimWlanTest22::doTestStepL()
/**
TEL-SIMTSY-MM-8666.
Client retrieves WLAN parameters from USIM after update done by another client.
*/
	{ 
	INFO_PRINTF1(_L("Two Client Scenario..."));
	
	TRequestStatus reqSts1,reqSts2,reqSts3,stat0,reqStatus,aReqStatus;
	
	RMobilePhone::TMobilePhoneSecurityCode whichCode = RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8);
	
	RMobilePhone::TMobilePassword code = _L("1111");
	RMobilePhone::TMobilePassword unblockCode = _L("1111");
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	
	usimWlanDataV8.iFieldsUsed = iWlanData->At(1).iFieldsUsed;
	iPhone.NotifyWlanDataChange(reqSts1,usimWlanDataV8Pckgd);
	iPhone.NotifySecurityEvent(reqSts2,securityEvent);
  		
	/* Create another thread and perform the authentication functionality. */
	RThread thread;
	INFO_PRINTF1(_L("A different client performing WLAN authentication"));
	TInt KStackSize=0x8000;
	TInt KHeapSize=0x8000;
	TInt KMaxHeapSize=0x80000;

	TInt res=thread.Create(_L("DifferentClient"), OtherCltWlan,KStackSize,KHeapSize,KMaxHeapSize,NULL, EOwnerThread);
	
	TEST(res == KErrNone);
	
	thread.Logon(reqSts3);	
	TEST(reqSts3.Int()==KRequestPending);
	
	thread.Resume();

	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int()==KErrNone);
	
	
	if (thread.ExitType() != EExitKill && thread.ExitReason() != 0)
		{
		User::Leave(thread.ExitReason());
		}
	thread.Close();
	
	User::WaitForRequest(reqSts1);
	TEST(reqSts1.Int() == KErrNone);
  	TEST(usimWlanDataV8.iFieldsUsed == iWlanData->At(1).iFieldsUsed);
  	TEST(usimWlanDataV8.iPseudonym == iWlanData->At(1).iPseudonym);
  	TEST(usimWlanDataV8.iReauthenticationId == iWlanData->At(1).iReauthenticationId);
  	TEST(usimWlanDataV8.iMasterKey == iWlanData->At(1).iMasterKey);
  	TEST(usimWlanDataV8.iCounter == iWlanData->At(1).iCounter);
  	iPhone.GetWlanData(reqStatus,usimWlanDataV8Pckgd);
	User::WaitForRequest(reqSts2);
	TEST(reqSts2.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinRequired);
	iPhone.NotifySecurityEvent(stat0,securityEvent);
	iPhone.VerifySecurityCode(aReqStatus,whichCode,code,unblockCode);
	User::WaitForRequest(aReqStatus);
	User::WaitForRequest(stat0);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(stat0.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinVerified);
	
  	User::WaitForRequest(reqStatus);
  	TEST(reqStatus.Int() == KErrNone);
  	TEST(usimWlanDataV8.iFieldsUsed == iWlanData->At(1).iFieldsUsed);
  	TEST(usimWlanDataV8.iPseudonym == iWlanData->At(1).iPseudonym);
  	TEST(usimWlanDataV8.iReauthenticationId == iWlanData->At(1).iReauthenticationId);
  	TEST(usimWlanDataV8.iMasterKey == iWlanData->At(1).iMasterKey);
  	TEST(usimWlanDataV8.iCounter == iWlanData->At(1).iCounter);
	return TestStepResult();	
	}

TInt OtherCltWlan(void* aThreadData)
	{
	
	(void)aThreadData; // as we are not using this variable.
	
	TInt ret =KErrNone;
	TRAP(ret,CSimUsimR6TestWlanThread* myInstance = CSimUsimR6TestWlanThread::getInstanceL());
	
	return ret;
	
	}

CSimUsimR6TestWlanThread* CSimUsimR6TestWlanThread::instance=NULL;

CSimUsimR6TestWlanThread* CSimUsimR6TestWlanThread::getInstanceL()
	{
	if(instance == NULL)
		{
		instance = new(ELeave) CSimUsimR6TestWlanThread();
		}
	return instance;
	}

CSimUsimR6TestWlanThread::CSimUsimR6TestWlanThread()
	{
	// Do Nothing Constructor
	}
	
CSimUsimR6TestWlanThread::CSimUsimR6TestWlanThread(const CSimUsimR6TestWlanThread&)
	{
	// Do Nothing copy Constructor
	}

CSimUsimR6TestWlanThread& CSimUsimR6TestWlanThread::operator=(const CSimUsimR6TestWlanThread&)
	{
	// Do Nothing Operator overloaded function
	CSimUsimR6TestWlanThread* test = CSimUsimR6TestWlanThread::getInstanceL();
	return (*test);
	}

TVerdict CSimUsimR6TestWlanThread::doTestStepL()
/**
TEL-SIMTSY-MM-8663.
Client stores the Wlan parameters on USIM successfully.
*/
	{
	// local variables used throughout the tests
	TRequestStatus reqStatus,aReqStatus,stat0,reqStatus1;
	RMobilePhone::TMobilePhoneSecurityCode whichCode = RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TUsimWlanDataV8 usimWlanDataV8;
  	RMobilePhone::TUsimWlanDataV8Pckg usimWlanDataV8Pckgd(usimWlanDataV8);
	
	RMobilePhone::TMobilePassword code = _L("1111");
	RMobilePhone::TMobilePassword unblockCode = _L("1111");
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	iPhone.NotifySecurityEvent(aReqStatus,securityEvent);
	usimWlanDataV8.iFieldsUsed = iWlanData->At(0).iFieldsUsed;
	iPhone.NotifyWlanDataChange(reqStatus1,usimWlanDataV8Pckgd);
	
	TestSmartCardAuthenticate();

  	usimWlanDataV8.iPseudonym = iWlanData->At(0).iPseudonym ;
  	usimWlanDataV8.iReauthenticationId = iWlanData->At(0).iReauthenticationId;	 
  	iPhone.SetWlanData(reqStatus,usimWlanDataV8Pckgd);
  	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinRequired);
	
	iPhone.NotifySecurityEvent(stat0,securityEvent);
	iPhone.VerifySecurityCode(aReqStatus,whichCode,code,unblockCode);
	User::WaitForRequest(aReqStatus);
	User::WaitForRequest(stat0);
	TEST(aReqStatus.Int() == KErrNone);
	TEST(securityEvent == RMobilePhone::EUSIMAppPinVerified);
  	User::WaitForRequest(reqStatus);
  	User::WaitForRequest(reqStatus1);
  	TEST(reqStatus.Int() == KErrNone);
  	TEST(reqStatus1.Int() == KErrNone);
  	TEST(usimWlanDataV8.iFieldsUsed == iWlanData->At(0).iFieldsUsed);
  	TEST(usimWlanDataV8.iPseudonym == iWlanData->At(0).iPseudonym);
  	TEST(usimWlanDataV8.iReauthenticationId == iWlanData->At(0).iReauthenticationId);
	return TestStepResult();	
	}
		
CTestRetrieveMobilePhonePreferredWlanSIDs* CTestRetrieveMobilePhonePreferredWlanSIDs::NewLC(RMobilePhone& aPhone)
	{
	CTestRetrieveMobilePhonePreferredWlanSIDs* r=new(ELeave) CTestRetrieveMobilePhonePreferredWlanSIDs(aPhone);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
  
CTestRetrieveMobilePhonePreferredWlanSIDs::CTestRetrieveMobilePhonePreferredWlanSIDs(RMobilePhone& aPhone)
: CActive(EPriorityNormal), iPhone(aPhone)
{}
 
void CTestRetrieveMobilePhonePreferredWlanSIDs::ConstructL()
	{
	CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhonePreferredWlanSIDs::NewL(iPhone);
	iTestWLAN = new (ELeave) CSimWlanTest6();
	}
     
CTestRetrieveMobilePhonePreferredWlanSIDs::~CTestRetrieveMobilePhonePreferredWlanSIDs()
	{
	delete iRetrieve;
	delete iTestWLAN;
	}

void CTestRetrieveMobilePhonePreferredWlanSIDs::Start()
	{
	iRetrieve->Start(iStatus); 
    SetActive();
	}       

void CTestRetrieveMobilePhonePreferredWlanSIDs::RunL()
	{
	TInt ret=iStatus.Int();
	if(ret==KErrPermissionDenied)
		{
		iTestWLAN->INFO_PRINTF1(_L("Permission Access Denied"));
		CActiveScheduler::Stop();
		return;
		}
	User::LeaveIfError(ret);
	ret=CheckWLANResults();
	User::LeaveIfError(ret);
	CActiveScheduler::Stop();
	}
  
void CTestRetrieveMobilePhonePreferredWlanSIDs::DoCancel()
	{
	iRetrieve->Cancel();
	}       
  
TInt CTestRetrieveMobilePhonePreferredWlanSIDs::CheckWLANResults()
	{
	TInt ret=KErrNone;

	iTestWLAN->iWlanList =NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, iTestWLAN->iWlanList=iRetrieve->RetrieveListL(););

	//Check that a list is returned back to the test
	iTestWLAN->TEST(iTestWLAN->iWlanList!=NULL);

	if (iTestWLAN->iWlanList)
		{
		//Check that the number of entries in the list is as expected
		iTestWLAN->TEST(iTestWLAN->iWlanList->Enumerate()==SIMTSY_PHONE_WLAN_SIDLIST_COUNT);
		// Get each entry
		RMobilePhone::TWlanSIDV8 wlanSIDV8;
		for (TInt i=0; i<SIMTSY_PHONE_WLAN_SIDLIST_COUNT; ++i)
			{
			TRAP(ret,wlanSIDV8=iTestWLAN->iWlanList->GetEntryL(i));
			if (ret != KErrNone)
				{
				break;
				}
			switch(i)
				{
			case 0:
				{
				iTestWLAN->TEST(wlanSIDV8.iWSID == SIMTSY_PHONE_WLAN_WSID_ONE);
				iTestWLAN->TEST(wlanSIDV8.iUserDefined == SIMTSY_PHONE_WLAN_USERDEFD_ONE);
				break;
				}
			case 1:
				{
				iTestWLAN->TEST(wlanSIDV8.iWSID == SIMTSY_PHONE_WLAN_WSID_TWO);
				iTestWLAN->TEST(wlanSIDV8.iUserDefined == SIMTSY_PHONE_WLAN_USERDEFD_TWO);
				break;
				}
			case 2:
				{
				iTestWLAN->TEST(wlanSIDV8.iWSID == SIMTSY_PHONE_WLAN_WSID_THREE);
				iTestWLAN->TEST(wlanSIDV8.iUserDefined == SIMTSY_PHONE_WLAN_USERDEFD_THREE);				break;
				}
			default:
				break;
				}
			}
		}
	return ret;
	}
	
/**************************************************************/
//
// Testing asynchronous retrieve of preferred stored network list
//
/**************************************************************/


CTestGetPreferredNetworks* CTestGetPreferredNetworks::NewLC(RMobilePhone& aPhone)
	{
    CTestGetPreferredNetworks* r=new(ELeave) CTestGetPreferredNetworks(aPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    CleanupStack::Pop();
    return r;
    }
      
CTestGetPreferredNetworks::CTestGetPreferredNetworks(RMobilePhone& aPhone)
    : CActive(EPriorityNormal), iPhone(aPhone)
    {}
      

void CTestGetPreferredNetworks::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhonePreferredNetworks::NewL(iPhone);
	iTestPhone = new (ELeave) CSimWlanTest6();
    }
         
CTestGetPreferredNetworks::~CTestGetPreferredNetworks()
    {
    delete iRetrieve;
    delete iTestPhone;
	}
      
void CTestGetPreferredNetworks::Start()
    {
    iRetrieve->Start(iStatus);
    SetActive();
    }       
      
void CTestGetPreferredNetworks::RunL()
    {
	TInt ret=iStatus.Int();
	if(ret==KErrPermissionDenied)
		{
		iTestPhone->INFO_PRINTF1(_L("Permission Access Denied"));
		CActiveScheduler::Stop();
		return;
		}
    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}
      

void CTestGetPreferredNetworks::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      
TInt CTestGetPreferredNetworks::CheckResults()
    {
	TInt ret=KErrNone;

	iTestPhone->iNetworkList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, iTestPhone->iNetworkList=iRetrieve->RetrieveListL(););

	if (iTestPhone->iNetworkList)
		{
		iTestPhone->TEST(iTestPhone->iNetworkList->Enumerate()==SIMTSY_PHONE_NTWK_LIST_ENTRIES);

		// Get each stored network entry
		RMobilePhone::TMobilePreferredNetworkEntryV3 entry;

		for (TInt i=0; i<SIMTSY_PHONE_NTWK_LIST_ENTRIES; ++i)
			{
			TRAP(ret,entry=iTestPhone->iNetworkList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestPhone->TEST(entry.iAccess == SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM);
					iTestPhone->TEST(entry.iUserDefined == SIMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE);
					iTestPhone->TEST(entry.iCountryCode == SIMTSY_PHONE_NTWK_LIST_COUNTRY_ID1);
					iTestPhone->TEST(entry.iNetworkId == SIMTSY_PHONE_NTWK_LIST_NETWORK_ID1);
					}
					break;
				case 1:
					{
					iTestPhone->TEST(entry.iAccess == SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSMC);
					iTestPhone->TEST(entry.iUserDefined == SIMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE);
					iTestPhone->TEST(entry.iCountryCode == SIMTSY_PHONE_NTWK_LIST_COUNTRY_ID2);
					iTestPhone->TEST(entry.iNetworkId == SIMTSY_PHONE_NTWK_LIST_NETWORK_ID2);
					}
					break;
				case 2:
				default:
					{
					iTestPhone->TEST(entry.iAccess == SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_UTRAN);
					iTestPhone->TEST(entry.iUserDefined == SIMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE);
					iTestPhone->TEST(entry.iCountryCode == SIMTSY_PHONE_NTWK_LIST_COUNTRY_ID2);
					iTestPhone->TEST(entry.iNetworkId == SIMTSY_PHONE_NTWK_LIST_NETWORK_ID1);
					}
					break;
				case 3:
					{
					iTestPhone->TEST(entry.iAccess == SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_WLAN);// - RAFIK UNCOMMENT THIS
					iTestPhone->TEST(entry.iUserDefined == SIMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE);
					iTestPhone->TEST(entry.iCountryCode == SIMTSY_PHONE_NTWK_LIST_COUNTRY_ID2);
					iTestPhone->TEST(entry.iNetworkId == SIMTSY_PHONE_NTWK_LIST_NETWORK_ID1);
					break;
					}
				}
			}
		}
   return ret;
    }
  

CSim3GSecurityContextTest1::CSim3GSecurityContextTest1() 
	{ 
	SetTestStepName(_L("3GSecurityContextTest1"));
	}
		
TVerdict CSim3GSecurityContextTest1::doTestStepL()
/**
TEL-SIMTSY-MM-8658.
Client performs EAP-AKA USIM authentication successfully.
*/
	{ 
	TestSmartCardAuthenticate();
	return TestStepResult();
	}
	
CSim3GSecurityContextTest2::CSim3GSecurityContextTest2() 
	{ 
	SetTestStepName(_L("CSim3GSecurityContextTest2"));
	}
		
TVerdict CSim3GSecurityContextTest2::doTestStepL()
/**
TEL-SIMTSY-MM-8659.
Client is unable to perform EAP-AKA USIM authentication due to lack of support in USIM service table.
*/
	{ 
	TRequestStatus reqStatus;
   	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;
	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);
	authenticateData.iAUTN = KConfigScWlanAuthAUTN();
	authenticateData.iRAND = KConfigScWlanAuthRAND();
	authenticateData.iCK = KConfigScWlanNotSupported();
	
	RMobilePhone::TAID appAID = KWlanAid();
	iPhone.SmartCardAuthenticate(reqStatus,appAID,authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNotSupported);
	return TestStepResult();
	}
	
CSim3GSecurityContextTest3::CSim3GSecurityContextTest3() 
	{ 
	SetTestStepName(_L("CSim3GSecurityContextTest3"));
	}
		
TVerdict CSim3GSecurityContextTest3::doTestStepL()
/**
TEL-SIMTSY-MM-8660.
Client is unable to perform EAP-AKA USIM authentication due to inactive USIM application.
*/
	{ 
	TRequestStatus reqStatus;
   	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;
	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);
	authenticateData.iAUTN = KConfigScWlanAuthAUTN();
	authenticateData.iRAND = KConfigScWlanAuthRAND();
	authenticateData.iCK = KConfigScWlan();
	
	RMobilePhone::TAID appAID;
	iPhone.SmartCardAuthenticate(reqStatus,appAID,authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrMMEtelScApplicationNotActive);
	return TestStepResult();
	}

CSim3GSecurityContextTest4::CSim3GSecurityContextTest4() 
	{ 
	SetTestStepName(_L("CSim3GSecurityContextTest4"));
	}
		
TVerdict CSim3GSecurityContextTest4::doTestStepL()
/**
TEL-SIMTSY-MM-8661.
Client is unable to perform EAP-AKA USIM authentication due to MAC verification failure.
*/
	{ 
	TRequestStatus reqStatus;
   	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;
	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);
	authenticateData.iCK = KConfigScWlan();
	
	RMobilePhone::TAID appAID = KWlanAid();
	iPhone.SmartCardAuthenticate(reqStatus,appAID,authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrMMEtelMacVerificationFailed);
	return TestStepResult();
	}

CSim3GSecurityContextTest5::CSim3GSecurityContextTest5() 
	{ 
	SetTestStepName(_L("CSim3GSecurityContextTest5"));
	}
		
TVerdict CSim3GSecurityContextTest5::doTestStepL()
/**
TEL-SIMTSY-MM-8662.
Client is unable to perform EAP-AKA USIM authentication due to synchronisation failure. 
*/
	{ 
	TRequestStatus reqStatus;
   	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;
	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);
	authenticateData.iAUTN = KConfigScWlanAuthAUTN();
	authenticateData.iCK = KConfigScWlan();
	
	RMobilePhone::TAID appAID = KWlanAid();
	iPhone.SmartCardAuthenticate(reqStatus,appAID,authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrMMEtelSqnVerificationFailed);
	return TestStepResult();
	}
		
/****************************************************************************\
 * 																			*
 * 			Generic Bootstrapping Architecture  Testcases	 				*	
 * 																			*	
 ****************************************************************************/

CSimUsimR6GBATest1::CSimUsimR6GBATest1() 
	{ 
	SetTestStepName(_L("UsimR6GBATest1"));
	}

TVerdict CSimUsimR6GBATest1::doTestStepL()
/**
TEL-SIMTSY-MM-8601.
Client confirms on the support of GBA functionality by the USIM.
*/
	{
	INFO_PRINTF1(_L("BeginUsimR6GBATest"));

	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	TRequestStatus reqStatus=KErrNone;

	//Verify the support of GBA in USIM Service Table

	RMobilePhone::TMobilePhoneServiceTableV8 asyncSstV8File;
	RMobilePhone::TMobilePhoneServiceTableV8Pckg asyncSstV8FilePckg(asyncSstV8File);
	RMobilePhone::TMobilePhoneServiceTable serviceTableType=RMobilePhone::EUSIMServiceTable;
	
	iPhone.GetServiceTable(reqStatus, serviceTableType, asyncSstV8FilePckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	if(reqStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for GBA passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for GBA failed"));
		}
	TEST((asyncSstV8File.iServices65To72) == RMobilePhone::KUstGBA);
	iPhone.Close();
	return TestStepResult();
 	}

CSimUsimR6GBATest2::CSimUsimR6GBATest2() 
	{	 
	SetTestStepName(_L("UsimR6GBATest2"));
	}

TVerdict CSimUsimR6GBATest2::doTestStepL()
/**
TEL-SIMTSY-MM-8602.
Client performs successfully GBA authentication in Bootstrap mode
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts1,reqSts2,reqSts3,reqStatus;
	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);

	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
	
	_LIT8(Krand,"RAND_IN"); 
	_LIT8(Kautn,"AUTN_IN"); 
	_LIT8(Kaid,"AE879");
	tGbaBootstrapAuthDataV8.iRAND = Krand;
	tGbaBootstrapAuthDataV8.iAUTN = Kautn;

	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	iPhone.NotifyAuthenticateDataChange(reqStatus,tAuthInfoV8Pckg);

	RMobilePhone::TAID appAID = Kaid();
	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));
	iPhone.SmartCardAuthenticate(reqSts2, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(reqSts2);
	TEST(reqSts2.Int() == KErrNone);
	
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	INFO_PRINTF1(_L("RMobilePhone::NotifyAuthenticateDataChange Completed."));
	
	tAuthInfoV8 = tAuthInfoV8Pckg();
	
	TEST(tAuthInfoV8.iAID == Kaid());
	TEST(tAuthInfoV8.iData == RMobilePhone::EGbaBootstrapParams );
	/* 	Client sends the RES to the network in the HTTP Digest AKA response. 
	 * 	The network (i.e. the BSF) authenticates the phone by verifying the 
	 * 	Digest AKA response. The network generates the Ks key, the 
	 *  transaction identifier (B-TID) and the Ks key Lifetime. 
	 *  The network (the BSF) then sends the B-TID value and the key 
	 * lifetime to the phone. 
	 */
	RMobilePhone::TBsfDataV8 tBsfDataV8;
	RMobilePhone::TBsfDataV8Pckg tBsfDataV8Pckg(tBsfDataV8);

	_LIT8(KBtid,"B2345");
	_LIT8(KKeyLifeTime,"D3789");
	tBsfDataV8.iBtid = KBtid;
	tBsfDataV8.iKeyLifetime = KKeyLifeTime;

	iPhone.SetGbaBootstrapParams(reqSts3,appAID,tBsfDataV8Pckg);

	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);

	INFO_PRINTF1(_L("NotifySecurityEvent completed with EUSIMAppPinRequired"));

	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "1234");
	pass.Copy(Kpswrd);
   
	INFO_PRINTF1(_L("Verifying the PIN1"));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2==KErrNone); 
	TEST(secEvent==RMobilePhone::EUSIMAppPinVerified);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed with EUSIMAppPinVerified"));

	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrNone);	
	
	if(reqSts3.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("SetGBABootParams Completed Successfully"));
		}
	else
		{
		INFO_PRINTF1(_L("SetGBABootParams Not Completed Successfully"));
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest3::CSimUsimR6GBATest3() 
	{ 
	SetTestStepName(_L("UsimR6GBATest3"));
	}

TVerdict CSimUsimR6GBATest3::doTestStepL()
/**
TEL-SIMTSY-MM-8603.
Client is unable to perform GBA authentication in Bootstrap mode as USIM application is not active
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(39);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts;

	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
	
	_LIT8(Krand,"RAND_IN"); 
	_LIT8(Kautn,"AUTN_IN"); 
	_LIT8(Kaid,"AE879");
	tGbaBootstrapAuthDataV8.iRAND = Krand;
	tGbaBootstrapAuthDataV8.iAUTN = Kautn;
	
	RMobilePhone::TAID appAID = Kaid();

	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));

	iPhone.SmartCardAuthenticate(reqSts, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrMMEtelScApplicationNotActive );
	
	if(reqSts.Int() == KErrMMEtelScApplicationNotActive)
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate completed with Application Not Active"));		
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest4::CSimUsimR6GBATest4() 
	{ 
	SetTestStepName(_L("UsimR6GBATest4"));
	}

TVerdict CSimUsimR6GBATest4::doTestStepL()
/**
TEL-SIMTSY-MM-8604.
Client is unable to perform GBA authentication in Bootstrap mode as 
USIM application is busy with another client’s request for GBA.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(40);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts;

	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
	
	_LIT8(Krand,"RAND_IN"); 
	_LIT8(Kautn,"AUTN_IN"); 
	_LIT8(Kaid,"AE879");
	tGbaBootstrapAuthDataV8.iRAND = Krand;
	tGbaBootstrapAuthDataV8.iAUTN = Kautn;
	
	RMobilePhone::TAID appAID = Kaid();

	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));

	iPhone.SmartCardAuthenticate(reqSts, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrInUse );
	
	INFO_PRINTF1(_L("SmartCardAuthenticate completed with Application busy."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest5::CSimUsimR6GBATest5() 
	{ 
	SetTestStepName(_L("UsimR6GBATest5"));
	}

TVerdict CSimUsimR6GBATest5::doTestStepL()
/**
TEL-SIMTSY-MM-8605.
Client is unable to update EFGBABP due to PIN verification failure.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts1,reqSts2,reqSts3;

	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);

	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
	
	_LIT8(Krand,"RAND_IN"); 
	_LIT8(Kautn,"AUTN_IN"); 
	_LIT8(Kaid,"AE879");
	tGbaBootstrapAuthDataV8.iRAND = Krand;
	tGbaBootstrapAuthDataV8.iAUTN = Kautn;

	RMobilePhone::TAID appAID = Kaid();

	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));

	iPhone.SmartCardAuthenticate(reqSts2, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(reqSts2);
	TEST(reqSts2.Int() == KErrNone);
 
	if(reqSts2.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate completed successfully."));
		}
	else
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate not completed successfully."));
		}
	
	/* 	Client sends the RES to the network in the HTTP Digest AKA response. 
	 * 	The network (i.e. the BSF) authenticates the phone by verifying the 
	 * 	Digest AKA response. The network generates the Ks key, the 
	 *  transaction identifier (B-TID) and the Ks key Lifetime. 
	 *  The network (the BSF) then sends the B-TID value and the key 
	 * lifetime to the phone. 
	 */
	RMobilePhone::TBsfDataV8 tBsfDataV8;
	RMobilePhone::TBsfDataV8Pckg tBsfDataV8Pckg(tBsfDataV8);

	_LIT8(KBtid,"B4567"); // not the value in the config file to return KErrAccessDenied
	_LIT8(KKeyLifeTime,"D3789");
	tBsfDataV8.iBtid = KBtid;
	tBsfDataV8.iKeyLifetime = KKeyLifeTime;

	iPhone.SetGbaBootstrapParams(reqSts3,appAID,tBsfDataV8Pckg);

	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);

	INFO_PRINTF1(_L("NotifySecurityEvent completed with EUSIMAppPinRequired"));

	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "4567");   // Invalid password
	pass.Copy(Kpswrd);
   
	INFO_PRINTF1(_L("Verifying the PIN1"));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2==KErrAccessDenied); 
	TEST(secEvent==RMobilePhone::EUSIMAppPinRequired);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed with USIM PIN Verification failed"));

	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrAccessDenied);	
	if(reqSts3.Int() == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("SetGbaBootstrapParams completed with KErrAccessDenied"));
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest6::CSimUsimR6GBATest6() 
	{ 
	SetTestStepName(_L("UsimR6GBATest6"));
	}

TVerdict CSimUsimR6GBATest6::doTestStepL()
/**
TEL-SIMTSY-MM-8606.
Client performs successfully GBA authentication 
in NAF derivation mode after deriving Bootstrap key (Ks).
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(49);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts1,reqSts2;
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	
	_LIT8(Kaid,"AE879");
	RMobilePhone::TAID appAID = Kaid();

 	/*Precondition:
 	 *  Client performed the GBA authentication in bootstrap mode successsfully 
 	 */
 	
	// Generation of Ks_ext_NAF using NAF derivation method
	
	_LIT8(KNafid,"C6453");
	_LIT8(KImpi,"E7654");
	
	iPhone.NotifyAuthenticateDataChange(reqSts1,tAuthInfoV8Pckg);
	
	RMobilePhone::TGbaNafAuthDataV8 tGbaNafAuthDataV8;
	RMobilePhone::TGbaNafAuthDataV8Pckg tGbaNafAuthDataV8Pckg(tGbaNafAuthDataV8);
	
	tGbaNafAuthDataV8.iNafId = KNafid;
	tGbaNafAuthDataV8.iImpi = KImpi;
		
	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));

	iPhone.SmartCardAuthenticate(reqSts2, appAID,tGbaNafAuthDataV8Pckg);
	User::WaitForRequest(reqSts2);
	TEST(reqSts2.Int() == KErrNone);

	if(reqSts2.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate completed successfully."));
		}
	else
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate not completed successfully."));
		}
	
	tGbaNafAuthDataV8 = tGbaNafAuthDataV8Pckg();
	_LIT8(KksExtNaf,"A6543");
	TEST(tGbaNafAuthDataV8.iKsExtNaf.Compare(KksExtNaf) == 0);
 
	User::WaitForRequest(reqSts1);
	TEST(reqSts1.Int() == KErrNone);
	INFO_PRINTF1(_L("RMobilePhone::NotifyAuthenticateDataChange Completed."));
	
	tAuthInfoV8 = tAuthInfoV8Pckg();
	TEST(tAuthInfoV8.iAID == Kaid());
	TEST(tAuthInfoV8.iData == RMobilePhone::EGbaNafIdList );

	iPhone.Close();	
	return TestStepResult();
	}

CSimUsimR6GBATest7::CSimUsimR6GBATest7() 
	{ 
	SetTestStepName(_L("UsimR6GBATest7"));
	}

TVerdict CSimUsimR6GBATest7::doTestStepL()
/**
TEL-SIMTSY-MM-8607.
Client performs successfully GBA authentication in 
NAF derivation mode using the existing Bootstrap key (Ks).
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts1,reqSts2,reqSts3;
	
	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);

	RMobilePhone::TGbaBootstrapParamsV8 tGbaBootstrapParamsV8;
	RMobilePhone::TGbaBootstrapParamsV8Pckg tGbaBootstrapParamsV8Pckg(tGbaBootstrapParamsV8);
	
	_LIT8(Kaid,"AE879");
	
	tAuthInfoV8.iData = RMobilePhone::EGbaBootstrapParams;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts3,tAuthInfoV8Pckg,tGbaBootstrapParamsV8Pckg);
	
	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);
	
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "1234");
	pass.Copy(Kpswrd);
	   
	INFO_PRINTF1(_L("Verifying the SIM APP PIN..."));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2==KErrNone); 
//	INFO_PRINTF2(_L("Value of secEvent = %d"),secEvent);
	TEST(secEvent==RMobilePhone::EUSIMAppPinVerified);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed with EUSIMAppPinVerified"));
	
	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrNone);
	if(reqSts3.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("GetAuthenticationParams completed successfully."));
		}
	else
		{
		INFO_PRINTF1(_L("GetAuthenticationParams not completed successfully."));
		}
	
	tGbaBootstrapParamsV8 = tGbaBootstrapParamsV8Pckg();
	TEST(tGbaBootstrapParamsV8.iRand == _L8("RAND_IN"));
	TEST(tGbaBootstrapParamsV8.iBtid == _L8("B2345"));
	TEST(tGbaBootstrapParamsV8.iKeyLifetime == _L8("D3789"));
		
	RMobilePhone::TGbaNafAuthDataV8 tGbaNafAuthDataV8;
	RMobilePhone::TGbaNafAuthDataV8Pckg tGbaNafAuthDataV8Pckg(tGbaNafAuthDataV8);
	
	_LIT8(KNafId,"C6453");
	_LIT8(KImpi,"E7654");
	
	tGbaNafAuthDataV8.iNafId = KNafId;
	tGbaNafAuthDataV8.iImpi = KImpi;
	
	RMobilePhone::TAID appAID = Kaid();
	
	iPhone.SmartCardAuthenticate(reqSts3,appAID,tGbaNafAuthDataV8Pckg);
	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrNone);
	
	tGbaNafAuthDataV8 = tGbaNafAuthDataV8Pckg();
	_LIT8(KksExtNaf,"A6543");
	TEST(tGbaNafAuthDataV8.iKsExtNaf.Compare(KksExtNaf) == 0);
	
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest8::CSimUsimR6GBATest8() 
	{ 
	SetTestStepName(_L("UsimR6GBATest8"));
	}

TVerdict CSimUsimR6GBATest8::doTestStepL()
/**
TEL-SIMTSY-MM-8608.
Client is unable to perform GBA authentication in NAF derivation mode as Ks is not available.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(41);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts3;
	
	RMobilePhone::TGbaNafAuthDataV8 tGbaNafAuthDataV8;
	RMobilePhone::TGbaNafAuthDataV8Pckg tGbaNafAuthDataV8Pckg(tGbaNafAuthDataV8);
	
	_LIT8(KNafId,"C6453");
	_LIT8(KImpi,"E7654");
	_LIT8(Kaid,"AE879");
	
	tGbaNafAuthDataV8.iNafId = KNafId;
	tGbaNafAuthDataV8.iImpi = KImpi;
	
	RMobilePhone::TAID appAID = Kaid();
	
	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));
	
	iPhone.SmartCardAuthenticate(reqSts3,appAID,tGbaNafAuthDataV8Pckg);
	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrMMEtelAuthKeyNotFound);
	
	if(reqSts3.Int() == KErrMMEtelAuthKeyNotFound)
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate completed with Ks Not Found."));
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest9::CSimUsimR6GBATest9() 
	{	 
	SetTestStepName(_L("UsimR6GBATest9"));
	}

TVerdict CSimUsimR6GBATest9::doTestStepL()
/**
TEL-SIMTSY-MM-8609.
Client is unable to perform GBA authentication in 
NAF derivation mode as USIM application is not active
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(39);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts;
	
	RMobilePhone::TGbaNafAuthDataV8 tGbaNafAuthDataV8;
	RMobilePhone::TGbaNafAuthDataV8Pckg tGbaNafAuthDataV8Pckg(tGbaNafAuthDataV8);
	
	_LIT8(KNafId,"C6453");
	_LIT8(KImpi,"E7654");
	_LIT8(Kaid,"AE879");
	
	tGbaNafAuthDataV8.iNafId = KNafId;
	tGbaNafAuthDataV8.iImpi = KImpi;
	
	RMobilePhone::TAID appAID = Kaid();
	
	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));
	
	iPhone.SmartCardAuthenticate(reqSts,appAID,tGbaNafAuthDataV8Pckg);
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrMMEtelScApplicationNotActive);
		
	if(reqSts.Int() == KErrMMEtelScApplicationNotActive)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::SmartCardAuthenticate Completed with Application Not Active."));
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest10::CSimUsimR6GBATest10() 
	{ 
	SetTestStepName(_L("UsimR6GBATest10"));
	}

TVerdict CSimUsimR6GBATest10::doTestStepL()
/**
TEL-SIMTSY-MM-8610.
Client is unable to perform GBA authentication in 
NAF derivation mode as USIM application is busy with another client’s request for GBA.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(40);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts;
	
	RMobilePhone::TGbaNafAuthDataV8 tGbaNafAuthDataV8;
	RMobilePhone::TGbaNafAuthDataV8Pckg tGbaNafAuthDataV8Pckg(tGbaNafAuthDataV8);
	
	_LIT8(KNafId,"C6453");
	_LIT8(KImpi,"E7654");
	_LIT8(Kaid,"AE879");
	
	tGbaNafAuthDataV8.iNafId = KNafId;
	tGbaNafAuthDataV8.iImpi = KImpi;
	
	RMobilePhone::TAID appAID = Kaid();
	
	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));
	
	iPhone.SmartCardAuthenticate(reqSts,appAID,tGbaNafAuthDataV8Pckg);
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrInUse);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate Completed with KErrInUse."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest11::CSimUsimR6GBATest11() 
	{ 
	SetTestStepName(_L("UsimR6GBATest11"));
	}

TVerdict CSimUsimR6GBATest11::doTestStepL()
/**
TEL-SIMTSY-MM-8611.
Client, at its discretion, retrieves GBA Bootstrapping parameters from UICC successfully.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 

	INFO_PRINTF1(_L("Two Client Scenario..."));
	
	TRequestStatus reqSts1,reqSts2,reqSts3;
	
	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);	
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	_LIT8(Kaid,"AE879");
	
	RMobilePhone::TGbaBootstrapParamsV8 tGbaBootstrapParamsV8;
	RMobilePhone::TGbaBootstrapParamsV8Pckg tGbaBootstrapParamsV8Pckg(tGbaBootstrapParamsV8);
	
	tAuthInfoV8.iData = RMobilePhone::EGbaBootstrapParams;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts3,tAuthInfoV8Pckg,tGbaBootstrapParamsV8Pckg);
	
	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);
	
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "1234");
	pass.Copy(Kpswrd);
	   
	INFO_PRINTF1(_L("Verifying the SIM APP PIN..."));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2==KErrNone); 
//	INFO_PRINTF2(_L("Value of secEvent = %d"),secEvent);
	TEST(secEvent==RMobilePhone::EUSIMAppPinVerified);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed Successfully with EUSIMAppPinVerified."));
	
	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrNone);
	if(reqSts3.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("GetAuthenticationParams completed Successfully."));		
		}
	
	tGbaBootstrapParamsV8 = tGbaBootstrapParamsV8Pckg();
	
	_LIT8(KRand,"RAND_IN");
	_LIT8(KBtid,"B2345");
	_LIT8(KKeyLifeTime,"D3789");
	
	TEST(tGbaBootstrapParamsV8.iRand.Compare(KRand)==0);
	TEST(tGbaBootstrapParamsV8.iBtid.Compare(KBtid)==0);
	TEST(tGbaBootstrapParamsV8.iKeyLifetime.Compare(KKeyLifeTime)==0);
	
	iPhone.Close();	
	return TestStepResult();
	}

CSimUsimR6GBATest12::CSimUsimR6GBATest12() 
	{ 
	SetTestStepName(_L("UsimR6GBATest12"));
	}

TVerdict CSimUsimR6GBATest12::doTestStepL()
/**
TEL-SIMTSY-MM-8612.
Clients retrieve GBA Bootstrapping parameters successfully upon notification from UICC.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 

	INFO_PRINTF1(_L("Two Client Scenario..."));
	
	TRequestStatus reqSts1,reqSts2,reqSts3;
	
	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);	
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	_LIT8(Kaid,"AE879");
	
	iPhone.NotifyAuthenticateDataChange(reqSts2,tAuthInfoV8Pckg);
	
	/* Create another thread and perform the authentication functionality. */
	RThread thread;
	INFO_PRINTF1(_L("A different client performing GBA authentication"));
	TInt KStackSize=0x8000;
	TInt KHeapSize=0x8000;
	TInt KMaxHeapSize=0x80000;

	TInt res=thread.Create(_L("DifferentClient"), OtherCltGBA,KStackSize,KHeapSize,KMaxHeapSize,NULL, EOwnerThread);
	
	TEST(res == KErrNone);
	
	thread.Logon(reqSts3);	
	TEST(reqSts3.Int()==KRequestPending);
	
	thread.Resume();

	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int()==KErrNone);
	
	/* Client has updated the EFGBA with the new authentication parameters */
	
	User::WaitForRequest(reqSts2);  // NotifyAuthenticateData completes as the 
									// authentication is performed by another client
	TEST(reqSts2.Int()==KErrNone);
	if(reqSts2.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("NotifyAuthenticateDataChange completed Successfully in Client 1."));
		}
	else
		{
		INFO_PRINTF1(_L("NotifyAuthenticateDataChange Not completed Successfully in Client 1."));
		}
	
	tAuthInfoV8 = tAuthInfoV8Pckg();

	TEST(tAuthInfoV8.iAID == Kaid());
	TEST(tAuthInfoV8.iData == RMobilePhone::EGbaBootstrapParams);
	
	if (thread.ExitType() != EExitKill && thread.ExitReason() != 0)
		{
		User::Leave(thread.ExitReason());
		}
	thread.Close();
	
	RMobilePhone::TGbaBootstrapParamsV8 tGbaBootstrapParamsV8;
	RMobilePhone::TGbaBootstrapParamsV8Pckg tGbaBootstrapParamsV8Pckg(tGbaBootstrapParamsV8);
	
	tAuthInfoV8.iData = RMobilePhone::EGbaBootstrapParams;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts3,tAuthInfoV8Pckg,tGbaBootstrapParamsV8Pckg);
	
	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);
	
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "1234");
	pass.Copy(Kpswrd);
	   
	INFO_PRINTF1(_L("Verifying the SIM APP PIN..."));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2==KErrNone); 
//	INFO_PRINTF2(_L("Value of secEvent = %d"),secEvent);
	TEST(secEvent==RMobilePhone::EUSIMAppPinVerified);
	
	INFO_PRINTF1(_L("GetAuthenticationParams completed Successfully in Client 1."));
	
	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrNone);
	if(reqSts3.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("NotifySecurityEvent completed with EUSIMAppPinVerified"));		
		}
	
	tGbaBootstrapParamsV8 = tGbaBootstrapParamsV8Pckg();
	
	_LIT8(KRand,"RAND_IN");
	_LIT8(KBtid,"B2345");
	_LIT8(KKeyLifeTime,"D3789");
	
	TEST(tGbaBootstrapParamsV8.iRand.Compare(KRand)==0);
	TEST(tGbaBootstrapParamsV8.iBtid.Compare(KBtid)==0);
	TEST(tGbaBootstrapParamsV8.iKeyLifetime.Compare(KKeyLifeTime)==0);
	
	iPhone.Close();	
	return TestStepResult();
	}

CSimUsimR6GBATestThread* CSimUsimR6GBATestThread::instance=NULL;

CSimUsimR6GBATestThread* CSimUsimR6GBATestThread::getInstanceL()
	{
	if(instance == NULL)
		{
		instance = new(ELeave) CSimUsimR6GBATestThread();
		}
	return instance;
	}

CSimUsimR6GBATestThread::CSimUsimR6GBATestThread()
	{
	// Do Nothing Constructor
	}

CSimUsimR6GBATestThread::CSimUsimR6GBATestThread(const CSimUsimR6GBATestThread&)
	{
	// Do Nothing copy Constructor
	}

CSimUsimR6GBATestThread& CSimUsimR6GBATestThread::operator=(const CSimUsimR6GBATestThread&)
	{
	// Do Nothing Operator overloaded function
	CSimUsimR6GBATestThread* test = CSimUsimR6GBATestThread::getInstanceL();
	return (*test);
	}

TVerdict CSimUsimR6GBATestThread::doTestStepL()
/**
TEL-SIMTSY-MM-
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	
	TRequestStatus reqSts1,reqSts2,reqSts3;

	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	
	iPhone.NotifyAuthenticateDataChange(reqSts2,tAuthInfoV8Pckg);
	
	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
		
	_LIT8(Krand,"ABCD"); 
	_LIT8(Kautn,"ADFC"); 
	_LIT8(Kaid,"AE879");
	tGbaBootstrapAuthDataV8.iRAND = Krand;
	tGbaBootstrapAuthDataV8.iAUTN = Kautn;
	
	RMobilePhone::TAID appAID = Kaid();
	
	iPhone.SmartCardAuthenticate(reqSts3, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrNone);
	 	
	User::WaitForRequest(reqSts2); // NotifyAuthenticateData completes successfully.
	TEST(reqSts2.Int() == KErrNone);
	if(reqSts2.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("NotifyAuthenticateDataChange completed Successfully in Client 1."));
		}
	else
		{
		INFO_PRINTF1(_L("NotifyAuthenticateDataChange Not completed Successfully in Client 1."));
		}
	
	tAuthInfoV8 = tAuthInfoV8Pckg();
	
	TEST(tAuthInfoV8.iAID == Kaid());
	TEST(tAuthInfoV8.iData == RMobilePhone::EGbaBootstrapParams );
	
	RMobilePhone::TBsfDataV8 tBsfDataV8;
	RMobilePhone::TBsfDataV8Pckg tBsfDataV8Pckg(tBsfDataV8);

	_LIT8(KBtid,"B2345");
	_LIT8(KKeyLifeTime,"D3789");
	tBsfDataV8.iBtid = KBtid;
	tBsfDataV8.iKeyLifetime = KKeyLifeTime;
	
	iPhone.SetGbaBootstrapParams(reqSts3,appAID,tBsfDataV8Pckg);

	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed with EUSIMAppPinRequired"));
	
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "1234");
	pass.Copy(Kpswrd);
	   
	INFO_PRINTF1(_L("Verifying the PIN1"));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2==KErrNone); 
//	INFO_PRINTF2(_L("Value of secEvent = %d"),secEvent);
	TEST(secEvent==RMobilePhone::EUSIMAppPinVerified);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed with EUSIMAppPinVerified"));
	
	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrNone);
	
	iPhone.Close();	
	return TestStepResult();
	}

TInt OtherCltGBA(void* /*aThreadData*/)
	{
	TInt ret =KErrNone;
	TRAP(ret,CSimUsimR6GBATestThread* myInstance = CSimUsimR6GBATestThread::getInstanceL());
	return ret;
	}

CSimUsimR6GBATest13::CSimUsimR6GBATest13() 
	{ 
	SetTestStepName(_L("UsimR6GBATest13"));
	}

TVerdict CSimUsimR6GBATest13::doTestStepL()
/**
TEL-SIMTSY-MM-8613.
Client is unable to retrieve GBA Bootstrapping parameters as the USIM application is not active.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(39);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	TRequestStatus reqSts;
	
	RMobilePhone::TGbaBootstrapParamsV8 tGbaBootstrapParamsV8;
	RMobilePhone::TGbaBootstrapParamsV8Pckg tGbaBootstrapParamsV8Pckg(tGbaBootstrapParamsV8);
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	
	_LIT8(Kaid,"AE879");
	
	tAuthInfoV8.iData = RMobilePhone::EGbaBootstrapParams;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts,tAuthInfoV8Pckg,tGbaBootstrapParamsV8Pckg);
	
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrMMEtelScApplicationNotActive);
	
	if(reqSts.Int() == KErrMMEtelScApplicationNotActive)
		{
		INFO_PRINTF1(_L("GetAuthenticationParams completed with Application Not Active"));		
		}
	iPhone.Close();
	return TestStepResult();	
	}

CSimUsimR6GBATest14::CSimUsimR6GBATest14() 
	{ 
	SetTestStepName(_L("UsimR6GBATest14"));
	}

TVerdict CSimUsimR6GBATest14::doTestStepL()
/**
TEL-SIMTSY-MM-8614.
Client is unable to retrieve GBA Bootstrapping parameters due to PIN verification failure.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(42);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts1,reqSts2,reqSts3;
	
	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);	
	
	RMobilePhone::TGbaBootstrapParamsV8 tGbaBootstrapParamsV8;
	RMobilePhone::TGbaBootstrapParamsV8Pckg tGbaBootstrapParamsV8Pckg(tGbaBootstrapParamsV8);
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	
	_LIT8(Kaid,"AE879");
	
	tAuthInfoV8.iData = RMobilePhone::EGbaBootstrapParams;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts3,tAuthInfoV8Pckg,tGbaBootstrapParamsV8Pckg);
	
	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);
	
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "3334"); // Invalid pin for the USIM Appln.
	pass.Copy(Kpswrd);
	   
	INFO_PRINTF1(_L("Verifying the SIM APP PIN..."));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2.Int() == KErrAccessDenied);  
	TEST(secEvent==RMobilePhone::EUSIMAppPinRequired);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed with USIM PIN Verification failed"));

	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrAccessDenied);	
	if(reqSts3.Int() == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("GetAuthenticationParams completed with KErrAccessDenied"));
		}	
	iPhone.Close();
	return TestStepResult();	
	}

CSimUsimR6GBATest15::CSimUsimR6GBATest15() 
	{ 
	SetTestStepName(_L("UsimR6GBATest15"));
	}

TVerdict CSimUsimR6GBATest15::doTestStepL()
/**
TEL-SIMTSY-MM-8615.
Client retrieves NAF_ID list from UICC successfully.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	CTestRetrieveMobilePhoneAuthenticationIds *pRetrieve = CTestRetrieveMobilePhoneAuthenticationIds::NewLC(iPhone,
				CTestRetrieveMobilePhoneAuthenticationIds::EGba);
	
	pRetrieve->SetAuthType(CTestRetrieveMobilePhoneAuthenticationIds::EGba);
	pRetrieve->TestRetrieve(RMobilePhone::EGbaNafIdList);

	//Delete test object
	CleanupStack::PopAndDestroy(); //pRetrieve	
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest16::CSimUsimR6GBATest16() 
	{ 
	SetTestStepName(_L("UsimR6GBATest16"));
	}	

TVerdict CSimUsimR6GBATest16::doTestStepL()
/**
TEL-SIMTSY-MM-8616.
Client is unable to perform GBA authentication due to MAC verification failure.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(44);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts;
	
	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
		
	_LIT8(Krand,"RAND_IN"); 
	_LIT8(Kautn,"AUTN_IN"); 
	_LIT8(Kaid,"AE879");
	tGbaBootstrapAuthDataV8.iRAND = Krand;
	tGbaBootstrapAuthDataV8.iAUTN = Kautn;
	
	RMobilePhone::TAID appAID = Kaid();
	
	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));
	
	iPhone.SmartCardAuthenticate(reqSts, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrMMEtelAuthMacInvalid);
	
	if(reqSts.Int() == KErrMMEtelAuthMacInvalid)
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate API completed with KErrMMEtelAuthMacInvalid"));
		}
	else
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate API not completed with KErrMMEtelAuthMacInvalid"));		
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest17::CSimUsimR6GBATest17() 
	{ 
	SetTestStepName(_L("UsimR6GBATest17"));
	}

TVerdict CSimUsimR6GBATest17::doTestStepL()
/**
TEL-SIMTSY-MM-8617.
Client is unable to perform GBA authentication due to invalid sequence.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(45);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts;
	
	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
		
	_LIT8(Krand,"RAND_IN"); 
	_LIT8(Kautn,"AUTN_IN"); 
	_LIT8(Kaid,"AE879");
	tGbaBootstrapAuthDataV8.iRAND = Krand;
	tGbaBootstrapAuthDataV8.iAUTN = Kautn;
	
	RMobilePhone::TAID appAID = Kaid();
	
	INFO_PRINTF1(_L("Testing SmartCardAuthenticate async with AID..."));
	
	iPhone.SmartCardAuthenticate(reqSts, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrMMEtelAuthVerificationFailure);
	
	if(reqSts.Int() == KErrMMEtelAuthVerificationFailure)
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate API completed with KErrMMEtelAuthVerificationFailure"));
		}
	else
		{
		INFO_PRINTF1(_L("SmartCardAuthenticate API not completed with KErrMMEtelAuthVerificationFailure"));		
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest18::CSimUsimR6GBATest18() 
	{ 
	SetTestStepName(_L("UsimR6GBATest18"));
	}

TVerdict CSimUsimR6GBATest18::doTestStepL()
/**
TEL-SIMTSY-MM-8618.
Cancel GBA authentication requests. (All APIs)
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts;
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	
	iPhone.NotifyAuthenticateDataChange(reqSts,tAuthInfoV8Pckg);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyAuthenticateDataChange);
	User::WaitForRequest(reqSts);
	TEST((reqSts.Int() == KErrNone) || (reqSts.Int() == KErrCancel));
	
	INFO_PRINTF1(_L("Cancellation of NotifyAuthenticateDataChange API Completed"));
	
	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
		
	_LIT8(Krand,"RAND_IN"); 
	_LIT8(Kautn,"AUTN_IN"); 
	_LIT8(Kaid,"AE879");
	
	tGbaBootstrapAuthDataV8.iRAND = Krand;
	tGbaBootstrapAuthDataV8.iAUTN = Kautn;
	
	RMobilePhone::TAID appAID = Kaid();
	
	iPhone.SmartCardAuthenticate(reqSts, appAID, tGbaBootstrapAuthDataV8Pckgd);
	iPhone.CancelAsyncRequest(EMobilePhoneSmartCardAuthenticate);
	User::WaitForRequest(reqSts);
		
	TEST((reqSts.Int() == KErrNone) || (reqSts.Int() == KErrCancel));
	INFO_PRINTF1(_L("Cancellation of SmartCardAuthenticate API Completed"));

	RMobilePhone::TGbaBootstrapParamsV8 tGbaBootstrapParamsV8;
	RMobilePhone::TGbaBootstrapParamsV8Pckg tGbaBootstrapParamsV8Pckg(tGbaBootstrapParamsV8);
		
	tAuthInfoV8.iData = RMobilePhone::EGbaBootstrapParams;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts,tAuthInfoV8Pckg,tGbaBootstrapParamsV8Pckg);
	iPhone.CancelAsyncRequest(EMobilePhoneGetAuthenticationParams);
	User::WaitForRequest(reqSts);
	
	TEST((reqSts.Int() == KErrNone) || (reqSts.Int() == KErrCancel));
	INFO_PRINTF1(_L("Cancellation of GetAuthenticationParams API Completed"));
	
	RMobilePhone::TBsfDataV8 tBsfDataV8;
	RMobilePhone::TBsfDataV8Pckg tBsfDataV8Pckg(tBsfDataV8);

	_LIT8(KBtid,"B2345");
	_LIT8(KKeyLifeTime,"D3789");
	tBsfDataV8.iBtid = KBtid;
	tBsfDataV8.iKeyLifetime = KKeyLifeTime;
	
	iPhone.SetGbaBootstrapParams(reqSts,appAID,tBsfDataV8Pckg);
	iPhone.CancelAsyncRequest(EMobilePhoneSetGbaBootstrapParams);
	User::WaitForRequest(reqSts);
	
	TEST((reqSts.Int() == KErrNone) || (reqSts.Int() == KErrCancel));
	INFO_PRINTF1(_L("Cancellation of SetGbaBootstrapParams API Completed"));
	
	/* Cancellation of the two phase retrieval */
	CTestRetrieveMobilePhoneAuthenticationIds *pRetrieve = CTestRetrieveMobilePhoneAuthenticationIds::NewLC(iPhone,
			CTestRetrieveMobilePhoneAuthenticationIds::EGba);

	pRetrieve->SetAuthType(CTestRetrieveMobilePhoneAuthenticationIds::EGba);
	pRetrieve->TestCancel(RMobilePhone::EGbaNafIdList);

	//Delete test object
	CleanupStack::PopAndDestroy(); //pRetrieve	
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest19::CSimUsimR6GBATest19() 
	{ 
	SetTestStepName(_L("UsimR6GBATest19"));
	}

TVerdict CSimUsimR6GBATest19::doTestStepL()
/**
TEL-SIMTSY-MM-8619.
Test SmartCardAuthenticate API with different capability
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Testing the SmartCardAuthenticate API with different capability"));
	
	/* Expected Capability = ReadDeviceData */
	/* Calling Client's capability = LocalServices */
	TRequestStatus reqSts;

	RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8;
	RMobilePhone::TGbaBootstrapAuthDataV8Pckg tGbaBootstrapAuthDataV8Pckgd(tGbaBootstrapAuthDataV8);
		
	_LIT8(Krand,"RAND_IN"); 
	_LIT8(Kautn,"AUTN_IN"); 
	_LIT8(Kaid,"AE879");
	tGbaBootstrapAuthDataV8.iRAND = Krand;
	tGbaBootstrapAuthDataV8.iAUTN = Kautn;
	
	RMobilePhone::TAID appAID = Kaid();
	
	iPhone.SmartCardAuthenticate(reqSts, appAID, tGbaBootstrapAuthDataV8Pckgd);
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrPermissionDenied);
	
	INFO_PRINTF1(_L("Testing of SmartCardAuthenticate API with different capability Completed"));	
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest20::CSimUsimR6GBATest20() 
	{ 
	SetTestStepName(_L("UsimR6GBATest20"));
	}

TVerdict CSimUsimR6GBATest20::doTestStepL()
/**
TEL-SIMTSY-MM-8620.
Test SetGbaBootstrapParams API with different capability
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Testing the SetGbaBootstrapParams API with different capability"));

	TRequestStatus reqSts;
	/* Expected Capability = WriteDeviceData */
	/* Calling Client's capability = LocalServices */
	
	_LIT8(Kaid,"AE879");
	RMobilePhone::TAID appAID = Kaid();
	
	RMobilePhone::TBsfDataV8 tBsfDataV8;
	RMobilePhone::TBsfDataV8Pckg tBsfDataV8Pckg(tBsfDataV8);

	_LIT8(KBtid,"B2345");
	_LIT8(KKeyLifeTime,"D3789");
	tBsfDataV8.iBtid = KBtid;
	tBsfDataV8.iKeyLifetime = KKeyLifeTime;
	
	iPhone.SetGbaBootstrapParams(reqSts,appAID,tBsfDataV8Pckg);
	User::WaitForRequest(reqSts);
	
	TEST(reqSts.Int() == KErrPermissionDenied);
	INFO_PRINTF1(_L("Testing of SetGbaBootstrapParams API with different capability Completed"));	
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest21::CSimUsimR6GBATest21() 
	{ 
	SetTestStepName(_L("UsimR6GBATest21"));
	}

TVerdict CSimUsimR6GBATest21::doTestStepL()
/**
TEL-SIMTSY-MM-8621.
Test NotifyAuthenticateDataChange API with different capability
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Testing the NotifyAuthenticateDataChange API with different capability"));

	TRequestStatus reqSts;
	/* Expected Capability = ReadDeviceData */
	/* Calling Client's capability = LocalServices */
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	
	iPhone.NotifyAuthenticateDataChange(reqSts,tAuthInfoV8Pckg);
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrPermissionDenied);
	
	INFO_PRINTF1(_L("Testing of NotifyAuthenticateDataChange API with different capability Completed"));	
	iPhone.Close();
	return TestStepResult();	
	}

CSimUsimR6GBATest22::CSimUsimR6GBATest22() 
	{ 
	SetTestStepName(_L("UsimR6GBATest22"));
	}

TVerdict CSimUsimR6GBATest22::doTestStepL()
/**
TEL-SIMTSY-MM-8622.
Test GetAuthenticationParams API with different capability
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(4);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Testing the GetAuthenticationParams API with different capability"));

	TRequestStatus reqSts;
	/* Expected Capability = ReadDeviceData */
	/* Calling Client's capability = LocalServices */
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);

	RMobilePhone::TGbaBootstrapParamsV8 tGbaBootstrapParamsV8;
	RMobilePhone::TGbaBootstrapParamsV8Pckg tGbaBootstrapParamsV8Pckg(tGbaBootstrapParamsV8);
	
	_LIT8(Kaid,"AE879");
	
	tAuthInfoV8.iData = RMobilePhone::EGbaBootstrapParams;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts,tAuthInfoV8Pckg,tGbaBootstrapParamsV8Pckg);
	User::WaitForRequest(reqSts);
	TEST(reqSts.Int() == KErrPermissionDenied);	
	
	INFO_PRINTF1(_L("Testing of GetAuthenticationParams API with different capability Completed"));	
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6GBATest23::CSimUsimR6GBATest23() 
	{ 
	SetTestStepName(_L("UsimR6GBATest23"));
	}

TVerdict CSimUsimR6GBATest23::doTestStepL()
/**
TEL-SIMTSY-MM-8623.
Test CRetrieveMobilePhoneAuthenticationIds::Start API with different capability
*/
	{	
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	CTestRetrieveMobilePhoneAuthenticationIds *pRetrieve = CTestRetrieveMobilePhoneAuthenticationIds::NewLC(iPhone,
				CTestRetrieveMobilePhoneAuthenticationIds::EGba);
	
	pRetrieve->SetAuthType(CTestRetrieveMobilePhoneAuthenticationIds::EGba);
	pRetrieve->TestRetrieve(RMobilePhone::EGbaNafIdList);

	//Delete test object
	CleanupStack::PopAndDestroy(); //pRetrieve	
	iPhone.Close();	
	return TestStepResult();
	}

/****************************************************************************\
 * 																			*
 * 			Multicast Broadcast Messaging Service Testcases 				*	
 * 																			*	
 ****************************************************************************/

CSimUsimR6MBMSTest1::CSimUsimR6MBMSTest1() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest1"));
	}

TVerdict CSimUsimR6MBMSTest1::doTestStepL()
/**
TEL-SIMTSY-MM-8624.
Client performs MBMS authentication in MSK update mode successfully.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Testing of MBMS Functionality "));
	
	/*Precondition:
 	 *  Client performed successfully GBA authentication in 
	 * 	NAF derivation mode after deriving Bootstrap key (Ks).
	 */
	
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");

	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	INFO_PRINTF1(_L("SmartCardAuthenticate API test completed "));
	
	TEST(tMbmsMskUpdateDataV8.iOutputMikey == _L8("OUT_MIKEY_V_BIT"));	
	iPhone.Close();
	return TestStepResult();
	}


CSimUsimR6MBMSTest2::CSimUsimR6MBMSTest2() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest2"));
	}

TVerdict CSimUsimR6MBMSTest2::doTestStepL()
/**
TEL-SIMTSY-MM-8625.
Client is unable to perform MBMS authentication in MSK update mode 
due to lack of MBMS service support in UICC.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(40);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Precondition:
	 * Client performed successfully GBA authentication in 
	 * NAF derivation mode after deriving Bootstrap key (Ks).
	 */
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotSupported);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrNotSupported."));
	iPhone.Close();	
	return TestStepResult();
	}

CSimUsimR6MBMSTest3::CSimUsimR6MBMSTest3() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest3"));
	}

TVerdict CSimUsimR6MBMSTest3::doTestStepL()
/**
TEL-SIMTSY-MM-8626.
Client is unable to perform MBMS authentication in 
MSK update mode because USIM application is not active.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(39);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	
	/* Precondition:
	 * 	Client performed successfully GBA authentication in 
	 * 	NAF derivation mode after deriving Bootstrap key (Ks).
	 */
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelScApplicationNotActive);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelScApplicationNotActive."));	
	iPhone.Close();	
	return TestStepResult();
	}

CSimUsimR6MBMSTest4::CSimUsimR6MBMSTest4() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest4"));
	}

TVerdict CSimUsimR6MBMSTest4::doTestStepL()
/**
TEL-SIMTSY-MM-8627.
Client is unable to perform MBMS authentication in 
MSK update mode because Ks_int_NAF is overwritten.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(41);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Precondition:
	 * 	Client performed successfully GBA authentication in 
	 * 	NAF derivation mode after deriving Bootstrap key (Ks).
	 * 	After the successful authentication, the KsIntNaf has been overwritten.
	 */
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAuthKeyNotFound);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelAuthKeyNotFound."));	
	iPhone.Close();	
	return TestStepResult();
	}

CSimUsimR6MBMSTest5::CSimUsimR6MBMSTest5() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest5"));
	}

TVerdict CSimUsimR6MBMSTest5::doTestStepL()
/**
TEL-SIMTSY-MM-8628.
Client is unable to perform MBMS authentication in MSK update mode due to out of memory.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(42);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Precondition:
	 * 	Client performed successfully GBA authentication in 
	 * NAF derivation mode after deriving Bootstrap key (Ks).
	 */
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNoMemory);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrNoMemory."));	
	iPhone.Close();
	return TestStepResult();
	}


CSimUsimR6MBMSTest6::CSimUsimR6MBMSTest6() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest6"));
	}

TVerdict CSimUsimR6MBMSTest6::doTestStepL()
/**
TEL-SIMTSY-MM-8629.
Client successfully creates space to store new MSK IDs and 
MUK IDs before initiating a MSK request procedure.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(42);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Precondition:
	 *	Client performed successfully GBA authentication in 
	 * NAF derivation mode after deriving Bootstrap key (Ks).
	 */
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");

	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNoMemory);
	if(reqStatus.Int() == KErrNoMemory)
		{
		INFO_PRINTF1(_L("RMobilePhone::SmartCardAuthenticate MSK Updation failed because of NoMemory."));
		}

	/*
	 * Client decides to delete the MSKs that are no longer needed to free up storage for new MSKs.   
	 */
	
	RMobilePhone::TMbmsMskDeleteDataV8 tMbmsMskDeleteDataV8;
	RMobilePhone::TMbmsMskDeleteDataV8Pckg tMbmsMskDeleteDataV8Pckgd(tMbmsMskDeleteDataV8);
	
	tMbmsMskDeleteDataV8.iKeyDomainId = _L8("KDN");
	tMbmsMskDeleteDataV8.iMskIdKeyGroup = _L8("GR");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMskDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	if(reqStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("RMobilePhone::SmartCardAuthenticate MSK Deletion successful."));
		}
	
	RMobilePhone::TMbmsMukDeleteDataV8 tMbmsMukDeleteDataV8;
	RMobilePhone::TMbmsMukDeleteDataV8Pckg tMbmsMukDeleteDataV8Pckgd(tMbmsMukDeleteDataV8);
	
	tMbmsMukDeleteDataV8.iMukIdTlv = _L8("MUK_ID_TLV");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMukDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	if(reqStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("RMobilePhone::SmartCardAuthenticate MUK Deletion successful."));
		}
	
	INFO_PRINTF1(_L("SmartCardAuthenticate API completed."));
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	if(reqStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("RMobilePhone::SmartCardAuthenticate MUK Updation successful."));
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest7::CSimUsimR6MBMSTest7() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest7"));
	}

TVerdict CSimUsimR6MBMSTest7::doTestStepL()
/**
TEL-SIMTSY-MM-8630.
Client runs GBA authentication in NAF derivation mode before re-running MSK update mode
*/
	{	
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(41);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	/* Precondition:
	 *	Client performed successfully GBA authentication in 
	 * NAF derivation mode after deriving Bootstrap key (Ks).
	 */
	
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAuthKeyNotFound);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelAuthKeyNotFound."));
	
	/* Precondition:
	 *	Client performed successfully GBA authentication in 
	 * NAF derivation mode after deriving Bootstrap key (Ks).
	 */
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	INFO_PRINTF1(_L("SmartCardAuthenticate API completed."));
	
	tMbmsMskUpdateDataV8 = tTMbmsMskUpdateDataV8Pckg();
	TEST(tMbmsMskUpdateDataV8.iOutputMikey == _L8("OUT_MIKEY_V_BIT"));
	
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest8::CSimUsimR6MBMSTest8() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest8"));
	}

TVerdict CSimUsimR6MBMSTest8::doTestStepL()
/**
TEL-SIMTSY-MM-8631.
Client is unable to perform MBMS authentication in 
MSK update mode because of MAC verification failure. 
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(44);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	/* Precondition:
	 * 	Client performed successfully GBA authentication in 
	 * NAF derivation mode after deriving Bootstrap key (Ks).
	 */
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAuthMacInvalid);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelAuthMacInvalid."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest9::CSimUsimR6MBMSTest9() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest9"));
	}

TVerdict CSimUsimR6MBMSTest9::doTestStepL()
/**
TEL-SIMTSY-MM-8632.
Client is unable to perform MBMS authentication in 
MSK update mode because of incorrect MUK. 
*/
	{	
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(41);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Precondition:
	 *	Client performed successfully GBA authentication in 
	 * NAF derivation mode after deriving Bootstrap key (Ks).
	 * After the succesful authentication, the KsIntNaf has been overwritten.
	 */
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAuthKeyNotFound);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelAuthKeyNotFound."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest10::CSimUsimR6MBMSTest10() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest10"));
	}

TVerdict CSimUsimR6MBMSTest10::doTestStepL()
/**
TEL-SIMTSY-MM-8633.
Client is unable to perform MBMS authentication in 
MSK update mode because of key freshness failure.
*/
	{	
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(45);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Precondition:
	 * Client performed successfully GBA authentication in 
	 * NAF derivation mode after deriving Bootstrap key (Ks).
	 * After the succesful authentication, the KsIntNaf has been overwritten.
	 */
	TRequestStatus reqStatus= KErrNone;
	
	RMobilePhone::TMbmsMskUpdateDataV8 tMbmsMskUpdateDataV8;
	RMobilePhone::TMbmsMskUpdateDataV8Pckg tTMbmsMskUpdateDataV8Pckg(tMbmsMskUpdateDataV8);
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	tMbmsMskUpdateDataV8.iInputMikey = _L8("IN_MIKEY_V_BIT");

	iPhone.SmartCardAuthenticate(reqStatus, appAID, tTMbmsMskUpdateDataV8Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAuthVerificationFailure);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelAuthVerificationFailure."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest11::CSimUsimR6MBMSTest11() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest11"));
	}

TVerdict CSimUsimR6MBMSTest11::doTestStepL()
/**
TEL-SIMTSY-MM-8634.
Client performs MBMS authentication in MTK generation mode successfully.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Client performs MBMS authentication in MTK generation mode successfully.*/
	
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	RMobilePhone::TMbmsMtkGenerateDataV8 tMbmsMtkGenerateDataV8;
	RMobilePhone::TMbmsMtkGenerateDataV8Pckg tMbmsMtkGenerateDataV8Pckgd(tMbmsMtkGenerateDataV8);

	tMbmsMtkGenerateDataV8.iMikey = _L8("IN_MIKEY_V_BIT");
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMtkGenerateDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	TEST(tMbmsMtkGenerateDataV8.iMtk == _L8("MBMS_TR_KEY"));
	TEST(tMbmsMtkGenerateDataV8.iSaltKey == _L8("SALT_KEY"));

	INFO_PRINTF1(_L("SmartCardAuthenticate API completed."));
	iPhone.Close();
	return TestStepResult();
	}


CSimUsimR6MBMSTest12::CSimUsimR6MBMSTest12() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest12"));
	}

TVerdict CSimUsimR6MBMSTest12::doTestStepL()
/**
TEL-SIMTSY-MM-8635.
Client is unable to perform MBMS authentication in MTK generation mode because 
this feature is not enabled in USIM service table.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(40);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	RMobilePhone::TMbmsMtkGenerateDataV8 tMbmsMtkGenerateDataV8;
	RMobilePhone::TMbmsMtkGenerateDataV8Pckg tMbmsMtkGenerateDataV8Pckgd(tMbmsMtkGenerateDataV8);

	tMbmsMtkGenerateDataV8.iMikey = _L8("IN_MIKEY_V_BIT");
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMtkGenerateDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotSupported);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrNotSupported."));
	iPhone.Close();
	return TestStepResult();
	}


CSimUsimR6MBMSTest13::CSimUsimR6MBMSTest13() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest13"));
	}

TVerdict CSimUsimR6MBMSTest13::doTestStepL()
/**
TEL-SIMTSY-MM-8636.
Client is unable to perform MBMS authentication in 
MSK update mode because USIM application is not active.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(39);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	RMobilePhone::TMbmsMtkGenerateDataV8 tMbmsMtkGenerateDataV8;
	RMobilePhone::TMbmsMtkGenerateDataV8Pckg tMbmsMtkGenerateDataV8Pckgd(tMbmsMtkGenerateDataV8);

	tMbmsMtkGenerateDataV8.iMikey = _L8("IN_MIKEY_V_BIT");
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMtkGenerateDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelScApplicationNotActive);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelScApplicationNotActive."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest14::CSimUsimR6MBMSTest14() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest14"));
	}

TVerdict CSimUsimR6MBMSTest14::doTestStepL()
/**
TEL-SIMTSY-MM-8637.
Client is unable to perform MBMS authentication in 
MTK generation mode due to unavailability of MSK.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(41);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	RMobilePhone::TMbmsMtkGenerateDataV8 tMbmsMtkGenerateDataV8;
	RMobilePhone::TMbmsMtkGenerateDataV8Pckg tMbmsMtkGenerateDataV8Pckgd(tMbmsMtkGenerateDataV8);

	tMbmsMtkGenerateDataV8.iMikey = _L8("IN_MIKEY_V_BIT");
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMtkGenerateDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAuthKeyNotFound);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelAuthKeyNotFound."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest15::CSimUsimR6MBMSTest15() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest15"));
	}

TVerdict CSimUsimR6MBMSTest15::doTestStepL()
/**
TEL-SIMTSY-MM-8638.
Client is unable to perform MBMS authentication in 
MTK generation mode because of invalidated MSK.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(45);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	RMobilePhone::TMbmsMtkGenerateDataV8 tMbmsMtkGenerateDataV8;
	RMobilePhone::TMbmsMtkGenerateDataV8Pckg tMbmsMtkGenerateDataV8Pckgd(tMbmsMtkGenerateDataV8);

	tMbmsMtkGenerateDataV8.iMikey = _L8("IN_MIKEY_V_BIT");
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMtkGenerateDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAuthVerificationFailure);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelAuthVerificationFailure."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest16::CSimUsimR6MBMSTest16() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest16"));
	}

TVerdict CSimUsimR6MBMSTest16::doTestStepL()
/**
TEL-SIMTSY-MM-8639.
Client is unable to perform MBMS authentication in 
MTK generation mode because of invalid MTK. 
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(49);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	RMobilePhone::TAID appAID = Kaid();
	
	RMobilePhone::TMbmsMtkGenerateDataV8 tMbmsMtkGenerateDataV8;
	RMobilePhone::TMbmsMtkGenerateDataV8Pckg tMbmsMtkGenerateDataV8Pckgd(tMbmsMtkGenerateDataV8);

	tMbmsMtkGenerateDataV8.iMikey = _L8("IN_MIKEY_V_BIT");
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMtkGenerateDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelAuthKeyInvalid);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelAuthKeyInvalid."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest17::CSimUsimR6MBMSTest17() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest17"));
	}

TVerdict CSimUsimR6MBMSTest17::doTestStepL()
/**
TEL-SIMTSY-MM-8640.
Client performs MBMS authentication in MSK Deletion Mode successfully.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/*Client retrieves MSK ID list from UICC successfully. */
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	RMobilePhone::TAID appAID = Kaid();
	
	/*Client decides to delete the MSKs that are no longer needed to free up storage for new MSKs.   */
	
	RMobilePhone::TMbmsMskDeleteDataV8 tMbmsMskDeleteDataV8;
	RMobilePhone::TMbmsMskDeleteDataV8Pckg tMbmsMskDeleteDataV8Pckgd(tMbmsMskDeleteDataV8);
	
	tMbmsMskDeleteDataV8.iKeyDomainId = _L8("KDN");
	tMbmsMskDeleteDataV8.iMskIdKeyGroup = _L8("GR");

	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMskDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	INFO_PRINTF1(_L("SmartCardAuthenticate API completed."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest18::CSimUsimR6MBMSTest18() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest18"));
	}

TVerdict CSimUsimR6MBMSTest18::doTestStepL()
/**
TEL-SIMTSY-MM-8641.
Client performs MBMS authentication in MSK Deletion Mode - record match not found.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(47);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/*Client retrieves MSK ID list from UICC successfully. */
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	RMobilePhone::TAID appAID = Kaid();
	
	/*Client decides to delete the MSKs that are no longer needed to free up storage for new MSKs.   */
	
	RMobilePhone::TMbmsMskDeleteDataV8 tMbmsMskDeleteDataV8;
	RMobilePhone::TMbmsMskDeleteDataV8Pckg tMbmsMskDeleteDataV8Pckgd(tMbmsMskDeleteDataV8);
	
	tMbmsMskDeleteDataV8.iKeyDomainId = _L8("KDN");
	tMbmsMskDeleteDataV8.iMskIdKeyGroup = _L8("GR");

	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMskDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotFound);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrNotFound."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest19::CSimUsimR6MBMSTest19() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest19"));
	}

TVerdict CSimUsimR6MBMSTest19::doTestStepL()
/**
TEL-SIMTSY-MM-8642.
Client is unable to perform MBMS authentication in 
MSK Deletion Mode because USIM application is not active.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(39);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/*Client retrieves MSK ID list from UICC successfully. */
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	RMobilePhone::TAID appAID = Kaid();
	
	/*Client decides to delete the MSKs that are no longer needed to free up storage for new MSKs.   */
	
	RMobilePhone::TMbmsMskDeleteDataV8 tMbmsMskDeleteDataV8;
	RMobilePhone::TMbmsMskDeleteDataV8Pckg tMbmsMskDeleteDataV8Pckgd(tMbmsMskDeleteDataV8);
	
	tMbmsMskDeleteDataV8.iKeyDomainId = _L8("KDN");
	tMbmsMskDeleteDataV8.iMskIdKeyGroup = _L8("GR");

	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMskDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelScApplicationNotActive);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelScApplicationNotActive."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest20::CSimUsimR6MBMSTest20() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest20"));
	}

TVerdict CSimUsimR6MBMSTest20::doTestStepL()
/**
TEL-SIMTSY-MM-8643.
Client is unable to perform MBMS authentication in 
MSK Deletion Mode due to lack of MBMS service support in UICC.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(40);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/*Client retrieves MSK ID list from UICC successfully. */
	TRequestStatus reqStatus = KErrNone;
	_LIT8(Kaid,"AE879");
	
	RMobilePhone::TAID appAID = Kaid();
	
	/*Client decides to delete the MSKs that are no longer needed to free up storage for new MSKs.   */
	
	RMobilePhone::TMbmsMskDeleteDataV8 tMbmsMskDeleteDataV8;
	RMobilePhone::TMbmsMskDeleteDataV8Pckg tMbmsMskDeleteDataV8Pckgd(tMbmsMskDeleteDataV8);
	
	tMbmsMskDeleteDataV8.iKeyDomainId = _L8("KDN");
	tMbmsMskDeleteDataV8.iMskIdKeyGroup = _L8("GR");

	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMskDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotSupported);

	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrNotSupported."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest21::CSimUsimR6MBMSTest21() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest21"));
	}

TVerdict CSimUsimR6MBMSTest21::doTestStepL()
/**
TEL-SIMTSY-MM-8644.
Client performs MBMS authentication in MUK Deletion Mode successfully.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Client retrieves MUK ID and timestamp counter successfully.*/
	
	TRequestStatus reqStatus=KErrNone;
	_LIT8(Kaid,"AE879");
	
	RMobilePhone::TAID appAID = Kaid();
	
	/*Client decides to delete the MUK to free up storage for new MUK */
	
	RMobilePhone::TMbmsMukDeleteDataV8 tMbmsMukDeleteDataV8;
	RMobilePhone::TMbmsMukDeleteDataV8Pckg tMbmsMukDeleteDataV8Pckgd(tMbmsMukDeleteDataV8);
	
	tMbmsMukDeleteDataV8.iMukIdTlv = _L8("MUK_ID_TLV");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMukDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate API completed."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest22::CSimUsimR6MBMSTest22() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest22"));
	}

TVerdict CSimUsimR6MBMSTest22::doTestStepL()
/**
TEL-SIMTSY-MM-8645.
Client performs MBMS authentication in MUK Deletion Mode - record match not found.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(47);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Client retrieves MUK ID and timestamp counter successfully.*/
	
	TRequestStatus reqStatus=KErrNone;
	_LIT8(Kaid,"AE879");
	
	RMobilePhone::TAID appAID = Kaid();
	
	/*Client decides to delete the MUK to free up storage for new MUK */
	
	RMobilePhone::TMbmsMukDeleteDataV8 tMbmsMukDeleteDataV8;
	RMobilePhone::TMbmsMukDeleteDataV8Pckg tMbmsMukDeleteDataV8Pckgd(tMbmsMukDeleteDataV8);
	
	tMbmsMukDeleteDataV8.iMukIdTlv = _L8("MUK_ID_TLV");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMukDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotFound);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrNotFound."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest23::CSimUsimR6MBMSTest23() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest23"));
	}

TVerdict CSimUsimR6MBMSTest23::doTestStepL()
/**
TEL-SIMTSY-MM-8646.
Client is unable to perform MBMS authentication in 
MSK update mode because UICC application is not active.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(39);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Client retrieves MUK ID and timestamp counter successfully.*/
	
	TRequestStatus reqStatus=KErrNone;
	_LIT8(Kaid,"AE879");
	
	RMobilePhone::TAID appAID = Kaid();
	
	/*Client decides to delete the MUK to free up storage for new MUK */
	
	RMobilePhone::TMbmsMukDeleteDataV8 tMbmsMukDeleteDataV8;
	RMobilePhone::TMbmsMukDeleteDataV8Pckg tMbmsMukDeleteDataV8Pckgd(tMbmsMukDeleteDataV8);
	
	tMbmsMukDeleteDataV8.iMukIdTlv = _L8("MUK_ID_TLV");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMukDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrMMEtelScApplicationNotActive);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrMMEtelScApplicationNotActive."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest24::CSimUsimR6MBMSTest24() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest24"));
	}

TVerdict CSimUsimR6MBMSTest24::doTestStepL()
/**
TEL-SIMTSY-MM-8647.
Client is unable to perform MBMS authentication in 
MSK update mode because this feature is not enabled in USIM service table
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(40);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	/* Client retrieves MUK ID and timestamp counter successfully.*/
	
	TRequestStatus reqStatus=KErrNone;
	_LIT8(Kaid,"AE879");
	
	RMobilePhone::TAID appAID = Kaid();
	
	/*Client decides to delete the MUK to free up storage for new MUK */
	
	RMobilePhone::TMbmsMukDeleteDataV8 tMbmsMukDeleteDataV8;
	RMobilePhone::TMbmsMukDeleteDataV8Pckg tMbmsMukDeleteDataV8Pckgd(tMbmsMukDeleteDataV8);
	
	tMbmsMukDeleteDataV8.iMukIdTlv = _L8("MUK_ID_TLV");
	
	INFO_PRINTF1(_L("Testing RMobilePhone::SmartCardAuthenticate async with AID."));
	
	iPhone.SmartCardAuthenticate(reqStatus, appAID, tMbmsMukDeleteDataV8Pckgd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotSupported);
	
	INFO_PRINTF1(_L("SmartCardAuthenticate completes with KErrNotSupported."));
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest25::CSimUsimR6MBMSTest25() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest25"));
	}

TVerdict CSimUsimR6MBMSTest25::doTestStepL()
/**
TEL-SIMTSY-MM-8648.
Client retrieves MUK ID and timestamp counter successfully.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts1,reqSts2,reqSts3;
	
	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);

	RMobilePhone::TMbmsUserKeyV8 tMbmsUserKeyV8;
	RMobilePhone::TMbmsUserKeyV8Pckg tMbmsUserKeyV8Pckg(tMbmsUserKeyV8);
	
	_LIT8(Kaid,"AE879");
	
	tAuthInfoV8.iData = RMobilePhone::EMbmsMuk;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts3,tAuthInfoV8Pckg,tMbmsUserKeyV8Pckg);
	
	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);
	
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "1234");
	pass.Copy(Kpswrd);
	   
	INFO_PRINTF1(_L("Verifying the SIM APP PIN..."));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2==KErrNone); 
	INFO_PRINTF2(_L("Value of secEvent = %d"),secEvent);
	TEST(secEvent==RMobilePhone::EUSIMAppPinVerified);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed with EUSIMAppPinVerified"));
	
	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrNone);
	if(reqSts3.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("GetAuthenticationParams completed successfully."));
		}
	else
		{
		INFO_PRINTF1(_L("GetAuthenticationParams not completed successfully."));
		}
	
	tMbmsUserKeyV8 = tMbmsUserKeyV8Pckg();  
	TEST(tMbmsUserKeyV8.iMukIdi == _L8("MUK_IDI"));
	TEST(tMbmsUserKeyV8.iMukIdr == _L8("MUK_IDR"));
	TEST(tMbmsUserKeyV8.iTimeStampCounter == _L8("STM"));
	
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest26::CSimUsimR6MBMSTest26() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest26"));
	}

TVerdict CSimUsimR6MBMSTest26::doTestStepL()
/**
TEL-SIMTSY-MM-8689.
Client is unable to retrieve MUK ID and timestamp counter because 
UICC application is not active.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(39);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts1,reqSts2,reqSts3;
	
	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);

	RMobilePhone::TMbmsUserKeyV8 tMbmsUserKeyV8;
	RMobilePhone::TMbmsUserKeyV8Pckg tMbmsUserKeyV8Pckg(tMbmsUserKeyV8);
	
	_LIT8(Kaid,"AE879");
	
	tAuthInfoV8.iData = RMobilePhone::EMbmsMuk;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts3,tAuthInfoV8Pckg,tMbmsUserKeyV8Pckg);
	
	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);
	
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "1234");
	pass.Copy(Kpswrd);
	   
	INFO_PRINTF1(_L("Verifying the SIM APP PIN..."));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2==KErrNone); 
	INFO_PRINTF2(_L("Value of secEvent = %d"),secEvent);
	TEST(secEvent==RMobilePhone::EUSIMAppPinVerified);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed with EUSIMAppPinVerified"));
	
	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrMMEtelScApplicationNotActive);
	if(reqSts3.Int() == KErrMMEtelScApplicationNotActive)
		{
		INFO_PRINTF1(_L("GetAuthenticationParams completed with KErrMMEtelScApplicationNotActive."));
		}
	else
		{
		INFO_PRINTF1(_L("GetAuthenticationParams not completed with KErrMMEtelScApplicationNotActive."));
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest27::CSimUsimR6MBMSTest27() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest27"));
	}

TVerdict CSimUsimR6MBMSTest27::doTestStepL()
/**
TEL-SIMTSY-MM-8650.
Client is unable to retrieve MUK ID due to PIN verification failure.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 
	SetTestNumberL(48);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));

	TRequestStatus reqSts1,reqSts2,reqSts3;
	
	RMobilePhone::TMobilePhoneSecurityEvent secEvent;
	iPhone.NotifySecurityEvent(reqSts1,secEvent);
	
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);

	RMobilePhone::TMbmsUserKeyV8 tMbmsUserKeyV8;
	RMobilePhone::TMbmsUserKeyV8Pckg tMbmsUserKeyV8Pckg(tMbmsUserKeyV8);
	
	_LIT8(Kaid,"AE879");
	
	tAuthInfoV8.iData = RMobilePhone::EMbmsMuk;
	tAuthInfoV8.iAID = Kaid();
	
	iPhone.GetAuthenticationParams(reqSts3,tAuthInfoV8Pckg,tMbmsUserKeyV8Pckg);
	
	User::WaitForRequest(reqSts1);
	TEST(secEvent == RMobilePhone::EUSIMAppPinRequired);
	
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityUSIMAppPin;
	RMobilePhone::TMobilePassword pass;
	_LIT8(Kpswrd, "6789");  // Invalid password
	pass.Copy(Kpswrd);
	   
	INFO_PRINTF1(_L("Verifying the SIM APP PIN..."));
	iPhone.NotifySecurityEvent(reqSts1, secEvent);
	iPhone.VerifySecurityCode(reqSts2, type, pass,pass);
	User::WaitForRequest(reqSts2);
	User::WaitForRequest(reqSts1);
	TEST(reqSts2==KErrAccessDenied); 
	TEST(secEvent==RMobilePhone::EUSIMAppPinRequired);
	
	INFO_PRINTF1(_L("NotifySecurityEvent completed with USIM PIN Verification failed"));

	User::WaitForRequest(reqSts3);
	TEST(reqSts3.Int() == KErrAccessDenied);	
	if(reqSts3.Int() == KErrAccessDenied)
		{
		INFO_PRINTF1(_L("GetAuthenticationParams completed with KErrAccessDenied"));
		}
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest28::CSimUsimR6MBMSTest28() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest28"));
	}

TVerdict CSimUsimR6MBMSTest28::doTestStepL()
/**
TEL-SIMTSY-MM-8651.
Client retrieves MSK ID list from UICC successfully.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(38);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	
	CTestRetrieveMobilePhoneAuthenticationIds *pRetrieve = CTestRetrieveMobilePhoneAuthenticationIds::NewLC(iPhone,
			CTestRetrieveMobilePhoneAuthenticationIds::EMbms);

	pRetrieve->SetAuthType(CTestRetrieveMobilePhoneAuthenticationIds::EMbms);
	pRetrieve->TestRetrieve(RMobilePhone::EMbmsMskIdList);

	//Delete test object
	CleanupStack::PopAndDestroy(); //pRetrieve	
	iPhone.Close();
	return TestStepResult();
	}

CSimUsimR6MBMSTest29::CSimUsimR6MBMSTest29() 
	{ 
	SetTestStepName(_L("UsimR6MBMSTest29"));
	}

TVerdict CSimUsimR6MBMSTest29::doTestStepL()
/**
TEL-SIMTSY-MM-8652.
Client confirms on the support of MBMS functionality by the USIM.
*/
	{
	CreateConfigFileL(_L("c:\\config3.txt")); 	
	SetTestNumberL(39);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L(""));
	TRequestStatus reqStatus=KErrNone;

	//Verify the support of MBMS in USIM Service Table

	RMobilePhone::TMobilePhoneServiceTableV8 asyncSstV8File;
	RMobilePhone::TMobilePhoneServiceTableV8Pckg asyncSstV8FilePckg(asyncSstV8File);
	RMobilePhone::TMobilePhoneServiceTable serviceTableType=RMobilePhone::EUSIMServiceTable;
	
	iPhone.GetServiceTable(reqStatus, serviceTableType, asyncSstV8FilePckg);
	
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	if(reqStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for MBMS passed"));
		}
	else
		{
		INFO_PRINTF1(_L("Test - RMobilePhone::GetServiceTable V8table for MBMS failed"));
		}

	TEST((asyncSstV8File.iServices65To72) == RMobilePhone::KUstMBMSSecurity);
	iPhone.Close();
	return TestStepResult();
	}

CTestCanceller::CTestCanceller(CActive* aRetrieve)
: CAsyncOneShot(EPriorityNormal),iRetrieve(aRetrieve)
	{}

CTestCanceller::~CTestCanceller()
	{}

void CTestCanceller::RunL()
	{
	iRetrieve->Cancel();
	CActiveScheduler::Stop();
	}

CTestRetrieveMobilePhoneAuthenticationIds* CTestRetrieveMobilePhoneAuthenticationIds::NewLC(RMobilePhone& aPhone,TTypeOfAuth aType)
	{
	CTestRetrieveMobilePhoneAuthenticationIds* r=new(ELeave) CTestRetrieveMobilePhoneAuthenticationIds(aPhone,aType);
	CleanupStack::PushL(r);
	r->ConstructL();
	return r;
	}
  
CTestRetrieveMobilePhoneAuthenticationIds::CTestRetrieveMobilePhoneAuthenticationIds(RMobilePhone& aPhone,TTypeOfAuth aType)
: CActive(EPriorityNormal), iPhone(aPhone), iAuthType(aType)
	{}
 
void CTestRetrieveMobilePhoneAuthenticationIds::ConstructL()
	{
	CActiveScheduler::Add(this);
	iRetrieveAuthId=CRetrieveMobilePhoneAuthenticationIds::NewL(iPhone);
	iCanceller=new (ELeave) CTestCanceller(this);
	iGBATest = new (ELeave) CSimUsimR6GBATest14();
	}
     
CTestRetrieveMobilePhoneAuthenticationIds::~CTestRetrieveMobilePhoneAuthenticationIds()
	{
	delete iRetrieveAuthId;
	delete iCanceller;
	delete iGBATest;
	}
void CTestRetrieveMobilePhoneAuthenticationIds::SetAuthType(TTypeOfAuth aAuthType)
	{
	iAuthType = aAuthType;
	iGBATest->INFO_PRINTF2(_L("Value of iAuthType = %d"),iAuthType);
	}

void CTestRetrieveMobilePhoneAuthenticationIds::TestRetrieve(RMobilePhone::TAuthData aAuthData)
	{
	Start(aAuthData);
	CActiveScheduler::Start();
	}

void CTestRetrieveMobilePhoneAuthenticationIds::Start(RMobilePhone::TAuthData aAuthData)
	{
	//If iTestVersion is set to EGba, test calling the Gba retrieval method
	RMobilePhone::TAuthInfoV8 tAuthInfoV8;
	RMobilePhone::TAuthInfoV8Pckg tAuthInfoV8Pckg(tAuthInfoV8);
	
	if(iAuthType==CTestRetrieveMobilePhoneAuthenticationIds::EGba)
		{
		tAuthInfoV8.iData = aAuthData;
		iRetrieveAuthId->Start(iStatus,tAuthInfoV8Pckg);
		TRequestStatus tmpStatus;
		iRetrieveAuthId->Start(tmpStatus,tAuthInfoV8Pckg);
		User::WaitForRequest(tmpStatus);
		iGBATest->TEST(tmpStatus.Int() == KErrInUse);
		SetActive();
		}
	//If iTestVersion is set to EMbms, test calling the Mbms retrieval method
	else if(iAuthType==CTestRetrieveMobilePhoneAuthenticationIds::EMbms)
		{
		tAuthInfoV8.iData = aAuthData;
		iRetrieveAuthId->Start(iStatus,tAuthInfoV8Pckg);
		TRequestStatus tmpStatus;
		iRetrieveAuthId->Start(tmpStatus,tAuthInfoV8Pckg);
		User::WaitForRequest(tmpStatus);
		iGBATest->TEST(tmpStatus.Int() == KErrInUse);
		SetActive();
		}
	else
		{}
	}       

void CTestRetrieveMobilePhoneAuthenticationIds::RunL()
	{
	TInt ret=iStatus.Int();
	if(ret == KErrPermissionDenied)
		{
		iGBATest->INFO_PRINTF1(_L("CRetrieveMobilePhoneAuthenticationId -> Start failed with Permission Denied"));
		CActiveScheduler::Stop();
		return;
		}
	else
		{
		User::LeaveIfError(ret);
		}

	iGBATest->INFO_PRINTF2(_L("In RunL with iAuthType = %d"),iAuthType);	

	if(iAuthType == EGba)
		{
		iGBATest->INFO_PRINTF1(_L("In RunL with iAuthType =EGba"));
		ret=CheckGBAResults();
		}
	else if(iAuthType == EMbms)
		{
		iGBATest->INFO_PRINTF1(_L("In RunL with iAuthType =EMbms"));
		ret=CheckMBMSResults();
		}
	else
		{
		iGBATest->INFO_PRINTF1(_L("In RunL with iAuthType =Error"));
		ret=KErrNotFound;
		}

	if((ret != KErrNone) && (ret != KErrNotFound))
		{
		User::LeaveIfError(ret);
		}

	CActiveScheduler::Stop();
	}

void CTestRetrieveMobilePhoneAuthenticationIds::TestCancel(RMobilePhone::TAuthData aAuthData)
	{
	Start(aAuthData);

	CTestCanceller* cancelAuthData = RetrieveCancelPtr();
	cancelAuthData->Call();

	CActiveScheduler::Start();
	TBool cancelled = (iStatus == KErrCancel || iStatus == KErrNone);

	// Ensure that AO can be used after its previous transaction was cancelled
	Start(aAuthData);
	CActiveScheduler::Start();

	if (cancelled)
		{
		iGBATest->INFO_PRINTF2(_L("Cancelled Type of Authentication is %d"),iAuthType);
		iGBATest->INFO_PRINTF1(_L("Test %d - CRetrieveMobilePhoneAuthenticationIds (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		iGBATest->INFO_PRINTF1(_L("Test %d - CRetrieveMobilePhoneAuthenticationIds (async & cancel (Request Not Cancelled)) passed"));
		}	
	}      

CTestCanceller* CTestRetrieveMobilePhoneAuthenticationIds::RetrieveCancelPtr()
	{
	return iCanceller;
	}
  
void CTestRetrieveMobilePhoneAuthenticationIds::DoCancel()
	{
	iRetrieveAuthId->Cancel();
	}       
  
TInt CTestRetrieveMobilePhoneAuthenticationIds::CheckGBAResults()
	{
	TInt ret=KErrNone;

	CMobilePhoneGbaNafIdList* nafList =NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, nafList=iRetrieveAuthId->RetrieveGbaListL(););

	//Check that a list is returned back to the test
	iGBATest->TEST(nafList!=NULL);

	if (nafList)
		{

		RMobilePhone::TGbaNafEntryV8 nafEntry;
		
		//Check that the number of entries in the list is as expected
		if(nafList->Enumerate() != SIMTSY_GBA_NAFLIST_COUNT)
			{
			TRAP(ret,nafEntry=nafList->GetEntryL(0));
			if (ret != KErrNone)
				{
				iGBATest-> INFO_PRINTF1(_L("The NAFLIST doesnot contain TGbaNafEntryV8 entries "));
				}
			delete nafList;
			return KErrNotFound;
			}

		// Get each NafList entry

		for (TInt i=0; i< SIMTSY_GBA_NAFLIST_COUNT; ++i)
			{
			TRAP(ret,nafEntry=nafList->GetEntryL(i));
			if (ret != KErrNone)
				{
				break;
				}
			switch(i)
				{
				case 0:
					{
					iGBATest->TEST(nafEntry.iBtid == _L8("C6453"));
					iGBATest->TEST(nafEntry.iNafId == _L8("B2345"));
					break;
					}
				case 1:
					{
					iGBATest->TEST(nafEntry.iBtid == _L8("D6453"));
					iGBATest->TEST(nafEntry.iNafId == _L8("C2345"));
					break;
					}
				case 2:
					{
					iGBATest->TEST(nafEntry.iBtid == _L8("E6453"));
					iGBATest->TEST(nafEntry.iNafId == _L8("D2345"));
					break;
					}
				default:
					{
					break;
					}
				}
			}
		delete nafList;
		}
	return ret;
	}

TInt CTestRetrieveMobilePhoneAuthenticationIds::CheckMBMSResults()
	{
	TInt ret=KErrNone;

	CMobilePhoneMbmsMskIdList* mbmsList =NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, mbmsList=iRetrieveAuthId->RetrieveMbmsListL(););

	//Check that a list is returned back to the test
	iGBATest->TEST(mbmsList!=NULL);

	if (mbmsList)
		{
		//Check that the number of entries in the list is as expected
		RMobilePhone::TMskEntryV8 tMskEntry;
		
		if(mbmsList->Enumerate() != SIMTSY_MBMS_MSKLIST_COUNT)
			{
			TRAP(ret,tMskEntry=mbmsList->GetEntryL(0));
			if (ret != KErrNone)
				{
				iGBATest-> INFO_PRINTF1(_L("The MBMSLIST doesnot contain TMskEntryV8 entries "));
				}
			delete mbmsList;
			return KErrNotFound;
			}
											  
		// Get each MbmsList entry

		for (TInt i=0; i<SIMTSY_MBMS_MSKLIST_COUNT; ++i)
			{
			TRAP(ret,tMskEntry=mbmsList->GetEntryL(i));
			if (ret != KErrNone)
				{
				break;
				}
			switch(i)
				{
				case 0:
					{
					iGBATest->TEST(tMskEntry.iMskId == _L8("MSK1"));
					iGBATest->TEST(tMskEntry.iKeyDomainId == _L8("KEYDMN1"));
					iGBATest->TEST(tMskEntry.iTimeStampCounter == _L8("STM1"));
					break;
					}
				case 1:
					{
					iGBATest->TEST(tMskEntry.iMskId == _L8("MSK2"));
					iGBATest->TEST(tMskEntry.iKeyDomainId == _L8("KEYDMN2"));
					iGBATest->TEST(tMskEntry.iTimeStampCounter == _L8("STM2"));
					break;
					}
				case 2:
					{
					iGBATest->TEST(tMskEntry.iMskId == _L8("MSK3"));
					iGBATest->TEST(tMskEntry.iKeyDomainId == _L8("KEYDMN3"));
					iGBATest->TEST(tMskEntry.iTimeStampCounter == _L8("STM3"));
					break;
					}
				default:
					break;
				}
			}
		delete mbmsList;
		}
	return ret;
	}



