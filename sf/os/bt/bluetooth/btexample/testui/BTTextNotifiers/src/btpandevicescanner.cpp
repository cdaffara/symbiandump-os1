// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "BTTextNotifiers.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, "TextNotifiers");
#endif

CBTPanDeviceSelectionNotifierDeviceDiscoverer* CBTPanDeviceSelectionNotifierDeviceDiscoverer::NewL(MBTPanDeviceDiscoveryNotify& aNotify)
/**

*/
	{
	LOG_STATIC_FUNC
	CBTPanDeviceSelectionNotifierDeviceDiscoverer* self = new(ELeave) CBTPanDeviceSelectionNotifierDeviceDiscoverer(aNotify);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBTPanDeviceSelectionNotifierDeviceDiscoverer::CBTPanDeviceSelectionNotifierDeviceDiscoverer(MBTPanDeviceDiscoveryNotify& aNotify) :
	CActive(EPriorityStandard), iNotify(aNotify)
/**
Set the inquiry address to the general inquiry access code, and ask for names as well
*/
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	iInquiryAddr.SetIAC(KGIAC);
	iInquiryAddr.SetAction(KHostResInquiry + KHostResName);	// Default: Both
	}

void CBTPanDeviceSelectionNotifierDeviceDiscoverer::ConstructL()
/**
Connect to the socket server and open the host resolver
*/
	{
	LOG_FUNC
	LEAVEIFERRORL(iSockServ.Connect());
	LEAVEIFERRORL(iHostResolver.Open(iSockServ, KBTAddrFamily, KBTLinkManager));
	}

CBTPanDeviceSelectionNotifierDeviceDiscoverer::~CBTPanDeviceSelectionNotifierDeviceDiscoverer()
/**
Free up resources
*/
	{
	LOG_FUNC
	iHostResolver.Close();
	iSockServ.Close();
	}

void CBTPanDeviceSelectionNotifierDeviceDiscoverer::StartDeviceDiscovery()
/**

*/
	{
	LOG_FUNC
	iHostResolver.GetByAddress(iInquiryAddr, iNameEntry, iStatus);
	SetActive();
	}

void CBTPanDeviceSelectionNotifierDeviceDiscoverer::RunL()
/**
Tell the notify class about the new device or error
*/
	{
	LOG_FUNC
	LOG1(_L("\tiStatus = %d"), iStatus.Int());

	if(iStatus==KErrNone)
		{
		TRAPD(err, iNotify.NewDeviceFoundL(iNameEntry()));
		if(err)
			{
			iNotify.DeviceSearchComplete(err);
			return;
			}
		iHostResolver.Next(iNameEntry, iStatus);
		SetActive();
		}
	else
		{
		iNotify.DeviceSearchComplete(iStatus.Int());
		}
	}
	
void CBTPanDeviceSelectionNotifierDeviceDiscoverer::DoCancel()
/**

*/
	{
	LOG_FUNC
	iHostResolver.Cancel();
	}




