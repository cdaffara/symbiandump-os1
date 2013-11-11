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
 @released
*/

#define SYMBIAN_NETWORKING_UPS

#include "ss_api_ext.h"

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_roles.h>
#include <ss_glob.h>
#include <es_panic.h>
#include <ss_std.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_internal.h>
#include <comms-infras/ss_nodemessages_legacy.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocks_p_xt, "ESockSSocks_p_xt");
#endif

using namespace Elements;
using namespace ESock;
using namespace Messages;

//
// CExtItfMsgPluginInfo
//

EXPORT_C CExtItfMsgPluginInfo* CExtItfMsgPluginInfo::NewInstanceL(const STypeId& aTypeId)
	{
	TUid destroyUid;
	CExtItfMsgPluginInfo* obj = reinterpret_cast<CExtItfMsgPluginInfo*>(REComSession::CreateImplementationL(aTypeId.iUid, destroyUid, (TAny*)aTypeId.iType));
	obj->iDestroyUid = destroyUid;
	return obj;
	}

EXPORT_C CExtItfMsgPluginInfo::~CExtItfMsgPluginInfo()
	{
	__ASSERT_DEBUG(iDestroyUid.iUid!=0, User::Panic(KSpecAssert_ESockSSocks_p_xt, 1));
	REComSession::DestroyedImplementation(iDestroyUid);
	}

//
// CCommsApiExtResponder
//

EXPORT_C CCommsApiExtResponder::CCommsApiExtResponder()
	{
	}

EXPORT_C CCommsApiExtResponder::~CCommsApiExtResponder()
	{
	}

EXPORT_C void CCommsApiExtResponder::Complete(CCommsApiExtResponder*& aThis, TInt aError)
	{
	if (aThis==NULL)
		{
		return;
		}

	aThis->DoComplete(aError);
	delete aThis;
	aThis = NULL;
	}

//
// CCommsApiExtIpcResponder
//

EXPORT_C void CCommsApiExtIpcResponder::DoComplete(TInt aError)
	{
	iResponseMsg.Complete(aError);
	}

EXPORT_C CCommsApiExtIpcResponder* CCommsApiExtIpcResponder::NewL(const RMessage2& aMessage)
	{
	return new (ELeave) CCommsApiExtIpcResponder(aMessage);
	}

EXPORT_C CCommsApiExtIpcResponder* CCommsApiExtIpcResponder::NewL(Elements::RResponseMsg& aResponseMsg)
	{
	return new (ELeave) CCommsApiExtIpcResponder(aResponseMsg);
	}

//
// RLegacyRepsonseMsg
//
EXPORT_C RLegacyResponseMsg::RLegacyResponseMsg()
	: iNodeAddr(Messages::TNodeCtxId::NullId())
	{}

EXPORT_C RLegacyResponseMsg::RLegacyResponseMsg(MeshMachine::TNodeContextBase& aContext)
	: Elements::RResponseMsg(), iNodeAddr(aContext.ActivityId(), aContext.NodeId())
	{}

EXPORT_C RLegacyResponseMsg::RLegacyResponseMsg(MeshMachine::TNodeContextBase& aContext, const RMessage2& aMessage, TInt aInterfaceId, TInt aRequestMsgParam, TInt aResponseMsgParam)
	: Elements::RResponseMsg(aMessage, aInterfaceId, aRequestMsgParam, aResponseMsgParam), iNodeAddr(aContext.ActivityId(), aContext.NodeId())
	{}
	
EXPORT_C RLegacyResponseMsg::RLegacyResponseMsg(MeshMachine::TNodeContextBase& aContext, const RMessage2& aMessage, const Den::TApiExtIdentification& aInterfaceId, TInt aRequestMsgParam, TInt aResponseMsgParam)
	: Elements::RResponseMsg(aMessage, aInterfaceId, aRequestMsgParam, aResponseMsgParam), iNodeAddr(aContext.ActivityId(), aContext.NodeId())
	{}
	
EXPORT_C void RLegacyResponseMsg::Complete(TInt aError) const
	{
	TCFLegacyMessage::TLegacyRMessage2Processed msg(TLegacyRMessage2Response(TLegacyRMessage2Response::ENormal, aError));
    RNodeInterface::OpenPostMessageClose(iNodeAddr, iNodeAddr, msg);
	iNodeAddr.SetNull();
	}

EXPORT_C void RLegacyResponseMsg::Panic(const TDesC& aCategory, TInt aReason) const
	{
	TCFLegacyMessage::TLegacyRMessage2Processed msg(TLegacyRMessage2Response(TLegacyRMessage2Response::EPanic, aReason, aCategory));
    RNodeInterface::OpenPostMessageClose(iNodeAddr, iNodeAddr, msg);
	iNodeAddr.SetNull();
	}

EXPORT_C TBool RLegacyResponseMsg::IsNull() const
	{
	return iNodeAddr.IsNull();
	}

//
// CCommsApiExtLegacyIpcResponder
//
EXPORT_C void CCommsApiExtLegacyIpcResponder::DoComplete(TInt aError)
	{
	iResponseMsg.Complete(aError);
	}

EXPORT_C CCommsApiExtLegacyIpcResponder* CCommsApiExtLegacyIpcResponder::NewL(MeshMachine::TNodeContextBase& aContext, const RMessage2& aMessage)
	{
	return new (ELeave) CCommsApiExtLegacyIpcResponder(aContext, aMessage);
	}

EXPORT_C CCommsApiExtLegacyIpcResponder* CCommsApiExtLegacyIpcResponder::NewL(RLegacyResponseMsg& aResponseMsg)
	{
	return new (ELeave) CCommsApiExtLegacyIpcResponder(aResponseMsg);
	}

// CCommsApiExtIpcOpenResponder

EXPORT_C CCommsApiExtIpcOpenResponder* CCommsApiExtIpcOpenResponder::NewL(TSupportedCommsApiExt aInterfaceId, TSubSessionUniqueId aClientId, const RMessage2& aMessage)
	{
	return new (ELeave) CCommsApiExtIpcOpenResponder(aInterfaceId, aClientId, aMessage);
	}

EXPORT_C void CCommsApiExtIpcOpenResponder::DoComplete(TInt aError)
	{
	if (aError!=KErrNone)
		{
		SockManGlobals::Get()->iCommsApiExtRegister.DeRegisterInterface(iInterfaceId, iClientId);
		}
	CCommsApiExtIpcResponder::DoComplete(aError);
	}


//
// AIPCClientInfo
//

/**
   @param aSubSessionUniqueId The id of the subsession to query
   @param aUidType The uid type
   @return KErrNone on success and KErrCorrupt when the current aSubSessionUniqueId
   could not be mapped to an existing client.
*/
EXPORT_C TInt AIPCClientInfo::ClientUidType(TSubSessionUniqueId aSubSessionUniqueId, TUidType& aUidType) const
	{
	TProcessId processId;
	TThreadId threadId;
	TInt err = GetSubsessionOwnerInfo(aSubSessionUniqueId, processId, aUidType, threadId);
	return err;
	}

/**
   Retrieves the secure id of the owner of the subsession
   @param aSubSessionUniqueId The id of the subsession to query
   @param aUid The secure id of the owner of the subsession
   @return KErrNone if the call is successful, otherwise one of the system-wide error codes.
*/
EXPORT_C TInt AIPCClientInfo::ClientSid(TSubSessionUniqueId aSubSessionUniqueId, TSecureId& aSid) const
	{
	RProcess process;
	TInt res = OpenClientProcess(aSubSessionUniqueId, process);
	if (res == KErrNone)
		{
        aSid = process.SecureId();
        process.Close();
		}

    return res;
	}

/**
   Opens the RProcess of the owning client process of the subsession. The caller is responsible
   for calling the RProcess.Close() method.
   @param aSubSessionUniqueId The subsession id of to obtain the client process information from
   @param aProcess Upon returning from a successful call aProcess is an opened RProcess.
   @return KErrNone if the call is successful, otherwise one of the system-wide error codes.
*/
EXPORT_C TInt AIPCClientInfo::OpenClientProcess(TSubSessionUniqueId aSubSessionUniqueId, RProcess& aProcess) const
    {
    TProcessId processId;
	TUidType uidType;
	TThreadId threadId;

	TInt err = GetSubsessionOwnerInfo(aSubSessionUniqueId, processId, uidType, threadId);
	if (err == KErrNone)
	    {
        err = aProcess.Open(processId);
	    }

    return err;
    }

/**
   Retrieves information about the owner of the subsession
   @param aSubSessionUniqueId The subsession id of to obtain the client process information from
   @return KErrNone if the call is successful, otherwise one of the system-wide error codes.
   @note The thread id. returned is that which opened the RSocketServ, not the RSocket.
*/
TInt AIPCClientInfo::GetSubsessionOwnerInfo(TSubSessionUniqueId aSubSessionUniqueId, TProcessId& aProcessId, TUidType& aUidType, TThreadId& aThreadId) const
    {
	CWorkerThread* worker = SockManGlobals::Get()->SelfWorker();
	__ASSERT_DEBUG(worker, Panic(ENonESockWorkerThread));
	
	CPlayer* player = worker->Player();
	__ASSERT_DEBUG(player, Panic(ENonESockWorkerThread));
	
	CSockSubSession* ss = player->SubSession(aSubSessionUniqueId);
	
	if (ss == NULL)
		{
		return KErrCorrupt;
		}

	ss->GetOwnerInfo(aProcessId, aUidType, aThreadId);
	return KErrNone;
   }

EXPORT_C TInt AIPCClientPlatsecInfo::SecureId(TSubSessionUniqueId aSubSessionUniqueId, TSecureId& aResult) const
    {
	return ClientSid(aSubSessionUniqueId, aResult);
    }


EXPORT_C TInt AIPCClientPlatsecInfo::VendorId(TSubSessionUniqueId aSubSessionUniqueId, TVendorId& aResult) const
    {
	RProcess process;
	TInt res = OpenClientProcess(aSubSessionUniqueId, process);
	if (res == KErrNone)
		{
        aResult = process.VendorId();
        process.Close();
		}

    return res;
    }


EXPORT_C TBool AIPCClientPlatsecInfo::HasCapability(TSubSessionUniqueId aSubSessionUniqueId, const TCapability aCapability) const
   {
   RProcess process;
   TInt res = OpenClientProcess(aSubSessionUniqueId, process);
   if (res != KErrNone) { return EFalse; }

   TBool platsecResult;
   platsecResult = process.HasCapability(aCapability);
   process.Close();

   return platsecResult;
   }


EXPORT_C TInt AIPCClientPlatsecInfo::CheckPolicy(TSubSessionUniqueId aSubSessionUniqueId, const TSecurityPolicy& aPolicy) const
    {
	RProcess process;
	TInt res = OpenClientProcess(aSubSessionUniqueId, process);
	if (res != KErrNone)
        {
        return res;
        }

    TBool platsecResult;
    platsecResult = aPolicy.CheckPolicy(process);
    process.Close();
    if (!platsecResult)
        {
        return KErrPermissionDenied;
        }
    return KErrNone;
    }

#ifdef SYMBIAN_NETWORKING_UPS
EXPORT_C TInt AIPCClientPlatsecInfo::GetProcessAndThreadId(TSubSessionUniqueId aSubSessionUniqueId, TProcessId& aProcessId, TThreadId& aThreadId) const
    {
    TUidType   uidType;
    
	TInt rc = GetSubsessionOwnerInfo(aSubSessionUniqueId, aProcessId, uidType, aThreadId);
	return rc;
    }
#endif //SYMBIAN_NETWORKING_UPS

/**

*/
EXPORT_C AExtensionInterfaceThickBase::AExtensionInterfaceThickBase(const Meta::STypeId& aMsgImplTid) 
:	iMsgImplTid(aMsgImplTid)
	{
	}

/**

*/
EXPORT_C AExtensionInterfaceThickBase::~AExtensionInterfaceThickBase()
	{
	}

/**
	Null default implementation
*/
EXPORT_C void AExtensionInterfaceThickBase::OpenExtensionInterface(TSubSessionUniqueId /* aClientId */, const TRuntimeCtxId& /*aControlClient*/, CCommsApiExtResponder* aResponder)
	{
	CCommsApiExtResponder::Complete(aResponder,KErrNone);
	}

/**
	Null default implementation
*/
EXPORT_C void AExtensionInterfaceThickBase::CancelExtensionInterface(TSubSessionUniqueId /* aClientId */)
	{
	}

/**
	Null default implementation
*/
EXPORT_C void AExtensionInterfaceThickBase::CloseExtensionInterface(TSubSessionUniqueId /* aClientId */)
	{
	}

/*
EXPORT_C void AExtensionInterfaceThickBase::RegisterMessagesL()
	{
	__ASSERT_DEBUG(iMsgPluginInfo==NULL, User::Panic(KSpecAssert_ESockSSocks_p_xt, 2)); //Well, we should not be opening the same interface 
		{
		iMsgPluginInfo = static_cast<MExtItfMsgPluginInfo*>(REComSession::CreateImplementationL(iMsgImplUid, iDtorIdKey));
		}
	const TImplementationProxy* implProxyTable = iMsgPluginInfo->ImplementationProxy();
	CWorkerThread* worker = SockManGlobals::Get()->SelfWorker();
	worker->RegisterInterfaceL(iMsgImplUid, iMsgPluginInfo->ImplementationProxyTableSize(), implProxyTable);
	}
	
EXPORT_C void AExtensionInterfaceThickBase::DeregisterMessages()
	{
	CWorkerThread* worker = SockManGlobals::Get()->SelfWorker();
	worker->DeregisterInterface(iMsgImplUid);
	REComSession::DestroyedImplementation(iDtorIdKey);
	REComSession::FinalClose();
	}
*/

