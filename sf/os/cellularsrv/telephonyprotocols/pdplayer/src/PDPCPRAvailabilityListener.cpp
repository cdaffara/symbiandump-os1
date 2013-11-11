// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PDP Connection Provider Event Listener implementation
// 
//

/**
 @file
 @internalComponent
*/


#include "pdpcpravailabilitylistener.h"
#include <networking/cfbearers.h>
#include <networking/etelbearers.h>
#include <elements/nm_messages_base.h>
#include <pcktcs.h>

CPDPCPRDynamicCapsListener* CPDPCPRDynamicCapsListener::NewL(const CTSYProvision* aTsyProvision)
	{
	CPDPCPRDynamicCapsListener* self = new (ELeave) CPDPCPRDynamicCapsListener(aTsyProvision);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CPDPCPRDynamicCapsListener::ConstructL()
	{
	User::LeaveIfError(iTelServer.Connect());
	User::LeaveIfError(iTelServer.LoadPhoneModule(iTsyProvision->iTsyName));
	User::LeaveIfError(iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended));
	
	// open telephony server
	GetPhoneInfoL();
	
	//Open phone
	User::LeaveIfError(iPhone.Open(iTelServer, iPhoneInfo.iName));
	User::LeaveIfError(iPhone.Initialise());
	
	// get the service status
	User::LeaveIfError(iPacketService.Open(iPhone));
	User::LeaveIfError(iPacketService.GetStatus(iPacketServiceStatus));
	
	// set the dyncaps
	InitialiseDynamicCaps();
	}

void CPDPCPRDynamicCapsListener::InitialiseDynamicCaps()
	{
	if (iPacketServiceStatus == RPacketService::EStatusActive)
		{
		iPacketService.GetDynamicCaps(iDynCaps); // ignore the error for now, since this function is only used for initialization.
		}
	}

CPDPCPRDynamicCapsListener::CPDPCPRDynamicCapsListener(const CTSYProvision* aTsyProvision) 
	: CActive(EPriorityNormal), 
	  iTsyProvision(aTsyProvision), 
	  iObserver(NULL),
	  iDynCaps(0)
	{
	CActiveScheduler::Add(this);
	}

CPDPCPRDynamicCapsListener::~CPDPCPRDynamicCapsListener()
	{
	Cancel();
	iPacketService.Close();
	iPhone.Close();
	iTelServer.Close();
	}

void CPDPCPRDynamicCapsListener::GetPhoneInfoL()
	{
	TInt count;
	User::LeaveIfError(iTelServer.EnumeratePhones(count));
	if (count <= 0)
		{
		User::Leave(KErrNotFound);
		}

	TBool found = EFalse;
	for (TInt i = 0; i < count; i++)
		{
		TBuf<KCommsDbSvrMaxFieldLength> currentTsyName;
		User::LeaveIfError(iTelServer.GetTsyName(i,currentTsyName));

		TInt r = currentTsyName.Locate('.');
		if (r != KErrNotFound)
			{
			currentTsyName.SetLength(r);
			}
		if (currentTsyName.CompareF(iTsyProvision->iTsyName) == KErrNone)
			{
			User::LeaveIfError(iTelServer.GetPhoneInfo(i,iPhoneInfo));
			found = ETrue;
			break;
			}
		}

	if (!found)
		{
		User::Leave(KErrNotFound);
		}
	}

TBool CPDPCPRDynamicCapsListener::IsDynamicCapsSupported()
	{
	if (iPacketServiceStatus == RPacketService::EStatusActive)
		{
		if(iPacketService.GetDynamicCaps(iDynCaps) == KErrNone)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TUint CPDPCPRDynamicCapsListener::DynamicCaps()
	{
	return iDynCaps;
	}

void CPDPCPRDynamicCapsListener::NotifyDynamicCapsChange(MPDPCPRBearerObserver* aObserver)
	{
	if(!IsActive())
		{
	iObserver = aObserver;
	iPacketService.NotifyDynamicCapsChange(iStatus,iDynCaps);
	SetActive();
		}
	}

void CPDPCPRDynamicCapsListener::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	if (iObserver)
		{
		iObserver->BearerChangeDetectedL();
		}
	NotifyDynamicCapsChange(iObserver);
	}

void CPDPCPRDynamicCapsListener::DoCancel()
	{
	iPacketService.CancelAsyncRequest(EPacketNotifyDynamicCapsChange);
	}

TInt CPDPCPRDynamicCapsListener::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}


CPDPCPRNetworkModeListener* CPDPCPRNetworkModeListener::NewL(const CTSYProvision* aTsyProvision)
	{
	CPDPCPRNetworkModeListener* self = new (ELeave) CPDPCPRNetworkModeListener(aTsyProvision);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CPDPCPRNetworkModeListener::ConstructL()
	{
	User::LeaveIfError(iTelServer.Connect());
	User::LeaveIfError(iTelServer.LoadPhoneModule(iTsyProvision->iTsyName));
	User::LeaveIfError(iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended));
	
	// open telephony server
	GetPhoneInfoL();
	
	//Open phone
	User::LeaveIfError(iPhone.Open(iTelServer, iPhoneInfo.iName));
	User::LeaveIfError(iPhone.Initialise());
	InitialiseNetworkMode();
	}

void CPDPCPRNetworkModeListener::InitialiseNetworkMode()
	{
	iPhone.GetCurrentMode(iNetworkMode);
	}

CPDPCPRNetworkModeListener::CPDPCPRNetworkModeListener(const CTSYProvision* aTsyProvision) 
	: CActive(EPriorityNormal), 
	  iTsyProvision(aTsyProvision), 
	  iObserver(NULL),
	  iNetworkMode(RMobilePhone::ENetworkModeUnknown)
	{
	CActiveScheduler::Add(this);
	}

CPDPCPRNetworkModeListener::~CPDPCPRNetworkModeListener()
	{
	Cancel();
	iPhone.Close();
	iTelServer.Close();
	}

void CPDPCPRNetworkModeListener::GetPhoneInfoL()
	{
	TInt count;
	User::LeaveIfError(iTelServer.EnumeratePhones(count));
	if (count <= 0)
		{
		User::Leave(KErrNotFound);
		}

	TBool found = EFalse;
	for (TInt i = 0; i < count; i++)
		{
		TBuf<KCommsDbSvrMaxFieldLength> currentTsyName;
		User::LeaveIfError(iTelServer.GetTsyName(i,currentTsyName));

		TInt r = currentTsyName.Locate('.');
		if (r != KErrNotFound)
			{
			currentTsyName.SetLength(r);
			}
		if (currentTsyName.CompareF(iTsyProvision->iTsyName) == KErrNone)
			{
			User::LeaveIfError(iTelServer.GetPhoneInfo(i,iPhoneInfo));
			found = ETrue;
			break;
			}
		}

	if (!found)
		{
		User::Leave(KErrNotFound);
		}
	}

RMobilePhone::TMobilePhoneNetworkMode CPDPCPRNetworkModeListener::NetworkMode()
	{
	return iNetworkMode;
	}

void CPDPCPRNetworkModeListener::NotifyNetworkModeChange(MPDPCPRBearerObserver* aObserver)
	{
	if(!IsActive())
		{
	iObserver = aObserver;
	iPhone.NotifyModeChange(iStatus, iNetworkMode);
	SetActive();
		}
	}

void CPDPCPRNetworkModeListener::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	if (iObserver)
		{
		iObserver->BearerChangeDetectedL();
		}
	NotifyNetworkModeChange(iObserver);
	}

void CPDPCPRNetworkModeListener::DoCancel()
	{
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyModeChange);
	}

TInt CPDPCPRNetworkModeListener::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

