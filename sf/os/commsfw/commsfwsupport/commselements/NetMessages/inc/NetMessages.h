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


#if !defined(NETMESSAGES_H)
#define NETMESSAGES_H

#include <elements/metadata.h>
#include <elements/metabuffer.h>

#define NET_MESSAGE_DECL( _class, _uid, _type ) \
public: \
	static inline _class * NewL(void) \
		{ Meta::STypeId typeId = { (_uid ) , (_type) }; \
	  return static_cast< _class *>(Meta::SMetaDataECom::NewInstanceL(typeId)); } \
	static inline _class * LoadL(TPtrC8& aDes) \
		{ return static_cast< _class *>(Meta::SMetaDataECom::LoadL(aDes)); } \
	static inline TUid GetUid(void) { return TUid::Uid(_uid) ; } \
public: \
	DATA_VTABLE

namespace NetMessages
{

/** An class recognised by CWorkerThread. When a client of the root server wants to send a message to a module
via RRootServ::SendMessage method it serialises an instance of CNetMessage based class and passes it as
a aData parameter
@see RRootServ::SendMessage
@released since 9.1
*/
class CMessage : public Meta::SMetaDataECom
	{
public:
	TYPEID_TABLE
	};

} // namespace NetMessages
#endif	// NETMESSAGES_H

