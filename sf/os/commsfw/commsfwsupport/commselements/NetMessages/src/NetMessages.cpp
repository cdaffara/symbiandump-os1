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

/**
 @file
 @internalTechnology
*/

#include <elements/netmessages.h>
#include <elements/idquerynetmsg.h>
#include <ecom/implementationproxy.h>

using namespace NetMessages;

REGISTER_TYPEID( CMessage, NetMessages::KInterfaceUid, EMessageAny )

START_ATTRIBUTE_TABLE( CTypeIdQuery, NetMessages::KInterfaceUid, ETypeIdQueryId )
	REGISTER_ATTRIBUTE( CTypeIdQuery, iUid, TMeta<TInt32> )
	REGISTER_ATTRIBUTE( CTypeIdQuery, iTypeId, TMeta<TInt32> )
	REGISTER_ATTRIBUTE( CTypeIdQuery, iHandle, TMetaNumber )
	REGISTER_ATTRIBUTE( CTypeIdQuery, iOid, TMeta< TBuf8<KMaxOIDLength> > )
END_ATTRIBUTE_TABLE_BASE( CMessage, 0 )

START_ATTRIBUTE_TABLE( CTypeIdDataQuery, NetMessages::KInterfaceUid, ETypeIdQueryDataId )
	REGISTER_ATTRIBUTE( CTypeIdDataQuery, iData, TMetaBuf )
END_ATTRIBUTE_TABLE_BASE( CTypeIdQuery, 0 )

/**
ECOM Implementation Factories
*/

class CNetMsgFactory : public CBase
/**
Connection generic event factory.

@internalComponent
@released since v9.2
*/
    {
public:
    static Meta::SMetaDataECom* InstantiateL(TAny* aParams);
    };

/**
 * @internalTechnology
 */
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KInterfaceUid, CNetMsgFactory::InstantiateL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

Meta::SMetaDataECom* CNetMsgFactory::InstantiateL(TAny* aParams)
	{
	TInt32 type = reinterpret_cast<TInt32>(aParams);

	Meta::SMetaDataECom* newInst = 0;

	switch (type)
		{
	case (ETypeIdQueryId):
		newInst = new (ELeave) CTypeIdQuery;
		break;
	case (ETypeIdQueryDataId):
		newInst = new (ELeave) CTypeIdDataQuery;
		break;
	default:
		User::Leave(KErrNotFound);
		}
	return newInst;
	}


