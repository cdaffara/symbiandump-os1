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
// Assigns default values to various packet context related structures
// 
//

/**
 @file
 @internalComponent
*/

#include "DefaultContextParameters.h"

_LIT(KTxtSimTsy,"SIM");
TName KSimTsyName(KTxtSimTsy);
TName& SimTsyName()
	{
	return KSimTsyName;
	}

_LIT(KApn, "ACME INTERNET PROVIDERS LTD");
_LIT(KProtocolAddress, "i-MODE GATEWAY");
_LIT(KApnR5, "SYMBIAN INTERNET PROVIDER");
_LIT(KProtocolAddressR5, "PROTOCOL ADDRESS");
_LIT(KUsername1, "USER1");
_LIT(KPassword1, "PASSWORD1");
_LIT(KChallenge1, "CHALLENGE1");
_LIT(KResponse1, "RESPONSE1");

_LIT(KUsername2, "USER2");
_LIT(KPassword2, "PASSWORD2");
_LIT(KChallenge2, "NULL");
_LIT(KResponse2, "NULL");

_LIT(KUsername3, "USER3");
_LIT(KPassword3, "PASSWORD3");
_LIT(KChallenge3, "CHALLENGE3");
_LIT(KResponse3, "RESPONSE3");

_LIT(KPrimaryDNS1, "PRIMARYSERVER1");
_LIT(KSecondaryDNS1, "SECONDARYSERVER1");

_LIT(KPrimaryDNS2, "PRIMARYSERVER2");
_LIT(KSecondaryDNS2, "SECONDARYSERVER2");

_LIT(KPrimaryDNS3, "PRIMARYSERVER3");
_LIT(KSecondaryDNS3, "SECONDARYSERVER3");

_LIT8(KAdditionalParam, "ADDITIONALPARAM1");

const TInt KAdditionalParamLength = 16;   

void DefaultContextConfigGPRS(RPacketContext::TContextConfigGPRS& aParam)
	{
	aParam.iPdpType = static_cast<RPacketContext::TProtocolType>(1);
	aParam.iAccessPointName.Copy(KApn);
	aParam.iPdpAddress.Copy(KProtocolAddress);
	aParam.iPdpCompression = 1;
	aParam.iAnonymousAccessReqd = static_cast<RPacketContext::TAnonymousAccess>(1);
	aParam.iUseEdge = ETrue;
	aParam.iProtocolConfigOption.iAuthInfo.iProtocol = static_cast<RPacketContext::TAuthProtocol>(1);
	aParam.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername1);
	aParam.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword1);
	aParam.iProtocolConfigOption.iChallenge.Copy(KChallenge1);
	aParam.iProtocolConfigOption.iResponse.Copy(KResponse1);
	aParam.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDNS1);
	aParam.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDNS1);
	aParam.iProtocolConfigOption.iId = 1;
	}

void DefaultQoSR99_R4Requested(RPacketQoS::TQoSR99_R4Requested& aQosRequested)
	{
	aQosRequested.iReqTrafficClass = static_cast<RPacketQoS::TTrafficClass>(2);
	aQosRequested.iMinTrafficClass = static_cast<RPacketQoS::TTrafficClass>(2);
	aQosRequested.iReqDeliveryOrderReqd = static_cast<RPacketQoS::TDeliveryOrder>(2);
	aQosRequested.iMinDeliveryOrderReqd = static_cast<RPacketQoS::TDeliveryOrder>(2);
	aQosRequested.iReqDeliverErroneousSDU = static_cast<RPacketQoS::TErroneousSDUDelivery>(2);
	aQosRequested.iMinDeliverErroneousSDU = static_cast<RPacketQoS::TErroneousSDUDelivery>(2);
	aQosRequested.iReqMaxSDUSize = 2;
	aQosRequested.iMinAcceptableMaxSDUSize = 2;
	aQosRequested.iReqMaxRate.iUplinkRate = 2;
	aQosRequested.iReqMaxRate.iDownlinkRate = 2;
	aQosRequested.iMinGuaranteedRate.iUplinkRate = 2;
	aQosRequested.iMinGuaranteedRate.iDownlinkRate = 2;
	aQosRequested.iReqBER = static_cast<RPacketQoS::TBitErrorRatio>(2);
	aQosRequested.iMaxBER = static_cast<RPacketQoS::TBitErrorRatio>(2);
	aQosRequested.iReqSDUErrorRatio = static_cast<RPacketQoS::TSDUErrorRatio>(2);
	aQosRequested.iMaxSDUErrorRatio = static_cast<RPacketQoS::TSDUErrorRatio>(2);
	aQosRequested.iReqTrafficHandlingPriority = static_cast<RPacketQoS::TTrafficHandlingPriority>(2);
	aQosRequested.iMinTrafficHandlingPriority = static_cast<RPacketQoS::TTrafficHandlingPriority>(2);
	aQosRequested.iReqTransferDelay = 2;
	aQosRequested.iMaxTransferDelay = 2;
	aQosRequested.iReqGuaranteedRate.iUplinkRate = 2;
	aQosRequested.iReqGuaranteedRate.iDownlinkRate = 2;
	aQosRequested.iMinGuaranteedRate.iUplinkRate = 2;
	aQosRequested.iMinGuaranteedRate.iDownlinkRate = 2;
	}
	
void SecondQoSR99_R4Requested(RPacketQoS::TQoSR99_R4Requested& aQosRequested)
	{
	aQosRequested.iReqTrafficClass = static_cast<RPacketQoS::TTrafficClass>(1);
	aQosRequested.iMinTrafficClass = static_cast<RPacketQoS::TTrafficClass>(1);
	aQosRequested.iReqDeliveryOrderReqd = static_cast<RPacketQoS::TDeliveryOrder>(1);
	aQosRequested.iMinDeliveryOrderReqd = static_cast<RPacketQoS::TDeliveryOrder>(1);
	aQosRequested.iReqDeliverErroneousSDU = static_cast<RPacketQoS::TErroneousSDUDelivery>(1);
	aQosRequested.iMinDeliverErroneousSDU = static_cast<RPacketQoS::TErroneousSDUDelivery>(1);
	aQosRequested.iReqMaxSDUSize = 1;
	aQosRequested.iMinAcceptableMaxSDUSize = 1;
	aQosRequested.iReqMaxRate.iUplinkRate = 1;
	aQosRequested.iReqMaxRate.iDownlinkRate = 1;
	aQosRequested.iMinGuaranteedRate.iUplinkRate = 1;
	aQosRequested.iMinGuaranteedRate.iDownlinkRate = 1;
	aQosRequested.iReqBER = static_cast<RPacketQoS::TBitErrorRatio>(1);
	aQosRequested.iMaxBER = static_cast<RPacketQoS::TBitErrorRatio>(1);
	aQosRequested.iReqSDUErrorRatio = static_cast<RPacketQoS::TSDUErrorRatio>(1);
	aQosRequested.iMaxSDUErrorRatio = static_cast<RPacketQoS::TSDUErrorRatio>(1);
	aQosRequested.iReqTrafficHandlingPriority = static_cast<RPacketQoS::TTrafficHandlingPriority>(1);
	aQosRequested.iMinTrafficHandlingPriority = static_cast<RPacketQoS::TTrafficHandlingPriority>(1);
	aQosRequested.iReqTransferDelay = 1;
	aQosRequested.iMaxTransferDelay = 1;
	aQosRequested.iReqGuaranteedRate.iUplinkRate = 1;
	aQosRequested.iReqGuaranteedRate.iDownlinkRate = 1;
	aQosRequested.iMinGuaranteedRate.iUplinkRate = 1;
	aQosRequested.iMinGuaranteedRate.iDownlinkRate = 1;
	}

#ifdef SYMBIAN_NETWORKING_UMTSR5
void DefaultQoSR5Requested(RPacketQoS::TQoSR5Requested& aQosRequested)
    {
    DefaultQoSR99_R4Requested (aQosRequested);
    aQosRequested.iSignallingIndication = EFalse;
    aQosRequested.iSignallingIndication = RPacketQoS::ESourceStatisticsDescriptorUnknown;
    }
    
void SecondQoSR5Requested(RPacketQoS::TQoSR5Requested& aQosRequested)
    {
    SecondQoSR99_R4Requested (aQosRequested);
    aQosRequested.iSignallingIndication = EFalse;
    aQosRequested.iSignallingIndication = RPacketQoS::ESourceStatisticsDescriptorUnknown;
    }

void DefaultQoSR5Negotiated(RPacketQoS::TQoSR5Negotiated& aQosNegotiated)
	{
	DefaultQoSR99_R4Negotiated(aQosNegotiated);
	aQosNegotiated.iSignallingIndication = EFalse;
    aQosNegotiated.iSignallingIndication = RPacketQoS::ESourceStatisticsDescriptorUnknown;
    }

void SecondQoSR5Negotiated(RPacketQoS::TQoSR5Negotiated& aQosNegotiated)
	{
    SecondQoSR99_R4Negotiated(aQosNegotiated);
    aQosNegotiated.iSignallingIndication = EFalse;
    aQosNegotiated.iSignallingIndication = RPacketQoS::ESourceStatisticsDescriptorUnknown;
    }


#endif
	
void DefaultQoSR99_R4Negotiated(RPacketQoS::TQoSR99_R4Negotiated& aQosNegotiated)
	{
	aQosNegotiated.iTrafficClass = static_cast<RPacketQoS::TTrafficClass>(1);
	aQosNegotiated.iDeliveryOrderReqd = static_cast<RPacketQoS::TDeliveryOrder>(1);
	aQosNegotiated.iDeliverErroneousSDU = static_cast<RPacketQoS::TErroneousSDUDelivery>(1);
	aQosNegotiated.iMaxSDUSize = 1;
	aQosNegotiated.iBER = static_cast<RPacketQoS::TBitErrorRatio>(1);
	aQosNegotiated.iSDUErrorRatio = static_cast<RPacketQoS::TSDUErrorRatio>(1);
	aQosNegotiated.iTrafficHandlingPriority = static_cast<RPacketQoS::TTrafficHandlingPriority>(1);
	aQosNegotiated.iTransferDelay = 1;
	aQosNegotiated.iGuaranteedRate.iUplinkRate = 1;
	aQosNegotiated.iGuaranteedRate.iDownlinkRate = 1;
	aQosNegotiated.iMaxRate.iUplinkRate = 1;
	aQosNegotiated.iMaxRate.iDownlinkRate = 1;
	}
	
void SecondQoSR99_R4Negotiated(RPacketQoS::TQoSR99_R4Negotiated& aQosNegotiated)
	{
	aQosNegotiated.iTrafficClass = static_cast<RPacketQoS::TTrafficClass>(2);
	aQosNegotiated.iDeliveryOrderReqd = static_cast<RPacketQoS::TDeliveryOrder>(2);
	aQosNegotiated.iDeliverErroneousSDU = static_cast<RPacketQoS::TErroneousSDUDelivery>(2);
	aQosNegotiated.iMaxSDUSize = 2;
	aQosNegotiated.iBER = static_cast<RPacketQoS::TBitErrorRatio>(2);
	aQosNegotiated.iSDUErrorRatio = static_cast<RPacketQoS::TSDUErrorRatio>(2);
	aQosNegotiated.iTrafficHandlingPriority = static_cast<RPacketQoS::TTrafficHandlingPriority>(2);
	aQosNegotiated.iTransferDelay = 2;
	aQosNegotiated.iGuaranteedRate.iUplinkRate = 2;
	aQosNegotiated.iGuaranteedRate.iDownlinkRate = 2;
	aQosNegotiated.iMaxRate.iUplinkRate = 2;
	aQosNegotiated.iMaxRate.iDownlinkRate = 2;
	}


/**
Setup default for Rel 5 config. TTlvStruct::AppendItemL is trapped, function returns if 
there is an error.
*/
TInt DefaultContextConfig_R5(RPacketContext::TContextConfig_R5& aParam)
	{
	aParam.iAccessPointName.Copy(KApnR5);
	aParam.iPdpAddress.Copy(KProtocolAddressR5);
	aParam.iPdpType = static_cast<RPacketContext::TProtocolType>(0);
	aParam.iUseEdge = EFalse;
	aParam.iProtocolConfigOption.iAuthInfo.iProtocol  = static_cast<RPacketContext::TAuthProtocol>(2);
	aParam.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername3);
	aParam.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword3);
	aParam.iProtocolConfigOption.iChallenge.Copy(KChallenge3);
	aParam.iProtocolConfigOption.iResponse.Copy(KResponse3);
	aParam.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDNS3);
	aParam.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDNS3);
	aParam.iProtocolConfigOption.iId = 3;
	aParam.iPdpHeaderCompression = static_cast<RPacketContext::THeaderCompression>(2);
	aParam.iPdpDataCompression = static_cast<RPacketContext::TDataCompression>(1);
	
	TBuf8<KAdditionalParamLength> additionalParamData(KAdditionalParam);	
	additionalParamData.SetLength(KAdditionalParamLength);

	RPacketContext::TPcoId pcoId(4);

	RPacketContext::TMiscProtocolBuffer miscBuffer;
	miscBuffer.Zero();

	TPtr8 pcoPtr(const_cast<TUint8*>(miscBuffer.Ptr()), RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtr(const_cast<TUint8*>(additionalParamData.Ptr()),
		additionalParamData.Length(), additionalParamData.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLV(pcoPtr,0);	

	TRAPD(err, theTLV.AppendItemL(pcoId,additionalParamDataPtr));
	if (err != KErrNone)
		{
		return err;
		}

	miscBuffer.SetLength(pcoPtr.Length());

	aParam.iProtocolConfigOption.iMiscBuffer = miscBuffer;
	return KErrNone;

	}

void DefaultContextConfigR99_R4(RPacketContext::TContextConfigR99_R4& aParam)
	{
	aParam.iPdpType = static_cast<RPacketContext::TProtocolType>(3);
	aParam.iPdpAddress.Copy(KProtocolAddress);
	aParam.iAccessPointName.Copy(KApn);
	aParam.iUseEdge = ETrue;
	aParam.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername2);
	aParam.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword2);
	aParam.iProtocolConfigOption.iAuthInfo.iProtocol = static_cast<RPacketContext::TAuthProtocol>(1);
	aParam.iProtocolConfigOption.iChallenge.Copy(KChallenge2);
	aParam.iProtocolConfigOption.iResponse.Copy(KResponse2);
	aParam.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDNS2);
	aParam.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDNS2);
	aParam.iProtocolConfigOption.iId = 2;
	}




TUint8 defaultSrcAddr[RPacketContext::KIPAddressSize] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 1, 1, 1, 1};
TUint8 defaultSrcAddrSubnetMask[RPacketContext::KIPAddressSize] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255};

static void DefaultPacketFilter1(RPacketContext::TPacketFilterV2& entry)
	{
	entry.iId = 0;
	entry.iEvaluationPrecedenceIndex = 1;
	
	for (TUint i = 0; i < RPacketContext::KIPAddressSize; i++)
		{
		entry.iSrcAddr[i] = defaultSrcAddr[i];
		entry.iSrcAddrSubnetMask[i] = defaultSrcAddrSubnetMask[i];
		}
	entry.iProtocolNumberOrNextHeader = 1;
	entry.iSrcPortMin = 1;
	entry.iSrcPortMax = 1;
	entry.iDestPortMin = 1;
	entry.iDestPortMax = 1;
	entry.iIPSecSPI = 1;
	entry.iTOSorTrafficClass = 1;
	entry.iFlowLabel = 1;
	}
	
static void DefaultPacketFilter2(RPacketContext::TPacketFilterV2& entry)
	{
	entry.iId = 1;
	entry.iEvaluationPrecedenceIndex = 2;
	
	for (TUint i = 0; i < RPacketContext::KIPAddressSize; i++)
		{
		entry.iSrcAddr[i] = defaultSrcAddr[i];
		entry.iSrcAddrSubnetMask[i] = defaultSrcAddrSubnetMask[i];
		}
	entry.iProtocolNumberOrNextHeader = 2;
	entry.iSrcPortMin = 2;
	entry.iSrcPortMax = 2;
	entry.iDestPortMin = 2;
	entry.iDestPortMax = 2;
	entry.iIPSecSPI = 2;
	entry.iTOSorTrafficClass = 2;
	entry.iFlowLabel = 2;
	}

static void DefaultPacketFilter3(RPacketContext::TPacketFilterV2& entry)
	{
	entry.iId = 2;
	entry.iEvaluationPrecedenceIndex = 3;
	
	for (TUint i = 0; i < RPacketContext::KIPAddressSize; i++)
		{
		entry.iSrcAddr[i] = defaultSrcAddr[i];
		entry.iSrcAddrSubnetMask[i] = defaultSrcAddrSubnetMask[i];
		}
	entry.iProtocolNumberOrNextHeader = 3;
	entry.iSrcPortMin = 3;
	entry.iSrcPortMax = 3;
	entry.iDestPortMin = 3;
	entry.iDestPortMax = 3;
	entry.iIPSecSPI = 3;
	entry.iTOSorTrafficClass = 3;
	entry.iFlowLabel = 3;
	}

void DefaultTftInfo(TTFTInfo& aTTFTInfo)
	{
	RPacketContext::TPacketFilterV2 entry1, entry2;

	DefaultPacketFilter1(entry1);
	DefaultPacketFilter2(entry2);

	aTTFTInfo.AddPacketFilter(entry1);
	aTTFTInfo.AddPacketFilter(entry2);
	}
	
void SecondDefaultTftInfo(TTFTInfo& aTTFTInfo)
	{
	RPacketContext::TPacketFilterV2 entry;

	DefaultPacketFilter3(entry);

	aTTFTInfo.AddPacketFilter(entry);
	}
	
void CombinedDefaultTftInfo(TTFTInfo& aTTFTInfo)
	{
	RPacketContext::TPacketFilterV2 entry1, entry2, entry3;

	DefaultPacketFilter1(entry1);
	DefaultPacketFilter2(entry2);
	DefaultPacketFilter3(entry3);

	aTTFTInfo.AddPacketFilter(entry1);
	aTTFTInfo.AddPacketFilter(entry2);
	aTTFTInfo.AddPacketFilter(entry3);
	}

void DefaultContextStatus(RPacketContext::TContextStatus& aStatus)
	{
	aStatus = RPacketContext::EStatusInactive;
	}
	
void DefaultServiceStatus(RPacketService::TStatus& aStatus)
	{
	aStatus = RPacketService::EStatusAttached;
	}
	
_LIT(csy, "SIMTSYLOOPBACK.CSY");
_LIT(port, "SIMTSYLOOPBACK::0");
void DefaultDataChannel(RPacketContext::TDataChannelV2& aDataChannel)
	{
	aDataChannel.iCsy = csy;
	aDataChannel.iPort = port;		
	aDataChannel.iChannelId = port;
	}
