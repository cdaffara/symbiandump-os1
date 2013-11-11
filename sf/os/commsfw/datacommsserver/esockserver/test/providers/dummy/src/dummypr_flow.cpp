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
// Dummy implementation file for network tier flow
//
//

/**
 @file
 @internalComponent
*/


#include <ss_std.h>
#include <comms-infras/ss_log.h>
#include <ss_pman.h>
#include <comms-infras/ss_nodemessages_internal_esock.h>
#include <comms-infras/ss_protocolparameterset.h>
#include "dummypr_flow.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestdmyprnt, "ESockTestdmyprnt");
#endif

using namespace ESock;
using namespace Messages;




// Construction
//-------------
CDummyFlow* CDummyFlow::NewL(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
	{
	CDummyFlow* self = new (ELeave) CDummyFlow(aFactory, aSubConn, aProtocolIntf);
	return self;
	}

CDummyFlow::CDummyFlow(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
: CSubConnectionFlowBase(aFactory, aSubConn, aProtocolIntf)
	{
    LOG_NODE_CREATE(KESockFlowTag, CDummyFlow)
	}

CDummyFlow::~CDummyFlow()
	{
    LOG_NODE_DESTROY(KESockFlowTag, CDummyFlow)
	}

MSessionControl* CDummyFlow::GetControlL(
	TInt /*aSessionType*/,
	MSessionControlNotify& aSessionControlNotify)
    {
	// Apply binding locally
	__ASSERT_DEBUG(iSessionControlNotify == NULL, User::Panic(KSpecAssert_ESockTestdmyprnt, 1));
	iSessionControlNotify = &aSessionControlNotify;

	return this;
    }

MSessionData* CDummyFlow::BindL(MSessionDataNotify& aNotify)
    {
	LOG( ESockLog::Printf(_L8("CDummyFlow %08x:\tBindL(aNotify 0x%08x)"), this, &aNotify) );

	// Apply binding locally
	__ASSERT_DEBUG(iSessionDataNotify == NULL, User::Panic(KSpecAssert_ESockTestdmyprnt, 3));
	iSessionDataNotify = &aNotify;
	iSubConnectionProvider.RNodeInterface::PostMessage(
		Id(),
		TCFControlProvider::TActive().CRef());
	return this;
    }

void CDummyFlow::Unbind()
    {
	LOG( ESockLog::Printf(_L8("CDummyFlow %08x:\tUnbind()"), this) );
	ASSERT(iSessionDataNotify); //who else is it calling then?

    iSessionControlNotify = NULL;
    iSessionDataNotify = NULL;

    ASSERT(iSubConnectionProvider.IsOpen());
    iSubConnectionProvider.RNodeInterface::PostMessage(
        Id(),
        TCFControlProvider::TIdle().CRef()
        );
    }

ESock::CSubConnectionFlowBase& CDummyFlow::CloneFlowL()
    {
    TDefaultFlowFactoryQuery query (iProtocolIntf->ControlProviderId(), iSubConnectionProvider.RecipientId());

	ESock::CSubConnectionFlowBase& flow = static_cast<ESock::CSubConnectionFlowBase&>(*(Factory().CreateObjectL(query)));

	// Have the new flow become a data client of our subconnection too
	__ASSERT_DEBUG(iSubConnectionProvider.IsOpen(), User::Panic(KSpecAssert_ESockTestdmyprnt, 6));
	TCFPeer::TJoinRequest msg(
		flow.Id(),
		TCFClientType(TCFClientType::EData, TCFClientType::EActive)
		);

	iSubConnectionProvider.PostMessage(Id(), msg);

	return flow;
    }

CSubConnectionFlowBase* CDummyFlow::Flow()
	{
	return this;
	}


// MSessionControl
//----------------
void CDummyFlow::Start()
	{/*__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 7)); return iLowerFlowControl->Start()*/;}

void CDummyFlow::LocalName(TSockAddr& anAddr) const
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 8)); return iLowerFlowControl->LocalName(anAddr);}

void CDummyFlow::SetLocalName(TSockAddr& /*anAddr*/)
	{
    // Fetch the current flow parameters (those that were sent to us) and create a new set for the flow we want
    TFlowParams flowParams;
    flowParams.iProtocol = CDummyFlow::EProtocolId;
    flowParams.iSocketType = KSockDatagram;

    // Create and load a parameter set with the flow parameters that form our binder request
    CFlowRequestParameters* paramSet = CFlowRequestParameters::NewL();
    CleanupStack::PushL(paramSet);
    paramSet->SetFlowParams(flowParams);
    RCFParameterFamilyBundle flowParamsBundle;
    flowParamsBundle.CreateL();
    RParameterFamily family = flowParamsBundle.CreateFamilyL(KFlowParametersFamily);
    family.AddParameterSetL(paramSet, RParameterFamily::ERequested);

    iParamBundle.Open(flowParamsBundle);
    // Construct and send the message
    iSubConnectionProvider.RNodeInterface::PostMessage(
        Id(),
        TCFControlProvider::TNoBearer(iParamBundle).CRef()
        );

    CleanupStack::Pop(paramSet);
 	}

void CDummyFlow::RemName(TSockAddr& anAddr) const
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 10)); iLowerFlowControl->RemName(anAddr);}

TInt CDummyFlow::SetRemName(TSockAddr& anAddr)
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 11)); return iLowerFlowControl->SetRemName(anAddr);}

const TInt KDummyNetworkFlowOptionLevel = CDummyFlowFactory::iUid;
const TInt KFlowProtocolName = 1;

TInt CDummyFlow::GetOption(TUint aLevel, TUint aName, TDes8& anOption) const
	{
	__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 12));

	if(aLevel == KDummyNetworkFlowOptionLevel && aName == KFlowProtocolName)
		{
		anOption.Copy(KDummyProtocolName);
		return KErrNone;
		}
	else
		{
		return iLowerFlowControl->GetOption(aLevel, aName, anOption);
		}
	}

void CDummyFlow::Ioctl(TUint aLevel, TUint aName, TDes8* anOption)
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 13)); iLowerFlowControl->Ioctl(aLevel, aName, anOption);}

void CDummyFlow::CancelIoctl(TUint aLevel, TUint aName)
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 14)); iLowerFlowControl->CancelIoctl(aLevel, aName);}

TInt CDummyFlow::SetOption(TUint /*aLevel*/, TUint /*aName*/, const TDesC8 &/*anOption*/)
	{ return KErrNone; }

void CDummyFlow::ActiveOpen()
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 16)); iLowerFlowControl->ActiveOpen();}

void CDummyFlow::ActiveOpen(const TDesC8& aConnectionData)
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 17)); iLowerFlowControl->ActiveOpen(aConnectionData);}

TInt CDummyFlow::PassiveOpen(TUint aQueueSize)
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 18)); return iLowerFlowControl->PassiveOpen(aQueueSize);}

TInt CDummyFlow::PassiveOpen(TUint aQueueSize, const TDesC8& aConnectionData)
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 19)); return iLowerFlowControl->PassiveOpen(aQueueSize, aConnectionData);}

void CDummyFlow::Shutdown(MSessionControl::TCloseType aOption)
	{ 
    if (iLowerFlowControl)
        { 
        iLowerFlowControl->Shutdown(aOption);
        }
    else
        {
        iSessionControlNotify->CanClose(MSessionControlNotify::EDelete);
        }
	}

void CDummyFlow::Shutdown(MSessionControl::TCloseType aOption, const TDesC8& aDisconnectionData)
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 21)); iLowerFlowControl->Shutdown(aOption, aDisconnectionData);}

void CDummyFlow::AutoBind()
	{__ASSERT_DEBUG(iLowerFlowControl, User::Panic(KSpecAssert_ESockTestdmyprnt, 22));	iLowerFlowControl->AutoBind();}

TInt CDummyFlow::SecurityCheck(MProvdSecurityChecker* /*aChecker*/)
	{ return KErrNone;}


// MSessionData
//-------------
TUint CDummyFlow::Write(const TDesC8& aDesc, TUint aOptions, TSockAddr* anAddr)
	{__ASSERT_DEBUG(iLowerFlowData, User::Panic(KSpecAssert_ESockTestdmyprnt, 24));	return iLowerFlowData->Write(aDesc, aOptions, anAddr);}

TInt CDummyFlow::Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr)
	{__ASSERT_DEBUG(iLowerFlowData, User::Panic(KSpecAssert_ESockTestdmyprnt, 25));	return iLowerFlowData->Write(aData, aOptions, anAddr);}

void CDummyFlow::GetData(TDes8& aDesc, TUint aOptions, TSockAddr* anAddr)
	{__ASSERT_DEBUG(iLowerFlowData, User::Panic(KSpecAssert_ESockTestdmyprnt, 26));	iLowerFlowData->GetData(aDesc, aOptions, anAddr);}

TInt CDummyFlow::GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr)
	{__ASSERT_DEBUG(iLowerFlowData, User::Panic(KSpecAssert_ESockTestdmyprnt, 27));	return iLowerFlowData->GetData(aData, aLength, aOptions, anAddr);}


// MSessionControlNotify
//----------------------
void CDummyFlow::ConnectComplete()
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 28));	iSessionControlNotify->ConnectComplete();}

void CDummyFlow::ConnectComplete(const TDesC8& aConnectData)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 29));	iSessionControlNotify->ConnectComplete(aConnectData);}

void CDummyFlow::ConnectComplete(CSubConnectionFlowBase& anSSP)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 30));	iSessionControlNotify->ConnectComplete(anSSP);}

void CDummyFlow::ConnectComplete(CSubConnectionFlowBase& anSSP,const TDesC8& aConnectData)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 31));	iSessionControlNotify->ConnectComplete(anSSP, aConnectData);}

void CDummyFlow::CanClose(MSessionControlNotify::TDelete aDelete)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 32));	iSessionControlNotify->CanClose(aDelete);}

void CDummyFlow::CanClose(
	const TDesC8& aDisconnectData,
	MSessionControlNotify::TDelete aDelete)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 33));	iSessionControlNotify->CanClose(aDisconnectData, aDelete);}

TInt CDummyFlow::Error(TInt anError,TUint anOperationMask)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 34));	return iSessionControlNotify->Error(anError, anOperationMask);}

void CDummyFlow::Disconnect()
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 35));	iSessionControlNotify->Disconnect();}

void CDummyFlow::Disconnect(TDesC8& aDisconnectData)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 36));	iSessionControlNotify->Disconnect(aDisconnectData);}

void CDummyFlow::IoctlComplete(TDesC8* aBuf)
	{__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 37));	iSessionControlNotify->IoctlComplete(aBuf);}

void CDummyFlow::DisconnectFromListener(CSubConnectionFlowBase& aSSP)
	{
		__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 53));
        iSessionControlNotify->DisconnectFromListener(aSSP);
    }
void CDummyFlow::SetLocalNameComplete()
	{
	__ASSERT_DEBUG(iSessionControlNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 52));
	iSessionControlNotify->SetLocalNameComplete();
	}

// MSessionDataNotify
//-------------------
void CDummyFlow::NewData(TUint aCount)
	{__ASSERT_DEBUG(iSessionDataNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 39)); iSessionDataNotify->NewData(aCount);}

void CDummyFlow::CanSend()
	{__ASSERT_DEBUG(iSessionDataNotify, User::Panic(KSpecAssert_ESockTestdmyprnt, 40)); iSessionDataNotify->CanSend();}

MFlowBinderControl* CDummyFlow::DoGetBinderControlL()
	{
	return this;
	}


// Node stuff
//-----------
void CDummyFlow::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    CSubConnectionFlowBase::ReceivedL(aSender, aRecipient, aMessage);
    TInt err = KErrNone;
    
	if ( aMessage.IsMessage<TEBase::TError>() )
		{
		ASSERT(EFalse); //To be implemented
		}
	else if ( aMessage.IsMessage<TCFControlProvider::TBearer>() )
        {
        }
	else if (aMessage.IsMessage<TCFInternalEsock::TFlowProvision>())
	    {
	    }
	else if (TEChild::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TEChild::TDestroy::EId :
		    DeleteThisFlow();
			break;
		default:
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockTestdmyprnt, 41)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if (TCFDataClient::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
		case TCFDataClient::TStart::EId :
		    iSubConnectionProvider.PostMessage(Id(), TCFDataClient::TStarted().CRef());
			break;
		case TCFDataClient::TStop::EId :
            iParamBundle.Close(); //[RZ] There is a problem with RCFParameterBundle. Nominally this should be cleared (closed) on TBeaer
                                  //but that means that this thread (which constructed the bundle) will call Close before the recipient
                                  //refcounting will mean that it's the recipient that would then dealocate and crash against different heap.
                                  //Best if RCFParameterBundle were changed. Alternativelly, CNoBearer could get rid of the params 
                                  //earlier.

		    if (iLowerFlowBinderControl)
		           {
		           iLowerFlowBinderControl->Unbind(NULL,NULL);
		           iLowerFlowBinderControl->Unbind();
		           iLowerFlowBinderControl = NULL;
		           iLowerFlowControl = NULL;
		           iLowerFlowData = NULL;
		           }

		       iSubConnectionProvider.PostMessage(Id(), TCFDataClient::TStopped(
                           message_cast<TCFDataClient::TStop>(aMessage).iValue).CRef());
			break;
		case TCFDataClient::TProvisionConfig::EId:
			break;
		case TCFDataClient::TBindTo::EId:
		    {
			TCFDataClient::TBindTo& bindToMsg(static_cast<TCFDataClient::TBindTo&>(aMessage));
			TRAP(err, BindToL(bindToMsg));

	        if(err == KErrNone)
	            {
	            RClientInterface::OpenPostMessageClose(Id(), aSender, TCFDataClient::TBindToComplete().CRef());
	            }
	        else
	            {
	            RClientInterface::OpenPostMessageClose(Id(), aSender, TEBase::TError(aMessage.MessageId(), err).CRef());
	            }
	        
			if (iSessionControlNotify && !iParamBundle.IsNull())
			    {
                iSessionControlNotify->SetLocalNameComplete();
			    }
		    }
			break;
		default:
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockTestdmyprnt, 42)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else
		{
    	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockTestdmyprnt, 45)); //For debug configurations
    	User::Leave(KErrNotSupported); //For release configurations
		}
    }

void CDummyFlow::BindToL(TCFDataClient::TBindTo& aBindTo)
/**
Request from control side (at network layer) to indicate that the SubConnection is
up and running and that we should bind to a Flow below.

@param aLowerFlow Flow below to bind to.
*/
	{
	//provisioning message must come before bindto in case we didn't get it after we've joined
	//the sub-connection
    if (aBindTo.iNodeId == Messages::TNodeId::NullId())
        {
        return;
        }
	NM_LOG((KESockServerTag, _L8("CDummyFlow %08x:\tSynchronous call: From=%08x To=%08x Func=BindToL"),
			this, static_cast<Messages::ANode*>(this), &aBindTo.iNodeId.Node()) )

	const TNodeId& commsId = aBindTo.iNodeId;

#if defined(__GCCXML__)
    CSubConnectionFlowBase* flow = reinterpret_cast<CSubConnectionFlowBase*>(&commsId.Node());
#else
	CSubConnectionFlowBase* flow = Messages::mnode_cast<CSubConnectionFlowBase>(&commsId.Node());
#endif

	// Flows can only be directly bound when running in the same thread
	__ASSERT_DEBUG(commsId.Thread() == Id().Thread(), User::Panic(KSpecAssert_ESockTestdmyprnt, 47));

	if (iLowerFlowBinderControl && iLowerFlowBinderControl->Flow() != flow )
		{
		// Already bound to something else so unbind first
		iLowerFlowBinderControl->Unbind(NULL,NULL);
		iLowerFlowBinderControl = NULL;

		iLowerFlowBinderControl->Unbind();
		iLowerFlowControl = NULL;
		iLowerFlowData = NULL;
		}

	if (iLowerFlowBinderControl == NULL)
		{
		// Protocol binding
		iLowerFlowBinderControl = flow->GetBinderControlL();
		iLowerFlowControl = iLowerFlowBinderControl->GetControlL(KSockDatagram, *this);
		iLowerFlowData = iLowerFlowBinderControl->BindL(*this);
		}
	}



