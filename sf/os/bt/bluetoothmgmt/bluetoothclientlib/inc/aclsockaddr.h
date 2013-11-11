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

#ifndef ACL_SOCK_H
#define ACL_SOCK_H

#include <bt_sock.h>

/** 
@file
@internalComponent
*/

enum TACLPort
/** ACL port types.
@internalComponent
*/
	{
	EACLPortRaw		= 0x00,		/*!< Raw port type */
	EACLPortL2CAP	= 0x01,		/*!< L2CAP port type */
	EACLPortUnset	= 0xFF,		/*!< Unspecified ACL port type */
	};


// SAP types for baseband
/** ACL socket type
@internalTechnology
*/
static const TUint KSockBluetoothTypeACL = KMaxTUint;
/** Raw broadcast socket type
@internalTechnology
*/
static const TUint KSockBluetoothTypeRawBroadcast = KMaxTUint-2;

NONSHARABLE_CLASS(TACLSockAddr) : public TBTSockAddr
/** ACL Socket Address.

Use this class to specify a local or remote baseband connection end-point,
This is tied to the flags field in ACL data packets
@internalComponent
*/
	{
public:
	IMPORT_C TACLSockAddr();
	IMPORT_C TACLSockAddr(const TSockAddr& aSockAddr);
	IMPORT_C static TACLSockAddr& Cast(const TSockAddr& aSockAddr);
	//
	IMPORT_C TACLPort Port() const;
	IMPORT_C void SetPort(TACLPort aPort);
	};


#endif // ACL_SOCK_H
