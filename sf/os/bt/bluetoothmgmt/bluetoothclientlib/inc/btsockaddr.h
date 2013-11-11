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

#ifndef BTSOCKADDR_H
#define BTSOCKADDR_H

/**
@file
@publishedAll
@released
*/

#include <es_sock.h>
#include <bttypes.h>
#include <btsecurity.h>

NONSHARABLE_CLASS(TBTSockAddr) : public TSockAddr
/** Bluetooth socket address

Stores bluetooth device address, and security - these are common to all Bluetooth addresses
@publishedAll
@released
*/
	{
struct SBTAddrSecurity
	{		
	TBTDevAddr iAddress;
	TBTServiceSecurity iSecurity;
	};
	
public:
	IMPORT_C TBTSockAddr();
	IMPORT_C TBTSockAddr(const TSockAddr& aAddr);
	IMPORT_C TBTDevAddr BTAddr() const;
	IMPORT_C void SetBTAddr(const TBTDevAddr& aRemote);
	IMPORT_C void SetSecurity(const TBTServiceSecurity& aSecurity);
	IMPORT_C TBTServiceSecurity BTSecurity() const;
	IMPORT_C static TBTSockAddr& Cast(const TSockAddr& aAddr);
	
protected:
	IMPORT_C TAny* EndBTSockAddrPtr() const;
	
private:
	SBTAddrSecurity& BTAddrSecStruct() const;
	TPtr8 AddressPtr() const;
	};

#endif // BTSOCKADDR_H
