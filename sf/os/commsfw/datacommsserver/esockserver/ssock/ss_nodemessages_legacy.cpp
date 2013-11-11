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
#include "ss_nodemessages_legacy.h"

#include <comms-infras/api_ext_list.h>
#include <comms-infras/api_ext_msg.h>
#include <cdblen.h>
#include <commdb.h>
#include <ss_glob.h>
#include <comms-infras/ss_connprov.h>
#include "ss_connstates.h"
#include <comms-infras/ss_connlegacy.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/metatype.h>
#include <comms-infras/ss_datamon_apiext.h>
#include <comms-infras/ss_connlegacy.h>
#include "SS_conn.H"

#include <comms-infras/ss_nodemessages_internal_esock.h>
#include "ss_internal_activities.h" 
// although we don't need the XConnectionQueryBase::LoadL() method
// including this header forces us to link against commsdataobjects.lib
// when compiling with Armv5
#include <comms-infras/connectionqueryset.h>

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_commsdataobject.h>
#include <comms-infras/ss_roles.h>
#include <ss_sock.h>
#include "SS_rslv.H"
#include <comms-infras/ss_subconnflow.h>
#include <elements/responsemsg.h>

#ifdef __CFLOG_ACTIVE
	#define KShimCprTag KESockConnectionTag
	_LIT8(KShimCprSubTag, "shimcpr");	// logging tag
#endif

using namespace ESock;
using namespace Elements;
using namespace Messages;
using namespace MeshMachine;


/**
 * TCFSigLegacyRMessage2Ext
 */
TCFSigLegacyRMessage2Ext::TCFSigLegacyRMessage2Ext()
    : Messages::TSignatureBase() {}

TCFSigLegacyRMessage2Ext::TCFSigLegacyRMessage2Ext(const Messages::TNodeSignal::TMessageId& aMessageId, const Den::RSafeMessage& aMessage)
	: Messages::TSignatureBase(aMessageId), iMessage() { iMessage.Duplicate(aMessage); }

TBool TCFSigLegacyRMessage2Ext::HasInterface(MeshMachine::TNodeContextBase& aContext, TInt aInterface)
	{
	TRAPD(err, FetchInterfaceL(aContext.Node(), aInterface));
	
    if (err != KErrNone)
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

TAny* TCFSigLegacyRMessage2Ext::FetchInterfaceL(MeshMachine::AMMNodeBase& aNode, TInt aInterface)
	{
	NetInterfaces::TInterfaceControl* ic = NULL;
	ic = aNode.Id().Node().FetchNodeInterfaceControlL(aInterface);

	return ic->FetchInterfaceL(aInterface);
	}

TAny* TCFSigLegacyRMessage2Ext::FetchInterfaceL(Messages::ANode& aNode, TInt aInterface)
	{
	NetInterfaces::TInterfaceControl* ic = NULL;
	ic = aNode.FetchNodeInterfaceControlL(aInterface);

	return ic->FetchInterfaceL(aInterface);
	}

void TCFSigLegacyRMessage2Ext::ForwardRequestL(MeshMachine::TNodeContextBase& aContext)
	{
	ASSERT(aContext.Activity() != NULL);
	RNodeInterface* rcpt = NULL;

	rcpt = aContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider, TCFClientType::EActive),
			Messages::TClientType(0, Messages::TClientType::ELeaving));

    if(rcpt)
    	{
		aContext.Activity()->PostRequestTo(*rcpt, *this);
    	}
	else
		{
		User::Leave(KErrNotReady);
		}

	}

EXPORT_C TInt TCFSigLegacyRMessage2PlatsecExt::SecureId(TSecureId& aResult) const
    {
    aResult = iMessage.SecureId();

    return KErrNone;
    }

EXPORT_C TInt TCFSigLegacyRMessage2PlatsecExt::VendorId(TVendorId& aResult) const
    {
    aResult = iMessage.VendorId();

    return KErrNone;
    }

EXPORT_C TBool TCFSigLegacyRMessage2PlatsecExt::HasCapability(const TCapability aCapability) const
    {
    return iMessage.HasCapability(aCapability);
    }

EXPORT_C TInt TCFSigLegacyRMessage2PlatsecExt::CheckPolicy(const TSecurityPolicy& aPolicy) const
    {
    TBool platsecResult;
    platsecResult = aPolicy.CheckPolicy(iMessage);
    if (!platsecResult)
        {
        return KErrPermissionDenied;
        }
    return KErrNone;
    }

/**
 *
 * TMCprGetConnectionSetting
 *
 */
TBool TMCprGetConnectionSetting::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, MLinkMCprLegacyDataAccessApiExt::KInterfaceId);
	}

void TMCprGetConnectionSetting::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	if (aContext.Activity()->ActivitySigId() == ECFActivityConnectionLegacyRMessage2Handler)
		{
		TClientIter<TDefaultClientMatchPolicy> iter = aContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider, TCFClientType::EAvailabilityProvider));
		RNodeInterface* sp = iter[0];
		
		if (!sp)
			{
			User::Leave(KErrNotReady);
			}
		
		aContext.Activity()->PostRequestTo(*sp, *this);		
		}
	else
		{
		ForwardRequestL(aContext);
		}
	}
	
void TMCprGetConnectionSetting::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), MLinkMCprLegacyDataAccessApiExt::KInterfaceId);
	ASSERT(interface);

	MLinkMCprLegacyDataAccessApiExt& dataAccessItf = *static_cast<MLinkMCprLegacyDataAccessApiExt*>(interface);
	
	TBuf<KCommsDbSvrMaxColumnNameLength*2 +1> field;
	iMessage.ReadL(0, field);
	
	switch (iSettingType)
		{
	case EIntSetting:
		{
		TUint32 value(0);
		dataAccessItf.GetIntSettingL(field, value, this);
		TPckg<TUint32> pckg(value);
		iMessage.WriteL(1, pckg);
		}
		break;

	case EBoolSetting:
	    {
		TBool value(EFalse);
		dataAccessItf.GetBoolSettingL(field, value, this);
		TPckg<TBool> pckg(value);
		iMessage.WriteL(1, pckg);
		}
		break;

    case EDes8Setting:
	    {
		TBuf8<KCommsDbSvrMaxFieldLength> value;
		dataAccessItf.GetDes8SettingL(field, value, this);
		iMessage.WriteL(1, value);
		}
		break;

    case EDes16Setting:
	    {
		TBuf16<KCommsDbSvrRealMaxFieldLength> value;
		dataAccessItf.GetDes16SettingL(field, value, this);
		iMessage.WriteL(1, value);
		}
		break;

	case ELongDesSetting:
	    {
		HBufC* valueBuf(NULL);
		dataAccessItf.GetLongDesSettingL(field, valueBuf, this);
		CleanupStack::PushL(valueBuf);
		TPtr valuePtr(valueBuf->Des());
		iMessage.WriteL(1,valuePtr);
		CleanupStack::PopAndDestroy(valueBuf);
		}
		break;
		
    default:
		// Should never be able to reach here
		__ASSERT_ALWAYS(EFalse, User::Panic(_L("LegacyDataAccess"), KErrArgument));
		break;
		}
	
	__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprGetConnectionSetting %08x:\tDispatchL() completing message (%08X) with KErrNone"),
			this, iMessage.Handle()));

	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
	responseMsg.Complete(KErrNone);
	}

/**
 *
 * TCprRetrieveProgress
 *
 */
TBool TCprRetrieveProgress::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, MLinkCprApiExt::KInterfaceId);
	}

void TCprRetrieveProgress::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	ForwardRequestL(aContext);
	}

void TCprRetrieveProgress::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = NULL;
	TRAPD(err, interface = FetchInterfaceL(aContext.Node(), MLinkCprApiExt::KInterfaceId));
	ASSERT(interface);

	MLinkCprApiExt::TProgressBuf prog;
	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());

	if (err == KErrNotFound)
	    {
        // If an interface can't be found complete the client with the last
        // progress cached on the CConnection
        MLinkCprApiExt::TProgressBuf prog(iLastProgress);
        responseMsg.WriteL(0, prog);
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprRetrieveProgress %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
        responseMsg.Complete(KErrNone);
        return;
	    }

    if (interface)
    	{
    	MLinkCprApiExt& linkCprItf = *static_cast<MLinkCprApiExt*>(interface);
    	linkCprItf.ProgressL(prog);
        responseMsg.WriteL(0, prog);
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprRetrieveProgress %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
        responseMsg.Complete(KErrNone);
    	}
	}

TInt TCprRetrieveProgress::CheckError(MeshMachine::TNodeContextBase& aContext, TInt aError)
	{
	CConnection* conn = static_cast<CConnection*>(&aContext.Node());
    ConnActivities::CConnLegacyRMessage2Activity* act = static_cast<ConnActivities::CConnLegacyRMessage2Activity*>(aContext.Activity());
    if (aError == KErrNotReady)
        {
        TPckgBuf<TStateChange> progressBuf;
		progressBuf().iStage = conn->iLastProgress.iStage;
		progressBuf().iError = conn->iLastProgress.iError;

        TRAPD(err, act->iSafeMessage.WriteL(0, progressBuf));
		return err;
        }

	return aError;
	}

/**
 *
 * TCprRetrieveLastProgressError
 *
 */
TBool TCprRetrieveLastProgressError::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, MLinkCprApiExt::KInterfaceId);
	}

void TCprRetrieveLastProgressError::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	ForwardRequestL(aContext);
	}

void TCprRetrieveLastProgressError::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = NULL;
	TRAPD(err, interface = FetchInterfaceL(aContext.Node(), MLinkCprApiExt::KInterfaceId));
	ASSERT(interface);

	MLinkCprApiExt::TProgressBuf prog;
	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());

	if (err == KErrNotFound)
        {
        // If an interface can't be found complete the client with the last
        // progress error cached on the CConnection
        MLinkCprApiExt::TProgressBuf prog(iLastProgressError);
        responseMsg.WriteL(0, prog);
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprRetrieveLastProgressError %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
		responseMsg.Complete(KErrNone);
        return;
        }

    if (interface)
    	{
    	MLinkCprApiExt& linkCprItf = *static_cast<MLinkCprApiExt*>(interface);
    	linkCprItf.LastProgressError(prog);
    	responseMsg.WriteL(0, prog);
		__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprRetrieveLastProgressError %08x:\tDispatchL() completing message (%08X) with KErrNone"),
							 this, iMessage.Handle()));
        responseMsg.Complete(KErrNone);
    	}
	}

TInt TCprRetrieveLastProgressError::CheckError(MeshMachine::TNodeContextBase& aContext, TInt aError)
	{
	CConnection* conn = static_cast<CConnection*>(&aContext.Node());
    ConnActivities::CConnLegacyRMessage2Activity* act = static_cast<ConnActivities::CConnLegacyRMessage2Activity*>(aContext.Activity());
    if (aError == KErrNotReady)
        {
        TPckgBuf<TStateChange> progressBuf;
		progressBuf().iStage = conn->iLastProgressError.iStage;
		progressBuf().iError = conn->iLastProgressError.iError;
		conn->ResetLastProgressError();

        TRAPD(err, act->iSafeMessage.WriteL(0, progressBuf));
		return err;
        }

	return aError;
	}

/**
 *
 * TCprRequestServiceNotification
 *
 */
TBool TCprRequestServiceNotification::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, MLinkCprServiceChangeNotificationApiExt::KInterfaceId);
	}

void TCprRequestServiceNotification::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	ForwardRequestL(aContext);
	}

void TCprRequestServiceNotification::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), MLinkCprServiceChangeNotificationApiExt::KInterfaceId);
	ASSERT(interface);

	MLinkCprServiceChangeNotificationApiExt& linkCprItf = *static_cast<MLinkCprServiceChangeNotificationApiExt*>(interface);
	RLegacyResponseMsg response(aContext, iMessage, iMessage.Int0());
	linkCprItf.RequestServiceChangeNotificationL(iSender, response);
	}

void TCprRequestServiceNotification::Cancel(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), MLinkCprServiceChangeNotificationApiExt::KInterfaceId);
	ASSERT(interface);

	MLinkCprServiceChangeNotificationApiExt& linkCprItf = *static_cast<MLinkCprServiceChangeNotificationApiExt*>(interface);
	linkCprItf.CancelServiceChangeNotification(iSender);
	}

/**
 *
 * TCprSendIoctl
 *
 */
TBool TCprSendIoctl::CanProcess(MeshMachine::TNodeContextBase& /*aContext*/)
	{
	return EFalse; // always forward. If a node is going to process, it will do regardless of this
	}

void TCprSendIoctl::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	ForwardRequestL(aContext);
	}

void TCprSendIoctl::ProcessL(MeshMachine::TNodeContextBase& /*aContext*/)
	{
	// do nothing, the nodes that receive the ioctl will handle it themselves
	ASSERT(0);
	}

void TCprSendIoctl::Cancel(MeshMachine::TNodeContextBase& /*aContext*/)
	{
	// cancellation is handled by the node handling the ioctl, this should never be called
	ASSERT(0);
	}

/**
 *
 * TCprFactEnumerateConnections
 *
 */
TBool TCprFactEnumerateConnections::CanProcess(MeshMachine::TNodeContextBase& /*aContext*/)
	{
	return ETrue;
	}

void TCprFactEnumerateConnections::ForwardL(MeshMachine::TNodeContextBase& /*aContext*/)
	{
	ASSERT(0); // should never forward
	}

void TCprFactEnumerateConnections::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
    const TNodeId& nodeId = address_cast<const TNodeId>(aContext.NodeId());  //This message type operates on nodes
    ASSERT(nodeId == SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EConnPlane)));
    TAny* interface = NULL;
    TInt count = 0;
	
    TRAP_IGNORE(interface = FetchInterfaceL(nodeId.Node(), MLinkCprFactApiExt::KInterfaceId));
    if (interface)
        {
    	MLinkCprFactApiExt* cenumapi = reinterpret_cast<MLinkCprFactApiExt*>(interface);
    	cenumapi->EnumerateConnectionsL(*iConnectionInfoPtrArray);
    	count = iConnectionInfoPtrArray->Count();
        }
	
	responseMsg.WriteL(0, TPckg<TUint>(count));
	__CFLOG_VAR((KShimCprTag, KShimCprSubTag, _L8("TCprFactEnumerateConnections %08x:\tDispatchL() completing message (%08X) with KErrNone"),
						 this, iMessage.Handle()));

	responseMsg.Complete(KErrNone);
    }

/**
 * TCprAllSubConnectionNotificationEnable
 * This is the only legacy self dispatcher left.
 */
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
    ASSERT(EFalse);//should never get there. if so than we need to derive from TCFSigRMessage2Ext
    //to add RMessage and complete with error
    }


void CLegacyDataMonitoringResponder::DoComplete(TInt aError)
    {
    iLegacyResponseMsg.Complete(aError);
    }

void CLegacyDataMonitoringResponder::DoCancelRequest()
	{
	iLegacyResponseMsg.Complete(KErrCancel);
	}

void CLegacyDataMonitoringResponder::DoDataTransferred(TUint32 aReceivedBytes, TUint32 aSentBytes)
	{
	TPckg<TUint> uplinkDataVolumePckg(aSentBytes);
	TPckg<TUint> downlinkDataVolumePckg(aReceivedBytes);

	TInt err = iLegacyResponseMsg.Write(1, uplinkDataVolumePckg);
	if(err != KErrNone)
		{
		iLegacyResponseMsg.Complete(err);
		return;
		}
		
	err = iLegacyResponseMsg.Write(2, downlinkDataVolumePckg);
	if(err != KErrNone)
		{
		iLegacyResponseMsg.Complete(err);
		return;
		}
	
	iLegacyResponseMsg.Complete(KErrNone);
	}
	
void CLegacyDataMonitoringResponder::DoDataSentNotification(TUint32 aSentBytes)
	{
	TPckg<TUint> sentBytesPckg(aSentBytes);	

	TInt err = iLegacyResponseMsg.Write(2, sentBytesPckg);
	if(err != KErrNone)
		{
		iLegacyResponseMsg.Complete(err);
		return;
		}
	
	iLegacyResponseMsg.Complete(KErrNone);
	}
	
void CLegacyDataMonitoringResponder::DoDataReceivedNotification(TUint32 aReceivedBytes)
	{
	TPckg<TUint> receivedBytesPckg(aReceivedBytes);	

	TInt err = iLegacyResponseMsg.Write(2, receivedBytesPckg);
	if(err != KErrNone)
		{
		iLegacyResponseMsg.Complete(err);
		return;
		}
	
	iLegacyResponseMsg.Complete(KErrNone);
	}
	
EXPORT_C CLegacyDataMonitoringResponder* CLegacyDataMonitoringResponder::NewL(ESock::RLegacyResponseMsg& aResponseMsg)
	{
	return new(ELeave) CLegacyDataMonitoringResponder(aResponseMsg);
	}

CLegacyDataMonitoringResponder::CLegacyDataMonitoringResponder(ESock::RLegacyResponseMsg& aResponseMsg)
	: CDataMonitoringResponder(aResponseMsg), iLegacyResponseMsg(aResponseMsg)
	{
	}

/**
 *
 * TLegacyDataMonitoringTransferredRequest
 *
 */
TBool TLegacyDataMonitoringTransferredRequest::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, EDataMonitoringApiExt);
	}

void TLegacyDataMonitoringTransferredRequest::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	// If we're at the connection, we must decide which plane to go down
	if (aContext.Activity()->ActivitySigId() == ECFActivityConnectionLegacyRMessage2Handler)
		{
		TInt subConnUniqueId = iMessage.Int0();
		CConnection& connection = static_cast<CConnection&>(aContext.Node());
		
		switch(subConnUniqueId)
			{
			case KNifEMCompatibilityLayerEntireSubConnectionUid:
				if(!connection.ServiceProvider())
					{
					User::Leave(KErrNotReady);
					}
				else
					{
					aContext.Activity()->PostRequestTo(*connection.ServiceProvider(), *this);
					}
				break;
				
			case KNifEMCompatibilityLayerFakeSubConnectionId:
				if(!connection.DefaultSubConnectionServiceProvider())
					{
					User::Leave(KErrNotReady);
					}
				else
					{
					aContext.Activity()->PostRequestTo(*connection.DefaultSubConnectionServiceProvider(), *this);
					}
				break;
				
			default:
				LOG(ESockLog::Printf(KESockConnectionTag,
						_L8("CConnectionLegacy(%08x)::DataMonitoringNotificationRequestL - client return set to %d. Bad subconnection id. (%d) specified."),
						&connection, KErrArgument, subConnUniqueId));
				
				User::Leave(KErrArgument);
				return;
			}
		}
	else 
		{
		ForwardRequestL(aContext);
		}
	}

void TLegacyDataMonitoringTransferredRequest::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface =  FetchInterfaceL(aContext.Node(), EDataMonitoringApiExt);
	ASSERT(interface);
	
	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
	CDataMonitoringResponder* responder = CLegacyDataMonitoringResponder::NewL(responseMsg);
	ADataMonitoringProtocolReq& dataMonItf = *static_cast<ADataMonitoringProtocolReq*>(interface);
	dataMonItf.RequestDataTransferred(responder, iClientId);
	}

void TLegacyDataMonitoringTransferredRequest::Cancel(MeshMachine::TNodeContextBase& /*aContext*/)
	{
	// do nothing, a DataMonitoringTransferedRequest cannot be cancelled as the response is immediate
	}

/**
Return ETrue if the node has a Service Provider, else EFalse.

Code copied from TCFItfExt::ForwardMessageLTCFItfExt::ForwardMessageL().
*/
TBool TLegacyDataMonitoringTransferredRequest::HasServiceProviderL(const Messages::TRuntimeCtxId& aRecipient)
	{
	ANode& node = address_cast<const TNodeId>(aRecipient).Node();  //This message type operates on nodes
	NetInterfaces::TInterfaceControl* ic = node.FetchNodeInterfaceControlL(AMMNodeBase::KInterfaceId);
	MeshMachine::AMMNodeBase* nodeBase = reinterpret_cast<MeshMachine::AMMNodeBase*>(ic->FetchInterfaceL(AMMNodeBase::KInterfaceId));
	return (nodeBase->GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider)) != NULL);
	}

/**
 *
 * TLegacyDataMonitoringNotificationRequest
 *
 */
TBool TLegacyDataMonitoringNotificationRequest::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, EDataMonitoringApiExt);
	}

void TLegacyDataMonitoringNotificationRequest::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	// If we're at the connection, we must decide which plane to go down
	if (aContext.Activity()->ActivitySigId() == ECFActivityConnectionLegacyRMessage2Handler)
		{
		TInt subConnUniqueId = iMessage.Int0();
		CConnection& connection = static_cast<CConnection&>(aContext.Node());
		
		switch(subConnUniqueId)
			{
			case KNifEMCompatibilityLayerEntireSubConnectionUid:
				if(!connection.ServiceProvider())
					{
					User::Leave(KErrNotReady);
					}
				else
					{
					aContext.Activity()->PostRequestTo(*connection.ServiceProvider(), *this);
					}
				break;
				
			case KNifEMCompatibilityLayerFakeSubConnectionId:
				if(!connection.DefaultSubConnectionServiceProvider())
					{
					User::Leave(KErrNotReady);
					}
				else
					{
					aContext.Activity()->PostRequestTo(*connection.DefaultSubConnectionServiceProvider(), *this);
					}
				break;
				
			default:
				LOG(ESockLog::Printf(KESockConnectionTag,
						_L8("CConnectionLegacy(%08x)::DataMonitoringNotificationRequestL - client return set to %d. Bad subconnection id. (%d) specified."),
						&connection, KErrArgument, subConnUniqueId));
				
				User::Leave(KErrArgument);
				return;
			}
		}
	else 
		{
		ForwardRequestL(aContext);
		}
	}

void TLegacyDataMonitoringNotificationRequest::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), EDataMonitoringApiExt);
	ASSERT(interface);
	
	// Pluck out the parameters from the IPC message
	TUint delta = static_cast<TUint>(iMessage.Int1());
	TUint volume = 0;
	TPckg<TUint> volumeBuf(volume);
	iMessage.ReadL(2, volumeBuf);
	
	// Create a ResponseMsg and responder for completing the client
	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
	CDataMonitoringResponder* responder = CLegacyDataMonitoringResponder::NewL(responseMsg);

    ADataMonitoringProtocolReq& dataMonItf = *static_cast<ADataMonitoringProtocolReq*>(interface);
    switch(iDirection)
        {
		case ESent:
			dataMonItf.RequestDataSentNotification(responder, delta, volume, iClientId);
			break;

		case EReceived:
			dataMonItf.RequestDataReceivedNotification(responder, delta, volume, iClientId);
			break;
		
		default:
			delete responder;
        }
	// coverity [leaked_storage] - responder is owned by dataMonItf.RequestDataSentNotification, dataMonItf.RequestDataReceivedNotification
	// therefore we don't need to push them onto the cleanup stack.
	}

void TLegacyDataMonitoringNotificationRequest::Cancel(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), EDataMonitoringApiExt);
	ASSERT(interface);
	
    if (interface)
    	{
    	ADataMonitoringProtocolReq& dataMonItf = *static_cast<ADataMonitoringProtocolReq*>(interface);
		
    	switch(iDirection)
    		{
		case ESent:
			dataMonItf.CancelDataSentNotificationRequest(iClientId);
			break;

		case EReceived:
			dataMonItf.CancelDataReceivedNotificationRequest(iClientId);
			break;
    		}
    	}
	}

/**
 *
 * TLegacyEnumerateSubConnections
 *
 */
TBool TLegacyEnumerateSubConnections::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, ELegacyEnumerateSubConnectionsApiExt);
	}

void TLegacyEnumerateSubConnections::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	ForwardRequestL(aContext);
	}

void TLegacyEnumerateSubConnections::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), ELegacyEnumerateSubConnectionsApiExt);
	ASSERT(interface);

	// Create a ResponseMsg and responder for completing the client
	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
	CLegacyEnumerateSubConnectionsResponder* responder = CLegacyEnumerateSubConnectionsResponder::NewL(responseMsg);
	
	ALegacyEnumerateSubConnectionsApiExt& itf = *static_cast<ALegacyEnumerateSubConnectionsApiExt*>(interface);
	itf.EnumerateSubConnections(responder);
	}


CLegacyEnumerateSubConnectionsResponder* CLegacyEnumerateSubConnectionsResponder::NewL(RLegacyResponseMsg& aResponseMsg)
	{
	return new(ELeave) CLegacyEnumerateSubConnectionsResponder(aResponseMsg);
	}

EXPORT_C void CLegacyEnumerateSubConnectionsResponder::CompleteClient(CLegacyEnumerateSubConnectionsResponder*& aThis, TInt aCount)
	{
	if (aThis==NULL)
		return;

	TPckg<TUint> countPckg(aCount);
	TInt err = aThis->iResponseMsg.Write(0, countPckg);
	aThis->iResponseMsg.Complete(err);

	delete aThis;
	aThis = NULL;
	}

/**
 *
 * TLegacyControlMessage
 *
 */
TBool TLegacyControlMessage::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, MLegacyControlApiExt::KInterfaceId);
	}

void TLegacyControlMessage::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	ForwardRequestL(aContext);
	}

void TLegacyControlMessage::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), MLegacyControlApiExt::KInterfaceId);
	ASSERT(interface);

	MLegacyControlApiExt& controlItf = *static_cast<MLegacyControlApiExt*>(interface);

	TInt length = iMessage.GetDesLengthL(2);
	RBuf8 option;
	option.CreateL(length);
	CleanupClosePushL(option);
	iMessage.ReadL(2, option);
	
	TUint optionLevel = static_cast<TUint>(iMessage.Int0());
	TUint optionName  = static_cast<TUint>(iMessage.Int1());
	
	TInt ret = controlItf.ControlL(optionLevel, optionName, option, this);
	if (ret == KErrNone)
		{
		// write options buffer back to client if required
		if (optionName & KConnWriteUserDataBit)
			{
			iMessage.WriteL(2, option);
			}
		}

	CleanupStack::PopAndDestroy();	// option

	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());

	switch (ret)
		{
		case KErrNone:
	 		__CFLOG_VAR((KESockComponentTag, KCFNodeTag, _L8("TLegacyControlMessage %08x:\tDispatchL() completing message (%08X) with KErrNone"),
					this, iMessage.Handle()));
           	responseMsg.Complete(KErrNone);
			return;
			
		case KErrNotSupported:
			// The option level/name wasn't supported on this node
			// Continue forwarding down until we find an interface that does support
			// it or we hit the bottom
			TRAP(ret, ForwardRequestL(aContext));
			
			if(ret == KErrNotReady)
				{
				// We've reached the bottom of the stack and control call was never serviced
				User::Leave(KErrNotSupported);
				}
			break;
			
		default:
			__CFLOG_VAR((KESockComponentTag, KCFNodeTag, _L8("TLegacyControlMessage %08x:\tDispatchL() completing message (%08X) with %d"),
					this, iMessage.Handle(), ret));
			responseMsg.Complete(ret);
			break;
		}
	}

/**
 *
 * TLegacyConnectionEnumRequest
 *
 */
TLegacyConnectionEnumRequest::TLegacyConnectionEnumRequest(RArray<TNodeId>& aSCPRCandidates, TLegacyConnEnumClients aClientType)
:	iSCPRCandidates(&aSCPRCandidates),
	iClientType(aClientType)
	{
	}

void TLegacyConnectionEnumRequest::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& /*aRecipient*/)
	{
	CSockManData* globs = SockManGlobals::Get();
	CPlayer* player = globs->SelfPlayer();
	ASSERT(player->HasDataPlane());
	CPlayer::TSubSessionContainer subSessions(player->SubSessions());

	// Memory handling "cheat"; alloc the max possible req here and shrink later. Avoids need to
	// keep reallocing & handling errors, and in practice this is a rare function and the overshoot
	// won't be bad (not so many sockets usually, and reallocing more liable to fragmentation)
	const TInt entrySize = (iClientType == ESocket)? sizeof(TConnectionSocketInfo): sizeof(TConnectionClientInfo);
	HBufC8* resBuf = HBufC8::NewL(entrySize * subSessions.Count());
	TPtr8 resDes(resBuf->Des());
	TInt entries = 0;

	TNodeId flow;
	TNodeId scpr;
	TUidType uidType;

	for(TInt i = subSessions.Count() - 1; i >= 0; --i)
		{
		const CSockSubSession* ss = static_cast<const CSockSubSession*>(subSessions[i]);
		if(ss->Type().iType == TCFSubSessInfo::ESocket)
			{
			const CSocket* sock = static_cast<const CSocket*>(ss);
			if(!sock->GetFlowAndSCPR(flow, scpr))
				{
				ss = NULL;
				}
			}
		else if(iClientType == EAll && ss->Type().iType == TCFSubSessInfo::EHostResolver)
			{
			const CHostResolver* hr = static_cast<const CHostResolver*>(ss);
			if(!hr->GetFlowAndSCPR(flow, scpr))
				{
				ss = NULL;
				}
			}
		else
			{
			ss = NULL;
			}

		if(ss != NULL && iSCPRCandidates->Find(scpr) >= 0)
			{
			if(iClientType == ESocket)
				{
				// Enumerating sockets
				TConnectionSocketInfo info;
				if(static_cast<const CSocket*>(ss)->GetConnectionSocketInfo(info) == KErrNone)
					{
					resDes.Append(reinterpret_cast<TUint8*>(&info), sizeof(info));
					++entries;
					}
				}
			else
				{
				// Enumerating clients
				TConnectionClientInfo info;
				ss->GetOwnerInfo(info.iProcessId, uidType, info.iThreadId);
				info.iUid = uidType.MostDerived();
				if (!AConnectionLegacy::DoesConnectionInfoExist(resDes, info))
					{
					resDes.Append(reinterpret_cast<TUint8*>(&info), sizeof(info));
					++entries;
					}
				}
			}
		}
	if(entries < subSessions.Count())
		{
		resBuf = resBuf->ReAlloc(resDes.Length());
		ASSERT(resBuf);	// realloc is shrinking buffer so this is allocator misbehaviour
		}
	TNodeId selfId(globs->iSubConnectionFlowFactories->Id());
	TCFInternalEsock::TLegacyConnectionEnumResponse resp(entries, reinterpret_cast<TInt>(resBuf), KErrNone);
	RClientInterface::OpenPostMessageClose(selfId, aSender, resp);
	}


void TLegacyConnectionEnumRequest::Error(const TRuntimeCtxId& aSender, const TRuntimeCtxId& /*aRecipient*/, TInt aError)
	{
	TCFInternalEsock::TLegacyConnectionEnumResponse resp(0, NULL, aError);
	RClientInterface::OpenPostMessageClose(TNodeId(), aSender, resp); //TODO[PROD] - is this TNodeId() correct????
	}


/**
 *
 * TLegacySubConnectionActiveRequest
 *
 */
TBool TLegacySubConnectionActiveRequest::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, ELegacySubConnectionActiveApiExt);
	}

void TLegacySubConnectionActiveRequest::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	// If we're at the connection, we must decide which plane to go down
	if (aContext.Activity()->ActivitySigId() == ECFActivityConnectionLegacyRMessage2Handler)
		{
		TInt subConnUniqueId = iMessage.Int0();
		CConnection& connection = static_cast<CConnection&>(aContext.Node());
		
		switch(subConnUniqueId)
			{
			case KNifEMCompatibilityLayerEntireSubConnectionUid:
				if(!connection.ServiceProvider())
					{
					User::Leave(KErrNotReady);
					}
				else
					{
					aContext.Activity()->PostRequestTo(*connection.ServiceProvider(), *this);
					}
				break;
				
			case KNifEMCompatibilityLayerFakeSubConnectionId:
				if(!connection.DefaultSubConnectionServiceProvider())
					{
					User::Leave(KErrNotReady);
					}
				else
					{
					aContext.Activity()->PostRequestTo(*connection.DefaultSubConnectionServiceProvider(), *this);
					}
				break;
				
			default:
				LOG(ESockLog::Printf(KESockConnectionTag,
						_L8("TLegacySubConnectionActive(%08x)::ForwardL - client return set to %d. Bad subconnection id. (%d) specified."),
						&connection, KErrArgument, subConnUniqueId));
				
				User::Leave(KErrArgument);
				return;
			}
		}
	else 
		{
		ForwardRequestL(aContext);
		}
	}

void TLegacySubConnectionActiveRequest::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), ELegacySubConnectionActiveApiExt);
	ASSERT(interface);

	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
	CLegacySubConnectionActiveResponder* responder = CLegacySubConnectionActiveResponder::NewL(responseMsg, iMessage.Int1(), iClientId);
	
	ALegacySubConnectionActiveApiExt& subConnActiveItf = *static_cast<ALegacySubConnectionActiveApiExt*>(interface);
	subConnActiveItf.IsSubConnectionActiveRequest(*responder);
	}

void TLegacySubConnectionActiveRequest::Cancel(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), ELegacySubConnectionActiveApiExt);
	ASSERT(interface);

	ALegacySubConnectionActiveApiExt& subConnActiveItf = *static_cast<ALegacySubConnectionActiveApiExt*>(interface);
	subConnActiveItf.CancelIsSubConnectionActiveRequest(iClientId);
	}

CLegacySubConnectionActiveResponder* CLegacySubConnectionActiveResponder::NewL(ESock::RLegacyResponseMsg& aResponseMsg, TUint aTimerPeriod, TUint aClientId)
	{
	CLegacySubConnectionActiveResponder* self = new(ELeave) CLegacySubConnectionActiveResponder(aResponseMsg, aTimerPeriod, aClientId);
	CleanupStack::PushL(self);

	if (self->iTimerPeriod > KMaxTimerPeriod) // secs; underlying CTimer limitation
		{
		User::Leave(KErrArgument);
		}
	
	TPckg<TBool> stateBuf(self->iClientPerceivedState);
	aResponseMsg.ReadL(2, stateBuf);

	CleanupStack::Pop(self);
	return self;
	}

void CLegacySubConnectionActiveResponder::SubConnectionActive(TBool aState)
	{
	TPckg<TBool> subConnectionActiveBuf(aState);
	TInt err = iResponseMsg.Write(2, subConnectionActiveBuf);
	Complete(err);
	}

void CLegacySubConnectionActiveResponder::Error(TInt aError)
	{
	Complete(aError);	
	}

void CLegacySubConnectionActiveResponder::Complete(TInt aError)
	{
	DoComplete(aError);
	
	delete this;
	}

/**
 *
 * TLegacyGetSubConnectionInfo
 *
 */
TBool TLegacyGetSubConnectionInfo::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	return HasInterface(aContext, ELegacyEnumerateSubConnectionsApiExt);
	}

void TLegacyGetSubConnectionInfo::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	ForwardRequestL(aContext);
	}

void TLegacyGetSubConnectionInfo::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = FetchInterfaceL(aContext.Node(), ELegacyEnumerateSubConnectionsApiExt);
	ASSERT(interface);

	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
	ALegacyEnumerateSubConnectionsApiExt& subConnItf = *static_cast<ALegacyEnumerateSubConnectionsApiExt*>(interface);

	subConnItf.GetSubConnectionInfo(iSubConnectionInfo);

	TPckg<TSubConnectionInfo> pckg(iSubConnectionInfo);
	responseMsg.Write(1, pckg);
	responseMsg.Complete(KErrNone);
	}

/**
 *
 * Attribute Tables
 *
 */
START_ATTRIBUTE_TABLE(TCprRetrieveProgress, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ECprRetrieveProgress)
	REGISTER_ATTRIBUTE(TCprRetrieveProgress, iLastProgress, TMeta<TStateChange>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprRetrieveLastProgressError, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ECprRetrieveLastProgressError)
	REGISTER_ATTRIBUTE(TCprRetrieveLastProgressError, iLastProgressError, TMeta<TStateChange>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprRequestServiceNotification, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ECprRequestServiceNotification)
	REGISTER_ATTRIBUTE(TCprRequestServiceNotification, iSender, TMeta<Messages::TNodeId>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprSendIoctl, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ECprSendIoctl)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TCprAllSubConnectionNotificationEnable, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ECprAllSubConnectionNotificationEnable)
END_ATTRIBUTE_TABLE_BASE(TSignalBase, 0)

START_ATTRIBUTE_TABLE(TCprFactEnumerateConnections, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ECprEnumerateConnections)
	REGISTER_ATTRIBUTE(TCprFactEnumerateConnections, iConnectionInfoPtrArray, TMeta<RPointerArray<TConnectionInfo>*>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TCFSigLegacyRMessage2Ext, TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId ) //has to be derived from
    REGISTER_ATTRIBUTE( TCFSigLegacyRMessage2Ext, iMessage, TMeta<RReadOnlyRMessage> )
END_ATTRIBUTE_TABLE_BASE(Messages::TSignatureBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TMCprGetConnectionSetting, TCFLegacyMessage::ERealmId, TMCprGetConnectionSetting::EId)
	REGISTER_ATTRIBUTE(TMCprGetConnectionSetting, iSettingType, TMeta<TConnectionSettingType>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigLegacyRMessage2Response, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ESignatureRMessage2Response )
	REGISTER_ATTRIBUTE( TSigLegacyRMessage2Response, iResponse, TMeta<TLegacyRMessage2Response> )
END_ATTRIBUTE_TABLE_BASE( Messages::TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TGetOrSetParameters, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::EGetOrSetParameters)
	REGISTER_ATTRIBUTE(TGetOrSetParameters, iDataObject, TMeta<XCommsDataObject**>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TLegacyDataMonitoringTransferredRequest, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ELegacyDataMonitoringTransferredRequest)
	REGISTER_ATTRIBUTE(TLegacyDataMonitoringTransferredRequest, iClientId, TMeta<TSubSessionUniqueId>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TLegacyDataMonitoringNotificationRequest, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ELegacyDataMonitoringNotificationRequest)
	REGISTER_ATTRIBUTE(TLegacyDataMonitoringNotificationRequest, iDirection, TMeta<TDataMonitoringDirection>)
	REGISTER_ATTRIBUTE(TLegacyDataMonitoringNotificationRequest, iClientId, TMeta<TSubSessionUniqueId>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TLegacyEnumerateSubConnections, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ELegacyEnumerateSubConnections)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TLegacyControlMessage, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ELegacyControlMessage)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TLegacyConnectionEnumRequest, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ELegacyConnEnumMessage)
	REGISTER_ATTRIBUTE( TLegacyConnectionEnumRequest, iSCPRCandidates, TMeta<TAny*> )
	REGISTER_ATTRIBUTE( TLegacyConnectionEnumRequest, iClientType, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE(TSignalBase, 0)

START_ATTRIBUTE_TABLE(TLegacySubConnectionActiveRequest, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ELegacySubConnectionActiveRequest)
	REGISTER_ATTRIBUTE(TLegacySubConnectionActiveRequest, iClientId, TMeta<TSubSessionUniqueId>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

START_ATTRIBUTE_TABLE(TLegacyGetSubConnectionInfo, TCFLegacyMessage::ERealmId, TCFLegacyMessageInternal::ELegacyGetSubConnectionInfo)
	REGISTER_ATTRIBUTE(TLegacyGetSubConnectionInfo, iSubConnectionInfo, TMeta<TSubConnectionInfo>)
END_ATTRIBUTE_TABLE_BASE(TCFSigLegacyRMessage2Ext, 0)

/**
 * Constuctors
 */
DEFINE_MVIP_CTR(TSigLegacyRMessage2Response);
DEFINE_MVIP_CTR(TGetOrSetParameters);
DEFINE_MVIP_CTR(TMCprGetConnectionSetting);
DEFINE_MVIP_CTR(TCprRetrieveProgress);
DEFINE_MVIP_CTR(TCprRetrieveLastProgressError);
DEFINE_MVIP_CTR(TCprRequestServiceNotification);
DEFINE_MVIP_CTR(TCprSendIoctl);
DEFINE_MVIP_CTR(TCprAllSubConnectionNotificationEnable);
DEFINE_MVIP_CTR(TCprFactEnumerateConnections);
DEFINE_MVIP_CTR(TLegacyDataMonitoringTransferredRequest);
DEFINE_MVIP_CTR(TLegacyDataMonitoringNotificationRequest);
DEFINE_MVIP_CTR(TLegacyEnumerateSubConnections);
DEFINE_MVIP_CTR(TLegacyControlMessage);
DEFINE_MVIP_CTR(TLegacyConnectionEnumRequest);
DEFINE_MVIP_CTR(TLegacySubConnectionActiveRequest);
DEFINE_MVIP_CTR(TLegacyGetSubConnectionInfo);

const TImplementationProxy SignatureImplementationTable[] =
	{


	MVIP_CTR_ENTRY(TMCprGetConnectionSetting::EId, TMCprGetConnectionSetting),                //101

	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::EGetOrSetParameters,TGetOrSetParameters),				             //200
	
	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ECprRetrieveProgress,TCprRetrieveProgress),						     //201
	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ECprRetrieveLastProgressError,TCprRetrieveLastProgressError),         //202
	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ECprRequestServiceNotification,TCprRequestServiceNotification),	     //203
	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ECprSendIoctl,TCprSendIoctl),									     //204

	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ECprAllSubConnectionNotificationEnable,TCprAllSubConnectionNotificationEnable),	//205
	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ECprEnumerateConnections,TCprFactEnumerateConnections),			     //206
	
	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ELegacyDataMonitoringTransferredRequest, TLegacyDataMonitoringTransferredRequest),	//207
	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ELegacyDataMonitoringNotificationRequest, TLegacyDataMonitoringNotificationRequest),	//208

	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ELegacySubConnectionActiveRequest, TLegacySubConnectionActiveRequest), //209
	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ELegacyEnumerateSubConnections, TLegacyEnumerateSubConnections),	      //210

	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ELegacyControlMessage, TLegacyControlMessage),                         //211
	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ELegacyConnEnumMessage, TLegacyConnectionEnumRequest),                 //212

	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ESignatureRMessage2Response, TSigLegacyRMessage2Response),            //213

	MVIP_CTR_ENTRY(TCFLegacyMessageInternal::ELegacyGetSubConnectionInfo, TLegacyGetSubConnectionInfo),            //214
	};

void TCFLegacyMessage::RegisterL()
	{
	TlsGlobals::Get().RegisterInterfaceL(TUid::Uid(TCFLegacyMessage::ERealmId), sizeof(SignatureImplementationTable) / sizeof(SignatureImplementationTable[0]), SignatureImplementationTable);
	}

void TCFLegacyMessage::DeRegister()
	{
	TlsGlobals::Get().DeregisterInterface(TUid::Uid(TCFLegacyMessage::ERealmId));
	}


