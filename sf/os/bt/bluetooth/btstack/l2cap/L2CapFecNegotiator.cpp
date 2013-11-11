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
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "L2CapFecNegotiator.h"
#include "L2CapPDU.h"

static TBool operator<=(TL2CapChannelMode aLeft, TL2CapChannelMode aRight);
template<typename T> static inline TBool IsWithinBounds(const T aValue, const T aLeftBound, const T aRightBound);

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

//
// Mode-specific handlers
//

TBool TModeSpecificFecOptionHandlerBase::BuildPositiveResponse(TRetransmissionAndFlowControlOption& aPreferred,
															   const TRetransmissionAndFlowControlOption& aPeer) const
	{
	LOG_FUNC
	// As simple as that according to the general negotiation process, i.e. not when negotiating
	// the 2.1 Core Spec Addendum 1 - introduced modes.
	aPreferred = aPeer;
	// Don't include the exactly same FEC in response.
	return EFalse;
	}

void TModeSpecificFecOptionHandlerBase::BuildNegativeResponse(TRetransmissionAndFlowControlOption& /*aPreferred*/,
															  const TRetransmissionAndFlowControlOption& /*aPeer*/) const
	{
	LOG_FUNC
	// Just send what we've got in Preferred.
	}

void TModeSpecificFecOptionHandlerBase::BuildRequestBasedOnUnacceptableParamsResponse(
											TRetransmissionAndFlowControlOption& aPreferred,
											const TRetransmissionAndFlowControlOption& aPeer,
											const TL2CapFecNegotiator& aFecNegotiator) const
	{
	LOG_FUNC
	// In general the only negotiable parameter is channel mode - that's what we should
	// take from the suggested response passed here in aPeer. The rest of the parameters are
	// informative and we should use our own values, based on the mode - the remote can not
	// reject informative parameters.
	// Note on interop:
	// Unfortunately <= 9.4 Symbian code _WILL_ reject if e.g. we send a MaxTransmit value
	// which is greater than its own preference. To interoperate with those this method is
	// overridden in the Legacy handler.

	aPreferred = TRetransmissionAndFlowControlOption(aPeer.LinkMode(), ETrue);
	SetMaxTransmit(aPreferred, aFecNegotiator.MaxTransmit());
	ZeroUnspecifiedRequestFields(aPreferred);
	}

void TModeSpecificFecOptionHandlerBase::SetMaxTransmit(TRetransmissionAndFlowControlOption& aFecOption, TUint8 /*aMaxTransmit*/) const
	{
	LOG_FUNC
	aFecOption.SetMaxTransmit(0);
	}

void TModeSpecificFecOptionHandlerBase::PrepareImplicitPeerResponse(TRetransmissionAndFlowControlOption& aImplicitResponse,
																	const TRetransmissionAndFlowControlOption& aPreferred) const
	{
	LOG_FUNC
	aImplicitResponse = aPreferred;
	}

void TModeSpecificFecOptionHandlerBase::ZeroUnspecifiedRequestFields(TRetransmissionAndFlowControlOption& /*aFecOption*/) const
	{LOG_FUNC}

void TModeSpecificFecOptionHandlerBase::ZeroUnspecifiedResponseFields(TRetransmissionAndFlowControlOption& /*aFecOption*/) const
	{LOG_FUNC}


// Streaming Mode

TBool TStreamingFecHandler::IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const
	{
	LOG_FUNC
	return aFecOption.MaximumPDUSize() >= TRetransmissionAndFlowControlOption::KMinValidMaximumPDUSize;
	}

TBool TStreamingFecHandler::IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
													 const TRetransmissionAndFlowControlOption& aPeer) const
	{
	LOG_FUNC
	return aPeer.MaximumPDUSize() <= aPreferred.MaximumPDUSize();
	}

TBool TStreamingFecHandler::BuildPositiveResponse(TRetransmissionAndFlowControlOption& aPreferred,
												  const TRetransmissionAndFlowControlOption& aPeer) const
	{
	LOG_FUNC
	TRetransmissionAndFlowControlOption response = aPeer;
	// Trim MPS to our desired value.
	response.SetMaximumPDUSize(Min(TRetransmissionAndFlowControlOption::KDefaultMaximumPDUSize,
								   response.MaximumPDUSize()));
	// Zero the ignored fields no matter what peer sent us in them.
	ZeroUnspecifiedResponseFields(response);

	aPreferred = response;
	// Include the FEC in ConfigRsp.
	return ETrue;
	}

void TStreamingFecHandler::BuildNegativeResponse(TRetransmissionAndFlowControlOption& aPreferred,
                                                 const TRetransmissionAndFlowControlOption& /*aPeer*/) const
    {
    LOG_FUNC
    // Channel Mode has been already set and that's the parameter that what we're actually
    // rejecting. The rest is informational and should be set by the remote to its own
    // liking based on the mode proposed, so we _could_ set them to 0, but just in case
    // we're talking to a dumb peer - or a bit older version of my own code that wouldn't
    // accept 0s, for that matter - set them to sensible defaults.
    aPreferred = TRetransmissionAndFlowControlOption(aPreferred.LinkMode(), ETrue);
    ZeroUnspecifiedRequestFields(aPreferred);
    }

void TStreamingFecHandler::ZeroUnspecifiedRequestFields(TRetransmissionAndFlowControlOption& aFecOption) const
	{
	LOG_FUNC
	aFecOption.SetTxWindowSize(0);
	aFecOption.SetMaxTransmit(0);
	aFecOption.SetRetransmissionTimeout(0);
	aFecOption.SetMonitorTimeout(0);
	}

void TStreamingFecHandler::ZeroUnspecifiedResponseFields(TRetransmissionAndFlowControlOption& aFecOption) const
	{
	LOG_FUNC
	aFecOption.SetTxWindowSize(0);
	aFecOption.SetMaxTransmit(0);
	aFecOption.SetRetransmissionTimeout(0);
	aFecOption.SetMonitorTimeout(0);
	}


// Enhanced Retransmission Mode

TBool TErtmFecHandler::IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const
	{
	LOG_FUNC
	return aFecOption.MaximumPDUSize() >= TRetransmissionAndFlowControlOption::KMinValidMaximumPDUSize &&
		   aFecOption.TxWindowSize() >= TRetransmissionAndFlowControlOption::KMinValidTxWindowSize &&
		   aFecOption.TxWindowSize() <= TRetransmissionAndFlowControlOption::KMaxValidEnhancedTxWindowSize;
	}

TBool TErtmFecHandler::IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
												const TRetransmissionAndFlowControlOption& aPeer) const
	{
	LOG_FUNC
	return // MaxTransmit in response doesn't matter.
		   IsWithinBounds(aPeer.RetransmissionTimeout(),
				   		  TRetransmissionAndFlowControlOption::KMinAcceptableRetransmissionTimeout,
				   		  TRetransmissionAndFlowControlOption::KMaxAcceptableRetransmissionTimeout) &&
		   IsWithinBounds(aPeer.MonitorTimeout(),
				   		  TRetransmissionAndFlowControlOption::KMinAcceptableMonitorTimeout,
				   		  TRetransmissionAndFlowControlOption::KMaxAcceptableMonitorTimeout) &&
		   aPeer.TxWindowSize() <= aPreferred.TxWindowSize() &&
		   aPeer.MaximumPDUSize() <= aPreferred.MaximumPDUSize();
	}

TBool TErtmFecHandler::BuildPositiveResponse(TRetransmissionAndFlowControlOption& aPreferred,
											 const TRetransmissionAndFlowControlOption& aPeer) const
	{
	LOG_FUNC
	TRetransmissionAndFlowControlOption response = aPeer;
	// Peer sends us 0s in Retransmission and Monitor time-outs in Request and expects us
	// to send values for the timers we're going to use in the response.
	response.SetRetransmissionTimeout(TRetransmissionAndFlowControlOption::KDefaultRetransmissionTimeout);
	response.SetMonitorTimeout(TRetransmissionAndFlowControlOption::KDefaultMonitorTimeout);
	ZeroUnspecifiedResponseFields(response);

	// Trim TxWindow and MPS to our preferred values.
	response.SetTxWindowSize(Min(TRetransmissionAndFlowControlOption::KDefaultEnhancedTxWindowSize,
								 response.TxWindowSize()));
	response.SetMaximumPDUSize(Min(TRetransmissionAndFlowControlOption::KDefaultMaximumPDUSize,
								   response.MaximumPDUSize()));

	aPreferred = response;
	// Include the FEC in ConfigRsp.
	return ETrue;
	}

void TErtmFecHandler::BuildNegativeResponse(TRetransmissionAndFlowControlOption& aPreferred,
                                            const TRetransmissionAndFlowControlOption& /*aPeer*/) const
    {
    LOG_FUNC
    // Channel Mode has been already set and that's the parameter that what we're actually
    // rejecting. The rest is informational and should be set by the remote to its own
    // liking based on the mode proposed, so we _could_ set them to 0, but just in case
    // we're talking to a dumb peer - or a bit older version of my own code that wouldn't
    // accept 0s, for that matter - set them to sensible defaults.
    aPreferred = TRetransmissionAndFlowControlOption(aPreferred.LinkMode(), ETrue);
    ZeroUnspecifiedRequestFields(aPreferred);
    }

void TErtmFecHandler::SetMaxTransmit(TRetransmissionAndFlowControlOption& aFecOption, TUint8 aMaxTransmit) const
	{
	aFecOption.SetMaxTransmit(aMaxTransmit);
	}

void TErtmFecHandler::PrepareImplicitPeerResponse(TRetransmissionAndFlowControlOption& aImplicitResponse,
												  const TRetransmissionAndFlowControlOption& aPreferred) const
	{
	LOG_FUNC
	TModeSpecificFecOptionHandlerBase::PrepareImplicitPeerResponse(aImplicitResponse, aPreferred);
	// Peer is supposed to send us time-out values that it will use. It should always send
	// a response FEC if we're negotiating enhanced modes, so eventually we will get the real
	// values (unless the peer is broken).
	aImplicitResponse.SetRetransmissionTimeout(TRetransmissionAndFlowControlOption::KDefaultRetransmissionTimeout);
	aImplicitResponse.SetMonitorTimeout(TRetransmissionAndFlowControlOption::KDefaultMonitorTimeout);
	}

void TErtmFecHandler::ZeroUnspecifiedRequestFields(TRetransmissionAndFlowControlOption& aFecOption) const
	{
	aFecOption.SetRetransmissionTimeout(0);
	aFecOption.SetMonitorTimeout(0);
	}

void TErtmFecHandler::ZeroUnspecifiedResponseFields(TRetransmissionAndFlowControlOption& aFecOption) const
	{
	aFecOption.SetMaxTransmit(0);
	}

// Flow Control Mode and Retransmission Mode

TBool TLegacyFecHandler::IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const
	{
	LOG_FUNC
	return aFecOption.MaximumPDUSize() >= TRetransmissionAndFlowControlOption::KMinValidMaximumPDUSize &&
		   aFecOption.TxWindowSize() >= TRetransmissionAndFlowControlOption::KMinValidTxWindowSize &&
		   aFecOption.TxWindowSize() <= TRetransmissionAndFlowControlOption::KMaxValidLegacyTxWindowSize &&
		   aFecOption.MaxTransmit() >= TRetransmissionAndFlowControlOption::KMinValidNumberTransmit &&
		   aFecOption.MaxTransmit() <= TRetransmissionAndFlowControlOption::KMaxValidLegacyNumberTransmit;
	}

TBool TLegacyFecHandler::IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
												  const TRetransmissionAndFlowControlOption& aPeer) const
	{
	LOG_FUNC
	return aPeer == aPreferred;
	}

void TLegacyFecHandler::SetMaxTransmit(TRetransmissionAndFlowControlOption& aFecOption, TUint8 aMaxTransmit) const
	{
	aFecOption.SetMaxTransmit(aMaxTransmit == TRetransmissionAndFlowControlOption::KMaxValidEnhancedNumberTransmit ?
							  TRetransmissionAndFlowControlOption::KMaxValidLegacyNumberTransmit :
							  aMaxTransmit);
	}

void TLegacyFecHandler::BuildRequestBasedOnUnacceptableParamsResponse(
		TRetransmissionAndFlowControlOption& aPreferred,
		const TRetransmissionAndFlowControlOption& aPeer,
		const TL2CapFecNegotiator& aFecNegotiator) const
    {
    LOG_FUNC
	// In general the only negotiable parameter is channel mode - that's what we should
	// take from the suggested response passed here in aPeer. The rest of the parameters are
	// informative and we should use our own values, based on the mode - the remote can not
	// reject informative parameters.
	//
	// HOWEVER:
	//
	// Unfortunately <= 9.4 Symbian code _WILL_ reject if e.g. we send a MaxTransmit value
	// which is greater than its own preference. To interoperate with those we take
	// the whole FEC option suggested by the remote, including the informational parameters.
	// <= 9.4 Symbian devices implement RTM and FC modes (and not ERTM and Streaming).
	// Additionally, pretty much noone else in the market does - most people jumped from
	// Basic straight to ERTM & Streaming. So it can be assumed that:
	// Legacy (RTM & FC) ~= pre-ERTM Symbian. 

	typedef TRetransmissionAndFlowControlOption TFec;	// just 'coz I'm lazy

	// Use the remote's whole suggestion if all informational parameters look edible ...
	if (aPeer.TxWindowSize()          >= TFec::KMinValidTxWindowSize &&
		aPeer.MaxTransmit()           >= TFec::KMinValidNumberTransmit &&
		aPeer.RetransmissionTimeout() >= TFec::KMinAcceptableRetransmissionTimeout &&
		aPeer.MonitorTimeout()        >= TFec::KMinAcceptableMonitorTimeout &&
		aPeer.MaximumPDUSize()        >= TFec::KMinValidMaximumPDUSize)
		{
		aPreferred = aPeer;
		}
	else // ... otherwise only use channel mode to prevent DoS attacks.
		{
		aPreferred = TRetransmissionAndFlowControlOption(aPeer.LinkMode(), ETrue);
		SetMaxTransmit(aPreferred, aFecNegotiator.MaxTransmit());
		ZeroUnspecifiedRequestFields(aPreferred);
		}
	}


// Basic mode

TBool TBasicFecHandler::IsOptionValid(const TRetransmissionAndFlowControlOption& /*aFecOption*/) const
	{
	LOG_FUNC
	// Only the mode field is specified for Basic, and we know the mode is Basic already...
	return ETrue;
	}

TBool TBasicFecHandler::IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& /*aPreferred*/,
												 const TRetransmissionAndFlowControlOption& /*aPeer*/) const
	{
	LOG_FUNC
	// Only the mode field is specified for Basic, and we know the mode is Basic already...
	return ETrue;
	}

// TFecOptionHandlerDelegator

TModeSpecificFecOptionHandlerBase& TFecOptionHandlerDelegator::Handler(const TRetransmissionAndFlowControlOption& aFecOption)
	{
	TModeSpecificFecOptionHandlerBase* handler = ModeToHandler(aFecOption.LinkMode());
	__ASSERT_ALWAYS(handler != NULL, Panic(EL2CAPUnknownChannelMode));
	return *handler;
	}

const TModeSpecificFecOptionHandlerBase& TFecOptionHandlerDelegator::Handler(const TRetransmissionAndFlowControlOption &aFecOption) const
	{
	const TModeSpecificFecOptionHandlerBase* handler = ModeToHandler(aFecOption.LinkMode());
	__ASSERT_ALWAYS(handler != NULL, Panic(EL2CAPUnknownChannelMode));
	return *handler;
	}

TModeSpecificFecOptionHandlerBase* TFecOptionHandlerDelegator::ModeToHandler(TL2CapChannelMode aMode)
	{
	TModeSpecificFecOptionHandlerBase* processor = 0;
	switch (aMode)
		{
		case EL2CAPStreamingMode:
			processor = &iStreamingFecHandler;
			break;
		case EL2CAPEnhancedRetransmissionMode:
			processor = &iErtmFecHandler;
			break;
		case EL2CAPRetransmissionMode:
		case EL2CAPFlowControlMode:
			processor = &iLegacyFecHandler;
			break;
		case EL2CAPBasicMode:
			processor = &iBasicFecHandler;
			break;
		default:
			break;
			// yes, return NULL.
		}
	return processor;
	}

const TModeSpecificFecOptionHandlerBase* TFecOptionHandlerDelegator::ModeToHandler(TL2CapChannelMode aMode) const
	{
	return const_cast<TFecOptionHandlerDelegator&>(*this).ModeToHandler(aMode);
	}

#ifdef __FLOG_ACTIVE
void TFecOptionHandlerDelegator::LogCurrentValues(const TRetransmissionAndFlowControlOption& aPreferred,
												  const TRetransmissionAndFlowControlOption& aPeer) const
	{
	TBuf<TRetransmissionAndFlowControlOption::KReadableDesSpaceRequired> buf;
	aPeer.GetReadable(buf);
	LOG1(_L("\tCurrent Peer: %S"), &buf)
	buf.Zero();
	aPreferred.GetReadable(buf);
	LOG1(_L("\tCurrent Preferred: %S"), &buf)
	}
#endif

//
// TL2CapSingleDirectionFecNegotiatorBase
//
void TL2CapSingleDirectionFecNegotiatorBase::SetupForRenegotiation()
	{
	LOG_FUNC
	if (iFecNegotiator.DesiredMode() == EL2CAPBasicMode &&
		// Only skip negotiating Basic if it still is the last accepted value.
		iPeer.LinkMode() == EL2CAPBasicMode)
		{
		// See comment in Setup().
		iConfigStatus = EOptionConfigComplete;
		}
#ifdef __FLOG_ACTIVE
	LogState();
#endif
	}

TBool TL2CapSingleDirectionFecNegotiatorBase::IsPeerModeAcceptable(TL2CapChannelMode aPeerMode,
																   TBool& aDisconnect) const
	{
	LOG_FUNC
	TBool acceptable = ETrue;
	aDisconnect = EFalse;

	LOG3(_L("\tNegotiation Behavior = %d, Desired Mode = %d, Peer Mode = %d"),
		 iFecNegotiator.NegotiationBehavior(), iFecNegotiator.DesiredMode(), aPeerMode)

	if (iFecNegotiator.NegotiationBehavior() == TL2CapFecNegotiator::EState2)
		{
		if (iFecNegotiator.DesiredMode() != aPeerMode)
			{
			// Game over.
			acceptable = EFalse;
			aDisconnect = ETrue;
			}
		}
	else // some sort of State 1
		{
		// A safety net - check that it's legal for our peer to propose this mode.
		if (!iFecNegotiator.IsModeLegal(aPeerMode))
			{
			// Remote is behaving out of spec.
			acceptable = EFalse;
			LOG1(_L("Peer proposed an illegal mode = %d"), aPeerMode);
			}
		else
			{
			// Check that the mode is within our limits according to the spec-defined precedence
			// hierarchy. Note that the spec only defines the precedence and State 1/2 algorithm
			// for the new modes + Basic, but we try and apply it to the legacy modes as well for
			// predictable & consistent behavior.
			acceptable = aPeerMode <= iFecNegotiator.DesiredMode();
			// Now, the State 1 algorithm gives us two choices in case peer proposes a mode
			// that's higher precedence than our desired mode (aPeerMode < iDesiredMode) -
			// - we can either accept it or close the connection - we cannot propose a different
			// one. EState1NoUnreliableToReliable means we do not want to allow fallback from an
			// unreliable mode to a reliable one, i.e. when we propose an unreliable mode,
			// but the remote proposes a reliable one (ERTM is higher prec. than Streaming,
			// ditto RTM vs FC).
			if (acceptable && iFecNegotiator.NegotiationBehavior() == TL2CapFecNegotiator::EState1NoUnreliableToReliable &&
				(iFecNegotiator.DesiredMode() == EL2CAPStreamingMode || iFecNegotiator.DesiredMode() == EL2CAPFlowControlMode) &&
				(aPeerMode == EL2CAPEnhancedRetransmissionMode || aPeerMode == EL2CAPRetransmissionMode))
				{
				acceptable = EFalse;
				if (!(iFecNegotiator.DesiredMode() == EL2CAPFlowControlMode && aPeerMode == EL2CAPRetransmissionMode))
					{
					// Only disconnect immediately if the peer seems to be implementing enhanced
					// modes - mostly to be interoperable with pre-2.1 Core Spec Addendum 1 Symbian
					// code, which doesn't handle disconnects well when there're still unresponded
					// request commands.
					// If we don't disconnect, a FEC for our desired mode will be sent, and we'll
					// see what happens - free style negotiation is allowed with legacy remotes.
					aDisconnect = ETrue;
					}
				LOG(_L("\tRefusing to fall back from Unreliable to Reliable"));
				}
			}
		} // State 1

	LOG2(_L("\tPeer channel mode acceptable = %d, disconnect required = %d"), acceptable, aDisconnect);
	return acceptable;
	}

#ifdef __FLOG_ACTIVE
void TL2CapSingleDirectionFecNegotiatorBase::LogState() const
	{
	LOG3(_L("\tdesired channel mode = %d, mode negotiation behavior = %d, config status = %d"),
			iFecNegotiator.DesiredMode(), iFecNegotiator.NegotiationBehavior(), iConfigStatus)
	TBuf<TRetransmissionAndFlowControlOption::KReadableDesSpaceRequired> readable;
	iPreferred.GetReadable(readable);
	LOG1(_L("\tpreferred FEC = %S"), &readable);
	readable.Zero();
	iPeer.GetReadable(readable);
	LOG1(_L("\tpeer FEC = %S"), &readable);
	}
#endif

//
// TL2CapIncomingFecNegotiator
//
void TL2CapIncomingFecNegotiator::Setup()
    {
    LOG_FUNC
    // Set up spec default as initial peer value.
    iPeer = TRetransmissionAndFlowControlOption();

    // Set up our initial request.
    BuildRequest(iFecNegotiator.DesiredMode(), iPreferred);

    if (iFecNegotiator.DesiredMode() == EL2CAPBasicMode)
        {
        // Basic mode is the implicit default, so it's complete unless peer says otherwise.
        // Besides, the fact that Basic is our desired mode normally means that the peer doesn't
        // support anything else, so it would not be able to parse a TRetransmissionAndFlowControl
        // option if we sent it.
        iConfigStatus = EOptionConfigComplete;
        }

#ifdef __FLOG_ACTIVE
    LogState();
#endif
    }

void TL2CapIncomingFecNegotiator::ProcessPeerValue(const TRetransmissionAndFlowControlOption& aFecOption,
												   TBool aIsUnacceptableParameters)
	{
	LOG_FUNC

	iPeer = aFecOption;

	// 'return' parameter from IsPeerModeAcceptable() - we'll ignore it because in
	// incoming direction we disconnect if response config status is failed anyway.
	TBool disconnect;

	TBool peerAcceptable = IsPeerModeAcceptable(iPeer.LinkMode(), disconnect);
	if (peerAcceptable && !aIsUnacceptableParameters)
		{
		peerAcceptable = iFecNegotiator.ModeSpecificHandlers().IsPeerResponseAcceptable(iPreferred, iPeer);
		}

 	if (!peerAcceptable)
 		{
		iConfigStatus = EOptionConfigFailed;
 		}
 	else
 		{
	 	if (aIsUnacceptableParameters)
			{
	 		iConfigStatus = EOptionConfigOutstanding;
	 		// Build a new request based on suggestion sent by the peer.
	 		iFecNegotiator.ModeSpecificHandlers().BuildRequestBasedOnUnacceptableParamsResponse(
	 				iPreferred, iPeer, iFecNegotiator);
			}
		else
			{
			iConfigStatus = EOptionConfigComplete;
			}
		}
 	LOG1(_L("\tIncoming FEC Config Status is now %d"), iConfigStatus);
	}

void TL2CapIncomingFecNegotiator::ProcessImplicitPeerValue()
	{
	LOG_FUNC
	// We need to assume that the peer accepted our request and sent an appropriate response back.
	TRetransmissionAndFlowControlOption response;
	iFecNegotiator.ModeSpecificHandlers().PrepareImplicitPeerResponse(response, iPreferred);
	ProcessPeerValue(response, EFalse);
	}

void TL2CapIncomingFecNegotiator::DowngradeToBasic()
	{
	LOG_FUNC
	iPreferred = TRetransmissionAndFlowControlOption();
	iConfigStatus = EOptionConfigComplete;
	}


void TL2CapIncomingFecNegotiator::BuildRequest(TL2CapChannelMode aMode, TRetransmissionAndFlowControlOption& aFecOption)
    {
    LOG_FUNC
    aFecOption = TRetransmissionAndFlowControlOption(aMode, ETrue);
    iFecNegotiator.ModeSpecificHandlers().SetMaxTransmit(aFecOption, iFecNegotiator.MaxTransmit());
    iFecNegotiator.ModeSpecificHandlers().ZeroUnspecifiedRequestFields(aFecOption);
    }

//
// TL2CapOutgoingFecNegotiator
//
void TL2CapOutgoingFecNegotiator::Setup()
    {
    LOG_FUNC
    // Set up spec default as initial peer value.
    iPeer = TRetransmissionAndFlowControlOption();

    // iPreferred will be constructed when we process peer value once a Config Request has been received.
#ifdef __FLOG_ACTIVE
    LogState();
#endif
    }

TInt TL2CapOutgoingFecNegotiator::ProcessPeerValue(const TRetransmissionAndFlowControlOption& aFecOption)
	{
	LOG_FUNC

	TInt err = KErrNone;
	TBool disconnect = EFalse;
	TBool peerModeAcceptable = IsPeerModeAcceptable(aFecOption.LinkMode(), disconnect);
	
	if (peerModeAcceptable)
		{
		iPeer = aFecOption;
		iIncludeValueInPositiveConfigResponse = iFecNegotiator.ModeSpecificHandlers().BuildPositiveResponse(iPreferred, iPeer);
		iConfigStatus = EOptionConfigComplete;
		}
	else
		{
		if (disconnect)
			{
			// Disconnect immediately without sending an Unacceptable Parameters response.
			iConfigStatus = EOptionConfigFailed;
			err = KErrConfigRejected;
			}
		else
			{
            iPreferred.SetLinkMode(iFecNegotiator.DesiredMode());			
			iFecNegotiator.ModeSpecificHandlers().BuildNegativeResponse(iPreferred, aFecOption);
			// Cause an Unacceptable Parameters response.
			iConfigStatus = EOptionConfigFailed;
			// Preferred contains our desired FEC, it will be included in the Unaccepted Parameters response.
			}
		}
	LOG1(_L("\tOutgoing FEC Config Status is now %d"), iConfigStatus);
	return err;
	}


//
// TL2CapFecNegotiator
//
TBool TL2CapFecNegotiator::Setup(TL2CapConfig::TChannelReliability aChannelReliability,
								 TBool aLegacyModesDisallowed,
								 const TL2CapEntityInfo& aPeerEntityConfig,
								 TUint8 aMaxTransmit)
	{
	LOG_FUNC

	__ASSERT_DEBUG(aPeerEntityConfig.LinkInfoState() == EL2CapEntityInfoDefined,
				   Panic(EL2CAPFecConfigAttemptWithoutPeerInfo));
	iPeerSupportedModes = aPeerEntityConfig;
    iMaxTransmit = aMaxTransmit;
    
	TBool modeNegotiable = EFalse;

	iDesiredMode = EL2CAPBasicMode;

	// From 2.4 Modes of Operation:
	// "Flow Control Mode and Retransmission mode shall only be enabled when communicating with
	// L2CAP entities that do not support either Enhanced Retransmission mode or Streaming mode."
	// From 5.4 Retransmission And Flow Control Option:
	// "Basic mode, Flow Control mode and Retransmission mode shall only be used for backwards
	// compatibility with L2CAP entities that do not support Enhanced Retransmission mode or
	// Streaming mode."
	//
	// Make of that what you will, but the official Symbian interpretation is that RTM or FC may
	// only be used if the remote doesn't support any of the new modes.
	const TBool enhancedModeSupported = iPeerSupportedModes.SupportsEnhancedRetransmissionMode() ||
										iPeerSupportedModes.SupportsStreamingMode();

	switch (aChannelReliability)
		{
	case TL2CapConfig::EReliableChannel:
		iNegotiationBehavior = aLegacyModesDisallowed ? EState2 : EState1;
		if (iPeerSupportedModes.SupportsEnhancedRetransmissionMode())
			{
			iDesiredMode = EL2CAPEnhancedRetransmissionMode;
			modeNegotiable = ETrue;
			}
		else if (!aLegacyModesDisallowed)
			{
			if (iPeerSupportedModes.SupportsRetranmission() && !enhancedModeSupported)
				{
				iDesiredMode = EL2CAPRetransmissionMode;
				modeNegotiable = ETrue;
				}
			else
				{
				iDesiredMode = EL2CAPBasicMode;
				modeNegotiable = ETrue;
				}
			}
		break;

	case TL2CapConfig::EUnreliableChannel:
		iNegotiationBehavior = aLegacyModesDisallowed ? EState2 : EState1NoUnreliableToReliable;
		if (iPeerSupportedModes.SupportsStreamingMode())
			{
			iDesiredMode = EL2CAPStreamingMode;
			modeNegotiable = ETrue;
			}
		else if (!aLegacyModesDisallowed)
			{
			if (iPeerSupportedModes.SupportsFlowControl()  && !enhancedModeSupported)
				{
				iDesiredMode = EL2CAPFlowControlMode;
				modeNegotiable = ETrue;
				}
			else
				{
				iDesiredMode = EL2CAPBasicMode;
				modeNegotiable = ETrue;
				}
			}
		break;

	case TL2CapConfig::EUnreliableDesiredChannel:
		// Be open to all proposals within spec-defined constraints.
		iNegotiationBehavior = EState1;
		if (iPeerSupportedModes.SupportsStreamingMode())
			{
			iDesiredMode = EL2CAPStreamingMode;
			modeNegotiable = ETrue;
			}
		else if (iPeerSupportedModes.SupportsFlowControl() & !enhancedModeSupported)
			{
			iDesiredMode = EL2CAPFlowControlMode;
			modeNegotiable = ETrue;
			}
		else if (iPeerSupportedModes.SupportsEnhancedRetransmissionMode())
			{
			iDesiredMode = EL2CAPEnhancedRetransmissionMode;
			modeNegotiable = ETrue;
			}
		else if (iPeerSupportedModes.SupportsRetranmission() && !enhancedModeSupported)
			{
			iDesiredMode = EL2CAPRetransmissionMode;
			modeNegotiable = ETrue;
			}
		else
			{
			iDesiredMode = EL2CAPBasicMode;
			modeNegotiable = ETrue;
			}
		break;
		}

	if (modeNegotiable)
		{
        iIncomingNegotiator.Setup();
        iOutgoingNegotiator.Setup();
		}
	return modeNegotiable;
	}

void TL2CapFecNegotiator::SetupForRenegotiation()
	{
	LOG_FUNC
	iIncomingNegotiator.SetupForRenegotiation();
	iOutgoingNegotiator.SetupForRenegotiation();
	}

// A helper that optimizes the negotiation process by downgrading incoming preferred mode
// to Basic if the remote requests Basic, we accept it, and haven't sent out our request yet.
// Caveat: This should be only called if our Config Request hasn't been sent yet
// and we know we've received peer's Config Request.
void TL2CapFecNegotiator::DowngradeIncomingToBasicIfOutgoingIsBasic()
	{
	LOG_FUNC
	// If we've already accepted Peer's Basic mode request, we may downgrade the incoming
	// direction to Basic as well, since FEC in a single direction is forbidden.
	// Otherwise we may end up with FEC in just one direction and would have to
	// downgrade and renegotiate anyway.
	// Note: this only makes sense if we haven't sent our Config Request yet.
	// The caller is responsible for checking this.
	if ((iNegotiationBehavior == EState1 ||
		 iNegotiationBehavior == EState1NoUnreliableToReliable) &&
		iOutgoingNegotiator.ConfigOptionStatus() != TL2CapConfigurationOptionGroupBase::EOptionConfigFailed &&
		iOutgoingNegotiator.Preferred().LinkMode() == EL2CAPBasicMode &&
		iIncomingNegotiator.Preferred().LinkMode() != EL2CAPBasicMode)
		{
		LOG(_L("\tReceived Basic mode Config Request, downgrading incoming channel mode to Basic"));
		iIncomingNegotiator.DowngradeToBasic();
		}
	}

TInt TL2CapFecNegotiator::CheckNegotiatedChannelMode(TBool& aDowngrade)
	{
	LOG_FUNC
	TInt err = KErrNone;

	if ((iNegotiationBehavior == EState1 ||
		 iNegotiationBehavior == EState1NoUnreliableToReliable)
		&&
		 ((iOutgoingNegotiator.Preferred().LinkMode() == EL2CAPBasicMode &&
		   iIncomingNegotiator.Peer().LinkMode() != EL2CAPBasicMode)
		  ||
		  (iIncomingNegotiator.Peer().LinkMode() == EL2CAPBasicMode &&
		   iOutgoingNegotiator.Preferred().LinkMode() != EL2CAPBasicMode)))
		{
		LOG(_L("\tDowngrading unidirectional FEC to Basic in both directions"));

		TRetransmissionAndFlowControlOption basicFec;
		iIncomingNegotiator.SetPreferred(basicFec);
		iOutgoingNegotiator.SetPreferred(basicFec);

		SetupForRenegotiation();
		aDowngrade = ETrue;
		}
	else if (IncomingLinkMode() != OutgoingLinkMode())
		{
		LOG2(_L("\tSomehow managed to negotiate %d incoming mode and %d outgoing mode"),
			 IncomingLinkMode(), OutgoingLinkMode());
		err = KErrL2CAPNegotiatedDifferentModesForEachDirection;
		}
	else
		{
		aDowngrade = EFalse;
		}

	return err;
	}

TBool TL2CapFecNegotiator::IsModeLegal(TL2CapChannelMode aPeerMode) const
	{
	LOG_FUNC
	TBool legal = ETrue;
	if ((aPeerMode == EL2CAPEnhancedRetransmissionMode && !iPeerSupportedModes.SupportsEnhancedRetransmissionMode()) ||
		(aPeerMode == EL2CAPStreamingMode              && !iPeerSupportedModes.SupportsStreamingMode()) ||
		(aPeerMode == EL2CAPRetransmissionMode         && !iPeerSupportedModes.SupportsRetranmission()) ||
		(aPeerMode == EL2CAPFlowControlMode            && !iPeerSupportedModes.SupportsFlowControl()) ||
		// if any of the new modes is supported then none of the old ones shall be used
		((aPeerMode == EL2CAPRetransmissionMode || aPeerMode == EL2CAPFlowControlMode) &&
		 (iPeerSupportedModes.SupportsEnhancedRetransmissionMode() || iPeerSupportedModes.SupportsStreamingMode())))
		{
		legal = EFalse;
		}
	return legal;
	}

// The enhanced modes are ordered according to the 2.1 Core Spec Addendum 1
// "state 1" precedence (pt 5.4), but in reverse order.
//	1. Streaming
//	2. ERTM
//	3. Basic
// The legacy modes are ordered similarly:
//	1. Flow Control
//  2. RTM
//	3. Basic
// Additionally pairs consisting of a legacy and a new mode are never in relation.
static TBool operator<=(TL2CapChannelMode aLeft, TL2CapChannelMode aRight)
	{
	TBool inRelation = EFalse;
	switch(aLeft)
		{
		case EL2CAPBasicMode:
			inRelation = ETrue;
			break;

		case EL2CAPRetransmissionMode:
			switch (aRight)
				{
				case EL2CAPBasicMode:
					inRelation = EFalse;
					break;
				case EL2CAPRetransmissionMode:
					inRelation = ETrue;
					break;
				case EL2CAPFlowControlMode:
					inRelation = ETrue;
					break;
				case EL2CAPEnhancedRetransmissionMode:
					inRelation = EFalse;
					break;
				case EL2CAPStreamingMode:
					inRelation = EFalse;
					break;
				}
			break;

		case EL2CAPFlowControlMode:
			switch (aRight)
				{
				case EL2CAPBasicMode:
					inRelation = EFalse;
					break;
				case EL2CAPRetransmissionMode:
					inRelation = EFalse;
					break;
				case EL2CAPFlowControlMode:
					inRelation = ETrue;
					break;
				case EL2CAPEnhancedRetransmissionMode:
					inRelation = EFalse;
					break;
				case EL2CAPStreamingMode:
					inRelation = EFalse;
					break;
				}
			break;

		case EL2CAPEnhancedRetransmissionMode:
			switch (aRight)
				{
				case EL2CAPBasicMode:
					inRelation = EFalse;
					break;
				case EL2CAPRetransmissionMode:
					inRelation = EFalse;
					break;
				case EL2CAPFlowControlMode:
					inRelation = EFalse;
					break;
				case EL2CAPEnhancedRetransmissionMode:
					inRelation = ETrue;
					break;
				case EL2CAPStreamingMode:
					inRelation = ETrue;
					break;
				}
			break;

		case EL2CAPStreamingMode:
			switch (aRight)
				{
				case EL2CAPBasicMode:
					inRelation = EFalse;
					break;
				case EL2CAPRetransmissionMode:
					inRelation = EFalse;
					break;
				case EL2CAPFlowControlMode:
					inRelation = EFalse;
					break;
				case EL2CAPEnhancedRetransmissionMode:
					inRelation = EFalse;
					break;
				case EL2CAPStreamingMode:
					inRelation = ETrue;
					break;
				}
			break;
		}

	return inRelation;
	}

template<typename T>
static inline TBool IsWithinBounds(const T aValue, const T aLeftBound, const T aRightBound)
	{
	return aLeftBound <= aValue && aValue <= aRightBound;
	}
