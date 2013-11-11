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

#include "Te_SmsTestStepBase.h"

TVerdict CSmsTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	iTestScheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);
	
	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		delete iTestScheduler;
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	CConfigParams* testConfigParams=NULL;
	TRAP(ret,testConfigParams=CConfigParams::NewL(KTSSMSConfigFile()));
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to create config parameters (%d)"), ret);
		iTelServer.UnloadPhoneModule(KtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}
	const TPtrC scanumber = testConfigParams->FindAlphaVar(KTestSCANumber,KNullDesC);
	iSCANumber.Copy(scanumber);
	const TPtrC simnumber = testConfigParams->FindAlphaVar(KestSimNumber,KNullDesC);
	iTelNumber.Copy(simnumber);
	delete testConfigParams;

	ret=iPhone.Open(iTelServer,KPhoneName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}
//
//	This test requires a mobile phone to be connected to the serial port
//      specified in the .cfg file for the initialise below to work
//
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	ret=iSms.Open(iPhone); 
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open SMS"));
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CSmsTestStep::doTestStepPostambleL()
	{
	iSms.Close();
	iPhone.Close();
	iTelServer.UnloadPhoneModule(KtsyName);
	iTelServer.Close();
	delete iTestScheduler;

	__UHEAP_MARKEND;
	User::After(5000000);
	return TestStepResult();
	}

TInt CSmsTestStep::AppendAddressToAscii(TDes8& aAscii,const RMobilePhone::TMobileAddress& aAddress)
/**
 * THIS CODE HAS BEEN COPIED FROM MMTSY\MULTIMODE\SMS\SMSUTIL.CPP
 *
 * Default operation is to code Address-Length according to=
 * 04.11 spec (ie. Address-Length=number of digits in Address-Value).
 *
 * @return Standard KErr... values
 */
	{
	// Duplicate tel number, removing all the weird chars
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber;
	const TInt count(aAddress.iTelNumber.Length());
	TInt i;
	for(i=0;i<count;++i)
		{
		if(IsAddressChar(TChar(aAddress.iTelNumber[i])))
			telNumber.Append(aAddress.iTelNumber[i]);
		}

	const TInt telNumberLength(telNumber.Length());
	// Validate the size of the supplied SCA

	// Calculate the number of ascii chars we'll need
	// We need 4 chars to code the Address-Length and Address-Type fields.
	// We need to add on an extra 'padding' char if the total number of chars is odd.
	const TInt neededAsciiChars=(4+telNumberLength)+(telNumberLength%2);
	if((aAscii.MaxLength()-aAscii.Length())<neededAsciiChars)	
		return KErrOverflow;
	// Code Address-Length
	AppendOctet(telNumberLength,aAscii);
	// Code Type-Of-Address
	TInt typeOfNumber=ConvertTypeOfNumber(aAddress.iTypeOfNumber);
	TInt numberingPlan=ConvertNumberingPlan(aAddress.iNumberPlan);
	AppendOctet(0x80+(typeOfNumber<<4)+(numberingPlan),aAscii);
	// Code Address-Value
	TInt highSemiOctet;
	TInt lowSemiOctet;
	const TInt octets(telNumberLength/2);	// This division will be rounded down
	for(i=0;i<octets;++i)
		{
		// See ETSI 03.40 section 9.1.2.3
		// Address digits are coded into octets as pairs.
		lowSemiOctet=ConvertAddressChar(TChar(telNumber[i*2]));
		highSemiOctet=ConvertAddressChar(TChar(telNumber[(i*2)+1]));
		AppendOctet((highSemiOctet<<4)+lowSemiOctet,aAscii);
		}
	// If number of semi octects is odd then process the final octet
	if(telNumberLength%2==1)		
		{
		lowSemiOctet=ConvertAddressChar(TChar(telNumber[telNumberLength-1]));
		AppendOctet(0xf0+lowSemiOctet,aAscii);
		}
	return KErrNone;
	}

void CSmsTestStep::AppendOctet(TInt aOctet,TDes8& aAscii)
/**
 * THIS CODE HAS BEEN COPIED FROM MMTSY\MULTIMODE\SMS\SMSUTIL.CPP
 *
 * Converts a TInt octet value into ASCII representation and then appends that
 * ASCII representation to the end of the given ASCII string.
 *
 * @param aOctet the octet value to append
 * @param aAscii the ASCII string to which aOctet value should be appended
 */
	{
	// Ensure client has only passed us a octet (ie. low 8 bits only)
	aOctet=aOctet&0xff;
	// Append octet 
	// (prefix '0' if the octets value only uses one digit as final octet coding must use two digits)
	if(aOctet<=0x0f)
		aAscii.Append(TChar('0'));
	aAscii.AppendNum(aOctet,EHex);
	}

TBool CSmsTestStep::IsAddressChar(TChar aChar)
/**
 * THIS CODE HAS BEEN COPIED FROM MMTSY\MULTIMODE\SMS\SMSUTIL.CPP
 *
 * Returns ETrue if, and only if, the given ASCII charcater is valid as an ASCII address character.
 */
 	{
	if(aChar.IsDigit())
		return ETrue;
	if(aChar==TChar('*') ||
	   aChar==TChar('#') ||
	   aChar==TChar('a') ||
	   aChar==TChar('b') ||
	   aChar==TChar('c'))
		return ETrue;
	return EFalse;
	}

TInt CSmsTestStep::ConvertTypeOfNumber(RMobilePhone::TMobileTON aEnum) 
/**
 * THIS CODE HAS BEEN COPIED FROM MMTSY\MULTIMODE\SMS\SMSUTIL.CPP
 *
 * @param aEnum must be a RMobile::TMobileTON value 
 * @return The equivalent ETSI Type-Of-Number value for aEnum
 */
	{
	switch(aEnum)
		{
		// The below 'magic numbers' come from the ETSI 03.40
		// specification for Address Fields (section 9.1.2.5)
	case RMobilePhone::EInternationalNumber:
		return 1;
	case RMobilePhone::ENationalNumber:
		return 2;
	case RMobilePhone::ENetworkSpecificNumber:
		return 3;
	case RMobilePhone::ESubscriberNumber:
		return 4;
	case RMobilePhone::EUnknownNumber:
	case RMobilePhone::EAbbreviatedNumber:
	default:
		return 0;
		}
	}

TInt CSmsTestStep::ConvertNumberingPlan(RMobilePhone::TMobileNPI aEnum)  
/**
 * THIS CODE HAS BEEN COPIED FROM MMTSY\MULTIMODE\SMS\SMSUTIL.CPP
 *
 * @param aEnum must be a n RMobile::TMobileNPI value 
 * @return The equivalent ETSI Numbering-Plan-Identification value for aEnum
 */
	{
	switch(aEnum)
		{
		// The below 'magic numbers' come from the ETSI 03.40
		// specification for Address Fields (section 9.1.2.5)
	case RMobilePhone::EIsdnNumberPlan:
		return 1;
	case RMobilePhone::EDataNumberPlan:
		return 3;
	case RMobilePhone::ETelexNumberPlan:
		return 4;
	case RMobilePhone::ENationalNumberPlan:
		return 8;
	case RMobilePhone::EPrivateNumberPlan:
		return 9;
	case RMobilePhone::EUnknownNumberingPlan:
	default:
		return 0;
		}
	}

TInt CSmsTestStep::ConvertAddressChar(TChar aChar)
/**
 * THIS CODE HAS BEEN COPIED FROM MMTSY\MULTIMODE\SMS\SMSUTIL.CPP
 *
 * Returns the equivalent numeric value for a given ASCII address character.
 *
 * @param aChar the address character to be converted
 * @return The numeric value equivalent of the given address character.
 */
 	{
	aChar.LowerCase();
	if(aChar-TChar('0')<=9)  
		return aChar-TChar('0');		// Assumes digit characters are one after each other
	else if(aChar==TChar('*'))
		return 10;
	else if(aChar==TChar('#'))
		return 11;
	else if(aChar==TChar('a'))
		return 12;
	else if(aChar==TChar('b'))
		return 13;
	else if(aChar==TChar('c'))
		return 14;
	return 15;
	}

void CSmsTestStep::GetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry)
/**
 * This function tests CRetrieveMobilePhoneSmspList
 */
	{
	CTestGetSmspList* retrieveSmspList;
	CMobilePhoneSmspList* smspList;
	retrieveSmspList = CTestGetSmspList::NewLC(iSms);
	retrieveSmspList->Start();
	CActiveScheduler::Start();
	smspList = retrieveSmspList->RetrieveList();
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

void CSmsTestStep::SetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry)
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
	TEST(reqStatus.Int()==KErrNone);
	CleanupStack::PopAndDestroy(); // smspList;
	}

void CSmsTestStep::PrintMessageStoreCaps(TUint32 aCaps)
	{
	if(aCaps & RMobilePhoneStore::KCapsReadAccess)
		INFO_PRINTF1(_L("Store supports read access"));
	if(aCaps & RMobilePhoneStore::KCapsWriteAccess)
		INFO_PRINTF1(_L("Store supports write/delete access"));
	if(aCaps & RMobilePhoneStore::KCapsDeleteAll)
		INFO_PRINTF1(_L("Store supports deleting all entries at once"));
	if(aCaps & RMobilePhoneStore::KCapsNotifyEvent)
		INFO_PRINTF1(_L("Store supports event notification"));
	if(aCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore))
		INFO_PRINTF1(_L("Store supports reading whole store in a list"));
	if(aCaps & RMobilePhoneStore::KCapsIndividualEntry)
		INFO_PRINTF1(_L("Store supports reading entries one at a time"));
	if(aCaps & RMobileSmsStore::KCapsUnreadMessages)
		INFO_PRINTF1(_L("Store contains unread messages"));
	if(aCaps & RMobileSmsStore::KCapsReadMessages)
		INFO_PRINTF1(_L("Store contains read messages"));
	if(aCaps & RMobileSmsStore::KCapsSentMessages)
		INFO_PRINTF1(_L("Store contains sent messages"));
	if(aCaps & RMobileSmsStore::KCapsUnsentMessages)
		INFO_PRINTF1(_L("Store contains unsent messages"));
	if(aCaps & RMobileSmsStore::KCapsGsmMessages)
		INFO_PRINTF1(_L("Store contains GSM format messages"));
	if(aCaps & RMobileSmsStore::KCapsCdmaMessages)
		INFO_PRINTF1(_L("Store contains CDMA format messages"));
	}

TInt CSmsTestStep::SendSms(TBool aQuiet)
/**
 * SendSms test.
 */
	{
	_LIT8(KMsgDataBeforeTargetAddress,"1100");
	_LIT8(KMsgDataAfterTargetAddress,"0000a705f4f29cde00");

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
		TEST(ret == KErrNone);
		msgData.Append(TChar(val));
		}
	TBuf8<20> targetAddressAscii;
	targetAddressAscii.Zero();
	RMobilePhone::TMobileAddress targetAddress;
	targetAddress.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	targetAddress.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	targetAddress.iTelNumber.Copy(iTelNumber);
	ret = AppendAddressToAscii(targetAddressAscii,targetAddress);
	TEST(ret == KErrNone);
	const TInt count_address(targetAddressAscii.Length()/2);	// Assume length is always even
	for(i=0;i<count_address;++i)
		{
		lex=targetAddressAscii.Mid(i*2,2);
		ret = lex.Val(val,EHex);					
		TEST(ret == KErrNone);
		msgData.Append(TChar(val));
		}
	const TInt count_after((&KMsgDataAfterTargetAddress)->Length()/2);	// Assume length of pdu is always even
	for(i=0;i<count_after;++i)
		{
		lex=(&KMsgDataAfterTargetAddress)->Mid(i*2,2);
		ret = lex.Val(val,EHex);
		TEST(ret == KErrNone);
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
	// Send the message, try upto 3 times if phone is not ready
	TRequestStatus status;
	INFO_PRINTF1(_L(".."));
	iSms.SendMessage(status,msgData,msgAttrPkg);
	User::WaitForRequest(status);
	if(status==KErrGsmSMSFailureInME || status==KErrGeneral || status==KErrGsmSMSUnknownError)
		{
		INFO_PRINTF1(_L(".."));
		iSms.SendMessage(status,msgData,msgAttrPkg);
		User::WaitForRequest(status);
		if(status==KErrGsmSMSFailureInME || status==KErrGeneral || status==KErrGsmSMSUnknownError)
			{
			INFO_PRINTF1(_L(".."));
			iSms.SendMessage(status,msgData,msgAttrPkg);
			User::WaitForRequest(status);
			}
		}	
	TEST(status.Int() == KErrNone);
	// Validate message reference number has changed 
	TEST(msgAttr.iFlags&RMobileSmsMessaging::KMessageReference);
	if(!aQuiet) 
		INFO_PRINTF1(_L("test(s) passed"));
	return(KErrNone);
	}

void CSmsTestStep::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
/**
 * Timeout function
 */
	{
	TRequestStatus timerStatus;
	RTimer timer ;
	timer.CreateLocal() ;
	timer.After(timerStatus,aNumberOfMicroSeconds);

	User::WaitForRequest(aStatus, timerStatus);
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		INFO_PRINTF1(_L("Time is over!!!")) ;
		}
	timer.Close() ;
	}

CConfigParams* CConfigParams::NewL(const TDesC &aCfgFileName)
	{
	CConfigParams *f;
	f=new (ELeave) CConfigParams();
	CleanupStack::PushL(f);
	//A leave during the ConstrucL is not fatal, just means there's no param file
	TRAP_IGNORE(f->ConstructL(aCfgFileName));
	CleanupStack::Pop();
	return f;
	}

void CConfigParams::ConstructL(const TDesC &aCfgFileName)
	{
	//Find the config file
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();
	//Location for the test parameter config file
	_LIT(KTinetConfigFilePaths,"\\;\\system\\data\\");
	TFindFile filePath(fs.iObj);
	User::LeaveIfError(filePath.FindByPath(aCfgFileName,&KTinetConfigFilePaths));
	//read the content of the file
	TAutoClose<RFile> fl;
	fl.PushL();
	User::LeaveIfError(fl.iObj.Open(fs.iObj,filePath.File(),EFileShareExclusive));
	User::LeaveIfError(fl.iObj.Read(iConfigParamBuf8));
	iConfigParamBuf.Copy(iConfigParamBuf8);
	CleanupStack::PopAndDestroy(2);
	iFileExist = ETrue;
	}

const TPtrC CConfigParams::FindAlphaVar(const TDesC &aVarName, const TDesC &aDefault)
	{
	if(!iFileExist)
		return TPtrC(aDefault);
	TInt pos=iConfigParamBuf.Find(aVarName);
	if (pos==KErrNotFound)
		return TPtrC(aDefault);
	TLex lex(iConfigParamBuf.Mid(pos));
	lex.SkipCharacters();
	lex.SkipSpaceAndMark();		// Should be at the start of the data
	lex.SkipCharacters();
	return TPtrC(lex.MarkedToken().Ptr(),lex.MarkedToken().Length());
	}

TInt CConfigParams::FindNumVar(const TDesC &aVarName, const TInt aDefault)
	{
	TInt result;
	TPtrC ptr = FindAlphaVar(aVarName,KNullDesC);
	if(ptr.Length())
		{
		TLex lex(ptr);
		if (lex.Val(result)==KErrNone)
			return result;
		}
	return aDefault;
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

CMobilePhoneSmspList* CTestGetSmspList::RetrieveList()
	{
	CMobilePhoneSmspList* ptr=NULL;
	TRAP_IGNORE(ptr=iRetrieve->RetrieveListL());		// trap but ignore error
	return ptr;
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

