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
// This program is designed the test of the .
// 
//

/**
 @file
*/

#include "Te_SimSms.h"
#include "Te_SimSmsUtil.h"		// for catsmsutils


//------------------------------------------------------------------
//			** TEST SimSms STEPS ***

//------------------------------------------------------------------

CTestUtilClass::CTestUtilClass()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestUtilClass"));
	}
//------------------------------------------------------------------

TVerdict CTestUtilClass::doTestStepL( void )
/**
 * This functions tests most of CATSmsUtils class 
 */
	{
	INFO_PRINTF1(_L("CATSmsUtils."));
	TestUtilClass_Step1L();
	TestUtilClass_Step2L();
	TestUtilClass_Step3L();
	TestUtilClass_Step3point1L();
	TestUtilClass_Step4L();
	TestUtilClass_Step5L();
	INFO_PRINTF1(_L("TESTL(s) passed"));
	return TestStepResult();
	}

void CTestUtilClass::TestUtilClass_Step1L()
	{
	TBuf8<400> buf;

	//
	// Test AppendAddress
	//
	RMobilePhone::TMobileAddress addr;

	buf.Zero();
	addr.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("44 7785 014 208"));
	
	INFO_PRINTF1(_L(".."));
	TESTL(KErrNone == CATSmsUtils::AppendAddressToAscii(buf,addr));

	if(buf.CompareF(_L8("0c91447758102480"))!=0)
		TESTL(KErrCorrupt);
		
	addr.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("01632960000"));

	buf.Zero();	
	TESTL(KErrNone == CATSmsUtils::AppendAddressToAscii(buf,addr));
	if(buf.CompareF(_L8("0b914483056100f5"))!=0)
		TESTL(KErrCorrupt);

	addr.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("# * 44 77 85 01 60 05"));

	buf.Zero();	
	TESTL(KErrNone == CATSmsUtils::AppendAddressToAscii(buf,addr));
	if(buf.CompareF(_L8("0e91ab01632960000"))!=0)
		TESTL(KErrCorrupt);
	}

void CTestUtilClass::TestUtilClass_Step2L()
	{
	//
	// Test AppendBinaryDataToAscii
	//

	TBuf8<10> testPdu;
	testPdu.Append(0x00);
	testPdu.Append(0x10);
	testPdu.Append(0x20);
	testPdu.Append(0x40);
	testPdu.Append(0xf3);
	testPdu.Append(0x7b);
	testPdu.Append(0x99);
	testPdu.Append(0x5e);
	testPdu.Append(0x01);
	testPdu.Append(0x02);

	TBuf8<400> buf;
	buf.Zero();
	INFO_PRINTF1(_L(".."));
	CATSmsUtils::AppendDataToAscii(buf,testPdu);

	if(buf.Compare(_L8("00102040f37b995e0102"))!=0)
		TESTL(KErrCorrupt);

	TBuf8<10> testPdu2;

	INFO_PRINTF1(_L(".."));
	TESTL(KErrNone == CATSmsUtils::ConvertAsciiToBinary(buf,testPdu2));
	if(testPdu.Compare(testPdu2)!=0)
		TESTL(KErrCorrupt);
	}

void CTestUtilClass::TestUtilClass_Step3L()
	{
	//
	// Test removal of SCA from a PDU
	//

	// Create a PDU
	RMobilePhone::TMobileAddress addr;
	addr.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("# * 44 7785 016 005"));

	TBuf8<400> buf;
	buf.Zero();	
	TESTL(KErrNone == CATSmsUtils::AppendAddressToAscii(buf,addr));
	if(buf.CompareF(_L8("0e91ab01632960000"))!=0)
		TESTL(KErrCorrupt);

	_LIT8(KDummyData,"dummy data");		// must have even number of chars
	buf.Append(KDummyData);

	// fill data with garbage
	addr.iTypeOfNumber=RMobilePhone::EAbbreviatedNumber;
	addr.iNumberPlan=RMobilePhone::EPrivateNumberPlan;
	addr.iTelNumber.Copy(KTelNumberGarbage);

	// Get SCA from PDU and validate the output
	INFO_PRINTF1(_L(".."));
	TESTL(KErrNone == CATSmsUtils::ReadAndRemoveAddressFromAscii(buf,addr));
	if(addr.iTypeOfNumber!=RMobilePhone::EInternationalNumber ||
	   addr.iNumberPlan!=RMobilePhone::EIsdnNumberPlan || 
	   addr.iTelNumber.Compare(_L("#*01632960000"))!=0 ||
	   buf.Compare(KDummyData)!=0)
		TESTL(KErrCorrupt);
	}

void CTestUtilClass::TestUtilClass_Step3point1L()
	{
	//
	// Test reading of SCA form a PDU (no removal)
	//

	// Create a PDU
	RMobilePhone::TMobileAddress addr;
	addr.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("44 7785 016 005"));

	TBuf8<400> buf;
	buf.Zero();	
	TESTL(KErrNone == CATSmsUtils::AppendAddressToAscii(buf,addr));
	if(buf.CompareF(_L8("0c91447758100650"))!=0)
		TESTL(KErrCorrupt);

	// fill data with garbage
	addr.iTypeOfNumber=RMobilePhone::ESubscriberNumber;
	addr.iNumberPlan=RMobilePhone::EDataNumberPlan;
	addr.iTelNumber.Copy(KTelNumberGarbage);

	// Get SCA from PDU and validate the output
	INFO_PRINTF1(_L(".."));
	TESTL(KErrNone == CATSmsUtils::ReadAddressFromAscii(buf,addr));
	if(addr.iTypeOfNumber!=RMobilePhone::EInternationalNumber ||
	   addr.iNumberPlan!=RMobilePhone::EIsdnNumberPlan || 
	   addr.iTelNumber.Compare(_L("01632960000"))!=0 ||
	   buf.Length()!=16)
		TESTL(KErrCorrupt);

	// Create a PDU
	addr.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("44 7785 016 005"));

	buf.Zero();	
	TESTL(KErrNone == CATSmsUtils::AppendAddressToAscii(buf,addr));
	if(buf.CompareF(_L8("0c91447758100650"))!=0)
		TESTL(KErrCorrupt);

	// Modify PDU so it uses 03.40 SCA length format
	buf[1]=static_cast<TUint8>(TChar('7'));

	// fill data with garbage
	addr.iTypeOfNumber=RMobilePhone::ESubscriberNumber;
	addr.iNumberPlan=RMobilePhone::EDataNumberPlan;
	addr.iTelNumber.Copy(KTelNumberGarbage);

	// Get SCA from PDU (specifying PDU is in 03.40 format) and validate the output
	INFO_PRINTF1(_L(".."));
	TESTL(KErrNone == CATSmsUtils::ReadAddressFromAscii(buf,addr,ETrue));
	if(addr.iTypeOfNumber!=RMobilePhone::EInternationalNumber ||
	   addr.iNumberPlan!=RMobilePhone::EIsdnNumberPlan || 
	   addr.iTelNumber.Compare(_L("01632960000"))!=0 ||
	   buf.Length()!=16)
		TESTL(KErrCorrupt);
	}


void CTestUtilClass::TestUtilClass_Step4L()
	{
	//
	// Test ParseRxResultsForCSCAResponse
	//
	// fill data with garbage
	RMobilePhone::TMobileAddress addr;
	addr.iTypeOfNumber=RMobilePhone::ESubscriberNumber;
	addr.iNumberPlan=RMobilePhone::EDataNumberPlan;
	addr.iTelNumber.Copy(_L("000111000111000111000111"));

	// setup a response to be parsed
	_LIT8(KAddressValue,"01632960000");
	_LIT8(KAddressType,"129");
	TPtrC8 addressValue(KAddressValue);
	TPtrC8 addressType(KAddressType);

	// Parse reponse and validate output
	INFO_PRINTF1(_L(".."));
	TESTL(KErrNone == CATSmsUtils::CopyAddressStringToAddressStruct(addressValue,addressType,addr));
	if(addr.iTypeOfNumber!=RMobilePhone::EUnknownNumber ||
	   addr.iNumberPlan!=RMobilePhone::EIsdnNumberPlan || 
	   addr.iTelNumber.Compare(_L("01632960000"))!=0)
		TESTL(KErrCorrupt);
	}

void CTestUtilClass::TestUtilClass_Step5L()
	{

	//
	// Test conversion of Type-Of-Adddress to a decimal
	//
	RMobilePhone::TMobileAddress addr;
	addr.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("44 7785 016 005"));
	TUint val;
	INFO_PRINTF1(_L(".."));
	CATSmsUtils::GetTypeOfAddressInDecimal(addr,val);
	if(val!=145)
		TESTL(KErrCorrupt);
	addr.iTypeOfNumber=RMobilePhone::EUnknownNumber;
	addr.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	addr.iTelNumber.Copy(_L("44 7785 016 005"));
	INFO_PRINTF1(_L(".."));
	CATSmsUtils::GetTypeOfAddressInDecimal(addr,val);
	if(val!=129)
		TESTL(KErrCorrupt);
	}


//------------------------------------------------------------------

CGetCaps::CGetCaps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetCaps"));
	}
//------------------------------------------------------------------

TVerdict CGetCaps::doTestStepL( void )
/**
 * This function tests the capabilities API of the Messaging subssession.
 */
	{
	INFO_PRINTF1(_L("Get Capabilities."));
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(iSmsCaps);
	TESTL(KErrNone == iSms.GetCaps(smsCapsPckg));

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestSmspList::CTestSmspList()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSmspList"));
	}
//------------------------------------------------------------------

TVerdict CTestSmspList::doTestStepL( void )
/**
 * This function tests get/set service centre address
 */
	{
	INFO_PRINTF1(_L("Check TSY's SMSP capabilities.."));
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(iSmsCaps);
	TESTL(KErrNone == iSms.GetCaps(smsCapsPckg));

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
		if(ret)
			return EFail;
		INFO_PRINTF1(_L("Store new SMSP list"));
		newSmspEntry.iValidParams = RMobileSmsMessaging::KSCAIncluded;
		newSmspEntry.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
		newSmspEntry.iServiceCentre.iNumberPlan=RMobilePhone:: EIsdnNumberPlan;
		newSmspEntry.iServiceCentre.iTelNumber.Copy(_L("01632960000"));
		TRAP(ret, SetSmspListL(newSmspEntry));
		if(ret)
			return EFail;
		INFO_PRINTF1(_L("Check SMSP list contains new entry"));
		TRAP(ret, GetSmspListL(checkSmspEntry));
		if(ret)
			return EFail;

		INFO_PRINTF1(_L("Rewrite default SMSP list"));
		TRAP(ret, SetSmspListL(defaultSmspEntry));
		if(ret)
			return(EFail);
		INFO_PRINTF1(_L("Check SMSP list contains default entry"));
		TRAP(ret, GetSmspListL(checkSmspEntry));
		if(ret)
			return EFail;
		}
	return TestStepResult();
	}

void CTestSmspList::GetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry)
/**
 * This function tests CRetrieveMobilePhoneSmspList
 */
	{
	CTestGetSmspList* retrieveSmspList;
	CMobilePhoneSmspList* smspList;
	
	retrieveSmspList = CTestGetSmspList::NewLC(iSms);
	retrieveSmspList->Start();
	CActiveScheduler::Start();
	smspList = retrieveSmspList->RetrieveListL();
	if(!smspList)
		return;		// Ooooooooooooops ;-(


	TInt count = smspList->Enumerate();
	INFO_PRINTF2(_L("SMSP list has %d entries"), count);

	TInt i;
	for (i=0; i<count; i++)
		{
		aSmspEntry = smspList->GetEntryL(i);
		INFO_PRINTF3(_L("Entry %d: Service Centre = >%S<"),i,&aSmspEntry.iServiceCentre.iTelNumber);
		}

	CleanupStack::PopAndDestroy(); // retrieveSmspList;
	delete smspList;
	User::After(1000000);		// Give user time to see the test results 
	}


void CTestSmspList::SetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry)
/**
 * This function tests writing service centre address
 */
	{
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	smspList->AddEntryL(aSmspEntry);

	TRequestStatus reqStatus;

	iSms.StoreSmspListL(reqStatus, smspList);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	CleanupStack::PopAndDestroy(); // smspList;
	}

//------------------------------------------------------------------

CMessageStoreInfo::CMessageStoreInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("MessageStoreInfo"));
	}
//------------------------------------------------------------------

TVerdict CMessageStoreInfo::doTestStepL( void )
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
	if (!ret)
	{
		INFO_PRINTF2(_L("Error %d retrieving message store count"), ret); 
	}
	else
	{
		INFO_PRINTF2(_L("There are %d SMS message stores"), count);
	}

	INFO_PRINTF1(_L("Test RMobileSmsMessaging::GetMessageStoreInfo...")); 

	TRequestStatus reqStatus;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 smsInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg smsInfoPckg(smsInfo);

	for (i=0; i<count; i++)
		{
		iSms.GetMessageStoreInfo(reqStatus, i, smsInfoPckg);
		User::WaitForRequest(reqStatus);

		if (reqStatus!=KErrNone)
			{
			INFO_PRINTF2(_L("Error %d retrieving message store info"), ret); 
			break;
			}

		TESTL(smsInfo.iType==RMobilePhoneStore::EShortMessageStore);

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
	INFO_PRINTF1(_L("Test cancelling of GetMessageStoreInfo request"));
	iSms.GetMessageStoreInfo(reqStatus, 0, smsInfoPckg);
	iSms.CancelAsyncRequest(EMobileSmsMessagingGetMessageStoreInfo);
	User::WaitForRequest(reqStatus);

	if (reqStatus==KErrCancel)
	{
		INFO_PRINTF1(_L("Request successfully cancelled"));
	}
	else
	{
		INFO_PRINTF1(_L("Request NOT cancelled"));
	}

	// Test delayed cancelling of request
	INFO_PRINTF1(_L("Test cancelling of GetMessageStoreInfo request"));
	iSms.GetMessageStoreInfo(reqStatus, 1, smsInfoPckg);
//	User::After(500000);	
	iSms.CancelAsyncRequest(EMobileSmsMessagingGetMessageStoreInfo);
	User::WaitForRequest(reqStatus);

	if (reqStatus==KErrCancel)
	{
		INFO_PRINTF1(_L("Request successfully cancelled"));
	}
	else
	{
		INFO_PRINTF1(_L("Request NOT cancelled"));
	}

	return TestStepResult();
	}
//------------------------------------------------------------------

CMessageStoreOps::CMessageStoreOps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("MessageStoreOps"));
	}
//------------------------------------------------------------------

TVerdict CMessageStoreOps::doTestStepL( void )
/**
 * This function performs Message Store Operations.
 * It opens the SIM and ME Stores, writes & reads sms' to these stores, performs
 * appropriate Cancel operations and gets information on the SIM and ME stores.
 */
	{
	// Test open message store and get info

	RMobileSmsStore simStore;

	TInt ret=simStore.Open(iSms, KETelIccSmsStore);
	TESTL(ret==KErrNone);
	INFO_PRINTF1(_L("SIM message store opened..."));

	TRequestStatus reqStatus;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 simSmsInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg simSmsInfoPckg(simSmsInfo);

	simStore.GetInfo(reqStatus, simSmsInfoPckg);
	User::WaitForRequest(reqStatus);
	if (reqStatus!=KErrNone)
	{
		INFO_PRINTF2(_L("Error %d retrieving SIM message store info"), ret); 
	}
	else
		{
		TESTL(simSmsInfo.iType==RMobilePhoneStore::EShortMessageStore);

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

	INFO_PRINTF1(_L("Test immediate cancelling of GetInfo request"));
	simStore.GetInfo(reqStatus, simSmsInfoPckg2);
	simStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);

	if (reqStatus==KErrCancel)
	{
		INFO_PRINTF1(_L("Request successfully cancelled"));
	}
	else
	{
		INFO_PRINTF1(_L("Request NOT cancelled"));
	}

	// Test delayed cancelling of request
	INFO_PRINTF1(_L("Test delayed cancelling of GetInfo request"));
	simStore.GetInfo(reqStatus, simSmsInfoPckg2);
	User::After(500000);	
	simStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);

	if (reqStatus==KErrCancel)
	{
		INFO_PRINTF1(_L("Request successfully cancelled"));
	}
	else
	{
		INFO_PRINTF1(_L("Request NOT cancelled"));
	}

	// Test the same information retrieval for ME SMS store
	RMobileSmsStore meStore;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 meSmsInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg meSmsInfoPckg(meSmsInfo);

	ret=meStore.Open(iSms, KETelMeSmsStore);
	if(ret != KErrNone)		// If KETelMeSmsStore does not exist, re-open previous store (KETelIccSmsStore)
		ret=meStore.Open(iSms, KETelIccSmsStore);
	
	INFO_PRINTF1(_L("ME message store opened..."));

	meStore.GetInfo(reqStatus, meSmsInfoPckg);
	User::WaitForRequest(reqStatus);
	if (reqStatus!=KErrNone)
	{
		INFO_PRINTF2(_L("Error %d retrieving SIM message store info"), ret); 
	}
	else
		{
		TESTL(meSmsInfo.iType==RMobilePhoneStore::EShortMessageStore);

		// Print the result
		INFO_PRINTF1(_L("Information retrieved about the message store"));
		INFO_PRINTF2(_L("Name					 : %S"), &meSmsInfo.iName);
		INFO_PRINTF2(_L("Total number of entries  : %d"), meSmsInfo.iTotalEntries);
		INFO_PRINTF2(_L("Number of used entries   : %d"), meSmsInfo.iUsedEntries);
		INFO_PRINTF2(_L("Capabilities(in hex)	 : %X"), meSmsInfo.iCaps);
		PrintMessageStoreCaps(meSmsInfo.iCaps);
		User::After(10000000);		// Give user time to see the test results 
		}

	// Test immediate cancelling of request
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 meSmsInfo2;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg meSmsInfoPckg2(meSmsInfo2);
	INFO_PRINTF1(_L("Test immediate cancelling of GetInfo request"));
	meStore.GetInfo(reqStatus, meSmsInfoPckg2);
	meStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);

	if (reqStatus==KErrCancel)
	{
		INFO_PRINTF1(_L("Request successfully cancelled"));
	}
	else
	{
		INFO_PRINTF1(_L("Request NOT cancelled"));
	}

	// Test delayed cancelling of request
	INFO_PRINTF1(_L("Test delayed cancelling of GetInfo request"));
	meStore.GetInfo(reqStatus, meSmsInfoPckg2);
	User::After(500000);	
	meStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);

	if (reqStatus==KErrCancel)
	{
		INFO_PRINTF1(_L("Request successfully cancelled"));
	}
	else
	{
		INFO_PRINTF1(_L("Request NOT cancelled"));
	}

	// Test reading all the messages in SIM store
	// Print details of each message
	INFO_PRINTF1(_L("Reading Message from SIM Store"));

	RMobileSmsStore::TMobileGsmSmsEntryV1 smsEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsEntryPckg(smsEntry);

	for (TInt i=1; i<=simSmsInfo.iUsedEntries; i++)
		{
		smsEntry.iIndex=i;		// Entry to read
		simStore.Read(reqStatus, smsEntryPckg);
		User::WaitForRequest(reqStatus);
		if (reqStatus == KErrNotFound)
			{
			INFO_PRINTF2((_L("The entry was not found, request status = %d")), reqStatus.Int());
			}
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

	if (cancelReqStatus==KErrCancel)
	{
		INFO_PRINTF1(_L("Read Request successfully cancelled"));
	}
	else
	{
		INFO_PRINTF1(_L("Read Request NOT cancelled"));
	}

	// Test writing a dummy PDU to SIM store
	// And then test reading it out and comparing it
	INFO_PRINTF1(_L("Test Write to SIM Store"));

	const TText8 MyPdu[] = {0x24, 0x0c,0x91,0x44,0x97,0x10,0x91,0x06,0x25,0x00,0x00,0x10,0x70,0x92,0x40,0x83,0x50,0x40,0x37,0xD6,0x77,0x7A,0x5C,0x6E,0x87,0xD3,0x6C,0x10,0x3A,0x3C,0x07,0xCD,0x40,0xEE,0xF2,0x1D,0xD4,0x2E,0xCF,0xE7,0xE1,0x73,0x79,0xEE,0x02,0x41,0xD9,0xE5,0xF0,0xBC,0x0C,0x1A,0x87,0xD9,0x6C,0xD0,0x8A,0x46,0xBB,0xE5,0x60,0xB1,0x58,0xCE,0x06,0xAB,0xC9,0x00};

	smsEntry.iMsgStatus=RMobileSmsStore::EStoredMessageUnsent;
	smsEntry.iMsgData.Copy(MyPdu, sizeof(MyPdu));

	simStore.Write(reqStatus, smsEntryPckg);
	User::WaitForRequest(reqStatus);

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
				INFO_PRINTF1(_L("Read entry NOT matched with Write entry"));
				}
			}
		}

	// Test writing and cancelling to ME store first
	INFO_PRINTF1(_L("Test Cancel Write to ME Store"));
	meStore.Write(cancelReqStatus, smsEntryPckg);
	meStore.CancelAsyncRequest(EMobilePhoneStoreWrite);
	User::WaitForRequest(cancelReqStatus);

	if (cancelReqStatus==KErrCancel)
		{
		INFO_PRINTF1(_L("Write Request successfully cancelled"));
		}
	else
		{
		INFO_PRINTF1(_L("Write Request NOT cancelled"));
		}

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
		}
		else
			{
			INFO_PRINTF2(_L("Entry %d: Successfully read"), smsReadEntry2.iIndex);
			INFO_PRINTF2(_L("Status of entry = %d"),smsReadEntry2.iMsgStatus);
			INFO_PRINTF2(_L("Length of PDU in octets = %d"),smsReadEntry2.iMsgData.Size());
		
			if (smsReadEntry2.iMsgData.Compare(smsEntry.iMsgData)==0)
			{
				INFO_PRINTF1(_L("Read entry successfully compared with Write entry"));
			}
			else
			{
				INFO_PRINTF1(_L("Read entry NOT matched with Write entry"));
			}
			}
		}

	// Now test deletion of entries - test deletion of last entry
	// Get info again in order to get up-to-date number of used entries

	simStore.GetInfo(reqStatus, simSmsInfoPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	if (simSmsInfo.iUsedEntries > 0)
		{
		simStore.Delete(reqStatus, simSmsInfo.iUsedEntries);
		User::WaitForRequest(reqStatus);
		if (reqStatus == KErrNotFound)
			{
			INFO_PRINTF2((_L("The entry was not found, request status = %d")), reqStatus.Int());
			}
		else
			{
			INFO_PRINTF2(_L("Entry %d: Successfully deleted"), simSmsInfo.iUsedEntries);
			}
		}

	meStore.GetInfo(reqStatus, meSmsInfoPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	if (meSmsInfo.iUsedEntries > 0)
		{
		meStore.Delete(reqStatus, meSmsInfo.iUsedEntries);
		User::WaitForRequest(reqStatus);
		if (reqStatus == KErrNotFound)
			{
			INFO_PRINTF2((_L("The entry was not found, request status = %d")), reqStatus.Int());
			}
		else
			{
			INFO_PRINTF2(_L("Entry %d: Successfully deleted"), meSmsInfo.iUsedEntries);
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
		{
			INFO_PRINTF1(_L("Delete Request successfully cancelled"));
		}
		else
		{
			INFO_PRINTF1(_L("Delete Request NOT cancelled"));
		}
		}

	simStore.Close();
	meStore.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CSendSms::CSendSms()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SendSms"));
	}
//------------------------------------------------------------------

TVerdict CSendSms::doTestStepL( void )
/**
 * SendSms test.
 */
	{
	SendSms() ;
	return TestStepResult();
	}
//------------------------------------------------------------------

CReceiveUnstoredSms::CReceiveUnstoredSms()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("ReceiveUnstoredSms"));
	}
//------------------------------------------------------------------

TVerdict CReceiveUnstoredSms::doTestStepL( void )
/**
 * Receive an unstored Sms test.
 * Currently the MMTSY does not support receive unstored mode, so that is what this test
 * tests for.
 */
	{

//  Added during port to test execute
//  Not part of original scheduletest test harness
//  Added because this test seems to rely upon previous test
//  Please review!!!!
//  The original code for this test step seems to rely on things that it should not!
//  vvvvvvvvvv
	CSimSmsTestStepBase::SendSms();
//  ^^^^^^^^^^

	TRequestStatus aStatus,stat0;

	INFO_PRINTF1(_L("Setting receive mode to EReceiveUnstoredClientAck."));

	//User::After(2000000);		// Gives a bit of time 

	iSms.SetReceiveMode(aStatus, RMobileSmsMessaging::EReceiveUnstoredClientAck);
	User::WaitForRequest(aStatus);
	if(aStatus.Int()) return EFail;

	INFO_PRINTF1(_L("Calling Receive API ..."));
	
	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 recAtt;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg recAttPckg(recAtt);

	iSms.ReceiveMessage(aStatus, gsmMsg, recAttPckg);
	
	//ack message
	TPtrC8 nullPtr(NULL,0);
	iSms.AckSmsStored(stat0,nullPtr,EFalse);
	iSms.CancelAsyncRequest(EMobileSmsMessagingAckSmsStored);//no implementation-does nothing
	User::WaitForRequest(stat0);
	TEST(stat0==KErrGeneral);
	INFO_PRINTF1(_L("Message 1 ack"));

	INFO_PRINTF1(_L("Sending out a SMS ..."));

	TRAPD(ret, SendSms());
	if(ret) return EFail;

	// Wait for the sent SMS to return to this phone
	User::WaitForRequest(aStatus);
	if(aStatus.Int()) return(EFail);
	return TestStepResult();
	}
//------------------------------------------------------------------

CReceiveStoredSms::CReceiveStoredSms()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("ReceiveStoredSms"));
	}
//------------------------------------------------------------------

TVerdict CReceiveStoredSms::doTestStepL( void )
/**
 * Receive a stored Sms test.
 * Currently the MMTSY does not support receive unstored mode, so that is what this test
 * tests for.
 */
	{
	
//  Added during port to test execute
//  Not part of original scheduletest test harness
//  Added because this test seems to rely upon previous test
//  Please review!!!!
//  The original code for this test step seems to rely on things that it should not!
//  vvvvvvvvvv
	CSimSmsTestStepBase::SendSms();
//  ^^^^^^^^^^

	TRequestStatus aStatus;

	INFO_PRINTF1(_L("Setting receive mode to EReceiveStored."));

	User::After(2000000);		// Gives a bit of time 

	iSms.SetReceiveMode(aStatus, RMobileSmsMessaging::EReceiveStored);
	User::WaitForRequest(aStatus);
	if(aStatus.Int()==KErrNotSupported) 
		return TestStepResult();

	
	INFO_PRINTF1(_L("Calling Receive API ..."));

	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 recAtt;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg recAttPckg(recAtt);

	iSms.ReceiveMessage(aStatus, gsmMsg, recAttPckg);

	INFO_PRINTF1(_L("Sending out a SMS ..."));
	TRAPD(ret, SendSms(ETrue,iSms));
	if(ret!=KErrNone) 
		return (EFail);

	// Wait for the sent SMS to return to this phone
	User::WaitForRequest(aStatus);
	if(aStatus.Int()!=KErrNone) 
		return(EFail); 
	return TestStepResult();
	}
//------------------------------------------------------------------

CReceiveStoredSmsAndCancel::CReceiveStoredSmsAndCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("ReceiveStoredSmsAndCancel"));
	}
//------------------------------------------------------------------

TVerdict CReceiveStoredSmsAndCancel::doTestStepL( void )
	{
	TRequestStatus aStatus;

	INFO_PRINTF1(_L("Setting receive mode to EReceiveStored."));

	User::After(2000000);		// Gives a bit of time 

	iSms.SetReceiveMode(aStatus, RMobileSmsMessaging::EReceiveStored);
	User::WaitForRequest(aStatus);
	if(aStatus.Int()==KErrNotSupported) 
		return TestStepResult();

	
	INFO_PRINTF1(_L("Calling Receive API ..."));

	RMobileSmsMessaging::TMobileSmsGsmTpdu gsmMsg;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 recAtt;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg recAttPckg(recAtt);

	iSms.ReceiveMessage(aStatus, gsmMsg, recAttPckg);

	INFO_PRINTF1(_L("Sending out a SMS ..."));
	TRAPD(ret, SendSms(ETrue,iSms));
	if(ret) 
		return EFail;

	iSms.CancelAsyncRequest(EMobileSmsMessagingReceiveMessage);		// Cancels ...

	// Wait for the sent SMS to return to this phone
	User::WaitForRequest(aStatus);
	if((aStatus.Int() == KErrCancel) || (aStatus.Int() == KErrNone)) 
		return(EPass);
	else
		return(EFail); 
	}
//------------------------------------------------------------------

CSendSmsAndCancel::CSendSmsAndCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SendSmsAndCancel"));
	}
//------------------------------------------------------------------

TVerdict CSendSmsAndCancel::doTestStepL( void )
	{
	  TInt i;
	TRequestStatus status;
	INFO_PRINTF1(_L("Sending SMS and cancelling."));

	// Create message PDU and convert to binary
	_LIT8(KMsgDataBeforeTargetAddress,"1d00");		
	_LIT8(KMsgDataAfterTargetAddress,"01A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A001A0");
	TBuf8<400> msgData;
	TLex8 lex;
	TUint8 val;
	msgData.Zero();

	const TInt count_before((&KMsgDataBeforeTargetAddress)->Length()/2);	// Assume length of pdu is always even
	for(i=0;i<count_before;++i)
		{
		lex=(&KMsgDataBeforeTargetAddress)->Mid(i*2,2);
		TESTL(KErrNone == lex.Val(val,EHex));					
		msgData.Append(TChar(val));
		}

	TBuf8<20> targetAddressAscii;
	targetAddressAscii.Zero();
	RMobilePhone::TMobileAddress targetAddress;
	targetAddress.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	targetAddress.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	targetAddress.iTelNumber.Copy(iTelNumber);
	TInt ret = CATSmsUtils::AppendAddressToAscii(targetAddressAscii,targetAddress);
	if(ret) return(EFail);

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
		
	msgAttr.iFlags=0x184;
	msgAttr.iDataFormat=RMobileSmsMessaging::EFormatGsmTpdu;
	msgAttr.iGsmServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	msgAttr.iGsmServiceCentre.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	msgAttr.iGsmServiceCentre.iTelNumber.Copy(iSCANumber);

	// Package up data ready for sending to etel
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg msgAttrPkg(msgAttr);

	// Send the message
	INFO_PRINTF1(_L(".."));

	TSmsPdu smsPduSent;

	smsPduSent.Copy(KTestSendPduA,sizeof(KTestSendPduA));


	iSms.SendMessage(status,smsPduSent,msgAttrPkg);

	// Wait for a bit
	User::After(KOneSecond*2);
	// Cancel sending, then wait for sending operation to complete
	iSms.CancelAsyncRequest(EMobileSmsMessagingSendMessage);
	//if(status==KRequestPending)
	User::WaitForRequest(status);			

	ret=KErrNone;
	if(status!=KErrCancel && status!=KErrNone)
		ret=status.Int();
	else
		INFO_PRINTF1(_L("TESTL(s) passed"));	// If we don't cause a panic or leave the test has passed

/*	iSms.Close();
	iPhone->Close();

	User::After(500000L);

	iPhone.Open(iTelServer,GSM_NAME);
	iSms.Open(iPhone);
	*/
	return TestStepResult();
	}
//------------------------------------------------------------------

CAckSmsStored::CAckSmsStored()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("AckSmsStored"));
	}
//------------------------------------------------------------------

TVerdict CAckSmsStored::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
	TRequestStatus aStatus;

	TPtrC8 nullPtr(NULL,0);

	//ack the pending one
	iSms.AckSmsStored(aStatus, nullPtr);
	User::WaitForRequest(aStatus);
	//no more to ack
	iSms.AckSmsStored(aStatus, nullPtr);
	User::WaitForRequest(aStatus);

	if(aStatus.Int() == KErrGeneral)	//There is no outstanding SMS to Ack, so KErrGeneral is the correct response.
		return(EPass);
	else
		ERR_PRINTF1(_L("SIMTSY dehavior is diferent, but why?"));
	return	EFail ;
	}
//------------------------------------------------------------------

CAckSmsStoredCancel::CAckSmsStoredCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("AckSmsStoredCancel"));
	}
//------------------------------------------------------------------

TVerdict CAckSmsStoredCancel::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.AckSmsStoredCancel();		// This API doesn't exist anymore
	return TestStepResult();
	}
//------------------------------------------------------------------

CNackSmsStored::CNackSmsStored()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NackSmsStored"));
	}
//------------------------------------------------------------------

TVerdict CNackSmsStored::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
	TRequestStatus aStatus;

	TPtrC8 nullPtr(NULL,0);

	iSms.NackSmsStored(aStatus, nullPtr, KErrGsmRRInvalidMessage);
	User::WaitForRequest(aStatus);			//  There is no sms to NACK, so KErrGeneral is the correct response.
	if(aStatus.Int() == KErrGeneral)
		return(EPass);
	else
		return(EFail);
	}
//------------------------------------------------------------------

CNackSmsStoredCancel::CNackSmsStoredCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NackSmsStoredCancel"));
	}
//------------------------------------------------------------------

TVerdict CNackSmsStoredCancel::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NackSmsStoredCancel();		// API doesn't exist anymore
	return TestStepResult();
	}
//------------------------------------------------------------------

CAckSmsStoredWithResponse::CAckSmsStoredWithResponse()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("AckSmsStoredWithResponse"));
	}
//------------------------------------------------------------------

TVerdict CAckSmsStoredWithResponse::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.AckSmsStoredWithResponse();	// API doesn't exist anymore
	return TestStepResult();
	}
//------------------------------------------------------------------

CAckSmsStoredWithResponseCancel::CAckSmsStoredWithResponseCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("AckSmsStoredWithResponseCancel"));
	}
//------------------------------------------------------------------

TVerdict CAckSmsStoredWithResponseCancel::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.AckSmsStoredWithResponseCancel();		// API doesn't exist anymore
	return TestStepResult();
	}
//------------------------------------------------------------------

CNackSmsStoredWithResponse::CNackSmsStoredWithResponse()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NackSmsStoredWithResponse"));
	}
//------------------------------------------------------------------

TVerdict CNackSmsStoredWithResponse::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NackSmsStoredWithResponse();		// API doesn't exist anymore
	return TestStepResult();
	}
//------------------------------------------------------------------

CNackSmsStoredWithResponseCancel::CNackSmsStoredWithResponseCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NackSmsStoredWithResponseCancel"));
	}
//------------------------------------------------------------------

TVerdict CNackSmsStoredWithResponseCancel::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NackSmsStoredWithResponse();		// API doesn't exist anymore
	return TestStepResult();
	}
//------------------------------------------------------------------

CResumeSmsReception::CResumeSmsReception()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("ResumeSmsReception"));
	}
//------------------------------------------------------------------

TVerdict CResumeSmsReception::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
	TRequestStatus aStatus;

	iSms.ResumeSmsReception(aStatus);
	User::WaitForRequest(aStatus);		// There has been no SMS suspension, so KErrGeneral is the correct response.
	if(aStatus.Int() == KErrGeneral)
		return(EPass);
	else
		return(EFail);
	}
//------------------------------------------------------------------

CNotifySmsEvent::CNotifySmsEvent()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifySmsEvent"));
	}
//------------------------------------------------------------------

TVerdict CNotifySmsEvent::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NotifySmsEvent();		// API doesn't exist anymore
	return TestStepResult();
	}
//------------------------------------------------------------------

CNotifySmsEventCancel::CNotifySmsEventCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifySmsEventCancel"));
	}
//------------------------------------------------------------------

TVerdict CNotifySmsEventCancel::doTestStepL( void )
/**
 * This function Tests an API that should return
 * KErrNotSupported
 */
	{
//	iSms.NotifySmsEventCancel();		// API doesn't exist anymore
	return TestStepResult();
	}


/**************************************************************/
//
// Testing asynchronous retrieve of SMSP list
//
/**************************************************************/

CTestGetSmspList* CTestGetSmspList::NewLC(RMobileSmsMessaging& aSmsMessaging)
	{
    CTestGetSmspList* r=new(ELeave) CTestGetSmspList(aSmsMessaging);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestGetSmspList::CTestGetSmspList(RMobileSmsMessaging& aSmsMessaging)
    : CActive(EPriorityNormal), iSmsMessaging(aSmsMessaging)
    {
	}
      
void CTestGetSmspList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneSmspList::NewL(iSmsMessaging);
    }
         
CTestGetSmspList::~CTestGetSmspList()
    {
	delete iRetrieve;
	}
      
void CTestGetSmspList::Start()
    {
    iRetrieve->Start(iStatus);
    SetActive();
    }       

CMobilePhoneSmspList* CTestGetSmspList::RetrieveListL()
	{
	return(iRetrieve->RetrieveListL());
	}
          
void CTestGetSmspList::RunL()
    {
	TInt ret=iStatus.Int();
    User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}

void CTestGetSmspList::DoCancel()
    {
    iRetrieve->Cancel();
    }       

TInt CReceiveStoredSmsAndCancel::SendSms(TBool aQuiet, RMobileSmsMessaging& aSms)
/**
 * SendSms test.
 */
	{
	if(!aQuiet) 
		INFO_PRINTF1(_L("Sending SMS."));

	// Create message PDU and convert to binary 
	TBuf8<400> msgData;
	TLex8 lex;
	TUint8 val;
	TInt i;

	TInt ret;
	msgData.Zero();



	const TInt count_before((&KMsgDataBeforeTargetAddress)->Length()/2);	// Assume length of pdu is always even
	for(i=0;i<count_before;++i)
		{
		lex=(&KMsgDataBeforeTargetAddress)->Mid(i*2,2);
		ret = lex.Val(val,EHex);					

		if(ret) return ret;
		msgData.Append(TChar(val));
		}

	TBuf8<20> targetAddressAscii;
	targetAddressAscii.Zero();
	RMobilePhone::TMobileAddress targetAddress;
	targetAddress.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	targetAddress.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	targetAddress.iTelNumber.Copy(iTelNumber);
	ret = CATSmsUtils::AppendAddressToAscii(targetAddressAscii,targetAddress);

	if(ret) return ret;

	const TInt count_address(targetAddressAscii.Length()/2);	// Assume length is always even
	for(i=0;i<count_address;++i)
		{
		lex=targetAddressAscii.Mid(i*2,2);
		ret = lex.Val(val,EHex);					

		if(ret) return ret;
		msgData.Append(TChar(val));
		}

	const TInt count_after((&KMsgDataAfterTargetAddress)->Length()/2);	// Assume length of pdu is always even
	for(i=0;i<count_after;++i)
		{
		lex=(&KMsgDataAfterTargetAddress)->Mid(i*2,2);
		ret = lex.Val(val,EHex);

		if(ret) return ret;
		msgData.Append(TChar(val));
		}

	// Create message attibutes 
	RMobileSmsMessaging::TMobileSmsSendAttributesV1	msgAttr;
		
	msgAttr.iFlags=0x184;
	msgAttr.iDataFormat=RMobileSmsMessaging::EFormatGsmTpdu;
	msgAttr.iGsmServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	msgAttr.iGsmServiceCentre.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	msgAttr.iGsmServiceCentre.iTelNumber.Copy(iSCANumber);

	// Package up data ready for sending to etel
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg msgAttrPkg(msgAttr);

	// Send the message, try upto 3 times if phone is not ready
	TRequestStatus status;
	INFO_PRINTF1(_L(".."));
	TSmsPdu smsPduSent;

	smsPduSent.Copy(KTestSendPduA,sizeof(KTestSendPduA));

	aSms.SendMessage(status,smsPduSent,msgAttrPkg);
	User::WaitForRequest(status);
	if(status==KErrGsmSMSFailureInME || status==KErrGeneral || status==KErrGsmSMSUnknownError)
		{
		INFO_PRINTF1(_L(".."));
		aSms.SendMessage(status,smsPduSent,msgAttrPkg);
		User::WaitForRequest(status);
		if(status==KErrGsmSMSFailureInME || status==KErrGeneral || status==KErrGsmSMSUnknownError)
			{
			INFO_PRINTF1(_L(".."));
			aSms.SendMessage(status,smsPduSent,msgAttrPkg);
			User::WaitForRequest(status);
			}
		}	
	
	ret = status.Int();

	if(ret) return ret;

	// Validate message reference number has changed 
	if(!(msgAttr.iFlags&RMobileSmsMessaging::KMessageReference))
		return(KErrCorrupt);

	if(!aQuiet) 
		INFO_PRINTF1(_L("test(s) passed"));

	return(KErrNone);
	}


TInt CReceiveStoredSms::SendSms(TBool aQuiet, RMobileSmsMessaging& aSms)
/**
 * SendSms test.
 */
	{
	if(!aQuiet) 
		INFO_PRINTF1(_L("Sending SMS."));

	// Create message PDU and convert to binary 
	TBuf8<400> msgData;
	TLex8 lex;
	TUint8 val;
	TInt i;

	TInt ret;
	msgData.Zero();



	const TInt count_before((&KMsgDataBeforeTargetAddress)->Length()/2);	// Assume length of pdu is always even
	for(i=0;i<count_before;++i)
		{
		lex=(&KMsgDataBeforeTargetAddress)->Mid(i*2,2);
		ret = lex.Val(val,EHex);					

		if(ret) return ret;
		msgData.Append(TChar(val));
		}

	TBuf8<20> targetAddressAscii;
	targetAddressAscii.Zero();
	RMobilePhone::TMobileAddress targetAddress;
	targetAddress.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	targetAddress.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	targetAddress.iTelNumber.Copy(iTelNumber);
	ret = CATSmsUtils::AppendAddressToAscii(targetAddressAscii,targetAddress);

	if(ret) return ret;

	const TInt count_address(targetAddressAscii.Length()/2);	// Assume length is always even
	for(i=0;i<count_address;++i)
		{
		lex=targetAddressAscii.Mid(i*2,2);
		ret = lex.Val(val,EHex);					

		if(ret) return ret;
		msgData.Append(TChar(val));
		}

	const TInt count_after((&KMsgDataAfterTargetAddress)->Length()/2);	// Assume length of pdu is always even
	for(i=0;i<count_after;++i)
		{
		lex=(&KMsgDataAfterTargetAddress)->Mid(i*2,2);
		ret = lex.Val(val,EHex);

		if(ret) return ret;
		msgData.Append(TChar(val));
		}

	// Create message attibutes 
	RMobileSmsMessaging::TMobileSmsSendAttributesV1	msgAttr;
		
	msgAttr.iFlags=0x184;
	msgAttr.iDataFormat=RMobileSmsMessaging::EFormatGsmTpdu;
	msgAttr.iGsmServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	msgAttr.iGsmServiceCentre.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	msgAttr.iGsmServiceCentre.iTelNumber.Copy(iSCANumber);

	// Package up data ready for sending to etel
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg msgAttrPkg(msgAttr);

	// Send the message, try upto 3 times if phone is not ready
	TRequestStatus status;
	INFO_PRINTF1(_L(".."));
	TSmsPdu smsPduSent;

	smsPduSent.Copy(KTestSendPduA,sizeof(KTestSendPduA));

	aSms.SendMessage(status,smsPduSent,msgAttrPkg);
	User::WaitForRequest(status);
	if(status==KErrGsmSMSFailureInME || status==KErrGeneral || status==KErrGsmSMSUnknownError)
		{
		INFO_PRINTF1(_L(".."));
		aSms.SendMessage(status,smsPduSent,msgAttrPkg);
		User::WaitForRequest(status);
		if(status==KErrGsmSMSFailureInME || status==KErrGeneral || status==KErrGsmSMSUnknownError)
			{
			INFO_PRINTF1(_L(".."));
			aSms.SendMessage(status,smsPduSent,msgAttrPkg);
			User::WaitForRequest(status);
			}
		}	
	
	ret = status.Int();

	if(ret) return ret;

	// Validate message reference number has changed 
	if(!(msgAttr.iFlags&RMobileSmsMessaging::KMessageReference))
		{
		return(KErrCorrupt);
		}

	if(!aQuiet) 
		{
		INFO_PRINTF1(_L("test(s) passed"));
		}

	return(KErrNone);
	}
