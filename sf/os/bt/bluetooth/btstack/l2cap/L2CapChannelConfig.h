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
// Defines the L2CAP Channel Config class
// 
//

#ifndef L2CAPCHANNELCONFIG_H
#define L2CAPCHANNELCONFIG_H

#include <bt_sock.h>
#include <bttypes.h>

#include "L2types.h"
#include "L2CapFecNegotiator.h"
#include "l2capEntityConfig.h"
#include "l2capSigPacketConfigure.h"

// Forward declarations
class TL2CAPEntityConfig;
class HConfigureRequest;
class HConfigureResponse;
class RMBufChain;

#ifndef __UNITTest__
	class CL2CapSAPSignalHandler;
#else
	class CL2CapConfigProxy;
#endif

NONSHARABLE_CLASS(CL2CapChannelConfig) : public CBase
	{
public:
	enum TChannelConfigStatus
		{
		EChannelConfigOutstanding,
		EChannelConfigComplete,
		EChannelConfigFailed,
		};

	// Configuration constants
	const static TUint16 KMinOutgoingFlushTimeout = TL2CapConfig::EMinDataObsolescenceTimeout; //min physically possible
	~CL2CapChannelConfig();
	
	static CL2CapChannelConfig* NewL(CL2CapSAPSignalHandler& aSAPSignalHandler);

	TInt HandleConfigRequest(HConfigureRequest& aConfigRequest, RMBufChain& aUnknownOptions);
	TInt HandleConfigResponse(HConfigureResponse& aConfigResponse, RMBufChain& aUnknownOptions);
	// This does the actual handling and sends a response to a whole Config Request "transaction",
	// so that segmented config requests are correctly handled.
	TInt ConfigRequestComplete(TBool aConfigRequestSent, TConfigResponseResult& aResult);

	void CloneChannelConfig(const CL2CapChannelConfig& aCL2CapChannelConfig);
	TInt UpdateLocalConfigWithEntityCapabilities();

	TInt UpdateConfigAPIChange(const TL2CapConfig& aAPIConfig, TBool& aOnTheAirReconfigRequired);

	void RegisterL2CapEntityConfig(TL2CAPEntityConfig& aL2CapEntityConfig);
	void DetachChannelConfig();
	inline TBool IsPeerInfoDefined();

	TChannelConfigStatus LocalConfigurationStatus() const;

	TInt GetChannelMode(TL2CapChannelMode& aMode) const;
	
	// Accessors for configuration options.
	inline TL2CapConfigurationOptionGroup<TMTUOption>& IncomingMTU() const;
	inline TL2CapConfigurationOptionGroup<TMTUOption>& OutgoingMTU() const;
	inline TL2CapConfigurationOptionGroup<TFlushTimeoutDurationOption>& IncomingFlushTimeout() const;
	inline TL2CapConfigurationOptionGroup<TFlushTimeoutDurationOption>& OutgoingFlushTimeout() const;
	inline TL2CapConfigurationOptionGroup<TQualityOfServiceOption>& IncomingQOS() const;
	inline TL2CapConfigurationOptionGroup<TQualityOfServiceOption>& OutgoingQOS() const;
	inline TL2CapFecNegotiator& FecNegotiator();
	inline const TL2CapFecNegotiator& FecNegotiator() const;

private:
#ifndef __UNITTest__
	CL2CapChannelConfig(CL2CapSAPSignalHandler& aSAPSignalHandler);
#else
	CL2CapChannelConfig(CL2CapConfigProxy& aSAPSignalHandler);
#endif	
	void ConstructL();

	TBool GenerateFecOptions(const TL2CapEntityInfo& aPeerEntityConfig);
	TInt UpdateReliability(const TL2CapConfig& aApiConfig);
	TInt UpdateMtuMru(const TL2CapConfig& aApiConfig, TBool& aOnTheAirReconfigRequired);

private:
	// Configuration members.
	// MTU / MRU
	TL2CapConfigurationOptionGroup<TMTUOption>* iIncomingMTU;
	TL2CapConfigurationOptionGroup<TMTUOption>* iOutgoingMTU;

	// Incoming and outgoing Flush timers.
	TL2CapConfigurationOptionGroup<TFlushTimeoutDurationOption>* iIncomingFlushTimeoutDuration;
	TL2CapConfigurationOptionGroup<TFlushTimeoutDurationOption>* iOutgoingFlushTimeoutDuration;

	// Incoming and outgoing QoS
	TL2CapConfigurationOptionGroup<TQualityOfServiceOption>* iIncomingQOS;
	TL2CapConfigurationOptionGroup<TQualityOfServiceOption>* iOutgoingQOS;

	// Incoming and outgoing FEC
	TL2CapFecNegotiator iFecNegotiator;

#ifndef	 __UNITTest__
	CL2CapSAPSignalHandler& 	iSAPSignalHandler;
#else
	CL2CapConfigProxy& 			iSAPSignalHandler;
#endif
	TL2CAPEntityConfig*	 		iL2CapEntityConfig;  // Non-owned.

	TL2CapConfig::TChannelReliability		iRequestedChannelReliability;
	TBool									iLegacyModesDisallowed;
	TUint16									iRequestedFlushTimeout;
	TUint16									iRequestedRetransmissionTimeout;

	// Thanks to the possibility of segmented ConfigRequests, we need
	// this to collect all the options as Config Request segments flow in.
	// Then once the whole ConfigRequest transaction is received, we process this.
	TL2CapConfigParamOptions				iReceivedConfigRequestOptions;
	};


// Accessors for configuration options.
inline TL2CapConfigurationOptionGroup<TMTUOption>& CL2CapChannelConfig::IncomingMTU() const
	{
	return *iIncomingMTU;
	}
	
inline TL2CapConfigurationOptionGroup<TMTUOption>& CL2CapChannelConfig::OutgoingMTU() const
	{
	return *iOutgoingMTU;
	}

inline TL2CapConfigurationOptionGroup<TFlushTimeoutDurationOption>& CL2CapChannelConfig::IncomingFlushTimeout() const
	{
	return *iIncomingFlushTimeoutDuration;
	}

inline TL2CapConfigurationOptionGroup<TFlushTimeoutDurationOption>& CL2CapChannelConfig::OutgoingFlushTimeout() const
	{
	return *iOutgoingFlushTimeoutDuration;
	}

inline TL2CapConfigurationOptionGroup<TQualityOfServiceOption>& CL2CapChannelConfig::IncomingQOS() const
	{
	return *iIncomingQOS;
	}

inline TL2CapConfigurationOptionGroup<TQualityOfServiceOption>& CL2CapChannelConfig::OutgoingQOS() const
	{
	return *iOutgoingQOS;
	}

inline TL2CapFecNegotiator& CL2CapChannelConfig::FecNegotiator()
	{
	return iFecNegotiator;
	}

inline const TL2CapFecNegotiator& CL2CapChannelConfig::FecNegotiator() const
    {
    return iFecNegotiator;
    }

inline TBool CL2CapChannelConfig::IsPeerInfoDefined()
	{
	return iL2CapEntityConfig && iL2CapEntityConfig->PeerInfoDefined();
	}

#endif
