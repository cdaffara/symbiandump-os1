// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_DEN_APIEXTENSIONROUTING_H
#define SYMBIAN_DEN_APIEXTENSIONROUTING_H

#include <ecom/ecom.h>
#include <elements/nm_signals.h>
#include <elements/vc.h>

namespace Den
{
class CWorkerSession;
class CWorkerSubSession;
class AIpcExtensionInterfaceBase;
class TApiExtIdentification;

//Base routing message
class TIpcExtensionRoutingMessageBase : public Messages::TSignalBase
/**
@internalTechnology
*/
    {
public:
	explicit TIpcExtensionRoutingMessageBase() {}
	IMPORT_C void RegisterMessagesAtDestinationL(AIpcExtensionInterfaceBase& aInterface);
	IMPORT_C void DeregisterMessagesAtDestination(AIpcExtensionInterfaceBase& aInterface);
	EXPORT_TYPEID_TABLE
    };

//Base routing message
class TIpcExtensionRoutingMessageFromSession : public TIpcExtensionRoutingMessageBase
/**
@internalTechnology
*/
    {
public:
	explicit TIpcExtensionRoutingMessageFromSession() {}
	//[MZ]: talk to Denis May about the possibility to pass the aFunction
	//and aInterfaceId as they would be in the
	virtual void KickOffL(const RSafeMessage& aMessage, CWorkerSession& aSession, const TApiExtIdentification& aIdentification) = 0;
	EXPORT_TYPEID_TABLE
    };

//Base routing message
class TIpcExtensionRoutingMessageFromSubSession : public TIpcExtensionRoutingMessageBase
/**
@internalTechnology
*/
    {
public:
	//Under this UID the host registers all routing messages that
	//kickoff from the subsession object.
	static const TUint32 KInPlaceConstructorTableUid = 0x12349999; //TODO: allocate an UID

public:
	explicit TIpcExtensionRoutingMessageFromSubSession() {}
	virtual void KickOffL(const RSafeMessage& aMessage, CWorkerSubSession& aSubSession, const TApiExtIdentification& aIdentification) = 0;
	EXPORT_TYPEID_TABLE
    };

} //namespace Den

#endif //SYMBIAN_DEN_APIEXTENSIONROUTING_H

