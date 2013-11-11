//
// testltsysmsreceive.cpp
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

//user include
#include "testltsysmsreceive.h"
#include "smsatutil.h"


CTestLtsySmsReceive::CTestLtsySmsReceive(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsReceive::~CTestLtsySmsReceive()
	{
	}

CTestLtsySmsReceive* CTestLtsySmsReceive::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsReceive* self = new (ELeave)CTestLtsySmsReceive(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsReceive* CTestLtsySmsReceive::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsReceive* self=CTestLtsySmsReceive::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsReceive::ConstructL()
	{

	}

TVerdict CTestLtsySmsReceive::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsReceive::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsReceive::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsReceive::doTestStepL called"));
	TRAPD(err,ReceiveSmsL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when receive sms"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsReceive::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsReceive::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsySmsReceive::ReceiveSmsL()
	{
	TRequestStatus status;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 receiveAttr;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(receiveAttr);
	Sms().ReceiveMessage(status, iRecvMsg, receiveAttrPckg);
	User::WaitForRequest(status);
	TInt r = status.Int();
	TBuf<200>  outReceive;
	outReceive.Copy(iRecvMsg);
	INFO_PRINTF2(_L("Receiveing TPDU=%S"),&outReceive);
	INFO_PRINTF2(_L("ReceiveMessage() request status = %d"),r);
	}


