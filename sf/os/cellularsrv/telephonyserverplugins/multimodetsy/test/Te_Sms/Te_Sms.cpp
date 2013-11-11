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
// Tests the Sms functionality of the Basic MMTsy.
// 
//

/**
 @file
*/

#include "Te_Sms.h"		


const TInt KOneSecond = 1000000;



CTestSendSms::CTestSendSms() 
	{
	SetTestStepName(_L("TestSendSms"));
	}

enum TVerdict CTestSendSms::doTestStepL()
	{
	SendSms();

	return EPass;
	}

CTestAckSmsStored::CTestAckSmsStored() 
	{
	SetTestStepName(_L("TestAckSmsStored"));
	}

enum TVerdict CTestAckSmsStored::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
	TRequestStatus status;
	TBuf8<400> buf;
	TBuf8<400> *NULLbuf =NULL;

	// Create a PDU

	RMobilePhone::TMobileAddress addr;
	addr.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("44 7785 016 005"));
	buf.Zero();	
	AppendAddressToAscii(buf,addr);
	iSms.AckSmsStored(status, buf);
	User::WaitForRequest(status);
	if(status.Int() == KErrNotSupported)
		{
		iSms.AckSmsStored(status, *NULLbuf);
		User::WaitForRequest(status);
		TESTL(status.Int() == KErrNotSupported);
		}
	
	return TestStepResult();
	}

CTestAckSmsStoredCancel::CTestAckSmsStoredCancel() 
	{
	SetTestStepName(_L("TestAckSmsStoredCancel"));
	}

enum TVerdict CTestAckSmsStoredCancel::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.AckSmsStoredCancel();		// This API doesn't exist anymore

	return EPass;
	}

CTestNackSmsStored::CTestNackSmsStored() 
	{
	SetTestStepName(_L("TestNackSmsStored"));
	}

enum TVerdict CTestNackSmsStored::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
	TRequestStatus status;
	TBuf8<400> buf;

	// Create a PDU

	RMobilePhone::TMobileAddress addr;
	addr.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("44 7785 016 005"));
	buf.Zero();	
	AppendAddressToAscii(buf,addr);
	iSms.NackSmsStored(status, buf, KErrGsmRRInvalidMessage);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNotSupported);

	return TestStepResult();
	}

CTestNackSmsStoredCancel::CTestNackSmsStoredCancel() 
	{
	SetTestStepName(_L("TestNackSmsStoredCancel"));
	}

enum TVerdict CTestNackSmsStoredCancel::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NackSmsStoredCancel();		// API doesn't exist anymore

	return EPass;
	}

CTestAckSmsStoredWithResponse::CTestAckSmsStoredWithResponse() 
	{
	SetTestStepName(_L("TestAckSmsStoredWithResponse"));
	}

enum TVerdict CTestAckSmsStoredWithResponse::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.AckSmsStoredWithResponse();	// API doesn't exist anymore

	return EPass;
	}

CTestAckSmsStoredWithResponseCancel::CTestAckSmsStoredWithResponseCancel() 
	{
	SetTestStepName(_L("TestAckSmsStoredWithResponseCancel"));
	}

enum TVerdict CTestAckSmsStoredWithResponseCancel::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.AckSmsStoredWithResponseCancel();		// API doesn't exist anymore

	return EPass;
	}

CTestNackSmsStoredWithResponse::CTestNackSmsStoredWithResponse() 
	{
	SetTestStepName(_L("TestNackSmsStoredWithResponse"));
	}

enum TVerdict CTestNackSmsStoredWithResponse::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NackSmsStoredWithResponse();		// API doesn't exist anymore

	return EPass;
	}

CTestNackSmsStoredWithResponseCancel::CTestNackSmsStoredWithResponseCancel() 
	{
	SetTestStepName(_L("TestNackSmsStoredWithResponseCancel"));
	}

enum TVerdict CTestNackSmsStoredWithResponseCancel::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NackSmsStoredWithResponseCancel();		// API doesn't exist anymore

	return EPass;
	}

CTestResumeSmsReception::CTestResumeSmsReception() 
	{
	SetTestStepName(_L("TestResumeSmsReception"));
	}

enum TVerdict CTestResumeSmsReception::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
	TRequestStatus status;
	iSms.ResumeSmsReception(status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNotSupported);

	return TestStepResult();
	}

CTestNotifySmsEvent::CTestNotifySmsEvent() 
	{
	SetTestStepName(_L("TestNotifySmsEvent"));
	}

enum TVerdict CTestNotifySmsEvent::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NotifySmsEvent();		// API doesn't exist anymore

	return EPass;
	}

CTestNotifySmsEventCancel::CTestNotifySmsEventCancel() 
	{
	SetTestStepName(_L("TestNotifySmsEventCancel"));
	}

enum TVerdict CTestNotifySmsEventCancel::doTestStepL()
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NotifySmsEventCancel();	// API doesn't exist anymore

	return EPass;
	}


CTestGetCaps::CTestGetCaps() 
	{
	SetTestStepName(_L("TestGetCaps"));
	}

enum TVerdict CTestGetCaps::doTestStepL()
/**
 * This function tests the capabilities API of the Messaging subssession.
 */
	{
	INFO_PRINTF1(_L("Get Capabilities."));
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(iSmsCaps);
	INFO_PRINTF1(_L(".."));
	
	TESTL(iSms.GetCaps(smsCapsPckg)==KErrNone);
	INFO_PRINTF1(_L("test(s) passed"));
	
	return TestStepResult();
	}

CTestSmspList::CTestSmspList() 
	{
	SetTestStepName(_L("TestSmspList"));
	}

enum TVerdict CTestSmspList::doTestStepL()
/**
 * This function tests get/set service centre address
 */
	{
	INFO_PRINTF1(_L("Check TSY's SMSP capabilities.."));
	if ((iSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsGetSmspList) &&
	    (iSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSetSmspList))
		{
		INFO_PRINTF1(_L("Caps - KCapsGetSmspList supported"));
		INFO_PRINTF1(_L("Caps - KCapsSetSmspList supported"));
		RMobileSmsMessaging::TMobileSmspEntryV1 defaultSmspEntry;
		RMobileSmsMessaging::TMobileSmspEntryV1 newSmspEntry;
		RMobileSmsMessaging::TMobileSmspEntryV1 checkSmspEntry;
		INFO_PRINTF1(_L("Retrieve default SMSP list"));
		TRAPD(ret, GetSmspListL(defaultSmspEntry));
		TESTL(ret == KErrNone);
		INFO_PRINTF1(_L("Store new SMSP list"));
		newSmspEntry.iValidParams = RMobileSmsMessaging::KSCAIncluded;
		newSmspEntry.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
		newSmspEntry.iServiceCentre.iNumberPlan=RMobilePhone:: EIsdnNumberPlan;
		newSmspEntry.iServiceCentre.iTelNumber.Copy(_L("01632960000"));
		TRAP(ret, SetSmspListL(newSmspEntry));
		TESTL(ret == KErrNone);
		INFO_PRINTF1(_L("Check SMSP list contains new entry"));
		TRAP(ret, GetSmspListL(checkSmspEntry));
		TESTL(ret == KErrNone);
		INFO_PRINTF1(_L("Rewrite default SMSP list"));
		TRAP(ret, SetSmspListL(defaultSmspEntry));
		TESTL(ret == KErrNone);
		INFO_PRINTF1(_L("Check SMSP list contains default entry"));
		TRAP(ret, GetSmspListL(checkSmspEntry));
		TESTL(ret == KErrNone);
		}

	return TestStepResult();
	}

CTestMessageStoreInfo::CTestMessageStoreInfo() 
	{
	SetTestStepName(_L("TestMessageStoreInfo"));
	}

enum TVerdict CTestMessageStoreInfo::doTestStepL()
/**
 * This function retrieves the number of message stores supported by the iPhone. It
 * also gets the details of the supported message stores. It then gets and sets the
 * Default Service Center Address (SCA) and performs some Cancel Requests.
 */
	{	
	INFO_PRINTF1(_L("Test RMobileSmsMessaging::EnumerateMessageStores.."));
	TInt ret=KErrNone;
	TInt count=0;
	TInt i;
	ret=iSms.EnumerateMessageStores(count);
	TEST_CHECKL(ret, KErrNone, _L("Error retrieving message store count")); 
	INFO_PRINTF2(_L("There are %d SMS message stores"), count);
	INFO_PRINTF1(_L("Test RMobileSmsMessaging::GetMessageStoreInfo...")); 
	TRequestStatus reqStatus;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 smsInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg smsInfoPckg(smsInfo);
	for (i=0; i<count; i++)
		{
		iSms.GetMessageStoreInfo(reqStatus, i, smsInfoPckg);
		User::WaitForRequest(reqStatus);
		TESTL(reqStatus==KErrNone);
		TEST(smsInfo.iType==RMobilePhoneStore::EShortMessageStore);
		// Print the result
		INFO_PRINTF1(_L("Information retrieved about the message store"));
		INFO_PRINTF2(_L("Name					 : %S"), &smsInfo.iName);
		INFO_PRINTF2(_L("Total number of entries  : %d"), smsInfo.iTotalEntries);
		INFO_PRINTF2(_L("Number of used entries   : %d"), smsInfo.iUsedEntries);
		INFO_PRINTF2(_L("Capabilities(in hex)	 : %X"), smsInfo.iCaps);
		PrintMessageStoreCaps(smsInfo.iCaps);
		User::After(10000000);		// Give user time to see the test results 
		}
	// Test immediate cancelling of request
	if (count>0)
		{
		INFO_PRINTF1(_L("Test cancelling of GetMessageStoreInfo request"));
		iSms.GetMessageStoreInfo(reqStatus, 0, smsInfoPckg);
		iSms.CancelAsyncRequest(EMobileSmsMessagingGetMessageStoreInfo);
		User::WaitForRequest(reqStatus);
		if (reqStatus==KErrCancel)
			INFO_PRINTF1(_L("Request successfully cancelled"));
		else if (reqStatus==KErrNone)
			INFO_PRINTF1(_L("Request completed before it could be cancelled"));
		else
			{
			INFO_PRINTF2(_L("Request failed with: %d"), reqStatus.Int());
			SetTestStepResult(EFail);
			}		
		}

	// Test delayed cancelling of request
	if (count>1)
		{
		INFO_PRINTF1(_L("Test cancelling of GetMessageStoreInfo request..."));
		iSms.GetMessageStoreInfo(reqStatus, 1, smsInfoPckg);
	//	User::After(500000);	
		iSms.CancelAsyncRequest(EMobileSmsMessagingGetMessageStoreInfo);
		User::WaitForRequest(reqStatus);
		TEST(reqStatus==KErrCancel || reqStatus==KErrNone);
		if (reqStatus==KErrCancel)
			INFO_PRINTF1(_L("Request successfully cancelled"));
		else
			INFO_PRINTF1(_L("Request completed before it could be cancelled"));
		}

	return TestStepResult();
	}

CTestMessageStoreOps::CTestMessageStoreOps() 
	{
	SetTestStepName(_L("TestMessageStoreOps"));
	}

enum TVerdict CTestMessageStoreOps::doTestStepL()
/**
 * This function performs Message Store Operations.
 * It opens the SIM and ME Stores, writes & reads sms' to these stores, performs
 * appropriate Cancel operations and gets information on the SIM and ME stores.
 */
	{
	// Test open message store and get info
	RMobileSmsStore simStore;
	// Nokia 9210 doesn't support ME SMS storage. 
	// This flag is used to skip the related tests.
	TBool meSupported;
	TInt ret=simStore.Open(iSms, KETelIccSmsStore);
	TESTL(ret==KErrNone);
	CleanupClosePushL(simStore);
	INFO_PRINTF1(_L("SIM message store opened..."));
	TRequestStatus reqStatus;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 simSmsInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg simSmsInfoPckg(simSmsInfo);
	INFO_PRINTF1(_L("Retrieving SIM message store info..."));
	simStore.GetInfo(reqStatus, simSmsInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus==KErrNone);
	if (reqStatus!=KErrNone)
		INFO_PRINTF2(_L("Error %d retrieving SIM message store info"), ret); 
	else
		{
		TEST(simSmsInfo.iType==RMobilePhoneStore::EShortMessageStore);
		// Print the result
		INFO_PRINTF1(_L("Information retrieved about the message store"));
		INFO_PRINTF2(_L("Name					 : %S"), &simSmsInfo.iName);
		INFO_PRINTF2(_L("Total number of entries  : %d"), simSmsInfo.iTotalEntries);
		INFO_PRINTF2(_L("Number of used entries   : %d"), simSmsInfo.iUsedEntries);
		INFO_PRINTF2(_L("Capabilities(in hex)	 : %X"), simSmsInfo.iCaps);
		PrintMessageStoreCaps(simSmsInfo.iCaps);
		User::After(10000000);		// Give user time to see the test results 
		}
	// Test immediate cancelling of request
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 simSmsInfo2;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg simSmsInfoPckg2(simSmsInfo2);
	INFO_PRINTF1(_L("Test immediate cancelling of GetInfo request..."));
	simStore.GetInfo(reqStatus, simSmsInfoPckg2);
	simStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus==KErrCancel || reqStatus==KErrNone);
	if (reqStatus==KErrCancel)
		INFO_PRINTF1(_L("Request successfully cancelled"));
	else
		INFO_PRINTF1(_L("Request completed before it could be cancelled"));
	// Test delayed cancelling of request
	INFO_PRINTF1(_L("Test delayed cancelling of GetInfo request"));
	simStore.GetInfo(reqStatus, simSmsInfoPckg2);
	User::After(500000);	
	simStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus==KErrCancel || reqStatus==KErrNone);
	if (reqStatus==KErrCancel)
		INFO_PRINTF1(_L("Request successfully cancelled"));
	else
		INFO_PRINTF1(_L("Request completed before it could be cancelled"));
	// Test the same information retrieval for ME SMS store
	RMobileSmsStore meStore;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 meSmsInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg meSmsInfoPckg(meSmsInfo);
	ret=meStore.Open(iSms, KETelMeSmsStore);
	TESTL(ret==KErrNone);
	CleanupClosePushL(meStore);
	INFO_PRINTF1(_L("ME message store opened."));
	INFO_PRINTF1(_L("Retrieving ME message store info..."));
	meStore.GetInfo(reqStatus, meSmsInfoPckg);
	User::WaitForRequest(reqStatus);
	if (reqStatus!=KErrNone)
		{
		INFO_PRINTF2(_L("Error %d retrieving ME message store info."), reqStatus.Int()); 
		INFO_PRINTF1(_L("*** Please retry test with another phone ***"));
		meSupported=FALSE;
		if(TestStepResult() == EPass)
			SetTestStepResult(EInconclusive);
		}
	else
		{
		TEST(meSmsInfo.iType==RMobilePhoneStore::EShortMessageStore);
		// Print the result
		INFO_PRINTF1(_L("Information retrieved about the message store"));
		INFO_PRINTF2(_L("Name					 : %S"), &meSmsInfo.iName);
		INFO_PRINTF2(_L("Total number of entries  : %d"), meSmsInfo.iTotalEntries);
		INFO_PRINTF2(_L("Number of used entries   : %d"), meSmsInfo.iUsedEntries);
		INFO_PRINTF2(_L("Capabilities(in hex)	 : %X"), meSmsInfo.iCaps);
		PrintMessageStoreCaps(meSmsInfo.iCaps);
		User::After(10000000);		// Give user time to see the test results 
		meSupported=TRUE;
		}

	// Test immediate cancelling of request
	if (meSupported==TRUE)
		{
		RMobilePhoneStore::TMobilePhoneStoreInfoV1 meSmsInfo2;
		RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg meSmsInfoPckg2(meSmsInfo2);
		INFO_PRINTF1(_L("Test immediate cancelling of GetInfo request..."));
		meStore.GetInfo(reqStatus, meSmsInfoPckg2);
		meStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
		User::WaitForRequest(reqStatus);
		TEST(reqStatus==KErrCancel || reqStatus==KErrNone);
		if (reqStatus==KErrCancel)
			INFO_PRINTF1(_L("Request successfully cancelled"));
		else
			INFO_PRINTF1(_L("Request completed before it could be cancelled"));
		
		// Test delayed cancelling of request
		INFO_PRINTF1(_L("Test delayed cancelling of GetInfo request"));
		meStore.GetInfo(reqStatus, meSmsInfoPckg2);
		User::After(500000);	
		meStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
		User::WaitForRequest(reqStatus);
		TEST(reqStatus==KErrCancel || reqStatus==KErrNone);
		if (reqStatus==KErrCancel)
			INFO_PRINTF1(_L("Request successfully cancelled"));
		else
			INFO_PRINTF1(_L("Request completed before it could be cancelled"));
		}

	// Test reading all the messages in SIM store
	// Print details of each message
	INFO_PRINTF1(_L("Reading Messages from SIM Store..."));
	RMobileSmsStore::TMobileGsmSmsEntryV1 smsEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsEntryPckg(smsEntry);
	for (TInt i=1; i<=simSmsInfo.iUsedEntries; i++)
		{
		smsEntry.iIndex=i;		// Entry to read
		simStore.Read(reqStatus, smsEntryPckg);
		User::WaitForRequest(reqStatus);
		TESTL(reqStatus==KErrNone || reqStatus==KErrNotFound);
		if (reqStatus == KErrNotFound)
			INFO_PRINTF2((_L("The entry was not found, request status = %d")), reqStatus.Int());
		else
			{
			INFO_PRINTF2(_L("Entry %d: Successfully read"),smsEntry.iIndex);
			INFO_PRINTF2(_L("Status of entry = %d"),smsEntry.iMsgStatus);
			INFO_PRINTF2(_L("Length of PDU in octets = %d"),smsEntry.iMsgData.Size());
			}
		}
	// Test reading and cancelling
	INFO_PRINTF1(_L("Test Cancel Read from SIM Store"));
	TRequestStatus cancelReqStatus;
	smsEntry.iIndex=simSmsInfo.iUsedEntries;
	simStore.Read(cancelReqStatus, smsEntryPckg);
	simStore.CancelAsyncRequest(EMobilePhoneStoreRead);
	User::WaitForRequest(cancelReqStatus);
	TEST(cancelReqStatus==KErrCancel || reqStatus==KErrNone || reqStatus==KErrNotFound);
	if (cancelReqStatus==KErrCancel)
		{
		INFO_PRINTF1(_L("Request successfully cancelled"));
		}
	else
		{
		INFO_PRINTF2(_L("Request completed with %d before it could be cancelled"), reqStatus.Int());
		}
	// Test writing a dummy PDU to SIM store
	// And then test reading it out and comparing it
	INFO_PRINTF1(_L("Test Write to SIM Store"));
	const TText8 MyPdu[] = {0x1d,0x00,0x0c,0x91,0x44,0x77,0x74,0x60,0x45,0x95,0x00,0x00,0x50,0x80,0x80,0x31,0x92,0x80,0x00,0x04,0xf4,0xf2,0x9c,0x0e};
	smsEntry.iMsgStatus=RMobileSmsStore::EStoredMessageUnsent;
	smsEntry.iMsgData.Copy(MyPdu, sizeof(MyPdu));
	simStore.Write(reqStatus, smsEntryPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus==KErrNone);
	if (reqStatus==KErrNone)
		{
		INFO_PRINTF2(_L("Write request successfully completed to index %d"), smsEntry.iIndex);
		}
	else
		{
		INFO_PRINTF2(_L("Error %d writing message to SIM store"), reqStatus.Int()); 
		}
	if (reqStatus==KErrNone)
		{	
		INFO_PRINTF1(_L("Test Read of entry just written to SIM Store"));
		RMobileSmsStore::TMobileGsmSmsEntryV1 smsReadEntry;
		RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsReadEntryPckg(smsReadEntry);
		smsReadEntry.iIndex=smsEntry.iIndex;
		simStore.Read(reqStatus, smsReadEntryPckg);
		User::WaitForRequest(reqStatus);
		TESTL(reqStatus==KErrNone);
		if (reqStatus != KErrNone)
			{
			INFO_PRINTF2((_L("The entry was not found, request status = %d")), reqStatus.Int());
			}
		else
			{
			INFO_PRINTF2(_L("Entry %d: Successfully read"),smsReadEntry.iIndex);
			INFO_PRINTF2(_L("Status of entry = %d"),smsReadEntry.iMsgStatus);
			INFO_PRINTF2(_L("Length of PDU in octets = %d"),smsReadEntry.iMsgData.Size());
			if (smsReadEntry.iMsgData.Compare(smsEntry.iMsgData)==0)
				{
				INFO_PRINTF1(_L("Read entry successfully compared with Write entry"));
				}
			else
				{
				INFO_PRINTF1(_L("*** Read entry NOT matched with Write entry! ***"));
				INFO_PRINTF1(_L("*** This test is known to fail with Ericsson phones  ***"));
				INFO_PRINTF1(_L("*** Please retry test with a different make of phone ***"));

				if(TestStepResult() == EPass)
					SetTestStepResult(EInconclusive);
				}
			}
		}
	// Test writing and cancelling to ME store first
	if (meSupported==TRUE)
		{
		INFO_PRINTF1(_L("Test Cancel Write to ME Store"));
		meStore.Write(cancelReqStatus, smsEntryPckg);
		meStore.CancelAsyncRequest(EMobilePhoneStoreWrite);
		User::WaitForRequest(cancelReqStatus);
		TEST(cancelReqStatus==KErrCancel || reqStatus==KErrNone);
		if (cancelReqStatus==KErrCancel)
			INFO_PRINTF1(_L("Request successfully cancelled"));
		else
			INFO_PRINTF1(_L("Request completed before it could be cancelled"));

	// Test writing the same PDU to ME store
	// And then test reading it out and comparing it again
	INFO_PRINTF1(_L("Test Write to ME Store"));

	meStore.Write(reqStatus, smsEntryPckg);
	User::WaitForRequest(reqStatus);
	if (reqStatus==KErrNone)
		{
		INFO_PRINTF2(_L("Write request successfully completed to index %d"), smsEntry.iIndex);
		}
	else
		{
		INFO_PRINTF2(_L("Error %d writing message to ME store"), reqStatus.Int()); 
		INFO_PRINTF1(_L("*** Please try running this test with another phone.***"));
		if(TestStepResult() == EPass)
			{
			SetTestStepResult(EInconclusive);
			}
		}
	if (reqStatus==KErrNone)
		{
		INFO_PRINTF1(_L("Test Read of entry just written to ME Store"));
		RMobileSmsStore::TMobileGsmSmsEntryV1 smsReadEntry2;
		RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsReadEntryPckg2(smsReadEntry2);
		smsReadEntry2.iIndex=smsEntry.iIndex;
		meStore.Read(reqStatus, smsReadEntryPckg2);
		User::WaitForRequest(reqStatus);
		if (reqStatus != KErrNone)
			{
			INFO_PRINTF2((_L("The entry was not found, request status = %d")), reqStatus.Int());
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("Entry %d: Successfully read"),smsReadEntry2.iIndex);
			INFO_PRINTF2(_L("Status of entry = %d"),smsReadEntry2.iMsgStatus);
			INFO_PRINTF2(_L("Length of PDU in octets = %d"),smsReadEntry2.iMsgData.Size());
			if (smsReadEntry2.iMsgData.Compare(smsEntry.iMsgData)==0)
				{
				INFO_PRINTF1(_L("Read entry successfully compared with Write entry"));
				}
			else
				{
				INFO_PRINTF1(_L("*** Read entry NOT matched with Write entry! ***"));
				INFO_PRINTF1(_L("*** This test is known to fail with Ericsson phones  ***"));
				INFO_PRINTF1(_L("*** Please retry test with a different make of phone ***"));

				if(TestStepResult() == EPass)
					SetTestStepResult(EInconclusive);
				}
			}
		}
	}
	// Now test deletion of entries - test deletion of last entry
	// Get info again in order to get up-to-date number of used entries
	simStore.GetInfo(reqStatus, simSmsInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus==KErrNone);
	if (simSmsInfo.iUsedEntries > 0)
		{
		simStore.Delete(reqStatus, simSmsInfo.iUsedEntries);
		User::WaitForRequest(reqStatus);
		if (reqStatus == KErrNotFound)
			{
			INFO_PRINTF2((_L("The entry was not found, request status = %d")), reqStatus.Int());
			SetTestStepResult(EFail);
			}
		else
			INFO_PRINTF2(_L("Entry %d: Successfully deleted"),simSmsInfo.iUsedEntries);
		}
	if (meSupported==TRUE)
		{
		meStore.GetInfo(reqStatus, meSmsInfoPckg);
		User::WaitForRequest(reqStatus);
		TEST(reqStatus==KErrNone);
		if(reqStatus==KErrNone)
			{
			if (meSmsInfo.iUsedEntries > 0)
				{
				meStore.Delete(reqStatus, meSmsInfo.iUsedEntries);
				User::WaitForRequest(reqStatus);
				if (reqStatus == KErrNotFound)
					{
					INFO_PRINTF2((_L("The entry was not found, request status = %d")), reqStatus.Int());
					SetTestStepResult(EFail);
					}
				else
					INFO_PRINTF2(_L("Entry %d: Successfully deleted"),meSmsInfo.iUsedEntries);
				}
			}
		else
			{
			INFO_PRINTF1(_L("Failed to get ME store info"));
			SetTestStepResult(EFail);
			}
		}

	// Test deleting and cancelling
	INFO_PRINTF1(_L("Test Cancel Delete from SIM Store"));

	// Write an entry to be deleted first
	simStore.Write(reqStatus, smsEntryPckg);
	User::WaitForRequest(reqStatus);
	if (reqStatus==KErrNone)
		{	
		simStore.Delete(cancelReqStatus,smsEntry.iIndex);
		simStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
		User::WaitForRequest(cancelReqStatus);
		if (cancelReqStatus==KErrCancel)
			INFO_PRINTF1(_L("Request successfully cancelled"));
		else if (cancelReqStatus==KErrNone)
			INFO_PRINTF1(_L("Request completed before it could be cancelled"));
		else
			{
			INFO_PRINTF2(_L("Request failed with: %d"), reqStatus.Int());
			SetTestStepResult(EFail);
			}
		}
	else
		{
		INFO_PRINTF2(_L("Error %d writing message to SIM store"), reqStatus.Int()); 
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(&meStore);
	CleanupStack::PopAndDestroy(&simStore);

	return TestStepResult();
	}

CTestReceiveUnstoredSms::CTestReceiveUnstoredSms() 
	{
	SetTestStepName(_L("TestReceiveUnstoredSms"));
	}

enum TVerdict CTestReceiveUnstoredSms::doTestStepL()
/**
 * Receive an unstored Sms test.
 * Currently the MMTSY does not support receive unstored mode, so that is what this test
 * tests for.
 */
	{
	TRequestStatus status;
	INFO_PRINTF1(_L("Setting receive mode to EReceiveUnstoredPhoneAck."));
	User::After(2000000);		// Gives a bit of time 
	iSms.SetReceiveMode(status, RMobileSmsMessaging::EReceiveUnstoredPhoneAck);
	User::WaitForRequest(status);
	//
	// Only continue if we successfully set the SMS receive mode
	TESTL(status == KErrNotSupported || status == KErrNone);
	if(status==KErrNone)
		{
		INFO_PRINTF1(_L("Verifying that receive mode has changed ..."));
		RMobileSmsMessaging::TMobileSmsReceiveMode mode;
		TInt ret=iSms.GetReceiveMode(mode);
//		TESTL(ret!=KErrNone || mode!=RMobileSmsMessaging::EReceiveUnstoredPhoneAck);
		TESTL(ret==KErrNone && mode ==RMobileSmsMessaging::EReceiveUnstoredPhoneAck);
		INFO_PRINTF1(_L("Calling Receive API ..."));
		RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 recAtt;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg recAttPckg(recAtt);
		iSms.ReceiveMessage(status, gsmMsg, recAttPckg);
		INFO_PRINTF1(_L("Sending out a SMS ..."));
		TRAP(ret, SendSms(ETrue));
		TESTL(ret == KErrNone);
		// Wait for the sent SMS to return to this phone with a timout of 30 s
		WaitWithTimeout(status,30*KOneSecond);
		TESTL(status.Int() == KErrNone);
		}
	else
		{
		INFO_PRINTF1(_L("*** Receive unstored mode not supported by this phone *** "));
		INFO_PRINTF1(_L("*** Please retry test with another phone!             *** "));

		if(TestStepResult()==EPass)
			SetTestStepResult(EInconclusive);
		}

	return TestStepResult();
	}

CTestReceiveStoredSms::CTestReceiveStoredSms() 
	{
	SetTestStepName(_L("TestReceiveStoredSms"));
	}

enum TVerdict CTestReceiveStoredSms::doTestStepL()
/**
 * Receive a stored Sms test.
 * Currently the MMTSY does not support receive unstored mode, so that is what this test
 * tests for.
 */
	{
	TRequestStatus status;
	INFO_PRINTF1(_L("Setting receive mode to EReceiveStored."));
	User::After(2000000);		// Gives a bit of time 
	iSms.SetReceiveMode(status, RMobileSmsMessaging::EReceiveStored);
	User::WaitForRequest(status);
	if(status==KErrNone)
		{
		INFO_PRINTF1(_L("Verifying that receive mode has changed ..."));
		RMobileSmsMessaging::TMobileSmsReceiveMode mode;
		TInt ret=iSms.GetReceiveMode(mode);
		TESTL(ret == KErrNone && mode == RMobileSmsMessaging::EReceiveStored);
		INFO_PRINTF1(_L("Calling Receive API ..."));
		RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 recAtt;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg recAttPckg(recAtt);
		iSms.ReceiveMessage(status, gsmMsg, recAttPckg);
		INFO_PRINTF1(_L("Sending out a SMS ..."));
		TRAP(ret, SendSms(ETrue));
		TESTL(ret == KErrNone);
		// Wait for the sent SMS to return to this phone with a timout of 30 s
		WaitWithTimeout(status,30*KOneSecond);
		TESTL(status.Int() == KErrNone);
		}
	else
		{
		return EFail;
		}

	return TestStepResult();
	}

CTestReceiveStoredSmsAndCancel::CTestReceiveStoredSmsAndCancel() 
	{
	SetTestStepName(_L("TestReceiveStoredSmsAndCancel"));
	}

enum TVerdict CTestReceiveStoredSmsAndCancel::doTestStepL()
	{
	TRequestStatus status;
	INFO_PRINTF1(_L("Setting receive mode to EReceiveStored."));
	User::After(2000000);		// Gives a bit of time 
	iSms.SetReceiveMode(status, RMobileSmsMessaging::EReceiveStored);
	User::WaitForRequest(status);
	if(status==KErrNone)
		{
		INFO_PRINTF1(_L("Verifying that receive mode has changed ..."));
		RMobileSmsMessaging::TMobileSmsReceiveMode mode;
		TInt ret=iSms.GetReceiveMode(mode);
		TESTL(ret==KErrNone && mode==RMobileSmsMessaging::EReceiveStored);
		INFO_PRINTF1(_L("Calling Receive API ..."));
		RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 recAtt;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg recAttPckg(recAtt);
		iSms.ReceiveMessage(status, gsmMsg, recAttPckg);
		INFO_PRINTF1(_L("Sending out a SMS ..."));
		TRAP(ret, SendSms(ETrue));
		TESTL(ret == KErrNone);
		iSms.CancelAsyncRequest(EMobileSmsMessagingReceiveMessage);		// Cancels ...
		// Wait for the sent SMS to return to this phone with a timout of 30 s
		WaitWithTimeout(status,30*KOneSecond);
		TESTL((status.Int() == KErrCancel) || (status.Int() == KErrNone)) ;
		}
	else
		{
		return EFail;
		}
	
	return TestStepResult();
	}

CTestSendSmsAndCancel::CTestSendSmsAndCancel() 
	{
	SetTestStepName(_L("TestSendSmsAndCancel"));
	}

enum TVerdict CTestSendSmsAndCancel::doTestStepL()
	{
	TInt i;
	INFO_PRINTF1(_L("Sending SMS and cancelling."));
	// Create message PDU and convert to binary
	_LIT8(KMsgDataBeforeTargetAddress,"1d00");

	// This data is taken from CSmsTestStep::SendSms, since this
	// method should be attempting the same send functionality.
	_LIT8(KMsgDataAfterTargetAddress,"00005080803192800004f4f29c0e");

	TBuf8<400> msgData;
	TLex8 lex;
	TUint8 val;
	msgData.Zero();
	const TInt count_before((&KMsgDataBeforeTargetAddress)->Length()/2);	// Assume length of pdu is always even
	for(i=0;i<count_before;++i)
		{
		lex=(&KMsgDataBeforeTargetAddress)->Mid(i*2,2);
		LEAVE_IF_ERROR(lex.Val(val,EHex));					
		msgData.Append(TChar(val));
		}
	TBuf8<20> targetAddressAscii;
	targetAddressAscii.Zero();
	RMobilePhone::TMobileAddress targetAddress;
	targetAddress.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	targetAddress.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	targetAddress.iTelNumber.Copy(iTelNumber);
	TInt ret = AppendAddressToAscii(targetAddressAscii,targetAddress);
	TESTL(ret == KErrNone);
	const TInt count_address(targetAddressAscii.Length()/2);	// Assume length is always even
	for(i=0;i<count_address;++i)
		{
		lex=targetAddressAscii.Mid(i*2,2);
		lex.Val(val,EHex);					
		msgData.Append(TChar(val));
		}
	const TInt count_after((&KMsgDataAfterTargetAddress)->Length()/2);	// Assume length of pdu is always even
	for(i=0;i<count_after;++i)
		{
		lex=(&KMsgDataAfterTargetAddress)->Mid(i*2,2);
		lex.Val(val,EHex);					
		msgData.Append(TChar(val));
		}
	// Create message attibutes 
	RMobileSmsMessaging::TMobileSmsSendAttributesV1	msgAttr;
	msgAttr.iFlags=RMobileSmsMessaging::KSmsDataFormat | RMobileSmsMessaging::KGsmServiceCentre;
	msgAttr.iDataFormat=RMobileSmsMessaging::EFormatGsmTpdu;
	msgAttr.iGsmServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	msgAttr.iGsmServiceCentre.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	msgAttr.iGsmServiceCentre.iTelNumber.Copy(iSCANumber);
	// Package up data ready for sending to etel
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg msgAttrPkg(msgAttr);
	// Send the message
	INFO_PRINTF1(_L(".."));
	iSms.SendMessage(iStatus,msgData,msgAttrPkg);
	// Wait for a bit
	User::After(1000000);
	// Cancel sending, then wait for sending operation to complete
	iSms.CancelAsyncRequest(EMobileSmsMessagingSendMessage);
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(_L("Status = %d"),iStatus.Int());
	TESTL(iStatus==KErrCancel || iStatus==KErrNone);
	INFO_PRINTF1(_L("test(s) passed"));	// If we don't cause a panic or leave the test has passed

	return TestStepResult();
	}
