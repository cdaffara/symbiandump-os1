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
 @internalComponent
*/

#include <comms-infras/api_ext_list.h>
#include <cdblen.h>
#include <ss_glob.h>
#include <comms-infras/ss_connprov.h>
#include "ss_linkcprnodemessages.h"
#include "ss_legacynodemessages.h"
#include <comms-infras/ss_thread.h>
#include <comms-infras/metatype.h>
#include <comms-infras/ss_log.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_partner.h>
#endif


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockslnkcp, "ESockSSockslnkcp");
#endif

using namespace ESock;
using namespace Elements;
using namespace Messages;

#ifdef __CFLOG_ACTIVE
	#define KShimCprTag KESockConnectionTag
	_LIT8(KShimCprSubTag, "shimcpr");	// logging tag
#endif


// ===== MLinkCprExtensionApi =====

// ----- TCprRetrieveProgress -----
void TCprRetrieveProgress::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
    MLinkCprApiExt::TProgressBuf prog;
    TAny* interface = NULL;
    TRAPD(err,
	     interface = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), MLinkCprApiExt::KInterfaceId);
	     );

	if (err == KErrNotFound)
	    {
        // If an interface can't be found complete the client with the last
        // progress cached on the CConnection
        MLinkCprApiExt::TProgressBuf prog(iLastProgress);
        iMessage.WriteL(0, prog);
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprRetrieveProgress %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
        iMessage.Complete(KErrNone);
        return;
	    }

    if (interface)
    	{
    	MLinkCprApiExt& linkCprItf = *static_cast<MLinkCprApiExt*>(interface);
    	linkCprItf.ProgressL(prog);
        iMessage.WriteL(0, prog);
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprRetrieveProgress %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
        iMessage.Complete(KErrNone);
    	}
    }


// ----- TCprRetrieveLastProgressError -----
void TCprRetrieveLastProgressError::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
    MLinkCprApiExt::TProgressBuf prog;
	TAny* interface = NULL;
	TRAPD(err,
	    interface = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), MLinkCprApiExt::KInterfaceId);
	    );

    if (err == KErrNotFound)
        {
        // If an interface can't be found complete the client with the last
        // progress error cached on the CConnection
        MLinkCprApiExt::TProgressBuf prog(iLastProgressError);
        iMessage.WriteL(0, prog);
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprRetrieveLastProgressError %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
		iMessage.Complete(KErrNone);
        return;
        }

    if (interface)
    	{
    	MLinkCprApiExt& linkCprItf = *static_cast<MLinkCprApiExt*>(interface);
    	linkCprItf.LastProgressError(prog);
    	iMessage.WriteL(0, prog);
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprRetrieveLastProgressError %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
        iMessage.Complete(KErrNone);
    	}
    }


// ----- TCprRequestServiceNotification -----
void TCprRequestServiceNotification::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	TAny* interface = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), MLinkCprServiceChangeNotificationApiExt::KInterfaceId);

    if (interface)
    	{
    	MLinkCprServiceChangeNotificationApiExt& linkCprItf = *static_cast<MLinkCprServiceChangeNotificationApiExt*>(interface);
		Elements::RResponseMsg response(iMessage, iMessage.Int0());
    	linkCprItf.RequestServiceChangeNotificationL(iSender, response);
    	}
    }


// ----- TCprCancelServiceNotification -----
void TCprCancelServiceNotification::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	TAny* interface = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), MLinkCprServiceChangeNotificationApiExt::KInterfaceId);

    if (interface)
    	{
    	MLinkCprServiceChangeNotificationApiExt& linkCprItf = *static_cast<MLinkCprServiceChangeNotificationApiExt*>(interface);
    	linkCprItf.CancelServiceChangeNotification(iSender);
    	}
    }


void TCprCancelServiceNotification::Error(const TRuntimeCtxId& /*aSender*/, const TRuntimeCtxId& /*aRecipient*/, TInt /*aError*/)
    {
    // This happens when someone (IPProto Layer?) try to 
    // send CancelServiceChangeNotification BUT the node supposed to receive
    // it (AgentCpr) is ALREADY marked as Leaving from IPProto.
    
    // In general, it should not happen.
    __ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockslnkcp, 1));
    }



// ----- TCprEnumerateSubConnections -----
void TCprEnumerateSubConnections::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	TAny* interface = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), MLinkCprApiExt::KInterfaceId);

    if (interface)
    	{
//		MLinkCprApiExt& linkCprItf = *static_cast<MLinkCprApiExt*>(interface);
    	TUint connectionCount;
//    	User::LeaveIfError(linkCprItf.EnumerateSubConnectionsL(connectionCount));
   		TPckg<TUint> connectionCountPckg(connectionCount);
   		iMessage.WriteL(0, connectionCountPckg);
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprEnumerateSubConnections %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
		iMessage.Complete(KErrNone);
        }
    }



// ----- TCprSendIoctl -----
void TCprSendIoctl::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	TAny* interface = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), MLinkCprApiExt::KInterfaceId);

    if (interface)
    	{
    	MLinkCprApiExt& linkCprItf = *static_cast<MLinkCprApiExt*>(interface);
    	linkCprItf.SendIoctlMessageL(iMessage);
        }
    }



// ----- TCprCancelIoctl -----
void TCprCancelIoctl::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	TAny* interface = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), MLinkCprApiExt::KInterfaceId);

    if (interface)
    	{
    	MLinkCprApiExt& linkCprItf = *static_cast<MLinkCprApiExt*>(interface);
    	linkCprItf.SendCancelIoctl();
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprCancelIoctl %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
        iMessage.Complete(KErrNone);
    	}
    }



// ----- TCprAllSubConnectionNotificationEnable -----
void TCprAllSubConnectionNotificationEnable::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
	TAny* interface = FetchInterfaceOrForwardMsgL(aSender, *this, nodeId.Node(), MLinkCprApiExt::KInterfaceId);

    if (interface)
    	{
    	MLinkCprApiExt& linkCprItf = *static_cast<MLinkCprApiExt*>(interface);
    	linkCprItf.AllSubConnectionNotificationEnable();
    	}
    }

void TCprAllSubConnectionNotificationEnable::Error(const TRuntimeCtxId& /*aSender*/, const TRuntimeCtxId& /*aRecipient*/, TInt /*aError*/)
    {
    __ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockslnkcp, 2));//should never get there. if so than we need to derive from TCFSigRMessage2Ext
    //to add RMessage and complete with error
    }


// ===== MLinkCprFactExtensionApi =====

// ----- TCprFactEnumerateConnections -----
void TCprFactEnumerateConnections::DispatchL(const TRuntimeCtxId& /*aSender*/, const TRuntimeCtxId& aRecipient)
    {
    const TNodeId& nodeId = address_cast<const TNodeId>(aRecipient);  //This message type operates on nodes
    __ASSERT_DEBUG(nodeId == SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EConnPlane)), User::Panic(KSpecAssert_ESockSSockslnkcp, 3));
    TAny* interface = NULL;
    TInt count = 0;
    TRAP_IGNORE(interface = FetchInterfaceL(nodeId.Node(), MLinkCprFactApiExt::KInterfaceId));
    if (interface)
        {
    	MLinkCprFactApiExt* cenumapi = reinterpret_cast<MLinkCprFactApiExt*>(interface);
    	cenumapi->EnumerateConnectionsL(*iConnectionInfoPtrArray);
    	count = iConnectionInfoPtrArray->Count();
        }
	iMessage.WriteL(0, TPckg<TUint>(count));
	__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprFactEnumerateConnections %08x:\tDispatchL() completing message (%08X) with KErrNone"),
						 this, iMessage.Handle()));
	iMessage.Complete(KErrNone);
    }


START_ATTRIBUTE_TABLE(TCprRetrieveProgress, KESockCprLinkExtMessagesImplementationUid, ECprRetrieveProgress)
	REGISTER_ATTRIBUTE(TCprRetrieveProgress, iLastProgress, TMeta<TStateChange>)
END_ATTRIBUTE_TABLE_BASE(TCFSigRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprRetrieveLastProgressError, KESockCprLinkExtMessagesImplementationUid, ECprRetrieveLastProgressError)
	REGISTER_ATTRIBUTE(TCprRetrieveLastProgressError, iLastProgressError, TMeta<TStateChange>)
END_ATTRIBUTE_TABLE_BASE(TCFSigRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprRequestServiceNotification, KESockCprLinkExtMessagesImplementationUid, ECprRequestServiceNotification)
	REGISTER_ATTRIBUTE(TCprRequestServiceNotification, iSender, TMeta<Messages::TNodeId>)
END_ATTRIBUTE_TABLE_BASE(TCFSigRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprCancelServiceNotification, KESockCprLinkExtMessagesImplementationUid,  ECprCancelServiceNotification)
	REGISTER_ATTRIBUTE(TCprCancelServiceNotification, iSender, TMeta<Messages::TNodeId>)
END_ATTRIBUTE_TABLE_BASE(TSignalBase, 0)

START_ATTRIBUTE_TABLE(TCprEnumerateSubConnections, KESockCprLinkExtMessagesImplementationUid, ECprEnumerateSubConnections)
END_ATTRIBUTE_TABLE_BASE(TCFSigRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprSendIoctl, KESockCprLinkExtMessagesImplementationUid, ECprSendIoctl)
END_ATTRIBUTE_TABLE_BASE(TCFSigRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprCancelIoctl, KESockCprLinkExtMessagesImplementationUid, ECprCancelIoctl)
END_ATTRIBUTE_TABLE_BASE(TCFSigRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprAllSubConnectionNotificationEnable, KESockCprLinkExtMessagesImplementationUid, ECprAllSubConnectionNotificationEnable)
END_ATTRIBUTE_TABLE_BASE(TSignalBase, 0)

START_ATTRIBUTE_TABLE(TCprFactEnumerateConnections, KESockCprLinkExtMessagesImplementationUid, ECprEnumerateConnections)
	REGISTER_ATTRIBUTE(TCprFactEnumerateConnections, iConnectionInfoPtrArray, TMeta<RPointerArray<TConnectionInfo>*>)
END_ATTRIBUTE_TABLE_BASE(TCFSigRMessage2Ext, 0)

const TImplementationProxy SignatureImplementationTable[] =
	{
	//NOTE: Entries must be sorted for the binary search to work efficiently!
	MVIP_CTR_ENTRY(ECprRetrieveProgress,TCprRetrieveProgress),						//1
	MVIP_CTR_ENTRY(ECprRetrieveLastProgressError,TCprRetrieveLastProgressError),	//2
	MVIP_CTR_ENTRY(ECprRequestServiceNotification,TCprRequestServiceNotification),	//3
	MVIP_CTR_ENTRY(ECprCancelServiceNotification,TCprCancelServiceNotification),	//4
	MVIP_CTR_ENTRY(ECprEnumerateSubConnections,TCprEnumerateSubConnections),		//5
	MVIP_CTR_ENTRY(ECprSendIoctl,TCprSendIoctl),									//6
	MVIP_CTR_ENTRY(ECprCancelIoctl,TCprCancelIoctl),								//7
	MVIP_CTR_ENTRY(ECprAllSubConnectionNotificationEnable,TCprAllSubConnectionNotificationEnable),	//8
	MVIP_CTR_ENTRY(ECprEnumerateConnections,TCprFactEnumerateConnections),			//9
	
	MVIP_CTR_ENTRY(EMCprGetConnectionSetting,TMCprGetConnectionSetting),				//10 - legacy ext api

	// Move these to somewhere more appropriate?
	MVIP_CTR_ENTRY(ELegacyDataMonitoringTransferredRequest, TLegacyDataMonitoringTransferredRequest),	//11
	MVIP_CTR_ENTRY(ELegacyDataMonitoringNotificationRequest, TLegacyDataMonitoringNotificationRequest),	//12
	MVIP_CTR_ENTRY(ELegacyDataMonitoringNotificationCancel, TLegacyDataMonitoringNotificationCancel),	//13

	MVIP_CTR_ENTRY(ELegacySubConnectionActiveRequest, TLegacySubConnectionActiveRequest),	// 14
	MVIP_CTR_ENTRY(ELegacyCancelSubConnectionActive, TLegacyCancelSubConnectionActive),	// 15
	MVIP_CTR_ENTRY(ELegacyEnumerateSubConnections, TLegacyEnumerateSubConnections),	// 16
	};

DEFINE_MVIP_CTR(TCprRetrieveProgress);
DEFINE_MVIP_CTR(TCprRetrieveLastProgressError);
DEFINE_MVIP_CTR(TCprRequestServiceNotification);
DEFINE_MVIP_CTR(TCprCancelServiceNotification);
DEFINE_MVIP_CTR(TCprEnumerateSubConnections);
DEFINE_MVIP_CTR(TCprSendIoctl);
DEFINE_MVIP_CTR(TCprCancelIoctl);
DEFINE_MVIP_CTR(TCprAllSubConnectionNotificationEnable);
DEFINE_MVIP_CTR(TCprFactEnumerateConnections);

void TCprLinkExtMessageTables::RegisterL()
	{
	TlsGlobals::Get().RegisterInterfaceL(TUid::Uid(KESockCprLinkExtMessagesImplementationUid), sizeof(SignatureImplementationTable) / sizeof(SignatureImplementationTable[0]), SignatureImplementationTable);
	}

void TCprLinkExtMessageTables::DeRegister()
	{
	TlsGlobals::Get().DeregisterInterface(TUid::Uid(KESockCprLinkExtMessagesImplementationUid));
	}

