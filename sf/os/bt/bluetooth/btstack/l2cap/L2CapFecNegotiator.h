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

#ifndef L2CAPFECNEGOTIATOR_H
#define L2CAPFECNEGOTIATOR_H

#include "L2types.h"

class TL2CapFecNegotiator;
class TL2CapSingleDirectionFecNegotiatorBase;

// This encapsulates channel mode dependent FEC option handling behaviors.
// Channel mode policy is applied outside of this.
NONSHARABLE_CLASS(TModeSpecificFecOptionHandlerBase)
	{
public:
	// Checks whether the field values are valid (within specification limits).
	// To be used early, during the parsing of an option received from the peer.
	// Note: valid doesn't mean acceptable.
	virtual TBool IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const = 0;
	virtual TBool IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
										   const TRetransmissionAndFlowControlOption& aPeer) const = 0;
	// The return value says whether the option needs to be included in ConfigRsp.
	// [Only enhanced mode positive responses need to be included as they convey new information
	// on the timers and MPS]
	virtual TBool BuildPositiveResponse(TRetransmissionAndFlowControlOption& aPreferred,
			   							const TRetransmissionAndFlowControlOption& aPeer) const;
	virtual void BuildNegativeResponse(TRetransmissionAndFlowControlOption& aPreferred,
									   const TRetransmissionAndFlowControlOption& aPeer) const;

	virtual void BuildRequestBasedOnUnacceptableParamsResponse(
					TRetransmissionAndFlowControlOption& aPreferred,
					const TRetransmissionAndFlowControlOption& aPeer,
					const TL2CapFecNegotiator& aFecNegotiator) const;

	// In general configuration procedure (spec chapter 7 General Procedures) if the remote accepts
	// an option from our ConfigReq, it doesn't have to include it in the response. So before
	// processing the options from a ConfigRsp we've received, we go through the preferred values
	// we've sent in ConfigReq and pretend that the remote has accepted them without a hitch. Then
	// options included in the received ConfigRsp are processed. This works well with plain-vanilla
	// negotiated options, but TRetransmissionAndFlowControl for enhanced modes is different:
	// request and response semantics are a bit different, and so the request does not contain all
	// information needed in a response. So we have to take the request we've sent, guess the
	// missing information, and thus create a fake response. And that's what this method does.
	virtual void PrepareImplicitPeerResponse(TRetransmissionAndFlowControlOption& aImplicitResponse,
											 const TRetransmissionAndFlowControlOption& aPreferred) const;

	// Set user-requested MaxTransmit value. The value is given in the enhanced range
	// 0-255, where 0 means infinity and is only valid for ERTM. Deriving classes are
	// responsible for the cut-off of the infinity value on 255 if they can't handle it.
	virtual void SetMaxTransmit(TRetransmissionAndFlowControlOption& aFecOption, TUint8 aMaxTransmit) const;

	virtual void ZeroUnspecifiedRequestFields(TRetransmissionAndFlowControlOption& aFecOption) const;
	virtual void ZeroUnspecifiedResponseFields(TRetransmissionAndFlowControlOption& aFecOption) const;
	};

NONSHARABLE_CLASS(TStreamingFecHandler) : public TModeSpecificFecOptionHandlerBase
	{
public:
	virtual TBool IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const;
	virtual TBool IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
										   const TRetransmissionAndFlowControlOption& aPeer) const;
	virtual TBool BuildPositiveResponse(TRetransmissionAndFlowControlOption& aPreferred,
										const TRetransmissionAndFlowControlOption& aPeer) const;
    virtual void BuildNegativeResponse(TRetransmissionAndFlowControlOption& aPreferred,
                                       const TRetransmissionAndFlowControlOption& aPeer) const;	
	virtual void ZeroUnspecifiedRequestFields(TRetransmissionAndFlowControlOption& aFecOption) const; 
	virtual void ZeroUnspecifiedResponseFields(TRetransmissionAndFlowControlOption& aFecOption) const;
	};

NONSHARABLE_CLASS(TErtmFecHandler) : public TModeSpecificFecOptionHandlerBase
	{
public:
	virtual TBool IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const;
	virtual TBool IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
										   const TRetransmissionAndFlowControlOption& aPeer) const;
	virtual TBool BuildPositiveResponse(TRetransmissionAndFlowControlOption& aPreferred,
										const TRetransmissionAndFlowControlOption& aPeer) const;
	virtual void BuildNegativeResponse(TRetransmissionAndFlowControlOption& aPreferred,
									   const TRetransmissionAndFlowControlOption& aPeer) const;
	virtual void PrepareImplicitPeerResponse(TRetransmissionAndFlowControlOption& aImplicitResponse,
											 const TRetransmissionAndFlowControlOption& aPreferred) const;
	virtual void SetMaxTransmit(TRetransmissionAndFlowControlOption& aFecOption, TUint8 aMaxTransmit) const;
	virtual void ZeroUnspecifiedRequestFields(TRetransmissionAndFlowControlOption& aFecOption) const; 
	virtual void ZeroUnspecifiedResponseFields(TRetransmissionAndFlowControlOption& aFecOption) const;
	};

NONSHARABLE_CLASS(TLegacyFecHandler) : public TModeSpecificFecOptionHandlerBase
	{
public:
	virtual TBool IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const;
	virtual TBool IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
										   const TRetransmissionAndFlowControlOption& aPeer) const;
	virtual void BuildRequestBasedOnUnacceptableParamsResponse(
                        TRetransmissionAndFlowControlOption& aPreferred,
                        const TRetransmissionAndFlowControlOption& aPeer,
                        const TL2CapFecNegotiator& aFecNegotiator) const;

	virtual void SetMaxTransmit(TRetransmissionAndFlowControlOption& aFecOption, TUint8 aMaxTransmit) const;
	};

NONSHARABLE_CLASS(TBasicFecHandler) : public TModeSpecificFecOptionHandlerBase
	{
public:
	// Most peers will actually only request Basic implicitly i.e. by not including the FEC
	// option at all, but we treat implicit/explicit values uniformly and will still execute
	// these. And a Basic FEC can be explicit if the peer does new modes, but for some
	// reason doesn't want to use them and has sent us an Unacceptable Params ConfigRsp with
	// Basic in it.
	virtual TBool IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const;
	virtual TBool IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
										   const TRetransmissionAndFlowControlOption& aPeer) const;
	};

// This groups all mode-specific handlers and provides delegator methods that forward calls to
// appropriate FecHandlers.
NONSHARABLE_CLASS(TFecOptionHandlerDelegator)
	{
public:
	inline TBool IsOptionValid(const TRetransmissionAndFlowControlOption& aFecOption) const;
	inline TBool IsPeerResponseAcceptable(const TRetransmissionAndFlowControlOption& aPreferred,
										  const TRetransmissionAndFlowControlOption& aPeer) const;
	inline TBool BuildPositiveResponse(TRetransmissionAndFlowControlOption& aPreferred,
									   const TRetransmissionAndFlowControlOption& aPeer) const;
	inline void BuildNegativeResponse(TRetransmissionAndFlowControlOption& aPreferred,
									  const TRetransmissionAndFlowControlOption& aPeer) const;
	inline void BuildRequestBasedOnUnacceptableParamsResponse(TRetransmissionAndFlowControlOption& aPreferred,
															  const TRetransmissionAndFlowControlOption& aPeer,
															  const TL2CapFecNegotiator& aFecNegotiator) const;
	inline void PrepareImplicitPeerResponse(TRetransmissionAndFlowControlOption& aImplicitResponse,
											const TRetransmissionAndFlowControlOption& aPreferred) const;
	inline void SetMaxTransmit(TRetransmissionAndFlowControlOption& aFecOption, TUint8 aMaxTransmit) const;
	inline void ZeroUnspecifiedRequestFields(TRetransmissionAndFlowControlOption& aFecOption) const;
	inline void ZeroUnspecifiedResponseFields(TRetransmissionAndFlowControlOption& aFecOption) const;
private:
	TModeSpecificFecOptionHandlerBase& Handler(const TRetransmissionAndFlowControlOption& aFecOption);
	const TModeSpecificFecOptionHandlerBase& Handler(const TRetransmissionAndFlowControlOption& aFecOption) const;
	
	TModeSpecificFecOptionHandlerBase* ModeToHandler(TL2CapChannelMode aMode);
	const TModeSpecificFecOptionHandlerBase* ModeToHandler(TL2CapChannelMode aMode) const;
	
#ifdef __FLOG_ACTIVE
	void LogCurrentValues(const TRetransmissionAndFlowControlOption& aPreferred,
						  const TRetransmissionAndFlowControlOption& aPeer) const;
#endif
private:
	TStreamingFecHandler	iStreamingFecHandler;
	TErtmFecHandler			iErtmFecHandler;
	TLegacyFecHandler		iLegacyFecHandler;
	TBasicFecHandler		iBasicFecHandler;
	};

NONSHARABLE_CLASS(TL2CapSingleDirectionFecNegotiatorBase) : public TL2CapConfigurationOptionGroupBase
{
public:
	inline TL2CapSingleDirectionFecNegotiatorBase(const TL2CapFecNegotiator& aFecNegotiator);
	
	inline void SetPeer(const TRetransmissionAndFlowControlOption& aFecOption);
	inline void SetPreferred(const TRetransmissionAndFlowControlOption& aFecOption);

	void SetupForRenegotiation();
	
	inline TOptionConfigStatus ConfigOptionStatus() const;
	
	inline const TRetransmissionAndFlowControlOption& Peer() const;
	inline const TRetransmissionAndFlowControlOption& Preferred() const;


protected:
	TBool IsPeerModeAcceptable(TL2CapChannelMode aPeerMode, TBool& aDisconnect) const;

#ifdef __FLOG_ACTIVE
	void LogState() const;
#endif

protected:
	const TL2CapFecNegotiator&			iFecNegotiator;

	TOptionConfigStatus					iConfigStatus;
	TRetransmissionAndFlowControlOption iPeer;
	TRetransmissionAndFlowControlOption iPreferred;
	};

NONSHARABLE_CLASS(TL2CapIncomingFecNegotiator) : public TL2CapSingleDirectionFecNegotiatorBase
	{
public:
	inline TL2CapIncomingFecNegotiator(const TL2CapFecNegotiator& aFecNegotiator);
	void ProcessPeerValue(const TRetransmissionAndFlowControlOption& aFecOption,
						  TBool aIsUnacceptableParameters);
	void ProcessImplicitPeerValue();
	void DowngradeToBasic();
    void Setup();
protected:
    void BuildRequest(TL2CapChannelMode aMode, TRetransmissionAndFlowControlOption& aFecOption);
	};

NONSHARABLE_CLASS(TL2CapOutgoingFecNegotiator) : public TL2CapSingleDirectionFecNegotiatorBase
	{
public:
	inline TL2CapOutgoingFecNegotiator(const TL2CapFecNegotiator& aFecNegotiator);
	TInt ProcessPeerValue(const TRetransmissionAndFlowControlOption& aFecOption);
    void Setup();	
	inline TBool NeedToIncludeInPositiveConfigResponse();
	inline void SetIncludeInPositiveConfigResponse(TBool aVal);
private:
	TBool iIncludeValueInPositiveConfigResponse;
	};

// This is the interface to the FEC option negotiation functionality, through which
// all interaction with the external world should be done.
NONSHARABLE_CLASS(TL2CapFecNegotiator)
	{
public:
	// Variants of the negotiation procedure defined in 2.1 Core Spec Addendum 1,
	// chapter 5.4 Retransmission And Flow Control Option.
	enum TNegotiationBehavior
		{
		// Allows all spec-allowed fallback options: Streaming -> ERTM, ERTM -> Basic and
		// Streaming -> Basic. E.g. if our preferred mode is ERTM and peer proposes Basic,
		// we'll fall back to it. The negotiation procedure is a simple minimization
		// algorithm, where the party with the higher-precedence mode wins.
		// Also note that we try to apply the same hierarchy to RTM and FC, so effectively
		// you can read this as Unreliable -> Reliable, Reliable -> Basic and Unreliable -> Basic,
		// where Reliable = { ERTM, RTM } and Unreliable = { Streaming, FC }
		// For the precedence of modes, see channel mode <= operator or the spec, same section.
		EState1,
		// Like State1, only Unreliable -> Reliable is not allowed (Unreliable -> Basic and
		// Reliable -> Basic still are).
		EState1NoUnreliableToReliable,
		// No fallback - disconnect immediately if peer proposes a different channel mode than
		// we want.
		EState2
		};

public:
	inline TL2CapFecNegotiator();

	TBool Setup(TL2CapConfig::TChannelReliability aChannelReliability,
				TBool aLegacyModesDisallowed,
				const TL2CapEntityInfo& aPeerEntityConfig,
				TUint8 aMaxTransmit);

	// Can't renegotiate FEC currently but with Generic AMP time-outs will be renegotiable.
	void SetupForRenegotiation();

	// Methods below implement events in option state machine - they're identical
	// to the ones in the generic state machine in TL2CapConfigurationOptionGroup.
	inline TInt PeerRequestsOption(const TRetransmissionAndFlowControlOption& aFecOption);
	inline TInt PeerRequestsLastAcceptedValue();
	
	inline TInt PeerRejectsOption(const TRetransmissionAndFlowControlOption& aFecOption);
	inline TInt PeerAcceptsOption(const TRetransmissionAndFlowControlOption& aFecOption);
	inline void PeerAcceptsOption();

	inline TBool NeedToIncludeInPositiveConfigResponse();
	
	void DowngradeIncomingToBasicIfOutgoingIsBasic();
	TInt CheckNegotiatedChannelMode(TBool& aDowngrade);

	// [Methods below add a lot of fluff but they provide strong decoupling and encapsulation.
	// None of the internal classes are exposed.]
	
	inline const TRetransmissionAndFlowControlOption& IncomingPreferred() const;
	inline const TRetransmissionAndFlowControlOption& OutgoingPreferred() const;

	inline TL2CapConfigurationOptionGroupBase::TOptionConfigStatus IncomingConfigOptionStatus() const;
	inline TL2CapConfigurationOptionGroupBase::TOptionConfigStatus OutgoingConfigOptionStatus() const;

	inline TL2CapDataControllerConfig DataControllerConfig() const;

	// These abstract away the fact that some parameters are negotiated in opposite paths depending
	// on the modes used (eg Monitor TO), and which fields the values should be taken from (Peer or
	// Preferred).
	inline TL2CapChannelMode OutgoingLinkMode() const;
	inline TL2CapChannelMode IncomingLinkMode() const;
	inline TUint8 OutgoingLinkModeAsUnsignedByte() const;
	inline TUint8 IncomingLinkModeAsUnsignedByte() const;
	inline TUint8 OutgoingTxWindowSize() const;
	inline TUint8 IncomingTxWindowSize() const;
	inline TUint8 OutgoingMaxTransmit() const;
	inline TUint8 IncomingMaxTransmit() const;
	inline TUint16 OutgoingRetransmissionTimeout() const;
	inline TUint16 IncomingRetransmissionTimeout() const;
	inline TUint16 OutgoingMonitorTimeout() const;
	inline TUint16 IncomingMonitorTimeout() const;
	inline TUint16 OutgoingMaximumPDUSize() const;
	inline TUint16 IncomingMaximumPDUSize() const;

	// These are for the internal per-direction negotiator objects.
	TBool IsModeLegal(TL2CapChannelMode aPeerMode) const;
	inline const TL2CapEntityInfo& PeerSupportedModes() const;
	inline TNegotiationBehavior NegotiationBehavior() const;
	inline TL2CapChannelMode DesiredMode() const;
    inline TUint8 MaxTransmit() const;	
	inline const TFecOptionHandlerDelegator& ModeSpecificHandlers() const;
private:
	TL2CapIncomingFecNegotiator		iIncomingNegotiator;
	TL2CapOutgoingFecNegotiator		iOutgoingNegotiator;

	TL2CapEntityInfo				iPeerSupportedModes;
	TNegotiationBehavior			iNegotiationBehavior;
	TL2CapChannelMode				iDesiredMode;
    TUint8                          iMaxTransmit;
    
	TFecOptionHandlerDelegator		iModeSpecificHandlers;
	};

#include "L2CapFecNegotiator.inl"

#endif /*L2CAPFECNEGOTIATOR_H*/
