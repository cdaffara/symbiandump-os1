// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// gprsavailabilitylistener.cpp
// 
//

#include <etelmm.h>
#include <etelpckt.h>
#include <pcktcs.h>
#include <comms-infras/es_availability.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include "psdavailabilitylistener.h"
#include <elements/nm_messages_base.h>
#include <comms-infras/ss_nodemessages_availability.h>

using namespace Messages;
using namespace ESock;

 void GetPhoneInfoL(RTelServer& aTelServer, const TDesC& aLoadedTsyName, RTelServer::TPhoneInfo& aInfo)
	{
	TInt count;
	User::LeaveIfError(aTelServer.EnumeratePhones(count));
	if (count<=0)
		{
		User::Leave(KErrNotFound);
		}

	TBool found = EFalse;
	for (TInt i=0; i < count; i++)
		{
		TBuf<KCommsDbSvrMaxFieldLength> currentTsyName;
		User::LeaveIfError(aTelServer.GetTsyName(i,currentTsyName));

		TInt r=currentTsyName.Locate('.');
		if (r!=KErrNotFound)
			{
			currentTsyName.SetLength(r);
			}
		if (currentTsyName.CompareF(aLoadedTsyName)==KErrNone)
			{
			User::LeaveIfError(aTelServer.GetPhoneInfo(i,aInfo));
			found = ETrue;
			break;
			}
		}

	if (!found)
		{
		User::Leave(KErrNotFound);
		}
	}

CPsdAvailabilityListener* CPsdAvailabilityListener::NewL(const TNodeCtxId& aAvailabilityActivity, const CTSYProvision& aTsyProvision, TUint aApId, TBool aUnavailableDueToContention)
	{
	CPsdAvailabilityListener* self = new (ELeave) CPsdAvailabilityListener(aAvailabilityActivity, aTsyProvision, aApId, aUnavailableDueToContention);
	CleanupStack::PushL(self);
	self->StartL();
	CleanupStack::Pop(self);
	return self;
	}

// CActive
void CPsdAvailabilityListener::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EInitialising:
			//Open packet service
			User::LeaveIfError(iPacketService.Open(iPhone));

#ifdef _DEBUG
			// Current version of SIM TSY does not support RPacketService::EAttachWhenPossible
			if (iTsyProvision->iTsyName == _L("SIM"))
				iPacketService.SetAttachMode(iStatus, RPacketService::EAttachWhenNeeded);
			else
#endif
			//Put phone is attachwhenpossible mode.
			//In this mode, the phone will try to attach to the gprs network whenever it can.
			iPacketService.SetAttachMode(iStatus, RPacketService::EAttachWhenPossible);
			iState = EChecking;
			SetActive();
			break;
		case EChecking:
			User::LeaveIfError(iPacketService.GetStatus(iPsdStatus)); // get initial status
			//Fall through to EAttached / EUnAttached
		case EAttached:
		case EUnAttached:
			if (iPsdStatus == RPacketService::EStatusAttached
				|| iPsdStatus == RPacketService::EStatusActive)
				{
				if (iState!=EAttached) //Could be EUnAttached or EChecking
					{
					if (!iUnavailableDueToContention)
						{
						TAvailabilityStatus availabilityStatus(TAvailabilityStatus::EMaxAvailabilityScore);
						RClientInterface::OpenPostMessageClose(Id(), iAvailabilityActivity,
							TCFAvailabilityControlClient::TAvailabilityNotification(availabilityStatus).CRef());
						}
					iState = EAttached;
					}
				}
			else
				{
				if (iState!=EUnAttached) //Could be EAttached or EChecking
					{
					// Do not report if we have the contention, coz we have already done it
					if (!iUnavailableDueToContention)
						{
						TAvailabilityStatus availabilityStatus(TAvailabilityStatus::EMinAvailabilityScore);
						RClientInterface::OpenPostMessageClose(Id(), iAvailabilityActivity,
							TCFAvailabilityControlClient::TAvailabilityNotification(availabilityStatus).CRef());
						}
					iState = EUnAttached;
					}
				}
			iPacketService.NotifyStatusChange(iStatus, iPsdStatus);
			SetActive();
			break;

		default:
			ASSERT(EFalse);
		}
	}

TInt CPsdAvailabilityListener::RunError(TInt aError)
	{
	RClientInterface::OpenPostMessageClose(Id(), iAvailabilityActivity,
		TEBase::TError(TCFAvailabilityProvider::TAvailabilityNotificationRegistration::Id(), aError).CRef());

	return KErrNone;
	}

void CPsdAvailabilityListener::DoCancel()
	{
	switch (iState)
		{
		case EInitialising:
			iPhone.InitialiseCancel();
			break;
		case EChecking:
			iPacketService.CancelAsyncRequest(EPacketSetAttachMode);
			//Fall through to EAttached / EUnAttached
		case EAttached:
		case EUnAttached:
			iPacketService.CancelAsyncRequest(EPacketNotifyStatusChange);
			break;
		default:
			ASSERT(EFalse);
		}
	}

CPsdAvailabilityListener::~CPsdAvailabilityListener()
	{
	Cancel();
	iPacketService.Close();
	iPhone.Close();
	iTelServer.Close();
	}

CPsdAvailabilityListener::CPsdAvailabilityListener(const TNodeCtxId& aAvailabilityActivity, const CTSYProvision& aTsyProvision, TUint aApId, TBool aUnavailableDueToContention)
:	CActive(EPriorityNormal),
	iAvailabilityActivity(aAvailabilityActivity),
	iTsyProvision(&aTsyProvision),
	iApId(aApId),
	iState(EInitialising),
	iUnavailableDueToContention(aUnavailableDueToContention)
	{
	CActiveScheduler::Add(this);
	}

//We could use asynch variants here as CPsdAvailabilityListener::StartL can currently block for upto a minute..
void CPsdAvailabilityListener::StartL()
	{
	User::LeaveIfError(iTelServer.Connect());
	User::LeaveIfError(iTelServer.LoadPhoneModule(iTsyProvision->iTsyName));
	User::LeaveIfError(iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended));

	//Open telephony server
	RTelServer::TPhoneInfo phoneInfo;
	GetPhoneInfoL(iTelServer, iTsyProvision->iTsyName, phoneInfo);

	//Open phone
	User::LeaveIfError(iPhone.Open(iTelServer, phoneInfo.iName));

	RPhone::TStatus phoneStatus;
	User::LeaveIfError(iPhone.GetStatus(phoneStatus));

	if(phoneStatus.iMode==RPhone::EModeUnknown)
		{
		iPhone.Initialise(iStatus);
		}
	else
		{
        // Kick off this class' mini state machine in the RunL
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	SetActive();
	}

void CPsdAvailabilityListener::ReceivedL(const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
	{
	(void)aMessage;
	ASSERT(TEBase::ERealmId == aMessage.MessageId().Realm());
    ASSERT(TEBase::TCancel::EId == aMessage.MessageId().MessageId());
	delete this;
	}

#ifdef SYMBIAN_NETWORKING_CONTENTION_MANAGEMENT
void CPsdAvailabilityListener::ReportContentionAvailabilityStatusIfRequired(const TAvailabilityStatus& aAvailabilityStatus)
	{
	if (aAvailabilityStatus.Score() == TAvailabilityStatus::EMinAvailabilityScore && !iUnavailableDueToContention)
		{
		if (iState == EAttached)
			{
			RClientInterface::OpenPostMessageClose(Id(), iAvailabilityActivity,
				TCFAvailabilityControlClient::TAvailabilityNotification(aAvailabilityStatus).CRef());
			}
		iUnavailableDueToContention = ETrue;
		}
	else if (aAvailabilityStatus.Score() > TAvailabilityStatus::EMinAvailabilityScore && iUnavailableDueToContention)
		{
		if (iState == EAttached)
			{
			RClientInterface::OpenPostMessageClose(Id(), iAvailabilityActivity,
				TCFAvailabilityControlClient::TAvailabilityNotification(aAvailabilityStatus).CRef());
			}
		// if iState is not EAttached just reset iUnavailableDueToContention flag, 
		// the availability will be reported in RunL when network will become available
		iUnavailableDueToContention = EFalse;
		}
	}
#endif

