// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef L2CAPSIGPACKETCONFIGURE_H
#define L2CAPSIGPACKETCONFIGURE_H

#include "l2capCommand.h"

class CL2CapChannelConfig;
class TL2CapConfigParamOptions;

NONSHARABLE_CLASS(HConfigureRequest) : public HL2CapCommand
	{
public:	
	static HConfigureRequest* New(TL2CAPPort aRemotePort, CL2CapChannelConfig& aConfig,
                 				  TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	             				  TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);
	
	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);

	inline TL2CAPPort DestinationCID() const;
	inline TConfigFlags Flags() const;
	inline void SetDestinationCID(TL2CAPPort aPort);
	inline void SetFlags(TConfigFlags aFlags);

	TInt ParseOptions(TL2CapConfigParamOptions& aConfigOptions, RMBufChain& aUnknownOptions) const;
	TInt AddOptionsToCommand(CL2CapChannelConfig& aConfig);

private:
	HConfigureRequest(RMBufChain& aCommand,
                 	  TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	                  TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);

private:
	const static TUint8 KConfigRequestEmptyLength	= 8;
	
	const static TUint8 KDestinationCIDOffset		= 4;
	const static TUint8 KFlagsOffset				= 6;
	const static TUint8 KOptionsOffset				= 8;
	
	const static TUint8 KNoConfigurationFlags		= 0;
	};

inline TL2CAPPort HConfigureRequest::DestinationCID() const
	{
	return GetLittleEndian16(KDestinationCIDOffset, iCommand);
	}

inline TConfigFlags HConfigureRequest::Flags() const
	{
	return GetLittleEndian16(KFlagsOffset, iCommand);
	}

inline void HConfigureRequest::SetDestinationCID(TL2CAPPort aPort)
	{
	PutLittleEndian16(aPort, KDestinationCIDOffset, iCommand);
	}

inline void HConfigureRequest::SetFlags(TConfigFlags aFlags)
	{
	PutLittleEndian16(aFlags, KFlagsOffset, iCommand);
	}


NONSHARABLE_CLASS(HConfigureResponse) :public HL2CapCommand
	{
public:
	static HConfigureResponse* New(TUint8 aId, TL2CAPPort aRemotePort, TConfigFlags aFlags, TConfigResponseResult aResult);
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);
	
	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);

	inline TL2CAPPort SourceCID() const;
	inline TConfigFlags Flags() const;
	inline TConfigResponseResult Results() const;
	inline void SetSourceCID(TL2CAPPort aPort);
	inline void SetFlags(TConfigFlags aFlags);
	inline void SetResults(TConfigResponseResult aResult);

	TInt ParseOptions(TL2CapConfigParamOptions& aConfigOptions, RMBufChain& aUnknownOptions) const;
	TInt AddOptionsToCommand(CL2CapChannelConfig& aConfig, TConfigResponseResult aResult);
	void AddUnknownOptionsToCommand(RMBufChain& aUnknownOptions);

public:
	// Used on creation to specify that no flags
	// (i.e., the continuation flag) are set.
	const static TUint8 KNoConfigurationFlags		= 0;

private:
	HConfigureResponse(RMBufChain& aCommand);
		
private:
	const static TUint8 KConfigResponseEmptyLength	= 10;
	
	const static TUint8 KSourceCIDOffset			= 4;
	const static TUint8 KFlagsOffset				= 6;
	const static TUint8 KResultOffset				= 8;
	const static TUint8 KOptionsOffset				= 10;
	};

inline TL2CAPPort HConfigureResponse::SourceCID() const
	{
	return GetLittleEndian16(KSourceCIDOffset, iCommand);
	}

inline TConfigFlags HConfigureResponse::Flags() const
	{
	return GetLittleEndian16(KFlagsOffset, iCommand);
	}

inline TConfigResponseResult HConfigureResponse::Results() const
	{
	return static_cast<TConfigResponseResult>(GetLittleEndian16(KResultOffset, iCommand));
	}

inline void HConfigureResponse::SetSourceCID(TL2CAPPort aPort)
	{
	PutLittleEndian16(aPort, KSourceCIDOffset, iCommand);
	}

inline void HConfigureResponse::SetFlags(TConfigFlags aFlags)
	{
	PutLittleEndian16(aFlags, KFlagsOffset, iCommand);
	}

inline void HConfigureResponse::SetResults(TConfigResponseResult aResult)
	{
	PutLittleEndian16(static_cast<TUint16>(aResult), KResultOffset, iCommand);
	}

NONSHARABLE_CLASS(TL2CapConfigParamOptions)
	{
public:
	TL2CapConfigParamOptions();

	inline void SetMtu(const TMTUOption& aMtu);
	inline TBool IsMtuSet() const;
	inline const TMTUOption& Mtu() const;

	inline void SetFlushTimeout(const TFlushTimeoutDurationOption& aFlushTimeout);
	inline TBool IsFlushTimeoutSet() const;
	inline const TFlushTimeoutDurationOption& FlushTimeout() const;
	
	inline void SetFec(const TRetransmissionAndFlowControlOption& aFec);
	inline TBool IsFecSet() const;
	inline const TRetransmissionAndFlowControlOption& Fec() const;

	inline void SetQos(const TQualityOfServiceOption& aQos);
	inline TBool IsQosSet() const;
	inline const TQualityOfServiceOption& Qos() const;

	TBool operator==(const TL2CapConfigParamOptions& aThat);

	TInt ParseOptions(TUint8 aOptionOffset, const RMBufChain& aCommand, RMBufChain& aUnknownOptions);
	
	static TInt AddConfigRequestOptions(CL2CapChannelConfig& aConfig, TUint8 aOptionOffset, RMBufChain& aOptions);
	static TInt AddConfigResponseOptions(CL2CapChannelConfig& aConfig, TUint8 aOptionOffset, RMBufChain& aOptions, TConfigResponseResult aResult);
	static TInt AddMtuOptionL(TInt16 aMtu, TUint8 aOptionOffset, RMBufChain& aOptions);
	static TInt AddFlushOptionL(TInt16 aFlushTimeout, TUint8 aOptionOffset, RMBufChain& aOptions);
	static TInt AddQosOptionL(const TQualityOfServiceOption& aQosOption, TUint8 aOptionOffset, RMBufChain& aOptions);
	static TInt AddFecOptionL(const TRetransmissionAndFlowControlOption& aFecOption, TUint8 aOptionOffset, RMBufChain& aOptions);
	static TBool VerifyOptionsStructure(TUint8 aOptionOffset, const RMBufChain& aCommand);
private:
	// Options Constants
	const static TUint8 KTypeOffset   			= 0;
	const static TUint8 KLengthOffset 			= 1;
	const static TUint8 KDataOffset   			= 2;
	const static TUint8 KOptionHeaderLength		= 2;

	const static TUint8 KMTUOptionDataLen 		= 2;
	const static TUint8 KFlushOptionDataLen		= 2;
	const static TUint8 KQOSOptionDataLen 		= 22;
	const static TUint8 KFECOptionDataLen		= 9;
	const static TUint8 KFcsOptionDataLen		= 1;
	
	//QOS Options Offsets
	const static TUint8 KQOSServiceTypeOffset 	= 1;
	const static TUint8 KTokenRateOffset 		= 2;
	const static TUint8 KTokenBucketSizeOffset 	= 6;
	const static TUint8 KPeakBandwidthOffset 	= 10;
	const static TUint8 KLatencyOffset 			= 14;
	const static TUint8 KDelayVariationOffset 	= 18;

	// FEC Options Offsets
	const static TUint8 KLinkModeOffset					= 0;
	const static TUint8 KTxWindowSizeOffset 			= 1;
	const static TUint8 KMaxTransmitOffset 				= 2;
	const static TUint8 KRetransmissionTimeoutOffset 	= 3;
	const static TUint8 KMonitorTimeoutOffset 			= 5;
	const static TUint8 KMaximumPDUSizeOffset 			= 7;

private:
	TBool								iMtuSet;
	TMTUOption							iMtu;

	TBool								iFlushTimeoutSet;
	TFlushTimeoutDurationOption			iFlushTimeout;

	TBool								iFecSet;
	TRetransmissionAndFlowControlOption iFec;

	TBool								iQosSet;
	TQualityOfServiceOption				iQos;
	};


inline void TL2CapConfigParamOptions::SetMtu(const TMTUOption& aMtu)
	{
	iMtu = aMtu;
	iMtuSet = ETrue;
	}

inline TBool TL2CapConfigParamOptions::IsMtuSet() const
	{ return iMtuSet; }

inline const TMTUOption& TL2CapConfigParamOptions::Mtu() const
	{ return iMtu; }

inline void TL2CapConfigParamOptions::SetFlushTimeout(const TFlushTimeoutDurationOption& aFlushTimeout)
	{
	iFlushTimeout = aFlushTimeout;
	iFlushTimeoutSet = ETrue;
	}

inline TBool TL2CapConfigParamOptions::IsFlushTimeoutSet() const
	{ return iFlushTimeoutSet; }

inline const TFlushTimeoutDurationOption& TL2CapConfigParamOptions::FlushTimeout() const
	{ return iFlushTimeout; }

inline void TL2CapConfigParamOptions::SetFec(const TRetransmissionAndFlowControlOption& aFec)
	{
	iFec = aFec;
	iFecSet = ETrue;
	}

inline TBool TL2CapConfigParamOptions::IsFecSet() const
	{ return iFecSet; }

inline const TRetransmissionAndFlowControlOption& TL2CapConfigParamOptions::Fec() const
	{ return iFec; }

inline void TL2CapConfigParamOptions::SetQos(const TQualityOfServiceOption& aQos)
	{
	iQos = aQos;
	iQosSet = ETrue;
	}

inline TBool TL2CapConfigParamOptions::IsQosSet() const
	{ return iQosSet; }

inline const TQualityOfServiceOption& TL2CapConfigParamOptions::Qos() const
	{ return iQos; }

#endif
