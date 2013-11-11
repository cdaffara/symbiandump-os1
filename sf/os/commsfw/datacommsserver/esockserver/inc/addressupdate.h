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
 @publishedPartner
 @released
*/

#ifndef __ADDRESS_UPDATE_H__
#define __ADDRESS_UPDATE_H__

#include <es_sock.h>
#include <elements/nm_signatures.h>

namespace ESock
{
struct TAddrUpdate
/**
Structure to hold the details of a routed data client. Contains the source address,
destination address, protocol id and IAP id of the socket.
 */
    {
	/**
	Source address of the socket
	 */
    TSockAddr iSrcSockAddr;
	/**
	Destination address of the socket
	*/
    TSockAddr iDestSockAddr;
	/**
	Protocol id of the socket
	@see RSocket::Open
	*/
    TInt iProtocolId;
	/**
	Internet access point id of the socket
	*/
    TInt iIapId;

    TAddrUpdate(TSockAddr &aSrcSockAddr, TSockAddr& aDestSockAddr, TInt aProtocolId, TInt aIapId)
        :iSrcSockAddr(aSrcSockAddr),
         iDestSockAddr(aDestSockAddr),
         iProtocolId(aProtocolId),
         iIapId(aIapId)
         {}

    TAddrUpdate()
        :iProtocolId(-1),
         iIapId(-1)
        {}
    };

DECLARE_MESSAGE_SIG_1(SigAddrUpdate, TAddrUpdate, AddrUpdate)

} // namespace ESock

#endif // __ADDRESS_UPDATE_H__

