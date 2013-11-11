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

#include "iface.h"
#include "context.h"
#include "tc.h"
#include <networking/qoserr.h>
#include "guqos_err.h"
#include "async_request.h"
#include "guqos_log.h"



// Open Channel Machine
const SActionStep<COpenChannel> COpenChannel::iActionList[] =
	{
		&COpenChannel::ActionRemovePacketFilter,	// iState=0
		&COpenChannel::ActionCommit,				// iState=1
		&COpenChannel::ActionNewContext,			// iState=2
		&COpenChannel::DoRememberCreatedContext,	// iState=3
		&COpenChannel::ActionAddPacketFilter,		// iState=4
		&COpenChannel::ActionAddSblpParameter,		// iState=5 == KStateReuseOldContext!
		&COpenChannel::ActionSetQoS,
		&COpenChannel::ActionCommit,
		&COpenChannel::DoAddFlowToContext,
		&COpenChannel::ActionNegotiationComplete
	};
const TInt KStateReuseOldContext = 5;	// *KEEP THIS IN SYNCH WITH ABOVE!*



// Negotiate Channel Machine
const SActionStep<CNegotiateChannel> CNegotiateChannel::iActionList[] =
	{
		&CNegotiateChannel::ActionAddSblpParameter,
		&CNegotiateChannel::ActionSetQoS,
		&CNegotiateChannel::ActionCommit,
		&CNegotiateChannel::ActionNegotiationComplete,
	};

// Join Channel Machine
const SActionStep<CJoinRequest> CJoinRequest::iActionList[] =
	{
		&CJoinRequest::ActionRemovePacketFilter,
		&CJoinRequest::ActionCommit,
		&CJoinRequest::DoStartWithTargetContext,
		&CJoinRequest::ActionAddPacketFilter,
		&CJoinRequest::DoAddFlowToContext,
		&CJoinRequest::ActionRemovePacketFilter,	// Cleanup extra filters away, if present
		&CJoinRequest::ActionCommit,
		&CJoinRequest::ActionRequestComplete
	};

// Leave Channel Machine
const SActionStep<CLeaveRequest> CLeaveRequest::iActionList[] =
	{
		&CLeaveRequest::ActionRemovePacketFilter,
		&CLeaveRequest::ActionCommit,
		&CLeaveRequest::ActionRequestComplete
	};

// Update Channel Machine (misnamed as "Close")
const SActionStep<CClose> CClose::iActionList[] =
	{
		&CClose::ActionRemovePacketFilter,
		&CClose::ActionCommit,
		&CClose::ActionRequestComplete
	};


#ifdef _LOG
//
// Logging help classes
//

class TLogContextInfo : public TBuf<100>
	{
public:
	TLogContextInfo(const CPdpContext* aContext);	
	};

TLogContextInfo::TLogContextInfo(const CPdpContext* aContext)
	{
	if (aContext)
		{
		Format(_L("context[%u] id=%d"), (TInt)aContext, aContext->ContextId()); // + 32
		switch (aContext->ContextType())
			{
			case EPrimaryContext:	Append(_L(" primary")); break;
			case ESecondaryContext:	Append(_L(" secondary")); break;			// + 10
			case ENetworkInitiatedContext: Append(_L(" network")); break;
			default: Append(_L(" unknown")); break;
			}
		switch (aContext->ContextStatus())
			{
			case RPacketContext::EStatusInactive: Append(_L(" inactive")); break;
			case RPacketContext::EStatusActivating: Append(_L(" activating")); break;
			case RPacketContext::EStatusActive: Append(_L(" active")); break;
			case RPacketContext::EStatusDeactivating: Append(_L(" deactivating")); break;		// + 12
			case RPacketContext::EStatusSuspended: Append(_L(" suspended")); break;
			case RPacketContext::EStatusDeleted: Append(_L(" deleted")); break;
			default: Append(_L(" unknown")); break;			
			}
		AppendFormat(_L("(%d)"), (TInt)aContext->ContextStatus());				// + 4

		if (aContext->ChannelId())
			{
			AppendFormat(_L(" channel=%d"), aContext->ChannelId());				// + 18
			}
		AppendFormat(_L(", %d flows"), aContext->RefCount());					// + 11;
		}
	else
		{
		Append(_L("context NONE"));
		}
	}

#endif


CRequestBase::CRequestBase(CNif& aNif) : iNif(aNif)
	{
#ifdef _LOG
	// This is only fallback, and should be overriden by the
	//  derived classes!
	_LIT(KLogName, "Base");
	iName = &KLogName();
#endif
	}

CRequestBase::~CRequestBase()
	{
	LOG(Log::Printf(_L("~\trequest %S[%u] destructed"), iName, (TInt)this));
	}

void CRequestBase::SetContext(CPdpContext *aContext)
	/**
	* Set/clear the current context for the state machine.
	*/
	{
	LOG(TLogContextInfo info(aContext));
	LOG(Log::Printf(_L("\trequest %S[%u] -- %S set"), iName, (TInt)this, &info));
	if (iContext != aContext)
		{
		iContext = aContext;
		// Context has been changed, need to clear modified flag.
		iContextModified = EFalse;	
		}
	iParams = NULL;	// Paremeters can only come from NIF event.
	}

void CRequestBase::SetExpected(const TRequest aRequest)
	{
	/**
	* Define the expected result of the next asynchronous operation.
	*/
	iExpected = aRequest;
	// Assume that SetExpected is only used when a request to NIF
	// has been passed. Thus, any call to SetExpected implies that
	// the context will be modified by NIF.
	iContextModified = ETrue;
	}
	
	
static TBool CheckSblpErrorInPcoBuffer(const TContextParameters& aParams, TUint8& parsedSblpErrorValue)
	{
	TBool retValue = EFalse;
	TInt ret = KErrNone;
	
	// create the PCO id for rejection code
	RPacketContext::TPcoId sblpRejectionCodePcoId(
				RPacketContext::EEtelPcktPolicyControlRejectionCode); 
	/*
	 *  get the value of the RPacketContext::TContextConfigGPRS 
	 *  object from the TContextParameters object passed by the nif
	 */
	RPacketContext::TContextConfigGPRS configGPRS;
	aParams.iContextConfig.GetContextConfig(configGPRS);

	// adjust the PCO buffer 
	TPtr8 pcoBufferPtr(const_cast<TUint8*>
				(configGPRS.iProtocolConfigOption.iMiscBuffer.Ptr()),
				configGPRS.iProtocolConfigOption.iMiscBuffer.Length(),
				configGPRS.iProtocolConfigOption.iMiscBuffer.Length()); 

	// attach TTlv  to the buffer 
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> 
												pcoTLV(pcoBufferPtr,pcoBufferPtr.Length());		 
	
	// Retreive the sblp rejection code value from the buffer
	TBuf8<253> sblpRejectionCodeBuf; 
	sblpRejectionCodeBuf.SetLength(pcoBufferPtr.Length());		 
	TPtr8 sblpRejectionCodePtr(const_cast<TUint8*>(sblpRejectionCodeBuf.Ptr()),
						sblpRejectionCodeBuf.Length(),sblpRejectionCodeBuf.MaxLength()); 
	
	pcoTLV.ResetCursorPos(); 
	TRAPD(err1, ret=pcoTLV.NextItemL(sblpRejectionCodePcoId,sblpRejectionCodePtr) ); 
	if (err1 !=KErrNone || ret!=KErrNone)
		{
		return EFalse;
		}

	// sblpRejectionCodeBuf[0] shall contain SBLP Rejection code ( values 1-7)) 
	parsedSblpErrorValue = sblpRejectionCodePtr[0];
	if(parsedSblpErrorValue >= ESblpSubErrorValueMinimum && 
		parsedSblpErrorValue <= ESblpSubErrorValueMaximum)
		{
		retValue = ETrue;
		}
	return retValue;
	}


void CRequestBase::Run(const TRequest aRequest, CPdpContext* aContext, const TContextParameters& aParams)
	{
	LOG(const TDesC* const name(iName)); // because request can get destroyed from under us, save the name here!
	LOG(TLogContextInfo info(aContext));
	LOG(Log::Printf(_L("Run\trequest %S[%u] Actions on event=%d reason=%d expected event=%d on %S"),
		name, (TInt)this, (TInt)aRequest, aParams.iReasonCode, (TInt)iExpected, &info));

	iContext = aContext;
	iParams = &aParams;

	if (iExpected != aRequest && iExpected != EPendingAny)
		{
		CompleteAndDestruct(KErrCancel, NULL);
		}
	else if (aParams.iReasonCode == KErrNone)
		{
		// Reset to accepting any, actions must use SetExpected,
		// when they activate asynchronous operation with
		// a specific responce.
		iExpected = EPendingAny;
		do
			{
			++iState;	// Last state done OK, try next one.
			}
		while (!Action(iContext, iParams));
		// Note: there is no special terminating test, it is excepted that
		// the builder of action list guarantees that it always ends with
		// action which unconditionally returns ETrue.
		}
	else
		{
		// The expected request has failed.
		TInt reason = aParams.iReasonCode;

		iExtension.SetUmtsType();

		/*
		 *  if there is KErrGprsUserAuthenticationFailure error code returned by 
		 *  the etel via nif, then we must check for the possible specific sblp 
		 *  errors in the Protocol Configuration Option PCO buffer.
		 *  
		 *  If the buffer has a value for the 
		 *  TPcoIDNetworkToMS::EEtelPcktPolicyControlRejectionCode id, then we can
		 *  decide that sblp specific error has been occured if one of the the 
		 *  possible sub error codes (totally six!) matches.
		 */
		
		if (reason == KErrGprsUserAuthenticationFailure)
			{
			TUint8 parsedSblpErrorValue = KErrNone;
			/*
			 *  check if PCO buffer has sblp rejection code error value
			 *  calling CheckSblpErrorInPcoBuffer() returns true only
			 *  if there is error in the buffer and if the error range 
			 *  is between (1-7)
			 */
			if (CheckSblpErrorInPcoBuffer(aParams,parsedSblpErrorValue))
				{
				/*
				 *  it is definitely a sblp error
				 *  Set the sblp error flag
				 */
		 		iExtension.SetSblpType();

				// figure out the sub error type
				switch (parsedSblpErrorValue)
					{
				case 1:
					// Use the sblp error code to create Failure()
					reason = EGuqosSblpAuthorizationFailureOfRequest;
					break;
				case 2:
					reason = EGuqosSblpMissingBindingInfo;
					break;
				case 3:
					reason = EGuqosSblpInvalidBindingInfo;
					break;
				case 4:
					reason = EGuqosSblpBindingInfoNotAllowed;
					break;
				case 5:
					reason = EGuqosSblpAuthorizingEntityTemporarilyUnavailable;
					break;
				case 6:
					reason = EGuqosSblpNoCorrespondingSession;
					break;
				case 7:
					reason = EGuqosSblpInvalidBundling;
					break;
				default:
					/*
					 *  it will never reach here!
					 *  because the range checking is already done in
					 *  CheckSblpErrorInPcoBuffer(aParams,parsedSblpErrorValue) 
					 */
					LOG(Log::Printf(_L("\trequest%S[%u] SBLP error value range fault!"), name, (TInt)this));
					break;
					}
				}
			}
		// Deliver failure to the actual request implementation.
		Failure(aContext, reason);
		}
	LOG(Log::Printf(_L("\trequest %S[%u] Run Actions Exit"), name, (TInt)this));
	}


void CRequestBase::Start()
	{
	LOG(const TDesC* const name(iName)); // because request can get destroyed from under us, save the name here!
	LOG(TLogContextInfo info(iContext));
	LOG(Log::Printf(_L("\trequest %S[%u] Start Actions with %S"), name, (TInt)this, &info));
	// Rely on C-class 0-fill. Initially all state variables
	// are zero, e.g.
	//   iContext = NULL;
	//   iParams = NULL;
	//   iExpected = EPendingAny;
	//   iState = 0
	// This start does not touch those values and allows derived
	// class to override this function with something that may initialize
	// state, before calling the base implementation of the Start.

	// Excecute actions until something catches or request terminates.
	while (!Action(iContext, iParams))
		{
		++iState;
		}
	// Note: there is no special terminating test, it is excepted that
	// the builder of action list guarantees that it always ends with
	// action which unconditionally returns ETrue.
	LOG(Log::Printf(_L("\trequest %S[%u] Start Actions Exit"), name, (TInt)this));
	}

void CRequestBase::CompleteAndDestruct(TInt aErrorCode, const TQoSParameters* aParams, const TExtensionData& aExtension)
	{
	LOG(Log::Printf(_L("\trequest %S[%u] CompleteAndDestruct ErrorCode=%d"), iName, (TInt)this, aErrorCode));
	// Remove all references to this request...
	iNif.CloseRequest(this);
	// ...before delivering the completion. The delivery may call
	// back, and this request must be totally removed by then, or
	// bad things can happen (like, duplicate delete).
	if (iNotify)
		iNotify->RequestComplete(aErrorCode, aParams, aExtension);
	delete this;
	}


TBool CRequestBase::IsOk(TInt aResult, CPdpContext *aContext)
	{
	if (aResult >= 0)
		return ETrue;
	LOG(TLogContextInfo info(aContext));
	LOG(Log::Printf(_L("\trequest %S[%u] IsOk(%d) is not OK %S"), iName, (TInt)this, aResult, &info));
	Failure(aContext, aResult);
	return EFalse;
	}

TBool CRequestBase::ActionRemovePacketFilter(CPdpContext* aContext, const TContextParameters*)
	/**
	* Activate leaving flow from a context.
	*/
	{
	LOG(TLogContextInfo info(aContext));
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionRemovePacketFilter from %S"), iName, (TInt)this, iState, &info));
	if (aContext)
		{
		// This branch is needed for the primary too, but only in rare case
		// where a secondary is being changed into a primary.
		if (iFlow)
			{
			// Move the flow to default context. If remove
			// fails, it is higly likely that this flow was
			// not included in TFT.
			iFlow->SetContext(iNif.DefaultPdpContext());
			}
		if (aContext->RefCount() > 0 || aContext->IsPrimary())
			{
			// Context has remaining flows or is a Primary, collect unused filters.
			// (in case of primary, all filters will be unused, if any present).
			TTFTInfo tft;
			TInt ret = aContext->RemovePacketFilter(tft, iFlow);
			if (ret > 0)
				{
				LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionRemovePacketFilter -- removing %d filter(s)"), iName, (TInt)this, iState, ret));
				SetExpected(EPendingPacketFilterRemove);
				(void)IsOk(aContext->ModifyTft(KRemoveFilters, tft), aContext);
				return ETrue;
				}
			else if (ret < 0)
				{
				LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionRemovePacketFilter -- TFT construct failed with %d"), iName, (TInt)this, iState, ret));
				CompleteAndDestruct(ret, NULL);
				return ETrue;
				}
			}
		else
			{
			// All flows have been removed from the context. In the current implementation
			// the context has been scheduled for destruction, and there is no need for
			// removing any existing TFT...
			LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionRemovePacketFilter -- No flows left"), iName, (TInt)this, iState));
			}
		}
	// No TFT changes need to be done,
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionRemovePacketFilter -- no filters to remove"), iName, (TInt)this, iState));
	return EFalse;
	}
	

TBool CRequestBase::ActionAddPacketFilter(CPdpContext* aContext, const TContextParameters* /*aParams*/)
	{
	LOG(TLogContextInfo info(aContext));
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionAddPacketFilter on %S"), iName, (TInt)this, iState, &info));
	if (aContext && iFlow)
		{
		TTFTInfo tft;
		TInt err = aContext->AddPacketFilter(*iFlow, tft);
		if (err < 0)
			{
			LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionAddPacketFilter -- TFT construct failed with %d"), iName, (TInt)this, iState, err));
			Failure(aContext, err);
			return ETrue;
			}
		else if (err > 0)
			{
			LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionAddPacketFilter -- adding %d filter(s)"), iName, (TInt)this, iState, err));
			SetExpected(EPendingPacketFilterAdd);
			(void)IsOk(aContext->ModifyTft(KAddFilters, tft), aContext);
			return ETrue;
			}
		// Filter does not need modification, start some other operation...
		}
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionAddPacketFilter -- no filters added"), iName, (TInt)this, iState));
	return EFalse;
	}
	
TBool CRequestBase::ActionCommit(CPdpContext* aContext, const TContextParameters* /*aParams*/)
	/**
	* Activate changes made to a context.
	*
	* This action can be run after a sequence of context modifying
	* actions (filter, QoS, etc.). Depending on the state of the
	* context, this calls either ModifyActive or Activate.
	*/
	{
	if (aContext)
		{
		LOG(TLogContextInfo info(aContext));

		if (!aContext->ContextActive())
			{
			LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionCommit -- start Activate %S"), iName, (TInt)this, iState, &info));
			SetExpected(EPendingActivate);
			(void)IsOk(aContext->Activate(), aContext);
			}
		else if (iContextModified)
			{
			LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionCommit -- start ModifyActive %S"), iName, (TInt)this, iState, &info));
			SetExpected(EPendingModifyActive);
			(void)IsOk(aContext->ModifyActive(), aContext);
			}
		else
			{
			// For example all flows go into same tunnel and filter is same for everyone.
			// A flow leaving context does not affect the filter.
			LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionCommit -- skip ModifyActive, not modified %S"), iName, (TInt)this, iState, &info));
			return EFalse;
			}
		return ETrue;
		}
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionCommit -- nothing to do, no context"), iName, (TInt)this, iState));
	return EFalse;
	}


TBool CRequestBase::ActionRequestComplete(CPdpContext* /*aContext*/, const TContextParameters* /*aParams*/)
	{
	/**
	* Default termination of the request.
	*
	* This action can be used as a default terminator of the request,
	*/
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionRequestComplete"), iName, (TInt)this, iState));
	CompleteAndDestruct(KErrNone, NULL);
	return ETrue;
	}

TBool CRequestBase::DoAddFlowToContext(CPdpContext* aContext, const TContextParameters*)
	/**
	* Set add current flow to current context.
	*
	* Add current flow to current context internally (e.g. add CFlowData object to
	* CPdpContext object).
	*/
	{
	LOG(TLogContextInfo info(aContext));
	if (iFlow && aContext)
		{
		LOG(Log::Printf(_L("\trequest %S[%u] %2d.DoAddFlowToContext to %S"), iName, (TInt)this, iState, &info));
		iFlow->SetContext(aContext);
		}
	else
		{
		LOG(Log::Printf(_L("\trequest %S[%u] %2d.DoAddFlowToContext to %S [not done]"), iName, (TInt)this, iState, &info));
		}
	// This allows the use ActionRemovePacketFilter to be used after this
	// to clean out unnecessary filters. This is required in Join, because
	// when the last flow leaves, the filters are not updated (cannot be
	// removed because PDP Context cannot exist without filters, and just
	// deleting the context would be too drastic). Thus, when the same flow
	// or new flow joins back to existing context, we need to check and
	// clean out stale filters.
	iFlow = NULL;
	return EFalse;
	}

void CRequestBase::Cancel(CFlowData* aFlowData)
	/**
	* Default cancel based on flow.
	*
	* If the only CFlowData pointer in request is stored in iFlow,
	* then this default Cancel should be enough for most cases.
	*/
	{
	if (iFlow == aFlowData)
		{
		LOG(Log::Printf(_L("\trequest %S[%u] Cancel due flow[%u] -- only detach the flow"), iName, (TInt)this, (TInt)iFlow));
		// A running request is hard to cancel, because there is no way
		// to tell NIF about it. Thus, when flow is cancelled, just detach
		// it from the request and let the state machine run to completion
		// without the flow (the machine should adapt!)
		iFlow = NULL;
		}
	}

void CRequestBase::Failure(CPdpContext* aContext, TInt aErrorCode)
	/**
	* The default Failure handler.
	*
	* Load the error code into iExtension and destruct.
	*/
	{
	LOG(TLogContextInfo info(aContext));
	(void)aContext; // silence warning in MARM release compile.
	TInt ret = iExtension.SetErrorCode(aErrorCode);
	LOG(Log::Printf(_L("\trequest %S[%u] Failure %S errorcode=%d ret=%d"), iName, (TInt)this, &info, aErrorCode, ret));
	CompleteAndDestruct(ret == KErrNone ? aErrorCode : ret, NULL);
	}

// CNegotiationBase
CNegotiationBase::CNegotiationBase(CNif& aNif) : CRequestBase(aNif)
	{
#ifdef _LOG
	// This is only fallback, and should be ovvrriden by the
	//  derived classes!
	_LIT(KLogName, "Negotiation");
	iName = &KLogName();
#endif
	}

void CNegotiationBase::SetParameters(const TQoSParameters& aParams, CExtensionPolicy& aPolicy)
	{
	LOG(Log::Printf(_L("\trequest %S[%u] SetParamaters"), iName, (TInt)this));
	iGeneric = aParams;
	iUmts.ParsePolicyData(&aPolicy);
	}

void CNegotiationBase::SetParametersFlowExtn(CExtensionPolicy &aPolicy)
	{
	// ParsePolicyData will return true if SBLP presents
	if (iFlowExtnParams.ParsePolicyDataForSblp(&aPolicy))
		{
		// SBLP presents
		// so, set the Sblp flag for the channel
	 	iSblp = ETrue;	
		LOG(Log::Printf(_L("\trequest %S[%u] SetParametersFlowExtn -- SBLP"), iName, (TInt)this));
		}
	}
	
// CNegotiationBase
TBool CNegotiationBase::ActionAddSblpParameter(CPdpContext *aContext, const TContextParameters* /*aParams*/)
	{
	/*
	 *  if SBLP is supported, sblp parameter is added after setting the 
	 *  Qos parameters
	 *  
	 *  once sblp parameter is added, the operations continue in the same
	 *  way as it was 
	 */
	if(aContext && iSblp) // the negotiations requests sblp context
		{
		// call modifytft to add sblp
		LOG(TLogContextInfo info(aContext));
		TTFTInfo tft;
		if (IsOk(tft.AddSblpToken(iFlowExtnParams.iAuthorizationToken, iFlowExtnParams.iFlowIds), aContext))
			{
			LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionAddSblpParameter -- Adding SBLP for %S"), iName, (TInt)this, iState, &info));
			SetExpected(EPendingSblpParameterAdd);
			(void)IsOk(aContext->ModifyTft(KAddSblpParameter, tft), aContext);
			}
		else
			{
			LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionAddSblpParameter -- Failed constructing TFT for SBLP for %S"),
				iName, (TInt)this, iState, &info));
			}
		return ETrue;
		}
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionAddSblpParameter -- no SBLP"), iName, (TInt)this, iState));
	return EFalse;
	}
	
TBool CNegotiationBase::ActionSetQoS(CPdpContext *aContext, const TContextParameters* /*aParams*/)
	{
	if (aContext)
		{
		LOG(TLogContextInfo info(aContext));
		LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionSetQoS -- request %S"), iName, (TInt)this, iState, &info));
		SetExpected(EPendingSetQoS);
		(void)IsOk(aContext->SetQoS(iGeneric, iUmts), aContext);
		return ETrue;		
		}
		LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionSetQoS -- skipping"), iName, (TInt)this, iState));
	return EFalse;
	}


TBool CNegotiationBase::ActionNegotiationComplete(CPdpContext *aContext, const TContextParameters* aParams)
	{
	LOG(TLogContextInfo info(aContext));
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionNegotiationComplete %S"), iName, (TInt)this, iState, &info));
	if (aParams)
		{
		// Return results of the negotiation to upper layers.
		TInt ret = iExtension.CreateExtension(aParams->iContextConfig, aParams->iReasonCode);
		if (ret != KErrNone)
			{
			LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionNegotiationComplete -- CreateExtension fail=%d"), iName, (TInt)this, iState, ret));
			// Ugh.. can't construct the return value (probably out of memory)
			// Try plain error instead (if that does not work, then nothing can
			// be done about it...)
			(void)iExtension.SetErrorCode(ret);
			}
		}

	if (iFlow && aContext)
		{
		// If there is assocated flow, assume it will need to be moved
		// to the current context (nothing happens, if this is already
		// true.
		iFlow->SetContext(aContext);
		}

	CompleteAndDestruct(KErrNone, &iGeneric, iExtension);
	return ETrue;
	}
	
void CNegotiationBase::Failure(CPdpContext* /*aPdpContext*/, TInt aErrorCode)
	{
	// If allocation space for extn fails, there not much
	// that can be done, the request must be completed anyway
	// with empty extn.
	LOG(Log::Printf(_L("\trequest %S[%u] Failure code=%d"), iName, (TInt)this, aErrorCode));
	(void)iExtension.SetErrorCode(aErrorCode);
	CompleteAndDestruct(aErrorCode, &iGeneric, iExtension);
	}

	
// CClose

CClose* CClose::New(CPdpContext& aContext)
	{
	return new CClose(aContext);
	}

CClose::CClose(CPdpContext& aContext) : CRequestBase(aContext.Nif()), iClosing(aContext)
	{
#ifdef _LOG
	_LIT(KLogName, "Close");
	iName = &KLogName();
#endif
	LOG(Log::Printf(_L("new\trequest %S[%u] size=%d"), iName, (TInt)this, sizeof(*this)));
	iNotify = NULL;
	}
	
void CClose::Start()
	{
	LOG(TLogContextInfo info(&iClosing));
	LOG(Log::Printf(_L("Start\trequest %S[%u] %S"), iName, (TInt)this, &info));
	// Give the context to the state machine.
	SetContext(&iClosing);
	// Note: iFlow is NULL. This means that the packet
	// filter is just recomputed based on currenly
	// joined flows.
	CRequestBase::Start();
	}

CClose::~CClose()
	{
	}

void CClose::Cancel(CPdpContext* aContext)
	{
	if (&iClosing == aContext)
		{
		LOG(TLogContextInfo info(aContext));
		LOG(Log::Printf(_L("Cancel\trequest %S[%u] for %S"), iName, (TInt)this, &info));
		Failure(aContext, KErrDied);
		}
	}


CDeleteRequest* CDeleteRequest::NewL(CPdpContext *aContext)
	{
	return new (ELeave) CDeleteRequest(aContext);
	}

CDeleteRequest::CDeleteRequest(CPdpContext* aContext) : CRequestBase(aContext->Nif()), iDelete(aContext)
	{
#ifdef _LOG
	_LIT(KLogName, "Delete");
	iName = &KLogName();
#endif
	LOG(Log::Printf(_L("new\trequest %S[%u] size=%d"), iName, (TInt)this, sizeof(*this)));
	iNotify = NULL;
	}
	
void CDeleteRequest::Start()
	{
	LOG(TLogContextInfo info(iDelete));
	LOG(Log::Printf(_L("Start\trequest %S[%u] %S"), iName, (TInt)this, &info));
	// Issue a context delete for this context to NIF
	if (iDelete)
		{
		CPdpContext* context = iDelete;
		iDelete = NULL;		// Prevent Cancel() from triggering...
		context->Delete();	// Send delete request to NIF
		context->Nif().DeletePdpContext(context);	// Destroys the object (.. this calls my Cancel!)
		}
	CompleteAndDestruct(KErrNone, NULL);
	}

void CDeleteRequest::Cancel(CPdpContext* aContext)
	{
	if (aContext == iDelete)
		{
		LOG(TLogContextInfo info(aContext));
		LOG(Log::Printf(_L("Cancel\trequest %S[%u] for %S"), iName, (TInt)this, &info));
		Failure(aContext, KErrDied);
		}
	}


// COpenChannel

COpenChannel* COpenChannel::New(TUint aChannelId, CFlowData &aFlow, MQoSNegotiateEvent* aNotify)
	{
	return new COpenChannel(aChannelId, aFlow, aNotify);
	}

COpenChannel::COpenChannel(TInt aChannelId, CFlowData& aFlow, MQoSNegotiateEvent* aNotify) :
	CNegotiationBase(aFlow.Nif()),
	iChannelId(aChannelId)
	{
#ifdef _LOG
	_LIT(KLogName, "Open");
	iName = &KLogName();
#endif
	LOG(Log::Printf(_L("new\trequest %S[%u] size=%d"), iName, (TInt)this, sizeof(*this)));
	iNotify = aNotify;
	iFlow = &aFlow;
	}

COpenChannel::~COpenChannel()
	{
	}

TBool COpenChannel::ActionNewContext(CPdpContext */*aContext*/, const TContextParameters* /*aParams*/)
	{
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.ActionNewContext"), iName, (TInt)this, iState));
	SetExpected(EPendingCreate);
	(void)IsOk(iNif.NewPdpContext(), NULL);
	return ETrue;
	}
	
TBool COpenChannel::DoRememberCreatedContext(CPdpContext* aContext, const TContextParameters*)
	{
	iNewContext = aContext;
	if (aContext)
		{
		// ..and bind it to the channel id.
		aContext->SetChannelId(iChannelId);
		LOG(TLogContextInfo info(aContext));
		LOG(Log::Printf(_L("\trequest %S[%u] %2d.DoRememberCreatedContext -- %S"), iName, (TInt)this, iState, &info));
		}
	else
		{
		LOG(Log::Printf(_L("\trequest %S[%u] %2d.DoRememberCreatedContext -- No context created for channel=%d"),
			iName, (TInt)this, iState, iChannelId));
		}
	return EFalse;
	}

void COpenChannel::Start()
	{
	LOG(Log::Printf(_L("Start\trequest %S[%u] -- COpenChannel Begin"), iName, (TInt)this));
	if (!iFlow)
		{
		LOG(Log::Printf(_L("\trequest %S[%u] Flow has been cancelled"), iName, (TInt)this));
		CompleteAndDestruct(KErrNone, NULL);
		return;
		}

	// First check if currently used Pdp context can be modified!!
	CPdpContext* context = iFlow->PdpContext();
	SetContext(context);
	if (context && context != iNif.DefaultPdpContext())
		{
		if (context->RefCount() == 1)
			{
			LOG(TLogContextInfo info(context));
			LOG(Log::Printf(_L("\trequest %S[%u] -- reuse existsing %S"), iName, (TInt)this, &info));
			// Start state machine at different point.
			iState = KStateReuseOldContext;
			}
		}
	// If not reusing old, the machine starts by trying
	// to remove the TFT from old context, if any...
	CRequestBase::Start();
	}


void COpenChannel::Failure(CPdpContext* aPdpContext, TInt aErrorCode)
	{
	LOG(Log::Printf(_L("\trequest %S[%u] Failure code=%d"), iName, (TInt)this, aErrorCode));

	if (aPdpContext && aPdpContext == iNewContext)
		{
		iNewContext = NULL;
		aPdpContext->Delete();
		CNif& nif = aPdpContext->Nif();
		nif.DeletePdpContext(aPdpContext);
		}
	CNegotiationBase::Failure(aPdpContext, aErrorCode);	
	}

void COpenChannel::Cancel(CPdpContext* aContext)
	{
	if (aContext == iNewContext)
		{
		// Prevent Failure from deleting the context (the caller of Cancel is
		// just doing that).
		iNewContext = NULL;
		LOG(TLogContextInfo info(aContext));
		LOG(Log::Printf(_L("Cancel\trequest %S[%u] Cancelled for "), iName, (TInt)this, &info));
		Failure(aContext, KErrDied);
		}
	}


// CNegotiateChannel

CNegotiateChannel* CNegotiateChannel::New(CPdpContext* aContext, MQoSNegotiateEvent* aNotify)
	{
	return new CNegotiateChannel(aContext, aNotify);
	}

CNegotiateChannel::CNegotiateChannel(CPdpContext *aContext, MQoSNegotiateEvent* aNotify) :
	CNegotiationBase(aContext->Nif())
	{
#ifdef _LOG
	_LIT(KLogName, "Negotiate");
	iName = &KLogName();
#endif
	LOG(Log::Printf(_L("new\trequest %S[%u] size=%d"), iName, (TInt)this, sizeof(*this)));
	iNotify = aNotify;
	iChannel = aContext;
	}

CNegotiateChannel::~CNegotiateChannel()
	{
	}


void CNegotiateChannel::Start()
	{
	LOG(Log::Printf(_L("Start\trequest %S[%u] -- CNegotiateChannel Begin"), iName, (TInt)this));
	SetContext(iChannel);
	CRequestBase::Start();
	}


void CNegotiateChannel::Cancel(CPdpContext* aContext)
	{
	if (iChannel == aContext)
		{
		LOG(TLogContextInfo info(aContext));
		LOG(Log::Printf(_L("Cancel\trequest %S[%u] Cancelled for %S"), iName, (TInt)this, &info));
		iChannel = NULL;
		
		//??Where does this go?
		Failure(aContext, KErrDied);
		}
	}



// CJoinRequest
CJoinRequest* CJoinRequest::New(CPdpContext* aContext, CFlowData* aFlowData, MQoSNegotiateEvent* aNotify)
	{
	return new CJoinRequest(aContext, aFlowData, aNotify);
	}

CJoinRequest::CJoinRequest(CPdpContext* aContext,  CFlowData* aFlowData, MQoSNegotiateEvent* aNotify) :
	CRequestBase(aFlowData->Nif())
	{
#ifdef _LOG
	_LIT(KLogName, "Join");
	iName = &KLogName();
#endif
	LOG(Log::Printf(_L("new\trequest %S[%u] size=%d"), iName, (TInt)this, sizeof(*this)));
	iNotify = aNotify;
	iChannel = aContext;
	iFlow = aFlowData;
	}

CJoinRequest::~CJoinRequest()
	{
	}

void CJoinRequest::Start()
	{
	LOG(Log::Printf(_L("Start\trequest %S[%u] -- CJoinRequest Begin"), iName, (TInt)this));
	if (!iFlow)
		{
		LOG(Log::Printf(_L("\trequest %S[%u] Flow has been cancelled"), iName, (TInt)this));
		CompleteAndDestruct(KErrNone, NULL);
		return;
		}
	CPdpContext* context = iFlow->PdpContext();
	LOG(TLogContextInfo info(context));
	if (context == iChannel)
		{
		// Already joined to the target context
		LOG(Log::Printf(_L("\trequest %S[%u] already joined to %S"), iName, (TInt)this, &info));
		CompleteAndDestruct(KErrNone, NULL, iExtension);
		return;
		}
	// Start with removal of the TFT from old context.
	SetContext(context);
	CRequestBase::Start();
	}


TBool CJoinRequest::DoStartWithTargetContext(CPdpContext */*aContext*/, const TContextParameters* /*aParams*/)
	{
	LOG(TLogContextInfo info(iChannel));
	LOG(Log::Printf(_L("\trequest %S[%u] %2d.DoStartWithTargetContext -- target %S"), iName, (TInt)this, iState, &info));
	SetContext(iChannel);
	return EFalse;
	}


void CJoinRequest::Failure(CPdpContext* /*aPdpContext*/, TInt aErrorCode)
	{
	LOG(Log::Printf(_L("\trequest %S[%u] Failure code=%d"), iName, (TInt)this, aErrorCode));
	iExtension.SetErrorCode(aErrorCode);
	CompleteAndDestruct(EQoSJoinFailure, NULL, iExtension);
	}

void CJoinRequest::Cancel(CPdpContext* aContext)
	{
	if (iChannel == aContext)
		{
		LOG(TLogContextInfo info(aContext));
		LOG(Log::Printf(_L("Cancel\trequest %S[%u] Cancelled for %S"), iName, (TInt)this, &info));
		Failure(aContext, KErrDied);
		}
	}

// CLeaveRequest
CLeaveRequest* CLeaveRequest::New(CPdpContext* aContext, CFlowData* aFlowData, MQoSNegotiateEvent* aNotify)
	{
	return new CLeaveRequest(aContext, aFlowData, aNotify);
	}

CLeaveRequest::CLeaveRequest(CPdpContext* aContext, CFlowData* aFlowData, MQoSNegotiateEvent* aNotify) :
	CRequestBase(aContext->Nif())
	{
#ifdef _LOG
	_LIT(KLogName, "Leave");
	iName = &KLogName();
#endif
	LOG(Log::Printf(_L("new\trequest %S[%u] size=%d"), iName, (TInt)this, sizeof(*this)));
	iNotify = aNotify;
	iChannel = aContext;
	iFlow = aFlowData;
	//?? Why channel? it should be same as flow currently joined?
	//?? Or just use channel and recompute, leave == close?
	}

void CLeaveRequest::Start()
	{
	LOG(Log::Printf(_L("Start\trequest %S[%u] -- CLeaveRequest Begin"), iName, (TInt)this));
	// Start with removal of the TFT from old context.
	// The flow has nothing to do with this anymore?
	SetContext(iChannel);
	CRequestBase::Start();
	}

CLeaveRequest::~CLeaveRequest()
	{
	LOG(Log::Printf(_L("CLeaveRequest::~CLeaveRequest()")));
	}


void CLeaveRequest::Failure(CPdpContext* /*aPdpContext*/, TInt aErrorCode)
	{
	LOG(Log::Printf(_L("\trequest %S[%u] Failure code=%d"), iName, (TInt)this, aErrorCode));
	iExtension.SetErrorCode(aErrorCode);
	CompleteAndDestruct(EQoSLeaveFailure, NULL, iExtension);
	}

void CLeaveRequest::Cancel(CPdpContext* aContext)
	{
	if (iChannel == aContext)
		{
		LOG(TLogContextInfo info(aContext));
		LOG(Log::Printf(_L("Cancel\trequest %S[%u] Cancelled for %S"), iName, (TInt)this, &info));
		Failure(aContext, KErrDied);
		}
	}
