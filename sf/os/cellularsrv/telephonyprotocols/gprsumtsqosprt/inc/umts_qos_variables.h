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
// umts_qos_variables.h - QoS variable names
//

#ifndef __UMTS_QOS_VARIABLES_H__
#define __UMTS_QOS_VARIABLES_H__

_LIT(KDescDeliveryOfErroneusSduRequested,	"DeliveryOfErroneusSduRequested");
_LIT(KDescDeliveryOrderRequested,			"DeliveryOrderRequested");
_LIT(KDescErrorRatioRequested,				"ErrorRatioRequested");
_LIT(KDescGuaBitrateDownlinkRequested,		"GuaBitrateDownlinkRequested");
_LIT(KDescGuaBitrateUplinkRequested,		"GuaBitrateUplinkRequested");
_LIT(KDescMaxBitrateDownlinkRequested,		"MaxBitrateDownlinkRequested");
_LIT(KDescMaxBitrateUplinkRequested,		"MaxBitrateUplinkRequested");
_LIT(KDescMaxSduSizeRequested,				"MaxSduSizeRequested");
_LIT(KDescPriorityRequested,				"PriorityRequested");
_LIT(KDescResidualBerRequested,				"ResidualBerRequested");
_LIT(KDescTrafficClassRequested,			"TrafficClassRequested");
_LIT(KDescTransferDelayRequested,			"TransferDelayRequested");
#ifdef SYMBIAN_NETWORKING_UMTSR5  
_LIT(KDescSignallingIndicatorRequested,	"SignallingIndicatorRequested");
_LIT(KDescSrcStatDescRequested,			"SrcStatDescRequested");
#endif // SYMBIAN_NETWORKING_UMTSR5 

_LIT(KDescDeliveryOfErroneusSduMinimum,		"DeliveryOfErroneusSduMinimum");
_LIT(KDescDeliveryOrderMinimum,				"DeliveryOrderMinimum");
_LIT(KDescErrorRatioMinimum,				"ErrorRatioMinimum");
_LIT(KDescGuaBitrateDownlinkMinimum,		"GuaBitrateDownlinkMinimum");
_LIT(KDescGuaBitrateUplinkMinimum,			"GuaBitrateUplinkMinimum");
_LIT(KDescMaxBitrateDownlinkMinimum,		"MaxBitrateDownlinkMinimum");
_LIT(KDescMaxBitrateUplinkMinimum,			"MaxBitrateUplinkMinimum");
_LIT(KDescMaxSduSizeMinimum,				"MaxSduSizeMinimum");
_LIT(KDescPriorityMinimum,					"PriorityMinimum");
_LIT(KDescResidualBerMinimum,				"ResidualBerMinimum");
_LIT(KDescTrafficClassMinimum,				"TrafficClassMinimum");
_LIT(KDescTransferDelayMinimum,				"TransferDelayMinimum");


_LIT(KDescTrafficClassNegotiated,			"TrafficClassNegotiated");
_LIT(KDescDeliveryOrderNegotiated,			"DeliveryOrderNegotiated");
_LIT(KDescDeliveryOfErroneusSduNegotiated,	"DeliveryOfErroneusSduNegotiated");
_LIT(KDescResidualBerNegotiated,			"ResidualBerNegotiated");
_LIT(KDescErrorRatioNegotiated,				"ErrorRatioNegotiated");
_LIT(KDescPriorityNegotiated,				"PriorityNegotiated");
_LIT(KDescTransferDelayNegotiated,			"TransferDelayNegotiated");
_LIT(KDescMaxSduSizeNegotiated,				"MaxSduSizeNegotiated");
_LIT(KDescMaxBitrateUplinkNegotiated,		"MaxBitrateUplinkNegotiated");
_LIT(KDescMaxBitrateDownlinkNegotiated,		"MaxBitrateDownlinkNegotiated");
_LIT(KDescGuaBitrateUplinkNegotiated,		"GuaBitrateUplinkNegotiated");
_LIT(KDescGuaBitrateDownlinkNegotiated,		"GuaBitrateDownlinkNegotiated");
#ifdef SYMBIAN_NETWORKING_UMTSR5 
_LIT(KDescSignallingIndicatorNegotiated,	"SignallingIndicatorNegotiated");
_LIT(KDescSrcStatDescNegotiated,			"SrcStatDescNegotiated");
#endif // SYMBIAN_NETWORKING_UMTSR5

_LIT(KDescHeaderCompression,				"HeaderCompression");
_LIT(KDescErrorCode,						"ErrorCode");

_LIT(KDescSblpMediaAuthorizationToken,    	"SblpMediaAuthorizationToken");
_LIT(KDescSblpMediaComponentNumber,    	    "SblpMediaComponentNumber");
_LIT(KDescSblpIPFlowNumber,    			    "SblpIPFlowNumber");

#ifdef SYMBIAN_NETWORKING_UMTSR5 
_LIT(KDescIMSSignallingIndicator,    		"IMSSignallingIndicator");
#endif // SYMBIAN_NETWORKING_UMTSR5

#endif
