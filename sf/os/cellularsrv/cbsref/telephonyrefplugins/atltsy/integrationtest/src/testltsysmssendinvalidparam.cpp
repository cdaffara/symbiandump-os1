//
// testltsysmssendinvalidparam.cpp
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

//user include
#include "testltsysmssendinvalidparam.h"
#include "testltsysmsatutil.h"
//constant defination
_LIT8(KSmsMessagingData,"11000D91683156211075f6000800064F60597D0021");
_LIT8(KSmsMessagingData1,"11000D91683156211075f6000800064F60597D002111000D91683156211075f6000800064F60597D002111000D91683156211075f6000800064F60597D002111000D91683156211075f6000800064F60597D002111000D91683156211075f6000800064F60597D002111000D91683156211075f6000800064F60597D002111000D91683156211075f6000800064F60597D002111000D91683156211075f6000800064F6059");
_LIT(KSmsSCA,"8613800100500");  
_LIT(KSmsSCA1,"8613800100500138001005001380010050013800100500138001005001380010050013800100500138001005001380010050");   
_LIT(KSmsSCA2,"86138*#abc0W0"); 

CTestLtsySmsSendInvalidParam::CTestLtsySmsSendInvalidParam(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsSendInvalidParam::~CTestLtsySmsSendInvalidParam()
	{
	}

CTestLtsySmsSendInvalidParam* CTestLtsySmsSendInvalidParam::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsSendInvalidParam* self = new (ELeave)CTestLtsySmsSendInvalidParam(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsSendInvalidParam* CTestLtsySmsSendInvalidParam::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsSendInvalidParam* self=CTestLtsySmsSendInvalidParam::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsSendInvalidParam::ConstructL()
	{

	}

TVerdict CTestLtsySmsSendInvalidParam::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsSendInvalidParam::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsSendInvalidParam::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsSendInvalidParam::doTestStepL called"));
	TRAPD(err,SendSmsL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when send message"),err);
	    SetTestStepResult(EFail);
		}
	
	TRAPD(err1,SendSms1L());
	if(err1 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when send message"),err1);
	    SetTestStepResult(EFail);
		}
	
	TRAPD(err2,SendSmsL());
	if(err2 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when send message"),err2);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsSendInvalidParam::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsSendInvalidParam::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsySmsSendInvalidParam::SendSmsL()
	{
	TRequestStatus status;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttPckg(iSmsAttr);
	//There is too long data
	TPtrC8 smsAsciiPdu(KSmsMessagingData1);
	TBuf8<200>  smsPdu;
	SmsAtUtil::ConvertAsciiToBinary(smsAsciiPdu,smsPdu);
	FillSmsAttibutes(KSmsSCA());
	Sms().SendMessage(status,smsPdu,smsAttPckg);   
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("SendMessage() request status = %d"),r);
	}

void CTestLtsySmsSendInvalidParam::FillSmsAttibutes(const TDesC& aTelNumber)
	{
	_LIT8(KSubmit, "Submit");
    iSmsAttr.iGsmServiceCentre.iTypeOfNumber = RMobilePhone::ENationalNumber;
    iSmsAttr.iGsmServiceCentre.iNumberPlan = RMobilePhone::ENationalNumberPlan; 
    iSmsAttr.iGsmServiceCentre.iTelNumber.Copy(aTelNumber);
    iSmsAttr.iCdmaServiceCategory = 0;
    iSmsAttr.iCdmaTeleservice = 0;
    iSmsAttr.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
    iSmsAttr.iFlags = 0;
    iSmsAttr.iMsgRef = 0;
    iSmsAttr.iSubmitReport = KSubmit;
    iSmsAttr.iMore = EFalse;
	}
void CTestLtsySmsSendInvalidParam::SendSms1L()
	{
	TRequestStatus status;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttPckg(iSmsAttr);
	TPtrC8 smsAsciiPdu(KSmsMessagingData);
	TBuf8<200>  smsPdu;
	SmsAtUtil::ConvertAsciiToBinary(smsAsciiPdu,smsPdu);
	//There is too long sca
	FillSmsAttibutes(KSmsSCA1());
	Sms().SendMessage(status,smsPdu,smsAttPckg);   
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("SendMessage() request status = %d"),r);
	}

void CTestLtsySmsSendInvalidParam::FillSmsAttibutes1(const TDesC& aTelNumber)
	{
	_LIT8(KSubmit, "Submit");
    iSmsAttr.iGsmServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    iSmsAttr.iGsmServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    iSmsAttr.iGsmServiceCentre.iTelNumber.Copy(aTelNumber);
    iSmsAttr.iCdmaServiceCategory = 0;
    iSmsAttr.iCdmaTeleservice = 0;
    iSmsAttr.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
    iSmsAttr.iFlags = 0;
    iSmsAttr.iMsgRef = 0;
    iSmsAttr.iSubmitReport = KSubmit;
    iSmsAttr.iMore = EFalse;
	}
void CTestLtsySmsSendInvalidParam::SendSms2L()
	{
	TRequestStatus status;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttPckg(iSmsAttr);
	TPtrC8 smsAsciiPdu(KSmsMessagingData);
	TBuf8<200>  smsPdu;
	SmsAtUtil::ConvertAsciiToBinary(smsAsciiPdu,smsPdu);
	//There is special sca
	FillSmsAttibutes(KSmsSCA2());
	Sms().SendMessage(status,smsPdu,smsAttPckg);   
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("SendMessage() request status = %d"),r);
	}

void CTestLtsySmsSendInvalidParam::FillSmsAttibutes2(const TDesC& aTelNumber)
	{
	_LIT8(KSubmit, "Submit");
	//There is different TypeOfNumber and NumberPlan
    iSmsAttr.iGsmServiceCentre.iTypeOfNumber = RMobilePhone::EUnknownNumber;
    iSmsAttr.iGsmServiceCentre.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    iSmsAttr.iGsmServiceCentre.iTelNumber.Copy(aTelNumber);
    iSmsAttr.iCdmaServiceCategory = 0;
    iSmsAttr.iCdmaTeleservice = 0;
    iSmsAttr.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
    iSmsAttr.iFlags = 0;
    iSmsAttr.iMsgRef = 0;
    iSmsAttr.iSubmitReport = KSubmit;
    iSmsAttr.iMore = EFalse;
	}

//End of file

