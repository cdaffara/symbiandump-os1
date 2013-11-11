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
// Compares various packet context related structures
// 
//

/**
 @file
 @internalComponent
*/

#include "CompareContextParameters.h"

/**
 @return ETrue if the TContextInfo parameters match, EFalse if they don't
 */
TBool CompareContextInfo(const TContextInfo& aFirst, const TContextInfo& aSecond)
	{
	if (aFirst.iContextId != aSecond.iContextId)
		{
		return EFalse;
		}
	if (aFirst.iStatus != aSecond.iStatus)
		{
		return EFalse;
		}
	return ETrue;
	}
	
/**
 @return ETrue if the RPacketQoS::TBitRate parameters match, EFalse if they don't
 */
TBool CompareBitRate(const RPacketQoS::TBitRate &aFirst, const RPacketQoS::TBitRate &aSecond)
	{
	if (aFirst.iDownlinkRate != aSecond.iDownlinkRate)
		{
		return EFalse;
		}
	if (aFirst.iUplinkRate != aSecond.iUplinkRate)
		{
		return EFalse;
		}
	return ETrue;
	}
	
/**
 @return ETrue if the RPacketQoS::TQoSR99_R4Requested parameters match, EFalse if they don't
 */
TBool CompareUMTSQoSReq(const RPacketQoS::TQoSR99_R4Requested &aFirst, const RPacketQoS::TQoSR99_R4Requested &aSecond)
	{
	if (aFirst.iReqTrafficClass != aSecond.iReqTrafficClass)
		{
		return EFalse;
		}
	if (aFirst.iMinTrafficClass != aSecond.iMinTrafficClass)			
		{
		return EFalse;
		}
	if (aFirst.iReqDeliveryOrderReqd != aSecond.iReqDeliveryOrderReqd)		
		{
		return EFalse;
		}
	if (aFirst.iMinDeliveryOrderReqd != aSecond.iMinDeliveryOrderReqd)		
		{
		return EFalse;
		}
	if (aFirst.iReqDeliverErroneousSDU != aSecond.iReqDeliverErroneousSDU)	
		{
		return EFalse;
		}
	if (aFirst.iMinDeliverErroneousSDU != aSecond.iMinDeliverErroneousSDU)	
		{
		return EFalse;
		}
	if (aFirst.iReqMaxSDUSize != aSecond.iReqMaxSDUSize)				
		{
		return EFalse;
		}
	if (aFirst.iMinAcceptableMaxSDUSize != aSecond.iMinAcceptableMaxSDUSize)	
		{
		return EFalse;
		}
	if (!CompareBitRate(aFirst.iReqMaxRate, aSecond.iReqMaxRate))
		{
		return EFalse;
		}
	if (!CompareBitRate(aFirst.iMinAcceptableMaxRate, aSecond.iMinAcceptableMaxRate))
		{
		return EFalse;
		}
	if (aFirst.iReqBER != aSecond.iReqBER)					
		{
		return EFalse;
		}
	if (aFirst.iMaxBER != aSecond.iMaxBER)					
		{
		return EFalse;
		}
	if (aFirst.iReqSDUErrorRatio != aSecond.iReqSDUErrorRatio)			
		{
		return EFalse;
		}
	if (aFirst.iMaxSDUErrorRatio != aSecond.iMaxSDUErrorRatio)			
		{
		return EFalse;
		}
	if (aFirst.iReqTrafficHandlingPriority != aSecond.iReqTrafficHandlingPriority)
		{
		return EFalse;
		}
	if (aFirst.iMinTrafficHandlingPriority != aSecond.iMinTrafficHandlingPriority)
		{
		return EFalse;
		}
	if (aFirst.iReqTransferDelay != aSecond.iReqTransferDelay)			
		{
		return EFalse;
		}
	if (aFirst.iMaxTransferDelay != aSecond.iMaxTransferDelay)			
		{
		return EFalse;
		}
	if (aFirst.iReqGuaranteedRate.iDownlinkRate != aSecond.iReqGuaranteedRate.iDownlinkRate)
		{
		return EFalse;
		}
	if (aFirst.iReqGuaranteedRate.iUplinkRate != aSecond.iReqGuaranteedRate.iUplinkRate)
		{
		return EFalse;
		}
	if (aFirst.iMinGuaranteedRate.iDownlinkRate != aSecond.iMinGuaranteedRate.iDownlinkRate)
		{
		return EFalse;
		}
	if (aFirst.iMinGuaranteedRate.iUplinkRate != aSecond.iMinGuaranteedRate.iUplinkRate)
		{
		return EFalse;
		}
	return ETrue;
	}


#ifdef SYMBIAN_NETWORKING_UMTSR5
TBool CompareUMTSQoSReq(const RPacketQoS::TQoSR5Requested &aFirst, const RPacketQoS::TQoSR5Requested &aSecond)
    {
    TBool result = CompareUMTSQoSReq(static_cast<const RPacketQoS::TQoSR99_R4Requested&>(aFirst),
        static_cast<const RPacketQoS::TQoSR99_R4Requested&>(aSecond));
    
    if (!result) 
        {
        return EFalse;
        }
        
	if (aFirst.iSignallingIndication != aSecond.iSignallingIndication)
		{
		return EFalse;
		}

	if (aFirst.iSourceStatisticsDescriptor != aSecond.iSourceStatisticsDescriptor)
		{
		return EFalse;
		}

    return ETrue;    
    }
#endif


/**
 @return ETrue if the TContextConfig parameters match, EFalse if they don't
 */
TBool CompareUMTSQoSReq(const TContextConfig& aFirst, const TContextConfig& aSecond)
	{
#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested firstUMTSQoSRequest, secondUMTSQoSRequest;
	aFirst.GetUMTSQoSReq(firstUMTSQoSRequest);
	aSecond.GetUMTSQoSReq(secondUMTSQoSRequest);

#else
	RPacketQoS::TQoSR99_R4Requested firstUMTSQoSRequest, secondUMTSQoSRequest;
	aFirst.GetUMTSQoSReq(firstUMTSQoSRequest);
	aSecond.GetUMTSQoSReq(secondUMTSQoSRequest);
#endif
	
	return CompareUMTSQoSReq(firstUMTSQoSRequest, secondUMTSQoSRequest);
	}




/**
 @return ETrue if the IP address parameters match, EFalse if they don't
 */
TBool CompareIPAddress(const TUint8* aFirst, const TUint8* aSecond)
	{
	for (TUint i = 0; i < RPacketContext::KIPAddressSize; i++)
		{
		if (aFirst[i] != aSecond[i])
			{
			return EFalse;
			}
		}
	return ETrue;
	}

/**
 @return ETrue if the RPacketContext::TPacketFilterV2 parameters match, EFalse if they don't
 */
TBool ComparePacketFilterV2(const RPacketContext::TPacketFilterV2 &aFirst, const RPacketContext::TPacketFilterV2 &aSecond)
	{
	if (aFirst.iId != aSecond.iId)
		{
		return EFalse;
		}
	if (aFirst.iEvaluationPrecedenceIndex != aSecond.iEvaluationPrecedenceIndex)
		{
		return EFalse;
		}
	if (!CompareIPAddress((TUint8*)aFirst.iSrcAddr, (TUint8*)aSecond.iSrcAddr))
		{
		return EFalse;
		}
	if (!CompareIPAddress((TUint8*)aFirst.iSrcAddrSubnetMask, (TUint8*)aSecond.iSrcAddrSubnetMask))
		{
		return EFalse;
		}
	if (aFirst.iProtocolNumberOrNextHeader != aSecond.iProtocolNumberOrNextHeader)
		{
		return EFalse;
		}
	if (aFirst.iSrcPortMin != aSecond.iSrcPortMin)
		{
		return EFalse;
		}
	if (aFirst.iSrcPortMax != aSecond.iSrcPortMax)
		{
		return EFalse;
		}
	if (aFirst.iDestPortMin != aSecond.iDestPortMin)
		{
		return EFalse;
		}
	if (aFirst.iDestPortMax != aSecond.iDestPortMax)
		{
		return EFalse;
		}
	if (aFirst.iIPSecSPI != aSecond.iIPSecSPI)
		{
		return EFalse;
		}
	if (aFirst.iTOSorTrafficClass != aSecond.iTOSorTrafficClass)
		{
		return EFalse;
		}
	if (aFirst.iFlowLabel != aSecond.iFlowLabel)
		{
		return EFalse;
		}
	return ETrue;
	}
	
/**
 @return ETrue if the TTFTInfo parameters match, EFalse if they don't
 */
TBool CompareTFTInfo(TTFTInfo &aFirst, TTFTInfo &aSecond)
	{
	if (aFirst.FilterCount() != aSecond.FilterCount())
		{
		return EFalse;
		}
	
	for (TUint8 i = 0; i < aFirst.FilterCount(); i++)
		{
		RPacketContext::TPacketFilterV2 firstPacketFilter, secondPacketFilter;
		aFirst.GetPacketFilter(firstPacketFilter);
		aSecond.GetPacketFilter(secondPacketFilter);
		if (!ComparePacketFilterV2(firstPacketFilter, secondPacketFilter))
			{
			return EFalse;
			}
		}
	return ETrue;
	}

/**
 @return ETrue if the TContextConfig parameters match, EFalse if they don't
 */
TBool CompareTFTInfo(const TContextConfig& aFirst, const TContextConfig& aSecond)
	{
	TTFTInfo firstTFTInfo, secondTFTInfo;
	aFirst.GetTFTInfo(firstTFTInfo);
	aSecond.GetTFTInfo(secondTFTInfo);
	return CompareTFTInfo(firstTFTInfo, secondTFTInfo);
	}


/**
 @return ETrue if the RPacketQoS::TQoSR99_R4Negotiated parameters match, EFalse if they don't
 */
TBool CompareUMTSQoSNeg(const RPacketQoS::TQoSR99_R4Negotiated &aFirst, const RPacketQoS::TQoSR99_R4Negotiated &aSecond)
	{
	if (aFirst.iTrafficClass != aSecond.iTrafficClass)
		{
		return EFalse;
		}
	if (aFirst.iDeliveryOrderReqd != aSecond.iDeliveryOrderReqd)
		{
		return EFalse;
		}
	if (aFirst.iDeliverErroneousSDU != aSecond.iDeliverErroneousSDU)
		{
		return EFalse;
		}
	if (aFirst.iMaxSDUSize != aSecond.iMaxSDUSize)
		{
		return EFalse;
		}
	if (!CompareBitRate(aFirst.iMaxRate, aSecond.iMaxRate))
		{
		return EFalse;
		}
	if (aFirst.iBER != aSecond.iBER)
		{
		return EFalse;
		}
	if (aFirst.iSDUErrorRatio != aSecond.iSDUErrorRatio)
		{
		return EFalse;
		}
	if (aFirst.iTrafficHandlingPriority != aSecond.iTrafficHandlingPriority)
		{
		return EFalse;
		}
	if (aFirst.iTransferDelay != aSecond.iTransferDelay)
		{
		return EFalse;
		}
	if (!CompareBitRate(aFirst.iGuaranteedRate, aSecond.iGuaranteedRate))
		{
		return EFalse;
		}
	return ETrue;
	}

/**
 @return ETrue if the RPacketQoS::TBitRate parameters match, EFalse if they don't
 */
TBool CompareUMTSQoSNeg(const TContextConfig& aFirst, const TContextConfig& aSecond)
	{
	RPacketQoS::TQoSR99_R4Negotiated firstUMTSQoS, secondUMTSQoS;
	aFirst.GetUMTSQoSNeg(firstUMTSQoS);
	aSecond.GetUMTSQoSNeg(secondUMTSQoS);
	return CompareUMTSQoSNeg(firstUMTSQoS, secondUMTSQoS);
	}





/**
 @return ETrue if the RPacketContext::TAuthInfo parameters match, EFalse if they don't
 */
TBool CompareAuthInfo(const RPacketContext::TAuthInfo &aFirst, const RPacketContext::TAuthInfo &aSecond)
	{
	if (aFirst.iProtocol != aSecond.iProtocol)
		{
		return EFalse;
		}
    if (aFirst.iUsername != aSecond.iUsername)
		{
		return EFalse;
		}
    if (aFirst.iPassword != aSecond.iPassword)
		{
		return EFalse;
		}
	return ETrue;
	}

/**
 @return ETrue if the RPacketContext::TDnsInfoV2 parameters match, EFalse if they don't
 */
TBool CompareDnsInfoV2(const RPacketContext::TDnsInfoV2 &aFirst, const RPacketContext::TDnsInfoV2 &aSecond)
	{
	if (aFirst.iPrimaryDns != aSecond.iPrimaryDns)
		{
		return EFalse;
		}
	if (aFirst.iSecondaryDns != aSecond.iSecondaryDns)
		{
		return EFalse;
		}
	return ETrue;
	}
	
/**
 @return ETrue if the RPacketContext::TProtocolConfigOptionV2 parameters match, EFalse if they don't
 */
TBool CompareProtocolConfigOptionV2(const RPacketContext::TProtocolConfigOptionV2 &aFirst, const RPacketContext::TProtocolConfigOptionV2 &aSecond)
	{
	if (!CompareAuthInfo(aFirst.iAuthInfo, aSecond.iAuthInfo))
		{
		return EFalse;
		}
	if (aFirst.iChallenge != aSecond.iChallenge)
		{
		return EFalse;
		}
	if (aFirst.iResponse != aSecond.iResponse)
		{
		return EFalse;
		}
	if (aFirst.iId != aSecond.iId)
		{
		return EFalse;
		}
	if (!CompareDnsInfoV2(aFirst.iDnsAddresses, aSecond.iDnsAddresses))
		{
		return EFalse;
		}
	if (aFirst.iMiscBuffer != aSecond.iMiscBuffer)
		{
		return EFalse;
		}
	return ETrue;
	}
	
/**
 @return ETrue if the RPacketContext::TContextConfigGPRS parameters match, EFalse if they don't
 */
TBool CompareContextConfigGPRS(const RPacketContext::TContextConfigGPRS &aFirst, const RPacketContext::TContextConfigGPRS &aSecond)
	{
	if (aFirst.iPdpType != aSecond.iPdpType)
		{
		return EFalse;
		}
	if (aFirst.iAccessPointName != aSecond.iAccessPointName)
		{
		return EFalse;
		}
	if (aFirst.iPdpAddress != aSecond.iPdpAddress)
		{
		return EFalse;
		}
	if (aFirst.iPdpCompression != aSecond.iPdpCompression)
		{
		return EFalse;
		}
	if (aFirst.iAnonymousAccessReqd != aSecond.iAnonymousAccessReqd)
		{
		return EFalse;
		}
	if (aFirst.iUseEdge != aSecond.iUseEdge)
		{
		return EFalse;
		}
	if (!CompareProtocolConfigOptionV2(aFirst.iProtocolConfigOption, aSecond.iProtocolConfigOption))
		{
		return EFalse;
		}
	if (aFirst.iNWIContext != aSecond.iNWIContext)
		{
		return EFalse;
		}
	return ETrue;
	}

/**
 @return ETrue if the RPacketContext::TContextConfig_R5 parameters match, EFalse if they don't
 */
TBool CompareContextConfigR5(const RPacketContext::TContextConfig_R5 &aFirst, const RPacketContext::TContextConfig_R5 &aSecond)
	{
	if (aFirst.iPdpType != aSecond.iPdpType)
		{
		return EFalse;
		}
	if (aFirst.iAccessPointName != aSecond.iAccessPointName)
		{
		return EFalse;
		}
	if (aFirst.iPdpAddress != aSecond.iPdpAddress)
		{
		return EFalse;
		}
	if (aFirst.iPdpHeaderCompression != aSecond.iPdpHeaderCompression)
		{
		return EFalse;
		}
	if (aFirst.iPdpDataCompression != aSecond.iPdpDataCompression)
		{
		return EFalse;
		}
	if (aFirst.iUseEdge != aSecond.iUseEdge)
		{
		return EFalse;
		}
	if (!CompareProtocolConfigOptionV2(aFirst.iProtocolConfigOption, aSecond.iProtocolConfigOption))
		{
		return EFalse;
		}
	return ETrue;
	}

/**
 @return ETrue if the RPacketContext::TContextConfigR99_R4 parameters match, EFalse if they don't
 */
TBool CompareContextConfigR99R4(const RPacketContext::TContextConfigR99_R4 &aFirst, const RPacketContext::TContextConfigR99_R4 &aSecond)
	{
	if (aFirst.iPdpType != aSecond.iPdpType)
			{
			return EFalse;
			}
		if (aFirst.iAccessPointName != aSecond.iAccessPointName)
			{
			return EFalse;
			}
		if (aFirst.iPdpAddress != aSecond.iPdpAddress)
			{
			return EFalse;
			}
		if (aFirst.iUseEdge != aSecond.iUseEdge)
			{
			return EFalse;
			}
		if (!CompareProtocolConfigOptionV2(aFirst.iProtocolConfigOption, aSecond.iProtocolConfigOption))
			{
			return EFalse;
			}
		return ETrue;
	}

/**
 @return ETrue if the TContextConfig parameters match, EFalse if they don't
 */
TBool CompareContextConfigGPRS(const TContextConfig& aFirst, const TContextConfig& aSecond)
	{
	RPacketContext::TContextConfigGPRS firstUMTSConfig, secondUMTSConfig;
	aFirst.GetContextConfig(firstUMTSConfig);
	aSecond.GetContextConfig(secondUMTSConfig);
	return CompareContextConfigGPRS(firstUMTSConfig, secondUMTSConfig);
	}

/**
 @return ETrue if the RPacketQoS::TBitRate parameters match, EFalse if they don't
 */
TBool CompareAccessPointName(const RPacketContext::TGSNAddress &aFirst, const RPacketContext::TGSNAddress &aSecond)
	{
	if (aFirst.Compare(aSecond) == 0)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
 @return ETrue if the TContextConfig parameters match, EFalse if they don't
 */
TBool CompareAccessPointName(const TContextConfig& aFirst, const TContextConfig& aSecond)
	{
	RPacketContext::TGSNAddress firstAccessPointName, secondAccessPointName;
	aFirst.GetAccessPointName(firstAccessPointName);
	aSecond.GetAccessPointName(secondAccessPointName);
	return CompareAccessPointName(firstAccessPointName, secondAccessPointName);
	}	

/**
 @return ETrue if the RPacketContext::TProtocolAddress parameters match, EFalse if they don't
 */
TBool ComparePdpAddress(const RPacketContext::TProtocolAddress &aFirst, const RPacketContext::TProtocolAddress &aSecond)
	{
	if (aFirst.Compare(aSecond) == 0)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
 @return ETrue if the TContextConfig parameters match, EFalse if they don't
 */
TBool ComparePdpAddress(const TContextConfig& aFirst, const TContextConfig& aSecond)
	{
	RPacketContext::TProtocolAddress firstPdpAddress, secondPdpAddress;
	aFirst.GetPdpAddress(firstPdpAddress);
	aSecond.GetPdpAddress(secondPdpAddress);
	return ComparePdpAddress(firstPdpAddress, secondPdpAddress);
	}

/**
 @return ETrue if the TContextConfig parameters match, EFalse if they don't
 */
TBool CompareContextConfig(const TContextConfig& aFirst, const TContextConfig& aSecond)
	{
	if (!CompareUMTSQoSReq(aFirst, aSecond))
		{
		return EFalse;
		}

	if (!CompareTFTInfo(aFirst, aSecond))
		{
		return EFalse;
		}
	if (!CompareUMTSQoSNeg(aFirst, aSecond))
		{
		return EFalse;
		}
	if (!CompareContextConfigGPRS(aFirst, aSecond))
		{
		return EFalse;
		}
	
	TUint firstPdpCompression, secondPdpCompression;
	aFirst.GetPdpCompression(firstPdpCompression);
	aSecond.GetPdpCompression(secondPdpCompression);
	if (firstPdpCompression != secondPdpCompression)
		{
		return EFalse;
		}
	
	RPacketContext::TProtocolType firstPdpType, secondPdpType;
	aFirst.GetPdpType(firstPdpType);
	aSecond.GetPdpType(secondPdpType);
	if (firstPdpType != secondPdpType)
		{
		return EFalse;
		}
	
	if (!CompareAccessPointName(aFirst, aSecond))
		{
		return EFalse;
		}
	
	if (!ComparePdpAddress(aFirst, aSecond))
		{
		return EFalse;
		}
	
	return ETrue;
	}
	
/**
 @return ETrue if the TContextParameters parameters match, EFalse if they don't
 */
TBool CompareContextParameters(const TContextParameters& aFirst, const TContextParameters& aSecond)
	{
	if (aFirst.iContextType != aSecond.iContextType)
		{
		return EFalse;
		}
	
	if (!CompareContextInfo(aFirst.iContextInfo, aSecond.iContextInfo))
		{
		return EFalse;
		}
	
	if (!CompareContextConfig(aFirst.iContextConfig, aSecond.iContextConfig))
		{
		return EFalse;
		}
	
	if (aFirst.iReasonCode != aSecond.iReasonCode)
		{
		return EFalse;
		}
	
	if (aFirst.iTFTOperationCode != aSecond.iTFTOperationCode)
		{
		return EFalse;
		}
	
	return ETrue;
	}

/**
 @return ETrue if the RPacketContext::TDataChannelV2 parameters match, EFalse if they don't
 */
TBool CompareDataChannel(const RPacketContext::TDataChannelV2& aFirst, const RPacketContext::TDataChannelV2& aSecond)
	{
	if (aFirst.iCsy.Compare(aSecond.iCsy) != 0)
		{
		return EFalse;
		}
	if (aFirst.iPort.Compare(aSecond.iPort) != 0)
		{
		return EFalse;
		}
	if (aFirst.iChannelId.Compare(aSecond.iChannelId) != 0)
		{
		return EFalse;
		}
	return ETrue;
	}

/**
 @return ETrue if the TTFTInfo parameter matches the TFTs of the packet context aPacketContext, EFalse if they don't
 */
TBool CompareTFTInfo(TTFTInfo& aTFTInfo, RPacketContext& aPacketContext)
	{
	RPacketContext::TPacketFilterV2 packetFilter;
	RPacketContext::TPacketFilterV2Pckg packetFilterPckg(packetFilter);
	RPacketContext::TPacketFilterV2 expectedTftInfo;
	TRequestStatus status;
	TInt count;

	// make sure the count matches up
	aPacketContext.EnumeratePacketFilters(status, count);
	User::WaitForRequest(status);
	if (status != KErrNone)
		{
		return EFalse;
		}
	if (count != aTFTInfo.FilterCount())
		{
		return EFalse;
		}
		
	// The filters indexed by the second parameter of RPacketContext::GetPacketFilterInfo may not be contiguous,
	// so we go through each filter assigned to context aPacketContext and try to find a match in aTFTInfo
	TInt matchingCount = 0;
	for (TUint packetContextIndex = 0; packetContextIndex < KMaxNumberOfPacketFilters; packetContextIndex++)
		{
		TBool foundMatch = EFalse;
		aPacketContext.GetPacketFilterInfo(status, packetContextIndex, packetFilterPckg);
		User::WaitForRequest(status);
		aTFTInfo.SetToFirst();
		for (TInt tftInfoIndex = 0; tftInfoIndex < aTFTInfo.FilterCount() && !foundMatch; tftInfoIndex++)
			{
			aTFTInfo.NextPacketFilter(expectedTftInfo);
			if (ComparePacketFilterV2(packetFilter, expectedTftInfo))
				{
				matchingCount++;
				if (matchingCount == aTFTInfo.FilterCount())
					{
					return ETrue;
					}
				else
					{
					foundMatch = ETrue;
					}
				}
			}
		}
	return EFalse;
	}

/**
 @return ETrue if the context status of context aContext names matches that of aContextStatus, EFalse if they don't
 */
TBool CompareContextInfo(const RPacketService& aPacketService, const TDesC& aContextName, RPacketContext::TContextStatus aContextStatus)
	{
	TRequestStatus status;
	RPacketService::TContextInfo contextInfo;
	TInt numContexts, dummy;
	aPacketService.EnumerateContexts(status, numContexts, dummy);
	User::WaitForRequest(status);
	for (TInt i = 0; i < numContexts; i++)
		{
		aPacketService.GetContextInfo(status, i, contextInfo);
		User::WaitForRequest(status);
		if (aContextName.Compare(contextInfo.iName) == 0)
			{
			if (status != KErrNone)
				{
				return EFalse;
				}
			if (contextInfo.iStatus != aContextStatus)
				{
				return EFalse;
				}
			return ETrue;
			}
		}
	return EFalse;
	}
