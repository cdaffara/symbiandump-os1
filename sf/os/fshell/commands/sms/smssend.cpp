// smssend.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <gsmuset.h>
#include "smssend.h"

CSmsSender* CSmsSender::NewL(RFs& aFs, MSmsSendObserver& aParent, const TDesC& aMsgToSend, const TDesC& aDestination)
	{
	CSmsSender* self = new (ELeave) CSmsSender(aParent, aFs, aMsgToSend, aDestination);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSmsSender::CSmsSender(MSmsSendObserver& aParent, RFs& aFs, const TDesC& aMsgToSend, const TDesC& aDestination):
CActive(CActive::EPriorityStandard), iParent(aParent), iFs(aFs), iMsgContent(aMsgToSend), iDestination(aDestination), iState(ESmsSenderIdle)
	{
	CActiveScheduler::Add(this);
	}

CSmsSender::~CSmsSender()
	{
	// iMsgBuffer doesn't need to be explicitly deleted. iMsgSend takes care of that
	if (iMsgToSend)
		delete iMsgToSend;
	if (iSmspList)
		delete iSmspList;
	if (iMobileMessaging.SubSessionHandle() > 0)
		iMobileMessaging.Close();
	if (iPhone.SubSessionHandle() > 0)
		iPhone.Close();
	if (iTelServer.Handle() > 0)
		iTelServer.Close();
	if (iSocket.SubSessionHandle())
		iSocket.Close();
	if (iSocketServer.Handle())
		iSocketServer.Close();
	}

void CSmsSender::ConstructL()
	{
	// connect to ETel. Note we're assuming the TSY has been loaded already
	RTelServer::TPhoneInfo phoneInfo;
	User::LeaveIfError(iTelServer.Connect());
	User::LeaveIfError(iTelServer.GetPhoneInfo(0, phoneInfo));
	
	// open the first phone object
	User::LeaveIfError(iPhone.Open(iTelServer, phoneInfo.iName));
	
	// open a hook to messaging functionality
	User::LeaveIfError(iMobileMessaging.Open(iPhone));
	
	// JoeF: I can't see a sane way to just retrieve the gsm service centre address from the sim?!
	iSmspList = CRetrieveMobilePhoneSmspList::NewL(iMobileMessaging);
	
	// connect & bind the sms socket
	User::LeaveIfError(iSocketServer.Connect());
	User::LeaveIfError(iSocket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol));
	iSmsAddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(iSocket.Bind(iSmsAddr));
	}

void CSmsSender::StartL()
	{
	ASSERT(!IsActive());
	
	// retrieve the gsm service centre number from the sim
	iSmspList->Start(iStatus);
	iState = ESmsSenderGetServiceCentre;
	SetActive();
	}

void CSmsSender::RunL()
	{
	TInt error = iStatus.Int();
	if (error != KErrNone)
		{
		iParent.SendComplete(error);
		return;
		}
	switch (iState)
		{
		case ESmsSenderGetServiceCentre:
			{
			error = GetServiceCentreAndSendSms();
			if (error == KErrNone)
				return;
			}
		break;
		
		case ESmsSenderSendMessage:
			iState = ESmsSenderIdle;
		break;
		
		default:
		break;
		};
	iParent.SendComplete(error);
	}

void CSmsSender::DoCancel()
	{
	switch (iState)
		{
		case ESmsSenderGetServiceCentre:
			iSmspList->Cancel();
		break;
		
		case ESmsSenderSendMessage:
			iSocket.CancelIoctl();
		break;
		
		default:
		break;
		};
	}

//
// CSmsSender::GetServiceCentreAndSendSms
// retrieve the service centre phone number & 
// create a correctly formatted sms message from
// the constiuent parts gathered. Send the sms
//
TInt CSmsSender::GetServiceCentreAndSendSms()
	{
	TRAPD(err, RetrieveServiceCentreL());
	if (err == KErrNone)
		TRAP(err, CreateAndSendSmsL());
	return err;
	}

//
// CSmsSender::RetrieveServiceCentreL
// extract the service centre from the smsp list
//
void CSmsSender::RetrieveServiceCentreL()
	{
	if (iSmspList)
		{
		CMobilePhoneSmspList* list = iSmspList->RetrieveListL();
		if (list)
			{
			TInt count = list->Enumerate();
			while (--count >= 0)
				{
				const RMobileSmsMessaging::TMobileSmspEntryV1& entry = list->GetEntryL(count);
				if (entry.iServiceCentre.iTelNumber.Length() > 0)
					{
					iServiceCentre.iTypeOfNumber = entry.iServiceCentre.iTypeOfNumber;
					iServiceCentre.iNumberPlan = entry.iServiceCentre.iNumberPlan;
					iServiceCentre.iTelNumber = entry.iServiceCentre.iTelNumber;
					return;
					}
				}
			}
		}
	User::Leave(KErrNotFound);
	}

//
// CSmsSender::CreateAndSendSmsL
// create a correctly formatted sms from the available data
// send it on its way via smsstack
//
void CSmsSender::CreateAndSendSmsL()
	{
	TSmsUserDataSettings settings;
	settings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
	settings.SetTextCompressed(EFalse);
	
	iMsgBuffer = CSmsBuffer::NewL();
	iMsgBuffer->InsertL(0, iMsgContent);
	iMsgToSend = CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit, iMsgBuffer);
	iMsgToSend->SetUserDataSettingsL(settings);
	iMsgToSend->SetToFromAddressL(iDestination);
	iMsgToSend->SmsPDU().SetServiceCenterAddressL(iServiceCentre.iTelNumber);
	
	ASSERT(iSocket.SubSessionHandle() > 0);
	RSmsSocketWriteStream writestream(iSocket);
	writestream << *iMsgToSend;
	writestream.CommitL();

	TPckgBuf<TUint> sbuf;
	iSocket.Ioctl(KIoctlSendSmsMessage, iStatus, &sbuf, KSolSmsProv);
	SetActive();
	iState = ESmsSenderSendMessage;
	}



