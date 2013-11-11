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

#include "Te_SimSmsTestStepBase.h"
#include "Te_SimSmsUtil.h"

TVerdict CSimSmsTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 0);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed resetting"));
		User::Leave(ret);
		}

	iTestScheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);	

	ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		delete iTestScheduler;
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to initialise the phone"));
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	ret=iSms.Open(iPhone);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Cannot open Sms"));
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CSimSmsTestStepBase::doTestStepPostambleL()
	{
	iSms.Close();
	iPhone.Close();
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();
	delete iTestScheduler;

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CSimSmsTestStepBase::PrintMessageStoreCaps(TUint32 aCaps)
	{
	if(aCaps & RMobilePhoneStore::KCapsReadAccess)
	{
		INFO_PRINTF1(_L("Store supports read access"));
	}
	if(aCaps & RMobilePhoneStore::KCapsWriteAccess)
	{
		INFO_PRINTF1(_L("Store supports write/delete access"));
	}
	if(aCaps & RMobilePhoneStore::KCapsDeleteAll)
	{
		INFO_PRINTF1(_L("Store supports deleting all entries at once"));
	}
	if(aCaps & RMobilePhoneStore::KCapsNotifyEvent)
	{
		INFO_PRINTF1(_L("Store supports event notification"));
	}
	if(aCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore))
	{
		INFO_PRINTF1(_L("Store supports reading whole store in a list"));
	}
	if(aCaps & RMobilePhoneStore::KCapsIndividualEntry)
	{
		INFO_PRINTF1(_L("Store supports reading entries one at a time"));
	}
	if(aCaps & RMobileSmsStore::KCapsUnreadMessages)
	{
		INFO_PRINTF1(_L("Store contains unread messages"));
	}
	if(aCaps & RMobileSmsStore::KCapsReadMessages)
	{
		INFO_PRINTF1(_L("Store contains read messages"));
	}
	if(aCaps & RMobileSmsStore::KCapsSentMessages)
	{
		INFO_PRINTF1(_L("Store contains sent messages"));
	}
	if(aCaps & RMobileSmsStore::KCapsUnsentMessages)
	{
		INFO_PRINTF1(_L("Store contains unsent messages"));
	}
	if(aCaps & RMobileSmsStore::KCapsGsmMessages)
	{
		INFO_PRINTF1(_L("Store contains GSM format messages"));
	}
	if(aCaps & RMobileSmsStore::KCapsCdmaMessages)
	{
		INFO_PRINTF1(_L("Store contains CDMA format messages"));
	}
}

TVerdict CSimSmsTestStepBase::SendSms()
{
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

		if(ret) return EFail;
		msgData.Append(TChar(val));
		}

	TBuf8<20> targetAddressAscii;
	targetAddressAscii.Zero();
	RMobilePhone::TMobileAddress targetAddress;
	targetAddress.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	targetAddress.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	targetAddress.iTelNumber.Copy(iTelNumber);
	ret = CATSmsUtils::AppendAddressToAscii(targetAddressAscii,targetAddress);

	if(ret) return EFail;

	const TInt count_address(targetAddressAscii.Length()/2);	// Assume length is always even
	for(i=0;i<count_address;++i)
		{
		lex=targetAddressAscii.Mid(i*2,2);
		ret = lex.Val(val,EHex);					

		if(ret) return EFail;
		msgData.Append(TChar(val));
		}

	const TInt count_after((&KMsgDataAfterTargetAddress)->Length()/2);	// Assume length of pdu is always even
	for(i=0;i<count_after;++i)
		{
		lex=(&KMsgDataAfterTargetAddress)->Mid(i*2,2);
		ret = lex.Val(val,EHex);

		if(ret) return EFail;
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

	iSms.SendMessage(status,smsPduSent,msgAttrPkg);
	User::WaitForRequest(status);
	if(status==KErrGsmSMSFailureInME || status==KErrGeneral || status==KErrGsmSMSUnknownError)
		{
		INFO_PRINTF1(_L(".."));
		iSms.SendMessage(status,smsPduSent,msgAttrPkg);
		User::WaitForRequest(status);
		if(status==KErrGsmSMSFailureInME || status==KErrGeneral || status==KErrGsmSMSUnknownError)
			{
			INFO_PRINTF1(_L(".."));
			iSms.SendMessage(status,smsPduSent,msgAttrPkg);
			User::WaitForRequest(status);
			}
		}	
	
	ret = status.Int();

	if(ret) return EFail;

	// Validate message reference number has changed 
	if(!(msgAttr.iFlags&RMobileSmsMessaging::KMessageReference))
		return(EFail);

	INFO_PRINTF1(_L("TESTS passed"));
	return TestStepResult(); 
}
