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
// Implements the TAvctpSockAddr to TSockAddr munging
// This is because all ESOCK functions that take a TSockAddr will only
// pass sizeof(TSockAddr) bytes through.
// 
//

/**
 @file
 @internalTechnology
*/

#include <bluetoothav.h>
#include <bluetooth/avctptypes.h>

using namespace SymbianAvctp;

EXPORT_C TAvctpSockAddr::TAvctpSockAddr()
/**
	c'tor
*/
	{
	SetPort(0);
	}

EXPORT_C TAvctpSockAddr::TAvctpSockAddr(const TSockAddr& aAddr)
	: TBTSockAddr(aAddr)
/**
	c'tor
*/
	{
	}

EXPORT_C TAvctpSockAddr& TAvctpSockAddr::Cast(const TSockAddr& aAddr)
	{
	return *((TAvctpSockAddr *)&aAddr);
	}

EXPORT_C TPid TAvctpSockAddr::Pid() const
	{
	return static_cast<TPid>(TSockAddr::Port() & 0xFFFF);
	}

EXPORT_C void TAvctpSockAddr::SetPid(const TPid aPid)
	{
	TSockAddr::SetPort(Port() | (aPid & 0xFFFF));
	}

EXPORT_C TInt TAvctpSockAddr::Channel() const
	{
	// get from upper 16bits of Port
	return static_cast<TInt>((TSockAddr::Port() >> 16) & 0xffff);
	}

EXPORT_C void TAvctpSockAddr::SetChannel(TInt aChannel)
	{
	// store in upper 16bits of Port
	TUint port = Port();
	port &= 0x0000ffff;
	TSockAddr::SetPort(port | ((aChannel & 0xffff) << 16));
	}
	
// EOF 
