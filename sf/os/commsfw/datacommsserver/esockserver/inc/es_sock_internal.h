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

/**
 @file
 @Internal
*/

#if !defined(__ES_SOCK_INTERNAL_H__)
#define __ES_SOCK_INTERNAL_H__


#include <es_sock.h>

/**
Name of ESock server in EKA2
@internalComponent
*/
_LIT(SOCKET_SERVER_NAME, "!SocketServer");


/**
Id of the network layer.
@internalTechnology
@released
*/
const TUint KCommsNetworkLayerId = 0x10281DD8; //CNetTierManagerFactory::iUid

/**
Ioctls

Must not be set for client requests

@internalComponent
@released
*/
const TUint KInternalIoctlBit=0x80000000;



class TSockOpen
/** @internalComponent */
	{
public:
	TUint iAddrFamily;
	TUint iSockType;
	TUint iProtocol;
	TInt  iHandle;
	TInt  iReserved;
	};

class TSockOpenBufC : public TPckgC<TSockOpen>
/** @internalComponent */
	{
public:
	inline TSockOpenBufC();
	TSockOpen iArgs;
	};




#include <es_sock_internal.inl>

#endif // __ES_SOCK_INTERNAL_H__

