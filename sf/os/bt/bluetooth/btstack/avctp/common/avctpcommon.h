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

/**
 @file
 @internalTechnology
*/

#ifndef AVCTPCOMMON_H
#define AVCTPCOMMON_H

#include <bt_sock.h>
#include <remcon/remconserverid.h>
#include <bluetoothav.h>
#include <es_prot.h>
#include <bluetooth/avctptypes.h>

namespace SymbianAvctp
	{

	const TUint32 KDefaultSidforPids =  KRemConSrvTUint;
	
	const TInt KAvctpPrimaryChannel = 0;
	const TInt KAvctpSecondaryChannel = 1;
	const TInt KAvctpInvalidChannel = KMaxTInt;
	
	enum TControlIoctls
		{
		// commands from client to server
		EUndefinedIoctl = 0,
		EPrimaryChannelAttachToTransport = 1,
		ESecondaryChannelAttachToTransport = 2,
		EAwaitExtendedTransport = 3,
		EAwaitProvidedTransport = 4,
		EPrimaryChannelDetachFromTransport = 5,
		ESecondaryChannelDetachFromTransport = 6,
		EPrimaryChannelRefuseAttach = 7,
		ESecondaryChannelRefuseAttach = 8,
		EPrimaryChannelAgreeAttachment = 9,
		ESecondaryChannelAgreeAttachment = 10,
		// events from server to client
		EAttachIndicate = 11,
		EAttachConfirm = 12,
		EDetachConfirm = 13,
		EDetachIndicate = 14,
		ELinkUp = 15,
		ELinkDown = 16,
		EError = 17,
		};

	enum TSocketOpts
		{
		};
	}

NONSHARABLE_CLASS(TControlIoctlMessage)
	{
public:
	TControlIoctlMessage() : 
		iIoctl(SymbianAvctp::EUndefinedIoctl),
		iAddr(0)
		{
		}
	TControlIoctlMessage(SymbianAvctp::TControlIoctls aIoctl, const TBTDevAddr& aAddr, TInt aError = KErrNone) :
		iIoctl(aIoctl),
		iError(aError)
		{
		iAddr.SetBTAddr(aAddr);
		}
public:
	SymbianAvctp::TControlIoctls	iIoctl;
	TAvctpSockAddr					iAddr;
	TInt							iError;
	};

NONSHARABLE_CLASS(TOptionMessage)
	{
public:
	TBTDevAddr	iAddr;
	TInt		iMtu;
	};

#endif
// AVCTPCOMMON_H 

