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
// Types that are needed across L2CAP source
// 
//

#ifndef L2TYPES_H
#define L2TYPES_H

#include <e32std.h>
#include <e32cons.h>
#include <e32base.h>

#include <bttypes.h>
#include <bt_sock.h>

#include "l2constants.h"
#include "l2util.h"

enum TSignallingCommand
	{
	ECommandReject        = 0x01,
	EConnectionRequest    = 0x02,
	EConnectionResponse   = 0x03,
	EConfigureRequest     = 0x04,
	EConfigureResponse    = 0x05,
	EDisconnectionRequest = 0x06,
	EDisconnectionResponse= 0x07,
	EEchoRequest          = 0x08,
	EEchoResponse         = 0x09,
	EInformationRequest   = 0x0A,
	EInformationResponse  = 0x0B
	};
	
enum TL2CAPCommandRejectReason
	{
	ECommandNotUnderstood = 0x00,
	EMTUExceeded          = 0x01,
	EInvalidCID           = 0x02
	};
	
struct TL2CAPCommandRejectData
	{
	TL2CAPCommandRejectReason iReason;
	
	TUint16 iMTUExceeded;
	TUint16 iLocalEndpoint;
	TUint16 iRemoteEndpoint;
	};

enum TConnectResponseResult
	{
	EConnectSuccess           = 0x0000,
	EConnectPending           = 0x0001,
	EConnectPSMNotSupported   = 0x0002,
	EConnectSecurityBlock     = 0x0003,
	EConnectNoResources       = 0x0004
	};

enum TConnectResponseStatus
	{
	EConnectPendNoFurtherInfo  = 0x0000,
	EConnectPendAuthentication = 0x0001,
	EConnectPendAuthorization  = 0x0002
	};


// L2Cap Entity Supported Features
enum TInfoType
	{
	EConnectionlessMTU 			= 0x0001,
	EExtendedFeaturesSupported 	= 0x0002
	};

enum TInfoReqResult
	{
	ESuccess 				= 0x0000,
	ENotsupported			= 0x0001
	};

enum TExtendedFeaturesType
	{
	ENoExtendedFeatures				= 0x00,
	EFlowControlMode 				= 0x01,	
	ERetransmissionMode				= 0x02,
	EBiDirectionalQOS				= 0x04,
	EEnhancedRetransmissionMode		= 0x08,
	EStreamingMode					= 0x10,
	EFCSOption						= 0x20,
	};

enum TL2CapEntityInfoState
	{
	EL2CapEntityInfoRequested	=0x00,
	EL2CapEntityInfoUndef		=0x01,
	EL2CapEntityInfoDefined 	=0x02
	};


NONSHARABLE_CLASS(TL2CapEntityInfo)
	{
public:

	TL2CapEntityInfo();
	TL2CapEntityInfo(TUint8 aExtendedFeatures);
	TL2CapEntityInfoState LinkInfoState() const;
	inline void SetLinkInfoState(TL2CapEntityInfoState aLinkInfoState);

	inline void  SetSupportFlowControl();
	inline void  SetSupportRetranmission();
	inline void  SetSupportBiDirectionalQos();
	inline void  SetSupportEnhancedRetransmissionMode();
	inline void  SetSupportStreamingMode();
	inline void  SetSupportFCSOption();

	inline TBool SupportsFlowControl() const;
	inline TBool SupportsRetranmission() const;
	inline TBool SupportsBiDirectionalQOS() const;
	inline TBool SupportsEnhancedRetransmissionMode() const;
	inline TBool SupportsStreamingMode() const;
	inline TBool SupportsFCSOption() const;

	TUint8 ExtendedFeatures() const;

private:
	TUint8 iExtendedFeatures;
	TL2CapEntityInfoState	iLinkInfoState;
	};
	
inline TL2CapEntityInfoState TL2CapEntityInfo::LinkInfoState() const
	{
	return (iLinkInfoState);
	}

inline void TL2CapEntityInfo::SetLinkInfoState(TL2CapEntityInfoState aLinkInfoState)
	{
	iLinkInfoState  = aLinkInfoState;
	}

inline TBool TL2CapEntityInfo::SupportsFlowControl() const
	{
	return (iExtendedFeatures & EFlowControlMode);
	}

inline TBool TL2CapEntityInfo::SupportsRetranmission() const
	{
	return (iExtendedFeatures & ERetransmissionMode);
	}

inline TBool TL2CapEntityInfo::SupportsBiDirectionalQOS() const
	{
	return (iExtendedFeatures & EBiDirectionalQOS);
	}

inline TBool TL2CapEntityInfo::SupportsEnhancedRetransmissionMode() const
	{
	return (iExtendedFeatures & EEnhancedRetransmissionMode);
	}

inline TBool TL2CapEntityInfo::SupportsStreamingMode() const
	{
	return (iExtendedFeatures & EStreamingMode);
	}

inline TBool TL2CapEntityInfo::SupportsFCSOption() const
	{
	return (iExtendedFeatures & EFCSOption);
	}

inline void TL2CapEntityInfo::SetSupportFlowControl()
	{
	iExtendedFeatures |=  EFlowControlMode;
	}

inline void TL2CapEntityInfo::SetSupportRetranmission()
	{
	iExtendedFeatures |= ERetransmissionMode;
	}

inline void TL2CapEntityInfo::SetSupportBiDirectionalQos()
	{
	iExtendedFeatures |= EBiDirectionalQOS;
	}

inline void TL2CapEntityInfo::SetSupportEnhancedRetransmissionMode()
	{
	iExtendedFeatures |= EEnhancedRetransmissionMode;
	}

inline void TL2CapEntityInfo::SetSupportStreamingMode()
	{
	iExtendedFeatures |= EStreamingMode;
	}

inline void TL2CapEntityInfo::SetSupportFCSOption()
	{
	iExtendedFeatures |= EFCSOption;
	}

inline TUint8 TL2CapEntityInfo::ExtendedFeatures() const
	{
	return iExtendedFeatures;
	}

	

// ****************************************************************
// *** L2Cap Config Types
// ****************************************************************
enum TConfigOptionType
	{
	// NB: Maximum number is 0x7F as anything higher will set hint bit
	EConfigOptionTypeMTU					= 0x01,
	EConfigOptionTypeFlushTimeoutDuration	= 0x02,
	EConfigOptionTypeQOS					= 0x03,
	EConfigOptionTypeRTxAndFEC 				= 0x04,
	EConfigOptionTypeFcs					= 0x05,
	};

enum TQOSServiceType
	{
	ENoTraffic  = 0x00,
	EBestEffort = 0x01,
	EGuaranteed = 0x02
	};


// All configuration options should be derived from this interface class.
// The interface specifies the mandatory operations for a config option class.
class ML2CapConfigurationOption
	{
public:
	enum TStandardOptionValue
		{
		EAbsoluteMinimumValue,
		ESpecMinimumValue,
		ESpecDefaultValue,
		EMaximumValue,
		EPreferredValue,
		};
		
	virtual TConfigOptionType OptionType() const = 0;
	
	// Option operators - these must be implemented by any new options.
	virtual void operator=(const ML2CapConfigurationOption& aOption) = 0;
	virtual TBool operator<=(const ML2CapConfigurationOption& aOption) const = 0;
	virtual TBool operator==(const ML2CapConfigurationOption& aOption) const = 0;
	};
	
NONSHARABLE_CLASS(TMTUOption) : public ML2CapConfigurationOption
	{
public:
	TMTUOption(TUint16 aMTU);
	TMTUOption(ML2CapConfigurationOption::TStandardOptionValue aStandardOption);

	// Accessor methods.
	inline TUint16 MTU() const;
		
	// Option operators
	void operator=(const ML2CapConfigurationOption& aOption);
	TBool operator<=(const ML2CapConfigurationOption& aOption) const;
	TBool operator==(const ML2CapConfigurationOption& aOption) const;
	TBool operator>(const ML2CapConfigurationOption& aOption) const;
	void operator=(const TMTUOption& aOption);
	TBool operator<=(const TMTUOption& aOption) const;
	TBool operator==(const TMTUOption& aOption) const;
	TBool operator>(const TMTUOption& aOption) const;

private:
	TConfigOptionType OptionType() const;

	// Option constants.
	const static TUint16 KAbsMinMTU		= 1;
	const static TUint16 KSpecMinMTU 	= KL2MinMTU;
	const static TUint16 KMaxMTU 		= KL2CapMaxMTUSize;
	const static TUint16 KDefaultMTU 	= KL2CapDefaultMTUSize;
	const static TUint16 KPreferredMTU 	= KL2CapPreferredMTUSize;
	
	TUint16 iMTU;
	};

// Accessor methods.
inline TUint16 TMTUOption::MTU() const
	{
	return iMTU;
	}

	
NONSHARABLE_CLASS(TFlushTimeoutDurationOption) : public ML2CapConfigurationOption
	{
public:
	TFlushTimeoutDurationOption(TUint16 aFlushTimeoutDuration);
	TFlushTimeoutDurationOption(ML2CapConfigurationOption::TStandardOptionValue aStandardOption);
	
	// Accessor methods.
	inline TUint16 FlushTimeoutDuration() const;

	// Option operators
	void operator=(const ML2CapConfigurationOption& aOption);
	TBool operator<=(const ML2CapConfigurationOption& aOption) const;
	TBool operator==(const ML2CapConfigurationOption& aOption) const;
	void operator=(const TFlushTimeoutDurationOption& aOption);
	TBool operator<=(const TFlushTimeoutDurationOption& aOption) const;
	TBool operator==(const TFlushTimeoutDurationOption& aOption) const;

private:
	TConfigOptionType OptionType() const;

	// Option constants.
	const static TUint16 KMinFlush 	= 0x0001;  // 1 ms
	const static TUint16 KMaxFlush 	= 0xffff;
	const static TUint16 KDefaultFlush = KMaxFlush;

	TUint16 iFlushTimeoutDuration;
	};

// Accessor methods.
inline TUint16 TFlushTimeoutDurationOption::FlushTimeoutDuration() const
	{
	return iFlushTimeoutDuration;
	}

NONSHARABLE_CLASS(TQualityOfServiceOption) : public ML2CapConfigurationOption
	{
public:
	TQualityOfServiceOption(TQOSServiceType aServiceType,
							TUint32 aTokenRate,
							TUint32 aTokenBucketSize,
							TUint32 aPeakBandwidth,
							TUint32 aLatency,
							TUint32 aDelayVariation);
	TQualityOfServiceOption(ML2CapConfigurationOption::TStandardOptionValue aStandardOption);

	// Accessor Methods
	inline TQOSServiceType ServiceType() const;
	inline TUint32 TokenRate() const;
	inline TUint32 TokenBucketSize() const;
	inline TUint32 PeakBandwidth() const;
	inline TUint32 Latency() const;
	inline TUint32 DelayVariation() const;
	
	// Option operators
	void operator=(const ML2CapConfigurationOption& aOption);
	TBool operator<=(const ML2CapConfigurationOption& aOption) const;
	TBool operator==(const ML2CapConfigurationOption& aOption) const;
	void operator=(const TQualityOfServiceOption& aOption);
	TBool operator<=(const TQualityOfServiceOption& aOption) const;
	TBool operator==(const TQualityOfServiceOption& aOption) const;

private:
	TConfigOptionType OptionType() const;

	// Option constants.
	const static TUint32 KMinTokenRate 				= 0x00000000;
	const static TUint32 KMinTokenBucketSize 		= 0x00000000;
	const static TUint32 KMinPeakBandwidth 			= 0x00000000;
	const static TUint32 KMinLatency 				= 0x00000000;
	const static TUint32 KMinDelayVariation 		= 0x00000000;

	const static TUint32 KMaxTokenRate 				= 0xffffffff;
	const static TUint32 KMaxTokenBucketSize 		= 0xffffffff;
	const static TUint32 KMaxPeakBandwidth 			= 0xffffffff;
	const static TUint32 KMaxLatency 				= 0xffffffff;
	const static TUint32 KMaxDelayVariation 		= 0xffffffff;

	const static TUint32 KDefaultTokenRate 			= 0x00000000;
	const static TUint32 KDefaultTokenBucketSize 	= 0x00000000;
	const static TUint32 KDefaultPeakBandwidth 		= 0x00000000;
	const static TUint32 KDefaultLatency 			= 0xffffffff;
	const static TUint32 KDefaultDelayVariation		= 0xffffffff;

	TQOSServiceType iServiceType;
	TUint32 iTokenRate;
	TUint32 iTokenBucketSize;
	TUint32 iPeakBandwidth;
	TUint32 iLatency;
	TUint32 iDelayVariation;
	};

inline TQOSServiceType TQualityOfServiceOption::ServiceType() const
	{
	return iServiceType;
	}
	
inline TUint32 TQualityOfServiceOption::TokenRate() const
	{
	return iTokenRate;
	}
	
inline TUint32 TQualityOfServiceOption::TokenBucketSize() const
	{
	return iTokenBucketSize;
	}
	
inline TUint32 TQualityOfServiceOption::PeakBandwidth() const
	{
	return iPeakBandwidth;
	}
	
inline TUint32 TQualityOfServiceOption::Latency() const
	{
	return iLatency;
	}
	
inline TUint32 TQualityOfServiceOption::DelayVariation() const
	{
	return iDelayVariation;
	}

// Note that this class does _not_ inherit from ML2CapConfigurationOption,
// as the implementation of comparison operations is not trivial and done outside
// of the class.
NONSHARABLE_CLASS(TRetransmissionAndFlowControlOption)
	{
public:
	TRetransmissionAndFlowControlOption(TL2CapChannelMode aLinkMode,
										TUint8 aTxWindowSize,
										TUint8 aMaxTransmit,
										TUint16 aRetransmissionTimeout,
										TUint16 aMonitorTimeout,
										TUint16 aMaximumPDUSize);
	TRetransmissionAndFlowControlOption(TL2CapChannelMode aMode, TBool aFillWithPreferredValues);
	inline TRetransmissionAndFlowControlOption();
	
	// Accessor methods.
	inline TL2CapChannelMode LinkMode() const;
	inline TUint8 LinkModeAsUnsignedByte() const;
	inline TUint8 TxWindowSize() const;
	inline TUint8 MaxTransmit() const;
	inline TUint16 RetransmissionTimeout() const;
	inline TUint16 MonitorTimeout() const;
	inline TUint16 MaximumPDUSize() const;

	inline void SetLinkMode(TL2CapChannelMode aLinkMode);
	inline void SetTxWindowSize(TUint8 aTxWindowSize);
	inline void SetMaxTransmit(TUint8 aMaxTransmit);
	inline void SetRetransmissionTimeout(TUint16 aRetransmissionTimeout);
	inline void SetMonitorTimeout(TUint16 aMonitorTimeout);
	inline void SetMaximumPDUSize(TUint16 aPDUSize);

	TBool operator==(const TRetransmissionAndFlowControlOption& aOption) const;

	static TBool EnhancedMaxTransmitLessOrEqual(TUint aLeft, TUint aRight);
	static TBool IsModeReliable(TL2CapChannelMode aMode);
    static TBool IsModeValid(TL2CapChannelMode aMode);
    
#ifdef __FLOG_ACTIVE
	// Required free buffer space is specified in KReadableDesSpaceRequired.
	void GetReadable(TDes& aBuf) const;
#endif

private:
	void Initialize(TBool aIsEnhanced, ML2CapConfigurationOption::TStandardOptionValue aStandardOption);

	TConfigOptionType OptionType() const;

public:
	// Note: 'Valid' means 'sensible for parsing at all', 'Acceptable' means valid AND
	// something we can agree to use (e.g. 10ms is a 'valid' retransmission time out, but
	// not something we should accept).

	// Option constants.
	const static TUint8  KMinValidTxWindowSize 		= 0x01;
	const static TUint8  KMinValidNumberTransmit 	= 0x01;

	// 100ms.  There could be a risk of denial of service if retransmissions / monitor
	// timers were permitted at a frequency greater than this.
	const static TUint16 KMinAcceptableRetransmissionTimeout	= 100;
	const static TUint16 KMinAcceptableMonitorTimeout 			= 100;
	const static TUint16 KMaxAcceptableRetransmissionTimeout	= 0xffff;
	const static TUint16 KMaxAcceptableMonitorTimeout 			= 0xffff;

	const static TUint16 KMinValidMaximumPDUSize 			= 0x01;
	const static TUint16 KMaxAcceptableMaximumPDUSize		= 0xffff;

	const static TUint8  KMaxValidLegacyTxWindowSize 		= 32;
	const static TUint8  KMaxValidEnhancedTxWindowSize 		= 63;

	const static TUint8  KMaxValidLegacyNumberTransmit		= 0xff;
	const static TUint8  KMaxValidEnhancedNumberTransmit	= 0x0;	// 0 stands for infinity

	const static TUint8  KDefaultLegacyTxWindowSize 	= 32;
	const static TUint8  KDefaultEnhancedTxWindowSize 	= 16;
	const static TUint8  KDefaultNumberTransmit 		= 10;

	// Note: due to differences in the negotiation process, this default retransmission
	// timeout value is used in outgoing direction for ERTM and in incoming direction for
	// RTM (i.e. with RTM we'll actually use peer-proposed value for our retransmission
	// timer and the remote will use this one). 
	const static TUint16 KDefaultRetransmissionTimeout	= 2000;
	const static TUint16 KDefaultMonitorTimeout 		= 12000;
	const static TUint16 KDefaultMaximumPDUSize 		= 0xffff;

	const static TInt KReadableDesSpaceRequired = 128;

private:
	TL2CapChannelMode iLinkMode;
	TUint8 iTxWindowSize;
	TUint8 iMaxTransmit;
	TUint16 iRetransmissionTimeout;
	TUint16 iMonitorTimeout;
	TUint16 iMaximumPDUSize;
	};


inline TRetransmissionAndFlowControlOption::TRetransmissionAndFlowControlOption()
 :	iLinkMode(EL2CAPBasicMode),
 	iTxWindowSize(0),
 	iMaxTransmit(0),
 	iRetransmissionTimeout(0),
	iMonitorTimeout(0),
	iMaximumPDUSize(0)
	{}

// Accessor methods.

inline TL2CapChannelMode TRetransmissionAndFlowControlOption::LinkMode() const
	{
	return iLinkMode;
	}

inline TUint8 TRetransmissionAndFlowControlOption::LinkModeAsUnsignedByte() const
	{
	return static_cast<TUint8>(iLinkMode);
	}
	
inline TUint8 TRetransmissionAndFlowControlOption::TxWindowSize() const
	{
	return iTxWindowSize;
	}

inline TUint8 TRetransmissionAndFlowControlOption::MaxTransmit() const
	{
	return iMaxTransmit;
	}

inline TUint16 TRetransmissionAndFlowControlOption::RetransmissionTimeout() const
	{
	return iRetransmissionTimeout;
	}

inline TUint16 TRetransmissionAndFlowControlOption::MonitorTimeout() const
	{
	return iMonitorTimeout;
	}

inline TUint16 TRetransmissionAndFlowControlOption::MaximumPDUSize() const
	{
	return iMaximumPDUSize;
	}

inline void TRetransmissionAndFlowControlOption::SetLinkMode(TL2CapChannelMode aLinkMode)
	{
	iLinkMode = aLinkMode;
	}

inline void TRetransmissionAndFlowControlOption::SetTxWindowSize(TUint8 aTxWindowSize)
	{
	iTxWindowSize = aTxWindowSize;
	}

inline void TRetransmissionAndFlowControlOption::SetMaxTransmit(TUint8 aMaxTransmit)
	{
	iMaxTransmit = aMaxTransmit;
	}
	
inline void TRetransmissionAndFlowControlOption::SetRetransmissionTimeout(TUint16 aRetransmissionTimeout)
	{
	iRetransmissionTimeout = aRetransmissionTimeout;
	}

inline void TRetransmissionAndFlowControlOption::SetMonitorTimeout(TUint16 aMonitorTimeout)
	{
	iMonitorTimeout = aMonitorTimeout;
	}

inline void TRetransmissionAndFlowControlOption::SetMaximumPDUSize(TUint16 aPDUSize)
	{
	iMaximumPDUSize = aPDUSize;
	}


NONSHARABLE_CLASS(TL2CapConfigurationOptionGroupBase)
	{
public:
	enum TOptionConfigStatus
		{
		EOptionConfigOutstanding,
		EOptionConfigComplete,
		EOptionConfigFailed
		};

	enum TOptionType
		{
		ENegotiated,
		ENegotiateToMinimum,
		};
	};

// Thin template interface for config options group.
// This implements a generic option negotiation state machine.
// The state of the machine is worked out from the stored option values:
// preferred value, last negotiated value, most recently received peer value
// and limits on what we can accept from the peer.
// For the negotiation to be complete the Preferred and Negotiated values
// must be equal.
template<class T>
class TL2CapConfigurationOptionGroup : public TL2CapConfigurationOptionGroupBase
	{
public:
	inline TL2CapConfigurationOptionGroup(const T& aUpperLimit,
										  const T& aLowerLimit,
										  const T& aPreferred,
										  const T& aDefault,
										  TOptionType aOptionType);

	inline void operator=(const TL2CapConfigurationOptionGroup<T>& aOptionGroup);

	inline void SetPreferred(const T& aPreferred);
	inline void SetRequiredValue(const T& aUpperLimit, const T& aLowerLimit, const T& aPreferred);

	inline const T& Preferred() const;
	inline const T& Negotiated() const;
	inline const T& LowerLimit() const;
	inline const T& UpperLimit() const;

	inline TL2CapConfigurationOptionGroupBase::TOptionConfigStatus ConfigOptionStatus() const;

	// Events driving the state machine.

	// Acceptor path - handling of Config Request.
	inline void PeerRequestsOption(const T& aPeerRequestValue);
	inline void PeerRequestsLastAcceptedValue();

	// Initiator path - handling of Config Response after our Preferred has been
	// sent in a Config Request.
	inline void PeerAcceptsOption(const T& aPeerResponseValue);
	inline void PeerAcceptsOption();
	inline void PeerRejectsOption(const T& aPeerSuggestion);

	inline TBool NeedToIncludeInPositiveConfigResponse();
protected:
	// This implements the state machine behaviour.
	inline void SetPeer(const T& aPeer, TBool aIsUnacceptable);
protected:
	// Upper and lower limits on what compromises can be made wrt. the initial
	// preferred value. Options have to define comparison operators so that
	// values received from peer can be compared with the limits.
	T iUpperLimit;
	T iLowerLimit;
	// Initially the value that we'd like to negotiate, gets changed if we make
	// compromises during negotiation. This is what gets sent to the peer during
	// negotiation.
	T iPreferred;
	// Last value received from the peer - not necessarily agreed on.
	// Initially protocol default.
	T iPeer;
	// Last mutually agreed value. This is what should be used when the negotiation
	// is finished.
	// Initially protocol default. 
	T iNegotiated;

	TOptionType iOptionType;
	};
	
template<class T>
inline const T& TL2CapConfigurationOptionGroup<T>::Preferred() const
	{
	return iPreferred;
	}

template<class T>
inline const T& TL2CapConfigurationOptionGroup<T>::Negotiated() const
	{
	return iNegotiated;
	}

template<class T>
inline const T& TL2CapConfigurationOptionGroup<T>::LowerLimit() const
	{
	return iLowerLimit;
	}

template<class T>
inline const T& TL2CapConfigurationOptionGroup<T>::UpperLimit() const
	{
	return iUpperLimit;
	}

template<class T>
inline TL2CapConfigurationOptionGroup<T>::TL2CapConfigurationOptionGroup(const T& aUpperLimit,
															      		 const T& aLowerLimit,
															      		 const T& aPreferred,
															      		 const T& aDefault,
															      		 TOptionType aOptionType)
 :	iUpperLimit(aUpperLimit),
	iLowerLimit(aLowerLimit),
	iPreferred(aPreferred),
	iPeer(aDefault),
	iNegotiated(aDefault),
	iOptionType(aOptionType)
 	{
 	// Ensure the preferred / actual value always remains between the upper
	// and lower limit.
	__ASSERT_DEBUG(iLowerLimit <= iPreferred && iPreferred <= iUpperLimit, Panic(EL2CAPInvalidConfigOptionState));
  	}

template<class T>
inline void TL2CapConfigurationOptionGroup<T>::SetPreferred(const T& aPreferred)
	{
	// Ensure the preferred / actual value always remains between the upper and lower limit.
	__ASSERT_DEBUG(iLowerLimit <= aPreferred && aPreferred <= iUpperLimit, Panic(EL2CAPInvalidConfigOptionState));
	iPreferred = aPreferred;
	}

template<class T>
inline void TL2CapConfigurationOptionGroup<T>::SetRequiredValue(const T& aUpperLimit, const T& aLowerLimit, const T& aPreferred)
	{
	__ASSERT_DEBUG(aLowerLimit <= aPreferred && aPreferred <= aUpperLimit, Panic(EL2CAPInvalidConfigOptionState));
	iUpperLimit = aUpperLimit;
	iLowerLimit = aLowerLimit;
	iPreferred = aPreferred;
	}

template<class T>
inline void TL2CapConfigurationOptionGroup<T>::operator=(const TL2CapConfigurationOptionGroup<T>& aOptionGroup)
	{
	iUpperLimit = aOptionGroup.iUpperLimit;
	iLowerLimit = aOptionGroup.iLowerLimit;
	iPreferred = aOptionGroup.iPreferred;
	iPeer = aOptionGroup.iPeer;
	iNegotiated = aOptionGroup.iNegotiated;
	iOptionType = aOptionGroup.iOptionType;
	}

template<class T>
inline TL2CapConfigurationOptionGroupBase::TOptionConfigStatus TL2CapConfigurationOptionGroup<T>::ConfigOptionStatus() const
	{
	__ASSERT_DEBUG(iLowerLimit <= iPreferred && iPreferred <= iUpperLimit, Panic(EL2CAPInvalidConfigOptionState));
	TOptionConfigStatus status = EOptionConfigOutstanding;

	// Note: the order of the checks below is significant!
	// It's possible for iPeer to be outside of bounds and iActual == iNegotiated
	// e.g. in the following scenario, when MTU is being negotiated in the initiator
	// (incoming) path:
	// iUpperLimit = 672
	// iActual = 672
	// iNegotiated = 672
	// 1. ConfigReq with MTU=672 is sent to the peer.
	// 2. Peer responds with ConfigRsp(UnacceptableParams)[MTU=1691]
	//    This gets processed by SetPeer:
	//    iPeer := 1691
	//    iActual := iPeer cut off on iUpperLimit == 672
	// We want the status of this situation to resolve to failed.

	if (!(iLowerLimit <= iPeer && iPeer <= iUpperLimit))	
		{
		// The peer value is not within the locally acceptable range.
		status = EOptionConfigFailed;
		}
	else if (iPreferred == iNegotiated)
		{
		// Configuration is complete.
		status = EOptionConfigComplete;
		}
	return status;
	}
	
template<class T>
inline void TL2CapConfigurationOptionGroup<T>::SetPeer(const T& aPeer, TBool aIsUnacceptable)
	{
	// Store the new value proposed by peer. We use it later when assessing option
	// status in ConfigOptionStatus().
	iPeer = aPeer;

	// Check if the peer value is within the acceptable range.
	if(!(iLowerLimit <= aPeer))
		{
		// Peer's value is unacceptable for us. Prepare a suggested acceptable value.
		// Note: the suggested acceptable value is only sent if this is the initiator
		// path, acceptor path which gets here means failed negotiation unless we
		// do a new round of negotiation by sending a new Config Request. Which we don't,
		// it's inherently race-prone as if it's a positive response we're processing here
		// then the remote thinks it's finished with negotiation successfully and can start
		// sending data, if the other path is complete.
		iPreferred = iLowerLimit;
		}
	else
		{
		if(!(aPeer <= iUpperLimit))
			{
			// see note for lower limit.
			iPreferred = iUpperLimit;
			}
		else
			{
			// The Peer value is within the acceptable range.
			if (aIsUnacceptable)
				{
				// We're processing an Unacceptable Parameters Config Response
				// and the value specific by the peer (aPeer) is a suggested
				// value to be used in our next Config Request instead of the
				// unacceptable value that was previously attempted.
				iPreferred = aPeer;
				// Note: there was no mutual agreement so iNegotiated stays unchanged.
				}
			else
				{
				switch (iOptionType)
					{
				case ENegotiated:
					// Peer value is acceptable for us.
					// It now becomes the most recent mutually agreed value.
					// Note that if this is the initiator path (i.e. processing a Config
					// Response now) then iActual should already be equal to aPeer, as
					// the remote entity is agreeing with the value we sent.
					iPreferred = aPeer;
					iNegotiated = aPeer;
					break;
					
				case ENegotiateToMinimum:
					// Negotiate to the lowest mutually acceptable value.
					if(iPreferred <= aPeer)
						{
						iNegotiated = iPreferred;
						}
					else
						{
						iNegotiated = aPeer;
						iPreferred = aPeer;
						}
					break;
				
				default:
					Panic(EL2CAPInvalidConfigOptionState);
					break;
					} // switch (option type)
				} // not Unacceptable Parameters response
			} // peer value within bounds
		}
	}

template<class T>
inline void TL2CapConfigurationOptionGroup<T>::PeerRequestsOption(const T& aPeerRequestValue)
	{
	SetPeer(aPeerRequestValue, EFalse);
	}

template<class T>
inline void TL2CapConfigurationOptionGroup<T>::PeerRequestsLastAcceptedValue()
	{
	SetPeer(iNegotiated, EFalse);
	}

template<class T>
inline void TL2CapConfigurationOptionGroup<T>::PeerAcceptsOption(const T& aPeerResponseValue)
	{
	SetPeer(aPeerResponseValue, EFalse);
	}

template<class T>
inline void TL2CapConfigurationOptionGroup<T>::PeerAcceptsOption()
	{
	SetPeer(iPreferred, EFalse);
	}

template<class T>
inline void TL2CapConfigurationOptionGroup<T>::PeerRejectsOption(const T& aPeerSuggestion)
	{
	SetPeer(aPeerSuggestion, ETrue);
	}

template<class T>
inline TBool TL2CapConfigurationOptionGroup<T>::NeedToIncludeInPositiveConfigResponse()
	{
	return !(iPreferred == iPeer);
	}

typedef TUint16 TConfigFlags;

enum TConfigResponseResult
	{
	EConfigSuccess = 0x0000,
	EConfigUnacceptableParams = 0x0001,
	EConfigRejected = 0x0002,
	EConfigUnknownOption = 0x0003,
	EConfigNotRelevant = 0x0004
	};

NONSHARABLE_CLASS(TL2CapDataControllerConfig)
	{
public:
	enum 
		{
		ELowestPriority 	= 0,
		};
	
	TL2CapDataControllerConfig(TL2CapChannelMode aLinkMode,
					           TUint8 aTXWindowSize,
	                           TUint8 aMaxTransmit,
	                           TUint16 aRetransmissionTimeout,
	                           TUint16 aMonitorTimeout,
	                           TUint8 aPeerTXWindowSize,
	                           TUint16 aPeerRetransmissionTimeout,
	                           TUint16 aIncomingMps,
	                           TUint8 aPriority = ELowestPriority);

	inline TL2CapChannelMode LinkMode() const;
	inline TUint8 TXWindowSize() const;
	inline TUint8 MaxTransmit() const;
	inline TUint16 RetransmissionTimeout() const;
	inline TUint16 MonitorTimeout() const;
	inline TUint8 ChannelPriority() const;
	inline void SetChannelPriority(TUint8 aNewPriority);
	inline TUint8 PeerTXWindowSize() const;
	inline TUint16 PeerRetransmissionTimeout() const;
	inline TUint16 IncomingMps() const;

private:
	TL2CapChannelMode iLinkMode;			// Mode definition.
	TUint8 iTXWindowSize;  					// Range 1 - 63.
	TUint8 iMaxTransmit;					// Retransmissions before the link is failed.
	TUint16 iIncomingMps;
	TUint16 iRetransmissionTimeout; 		// In milliseconds
	TUint16 iMonitorTimeout;				// In milliseconds
	TUint8 iChannelPriority;				// Range: 0 (Lowest) - 0xff (Highest)
	
	// Peer values used to determine S-Frame acknowledgement scheme.
	TUint8 iPeerTXWindowSize;  				// Range 1 - 63.
	TUint16 iPeerRetransmissionTimeout; 	// In milliseconds
	};

inline TL2CapChannelMode TL2CapDataControllerConfig::LinkMode() const
	{
	return iLinkMode;
	}

inline TUint8 TL2CapDataControllerConfig::TXWindowSize() const
	{
	return iTXWindowSize;
	}
	
inline TUint8 TL2CapDataControllerConfig::MaxTransmit() const
	{
	return iMaxTransmit;
	}

inline TUint16 TL2CapDataControllerConfig::RetransmissionTimeout() const
	{
	return iRetransmissionTimeout;
	}

inline TUint16 TL2CapDataControllerConfig::MonitorTimeout() const
	{
	return iMonitorTimeout;
	}

inline TUint8 TL2CapDataControllerConfig::ChannelPriority() const
	{
	return iChannelPriority;
	}
	
inline void TL2CapDataControllerConfig::SetChannelPriority(TUint8 aNewPriority)
	{
	iChannelPriority = aNewPriority;
	}

inline TUint8 TL2CapDataControllerConfig::PeerTXWindowSize() const
	{
	return iPeerTXWindowSize;
	}
	
inline TUint16 TL2CapDataControllerConfig::PeerRetransmissionTimeout() const
	{
	return iPeerRetransmissionTimeout;
	}

inline TUint16 TL2CapDataControllerConfig::IncomingMps() const
	{
	return iIncomingMps;
	}


#ifdef _DEBUG
NONSHARABLE_CLASS(TL2DataPlaneConfig)
	{
public:
	TL2CapChannelMode iLinkMode;
	TUint8 iTxWindowSize;
	TUint8 iMaxTransmit;
	TUint16 iRetransmissionTimeout;
	TUint16 iMonitorTimeout;
	TUint16 iMaximumPDUSize;
	TUint8 iPriority;
	TUint8 iOutboundQueueSize;
	TUint16 iFlushTimeout;
	TUint16 iMaxOutgoingMTU;
	TUint16 iMaxIncomingMTU;
	};

typedef TPckgBuf<TL2DataPlaneConfig> TL2DataPlaneConfigPkg;
#endif

#endif
