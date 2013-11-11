//
// testltsysmswriteinvalidparam.cpp
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

//user include
#include "testltsysmswriteinvalidparam.h"
#include "testltsysmsatutil.h"

//constant defination
_LIT8(KSmsMessagingData,"11000D91683156211075f6000800064F60597D0021");
_LIT(KSmsSCA,"8613800100500"); 
_LIT(KSmsSCA1,"8613800100500138001005001380010050013800100500138001005001380010050013800100500138001005001380010050");  
_LIT(KSmsSCA2,"86138*#abc0W0");

CTestLtsySmsWriteInvalidParam::CTestLtsySmsWriteInvalidParam(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsWriteInvalidParam::~CTestLtsySmsWriteInvalidParam()
	{
	}

CTestLtsySmsWriteInvalidParam* CTestLtsySmsWriteInvalidParam::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsWriteInvalidParam* self = new (ELeave)CTestLtsySmsWriteInvalidParam(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsWriteInvalidParam* CTestLtsySmsWriteInvalidParam::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsWriteInvalidParam* self=CTestLtsySmsWriteInvalidParam::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsWriteInvalidParam::ConstructL()
	{

	}

TVerdict CTestLtsySmsWriteInvalidParam::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsWriteInvalidParam::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsWriteInvalidParam::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsWriteInvalidParam::doTestStepL called"));
	TRAPD(err,WriteSmsL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write message"),err);
	    SetTestStepResult(EFail);
		}
	
	TRAPD(err1,WriteSms1L());
	if(err1 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write message"),err1);
	    SetTestStepResult(EFail);
		}
	
	TRAPD(err2,WriteSms2L());
	if(err2 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write message"),err2);
	    SetTestStepResult(EFail);
		}
	
	TRAPD(err3,WriteSms3L());
	if(err3 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write message"),err3);
	    SetTestStepResult(EFail);
		}
	
	TRAPD(err4,WriteSms4L());
	if(err4 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write message"),err4);
	    SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

TVerdict CTestLtsySmsWriteInvalidParam::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsWriteInvalidParam::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsySmsWriteInvalidParam::WriteSmsL()
	{
	//There is too long sca
	RMobilePhone::TMobileAddress smsWriteAddr;
	    
    smsWriteAddr.iTypeOfNumber = RMobilePhone::EInternationalNumber;
  	smsWriteAddr.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
  	
  	smsWriteAddr.iTelNumber = KSmsSCA1;
  	TBuf8<400>   writePduAscii(KSmsMessagingData);                         //For GsmTpdu
  	TBuf8<400>   writePduData; 
  	writePduData.Zero();
  	SmsAtUtil::ConvertAsciiToBinary(writePduAscii,writePduData);
  	FillWriteSmsAttibutesL(writePduData,smsWriteAddr);
   	}

void CTestLtsySmsWriteInvalidParam::WriteSms1L()
	{
	//different TypeOfNumber and NumberPlan
	RMobilePhone::TMobileAddress smsWriteAddr;
	smsWriteAddr.iTypeOfNumber = RMobilePhone::ENetworkSpecificNumber;
	smsWriteAddr.iNumberPlan = RMobilePhone::EDataNumberPlan;
	
	smsWriteAddr.iTelNumber = KSmsSCA;
	 //For GsmTpdu
	TBuf8<400>   writePduAscii(KSmsMessagingData);                        
	TBuf8<400>   writePduData; 
	writePduData.Zero();
	SmsAtUtil::ConvertAsciiToBinary(writePduAscii,writePduData);
  	FillWriteSmsAttibutesL(writePduData,smsWriteAddr);
	}

void CTestLtsySmsWriteInvalidParam::WriteSms2L()
	{
	//different TypeOfNumber,NumberPlan and sca
	RMobilePhone::TMobileAddress smsWriteAddr;
	smsWriteAddr.iTypeOfNumber = RMobilePhone::ESubscriberNumber;
	smsWriteAddr.iNumberPlan = RMobilePhone::EPrivateNumberPlan;
	
	smsWriteAddr.iTelNumber = KSmsSCA2;
	 //For GsmTpdu
	TBuf8<400>   writePduAscii(KSmsMessagingData);                        
	TBuf8<400>   writePduData; 
	writePduData.Zero();
	SmsAtUtil::ConvertAsciiToBinary(writePduAscii,writePduData);
  	FillWriteSmsAttibutesL(writePduData,smsWriteAddr);
	}
void CTestLtsySmsWriteInvalidParam::WriteSms3L()
	{
	//different TypeOfNumber and NumberPlan
	RMobilePhone::TMobileAddress smsWriteAddr;
	
	smsWriteAddr.iTypeOfNumber = RMobilePhone::ENationalNumber;
	smsWriteAddr.iNumberPlan = RMobilePhone::ENationalNumberPlan;
	
	smsWriteAddr.iTelNumber = KSmsSCA;
	 //For GsmTpdu
	TBuf8<400>   writePduAscii(KSmsMessagingData);                        
	TBuf8<400>   writePduData; 
	writePduData.Zero();
	
	SmsAtUtil::ConvertAsciiToBinary(writePduAscii,writePduData);
  	FillWriteSmsAttibutesL(writePduData,smsWriteAddr);
	}

void CTestLtsySmsWriteInvalidParam::WriteSms4L()
	{
	//different TypeOfNumber and NumberPlan
	RMobilePhone::TMobileAddress smsWriteAddr;
	
	smsWriteAddr.iTypeOfNumber = RMobilePhone::EUnknownNumber;
	smsWriteAddr.iNumberPlan = RMobilePhone::RMobilePhone::EUnknownNumberingPlan;
	
	smsWriteAddr.iTelNumber = KSmsSCA;
	 //For GsmTpdu
	TBuf8<400>   writePduAscii(KSmsMessagingData);                        
	TBuf8<400>   writePduData; 
	writePduData.Zero();
	SmsAtUtil::ConvertAsciiToBinary(writePduAscii,writePduData);
  	FillWriteSmsAttibutesL(writePduData,smsWriteAddr);
	}
void CTestLtsySmsWriteInvalidParam::FillWriteSmsAttibutesL(TBuf8<400> aTpdu,RMobilePhone::TMobileAddress  aMsgSca)
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
