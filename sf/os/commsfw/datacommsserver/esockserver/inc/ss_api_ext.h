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

#ifndef SS_API_EXT_H
#define SS_API_EXT_H

#define SYMBIAN_NETWORKING_UPS

#include <es_sock.h>
#include <comms-infras/api_ext_msg.h>
#include <comms-infras/api_ext_list.h>
#include <elements/interfacetable.h>
#include <elements/responsemsg.h>
#include <ecom/ecom.h>

/**
   Forward declaration to avoid including mm_context.h
*/
namespace MeshMachine
{
    class TNodeContextBase;
}

namespace ESock
{

/**
	@internalTechnology
*/
class CExtItfMsgPluginInfo : public CBase
	{
public:
	IMPORT_C static CExtItfMsgPluginInfo* NewInstanceL(const STypeId& aTypeId);
	IMPORT_C ~CExtItfMsgPluginInfo();

	virtual const TImplementationProxy* ImplementationProxy() const = 0;
	virtual TInt ImplementationProxyTableSize() const = 0;

private:
	TUid iDestroyUid;
	};

/**
	@internalTechnology
*/
class CCommsApiExtResponder : public CBase
	{
public:
	IMPORT_C static void Complete(CCommsApiExtResponder*& aThis, TInt aError);

protected:
	IMPORT_C explicit CCommsApiExtResponder();
	IMPORT_C virtual ~CCommsApiExtResponder();

private:
	virtual void DoComplete(TInt aError) = 0;
	};

/**
	@internalTechnology
*/
class CCommsApiExtIpcResponder : public CCommsApiExtResponder 
	{
public:
	IMPORT_C static CCommsApiExtIpcResponder* NewL(const RMessage2& aMessage);
	IMPORT_C static CCommsApiExtIpcResponder* NewL(Elements::RResponseMsg& aResponseMsg);

protected:
	explicit CCommsApiExtIpcResponder(const RMessage2& aMessage)
	:	iResponseMsg(aMessage, aMessage.Int0(), 1, 2)
		{
		}
	explicit CCommsApiExtIpcResponder(Elements::RResponseMsg& aResponseMsg)
	:	iResponseMsg(aResponseMsg)
		{
		}

protected:
	IMPORT_C virtual void DoComplete(TInt aError);

protected:	
	Elements::RResponseMsg iResponseMsg;
	};

/**
   Specialised responder for Legacy messages. Wraps all RMessage2 functionallity,
   so that it cannot be completed without us knowing about it.

   @publishedPartner
   @released since tb92
*/
class RLegacyResponseMsg : public Elements::RResponseMsg
	{
public:
	IMPORT_C RLegacyResponseMsg();
	IMPORT_C explicit RLegacyResponseMsg(MeshMachine::TNodeContextBase& aContext);
	IMPORT_C explicit RLegacyResponseMsg(MeshMachine::TNodeContextBase& aContext, const RMessage2& aMessage, TInt aInterfaceId, TInt aRequestMsgParam = -1, TInt aResponseMsgParam = -1);
	IMPORT_C explicit RLegacyResponseMsg(MeshMachine::TNodeContextBase& aContext, const RMessage2& aMessage, const Den::TApiExtIdentification& aInterfaceId, TInt aRequestMsgParam = -1, TInt aResponseMsgParam = -1);

	IMPORT_C void Complete(TInt aError) const;
	IMPORT_C void Panic(const TDesC& aCategory, TInt aReason) const;

	IMPORT_C TBool IsNull() const;

private:
	mutable Messages::TNodeCtxId iNodeAddr;
	};

/**
	@internalTechnology
*/
class CCommsApiExtLegacyIpcResponder : public CCommsApiExtResponder 
	{
public:
	IMPORT_C static CCommsApiExtLegacyIpcResponder* NewL(MeshMachine::TNodeContextBase& aContext, const RMessage2& aMessage);
	IMPORT_C static CCommsApiExtLegacyIpcResponder* NewL(RLegacyResponseMsg& aResponseMsg);

protected:
	explicit CCommsApiExtLegacyIpcResponder(MeshMachine::TNodeContextBase& aContext, const RMessage2& aMessage)
		:	iResponseMsg(aContext, aMessage, aMessage.Int0(), 1, 2)
		{
		}
	explicit CCommsApiExtLegacyIpcResponder(RLegacyResponseMsg& aResponseMsg)
	:	iResponseMsg(aResponseMsg)
		{
		}

protected:
	IMPORT_C virtual void DoComplete(TInt aError);

protected:	
	RLegacyResponseMsg iResponseMsg;
	};


/**
	@internalTechnology
*/
class CCommsApiExtIpcOpenResponder : public CCommsApiExtIpcResponder 
	{
public:
	IMPORT_C static CCommsApiExtIpcOpenResponder* NewL(TSupportedCommsApiExt aInterfaceId, TSubSessionUniqueId aClientId, const RMessage2& aMessage);

protected:
	explicit CCommsApiExtIpcOpenResponder(TSupportedCommsApiExt aInterfaceId, TSubSessionUniqueId aClientId, const RMessage2& aMessage)
	:	CCommsApiExtIpcResponder(aMessage),
		iInterfaceId(aInterfaceId),
		iClientId(aClientId)
		{
		}

protected:
	IMPORT_C virtual void DoComplete(TInt aError);

protected:
	const TSupportedCommsApiExt iInterfaceId;
	const TSubSessionUniqueId iClientId;
	};


/**
	@internalTechnology
*/
class MExtensionInterfaceBase
	{
public:
	virtual void OpenExtensionInterface(TSubSessionUniqueId aClientId, const Messages::TRuntimeCtxId& aControlClient, CCommsApiExtResponder* aResponder) = 0;
	virtual void CloseExtensionInterface(TSubSessionUniqueId aClientId) = 0;
	virtual void CancelExtensionInterface(TSubSessionUniqueId aClientId) = 0;
	};

class AExtensionInterfaceThickBase : public MExtensionInterfaceBase
	{
public:
	// From MExtensionInterfaceBase
	IMPORT_C virtual void OpenExtensionInterface(TSubSessionUniqueId aClientId, const Messages::TRuntimeCtxId& aControlClient, CCommsApiExtResponder* aResponder);
	IMPORT_C virtual void CloseExtensionInterface(TSubSessionUniqueId aClientId);
	IMPORT_C virtual void CancelExtensionInterface(TSubSessionUniqueId aClientId);

	const Meta::STypeId& MsgImplTid() const
		{
		return iMsgImplTid;
		}

protected:
	IMPORT_C AExtensionInterfaceThickBase(const Meta::STypeId& aMsgImplTid);
	IMPORT_C virtual ~AExtensionInterfaceThickBase();

private:
	const Meta::STypeId iMsgImplTid;
	};


/**
	@internalTechnology
	
	Minimum functionality for an extension interface.  OpenL() 
	and Close() provided as a means of resource reservation and
	release.
*/
template <TInt32 MSG_IMPL_UID, TInt32 MSG_IMPL_TYPEID = 0>
class AExtensionInterfaceBase : public AExtensionInterfaceThickBase
	{
protected:
	AExtensionInterfaceBase()
		: AExtensionInterfaceThickBase(Meta::STypeId::CreateSTypeId(MSG_IMPL_UID,MSG_IMPL_TYPEID))
		{
		}
	//@todo - should AExtensionInterfaceThickBase have a gratuitous pure virtual to try to force use of this template? seems lame...
	};

/**
   @internalTechnology
*/
class AIPCClientInfo
	{
public:
	IMPORT_C TInt ClientUidType(TSubSessionUniqueId aSubSessionUniqueId, TUidType& aUidType) const;
	IMPORT_C TInt ClientSid(TSubSessionUniqueId aSubSessionUniqueId, TSecureId& aSid) const;
	
protected:
	IMPORT_C TInt OpenClientProcess(TSubSessionUniqueId aSubSessionUniqueId, RProcess& aProcess) const;
#ifndef SYMBIAN_NETWORKING_UPS
private:
#endif //SYMBIAN_NETWORKING_UPS
	TInt GetSubsessionOwnerInfo(TSubSessionUniqueId aSubSessionUniqueId, TProcessId& aProcessId, TUidType& aUidType, TThreadId& aThreadId) const;	
	};

/**
   @internalTechnology
*/
class AIPCClientPlatsecInfo : public AIPCClientInfo
    {
public:
    IMPORT_C TInt SecureId(TSubSessionUniqueId aSubSessionUniqueId, TSecureId& aResult) const;
    IMPORT_C TInt VendorId(TSubSessionUniqueId aSubSessionUniqueId, TVendorId& aResult) const;
    IMPORT_C TBool HasCapability(TSubSessionUniqueId aSubSessionUniqueId, const TCapability aCapability) const;
    IMPORT_C TInt  CheckPolicy(TSubSessionUniqueId aSubSessionUniqueId, const TSecurityPolicy& aPolicy) const;
#ifdef SYMBIAN_NETWORKING_UPS
    IMPORT_C TInt  GetProcessAndThreadId(TSubSessionUniqueId aSubSessionUniqueId, TProcessId& aProcessId, TThreadId& aThreadId) const;
#endif //SYMBIAN_NETWORKING_UPS
    };

} //namespace ESock

#endif // SS_API_EXT_H

