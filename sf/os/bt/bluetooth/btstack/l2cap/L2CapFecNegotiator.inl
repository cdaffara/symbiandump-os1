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

#ifndef L2CAPFECNEGOTIATOR_INL
#define L2CAPFECNEGOTIATOR_INL

// Not needed, but keeps Carbide's semantic analyzer a bit happier.
#include "L2CapFecNegotiator.h"

//
// TFecOptionHandlerDelegator inlines
//
inline TBool TFecOptionHandlerDelegator::IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const
	{
	const TModeSpecificFecOptionHandlerBase* handler = ModeToHandler(aFecOption.LinkMode());
	// handler == NULL means invalid mode field
	return handler && handler->IsOptionValid(aFecOption);
	}

inline TBool TFecOptionHandlerDelegator::IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
																  const TRetransmissionAndFlowControlOption& aPeer) const
	{
#ifdef __FLOG_ACTIVE
	LogCurrentValues(aPreferred, aPeer);
#endif
	__ASSERT_DEBUG(aPeer.LinkMode() == aPreferred.LinkMode(), Panic(EL2CAPPeerAndPreferredChannelModesNotEqual));

	// Logically it's Preferred checking whether Peer is acceptable, so dispatch on preferred.
	// (If we're here then both modes are equal though, so it doesn't really matter, but just
	// sayin' in case it changes).
	return Handler(aPreferred).IsPeerResponseAcceptable(aPreferred, aPeer);
	}

inline TBool TFecOptionHandlerDelegator::BuildPositiveResponse(TRetransmissionAndFlowControlOption& aPreferred,
															   const TRetransmissionAndFlowControlOption& aPeer) const
	{
#ifdef __FLOG_ACTIVE
	LogCurrentValues(aPreferred, aPeer);
#endif

	// Response will have peer - proposed mode so dispatch on peer.
	return Handler(aPeer).BuildPositiveResponse(aPreferred, aPeer);
	}

inline void TFecOptionHandlerDelegator::BuildNegativeResponse(TRetransmissionAndFlowControlOption& aPreferred,
															  const TRetransmissionAndFlowControlOption& aPeer) const
	{
#ifdef __FLOG_ACTIVE
	LogCurrentValues(aPreferred, aPeer);
#endif

	// Response will have our preferred mode so dispatch on preferred.
	Handler(aPreferred).BuildNegativeResponse(aPreferred, aPeer);
	}

inline void
TFecOptionHandlerDelegator::BuildRequestBasedOnUnacceptableParamsResponse(
		TRetransmissionAndFlowControlOption& aPreferred,
		const TRetransmissionAndFlowControlOption& aPeer,
		const TL2CapFecNegotiator& aFecNegotiator) const
	{
#ifdef __FLOG_ACTIVE
	LogCurrentValues(aPreferred, aPeer);
#endif
	// Response has suggested mode, dispatch on that.
	Handler(aPeer).BuildRequestBasedOnUnacceptableParamsResponse(aPreferred, aPeer, aFecNegotiator);
	}

inline void TFecOptionHandlerDelegator::PrepareImplicitPeerResponse(TRetransmissionAndFlowControlOption& aImplicitResponse,
																	const TRetransmissionAndFlowControlOption& aPreferred) const
	{
	// Response will have our preferred mode so dispatch on preferred.
	Handler(aPreferred).PrepareImplicitPeerResponse(aImplicitResponse, aPreferred);
	}

inline void TFecOptionHandlerDelegator::SetMaxTransmit(TRetransmissionAndFlowControlOption& aFecOption, TUint8 aMaxTransmit) const
	{
	Handler(aFecOption).SetMaxTransmit(aFecOption, aMaxTransmit);
	}

inline void TFecOptionHandlerDelegator::ZeroUnspecifiedRequestFields(TRetransmissionAndFlowControlOption& aFecOption) const
	{
	Handler(aFecOption).ZeroUnspecifiedRequestFields(aFecOption);
	}

inline void TFecOptionHandlerDelegator::ZeroUnspecifiedResponseFields(TRetransmissionAndFlowControlOption& aFecOption) const
	{
	Handler(aFecOption).ZeroUnspecifiedResponseFields(aFecOption);
	}


//
// TL2CapSingleDirectionFecNegotiatorBase inlines
//
inline TL2CapSingleDirectionFecNegotiatorBase::TL2CapSingleDirectionFecNegotiatorBase(const TL2CapFecNegotiator& aFecNegotiator)
 :	iFecNegotiator(aFecNegotiator),
	iConfigStatus(EOptionConfigOutstanding)
	{}

inline TL2CapConfigurationOptionGroupBase::TOptionConfigStatus TL2CapSingleDirectionFecNegotiatorBase::ConfigOptionStatus() const
	{
	return iConfigStatus;
	}

inline void TL2CapSingleDirectionFecNegotiatorBase::SetPeer(const TRetransmissionAndFlowControlOption& aFec)
	{
	iPeer = aFec;
	}

inline void TL2CapSingleDirectionFecNegotiatorBase::SetPreferred(const TRetransmissionAndFlowControlOption& aFec)
	{
	iPreferred = aFec;
	}

inline const TRetransmissionAndFlowControlOption& TL2CapSingleDirectionFecNegotiatorBase::Peer() const
	{
	return iPeer;
	}

inline const TRetransmissionAndFlowControlOption& TL2CapSingleDirectionFecNegotiatorBase::Preferred() const
	{
	return iPreferred;
	}


inline TL2CapIncomingFecNegotiator::TL2CapIncomingFecNegotiator(const TL2CapFecNegotiator& aFecNegotiator)
 :	TL2CapSingleDirectionFecNegotiatorBase(aFecNegotiator)
	{}

inline TL2CapOutgoingFecNegotiator::TL2CapOutgoingFecNegotiator(const TL2CapFecNegotiator& aFecNegotiator)
 :	TL2CapSingleDirectionFecNegotiatorBase(aFecNegotiator),
	iIncludeValueInPositiveConfigResponse(EFalse)
	{}

inline TBool TL2CapOutgoingFecNegotiator::NeedToIncludeInPositiveConfigResponse()
	{
	return iIncludeValueInPositiveConfigResponse;
	}

inline void TL2CapOutgoingFecNegotiator::SetIncludeInPositiveConfigResponse(TBool aVal)
    {
    iIncludeValueInPositiveConfigResponse = aVal;
    }


//
// TL2CapFecNegotiator inlines
//

inline TL2CapFecNegotiator::TL2CapFecNegotiator()
 :	iIncomingNegotiator(*this),
 	iOutgoingNegotiator(*this),
 	iNegotiationBehavior(EState1),
 	iDesiredMode(EL2CAPBasicMode)
	{}

inline TInt TL2CapFecNegotiator::PeerRequestsOption(const TRetransmissionAndFlowControlOption& aFecOption)
	{
	if (iModeSpecificHandlers.IsOptionValid(aFecOption))
		{
		return iOutgoingNegotiator.ProcessPeerValue(aFecOption);
		}
	else
		{
		return KErrBadPacketReceived;
		}
	}

inline TInt TL2CapFecNegotiator::PeerRequestsLastAcceptedValue()
	{
	TInt err = iOutgoingNegotiator.ProcessPeerValue(iOutgoingNegotiator.Peer());
	iOutgoingNegotiator.SetIncludeInPositiveConfigResponse(EFalse);
	return err;
	}

inline TInt TL2CapFecNegotiator::PeerRejectsOption(const TRetransmissionAndFlowControlOption& aFecOption)
	{
	TInt err = KErrNone;

    // Peer has sent us an Unacceptable Parameters response with a Retransmission And Flow Control
    // option in it. The only useful information there is the Channel Mode field - this is the one
    // actually being rejected, all the rest of them are non-negotiable (informational).
    // Some implementations send their default values for those informational parameters, others
    // just set them to 0 (that's what we do). Whatever their values are, we don't use them - we
    // use our own based on the mode suggested by the remote.
    // This is why we don't (and can't - they may be 0) check all the parameters with IsOptionValid
    // - channel mode is all we're interested in.

    if (TRetransmissionAndFlowControlOption::IsModeValid(aFecOption.LinkMode()))
		{
		iIncomingNegotiator.ProcessPeerValue(aFecOption, ETrue);
		}
	else
		{
		err = KErrBadPacketReceived;
		}
	return err;
	}

inline TInt TL2CapFecNegotiator::PeerAcceptsOption(const TRetransmissionAndFlowControlOption& aFecOption)
	{
	TInt err = KErrNone;
	if (iModeSpecificHandlers.IsOptionValid(aFecOption))
		{
		iIncomingNegotiator.ProcessPeerValue(aFecOption, EFalse);
		}
	else
		{
		err = KErrBadPacketReceived;
		}
	return err;
	}

inline void TL2CapFecNegotiator::PeerAcceptsOption()
	{
	iIncomingNegotiator.ProcessImplicitPeerValue();
	}

inline TBool TL2CapFecNegotiator::NeedToIncludeInPositiveConfigResponse()
	{
	return iOutgoingNegotiator.NeedToIncludeInPositiveConfigResponse();
	}

inline const TL2CapEntityInfo& TL2CapFecNegotiator::PeerSupportedModes() const
	{
	return iPeerSupportedModes;
	}

inline TL2CapFecNegotiator::TNegotiationBehavior TL2CapFecNegotiator::NegotiationBehavior() const
	{
	return iNegotiationBehavior;
	}

inline TL2CapChannelMode TL2CapFecNegotiator::DesiredMode() const
	{
	return iDesiredMode;
	}

inline TUint8 TL2CapFecNegotiator::MaxTransmit() const
    {
    return iMaxTransmit;
    }

inline const TFecOptionHandlerDelegator& TL2CapFecNegotiator::ModeSpecificHandlers() const
	{
	return iModeSpecificHandlers;
	}

inline const TRetransmissionAndFlowControlOption& TL2CapFecNegotiator::IncomingPreferred() const
	{
	return iIncomingNegotiator.Preferred();
	}

inline const TRetransmissionAndFlowControlOption& TL2CapFecNegotiator::OutgoingPreferred() const
	{
	return iOutgoingNegotiator.Preferred();
	}

inline TL2CapConfigurationOptionGroupBase::TOptionConfigStatus TL2CapFecNegotiator::IncomingConfigOptionStatus() const
	{
	return iIncomingNegotiator.ConfigOptionStatus();
	}

inline TL2CapConfigurationOptionGroupBase::TOptionConfigStatus TL2CapFecNegotiator::OutgoingConfigOptionStatus() const
	{
	return iOutgoingNegotiator.ConfigOptionStatus();
	}

inline TL2CapDataControllerConfig TL2CapFecNegotiator::DataControllerConfig() const
	{
	return TL2CapDataControllerConfig(OutgoingLinkMode(),
									  OutgoingTxWindowSize(),
									  OutgoingMaxTransmit(),
									  OutgoingRetransmissionTimeout(),
									  IncomingMonitorTimeout(),
									  IncomingTxWindowSize(),
									  IncomingRetransmissionTimeout(),
									  IncomingMaximumPDUSize());
	}


// The general rule for methods below is that for the Outgoing direction we get the parameters
// from the Preferred fields since in case of wildcard params (TxWin, MPS), these fields contain
// the correct minimized values. For the same reason the Incoming direction parameters are fetched
// from the Peer values.
// Note that it's not always that simple in case of non-negotiated parameters like MaxTransmit,
// Retransmission & Monitor TO, which is the reason these methods exist.

inline TL2CapChannelMode TL2CapFecNegotiator::OutgoingLinkMode() const
	{
	return iOutgoingNegotiator.Preferred().LinkMode();
	}

inline TL2CapChannelMode TL2CapFecNegotiator::IncomingLinkMode() const
	{
	return iIncomingNegotiator.Peer().LinkMode();
	}

inline TUint8 TL2CapFecNegotiator::OutgoingLinkModeAsUnsignedByte() const
	{
	return iOutgoingNegotiator.Preferred().LinkModeAsUnsignedByte();
	}

inline TUint8 TL2CapFecNegotiator::IncomingLinkModeAsUnsignedByte() const
	{
	return iIncomingNegotiator.Peer().LinkModeAsUnsignedByte();
	}

inline TUint8 TL2CapFecNegotiator::OutgoingTxWindowSize() const
	{
	return iOutgoingNegotiator.Preferred().TxWindowSize();
	}

inline TUint8 TL2CapFecNegotiator::IncomingTxWindowSize() const
	{
	return iIncomingNegotiator.Peer().TxWindowSize();
	}

inline TUint8 TL2CapFecNegotiator::OutgoingMaxTransmit() const
	{
	// Peer sends us the value to use in its Config Request.
	return iOutgoingNegotiator.Peer().MaxTransmit();
	}

inline TUint8 TL2CapFecNegotiator::IncomingMaxTransmit() const
	{
	// We send the peer the value to use in our Config Request.
	return iIncomingNegotiator.Preferred().MaxTransmit();
	}

inline TUint16 TL2CapFecNegotiator::OutgoingRetransmissionTimeout() const
	{
	return iOutgoingNegotiator.Preferred().RetransmissionTimeout();
	}

inline TUint16 TL2CapFecNegotiator::IncomingRetransmissionTimeout() const
	{
	return iIncomingNegotiator.Peer().RetransmissionTimeout();
	}

// Outgoing traffic -> Incoming S-Frames -> Monitor TO that our peer will use.
inline TUint16 TL2CapFecNegotiator::OutgoingMonitorTimeout() const
	{
	if (iIncomingNegotiator.Preferred().LinkMode() == EL2CAPEnhancedRetransmissionMode ||
		iIncomingNegotiator.Preferred().LinkMode() == EL2CAPStreamingMode)
		{
		// Peer sends the time-outs it'll use in its Config Response.
		return iIncomingNegotiator.Peer().MonitorTimeout();
		}
	else
		{
		// Negotiated in the direction it relates to.
		return iOutgoingNegotiator.Preferred().MonitorTimeout();
		}
	}

// Incoming traffic -> Outgoing S-Frames -> Monitor TO that we will use.
inline TUint16 TL2CapFecNegotiator::IncomingMonitorTimeout() const
	{
	if (iIncomingNegotiator.Preferred().LinkMode() == EL2CAPEnhancedRetransmissionMode ||
		iIncomingNegotiator.Preferred().LinkMode() == EL2CAPStreamingMode)
		{
		// We send the time-outs we'll use in our Config Response.
		return iOutgoingNegotiator.Preferred().MonitorTimeout();
		}
	else
		{
		// Negotiated in the direction it relates to.
		return iIncomingNegotiator.Peer().MonitorTimeout();
		}
	}

inline TUint16 TL2CapFecNegotiator::OutgoingMaximumPDUSize() const
	{
	return iOutgoingNegotiator.Preferred().MaximumPDUSize();
	}

inline TUint16 TL2CapFecNegotiator::IncomingMaximumPDUSize() const
	{
	return iIncomingNegotiator.Peer().MaximumPDUSize();
	}

#endif /*L2CAPFECNEGOTIATOR_INL*/
