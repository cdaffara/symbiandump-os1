// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef API_EXT_MSG_H
#define API_EXT_MSG_H

#include <elements/sd_std.h>
#include <comms-infras/metavirtctor.h>
#include <comms-infras/netmessages.h>

/**
 @publishedPartner
 @released
*/
typedef Den::TSubSessionUniqueId TSubSessionUniqueId;


#define TRIVIAL_ACCESSOR(className, memberType, memberNameRoot) \
	memberType className::memberNameRoot() const { return i##memberNameRoot; } \
	void className::Set##memberNameRoot(memberType a##memberNameRoot) { i##memberNameRoot = a##memberNameRoot; }


namespace NetInterfaces
	{
	class TInterfaceControl;
	}
namespace Elements
	{
	class RResponseMsg;
   }


/**
	Comms API extension request message.

	@publishedPartner
	@released
*/
class TCommsApiExtReqMsg : public Meta::SMetaDataNetCtor
	{
public:
	IMPORT_C virtual void DispatchL(NetInterfaces::TInterfaceControl& aIntfCtl, const TSubSessionUniqueId& aSubSessionUniqueId, Elements::RResponseMsg& aResponseMsg);
	};


/**
Comms API extension response message.

@publishedPartner
@released
*/
class CCommsApiExtRespMsg : public NetMessages::CMessage
	{
public:
	/** Creates a new API extension message using ECOM.

    @param aTypeId Id of the class (Composed of Uid of the implementation and an integer sub-type).
    @return a generic pointer to a specific message if successful, otherwise leaves with system error code.
    */
	IMPORT_C static CCommsApiExtRespMsg* NewL(const Meta::STypeId& aTypeId);

	/** Creates a new API extension message using ECOM.

    @param aMsgBuffer a buffer containing the serialised form of the message object.
    @return a generic pointer to a specific event if successful, otherwise leaves with system error code.
    */
	IMPORT_C static CCommsApiExtRespMsg* NewL(const TDesC8& aMsgBuffer);
	};


/**
Comms API extension response message.

@internalComponent

Kept for backward compatibility.
*/
#define CCommsApiExtensionMsg CCommsApiExtRespMsg

#endif
// API_EXT_MSG_H

