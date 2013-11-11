//
// testltsysmssend.cpp
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

//user include
#include "testltsysmssend.h"
#include "testltsysmsatutil.h"
//constant defination
_LIT8(KSmsMessagingData,"11000D91683156211075f6000800064F60597D0021");
_LIT(KSmsSCA,"8613800100500");  

CTestLtsySmsSend::CTestLtsySmsSend(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsSend::~CTestLtsySmsSend()
	{
	}

CTestLtsySmsSend* CTestLtsySmsSend::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsSend* self = new (ELeave)CTestLtsySmsSend(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsSend* CTestLtsySmsSend::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsSend* self=CTestLtsySmsSend::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsSend::ConstructL()
	{

	}

TVerdict CTestLtsySmsSend::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsSend::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsSend::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsSend::doTestStepL called"));
	TRAPD(err,SendSmsL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when send message"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsSend::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsSend::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsySmsSend::SendSmsL()
	{
	TRequestStatus status;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsAttPckg(iSmsAttr);
	TPtrC8 smsAsciiPdu(KSmsMessagingData);
	TBuf8<200>  smsPdu;
	SmsAtUtil::ConvertAsciiToBinary(smsAsciiPdu,smsPdu);
	FillSmsAttibutes(KSmsSCA());
	Sms().SendMessage(status,smsPdu,smsAttPckg);   
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("SendMessage() request status = %d"),r);
	}

void CTestLtsySmsSend::FillSmsAttibutes(const TDesC& aTelNumber)
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

//End of file
