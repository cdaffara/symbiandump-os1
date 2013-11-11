// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(SS_APIEXT_MESSAGES_H_INCLUDED)
#define SS_APIEXT_MESSAGES_H_INCLUDED

#include <e32def.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_dispatchers.h>
#include <comms-infras/api_ext_list.h>
#include <comms-infras/api_ext_msg.h>
#include <elements/responsemsg.h>

namespace ESock
{

const TInt KExtItfMsgImplementationUid = 0x102822F7;

enum EExtItfMessageId
	{
	EApiExtMsgDispatcher = 0,
	EOpenExtensionInterface,
	ECloseExtensionInterface,
	ECancelAndCloseAllClientExtItf
	};

/**
	@internalTechnology

	Utility class to register/deregister API extension interface messages
*/
struct TExtItfMsgTables
	{
	static void RegisterL();
	static void DeRegister();
	};

/**
	@internalTechnology

	A self-forwarding message that walks the chain of service providers looking
	for an implementor of the specified interface id.
*/
class TOpenExtensionInterface : public TCFSigRMessage2Ext
    {
public:
	explicit TOpenExtensionInterface() {}
	explicit TOpenExtensionInterface(TUint aClientId, TSupportedCommsApiExt aInterfaceId, const RMessage2& aMessage)
	:	TCFSigRMessage2Ext(aMessage),
		iClientId(aClientId),
		iInterfaceId(aInterfaceId)
		{
		}

	DECLARE_MVIP_CTR(TOpenExtensionInterface)
	DATA_VTABLE

protected:
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);

public:
	TUint iClientId;
    TSupportedCommsApiExt iInterfaceId;
    };

/**
   @internalTechnology

	A self-forwarding message that walks the chain of service providers looking
	for an implementor of the specified interface id.
*/
class TCloseExtensionInterface : public TCFSigRMessage2Ext
    {
public:
	explicit TCloseExtensionInterface() {}
	explicit TCloseExtensionInterface(TUint aClientId, TSupportedCommsApiExt aInterfaceId, const RMessage2& aMessage)
	:	TCFSigRMessage2Ext(aMessage),
		iClientId(aClientId),
		iInterfaceId(aInterfaceId)
        {
        }

	DECLARE_MVIP_CTR(TCloseExtensionInterface)
	DATA_VTABLE

protected:
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);

public:
	TUint iClientId;
    TSupportedCommsApiExt iInterfaceId;
    };

/**
	TCancelClientExtItfRequests forwards itself down the chain of service providers causing
	all requests for the specified client to be cancelled.

	@internalTechnology
*/

//MZ: This current hard-coded routing (and closing) behaviour is not ideal.

//We should migrate ESockSvr to the solution in Den,
//where each session & subsession has its own API ext register
//and closes all opened extension when going out of scope.
class TCancelAndCloseAllClientExtItf : public Messages::TSignalBase, public ESock::TCFClientItfExt<ESock::TCFClientType::ECtrl>
	{
public:
	explicit TCancelAndCloseAllClientExtItf() {}
    explicit TCancelAndCloseAllClientExtItf(TUint aClientId)
	:	iClientId(aClientId)
		{
		}

	DECLARE_MVIP_CTR(TCancelAndCloseAllClientExtItf)
	DATA_VTABLE

protected:
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);

private:
	void CancelCloseAllAndForwardMsgL(const Messages::TRuntimeCtxId& aSender, Messages::TSignalBase& aMessage, Messages::ANode& aNode);

private:
	TUint iClientId;
	};


/**
	TApiExtMsgDispatcher is a self-forwarding dispatcher of TCommsApiExtReqMsg messages.

	@internalTechnology
*/
class TApiExtMsgDispatcher : public Messages::TSignalBase, public ESock::TCFClientItfExt<ESock::TCFClientType::ECtrl>
    {
public:
	explicit TApiExtMsgDispatcher() { }
	explicit TApiExtMsgDispatcher(TUint aClientId, const Elements::RResponseMsg& aResponseMsg)
    :	iClientId(aClientId), iResponseMsg(aResponseMsg)
		{
		}

	DECLARE_MVIP_CTR(TApiExtMsgDispatcher)
	DATA_VTABLE

protected:
	virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	virtual void Error(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient, TInt aError);

private:
	TUint iClientId;
	Elements::RResponseMsg iResponseMsg;
	};


/**
@internalTechnology
*/
class TCommsClientSideApiExt
	{
public:
	static TBool IsSupported(TSupportedCommsApiExt aApiExtensionId);
	};

} // namespace ESock


#endif // SS_APIEXT_MESSAGES_H_INCLUDED

