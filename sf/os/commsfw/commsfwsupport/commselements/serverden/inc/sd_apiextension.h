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

#ifndef SYMBIAN_DEN_APIEXTENSION_H
#define SYMBIAN_DEN_APIEXTENSION_H

#include <ecom/ecom.h>
#include <elements/interfacetable.h>
#include <elements/responsemsg.h>
#include <elements/sd_std.h>
#include <elements/sd_apiextensionclient.h>

namespace Den
{

/**
	@internalTechnology
*/
class CExtItfMsgPluginInfo : public CBase
	{
public:
	IMPORT_C static CExtItfMsgPluginInfo* NewInstanceL(const Meta::STypeId& aTypeId);
	IMPORT_C ~CExtItfMsgPluginInfo();

	virtual const TImplementationProxy* ImplementationProxy() const = 0;
	virtual TInt ImplementationProxyTableSize() const = 0;

private:
	TUid iDestroyUid;
	};


/**
	@internalTechnology
*/
class MIpcExtensionInterface
	{
public:
	virtual TInt Open(TUint aClientId) = 0;
	virtual void Close(TUint aClientId) = 0;
	virtual Meta::STypeId MsgImplTid() const = 0;
	};

/**
	@internalTechnology
*/
class AIpcExtensionInterfaceBase : public MIpcExtensionInterface
	{
	friend class TIpcExtensionRoutingMessageBase;

protected:
	IMPORT_C AIpcExtensionInterfaceBase(); //sets iMsgPluginInfo to NULL
	virtual ~AIpcExtensionInterfaceBase()
		{
		}

private:
	//Only used internally by TIpcExtensionRoutingMessageBase - don't export!
	void RegisterMessagesL();
	void DeregisterMessages();

private:
	CExtItfMsgPluginInfo* iMsgPluginInfo;
	};

/**
	@internalTechnology
*/
template <TInt32 MSG_IMPL_UID, TInt32 MSG_IMPL_TYPEID = 0>
class AIpcExtensionInterface : public AIpcExtensionInterfaceBase
	{
public:
	enum {EUid = MSG_IMPL_UID};
	enum {ETypeId = MSG_IMPL_TYPEID};

protected:
	inline Meta::STypeId MsgImplTid() const
		{
		return Meta::STypeId::CreateSTypeId(MSG_IMPL_UID, MSG_IMPL_TYPEID);
		}
	};

} //namespace Den

#endif //SYMBIAN_DEN_APIEXTENSION_H

