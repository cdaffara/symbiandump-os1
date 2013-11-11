// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 

//user include
#include "testltsysmsstore.h"
#include "testltsysmsatutil.h"

CTestLtsySmsStore::CTestLtsySmsStore(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsStore::~CTestLtsySmsStore()
	{
	}

CTestLtsySmsStore* CTestLtsySmsStore::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsStore* self = new (ELeave)CTestLtsySmsStore(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsStore* CTestLtsySmsStore::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsStore* self=CTestLtsySmsStore::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsStore::ConstructL()
	{

	}

TVerdict CTestLtsySmsStore::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsStore::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsStore::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsStore::doTestStepL called"));
	TRAPD(err,StoreSmsL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when store sms"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsStore::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsStore::doTestStepPostambleL called"));
	return TestStepResult();
	}
void CTestLtsySmsStore::StoreSmsL()
	{
	ReceiveSmsL();
	TRAPD(err,WriteSmsL());
	if(err != KErrNone)
		{
		NackSmsStoredL();
		}
	else
		{
		AckSmsStoredL();
		}
	}

void CTestLtsySmsStore::ReceiveSmsL()
	{
	TRequestStatus status;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg receiveAttrPckg(iReceiveAttr);
	Sms().ReceiveMessage(status, iRecvMsg, receiveAttrPckg);
	User::WaitForRequest(status);
	TInt r = status.Int();
	TBuf<200>  outReceive;
	outReceive.Copy(iRecvMsg);
	INFO_PRINTF2(_L("Receiveing TPDU=%S"),&outReceive);
	INFO_PRINTF2(_L("ReceiveMessage() request status = %d"),r);
	}

void CTestLtsySmsStore::WriteSmsL()
	{
	TRequestStatus status;
	RMobilePhone::TMobileAddress smsWriteAddr;
	smsWriteAddr.iTypeOfNumber = iReceiveAttr.iGsmServiceCentre.iTypeOfNumber;
	smsWriteAddr.iNumberPlan = iReceiveAttr.iGsmServiceCentre.iNumberPlan;
	smsWriteAddr.iTelNumber = iReceiveAttr.iGsmServiceCentre.iTelNumber;
	
	//For GsmTpdu
    TBuf8<400>   writePduData; 
    writePduData =  iRecvMsg;
    FillWriteSmsAttibutesL(writePduData,smsWriteAddr);
	}

void CTestLtsySmsStore::AckSmsStoredL()
	{
	TBool full(EFalse);
	TRequestStatus status;
	_LIT8(KNullAck,"00");
	Sms().AckSmsStored(status,KNullAck(),full);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("AckSmsStored() request status = %d"),r);
	}

void CTestLtsySmsStore::NackSmsStoredL()
	{
	TRequestStatus status;
	TInt rpCause(2);  
	
	Sms().NackSmsStored(status,iRecvMsg,rpCause);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("AckSmsStored() request status = %d"),r);
	}

void CTestLtsySmsStore::FillWriteSmsAttibutesL(TBuf8<400> aTpdu,RMobilePhone::TMobileAddress  aMsgSca)
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
