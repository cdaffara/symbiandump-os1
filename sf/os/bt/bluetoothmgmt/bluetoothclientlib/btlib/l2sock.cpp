// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the TL2CAPSockAddr to TSockAddr munging
// This is because all ESOCK functions that take a TSockAddr will only
// pass sizeof(TSockAddr) bytes through.
// 
//

#include <bt_sock.h>

EXPORT_C TL2CAPSockAddr::TL2CAPSockAddr()
/** Constructor
*/
	{
	}

EXPORT_C TL2CAPSockAddr::TL2CAPSockAddr(const TSockAddr &aAddr)
	: TBTSockAddr(aAddr)
/** Construct from a BT address.
@param aAddr BT address used to populate the object.
*/
	{
	}

EXPORT_C TL2CAPSockAddr& TL2CAPSockAddr::Cast(const TSockAddr &aAddr)
	{
	return *((TL2CAPSockAddr *)&aAddr);
	}

EXPORT_C TUint16 TL2CAPSockAddr::Port() const
	{
	return static_cast<TUint16>(TSockAddr::Port());
	}

EXPORT_C void TL2CAPSockAddr::SetPort(const TUint16 aHomePort)
	{
	TSockAddr::SetPort(aHomePort);
	}

// EOF
