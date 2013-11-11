//
// testltsysmswrite.cpp
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

//user include
#include "testltsysmswrite.h"
#include "testltsysmsatutil.h"

//constant defination
_LIT8(KSmsMessagingData,"11000D91683156211075f6000800064F60597D0021");
_LIT(KSmsSCA,"8613800100500");  

CTestLtsySmsWrite::CTestLtsySmsWrite(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsWrite::~CTestLtsySmsWrite()
	{
	}

CTestLtsySmsWrite* CTestLtsySmsWrite::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsWrite* self = new (ELeave)CTestLtsySmsWrite(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsWrite* CTestLtsySmsWrite::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsWrite* self=CTestLtsySmsWrite::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsWrite::ConstructL()
	{

	}

TVerdict CTestLtsySmsWrite::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsWrite::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsWrite::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsWrite::doTestStepL called"));
	TRAPD(err,WriteSmsL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write message"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsWrite::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsWrite::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsySmsWrite::WriteSmsL()
	{

	RMobilePhone::TMobileAddress smsWriteAddr;
	smsWriteAddr.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	smsWriteAddr.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
	smsWriteAddr.iTelNumber = KSmsSCA;
	//For GsmTpdu
	TBuf8<400>   writePduAscii(KSmsMessagingData);                        
	TBuf8<400>   writePduData; 
	writePduData.Zero();
    SmsAtUtil::ConvertAsciiToBinary(writePduAscii,writePduData);
    FillWriteSmsAttibutesL(writePduData,smsWriteAddr);
 	}

void CTestLtsySmsWrite::FillWriteSmsAttibutesL(TBuf8<400> aTpdu,RMobilePhone::TMobileAddress  aMsgSca)
	{
	TRequestStatus status;
	RMobileSmsStore::TMobileGsmSmsEntryV1 smsEntry;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsEntryPckg(smsEntry);
	smsEntry.iMsgStatus=RMobileSmsStore::EStoredMessageUnsent;
	smsEntry.iMsgData = aTpdu;
	smsEntry.iServiceCentre = aMsgSca;
	SmsStore().Write(status, smsEntryPckg);
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("Write() request status = %d"),r);
	}

//End of file
