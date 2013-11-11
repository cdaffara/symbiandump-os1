// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
	
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <networking/qosparameters.h>
#include <networking/pfqosparser.h>

#if MM_ETEL_API
#include "uscl_qos.h"
#else
#include <etelqos.h>
#endif

const TUint KTransferDelayMax	= 4100;
const TUint KMaxSDUMax			= 1520;
const TUint KMaxBitRate			= 2048;
// SBLP - MAT string size
const TUint KMatSize			= 255;


#ifdef SYMBIAN_NETWORKING_UMTSR5  
// Requested QoS
class TQoSRequested : public RPacketQoS::TQoSR5Requested
	{
public:
	TQoSRequested();
	void Reset();
	void ParsePolicyData(CExtensionPolicy* aExtensionPolicy);
	void ClearMinimumParameters();
public:
	TUint iHeaderCompression;
	};

// Negotiated QoS
class TQoSNegotiated : public RPacketQoS::TQoSR5Negotiated
	{
public:
	TQoSNegotiated() 
		{
		iTrafficClass = RPacketQoS::ETrafficClassUnspecified;
		iDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
		iDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
		iMaxSDUSize = 0;
		iMaxRate.iUplinkRate = 0;
		iMaxRate.iDownlinkRate = 0;
		iBER = RPacketQoS::EBERUnspecified;
		iSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
		iTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
		iTransferDelay = 0;
		iGuaranteedRate.iUplinkRate = 0;
		iGuaranteedRate.iDownlinkRate = 0;
		iSignallingIndication = 0;
		iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;
		}
	};
#else
// Requested QoS
class TQoSRequested : public RPacketQoS::TQoSR99_R4Requested
	{
public:
	TQoSRequested();
	void Reset();
	void ParsePolicyData(CExtensionPolicy* aExtensionPolicy);
	void ClearMinimumParameters();
public:
	TUint iHeaderCompression;
	};

// Negotiated QoS
class TQoSNegotiated : public RPacketQoS::TQoSR99_R4Negotiated
	{
public:
	TQoSNegotiated() 
		{
		iTrafficClass = RPacketQoS::ETrafficClassUnspecified;
		iDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
		iDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
		iMaxSDUSize = 0;
		iMaxRate.iUplinkRate = 0;
		iMaxRate.iDownlinkRate = 0;
		iBER = RPacketQoS::EBERUnspecified;
		iSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
		iTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
		iTransferDelay = 0;
		iGuaranteedRate.iUplinkRate = 0;
		iGuaranteedRate.iDownlinkRate = 0;
		}
	};

#endif //SYMBIAN_NETWORKING_UMTSR5

// Packet filter
class TPacketFilter : public RPacketContext::TPacketFilterV2
	{
public:
	TPacketFilter()
		{
		TPtr8 tmp((TUint8*)this, sizeof(RPacketContext::TPacketFilterV2), sizeof(RPacketContext::TPacketFilterV2));
		tmp.FillZ();
		}
	};


// Parameter mapping methods
class ParameterMapper
	{
public:
	static void MapGenericToRel99(const TQoSParameters& aSpec, RPacketQoS::TQoSR99_R4Requested& aGprs);
	static TInt CombineParameters(const RPacketQoS::TQoSR99_R4Requested& aGprs, TQoSParameters& aFlowSpec);
	static void CombineDefault(const TQoSRequested& aDefault, TQoSRequested& aPolicy);
	static void CombineOverride(const TQoSRequested& aOverride, TQoSRequested& aPolicy);
private:
	inline static TUint Min(TUint a, TUint b) { return a < b ? a : b; };
	};



// For Flow extension parameters (SBLP)

class RFlowExtensionParams //: public RPacketContext::CTFTMediaAuthorizationV3
	{
public:
	~RFlowExtensionParams();
	TBool ParsePolicyDataForSblp(CExtensionPolicy * aPolicy);
	RFlowExtensionParams& operator=(const RFlowExtensionParams& aFlowExtParam);
	RFlowExtensionParams();

	// SBLP extension parameters
	RPacketContext::TAuthorizationToken iAuthorizationToken;
	RArray<RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier>	iFlowIds;
	};


#endif

