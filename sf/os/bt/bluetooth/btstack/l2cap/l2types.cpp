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

#include <bluetooth/logger.h>

#include <bt_sock.h>

#include "L2types.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

TL2CapEntityInfo::TL2CapEntityInfo()
	:	iExtendedFeatures(ENoExtendedFeatures),
		iLinkInfoState(EL2CapEntityInfoUndef)
	{
	LOG_FUNC
	}	
TL2CapEntityInfo::TL2CapEntityInfo(TUint8 aExtendedFeatures)
	:	iExtendedFeatures(aExtendedFeatures),
		iLinkInfoState(EL2CapEntityInfoUndef)
	{
	LOG_FUNC
	}	


// ****************************************************************
// *** L2Cap Config Types
// ****************************************************************

// ****************************************************************
// *** MTU Option
// ****************************************************************
TMTUOption::TMTUOption(TUint16 aMTU)
 : iMTU(aMTU)
 	{
	LOG_FUNC
 	}

TMTUOption::TMTUOption(ML2CapConfigurationOption::TStandardOptionValue aStandardOption)
 	{
	LOG_FUNC
 	switch(aStandardOption)
 		{
		case ML2CapConfigurationOption::EAbsoluteMinimumValue:
			iMTU = KAbsMinMTU;
			break;
			
 		case ML2CapConfigurationOption::ESpecMinimumValue:
 			iMTU = KSpecMinMTU;
 			break;
 			
		case ML2CapConfigurationOption::EMaximumValue:
 			iMTU = KMaxMTU;
 			break;
 		
		case ML2CapConfigurationOption::EPreferredValue:
			iMTU = KPreferredMTU;
			break;

		case ML2CapConfigurationOption::ESpecDefaultValue:
		default:
 			iMTU = KDefaultMTU;
 			break;
 		};
 	}

TConfigOptionType TMTUOption::OptionType() const
	{
	LOG_FUNC
	return EConfigOptionTypeMTU;
	}

// Option operators
void TMTUOption::operator=(const ML2CapConfigurationOption& aOption)
	{
	LOG_FUNC
	*this = (static_cast<const TMTUOption&>(aOption));
	}
	
TBool TMTUOption::operator<=(const ML2CapConfigurationOption& aOption) const
	{
	LOG_FUNC
	return *this <= (static_cast<const TMTUOption&>(aOption));
	}

TBool TMTUOption::operator==(const ML2CapConfigurationOption& aOption) const
	{
	LOG_FUNC
	return *this == (static_cast<const TMTUOption&>(aOption));
	}

TBool TMTUOption::operator>(const ML2CapConfigurationOption& aOption) const
	{
	LOG_FUNC
	return *this > (static_cast<const TMTUOption&>(aOption));
	}

void TMTUOption::operator=(const TMTUOption& aOption)
	{
	LOG_FUNC
	iMTU = aOption.iMTU;
	}
	
TBool TMTUOption::operator<=(const TMTUOption& aOption) const
	{
	LOG_FUNC
	return iMTU <= aOption.iMTU;
	}

TBool TMTUOption::operator==(const TMTUOption& aOption) const
	{
	LOG_FUNC
	return iMTU == aOption.iMTU;
	}

TBool TMTUOption::operator>(const TMTUOption& aOption) const
	{
	LOG_FUNC
	return iMTU > aOption.iMTU;
	}

// ****************************************************************
// *** Flush Timer Option
// ****************************************************************
TFlushTimeoutDurationOption::TFlushTimeoutDurationOption(TUint16 aFlushTimeoutDuration)
 : iFlushTimeoutDuration(aFlushTimeoutDuration)
 	{
	LOG_FUNC
 	}
	
TFlushTimeoutDurationOption::TFlushTimeoutDurationOption(ML2CapConfigurationOption::TStandardOptionValue aStandardOption)
 	{
	LOG_FUNC
 	switch(aStandardOption)
 		{
 		case ML2CapConfigurationOption::EAbsoluteMinimumValue:
 		case ML2CapConfigurationOption::ESpecMinimumValue:
 			iFlushTimeoutDuration = KMinFlush;
 			break;
 			
		case ML2CapConfigurationOption::EMaximumValue:
 			iFlushTimeoutDuration = KMaxFlush;
 			break;
 		
		case ML2CapConfigurationOption::EPreferredValue:
		case ML2CapConfigurationOption::ESpecDefaultValue:
		default:
 			iFlushTimeoutDuration = KDefaultFlush;
 			break;
 		};
 	}

TConfigOptionType TFlushTimeoutDurationOption::OptionType() const
	{
	LOG_FUNC
	return EConfigOptionTypeFlushTimeoutDuration;
	}

// Option operators
void TFlushTimeoutDurationOption::operator=(const ML2CapConfigurationOption& aOption)
	{
	LOG_FUNC
	*this = (static_cast<const TFlushTimeoutDurationOption&>(aOption));
	}
	
TBool TFlushTimeoutDurationOption::operator<=(const ML2CapConfigurationOption& aOption) const
	{
	LOG_FUNC
	return *this <= (static_cast<const TFlushTimeoutDurationOption&>(aOption));
	}

TBool TFlushTimeoutDurationOption::operator==(const ML2CapConfigurationOption& aOption) const
	{
	LOG_FUNC
	return *this == (static_cast<const TFlushTimeoutDurationOption&>(aOption));
	}

void TFlushTimeoutDurationOption::operator=(const TFlushTimeoutDurationOption& aOption)
	{
	LOG_FUNC
	iFlushTimeoutDuration = aOption.iFlushTimeoutDuration;
	}
	
TBool TFlushTimeoutDurationOption::operator<=(const TFlushTimeoutDurationOption& aOption) const
	{
	LOG_FUNC
	return iFlushTimeoutDuration <= aOption.iFlushTimeoutDuration;
	}

TBool TFlushTimeoutDurationOption::operator==(const TFlushTimeoutDurationOption& aOption) const
	{
	LOG_FUNC
	return iFlushTimeoutDuration == aOption.iFlushTimeoutDuration;
	}

// ****************************************************************
// *** Quality of Service Option
// ****************************************************************
TQualityOfServiceOption::TQualityOfServiceOption(TQOSServiceType aServiceType,
												 TUint32 aTokenRate,
												 TUint32 aTokenBucketSize,
												 TUint32 aPeakBandwidth,
												 TUint32 aLatency,
												 TUint32 aDelayVariation)
 : iServiceType(aServiceType),
   iTokenRate(aTokenRate),
   iTokenBucketSize(aTokenBucketSize),
   iPeakBandwidth(aPeakBandwidth),
   iLatency(aLatency),
   iDelayVariation(aDelayVariation)
   	{
	LOG_FUNC
   	}

TQualityOfServiceOption::TQualityOfServiceOption(ML2CapConfigurationOption::TStandardOptionValue aStandardOption)
 	{
	LOG_FUNC
 	switch(aStandardOption)
 		{
  		case ML2CapConfigurationOption::EAbsoluteMinimumValue:
 		case ML2CapConfigurationOption::ESpecMinimumValue:
 		    iServiceType = ENoTraffic;
			iTokenRate = KMinTokenRate;
			iTokenBucketSize = KMinTokenBucketSize;
			iPeakBandwidth = KMinPeakBandwidth;
			iLatency = KMinLatency;
			iDelayVariation = KMinDelayVariation;
 			break;
 			
		case ML2CapConfigurationOption::EMaximumValue:
			iServiceType = EGuaranteed;
			// These casts have been added to remove errors encountered
			// using the WINS compiler.  The casts will be optimised
			// out by all other compilers that use class local constants.
			iTokenRate = static_cast<TUint32>(KMaxTokenRate);
			iTokenBucketSize = static_cast<TUint32>(KMaxTokenBucketSize);
			iPeakBandwidth = static_cast<TUint32>(KMaxPeakBandwidth);
			iLatency = static_cast<TUint32>(KMaxLatency);
			iDelayVariation = static_cast<TUint32>(KMaxDelayVariation);
 			break;
 		
		case ML2CapConfigurationOption::EPreferredValue:
		case ML2CapConfigurationOption::ESpecDefaultValue:
		default:
			iServiceType = EBestEffort;
			iTokenRate = KDefaultTokenRate;
			iTokenBucketSize = KDefaultTokenBucketSize;
			iPeakBandwidth = KDefaultPeakBandwidth;
			// These casts have been added to remove errors encountered
			// using the WINS compiler.  The casts will be optimised
			// out by all other compilers that use class local constants.
			iLatency = static_cast<TUint32>(KDefaultLatency);
			iDelayVariation = static_cast<TUint32>(KDefaultDelayVariation);
 			break;
 		};
 	}

TConfigOptionType TQualityOfServiceOption::OptionType() const
	{
	LOG_FUNC
	return EConfigOptionTypeQOS;
	}
	
// Option operators
void TQualityOfServiceOption::operator=(const ML2CapConfigurationOption& aOption)
	{
	LOG_FUNC
	*this = static_cast<const TQualityOfServiceOption&>(aOption);
	}
	
TBool TQualityOfServiceOption::operator<=(const ML2CapConfigurationOption& aOption) const
	{
	LOG_FUNC
	return *this <= static_cast<const TQualityOfServiceOption&>(aOption);
	}

TBool TQualityOfServiceOption::operator==(const ML2CapConfigurationOption& aOption) const
	{
	LOG_FUNC
	return *this == static_cast<const TQualityOfServiceOption&>(aOption);
	}

void TQualityOfServiceOption::operator=(const TQualityOfServiceOption& aOption)
	{
	LOG_FUNC
	iServiceType = aOption.iServiceType;
	iTokenRate = aOption.iTokenRate;
	iTokenBucketSize = aOption.iTokenBucketSize;
	iPeakBandwidth = aOption.iPeakBandwidth;
	iLatency = aOption.iLatency;
	iDelayVariation = aOption.iDelayVariation;
	}
	
TBool TQualityOfServiceOption::operator<=(const TQualityOfServiceOption& aOption) const
	{
	LOG_FUNC
	return (iServiceType <= aOption.iServiceType &&
			iTokenRate <= aOption.iTokenRate &&
			iTokenBucketSize <= aOption.iTokenBucketSize &&
			iPeakBandwidth <= aOption.iPeakBandwidth &&
			iLatency <= aOption.iLatency &&
			iDelayVariation <= aOption.iDelayVariation);
	}

TBool TQualityOfServiceOption::operator==(const TQualityOfServiceOption& aOption) const
	{
	LOG_FUNC
	return (iServiceType == aOption.iServiceType &&
			iTokenRate == aOption.iTokenRate &&
			iTokenBucketSize == aOption.iTokenBucketSize &&
			iPeakBandwidth == aOption.iPeakBandwidth &&
			iLatency == aOption.iLatency &&
			iDelayVariation == aOption.iDelayVariation);
	}


// ****************************************************************
// *** Retransmission and Flow Control Option
// ****************************************************************
TRetransmissionAndFlowControlOption::TRetransmissionAndFlowControlOption(TL2CapChannelMode aLinkMode,
																		 TUint8 aTxWindowSize,
																		 TUint8 aMaxTransmit,
																		 TUint16 aRetransmissionTimeout,
																		 TUint16 aMonitorTimeout,
																		 TUint16 aMaximumPDUSize)
 : iLinkMode(aLinkMode),
   iTxWindowSize(aTxWindowSize),
   iMaxTransmit(aMaxTransmit),
   iRetransmissionTimeout(aRetransmissionTimeout),
   iMonitorTimeout(aMonitorTimeout),
   iMaximumPDUSize(aMaximumPDUSize)
	{
	LOG_FUNC
	}

TRetransmissionAndFlowControlOption::TRetransmissionAndFlowControlOption(TL2CapChannelMode aMode, TBool aFillWithPreferredValues)
	{
	LOG_FUNC

	iLinkMode = aMode;

	if (aFillWithPreferredValues && aMode != EL2CAPBasicMode)
		{
		const TBool useEnhancedModes = (aMode == EL2CAPEnhancedRetransmissionMode || aMode == EL2CAPStreamingMode);

		iTxWindowSize			= useEnhancedModes ? KDefaultEnhancedTxWindowSize : KDefaultLegacyTxWindowSize;
		iMaxTransmit			= KDefaultNumberTransmit;
		iRetransmissionTimeout	= KDefaultRetransmissionTimeout;
		iMonitorTimeout			= KDefaultMonitorTimeout;
		iMaximumPDUSize			= KDefaultMaximumPDUSize;
		}
	else
		{
		iTxWindowSize			= 0;
		iMaxTransmit			= 0;
		iRetransmissionTimeout	= 0;
		iMonitorTimeout			= 0;
		iMaximumPDUSize			= 0;
		}
	}

TConfigOptionType TRetransmissionAndFlowControlOption::OptionType() const
	{
	LOG_FUNC
	return EConfigOptionTypeRTxAndFEC;
	}

TBool TRetransmissionAndFlowControlOption::operator==(const TRetransmissionAndFlowControlOption& aOption) const
	{
	LOG_FUNC
	TBool rCode = ETrue;
	// If both options are basic mode then no need to check other parameters.
	if(!(iLinkMode == EL2CAPBasicMode && aOption.iLinkMode == EL2CAPBasicMode))
		{
		if(!(iLinkMode == aOption.iLinkMode &&
			 iTxWindowSize == aOption.iTxWindowSize &&
			 iMaxTransmit == aOption.iMaxTransmit &&
			 iRetransmissionTimeout == aOption.iRetransmissionTimeout &&
			 iMonitorTimeout == aOption.iMonitorTimeout &&
			 iMaximumPDUSize == aOption.iMaximumPDUSize))
			{
			rCode = EFalse;
			}
		}
	return rCode;
	}

/*static*/ TBool TRetransmissionAndFlowControlOption::EnhancedMaxTransmitLessOrEqual(TUint aLeft, TUint aRight)
	{
	LOG_STATIC_FUNC

	TBool inRelation = EFalse;
	const TUint KInfinity = 0;
	
	if (aRight == KInfinity)
		{
		inRelation = ETrue;
		}
	else if (aLeft == KInfinity)
		{
		inRelation = EFalse;
		}
	else
		{
		inRelation = (aLeft <= aRight);
		}
	return inRelation;
	}

/*static*/ TBool TRetransmissionAndFlowControlOption::IsModeReliable(TL2CapChannelMode aMode)
	{
    LOG_STATIC_FUNC	
	if (aMode == EL2CAPRetransmissionMode || aMode == EL2CAPEnhancedRetransmissionMode)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/*static*/ TBool TRetransmissionAndFlowControlOption::IsModeValid(TL2CapChannelMode aMode)
    {
    LOG_STATIC_FUNC
    switch (aMode)
        {
        case EL2CAPBasicMode:
        case EL2CAPRetransmissionMode:
        case EL2CAPFlowControlMode:
        case EL2CAPEnhancedRetransmissionMode:
        case EL2CAPStreamingMode:
            return ETrue;
        default:
            return EFalse;
        }
    }

#ifdef __FLOG_ACTIVE
void TRetransmissionAndFlowControlOption::GetReadable(TDes& aBuf) const
	{
	LOG_FUNC
	// Adjust KReadableDesSpaceRequired if you change this!

	aBuf.AppendFormat(_L("Mode=%d; "),	iLinkMode);
	aBuf.AppendFormat(_L("TxWin=%d; "),	iTxWindowSize);
	aBuf.AppendFormat(_L("MaxXmit=%d; "),iMaxTransmit);
	aBuf.AppendFormat(_L("RtxTo=%d; "),	iRetransmissionTimeout);
	aBuf.AppendFormat(_L("MonTo=%d; "),	iMonitorTimeout);
	aBuf.AppendFormat(_L("MPS=%d"),		iMaximumPDUSize);
	}
#endif

TL2CapDataControllerConfig::TL2CapDataControllerConfig(TL2CapChannelMode aLinkMode,
                        					           TUint8 aTXWindowSize,
	                           						   TUint8 aMaxTransmit,
	                                                   TUint16 aRetransmissionTimeout,
	                                                   TUint16 aMonitorTimeout,
							                           TUint8 aPeerTXWindowSize,
							                           TUint16 aPeerRetransmissionTimeout,
							                           TUint16 aIncomingMps,
	                                                   TUint8 aPriority)
 : iLinkMode(aLinkMode),
   iTXWindowSize(aTXWindowSize), 
   iMaxTransmit(aMaxTransmit),
   iIncomingMps(aIncomingMps),
   iRetransmissionTimeout(aRetransmissionTimeout),
   iMonitorTimeout(aMonitorTimeout),
   iChannelPriority(aPriority),
   iPeerTXWindowSize(aPeerTXWindowSize),
   iPeerRetransmissionTimeout(aPeerRetransmissionTimeout)
	{
	LOG_FUNC
#ifdef _DEBUG
	if(aLinkMode != EL2CAPBasicMode && aLinkMode != EL2CAPStreamingMode)
		{
   		__ASSERT_DEBUG(aTXWindowSize > 0 && aTXWindowSize <= 63,
                   	   Panic(EL2CAPBadDataPlaneConfiguration));
		}
#endif
   	}
