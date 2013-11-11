// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Manages the negotiation of L2Cap configuration parameters.
// 
//

#include <bluetooth/logger.h>
#include <es_prot.h>
#include "l2util.h"
#include "L2CapChannelConfig.h"
#include "l2capEntityConfig.h"
#include "l2capSigPacketConfigure.h"

#ifndef __UNITTest__
    #include "l2constants.h"
	#include "l2capSAPSignalHandler.h"
#else
	#include "cl2capconfigproxy.h"
	#ifdef _DEBUG
	#include "l2capDebugControlInterface.h"
	#endif
#endif

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif


// A helper for the implementation of a workaround for a bug in the negotiation algorithm
// of some remotes.
NONSHARABLE_STRUCT(SL2CapChannelIncomingConfigOptionSnapshot)
    {
    SL2CapChannelIncomingConfigOptionSnapshot(const CL2CapChannelConfig& aChannelConfig);
    TBool operator==(const SL2CapChannelIncomingConfigOptionSnapshot& aThat);

    TMTUOption                          iMtu;
    TL2CapConfigurationOptionGroupBase::TOptionConfigStatus iMtuStatus;

    TFlushTimeoutDurationOption         iFlushTimeout;
    TL2CapConfigurationOptionGroupBase::TOptionConfigStatus iFlushTimeoutStatus;

    TRetransmissionAndFlowControlOption iFec;
    TL2CapConfigurationOptionGroupBase::TOptionConfigStatus iFecStatus;

    TQualityOfServiceOption             iQos;
    TL2CapConfigurationOptionGroupBase::TOptionConfigStatus iQosStatus;
    };


/*static*/ CL2CapChannelConfig* CL2CapChannelConfig::NewL(CL2CapSAPSignalHandler& aSAPSignalHandler)
	{
	LOG_STATIC_FUNC
	CL2CapChannelConfig* self = new(ELeave) CL2CapChannelConfig(aSAPSignalHandler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CL2CapChannelConfig::~CL2CapChannelConfig()
	{
	LOG_FUNC
	delete iIncomingMTU;
	delete iOutgoingMTU;
	delete iIncomingFlushTimeoutDuration;
	delete iOutgoingFlushTimeoutDuration;
	delete iIncomingQOS;
	delete iOutgoingQOS;
	}
	
void CL2CapChannelConfig::ConstructL()
	{
	LOG_FUNC

	// Initialise the configuration options.
	iIncomingMTU = new (ELeave)TL2CapConfigurationOptionGroup<TMTUOption>
	           (TMTUOption::EPreferredValue,
				TMTUOption::EAbsoluteMinimumValue,
				TMTUOption::EPreferredValue,
				TMTUOption::ESpecDefaultValue,
				TL2CapConfigurationOptionGroupBase::ENegotiateToMinimum);
	iOutgoingMTU = new (ELeave)TL2CapConfigurationOptionGroup<TMTUOption>
	           (TMTUOption::EMaximumValue,
				TMTUOption::ESpecMinimumValue,
			    TMTUOption::EPreferredValue,
			    TMTUOption::ESpecDefaultValue,
				TL2CapConfigurationOptionGroupBase::ENegotiateToMinimum);

	// Flush timeout is settable in the SAP but not implemented so we're not looking at the SAP value.
	iIncomingFlushTimeoutDuration = new (ELeave)TL2CapConfigurationOptionGroup<TFlushTimeoutDurationOption>
	           (TFlushTimeoutDurationOption::EMaximumValue,
			    TFlushTimeoutDurationOption::ESpecMinimumValue,
	            TFlushTimeoutDurationOption::ESpecDefaultValue,
	            TFlushTimeoutDurationOption::ESpecDefaultValue,
				TL2CapConfigurationOptionGroupBase::ENegotiated);
	iOutgoingFlushTimeoutDuration = new (ELeave)TL2CapConfigurationOptionGroup<TFlushTimeoutDurationOption>
               (TFlushTimeoutDurationOption::EMaximumValue,
				TFlushTimeoutDurationOption::EMaximumValue,
				TFlushTimeoutDurationOption::ESpecDefaultValue,
				TFlushTimeoutDurationOption::ESpecDefaultValue,
				TL2CapConfigurationOptionGroupBase::ENegotiated);

	// QOS isn't implemented nor settable in the SAP.
	iIncomingQOS = new (ELeave)TL2CapConfigurationOptionGroup<TQualityOfServiceOption>
	           (TQualityOfServiceOption::EMaximumValue,
				TQualityOfServiceOption::ESpecMinimumValue,
				TQualityOfServiceOption::ESpecDefaultValue,
				TQualityOfServiceOption::ESpecDefaultValue,
				TL2CapConfigurationOptionGroupBase::ENegotiated);
	iOutgoingQOS = new (ELeave)TL2CapConfigurationOptionGroup<TQualityOfServiceOption>
	           (TQualityOfServiceOption::EMaximumValue,
				TQualityOfServiceOption::ESpecMinimumValue,
				TQualityOfServiceOption::ESpecDefaultValue,
				TQualityOfServiceOption::ESpecDefaultValue,
				TL2CapConfigurationOptionGroupBase::ENegotiated);

	iRequestedChannelReliability		= TL2CapConfig::EReliableChannel;
	iLegacyModesDisallowed				= EFalse;
	iRequestedFlushTimeout				= TL2CapConfig::EDefaultDataObsolescenceTimeout;
	iRequestedRetransmissionTimeout		= TL2CapConfig::EDefaultRetransmission;

	LOG2(_L("CL2CapChannelConfig.iFecNegotiator = %X.%X"), (TAny*)this, (TAny*)&iFecNegotiator)
	}


#ifndef __UNITTest__
CL2CapChannelConfig::CL2CapChannelConfig(CL2CapSAPSignalHandler& aSAPSignalHandler)
 : iSAPSignalHandler(aSAPSignalHandler)
#else
CL2CapChannelConfig::CL2CapChannelConfig(CL2CapConfigProxy& aSAPSignalHandler)
 : iSAPSignalHandler(aSAPSignalHandler)
#endif	
	{
	LOG_FUNC
	}

void CL2CapChannelConfig::CloneChannelConfig(const CL2CapChannelConfig& aCopy)
	{
	LOG_FUNC

	// Copy all aspects of this class that should be cloned from the listening SAP.
	*iIncomingMTU 	= *(aCopy.iIncomingMTU);
	*iOutgoingMTU 	= *(aCopy.iOutgoingMTU);
	*iIncomingFlushTimeoutDuration 	= *(aCopy.iIncomingFlushTimeoutDuration);
	*iOutgoingFlushTimeoutDuration 	= *(aCopy.iOutgoingFlushTimeoutDuration);
	*iOutgoingQOS 	= *(aCopy.iOutgoingQOS);
	*iIncomingQOS 	= *(aCopy.iIncomingQOS);
	
	iRequestedChannelReliability	= aCopy.iRequestedChannelReliability;
	iLegacyModesDisallowed			= aCopy.iLegacyModesDisallowed;
	iRequestedFlushTimeout			= aCopy.iRequestedFlushTimeout;
	iRequestedRetransmissionTimeout	= aCopy.iRequestedRetransmissionTimeout;
	// iFecNegotiator gets updated using the iRequested... field values on OpenChannelRequest. 
	}


TInt CL2CapChannelConfig::HandleConfigRequest(HConfigureRequest& aConfigRequest, RMBufChain& aUnknownOptions)
	{
	LOG_FUNC
	// Pile these options on top of the previously received ones in case it's
	// a continuation packet. They're all processed once we've received the
	// whole transaction, in ConfigRequestComplete().
	return aConfigRequest.ParseOptions(iReceivedConfigRequestOptions, aUnknownOptions);
	}

TInt CL2CapChannelConfig::HandleConfigResponse(HConfigureResponse& aConfigResponse, RMBufChain& aUnknownOptions)
	{
	LOG_FUNC
    // Note: If it's a positive response (Success), then any parameters not specified by
    // the peer are assumed to be as requested.
    // If it's negative then it only includes suggested values for rejected parameters
    // and the ones not rejected will have to be re-requested in following message exchanges.

    TL2CapConfigParamOptions options;
    TInt err = aConfigResponse.ParseOptions(options, aUnknownOptions);
    if (err)
        {
        return err;
        }

    const TBool isUnacceptableParameters = (aConfigResponse.Results() == EConfigUnacceptableParams);
    const TBool isSuccess = (aConfigResponse.Results() == EConfigSuccess);

    // Prelude to the negotiation problem described at the bottom:
    // snapshot current option status for comparison after the message is processed.
    SL2CapChannelIncomingConfigOptionSnapshot optionStatusBeforeProcessing(*this);
    
    // MTU
    if (options.IsMtuSet())
        {
        if (isUnacceptableParameters)
            {
            iIncomingMTU->PeerRejectsOption(options.Mtu());
            }
        else
            {
            iIncomingMTU->PeerAcceptsOption(options.Mtu());
            }
        }
    else if (isSuccess)
        {
        // A positive response acknowledges all options - if one is not
        // included then it means that the peer agrees with our proposal
        // (or the default, or the last accepted value).
        iIncomingMTU->PeerAcceptsOption();
        }
    // [else]: if an option is not explicitly mentioned and it's not
    // a positive response, then the option's status remains untouched.

    // Flush Timeout Duration
    if (options.IsFlushTimeoutSet())
        {
        if (isUnacceptableParameters)
            {
            iOutgoingFlushTimeoutDuration->PeerRejectsOption(options.FlushTimeout());
            }
        else
            {
            // Note: we're ignoring the explicitly set value as it can only
            // be what we sent, otherwise it's a broken/malicious peer and we don't
            // want to use the value.
            iOutgoingFlushTimeoutDuration->PeerAcceptsOption();
            }
        }
    else if (isSuccess)
        {
        iOutgoingFlushTimeoutDuration->PeerAcceptsOption();
        }

    // Quality Of Service
    if (options.IsQosSet())
        {
        if (isUnacceptableParameters)
            {
            iOutgoingQOS->PeerRejectsOption(options.Qos());
            }
        else
            {
            iOutgoingQOS->PeerAcceptsOption(options.Qos());
            }
        }
    else if (isSuccess)
        {
        iOutgoingQOS->PeerAcceptsOption();
        }

    // Retransmission And Flow Control
    if (options.IsFecSet())
        {
        if (isUnacceptableParameters)
            {
            err = iFecNegotiator.PeerRejectsOption(options.Fec());
            }
        else
            {
            err = iFecNegotiator.PeerAcceptsOption(options.Fec());
            }
        }
    else if (isSuccess)
        {
        iFecNegotiator.PeerAcceptsOption();
        }

    if (isUnacceptableParameters && !err)
        {
        // negotiation algorithm bug workaround.

        // The problem this tries to work around is as follows:
        // S - local Symbian entity, C - remote host
        // SDP connection is being opened, C supports (E)RTM so we try to negotiate it:
        // 1. S sends ConfigReq[FEC=ERTM, MTU=n].
        // 2. C responds with ConfigRsp(UnacceptableParameters)[MTU=n].
        // 3. S obeys the reject, thinking C is rejecting the MTU (note the MTU value proposed
        //    by C is the same value we sent in (1));
        //    S sends a new ConfigReq, including the proposed MTU and the FEC which is still pending
        //    a response, thus creating a ConfigReq identical to the one from (1).
        // 4. C responds like in (2).
        // 5. S reacts like in (3).
        // -- we got into an infinite loop --
        // The whole situation is due to C in (2) responding with ConfigRsp(UnacceptableParameters)[MTU=n]
        // while what it really means is ConfigRsp(UnacceptableParameters)[FEC=Basic].
        // This was found at UPF33 - June 2009 and they fixed their code then, but there still
        // might be a lot of devices out doing the wrong thing.
        //
        // The workaround is to snapshot option status before and after the peer's
        // Config Response is processed and compare it. If it's exactly the same then
        // we're in a stalemate, so we cross our fingers and then drop our FEC proposal
        // to Basic. Remote accepts the ConfigReq with Basic mode and the whole palaver is over.
        // (the status snapshot includes the Preferred value and the config status of all
        // options).
        SL2CapChannelIncomingConfigOptionSnapshot optionStatusAfterProcessing(*this);
        if (optionStatusBeforeProcessing == optionStatusAfterProcessing)
            {
            err = iFecNegotiator.PeerRejectsOption(TRetransmissionAndFlowControlOption());
            // Note: we'll also fall here if an option has started off with status Failed and
            // the peer rejected our preferred value, suggesting a value that's again unacceptable.
            // Example: user requests incoming MTU = 500 which is less than 672, which means
            // the default peer value of 672 is unacceptable and so the option's status is Failed
            // by default. If the value of 500 is then rejected by the peer, and it suggests
            // something else, be it 672 or anything > 500, the status and the actual value will
            // remain Failed and 500 - so if the status of the other options hasn't changed,
            // we'll fall in here. It's harmless though because it means we've failed to agree
            // and will disconnect, so it doesn't matter whether the FEC value is changed.
            }
        }
    return err;
    }

TInt CL2CapChannelConfig::ConfigRequestComplete(TBool aConfigRequestSent, TConfigResponseResult& aResult)
    {
    LOG_FUNC
    TInt err = KErrNone;

	// Note that this method applies a different interpretation of the spec than
	// HandleConfigResponse. This is due to an ambiguity in the spec which yields two
	// possible interpretations:
	// (Interpretation A) When handling a Config Response, we assume that:
	// (1) if it's positive, then it acknowledges all the options we've sent in Config Request;
	// (2) if it's negative, then it only rejects the options that it includes, and neither
	//     acknowledges nor rejects the ones that are not included.
	// (Interpretation B) When responding to a Config Request, we effectively assume
	// the following:
	// (1) is the same,
	// (2) if it's negative, then it only rejects the options that it includes and acknowledges
	//     the ones that are not included.
	// There are implementations using both interpetations, so we do different things depending
	// on negotiation direction for maximum interop - we assume that interpretation A taken in
	// HandleConfigResponse makes more sense, but to be interoperable with both algorithms, we
	// respond to a ConfigRequest using interpretation B.
	// Here is why, by example:
	// 1. Peer sends us Config Request with MTU and FEC;
	// 2. We reject the MTU but FEC is ok, so we send a Config Response [Unacceptable Parameters](MTU);
	//    According to interpretation B we also accept the FEC, while interpretation A
	//    means we ignore it and Peer will include it again in the following ConfigRequest.
	// 3a. If Peer implements interpretation B, then it assumes we've accepted the FEC
	//    and considers the option value to have been agreed, so it will not include it
	//    in the next Config Request along with MTU. Fortunately, we use the same
	//    interpretation here so we're in sync.
	// 3b. If Peer implements interpretation A, then it will include the FEC in the next
	//    Config Request anyway because it's still outstanding, so what we do to the option
	//    now doesn't matter. Theoretically Peer could suddenly change its mind and forego
	//    negotiating FEC, but obviously unless it's a human no implemntation will do it :)

    if (iReceivedConfigRequestOptions.IsMtuSet())
        {
        iOutgoingMTU->PeerRequestsOption(iReceivedConfigRequestOptions.Mtu());
        }
    else
        {
        iOutgoingMTU->PeerRequestsLastAcceptedValue();
        }

    if (iReceivedConfigRequestOptions.IsFlushTimeoutSet())
        {
        iIncomingFlushTimeoutDuration->PeerRequestsOption(iReceivedConfigRequestOptions.FlushTimeout());
        }
    else
        {
        iIncomingFlushTimeoutDuration->PeerRequestsLastAcceptedValue();
        }

    if (iReceivedConfigRequestOptions.IsQosSet())
        {
        iIncomingQOS->PeerRequestsOption(iReceivedConfigRequestOptions.Qos());
        }
    else
        {
        iIncomingQOS->PeerRequestsLastAcceptedValue();
        }

    if (iReceivedConfigRequestOptions.IsFecSet())
        {
        err = iFecNegotiator.PeerRequestsOption(iReceivedConfigRequestOptions.Fec());
        }
    else
        {
        err = iFecNegotiator.PeerRequestsLastAcceptedValue();
        if (err == KErrNone)
            {
            // Check if the outgoing FEC connection needs to be downgraded to basic mode.
            // NB This ONLY makes sense if our config request has not already been queued...
            //    ...otherwise it would break the config algorithm.
            if(!aConfigRequestSent)
                {
                FecNegotiator().DowngradeIncomingToBasicIfOutgoingIsBasic();
                }
            }
        }

    // Now reset the Config Request option store in case there's a next negotiation round.
    iReceivedConfigRequestOptions = TL2CapConfigParamOptions();
    
    /*
    For a config request we know the peer values - so the peer and the actual values should
    either be the same, or the peer should be outside our limits (See the implementation of
    TL2CapConfigurationOptionGroupBase::TOptionConfigStatus.)
    If this __ASSERT_DEBUG is hit, it is likely that the API has been upgraded or
    the default values have been upgraded in a way which breaks the configuration logic.
    */
    __ASSERT_DEBUG(iOutgoingMTU->ConfigOptionStatus() != TL2CapConfigurationOptionGroupBase::EOptionConfigOutstanding &&
                   iIncomingFlushTimeoutDuration->ConfigOptionStatus() != TL2CapConfigurationOptionGroupBase::EOptionConfigOutstanding &&
                   iIncomingQOS->ConfigOptionStatus() != TL2CapConfigurationOptionGroupBase::EOptionConfigOutstanding &&
                   iFecNegotiator.OutgoingConfigOptionStatus() != TL2CapConfigurationOptionGroupBase::EOptionConfigOutstanding,
                   Panic(EL2CAPInvalidConfigOptionState));

    // Check the response code required for the subsequent Config Response.
    if(iOutgoingMTU->ConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed ||
       iIncomingFlushTimeoutDuration->ConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed ||
       iIncomingQOS->ConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed ||
       iFecNegotiator.OutgoingConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed)
        {
        aResult = EConfigUnacceptableParams;
        }
    else
        {
        aResult = EConfigSuccess;
        }

    return err;
    }

/**	
	Process the entity information received from peer. Used to make sure that we don't try
	and negotiate a configuration that the peer is not capable of negotiating.  The predominant use
	case is to make sure that we do not negotiate FEC parameters with a device that is only capable
	of basic mode.
**/
TInt CL2CapChannelConfig::UpdateLocalConfigWithEntityCapabilities()
	{
	LOG_FUNC
	TL2CapEntityInfo config; 
	TInt err = iL2CapEntityConfig->PeerL2CapSupportedFeatures(config);
	if (err == KErrNone)
		{
		// Signalling state machine is responsible for not calling us until we've got
		// peer entity information.
		__ASSERT_DEBUG(config.LinkInfoState() == EL2CapEntityInfoDefined,
					   Panic(EL2CAPUpdateLocalConfigCalledWithoutPeerEntityConfig));
		// Based on knowledge about the peer's extended information, set up mode negotiator.
		TBool peerSupportsRequestedConfig = GenerateFecOptions(config);
		if (!peerSupportsRequestedConfig)
			{
			err = KErrL2CAPPeerDoesNotSupportRequestedChannelMode;
			}
		}
	return err;
	}

/**	
	Handles configurable options provided in TL2CapConfig by user APIs.
	aOnTheAirConfigRequired will be set to True if the specified parameters require the
	L2CAP reconfiguration process, False otherwise.
**/
TInt CL2CapChannelConfig::UpdateConfigAPIChange(const TL2CapConfig& aApiConfig, TBool& aOnTheAirReconfigRequired)
	{
	LOG_FUNC
	TInt err = KErrNone;

	aOnTheAirReconfigRequired = EFalse;

	err = UpdateReliability(aApiConfig);
	if (err == KErrNone)
		{
		err = UpdateMtuMru(aApiConfig, aOnTheAirReconfigRequired);
		}

	// GenerateFecOptions() will be called on OpenChannelRequest to transform these options
	// into concrete FEC option values groups.

	return err;
	}


/**	
	Returns true if all the configuration elements included in a incoming configuration response
	have been successfully configured
**/
CL2CapChannelConfig::TChannelConfigStatus CL2CapChannelConfig::LocalConfigurationStatus() const	
	{
	LOG_FUNC
	TChannelConfigStatus status = EChannelConfigOutstanding;
	
	if((iIncomingMTU->ConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigComplete) &&
	   (iOutgoingFlushTimeoutDuration->ConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigComplete) &&
	   (iOutgoingQOS->ConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigComplete) &&
	   (iFecNegotiator.IncomingConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigComplete))
		{
		status = EChannelConfigComplete;
		}
	else
		{
		if((iIncomingMTU->ConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed) ||
		   (iOutgoingFlushTimeoutDuration->ConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed) ||
		   (iOutgoingQOS->ConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed) ||
		   (iFecNegotiator.IncomingConfigOptionStatus() == TL2CapConfigurationOptionGroupBase::EOptionConfigFailed))
			{
			status = EChannelConfigFailed;
			}
		}
	return status;
	}

/**	
	Public function that registers the entity channel configuration with this channel configuration
**/
void CL2CapChannelConfig::RegisterL2CapEntityConfig(TL2CAPEntityConfig& aL2CapEntityConfig)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iL2CapEntityConfig, Panic(EL2CAPAttemptToRegisterEntityConfigTwice));
	iL2CapEntityConfig = &aL2CapEntityConfig;
	}	

/**
	Channel configuration is being taken down. 
**/
void CL2CapChannelConfig::DetachChannelConfig()
	{
	LOG_FUNC
	// If the channel config has registered with the Entity
	// config remove the association.
	iL2CapEntityConfig = NULL;
	}

// Translates abstract 'Channel Reliability' etc. into concrete option groups.
// Called just-in-time on OpenChannelRequest, when the L2CAP connection is being made.
// Should only be called once in the lifetime of a connection.
TBool CL2CapChannelConfig::GenerateFecOptions(const TL2CapEntityInfo& aPeerEntityConfig)
	{
	LOG_FUNC

	__ASSERT_DEBUG(aPeerEntityConfig.LinkInfoState() == EL2CapEntityInfoDefined,
				   Panic(EL2CAPFecConfigAttemptWithoutPeerInfo));

	// We may end up falling back from Streaming to ERTM, so set up a sensible MaxTransmit value
	// for both Reliable and Unreliable case.
	TUint8 maxTransmit = TRetransmissionAndFlowControlOption::KMaxValidEnhancedNumberTransmit;
	// TL2CapConfig::EDefaultRetransmission is 0xffff, which is a special value in the API that
	// means 'maximum possible MaxTransmit'. Note that we assume here that the maximum MaxTransmit
	// is 0, which is only true for enhanced modes. If a legacy mode is negotiated, FECNegotiator
	// will change the 0 to 255.
	if (iRequestedRetransmissionTimeout != TL2CapConfig::EDefaultRetransmission)
		{
		maxTransmit = static_cast<TUint8>(iRequestedRetransmissionTimeout / TRetransmissionAndFlowControlOption::KDefaultRetransmissionTimeout);
		}

	TBool peerSupportsRequestedConfig = iFecNegotiator.Setup(iRequestedChannelReliability, iLegacyModesDisallowed, aPeerEntityConfig, maxTransmit);
	if (peerSupportsRequestedConfig)
		{
		// Set the outgoing flush parameters NOT to allow flushing.
		// Note: the TL2CapConfig API does allow setting of the flush timeout, and we do have some
		// support for it across the board, but that code is unused. This is the place where
		// iRequestedFlushTimeout gets ignored and if ever start supporting flushing, then we should
		// take it into account and set the flushTimeoutDuration accordingly if unreliable channel
		// is requested.
		TFlushTimeoutDurationOption flushTimeoutDuration(TFlushTimeoutDurationOption::EMaximumValue);
		iOutgoingFlushTimeoutDuration->SetRequiredValue(flushTimeoutDuration, flushTimeoutDuration, flushTimeoutDuration);
		}

	return peerSupportsRequestedConfig;
	}

TInt CL2CapChannelConfig::UpdateReliability(const TL2CapConfig& aApiConfig)
	{
	LOG_FUNC
	TInt err = KErrNone;
	TBool specified = EFalse;
	TL2CapConfig::TChannelReliability reliability = aApiConfig.ChannelReliability(specified);
	if (specified)
		{
		if (iSAPSignalHandler.IsChannelClosed())
			{
			TBool getFlushTimeout = EFalse;
			TBool getRtxTimeout = EFalse;
	
			if (reliability == TL2CapConfig::EUnreliableDesiredChannel)
				{
				getFlushTimeout = getRtxTimeout = ETrue;
				}
			else if (reliability == TL2CapConfig::EUnreliableChannel)
				{
				getFlushTimeout = ETrue;
				}
			else // Reliable
				{
				getRtxTimeout = ETrue;
				}
	
			TUint16 timeout = 0;
	
			if (getFlushTimeout)
				{
				timeout = aApiConfig.ObsolescenceTimer(specified);
				if (!specified)
					{
					err = KErrArgument;
					}
				else
					{
					iRequestedFlushTimeout = timeout;
					}
				}
			
			if (getRtxTimeout)
				{
				timeout = aApiConfig.RetransmissionTimer(specified);
				if (!specified)
					{
					err = KErrArgument;
					}
				else
					{
					iRequestedRetransmissionTimeout = timeout;
					}
				}
	
			if (err == KErrNone)
				{
				iRequestedChannelReliability = reliability;
				iLegacyModesDisallowed = aApiConfig.LegacyModesDisallowed();
		
				if (iRequestedFlushTimeout < KMinOutgoingFlushTimeout)
					{
					iRequestedFlushTimeout = KMinOutgoingFlushTimeout;
					}
				}
			} // channel Closed
		else
			{
			// Channel not in Closed state - can't change reliability.
			err = KErrInUse;
			}
		} // reliability specified
	return err;
	}

TInt CL2CapChannelConfig::UpdateMtuMru(const TL2CapConfig& aApiConfig, TBool& aOnTheAirReconfigRequired)
	{
	LOG_FUNC

	// MTU-related params.
	TBool mtuSpecified = EFalse;
	TBool minMtuSpecified = EFalse;
	TUint16 newMTU = aApiConfig.MaxTransmitUnitSize(mtuSpecified);
	TUint16 newMinMTU = aApiConfig.MinMTU(minMtuSpecified);
	TMTUOption currentMTU = iOutgoingMTU->Preferred();

	// MRU-related params.
	TBool mruSpecified = EFalse;
	TBool minMruSpecified = EFalse;
	TUint16 newMRU = aApiConfig.MaxReceiveUnitSize(mruSpecified);
	TUint16 newMinMRU = aApiConfig.MinMRU(minMruSpecified);
	TMTUOption currentMRU = iIncomingMTU->Preferred();

	if ((mtuSpecified || minMtuSpecified || mruSpecified || minMruSpecified) &&
		!iSAPSignalHandler.IsChannelClosed() && !iSAPSignalHandler.IsChannelOpen())
		{
		// Not the brightest idea to mess with these during ongoing config.
		return KErrInUse;
		}
	else
		{
		if (mtuSpecified || minMtuSpecified)
			{
			TMTUOption mtuToSet = mtuSpecified ? newMTU : currentMTU;
			// if !minMtuSpecified then assume the lower bound to be = newMTU
			TMTUOption minMtuToSet = minMtuSpecified ? newMinMTU : newMTU;

			if (minMtuToSet > mtuToSet)
				{
				return KErrL2CAPAttemptToSetMinMtuGreaterThanMtu;
				}

			iOutgoingMTU->SetRequiredValue(TMTUOption(TMTUOption::EMaximumValue), minMtuToSet, mtuToSet);
			aOnTheAirReconfigRequired = ETrue;
			}

		if (mruSpecified || minMruSpecified)
			{
			TMTUOption mruToSet = mruSpecified ? newMRU : currentMRU;
			 // if !minMruSpecified then assume the lower bound is the spec minimum
			TMTUOption minMruToSet = minMruSpecified ? newMinMRU : TMTUOption(TMTUOption::ESpecMinimumValue);

			if (minMruToSet > mruToSet)
				{
				return KErrL2CAPAttemptToSetMinMruGreaterThanMru;
				}

			iIncomingMTU->SetRequiredValue(mruToSet, minMruToSet, mruToSet);
			aOnTheAirReconfigRequired = ETrue;
			}
		}
	return KErrNone;
	}

TInt CL2CapChannelConfig::GetChannelMode(TL2CapChannelMode& aMode) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(iFecNegotiator.IncomingLinkMode() == iFecNegotiator.OutgoingLinkMode(),
				   Panic(EL2CAPAsymmetricChannelModes));
	aMode = iFecNegotiator.IncomingLinkMode();
	return KErrNone;
	}


SL2CapChannelIncomingConfigOptionSnapshot::SL2CapChannelIncomingConfigOptionSnapshot(const CL2CapChannelConfig& aChannelConfig)
 :  iMtu(aChannelConfig.IncomingMTU().Preferred()),
    iMtuStatus(aChannelConfig.IncomingMTU().ConfigOptionStatus()),
    iFlushTimeout(aChannelConfig.OutgoingFlushTimeout().Preferred()),
    iFlushTimeoutStatus(aChannelConfig.OutgoingFlushTimeout().ConfigOptionStatus()),
    iFec(aChannelConfig.FecNegotiator().IncomingPreferred()),
    iFecStatus(aChannelConfig.FecNegotiator().IncomingConfigOptionStatus()),
    iQos(aChannelConfig.OutgoingQOS().Preferred()),
    iQosStatus(aChannelConfig.OutgoingQOS().ConfigOptionStatus())
    {
    LOG_FUNC
    }

TBool SL2CapChannelIncomingConfigOptionSnapshot::operator==(const SL2CapChannelIncomingConfigOptionSnapshot& aThat)
    {
    LOG_FUNC
    return iMtu == aThat.iMtu                   && iMtuStatus == aThat.iMtuStatus &&
           iFlushTimeout == aThat.iFlushTimeout && iFlushTimeoutStatus == aThat.iFlushTimeoutStatus &&
           iFec == aThat.iFec                   && iFecStatus == aThat.iFecStatus &&
           iQos == aThat.iQos                   && iQosStatus == aThat.iQosStatus;
    }
