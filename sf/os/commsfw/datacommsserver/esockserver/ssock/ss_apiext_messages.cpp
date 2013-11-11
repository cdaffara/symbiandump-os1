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

#include "ss_apiext_messages.h"
#include <comms-infras/ss_api_ext.h>
#include <comms-infras/ss_thread.h>
#include <ss_glob.h>
#include <comms-infras/es_api_ext.h>

using namespace ESock;
using namespace Elements;
using namespace Messages;
using namespace MeshMachine;

// ----- TOpenExtensionInterface -----

void TOpenExtensionInterface::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<TNodeId>(aRecipient);
	TAny* itf = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), iInterfaceId);

	if(itf)
		{
		AExtensionInterfaceThickBase& baseItf = *static_cast<AExtensionInterfaceThickBase*>(itf);

		//This fn will not allow an interface to be reqistered twice
		SockManGlobals::Get()->iCommsApiExtRegister.RegisterInterfaceL(iInterfaceId, baseItf.MsgImplTid(), iClientId);

		//When completed with an error code (!=KErrNone), this responder will deregister interface for this client
		baseItf.OpenExtensionInterface(iClientId, nodeId, CCommsApiExtIpcOpenResponder::NewL(iInterfaceId, iClientId, iMessage));
		// coverity [memory_leak] - the ownership of CCommsApiExtIpcOpenResponder::NewL(iInterfaceId, iClientId, iMessage) 
		// is taken by the function, OpenExtensionInterface. No need to setup cleanup stack for memory leak
		}
	}

void TOpenExtensionInterface::Error(const TRuntimeCtxId& /*aSender*/, const TRuntimeCtxId& /*aRecipient*/, TInt aError)
	{
   	if (aError==KErrNotFound)
   		{
   		//This IPC call has not been served.
   		aError=KErrNotSupported;
   		}

	iMessage.Complete(aError);
	}

DEFINE_MVIP_CTR(TOpenExtensionInterface)

START_ATTRIBUTE_TABLE(TOpenExtensionInterface, KExtItfMsgImplementationUid, EOpenExtensionInterface)
	REGISTER_ATTRIBUTE(TOpenExtensionInterface, iClientId, TMeta<TUint>)
	REGISTER_ATTRIBUTE(TOpenExtensionInterface, iInterfaceId, TMeta<TSupportedCommsApiExt>)
END_ATTRIBUTE_TABLE_BASE(TCFSigRMessage2Ext, ESignatureRMessage2)

// ----- TCloseExtensionInterface -----

void TCloseExtensionInterface::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<TNodeId>(aRecipient);
	TAny* itf = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), iInterfaceId);

	if(itf)
		{
		AExtensionInterfaceThickBase& baseItf = *static_cast<AExtensionInterfaceThickBase*>(itf);
		baseItf.CloseExtensionInterface(iClientId);
		SockManGlobals::Get()->iCommsApiExtRegister.DeRegisterInterface(iInterfaceId, iClientId);
		iMessage.Complete(KErrNone);
		}
	}

void TCloseExtensionInterface::Error(const TRuntimeCtxId& /*aSender*/, const TRuntimeCtxId& /*aRecipient*/, TInt /*aError*/)
	{
	// Anything to do?
	}

DEFINE_MVIP_CTR(TCloseExtensionInterface)

START_ATTRIBUTE_TABLE(TCloseExtensionInterface, KExtItfMsgImplementationUid, ECloseExtensionInterface)
	REGISTER_ATTRIBUTE(TCloseExtensionInterface, iClientId, TMeta<TUint>)
	REGISTER_ATTRIBUTE(TCloseExtensionInterface, iInterfaceId, TMeta<TSupportedCommsApiExt>)
END_ATTRIBUTE_TABLE_BASE(TCFSigRMessage2Ext, ESignatureRMessage2)

// ----- TCancelAndCloseAllClientExtItf -----

void TCancelAndCloseAllClientExtItf::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
	{
	CancelCloseAllAndForwardMsgL(aSender, *this, address_cast<TNodeId>(aRecipient).Node());
	}

void TCancelAndCloseAllClientExtItf::Error(const TRuntimeCtxId& /*aSender*/, const TRuntimeCtxId& /*aRecipient*/, TInt /*aError*/)
   	{
   	// Anything to do?
   	}

TBool ESock::TCommsClientSideApiExt::IsSupported(TSupportedCommsApiExt aApiExtensionId)
	{
	const TInt KEndOfUidListSentinelUid = 0x102864E6;

	const TSupportedCommsApiExt iList[] = 
		{
		EMobilityApiExt,
		ELegacySubConnectionActiveApiExt,
		EDataMonitoringApiExt,
		ELegacyEnumerateSubConnectionsApiExt,
		// End of list
		(TSupportedCommsApiExt)KEndOfUidListSentinelUid
		};

	for(TInt index = 0; iList[index] != KEndOfUidListSentinelUid; index++)
		{
		if(iList[index] == aApiExtensionId)
			{
			return ETrue;
			}
		}
	
	return EFalse;
	}

//MZ: This current hard-coded routing (and closing) behaviour is not ideal.
//We should migrate ESockSvr to the solution in Den,
//where each session & subsession has its own API ext register
//and closes all opened extension when going out of scope.
void TCancelAndCloseAllClientExtItf::CancelCloseAllAndForwardMsgL(const TRuntimeCtxId& aSender, TSignalBase& aMessage, Messages::ANode& aNode)
	{
	NetInterfaces::TInterfaceControl* ic = aNode.FetchNodeInterfaceControlL(AMMNodeBase::KInterfaceId);
	NetInterfaces::TInterfaceIter itfIter(*ic);

	TCommsApiExtRegister& itfRegister = SockManGlobals::Get()->iCommsApiExtRegister;
	TAny* itf = itfIter[0];

	while(itf)
		{
		TInt itfId = itfIter.ItfId();

		if(TCommsClientSideApiExt::IsSupported((TSupportedCommsApiExt)itfId))
			{
			AExtensionInterfaceThickBase& baseItf = *static_cast<AExtensionInterfaceThickBase*>(itf);
			if (iClientId!=0)
				{
				baseItf.CancelExtensionInterface(iClientId);
				baseItf.CloseExtensionInterface(iClientId);
				itfRegister.DeRegisterInterface(itfId, iClientId);
				}
			else
				{
				//TCommsApiExtRegisterClientIter clientIter(itfId, itfRegister);
				TUint client = itfRegister.FirstClient(itfId);
				while (client!=0)
					{
					baseItf.CancelExtensionInterface(client);
					baseItf.CloseExtensionInterface(client);
					client = itfRegister.DeRegisterInterface(itfId, client);
					}
				}
			}
		itf = itfIter++;
		}
	ForwardMessageL(aSender, aMessage, aNode);
	}

DEFINE_MVIP_CTR(TCancelAndCloseAllClientExtItf)

START_ATTRIBUTE_TABLE(TCancelAndCloseAllClientExtItf, KExtItfMsgImplementationUid, ECancelAndCloseAllClientExtItf)
	REGISTER_ATTRIBUTE(TCancelAndCloseAllClientExtItf, iClientId, TMeta<TUint>)
END_ATTRIBUTE_TABLE()

// ----- TApiExtMsgDispatcher -----

void TApiExtMsgDispatcher::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
	{
	const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);
	NetInterfaces::TInterfaceControl* tic = FetchInterfaceControlOrForwardMsgL(aSender, *this, nodeId.Node(), iResponseMsg.InterfaceId());

	if (tic)
	    {
		TBuf8<KMaxExtApiIpcMsgLength> msgDst;
		TCommsApiExtReqMsg* msg = static_cast<TCommsApiExtReqMsg*>(iResponseMsg.ReadClientReqMsg(msgDst));
		User::LeaveIfError(msg? KErrNone : KErrNotSupported);
		//TODO: ASSERT(msg->IsTypeOf(TCommsApiExtReqMsg::TypeId()));
		msg->DispatchL(*tic, iClientId, iResponseMsg);
	    }
	}

void TApiExtMsgDispatcher::Error(const TRuntimeCtxId& /*aSender*/, const TRuntimeCtxId& /*aRecipient*/, TInt aError)
   	{
   	if (aError==KErrNotFound)
   		{
   		//This IPC call has not been served.
   		aError=KErrNotSupported;
   		}

   	iResponseMsg.Complete(aError);
   	}

DEFINE_MVIP_CTR(TApiExtMsgDispatcher)

START_ATTRIBUTE_TABLE(TApiExtMsgDispatcher, KExtItfMsgImplementationUid, EApiExtMsgDispatcher)
	REGISTER_ATTRIBUTE(TApiExtMsgDispatcher, iClientId, TMeta<TUint>)
	REGISTER_ATTRIBUTE(TApiExtMsgDispatcher, iResponseMsg, TMeta<Elements::RResponseMsg>)
END_ATTRIBUTE_TABLE()

const TImplementationProxy ExtItfMsgImplementationTable[] =
	{
	MVIP_CTR_ENTRY(EApiExtMsgDispatcher, TApiExtMsgDispatcher),
	MVIP_CTR_ENTRY(EOpenExtensionInterface, TOpenExtensionInterface),
	MVIP_CTR_ENTRY(ECloseExtensionInterface, TCloseExtensionInterface),
	MVIP_CTR_ENTRY(ECancelAndCloseAllClientExtItf, TCancelAndCloseAllClientExtItf)
	};

void TExtItfMsgTables::RegisterL()
	{
	TlsGlobals::Get().RegisterInterfaceL(TUid::Uid(KExtItfMsgImplementationUid), sizeof(ExtItfMsgImplementationTable) / sizeof(ExtItfMsgImplementationTable[0]), ExtItfMsgImplementationTable);
	}

void TExtItfMsgTables::DeRegister()
	{
	TlsGlobals::Get().DeregisterInterface(TUid::Uid(KExtItfMsgImplementationUid));
	}




