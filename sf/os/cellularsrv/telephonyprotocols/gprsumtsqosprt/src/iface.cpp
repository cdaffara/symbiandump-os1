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

#include <in_iface.h>

#include "context.h"
#include "async_request.h"
#include "iface.h"
#include "tc.h"
#include "guqos.h"
#include "guqos_log.h"

CNif* CNif::NewL(CNifIfBase& aInterface, CModuleGuqos& aModule)
	{
	CNif* nif = new (ELeave) CNif(aInterface, aModule);
	CleanupStack::PushL(nif);
	nif->ConstructL();
	CleanupStack::Pop();
	return nif;
	}

//	XNifTimeoutLinkage
//	******************
//	Glue to bind timeout callback from the timeout manager into Timeout() call
//	on the CPdpContext
//
//	*NOTE*
//		This kludgery is all static and compile time, and only used in the constructor
//		of CPdpContext.
//

// This ungainly manoevure is forced on us because the offset is not evaluated early enough by GCC3.4 to be
// passed as a template parameter
#if defined(__X86GCC__) || defined(__GCCE__)
#define KNifTimeoutOffset 2144
__ASSERT_COMPILE(KNifTimeoutOffset == _FOFF(CNif, iTimeout));
#else
#define KNifTimeoutOffset _FOFF(CNif, iTimeout)
#endif

class XNifTimeoutLinkage : public TimeoutLinkage<CNif, KNifTimeoutOffset>
	{
public:
	static void Timeout(RTimeout &aLink, const TTime & /*aNow*/, TAny * /*aPtr*/)
		{
		Object(aLink)->RunPendingRequests();
		}
	};

CNif::CNif(CNifIfBase& aNif, CModuleGuqos& aModule) : iNif(aNif), iModule(aModule), iTimeout(XNifTimeoutLinkage::Timeout)
	{
	LOG(Log::Printf(_L("new\tCNif[%u] size=%d"), (TInt)this, sizeof(*this)));
	iContexts.SetOffset(_FOFF(CPdpContext, iNext));
	iPending.SetOffset(_FOFF(CRequestBase, iLink));
	}

void CNif::ConstructL()
	{
	// The instance for the primary context exists always for CNif
	iPrimary = CPdpContext::NewL(*this, EPrimaryContext, 0);
	AddContext(*iPrimary);

	User::LeaveIfError(RegisterEventHandler());

	// Get IapId from Nif
	TSoIfConnectionInfo netinfo;
	TPckg<TSoIfConnectionInfo> option(netinfo);
	if (iNif.Control(KSOLInterface, KSoIfGetConnectionInfo, option) == KErrNone)
		iIapId = netinfo.iIAPId;
	}

CNif::~CNif()
	{
	LOG(Log::Printf(_L("~\tCNif[%u] Start"), (TInt)this));
	// No upcalls from NIF, we are in destructor!
	CloseInterface();

	// Unconditionallly terminate all request activities

	// Note: CompleteAndDestruct ends up calling "CloseRequest",
	// which may attempt to activate the next request in queue. To avoid
	// this, set iCurrentRequest to NULL.
	iCurrentRequest = NULL;
	while (!iPending.IsEmpty())
		{
		CRequestBase* request = iPending.First();
		request->CompleteAndDestruct(KErrDied, NULL);
		}

	// Remove all PDP Contexts from NIF and GUQOS
	while (!iContexts.IsEmpty())
		{
		CPdpContext *context = iContexts.First();
		// Remove context from iContexts list before
		// giving control out (to NIF) After this
		// the context instance is no more "reachable"
		// from anywhere else.
		RemoveContext(*context);
		context->Delete();	// Delete context from NIF
		delete context;
		}
	iContexts.Reset();
	iTimeout.Cancel();
	LOG(Log::Printf(_L("\tCNif[%u] Destruction Completed"), (TInt)this));
	}

// Initialize the iParameters and return reference
TContextParameters& CNif::ContextParameters()
	{
	//?? Hope this doesn't generate a memory leak?
	//?? is iParameters.iContextConfig.Reset() required instead?
	iParameters = TContextParameters();
	return iParameters;
	}


CPdpContext* CNif::FindContext(TInt aContextId)
	{
	TContextIter iter(iContexts);
	CPdpContext *context;

	while ((context = iter++) != NULL)
		if (context->ContextId() == aContextId)
			return context;
	return NULL;
	}
	
CPdpContext* CNif::FindChannel(TInt aChannelId)
	{
	TContextIter iter(iContexts);
	CPdpContext *context;

	while ((context = iter++) != NULL)
		if (context->ChannelId() == aChannelId)
			return context;
	return NULL;
	}


// Choose new default Pdp context with the highest QoS profile (according to TS 23.107).
//
// *Note*
//		Because in 3GPP there can be only ONE Context without any TFT's, this code
//		assumes that the lower layer (NIF) has already deleted the current primary
//		context. If this is not true, the TFT removal will fail at 3GPP level!
void CNif::SelectNewDefaultContext()
	{
	TContextIter iter(iContexts);
	CPdpContext* context;
	CPdpContext* highestQoS=NULL;

	while ((context = iter++) != NULL)
		{
		if (context == DefaultPdpContext())
			continue;	// Exclude the current default from the search.

		//lint -e{961} would want terminating 'else' (we don't)
		if (!highestQoS)
			highestQoS = context;
		else if (context->GetQoSRanking() < highestQoS->GetQoSRanking())
			highestQoS = context;
		}
	if (highestQoS == NULL)
		{
		// No other contexts available, cannot delete or change the primary!
		LOG(Log::Printf(_L("\tOnly primary context available -- cannot be deleted")));
		return;
		}
	//
	// Assign the new default context!
	// (iPrimary pointer is only additional reference, it does not "own" the
	// pointed object, thus just overwriting it is ok -- all contexts are in
	// iContexts list).
	iPrimary = highestQoS;
	LOG(Log::Printf(_L("\tContext %d (channel=%d) is the new default context"), iPrimary->ContextId(), iPrimary->ChannelId()));
	// Use the CClose request to clean out all TFT filters
	// from the context (when context is primary, the RemoveFilters
	// removes all, regardless of how many flows are connected).
	CClose* request = CClose::New(*highestQoS);
	if (request)
		{
		highestQoS->Nif().AddRequest(*request);
		}
	else
		{
		// If allocation of the request fails, there is not much that can
		// be done, the filters will be left there and packets not matching
		// them will be dropped. However, the cleanup will be attempted
		// again any time the context is modified in such way that filters
		// need to be removed.
		LOG(Log::Printf(_L("\tCould not remove TFTs from new default context -- no room for request")));
		}
	}

// delete PDP context, and bind flows using this context to default PDP context
void CNif::DeletePdpContext(CPdpContext* aContext)
	{

	if (DefaultPdpContext() == aContext)
		{
		// Attempt to select a new default context (may fail)
		SelectNewDefaultContext();
		}

	CPdpContext *pdpDefault = DefaultPdpContext();
	if (pdpDefault != aContext)
		{
		while (!aContext->Flows().IsEmpty())
			{
			CFlowData* flow = aContext->Flows().First();
			// The SetContext will remove the flow from old
			// context, and eventually the aContext->Flows()
			// *MUST* become empty
			// (because aContext != pdpContext)
			flow->SetContext(pdpDefault);
			}
		RemoveContext(*aContext);
		delete aContext;
		}
	else
		{
		// Cannot delete the default context, just block flows (as the
		// context is not actually existing any more).
		aContext->Block();
		}
	}

TInt CNif::RegisterEventHandler()
	{
	TPckgBuf<TEvent> opt;
	opt().iEvent = this;
	LOG(Log::Printf(_L("\tcall NIF Control(KRegisterEventHandler)")));
	return iNif.Control(KSOLInterface, KRegisterEventHandler, opt);
	}

TInt CNif::SetEvents(TBool aValue)
	{
	TPckgBuf<TBool> opt;
	opt() = aValue;
	LOG(Log::Printf(_L("\tcall NIF Control(KContextSetEvents. %d)"), aValue));
	return iNif.Control(KSOLInterface, KContextSetEvents, opt);
	}


// event received from Nif when the Network status changes
TInt CNif::NetworkStatusEvent(const TNetworkParameters& aNetworkEvent)
	{
	switch(aNetworkEvent.iNetworkEventCode)
		{

	case KNetworkConnectionLost:
		break;

	case KNetworkInterfaceDown:
		iModule.IfManager()->DeleteNif(this);
		break;

	default:
		return KErrNotSupported;
		}

	return KErrNone;
	}


// Receive events from umtsnif
TInt CNif::Event(CProtocolBase* aProtocol, TUint aName, TDes8& aOption, TAny* /*aSource*/)
	{
	LOG(Log::Printf(_L("")));	// just make empty line into log
#ifdef _LOG
	TBuf<40> name;

	switch(aName)
		{
		case KContextDeleteEvent:				name.Append(_L("KContextDeleteEvent"));				break;
		case KContextActivateEvent:				name.Append(_L("KContextActivateEvent"));			break;
		case KContextParametersChangeEvent:		name.Append(_L("KContextParametersChangeEvent"));	break;
		case KContextBlockedEvent:				name.Append(_L("KContextBlockedEvent"));			break;
		case KContextUnblockedEvent:			name.Append(_L("KContextUnblockedEvent"));			break;
		case KNetworkStatusEvent:				name.Append(_L("KNetworkStatusEvent"));				break;
		case KContextQoSSetEvent:				name.Append(_L("KContextQoSSetEvent"));				break;
		case KContextTFTModifiedEvent:			name.Append(_L("KContextTFTModifiedEvent"));		break;
		case KPrimaryContextCreated:			name.Append(_L("KPrimaryContextCreated"));			break;
		case KSecondaryContextCreated:			name.Append(_L("KSecondaryContextCreated"));		break;
		case KContextModifyActiveEvent:			name.Append(_L("KContextModifyActiveEvent"));		break;
		case KGetNegQoSEvent:					name.Append(_L("KGetNegQoSEvent"));					break;
		default:								name.Append(_L("error"));							break;
		}

	LOG(Log::Printf(_L("CNif::Event,aName: %d, name: {%S}"), aName, &name));
#else
	LOG(Log::Printf(_L("CNif::Event(name=%d)"), aName));
#endif

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//PREQ399 had to disable this check because in the new architecture, in order to
//plug UmtsIf capable CFProtos (formerly Nifs) they need to be below IProto,
//and hence aProtocol will never match the iNif reference that we have here.
	if ((CNifIfBase *)aProtocol != &iNif)
		{
		LOG(Log::Printf(_L("\tThe CNifIfBase[%u] parameter does not match my CNifIBase[%u]"), (TInt)aProtocol, (TInt)&iNif));
		return KErrNotFound;	// Not called from what we expect! (invalid use of API)
		}
#else
    (void)aProtocol;
#endif
	// Handle KNetworkStatusEvent first.
	if (aName == KNetworkStatusEvent)
		{
		if (aOption.Length() >= (TInt)sizeof(TNetworkParameters))
			{
			const TNetworkParameters& opt = *(TNetworkParameters*)aOption.Ptr();
			NetworkStatusEvent(opt);
			return KErrNone;
			}
		LOG(Log::Printf(_L("\tThe aOption length=%d is too short for TNetworkParameters (size=%d)"),
			aOption.Length(), (TInt)sizeof(TNetworkParameters)));
		return KErrArgument;
		}

	// All remaining valid events use TContextParameters, do the shared
	// checking and preprocess before the switch...
	if (aOption.Length() != (TInt)sizeof(TContextParameters))
		{
		LOG(Log::Printf(_L("\tThe aOption length=%d does not match TContextParamaters (size=%d)"),
			aOption.Length(), (TInt)sizeof(TContextParameters)));
		return KErrArgument;
		}
	const TContextParameters& opt = *(TContextParameters*)aOption.Ptr();

	// PrimaryContextCreated/SecondaryContextCreated do not have the context
	// id in opt yet, handle those separately here (because searching PDP
	// context by id would fail.
	if (aName == KPrimaryContextCreated)
		{
		return PrimaryContextCreated(opt);
		}
	else if (aName == KSecondaryContextCreated)
		{
		SecondaryContextCreated(opt);
		return KErrNone;
		}

	// Even if the aOption is not TContextParameters, nothing bad happens.
	// In that case teh context is searced with random id, which either is
	// found or not.
	CPdpContext *context = FindContext(opt.iContextInfo.iContextId);
	if (context == NULL)
		{
		return KErrNotFound;	// Context does not exist any more
		}

	switch (aName)
		{
	case KContextUnblockedEvent:
		// independent of any request pending
		// NOTE: NIF *MUST* always provide the correct context status for Unblock
		context->SetContextStatus(opt.iContextInfo.iStatus);
		context->UnBlock();
		return KErrNone;

	case KContextBlockedEvent:
		// independent of any request pending
		// NOTE: NIF *MUST* always provide the correct context status for Block
		context->SetContextStatus(opt.iContextInfo.iStatus);
		context->Block();
		return KErrNone;

	case KContextDeleteEvent:
		// independent of any request pending
		context->SetContextStatus(opt.iContextInfo.iStatus);
				
		// qos framework has to be notified!!!
		context->DeleteEvent(opt);
		DeletePdpContext(context);
		return KErrNone;

	case KContextParametersChangeEvent:
		// independent of any request pending
		context->ParametersChangedEvent(opt);
		return KErrNone;

	case KContextQoSSetEvent:
		context->SetQoSReply(iCurrentRequest, opt);
		return KErrNone;

	case KContextTFTModifiedEvent:
		context->ModifyTftReply(iCurrentRequest, opt);
		return KErrNone;


	case KContextActivateEvent:
		context->ActivateReply(iCurrentRequest, opt);
		return KErrNone;

	case KContextModifyActiveEvent:
		context->ModifyActiveReply(iCurrentRequest, opt);
		return KErrNone;

	default:
		break;
		}
	return KErrNotSupported;
	}


void CNif::SetDefaultQoS()
	{
	LOG(Log::Printf(_L("\tSetDefaultQoS -- begin")));
	TQoSRequested policy;
	TInt ret = iModule.GetDefaultParameters(policy, iIapId);
	if (ret == KErrNone)
		{
		TContextParameters& parameters(ContextParameters());
		parameters.iContextConfig.SetUMTSQoSReq(policy);
		TPckg<TContextParameters> opt(parameters);
		LOG(Log::Printf(_L("\tcall NIF Control(KNifSetDefaultQoS)")));
		iNif.Control(KSOLInterface, KNifSetDefaultQoS, opt);
		}
	LOG(Log::Printf(_L("\tSetDefaultQoS -- end")));
	}

void CNif::AddContext(CPdpContext& aContext)
	{
	iContexts.AddLast(aContext);
	}

TInt CNif::PrimaryContextCreated(const TContextParameters& aParams)
	{
	TNifIfInfo info;
	iNif.Info(info);
	LOG(Log::Printf(_L("CNif::PrimaryContextCreated [Nif=%S]"),&info.iName));
	
	iPrimary->SetContextStatus(aParams.iContextInfo.iStatus);

	SetStatus(EReady);
	IssueRequest();
	return KErrNone;
	}

void CNif::SecondaryContextCreated(const TContextParameters& aParams)
	{
	if (iCurrentRequest)
		{
		SetStatus(EReady);
		CPdpContext* context=NULL;
		TRAPD(err, context = CPdpContext::NewL(*this, ESecondaryContext, aParams.iContextInfo.iContextId));
		if (err == KErrNone)
			{
			//coverity[leave_without_push]
			context->SetContextStatus(aParams.iContextInfo.iStatus);
			AddContext(*context);
			iCurrentRequest->Run(EPendingCreate, context, aParams);
			return;	// Do not fall to context destruction.		
			}
		else
			{
			iCurrentRequest->CompleteAndDestruct(err, NULL);
			// Fall to context destruction...
			}
		}
	// Either there was no request to receive this, or running low on memory
	LOG(Log::Printf(_L("CNif::SecondaryContextCreated -- but, GUGOS cannot use it, deleting context")));
	TPckg<TContextParameters> options(aParams);
	iNif.Control(KSOLInterface, KContextDelete, options);
	}

// Issue the creation of secondary PDP context
TInt CNif::NewPdpContext()
	{
	LOG(Log::Printf(_L("\t\tCNif::NewPdpContext -- request creation of secondary context")));
	TPckg<TContextParameters> opt(ContextParameters());
	opt().iContextType = ESecondaryContext;
	const TInt ret(iNif.Control(KSOLInterface, KContextCreate, opt));
	return (ret == KErrNone) ? opt().iReasonCode : ret;
	}


// Turn off events
TInt CNif::CloseInterface()
	{
	return SetEvents(EFalse);
	}

void CNif::AddRequest(CRequestBase& aRequest)
	{
	iPending.AddLast(aRequest);
	++iPendingSequence;	// iPending Modified
	LOG(Log::Printf(_L("\trequest %S[%u] -- Queued for activation on IAP=%u"), aRequest.iName, (TInt)&aRequest, iIapId));
	IssueRequest();
	}

void CNif::CancelPendingRequest(CFlowData* aFlowData)
	{
	LOG(Log::Printf(_L("\tCancelPendingRequest for FLOW -- BEGIN")));
	TUint32 mark;
	do
		{
		mark = iPendingSequence;
		TSglQueIter<CRequestBase> iter(iPending);
		CRequestBase* request;
		while (iPendingSequence == mark && (request = iter++) != NULL)
			request->Cancel(aFlowData);
		}
	while (mark != iPendingSequence);
	LOG(Log::Printf(_L("\tCancelPendingRequest FLOW -- END")));
	}


void CNif::CancelPendingRequest(CPdpContext* aContext)
	{
	LOG(Log::Printf(_L("\tCancelPendingRequest for PDP -- BEGIN")));

	TUint32 mark;
	do
		{
		mark = iPendingSequence;
		TSglQueIter<CRequestBase> iter(iPending);
		CRequestBase* request;
		while (iPendingSequence == mark && (request = iter++) != NULL)
			request->Cancel(aContext);
		}
	while (mark != iPendingSequence);

	LOG(Log::Printf(_L("\tCancelPendingRequest for PDP -- END")));
	}

void CNif::IssueRequest()
	{
	// Request a callback with delay 0, calls RunPendingRequests as soon as possible.
	iTimeout.Set(Module().TimeoutManager(), 0);
	};

void CNif::RunPendingRequests()
	{
	LOG(Log::Printf(_L("")));
	if (Status() != EReady)
		return;

	while (!iPending.IsEmpty() && iCurrentRequest == NULL)
		{
		iCurrentRequest = iPending.First();
		iCurrentRequest->Start();
		}
	}

// Recompute evaluation precedences in use from the current state of PDP contexts.
void CNif::RecomputeEvaluationPrecedences()
	{
	// This is called every time NIF returns the current set of filters.
	//
	// When new packet filters are added for the PDP Context, the
	// precedence indexes must be selected for each of them and
	// marked as used in the current table. Removal of filter must
	// release the index, however at the time of TFT removal request,
	// it is not yet known whether it will succeed.
	//
	// Eventually, NIF reports back what filters
	// and indexes are actually used. Just recomputing the used
	// status by scanning all (few) PDP Contexts and their current
	// filters is the simplest solution to keep the information in
	// in synch with NIF/network view of things.

	Mem::FillZ(iEvaluationPrecedenceMap, sizeof(iEvaluationPrecedenceMap));
	TContextIter iter(iContexts);
	CPdpContext* context;
	while ((context = iter++) != NULL)
		{
		for (TInt i = 0; i < context->iNumFilters; ++i)
			{
			const TInt p = context->iFilters[i].iEvaluationPrecedenceIndex;
			if (p >= 0 && p < KMaxEvaluationPrecedences)
				iEvaluationPrecedenceMap[p] = 1;
			}
		}
	}

// Evaluation precedence must be unique amongst all packet filters related to 
// same APN. 
TInt CNif::FindEvaluationPrecedence()
	{
	TUint i;
	for (i=0; i < KMaxEvaluationPrecedences; i++)
		{
		if (iEvaluationPrecedenceMap[i] == 0)
			{
			iEvaluationPrecedenceMap[i] = 1;
			return i;
			}
		}
	return KErrNotFound;
	}

void CNif::CloseRequest(CRequestBase* aRequest)
	{
	LOG(Log::Printf(_L("\t\tCNif::CloseRequest")));
	iPending.Remove(*aRequest);
	++iPendingSequence;
	// Note: If current request is already NULL, then this does not
	// activate a new request from queue, even if there would be some.
	// This is intentional -- see ~CNif() destructor!
	if (iCurrentRequest != aRequest)
		return;
	iCurrentRequest = NULL;
	IssueRequest();
	}
//
CNifManager* CNifManager::NewL()
	{
	CNifManager* manager = new (ELeave) CNifManager();
	CleanupStack::PushL(manager);
	manager->ConstructL();
	CleanupStack::Pop();
	return manager;
	}

CNifManager::~CNifManager()
	{
	while (!iNifs.IsEmpty())
		{
		CNif* nif = iNifs.First();
		iNifs.Remove(*nif);
		delete nif;
		}
	iNifs.Reset();
	}


CNifManager::CNifManager()
	{
	iNifs.SetOffset(_FOFF(CNif, iNext));
	}

void CNifManager::ConstructL()
	{
	}

CNif* CNifManager::CreateNifL(CNifIfBase& aInterface, CModuleGuqos& aModule)
	{
	CNif* nif = CNif::NewL(aInterface, aModule);
	iNifs.AddLast(*nif);
	nif->SetEvents(ETrue);
	nif->SetDefaultQoS();
	return nif;
	}

void CNifManager::DeleteNif(CNifIfBase* aInterface)
	{
	CNif* nif = FindInterface(aInterface);
	if (nif)
		DeleteNif(nif);
	}

void CNifManager::DeleteNif(CNif* aInterface)
	{
	iNifs.Remove(*aInterface);
	delete aInterface;
	}

CNif* CNifManager::FindInterface(const CNifIfBase *aIface)
	{
	TNifIter iter(iNifs);
	CNif* nif;
	while ((nif = iter++) != NULL)
		if (&nif->Interface() == aIface)
			return nif;
	return NULL;
	}
	
CPdpContext* CNifManager::FindChannel(TInt aChannelId)
	{
	TNifIter iter(iNifs);
	CNif* nif;
	while ((nif = iter++) != NULL)
		{
		CPdpContext* context = nif->FindChannel(aChannelId);
		if (context)
			return context;
		}
	return NULL;
	}


// delete all pending requests related to this flow
void CNifManager::CancelPendingRequests(CFlowData* aFlowData)
	{
	__ASSERT_ALWAYS(aFlowData!=NULL, User::Panic(_L("CNifManager::CancelPendingRequests"), 0));
	TNifIter iter(iNifs);
	CNif *nif;
	while ((nif = iter++) != NULL)
		nif->CancelPendingRequest(aFlowData);
	}
