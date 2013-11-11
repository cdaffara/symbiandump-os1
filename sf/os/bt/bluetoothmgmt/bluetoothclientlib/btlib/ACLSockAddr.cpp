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

#include <bt_sock.h>
#include <bluetooth/aclsockaddr.h>

EXPORT_C TACLSockAddr::TACLSockAddr()
/** Constructor
*/
	{
	}

EXPORT_C TACLSockAddr::TACLSockAddr(const TSockAddr &aAddr)
	: TBTSockAddr(aAddr)
/** Construct from a BT address.
@param aAddr BT address used to populate the object.
*/
	{
	}

EXPORT_C TACLSockAddr& TACLSockAddr::Cast(const TSockAddr &aAddr)
	{
	return *((TACLSockAddr*)&aAddr);
	}

EXPORT_C TACLPort TACLSockAddr::Port() const
	{
	return static_cast<TACLPort>(TSockAddr::Port());
	}

EXPORT_C void TACLSockAddr::SetPort(TACLPort aPort)
	{
	TSockAddr::SetPort(aPort);
	}

// EOF
