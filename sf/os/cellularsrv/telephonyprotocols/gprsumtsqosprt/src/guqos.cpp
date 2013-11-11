// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <es_ini.h>
#include <timeout.h>
#include <es_prot_internal.h>

#include "guqos.h"
#include "iface.h"
#include "tc.h"
#include "guqos_ini.h"
#include "context.h"
#include "async_request.h"
#include "guqos_log.h"

CModuleGuqos* CModuleGuqos::NewL()
	{
	CModuleGuqos* module = new (ELeave) CModuleGuqos();
	CleanupStack::PushL(module);
	module->ConstructL();
	CleanupStack::Pop();
	return module;
	}

CModuleGuqos::CModuleGuqos()
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("**** CREATING MODULE GUQOS (size=%u) ****"), sizeof(*this)));
	iFlowCount = 0;
	}

void CModuleGuqos::ConstructL()
	{
	iIfManager = CNifManager::NewL();
	iTimeoutManager = TimeoutFactory::NewL(100);	// Time Unit is [1/100 seconds]
	ReadConfigOptions();
	}

void CModuleGuqos::ReadConfigOptions()
	{
	CESockIniData* config = NULL;
	TRAP_IGNORE(config = CESockIniData::NewL(GUQOS_INI_DATA));
	TInt value;
	if (config == NULL || !config->FindVar(GUQOS_INI_VARIABLES, GUQOS_INI_TIMEOUT, value))
		value = KPdpContextDeleteTimeout;
	delete config;
	iOptions.iTimeout = (value + 9999) / 10000;
	LOG(Log::Printf(_L("%S = %d [microseconds] converted to %d [1/100 seconds]"), &GUQOS_INI_TIMEOUT, value, iOptions.iTimeout));
	}

CModuleGuqos::~CModuleGuqos()
	{
	LOG(Log::Printf(_L("~\tGUGOS DESTRUCTOR")));
	ASSERT(iFlowCount == 0);
	delete iIfManager;
	delete iTimeoutManager;
	}

void CModuleGuqos::InterfaceAttached(const TDesC& /*aName*/, CNifIfBase* aIf)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::InterfaceAttached -- start CNifIfBase[%u]"), (TInt)aIf));
	if (!aIf)
		return;		// Nothing to attach! (someone is making invalid calls!).

	if (!iIfManager->FindInterface(aIf))
		{
		TRAPD(err, iIfManager->CreateNifL(*aIf, *this));
		if (err != KErrNone)
			{
			LOG(Log::Printf(_L("\tError creating Nif [error code = %d]"), err));				
			}
		}
	LOG(Log::Printf(_L("Guqos::InterfaceAttached -- end")));
	}

void CModuleGuqos::InterfaceDetached(const TDesC& /*aName*/, CNifIfBase* aIf)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::InterfaceDetached -- start")));
	iIfManager->DeleteNif(aIf);
	LOG(Log::Printf(_L("Guqos::InterfaceDetached -- end")));
	}

// references to protocol must be removed, i.e. drop all packets because they contain CFlowContext references
void CModuleGuqos::Unbind(CProtocolBase* /*aProtocol*/, TUint /*aId = 0*/)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::Unbind -- Nothing to do")));
	// NOTHING TO ´DO FOR NOW
	// CFlowData may include CFlowContext reference, but those
	// are never dereferenced, unless the context is otherwise
	// known to be valid (e.g. in OpenL or is referenced from
	// RMBufSendInfo. Otherwise the reference is used only as
	// opaque identifier.
	}

void CModuleGuqos::InitModuleL(MEventInterface& aEventInterface, CExtension* /*aData*/)
	{
	iNotify = &aEventInterface;
	}

void CModuleGuqos::OpenL(CFlowContext& aFlow, CNifIfBase* aIf)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::OpenL -- start")));
    CNif* nif = iIfManager->FindInterface(aIf);
    if (!nif)
        {
        User::Leave(KErrNotFound);
        }
    CFlowData *flowdata = FindFlow(&aFlow);
    if (flowdata)
        {
        User::Leave(KErrAlreadyExists);
        }

	// Add new CFlowData to iList (will be the first element in the list)
	iList = CFlowData::NewL(aFlow, *nif, iList);
	iFlowCount++;
      aFlow.iIgnoreFlowControl = ETrue;
	iList->SetContext(nif->DefaultPdpContext());
	//aFlow.SetStatus(EFlow_READY);
    LOG(Log::Printf(_L("Guqos::OpenL -- end")));
	}

void CModuleGuqos::Close(CFlowContext& aFlow)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::Close -- start")));
	CFlowData* flowdata = CFlowData::Remove(&aFlow, &iList);
	if (flowdata)
		{
		CPdpContext* context = flowdata->PdpContext();
		if (context && !context->IsPrimary())
			{
			// Schedule a detached CClose request for updating
			// the TFT filters on secondary context. (Either
			// the context is flow specific or leave has not
			// been issued).
			CClose* request = CClose::New(*context);
			if (request)
				{
				context->Nif().AddRequest(*request);
				}
			else
				{
				// Note: If the CClose cannot be allocated due to
				// memory problem, the TFT will remain in effect,
				// it will automaticly correct itself when there
				// is another Leave or Close, that can be executed.
				LOG(Log::Printf(_L("\tCannot schedule TFT filter removal due to out of memory")));
				}
			}
		iFlowCount--;
		iIfManager->CancelPendingRequests(flowdata);
		delete flowdata;
		}
	LOG(Log::Printf(_L("Guqos::Close -- end")));
	}

void CModuleGuqos::Release(CFlowContext& aFlow)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::Release -- start")));
	CFlowData* flowdata = FindFlow(&aFlow);
	if (!flowdata)
		return;
	// Abort any pending actions on the flow
	iIfManager->CancelPendingRequests(flowdata);

	CPdpContext* context = flowdata->PdpContext();
	CPdpContext* default_context = flowdata->Nif().DefaultPdpContext();
	if (context && context != default_context)
		{
		// Flow was attached to some other context, shedule some
		// cleanup for that context.
		CClose* request = CClose::New(*context);
		if (request)
			{
			context->Nif().AddRequest(*request);
			}
		}
	// Note: this detaches the flow from the old context,
	// and may trigger destruction of the "context". Do not
	// reference "context" after this! (the sheduled
	// request is also cancelled implicitly in such case).
	flowdata->SetContext(default_context);
	// Flow will be in blocked state, if default_context does not exist.
	}

void CModuleGuqos::OpenChannel(TInt aChannelId, const TQoSParameters& aParams, CExtensionPolicy& aPolicy, MQoSNegotiateEvent& aNotify, CFlowContext& aFlow)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::OpenChannel -- start id=%d"), aChannelId));
	TInt ret = KErrNone; 

	for (;;) /* FOR EXISTS ONLY FOR BREAK EXITS */
		{
		if (aChannelId <= 0)
			{
			// Channel ID must be > 0
			ret = KErrArgument;
			break;
			}
		CFlowData *flowdata = FindFlow(&aFlow);
		if (flowdata == NULL)
			{
			ret = KErrNotFound;
			break;
			}

		if (IfManager()->FindChannel(aChannelId))
			{
			// Note: This could be due to FindChannel finding a channel
			// which has the delayed destruction and QoS framework is
			// for some reason reusing the channel id for new channel
			// (not possible currently, because QoS does not reuse
			// channel ids).
			ret = KErrAlreadyExists;
			break;		
			}

		// Instead of creating totally new secondary context, there
		// could be a check for expiring, but not yet deleted contexts,
		// and reuse one of them for this channel -- that action
		// would require a different request class, not COpenChannel,
		// which would renegotiate the channel and recompute it's
		// filters.
		// coverity[alloc_fn] coverity[assign] coverity [memory_leak]  	
		COpenChannel* open_request = COpenChannel::New(aChannelId, *flowdata, &aNotify);
		if (open_request == NULL)
			{
			ret = KErrNoMemory;
			break;
			}
		open_request->SetParameters(aParams, aPolicy);
		open_request->SetParametersFlowExtn(aPolicy);
		flowdata->Nif().AddRequest(*open_request);
		/* TERMINATE FOR LOOP -- OpenChannel success -- request completes asyncronously */
		LOG(Log::Printf(_L("Guqos::OpenChannel -- end")));
		return;
		}

	aNotify.RequestComplete(ret, &aParams);
	LOG(Log::Printf(_L("Guqos::OpenChannel -- end err=%d"), ret));
	}

void CModuleGuqos::CloseChannel(TInt aChannelId)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::CloseChannel -- start id=%d"), aChannelId));
	if (aChannelId <= 0)
		return;
	CPdpContext* context = IfManager()->FindChannel(aChannelId);
	if (!context)
		return;
	if (context->IsPrimary())
		{
		// This is a secondary context turned into a default context. The use
		// context as a channel is terminated, but it must continue to live as
		// a default context. Thus, only remove the channel association from
		// the context.
		LOG(Log::Printf(_L("\tTurning secondary context into default only")));
		context->SetChannelId(0);
		}
	else
		{
		//?? Need to use CDeleteRequest request to serialize NIF control!!!
		//?? What happens if this is called in the middle of some request processing?
		context->Delete();		// Notify NIF, destroy context on NIF.
		context->Nif().DeletePdpContext(context);
		}
	LOG(Log::Printf(_L("Guqos::CloseChannel -- end")));
	}

void CModuleGuqos::NegotiateChannel(TInt aChannelId, const TQoSParameters& aParams, CExtensionPolicy& aPolicy, MQoSNegotiateEvent& aNotify)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::NegotiateChannel -- start id=%d"), aChannelId));
	TInt ret = KErrNone;

	if (aChannelId <= 0)
		{
		ret = KErrArgument;
		}
	else
		{
		CPdpContext* context = IfManager()->FindChannel(aChannelId); 
		if (context == NULL)
			{
			ret = KErrNotFound;
			}
		else
			{
			CNegotiateChannel* request = CNegotiateChannel::New(context, &aNotify);
			if (request)
				{
				//coverity[leave_without_push]
				request->SetParameters(aParams, aPolicy);
				//coverity[leave_without_push]
				request->SetParametersFlowExtn(aPolicy);
				context->Nif().AddRequest(*request);
				}
			else
				ret = KErrNoMemory;
			}
		}

	if (ret != KErrNone)
		aNotify.RequestComplete(ret, &aParams);
	LOG(Log::Printf(_L("Guqos::NegotiateChannel -- end res=%d"), ret));
	}

void CModuleGuqos::Join(TInt aChannelId, CFlowContext& aFlow, MQoSNegotiateEvent& aNotify)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::JoinChannel -- start id=%d"), aChannelId));
	TInt ret = KErrNone;
	CPdpContext* channel = IfManager()->FindChannel(aChannelId);
	CFlowData *flowdata = FindFlow(&aFlow);
	if (!channel || !flowdata)
		ret = KErrNotFound;
	else
		{
		CJoinRequest* join_request = CJoinRequest::New(channel, flowdata, &aNotify);
		if (join_request)
			channel->Nif().AddRequest(*join_request);
		else
			ret = KErrNoMemory;
		}

	if (ret != KErrNone)
		aNotify.RequestComplete(ret, NULL);
	LOG(Log::Printf(_L("Guqos::JoinChannel -- end err=%d"), ret));
	}

void CModuleGuqos::Leave(TInt aChannelId, CFlowContext& aFlow, MQoSNegotiateEvent& aNotify)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::LeaveChannel -- start id=%d"), aChannelId));
	TInt ret = KErrNone;
	CPdpContext* channel = IfManager()->FindChannel(aChannelId);
	CFlowData *flowdata = FindFlow(&aFlow);
	if (!channel || !flowdata)
		ret = KErrNotFound;

	if (ret == KErrNone)
		{
		CLeaveRequest* leave_request = CLeaveRequest::New(channel, flowdata, &aNotify);
		if (leave_request)
			channel->Nif().AddRequest(*leave_request);
		else
			ret = KErrNoMemory;
		}

	if (ret != KErrNone)
		aNotify.RequestComplete(ret, NULL);
	LOG(Log::Printf(_L("Guqos::LeaveChannel -- end ret=%d"), ret));
	}

void CModuleGuqos::Negotiate(CFlowContext &aFlow, const TQoSParameters& aParams, MQoSNegotiateEvent& aNotify)
	{
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("Guqos::Negotiate -- start")));
	TInt ret = KErrNone;

	for (;;) /* FOR EXISTS ONLY FOR BREAK EXITS */
		{
		CFlowData* flowdata = FindFlow(&aFlow);
		if (!flowdata)
			{
			ret = KErrNotFound;
			break;
			}
		//?? This needs rework and specification of how things should work!
		//?? But, for now the assumption is that the Negotiate() is not used in the
		//?? current framework.
		
		// The Negotiate operation does not have the extension policies as parameter,
		// like OpenChannel or NegotiateChannel has. The code below attempts to fix
		// this by searching the matching policy. However, only first found is used
		// (in order Override, Application, Default). This gets murky, if more than
		// one match would exist -- should extensions be merged in such case?
		CExtensionPolicy *policy = NULL;
		if ((policy = (CExtensionPolicy*)iNotify->Lookup(aFlow, EPfqosExtensionPolicy, EPfqosOverridePriority)) == NULL &&
			(policy = (CExtensionPolicy*)iNotify->Lookup(aFlow, EPfqosExtensionPolicy, EPfqosApplicationPriority)) == NULL &&
			(policy = (CExtensionPolicy*)iNotify->Lookup(aFlow, EPfqosExtensionPolicy, EPfqosDefaultPriority)) == NULL)
			{
			ret = KErrNotFound;
			break;
			}

		// Instead of creating totally new context, one could search for
		// expiring contexts, and reuse one of them for this flow. Could
		// also prefer a context that has been used for this same flow
		// previously -- less negotiating, as the filter might already be
		// correct.
		COpenChannel* negotiate_request = COpenChannel::New(0, *flowdata, &aNotify);
		if (negotiate_request == NULL)
			{
			ret = KErrNoMemory;
			break;
			}
		//coverity[leave_without_push]
		negotiate_request->SetParameters(aParams, *policy);
		//coverity[leave_without_push]
		negotiate_request->SetParametersFlowExtn(*policy);
		flowdata->Nif().AddRequest(*negotiate_request);
		LOG(Log::Printf(_L("Guqos::Negotiate -- end OK")));
		return;
		}

	aNotify.RequestComplete(ret, &aParams);
	LOG(Log::Printf(_L("Guqos::Negotiate -- end ret=%d"), ret));
	}

TInt CModuleGuqos::Configure(TUint aLevel,TUint aName, TDes8& aOption, TAny* /*aSource*/)
	{
	LOG(Log::Printf(_L("guqos::Configure")));
	if (aLevel == KSOLQoSModule)
		{
		switch (aName)
			{
		case KSoCapabilities:
			if (aOption.Length() >= (TInt)sizeof(TInt))
				{
				//lint -e{826} complains, pointer conversion is ok
				TInt& opt = *(TInt*)aOption.Ptr();
				opt = KModuleCapabilites;
				return KErrNone;
				}
			return KErrArgument;
		default:
			break;
			}
		}
	return KErrNotSupported;
	}


TInt CModuleGuqos::Send(RMBufChain& aPacket, CProtocolBase* /*aSourceProtocol*/)
	{
	for (;;)	/* FOREVER, ONLY FOR BREAK EXITS */
		{
		RMBufSendInfo* const info = RMBufSendPacket::PeekInfoInChain(aPacket);
		if (!info)
			{
			// Malformed packet, cannot do anything with it
			LOG(Log::Printf(_L("Guqos::Send -- packet has no info block!")));	
			break;
			}
		CFlowContext* const context = info->iFlow.FlowContext();
		if (!context)
			{
			// Malformed packet, cannot do anything with it
			LOG(Log::Printf(_L("Guqos::Send -- packet has no flow context!")));
			break;
			}

		CFlowData* flowdata = FindFlow(context);
		if (!flowdata)
			{
			// Flow is not registered with GUQOS and this Send should
			// not have happened.
			info->iFlow.Close();
			LOG(Log::Printf(_L("Guqos::Send -- flow is not open in GUQOS!")));
			break;
			}
		return flowdata->Send(aPacket, *info);
		}
	aPacket.Free();
	return 1;
	}

void CModuleGuqos::Identify(TServerProtocolDesc* aProtocolDesc) const
	{
	Identify(*aProtocolDesc);
	}

void CModuleGuqos::Identify(TServerProtocolDesc& aDesc)
	{
	_LIT(Kguqos, "quqos");

	aDesc.iName=Kguqos;
	aDesc.iAddrFamily=KAfInet;
	aDesc.iSockType=KSockDatagram;
	aDesc.iProtocol=KModuleGUQoS;
	aDesc.iVersion=TVersion(KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber);
	aDesc.iByteOrder=EBigEndian;
	aDesc.iServiceInfo=0;
	aDesc.iNamingServices=0;
	aDesc.iSecurity=KSocketNoSecurity;
	aDesc.iMessageSize=0xffff;
	aDesc.iServiceTypeInfo=EPreferMBufChains | ENeedMBufs;
	aDesc.iNumSockets=KUnlimitedSockets;
	}



CFlowData* CModuleGuqos::FindFlow(const CFlowContext* aFlow)
	{
	return CFlowData::Find(aFlow, iList);
	}

// Default parameters are fetched from the QoS policy db.
TInt CModuleGuqos::GetDefaultParameters(TQoSRequested& aParameters, TUint32 aIapId)
	{
	TInetAddr addr;
	addr.SetAddress(KInet6AddrNone);
	TUidType uid(TUid::Uid(0), TUid::Uid(0), TUid::Uid(0));
	CExtensionPolicy* sel = (CExtensionPolicy*)iNotify->Lookup(addr, addr, 0, 0, 0, EPfqosExtensionPolicy, uid, aIapId, 0);
	if (!sel)
		return KErrNotFound;
	aParameters.ParsePolicyData(sel);
	return KErrNone;
	}
