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


#if !defined(IDQUERYNETMSG_H)
#define IDQUERYNETMSG_H

#include <elements/netmessages.h>
#include <elements/h_trbuf.h>

namespace NetMessages
{

/** Types of an KNetMessageInterface interface represented by CNetMessage. Could be exteded either by native types
less than KNetMessageInterfacePrivate or user types greater than KNetMessageInterfacePrivate
@released since 9.1
*/
enum TImplementations
	{
/** Any message type */
	EMessageAny = Meta::KNetMetaTypeAny,
/** Message type for a factory instance to return a handle to an object */
	ETypeIdQueryId,
/** Message type for a factory instance to return a handle to an object with additional data*/
	ETypeIdQueryDataId,

/** Custom message classes starts here the number should make sure that custom messages will not
	overlap the one in NetMessage.dll*/
	EInterfacePrivate = 128
	};

/** Factory commands
@see RRootServ::SendMessage
@see CTypeIdQuery
@released since 9.1
*/
const TInt KHandleRelease = 1;
const TInt KHandleAttach = 2;

const TInt KInterfaceUid = 0x102046A4;

/** Max length of CTypeIdQuery::iOid
@see CTypeIdQuery
@released since 9.1
*/
const TInt KMaxOIDLength = 32;

/** Message type for a factory instance to return a handle to an object
@released since 9.1
*/
class CTypeIdQuery : public CMessage
	{
	NET_MESSAGE_DECL( CTypeIdQuery, KInterfaceUid, ETypeIdQueryId )

	TInt32 iUid;		//[in] interface id (determines the factory scope e.g KProtocolFamilyInterfaceUidinstance)
	TInt32 iTypeId;	//[in] type id in the iUid scope (e.g IP protocol family in the scope of protocol families)
	TInt	iHandle; //[in/out] [in] - iHandle !=NULL => release the handle iHandle == NULL get the handle
	TBuf8<KMaxOIDLength> iOid;   //[in] Object identification (in a form the particular factory understands)
	};

/** Message type for a factory instance to return a handle to an object with additional data
@released since 9.1
*/
class CTypeIdDataQuery : public CTypeIdQuery
	{
	NET_MESSAGE_DECL( CTypeIdDataQuery, KInterfaceUid, ETypeIdQueryDataId )

	Elements::TRBuf iData; //[in/out] internal data exchange on creation
	};
}
#endif	// IDQUERYNETMSG_H

