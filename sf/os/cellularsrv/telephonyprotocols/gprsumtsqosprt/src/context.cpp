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

#include "guqos.h"
#include "iface.h"
#include "tc.h"
#include "parameters.h"
#include "guqos_err.h"
#include "async_request.h"
#include "context.h"
#include "guqos_log.h"

#ifdef _LOG
// *****************
// Logging utilities
// *****************

// Return count of contiguous most significant 1-bits in TUint32
static TInt MaskLength(TUint32 aAddr)
	{
	TInt count = 0;
	// obviously, this is "brute force" counting
	while (aAddr & 0x80000000)
		{
		count++;
		aAddr <<= 1;
		}
	return count;
	}

//Return count of contiguous most significant 1-bits in IPv6 address.
static TInt MaskLength(const TIp6Addr &aAddr)
	{
	TInt count = 0;
	for (TUint i = 0; i < sizeof(aAddr.u.iAddr8) / sizeof(aAddr.u.iAddr8[0]); ++i)
		if (aAddr.u.iAddr8[i] == 0xFF)
			count += 8;
		else
			{
			count += MaskLength(aAddr.u.iAddr8[i] << 24);
			break;
			}
	return count;
	}

// A buffer to contain textual format of internet address
class TAddressBuf : public TBuf<70>
	{
public:
	TAddressBuf(const TIp6Addr& aAddr, TInt aMask, TUint16 aPort = 0);
	};

TAddressBuf::TAddressBuf(const TIp6Addr& aAddr, TInt aMask, TUint16 aPort)
	{
	TInetAddr addr(aAddr, 0);
	addr.Output(*this);
	if (aMask != 128)
		{
		if (aAddr.IsV4Mapped())
			aMask -= 96;
		_LIT(KFormat1, "/%u");
		AppendFormat(KFormat1, aMask);
		}
	if (aPort)
		{
		_LIT(KFormat2, "#%u");
		AppendFormat(KFormat2, (TInt)aPort);
		}
	}

// Log packet filter address selector
// Make this not "static", so that it can be used by other modules too...
void LogPacketFilter(const TPacketFilter& aFilter)
	{
	TIp6Addr srcAddr;
	Mem::Copy(&srcAddr,aFilter.iSrcAddr, sizeof(srcAddr));
	TIp6Addr mask;
	Mem::Copy(&mask,aFilter.iSrcAddrSubnetMask,sizeof(mask));
	TAddressBuf addr(
		srcAddr, 
		MaskLength(mask),
		aFilter.iSrcPortMin);
	Log::Printf(_L("\t\t%u(%u) %S-%u dst#%u-%u SPI=%u TOS=%u FL=%u"),
		aFilter.iId, aFilter.iEvaluationPrecedenceIndex,
		&addr, (TInt)aFilter.iSrcPortMax,
		(TInt)aFilter.iDestPortMin, (TInt)aFilter.iDestPortMax,
		(TInt)aFilter.iIPSecSPI,
		(TInt)aFilter.iTOSorTrafficClass,
		(TInt)aFilter.iFlowLabel);
	}
#endif

//	XPdpContextTimeoutLinkage
//	************************
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
#define KPdpContextTimeoutOffset 716
__ASSERT_COMPILE(KPdpContextTimeoutOffset == _FOFF(CPdpContext, iTimeout));
#else
#define KPdpContextTimeoutOffset _FOFF(CPdpContext, iTimeout)
#endif

class XPdpContextTimeoutLinkage : public TimeoutLinkage<CPdpContext, KPdpContextTimeoutOffset>
	{
public:
	static void Timeout(RTimeout &aLink, const TTime & /*aNow*/, TAny * /*aPtr*/)
		{
		Object(aLink)->Timeout();
		}
	};


CPdpContext* CPdpContext::NewL(CNif& aNifItem, const TContextType& aType, TUint8 aContextId)
	{
	CPdpContext* context = new (ELeave) CPdpContext(aNifItem, aType, aContextId);
	CleanupStack::PushL(context);
	context->ConstructL();
	CleanupStack::Pop();
	return context;
	}

CPdpContext::CPdpContext(CNif& aNifItem, TContextType aType, TUint8 aContextId) : 
	iContextId(aContextId),
	iNifItem(aNifItem),
	iTimeout(XPdpContextTimeoutLinkage::Timeout)
	{
	iContextType = aType;
	iContextStatus = RPacketContext::EStatusUnknown;
	iFlows.SetOffset(_FOFF(CFlowData, iLink));
	//iTrafficClass = -1;
	}


void CPdpContext::ConstructL()
	{
	}

CPdpContext::~CPdpContext()
	{
	LOG(Log::Printf(_L("CPdpContext::~CPdpContext [ContextId=%d]"),iContextId));
	while (!iFlows.IsEmpty())
		{
		// Any flows still attached to the PDP Context instance when
		// we are in destructor, are deleted. If automatic move of
		// flows to another context is the desired, it must be taken
		// care before deleting the context (this destructor is also
		// called from CNif destructor, and we cannot reference
		// any other contexts on the CNif)
		CFlowData* flow = iFlows.First();
		// Deleting flow removes it from the list,
		// and eventually the iFlows will become EMPTY!
		delete flow;
		}
	iNifItem.CancelPendingRequest(this);
	iTimeout.Cancel();	// ..if any active
	}


void CPdpContext::Timeout()
	{
	LOG(Log::Printf(_L("")));
	if (Flows().IsEmpty())
		{
		LOG(Log::Printf(_L("CPdpContext::Timeout -- no flows, delete context")));
		Delete();
		Nif().DeletePdpContext(this);
		}
	else
		{
		LOG(Log::Printf(_L("CPdpContext::Timeout -- flows have re-appeared, keep context")));
		// Some flows have come back, context is not
		// deleted after all. But, because TFT's are
		// not updated when the last flow leaves the
		// context and this callback is scheduled, we
		// now may have garbage TFT's left on the
		// context. To cleanup the situation, schedule
		// a CClose request.
		CClose* request = CClose::New(*this);
		if (request)
			Nif().AddRequest(*request);
		// If allocating request object fails, just
		// ignore the issue of extra TFT's. It will
		// eventually fix itself next time some flow
		// leaves the context or context is deleted.
		}
	}
	
void CPdpContext::NotifyBlockingState(TUint aOldBlocking)
	{
	const TUint blocked = IsFlowBlocked();
	if (blocked == aOldBlocking)
		{
		LOG(Log::Printf(_L("\t\tContextId = %d (channel=%d) Blocking unchanged (%d)"), ContextId(), ChannelId(), blocked));
		return;		// No Change, no need to notify anyone.
		}
	LOG(Log::Printf(_L("\t\tContextId = %d (channel=%d) Blocking changed to (%d)"), ContextId(), ChannelId(), blocked));

	MEventInterface *notifier = Nif().Module().QoSEvent();
	if (!notifier)
		{
		LOG(Log::Printf(_L("\tOops -- QoS has not setup the MEventInterface")));
		return;
		}
	
	// Set all flows associated with this context into blocked or unblocked state
	TDblFlowIter iter(iFlows);
	CFlowData *flow;
	//?? Potentially dangerout iterator, becuase BlockFlow upcall
	//?? may call back and modify the list!
	while ((flow = iter++) != NULL)
		{
		// Throw away "const". The MEventInterface is misdeclared to
		// use non-const reference parameter, when const reference
		// would be the correct way...
		if (blocked)
			notifier->BlockFlow((CFlowContext &)flow->FlowContext());
		else
			notifier->UnBlockFlow((CFlowContext &)flow->FlowContext());
		}
	}

// Block flows on this context, no packets are sent to these flows after this.
void CPdpContext::Block()
	{
	const TUint blocked = IsFlowBlocked(); // get old state
	iBlocked = ETrue;
	NotifyBlockingState(blocked);
	}


// Unblock flows on this context. Re-enable packet sending for these flows.
void CPdpContext::UnBlock()
	{
	LOG(Log::Printf(_L("\tCPdpContext::UnBlock()")));
	const TUint blocked = IsFlowBlocked(); // get old state
	iBlocked = EFalse;
	NotifyBlockingState(blocked);
	}


// Check if CFlowContext matches with packet filter attributes
TBool CPdpContext::MatchPacketFilter(const TPacketFilter& aFlow, const TPacketFilter& aFilter) const
	{
	if (Mem::Compare(aFlow.iSrcAddr, 16, aFilter.iSrcAddr, 16) != 0)
		return EFalse;
	
	if (Mem::Compare(aFlow.iSrcAddrSubnetMask, 16, aFilter.iSrcAddrSubnetMask, 16) != 0)
		return EFalse;

	if (aFlow.iProtocolNumberOrNextHeader != (TUint)aFilter.iProtocolNumberOrNextHeader)
		return EFalse;

	if (aFlow.iDestPortMin < aFilter.iDestPortMin || aFlow.iDestPortMax > aFilter.iDestPortMax)
		return EFalse;

	if (aFlow.iSrcPortMin < aFilter.iSrcPortMin || aFlow.iSrcPortMax > aFilter.iSrcPortMax)
		return EFalse;

	if (aFlow.iFlowLabel != aFilter.iFlowLabel)
		return EFalse;

	if (aFlow.iTOSorTrafficClass != aFilter.iTOSorTrafficClass)
		return EFalse;

	return ETrue;
	}


// Return QoS ranking according to TS 23.107
//
// (only used for turning some secondary context into new default/primary context)
TInt CPdpContext::GetQoSRanking()
	{
	TInt ranking = 6;
	switch (TrafficClass())
		{
	case RPacketQoS::ETrafficClassConversational:
		ranking = 2;
		break;

	case RPacketQoS::ETrafficClassStreaming:
		ranking = 3;
		break;

	case RPacketQoS::ETrafficClassInteractive:
		{
		const TInt priority = iNegotiated.iTrafficHandlingPriority;
		if (priority == 1)
			ranking = 1;
		else if (priority == 2)
			ranking = 4;
		else
			ranking = 5;
		}
		break;

	default:
	case RPacketQoS::ETrafficClassBackground:
		ranking = 6;
		break;
		}
	return ranking;
	}


TInt CPdpContext::FindPacketFilterId()
	{
	// Valid filter id's are from 1 - 8, find one which
	// is not yet in use: First find out used id's into mask
	
	// Note: 3GPP specification uses 0-7 as the filter id.
	// The etelpckt.h range [1-8] is adjusted to [0-7] at
	// lower layers.
	TUint mask = 0;
	for (TInt i = iNumFilters; --i >= 0;)
		{
		const TPacketFilter& f = iFilters[i];
		// Note silly naked constants, but there are no symbols for these?
		if (f.iId > 0 && f.iId < 9)
			{
			mask |= (1 << f.iId);
			}
		}
	// Find an empty id slot.
	for (TInt j = 0; ++j < 9; )
		{
		if ((mask & (1 << j)) == 0)
			return j;
		}
	//??What error is supposed to be returned?
	//return KErrNotFound;
	return KErrOverflow;
	}


// Add packet filter to TFT, return error or number of filters added
TInt CPdpContext::AddPacketFilter(CFlowData& aFlow, TTFTInfo& aTft)
	{
	TInt ret = KErrNone;

	LOG(Log::Printf(_L("\t\tCPdpContext::AddPacketFilter")));


	TPacketFilter& flow_filter = aFlow.PacketFilter();

	for (TInt i = iNumFilters; --i >= 0; )
		if (MatchPacketFilter(flow_filter, iFilters[i]))
			{
			// The flow is already covered by an existing filter,
			// no need to add anything, return 0.
			return 0;
			}

	// Update flow filter with dynamic fields.
	// This only marks the Id and evaluation precedende as reserved. It
	// does not add the filter to the context object (iNumFilters is not
	// incremented).
	// It is assumed that once NIF completes the request, it will return
	// the complete state of the context, including all currently active
	// filters. At that point GUQOS state is recomputed to match the
	// actual reality.
	
	// Not touching the iNumFilters works for now, because there is at
	// most one negotiation open on any interface, and because negoatiation
	// currently adds at most one new filter.
	// If above changes, and some negotiation would need to add multiple
	// filters, then additional logic needs to remember the to-be-added
	// filters and assigned ids.
	ret = FindPacketFilterId();
	if (ret < 0)
		{
		// All filter slots already used!
		LOG(Log::Printf(_L("\t\tNo PacketFilterId -- all used")));
		return ret;
		}
	flow_filter.iId = ret;
	LOG(Log::Printf(_L("\t\tPacketFilterId: %d"), flow_filter.iId));

	flow_filter.iEvaluationPrecedenceIndex = Nif().FindEvaluationPrecedence();
	LOG(Log::Printf(_L("\t\tEvaluationPrecedenceIndex: %d"), flow_filter.iEvaluationPrecedenceIndex));
	LOG(LogPacketFilter(flow_filter));
	ret = aTft.AddPacketFilter(flow_filter);
	return ret < 0 ? ret : 1;	// if OK, 1 filter added
	}


// Remove packet filter(s) from the TFT, return error or number of filters to remove
TInt CPdpContext::RemovePacketFilter(TTFTInfo& aTft, const CFlowData* aFlow)
	/**
	* Find the set of unused filters.
	*
	* Scan all currently attached flows and match them against current set of
	* filters. Return the list of filters that didn't match any flows.
	*
	* One optional flow can be treated as non-attached (even if it appears
	* in the attached list) and excluded matching test. This allows caller
	* to leave a flow in the attached list for later removal.
	*
	* @retval aTft The list of unused filters
	* @par	aFlow Optional flow to exclude from the matching.
	*
	* @return Negative error code (< 0), or number of unsed filters (>= 0),
	* which were added into.
	*/
	{
	// Because filters can match multiple flows (if someone actually starts
	// using the netmask and port ranges), this must test whether any of the
	// current filters become unnecessary when the indicated flow has been
	// removed.

	LOG(Log::Printf(_L("\t\tCPdpContext::RemovePacketFilter")));
	TUint mask = 0;

	// If the context is current designated "primary", then all filters
	// must be removed (there can be filters only if this primary is
	// a secondary context that has been assigned as new default).
	if (!IsPrimary())
		{
		TDblFlowIter iter(iFlows);
		CFlowData *flow;
		while ((flow = iter++) != NULL)
			{
			if (flow == aFlow)
				continue;	// skip the flow being removed.
			const TPacketFilter& flow_filter = flow->PacketFilter();
			for (TInt i = iNumFilters; --i >= 0; )
				if (MatchPacketFilter(flow_filter, iFilters[i]))
					{
					mask |= (1 << i);	// Mark this filter as used.
					break;				// No need to test other filters for this flow.
					}
			}
		}
	// Schedule removal of all filters that were not used.
	TInt count = 0;
	for (TInt j = 0; j < iNumFilters; ++j, mask >>= 1)
		{
		if ((mask & 1) == 0)
			{
			// This filter is not needed anymore
			LOG(LogPacketFilter(iFilters[j]));
			TInt ret = aTft.AddPacketFilter(iFilters[j]);
			if (ret != KErrNone)
				return ret;
			++count;
			}
		}
	return count;
	}

//
// Set QoS parameters for a Pdp context.
// Note: Change is not signaled to network until Activate/ModifyActive is called.
//
TInt CPdpContext::SetQoS(const TQoSParameters& aGeneric, const TQoSRequested& aUmts)
	{
	LOG(Log::Printf(_L("\t\tCPdpContext::SetQoSL on PDP Context [ContextId=%d]"),iContextId));

	TContextParameters& parameters(Nif().ContextParameters());
	
	TQoSRequested requested;
	ParameterMapper::MapGenericToRel99(aGeneric, requested);

	// If adapt flag set, minimum params are set to unspecified
	if (aGeneric.AdaptMode())
		requested.ClearMinimumParameters();

	ParameterMapper::CombineOverride(aUmts, requested);

	parameters.iContextConfig.SetUMTSQoSReq(requested);
	parameters.iContextConfig.SetPdpCompression(requested.iHeaderCompression);
	parameters.iContextInfo.iContextId = iContextId;
	parameters.iContextType = iContextType;
	TPckg<TContextParameters> options(parameters);
	const TInt ret = Nif().Interface().Control(KSOLInterface, KContextQoSSet, options);
	return ret == KErrNone ? parameters.iReasonCode : ret;
	}


// Set QoS parameters for a Pdp context from defaults
// Note: Change is not signaled to network until Activate/ModifyActive is called.
TInt CPdpContext::SetQoS()
	{
	LOG(Log::Printf(_L("\t\tCPdpContext::SetQoSL on PDP Context [ContextId=%d ChannelId=%d]"), iContextId, iChannelId));

	TContextParameters& parameters(Nif().ContextParameters());
	TQoSRequested policy;
	Nif().Module().GetDefaultParameters(policy, Nif().IapId());
	parameters.iContextConfig.SetUMTSQoSReq(policy);
	parameters.iContextConfig.SetPdpCompression(policy.iHeaderCompression);
	parameters.iContextInfo.iContextId = iContextId;
	parameters.iContextType = iContextType;
	TPckg<TContextParameters> options(parameters);
	const TInt ret = Nif().Interface().Control(KSOLInterface, KContextQoSSet, options);
	return ret == KErrNone ? parameters.iReasonCode : ret;
	}


// SetQoS response
void CPdpContext::SetQoSReply(CRequestBase* aRequest, const TContextParameters& aParams)
	{
	LOG(Log::Printf(_L("CPdpContext::SetQoSReply [ContextId=%d] [ReasonCode=%d]"),iContextId,aParams.iReasonCode));

	// Note: Because Run may complete the request, and resulting upcall
	// to QoS may delete anything within the GUQOS, don't assume "this"
	// exists after Run! Any actions on this must be done before the
	// Run call.
	if (aRequest)
		aRequest->Run(EPendingSetQoS, this, aParams);
	}

TInt CPdpContext::ModifyTft(const TTFTOperationCode& aTFTOperationCode, const TTFTInfo& aTft)
	{
	LOG(Log::Printf(_L("\t\tModifyTFT on PDP Context [ContextId=%d]"),iContextId));

	TPckg<TContextParameters> options(Nif().ContextParameters());
	options().iContextConfig.SetTFTInfo(aTft);
	options().iContextInfo.iContextId = iContextId;
	options().iContextType = iContextType;
	options().iTFTOperationCode = aTFTOperationCode;

	const TInt ret = Nif().Interface().Control(KSOLInterface, KContextTFTModify, options);
	return ret == KErrNone ? options().iReasonCode : ret;
	}

void CPdpContext::ModifyTftReply(CRequestBase* aRequest, const TContextParameters& aParams)
	{
	LOG(Log::Printf(_L("CPdpContext::ModifyTftReply [ContextId=%d] [ReasonCode=%d]"),iContextId,aParams.iReasonCode));
	if (!aRequest)
		return;

	// Translate operation code into "request code"...
	TRequest code = EPendingAny;;
	switch(aParams.iTFTOperationCode)
		{
	case KAddFilters:
		code = EPendingPacketFilterAdd;
		break;

	case KRemoveFilters:
		code = EPendingPacketFilterRemove;
		break;

	case KDeleteTFT:
		code = EPendingTFTRemove;
		break;

	case KAddSblpParameter:
		code = EPendingSblpParameterAdd;
		break;
			
	case KRemoveSblpParameter:
		code = EPendingSblpParameterRemove;
		break;

	default:
		// Should not happen, but if it does, let the request state
		// machine handle the problem (code is now EPendingAny!)
		break;
		}
	// Note: Because Run may complete the request, and resulting upcall
	// to QoS may delete anything within the GUQOS, don't assume "this"
	// exists after Run! Any actions on this must be done before the
	// Run call.
	aRequest->Run(code, this, aParams);
	}


// Activate context
TInt CPdpContext::Activate()
	{
	LOG(Log::Printf(_L("\t\tCPdpContext::Activate PDP Context [ContextId=%d]"),iContextId));

	TContextParameters& parameters(Nif().ContextParameters());

	// Activate only needs the context id.
	parameters.iContextInfo.iContextId = iContextId;
	//?? Does it need the "context type"?
	parameters.iContextType = iContextType;
	TPckg<TContextParameters> options(parameters);
	const TInt ret =  Nif().Interface().Control(KSOLInterface, KContextActivate, options);
	return ret == KErrNone ? parameters.iReasonCode : ret;
	}

// Activate context response
void CPdpContext::ActivateReply(CRequestBase* aRequest, const TContextParameters& aParams)
	{
	LOG(Log::Printf(_L("CPdpContext::ActivateReply() [ContextId=%d]"),iContextId));

	Update(aParams);
	// Must be done before Run (see below)!
	if (ContextActive())
		UnBlock();
	// Note: Because Run may complete the request, and resulting upcall
	// to QoS may delete anything within the GUQOS, don't assume "this"
	// exists after Run! Any actions on this must be done before the
	// Run call.
	if (aRequest)
		{
		aRequest->Run(EPendingActivate, this, aParams);
		}
	}

// Modify active Pdp context
TInt CPdpContext::ModifyActive()
	{
	LOG(Log::Printf(_L("\t\tCPdpContext::ModifyActive [ContextId=%d]"),iContextId));

	TContextParameters& parameters(Nif().ContextParameters());
	parameters.iContextInfo.iContextId = iContextId;
	TPckg<TContextParameters> options(parameters);

	const TInt ret(Nif().Interface().Control(KSOLInterface, KContextModifyActive, options));
	return ret == KErrNone ? parameters.iReasonCode : ret;
	}

void CPdpContext::ModifyActiveReply(CRequestBase* aRequest, const TContextParameters& aParams)
	{
	LOG(Log::Printf(_L("CPdpContext::ModifyActiveReply [ContextId=%d] [ReasonCode=%d]"),iContextId,aParams.iReasonCode));
	Update(aParams);

	// Must be done before Run (see below)!
	if (ContextActive())
		UnBlock();
	// Note: Because Run may complete the request, and resulting upcall
	// to QoS may delete anything within the GUQOS, don't assume "this"
	// exists after Run! Any actions on this must be done before the
	// Run call.
	if (aRequest)
		aRequest->Run(EPendingModifyActive, this, aParams);
	}

// QoS parameters have changed
void CPdpContext::ParametersChangedEvent(const TContextParameters& aParams)
	{
	LOG(Log::Printf(_L("CPdpContext::ParametersChangedEvent [ContextId=%d] [ReasonCode=%d]"),iContextId,aParams.iReasonCode));
	Update(aParams);
	// Notify applications about QoS change
	GenerateEvent(KPfqosEventAdapt, aParams);
	}

void CPdpContext::DeleteEvent(const TContextParameters& aParams)
	{
	LOG(Log::Printf(_L("CPdpContext::DeleteEvent [ContextId=%d] [ReasonCode=%d]"),iContextId,aParams.iReasonCode));
	GenerateEvent(KPfqosEventFailure, aParams);
	}

// delete Pdp context
TInt CPdpContext::Delete()
	{
	if (!IsPrimary())
		{
		LOG(Log::Printf(_L("\t\tCPdpContext::Delete [ContextId=%d]"),iContextId));

		TContextParameters& parameters(Nif().ContextParameters());
		parameters.iContextInfo.iContextId = iContextId;
		TPckg<TContextParameters> options(parameters);
		Nif().Interface().Control(KSOLInterface, KContextDelete, options);
		}
	return KErrNone;
	}


void CPdpContext::FlowAttached()
	{
	++iRefs;	
	}

void CPdpContext::FlowDetached()
	{
	--iRefs;
	if (!IsPrimary() && iFlows.IsEmpty())
		{
		ASSERT(iRefs == 0);
		iTimeout.Set(Nif().Module().TimeoutManager(), Nif().Module().Options().iTimeout);
		}
	}

void CPdpContext::GenerateEvent(TUint aEventType, const TContextParameters& aParams)
	{
	// Generate event for channels
	MEventInterface *notifier = Nif().Module().QoSEvent();
	if (!notifier)
		return;	// Nobody to notify!

	RExtensionData extension;
	TInt ret = extension.CreateExtension(aParams.iContextConfig, aParams.iReasonCode);
	// if cannot generate full return information, try with plain reason code
	if (ret != KErrNone)
		(void)extension.SetErrorCode(aParams.iReasonCode);
	
	if (ChannelId())
		{
		// The PDP context represents a channel, one event for all joined flows
		notifier->NotifyEvent(ChannelId(), aEventType, NULL, extension);
		}
	else
		{
		// Generate event for each flow
		TDblFlowIter iter(iFlows);
		CFlowData *flow;
		while ((flow = iter++) != NULL)
			{
			// Throw away "const". The MEventInterface is misdeclared to
			// use non-const reference parameter, when const reference
			// would be the correct way...
			notifier->NotifyEvent((CFlowContext &)flow->FlowContext(), aEventType, NULL, extension);
			}
		}
	extension.Close();
	}


void CPdpContext::SetContextStatus(const RPacketContext::TContextStatus& aStatus)
	{
#ifdef _LOG
	TBuf<40> statusBefore;

	switch(iContextStatus)
		{
		case RPacketContext::EStatusUnknown:		statusBefore.Append(_L("EStatusUnknown"));		break;
		case RPacketContext::EStatusInactive:		statusBefore.Append(_L("EStatusInactive"));		break;
		case RPacketContext::EStatusActivating:		statusBefore.Append(_L("EStatusActivating"));	break;
		case RPacketContext::EStatusActive:			statusBefore.Append(_L("EStatusActive"));		break;
		case RPacketContext::EStatusDeactivating:	statusBefore.Append(_L("EStatusDeactivating"));	break;
		case RPacketContext::EStatusSuspended:		statusBefore.Append(_L("EStatusSuspended"));	break;
		case RPacketContext::EStatusDeleted:		statusBefore.Append(_L("EStatusDeleted"));		break;
		default:									statusBefore.Append(_L("error"));				break;
		}

	TBuf<40> statusAfter;

	switch(aStatus)
		{
		case RPacketContext::EStatusUnknown:		statusAfter.Append(_L("EStatusUnknown"));		break;
		case RPacketContext::EStatusInactive:		statusAfter.Append(_L("EStatusInactive"));		break;
		case RPacketContext::EStatusActivating:		statusAfter.Append(_L("EStatusActivating"));	break;
		case RPacketContext::EStatusActive:			statusAfter.Append(_L("EStatusActive"));		break;
		case RPacketContext::EStatusDeactivating:	statusAfter.Append(_L("EStatusDeactivating"));	break;
		case RPacketContext::EStatusSuspended:		statusAfter.Append(_L("EStatusSuspended"));		break;
		case RPacketContext::EStatusDeleted:		statusAfter.Append(_L("EStatusDeleted"));		break;
		default:									statusAfter.Append(_L("error"));				break;
		}

	LOG(Log::Printf(_L("\tCPdpContext::SetContextStatus, iContextStatus before: {%S}, after: {%S}"), &statusBefore, &statusAfter));
#endif

	const TUint blocked = IsFlowBlocked();
	iContextStatus = aStatus;
	NotifyBlockingState(blocked);
	}

void CPdpContext::Update(const TContextParameters& aParams)
	{
	LOG(Log::Printf(_L("\tUpdate Context id = %d (channel=%d)"), ContextId(), ChannelId()));
	// Save some basic negotiated values to the context instance.
	aParams.iContextConfig.GetUMTSQoSNeg(iNegotiated);

	// *NOTE* *ASSUMED*
	// This function is called on completion of MofifyActive/Activate and
	// in some other cases. Assume that the TContextParameters contains the
	// current state of the context as seen by lower layers. The iReasonCode
	// only indicates that the indicated operation failed or succeeded.
	// The current state is provided even if iReasonCode != KErrNone.

	LOG(Log::Printf(_L("\tCurrent TFT from NIF to PDP ContextId=%d"), ContextId()));
	//?? The specification of TTFTInfo class is plain idiotic. There is no way to
	//?? scan through filters with constant object. There is no way to get a
	//?? const ref to filter inside TFTInfo, instead all scanning always requires
	//?? inconvenient copying of the element (in addition to the fact that,
	//?? you often have to copy the whole TTFTInfo structure to local store
	//?? for a "modifiable copy" (which really should not be needed!)
	
	// Avoid some of the problems by *not* keeping TTFTInfo in CPdpContext, and
	// maintain own simple array of TPacketFilters instead, which can at least
	// be looked without needing to copy the elements each time...
	TTFTInfo& tft = ((RContextConfig&)aParams.iContextConfig).TFTInfo();
	tft.SetToFirst();
	iNumFilters = 0;
	while (iNumFilters < KMaxPacketFilters && tft.NextPacketFilter(iFilters[iNumFilters]) == KErrNone)
		{
		LOG(LogPacketFilter(iFilters[iNumFilters]));
		++iNumFilters;
		}
	Nif().RecomputeEvaluationPrecedences();

#ifdef _LOG
	RPacketContext::CTFTMediaAuthorizationV3 *sblpParams;
	TRAPD(err, sblpParams = RPacketContext::CTFTMediaAuthorizationV3::NewL());
	if (err != KErrNone)
		return;	// Blah!!
	if (tft.GetSblpToken(*sblpParams) == KErrNone)
		{
		TBuf<KMaxFQDNLength> mat;
		mat.Copy(sblpParams->iAuthorizationToken);
		//coverity[leave_without_push]
		Log::Printf(_L("\t\tMedia authorization: '%S'"),&mat);
		for( TInt i = 0; i < sblpParams->iFlowIds.Count(); i++ )
			{
			//coverity[leave_without_push]
			Log::Printf(_L("\t\tFlowId %d.  Media <%d> Flow <%d>"),
				i,
				sblpParams->iFlowIds[i].iMediaComponentNumber,
				sblpParams->iFlowIds[i].iIPFlowNumber);
			}
		}
	else
		//coverity[leave_without_push]
		Log::Printf(_L("\t\tNo SBLP"));
	delete sblpParams;
#endif
	// Do the status change last, because it may generate events
	// and callbacks. Now all the context state is updated...
	SetContextStatus(aParams.iContextInfo.iStatus);
	}

// Rename: IsDefaultContext()
TBool CPdpContext::IsPrimary() const
	{
	return (iNifItem.DefaultPdpContext() == this);
	}

