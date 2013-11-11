// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTDEF_H
#define __TESTDEF_H

#include <etelpckt.h>

//**********************************************************************************
// General Testing name definitions
//**********************************************************************************

/**
@internalComponent
*/
const TInt DPCKTTSY_TIMEOUT = 100000;

#define DPCKTTSY_PHONE_NAME				_L("DMobile")
#define DPCKTTSY_MODULE_NAME			_L("DPACKET")

/**
@internalComponent
*/
// CoreTsy functionality
#define DPCKTTSY_CALL_BEARER_CAPS		RCall::KBearerCapsProtocolALT
#define DPCKTTSY_CALL_BEARER_SPEED		RCall::EBearerData4800
/**
@internalComponent
*/
#define DPCKTTSY_FAX_BUF_SIZE			(50)
typedef TBuf8<DPCKTTSY_FAX_BUF_SIZE>	TAcqFaxBuf;
#define DPCKTTSY_FAX_DATA				_L8("0123456789")
/**
@internalComponent
*/
#define DPCKTTSY_NETWORK_TYPE			(RTelServer::ENetworkTypeMobileDigital)
#define DPCKTTSY_NUMBER_OF_PHONES		(1)
#define DPCKTTSY_NUMBER_OF_LINES		(4)
/**
@internalComponent
*/
#define DPCKTTSY_CORE_CALL_STATUS		RCall::EStatusIdle
#define DPCKTTSY_CALL_DURATION1			(10)
#define DPCKTTSY_CORE_LINE_CAPS			(RLine::KCapsVoice)
#define DPCKTTSY_CORE_LINE_STATUS		RCall::EStatusIdle
#define DPCKTTSY_CORE_LINE_STATUS2		RCall::EStatusRinging
#define DPCKTTSY_CORE_PHONE_STATUS		RPhone::EDetectedPresent
#define DPCKTTSY_GET_PROGRESS_SPEED		(11)
#define DPCKTTSY_MYFAX					_L("MY FAX") 

#define DPCKTTSY_PACKET_NAME			_L("PACKET_NAME")
#define DPCKTTSY_CONTEXT_NAME			_L("PACKET_CONTEXT1")
#define DPCKTTSY_QOS_NAME				_L("PACKET_QOS1")

#define DPCKTTSY_DNS_SERVER1			_L8("DNS_SERVER1")
#define DPCKTTSY_DNS_SERVER2			_L8("DNS_SERVER2")

//**********************************************************************************
// Testing RPacket
//**********************************************************************************
/**
@internalComponent
*/
#define DPCKTTSY_CONTEXT_ID				_L("NEW_CONTEXT")

#define DPCKTTSY_PACKET_STATUS1			RPacketService::EStatusActive
#define DPCKTTSY_PACKET_STATUS2			RPacketService::EStatusSuspended


#define DPCKTTSY_CONTEXT_INFO_NAME0		_L("Context0")
#define DPCKTTSY_CONTEXT_INFO_NAME1		_L("Context1")
#define DPCKTTSY_CONTEXT_INFO_STATUS0	RPacketContext::EStatusActivating
#define DPCKTTSY_CONTEXT_INFO_STATUS1	RPacketContext::EStatusActive

#define DPCKTTSY_REGISTRATION_STATUS1	RPacketService::ERegisteredOnHomeNetwork
#define DPCKTTSY_REGISTRATION_STATUS2	RPacketService::ERegisteredRoaming

#define DPCKTTSY_CURRENT_MS_CLASS		RPacketService::EMSClassCircuitSwitchedOnly
#define DPCKTTSY_MAX_MS_CLASS			RPacketService::EMSClassAlternateMode
#define DPCKTTSY_SET_MS_CLASS			RPacketService::EMSClassAlternateMode
#define DPCKTTSY_NEW_MS_CLASS			RPacketService::EMSClassCircuitSwitchedOnly

#define DPCKTTSY_MISC_CAPS			(RPacketService::KCapsSuspendSupported |\
							RPacketService::KCapsAASupported |\
							RPacketService::KCapsNetworkAvailabilitySupported |\
							RPacketService::KCapsSetDefaultContextSupported|\
							RPacketService::KCapsHSDPASupported|\
							RPacketService::KCapsEGPRSSupported|\
							RPacketService::KCapsHSUPASupported|\
							RPacketService::KCapsMBMSSupported)

#define DPCKTTSY_DYNAMIC_CAPS1			(RPacketService::KCapsManualAttach |\
							 RPacketService::KCapsManualDetach |\
							 RPacketService::KCapsActivate |\
							 RPacketService::KCapsHSUPA |\
							 RPacketService::KCapsMBMS)

#define DPCKTTSY_DYNAMIC_CAPS2			(RPacketService::KCapsManualAttach |\
							 RPacketService::KCapsRxContextActivationReq)

#define DPCKTTSY_PREFERRED_BEARER1		RPacketService::EBearerPacketSwitched
#define DPCKTTSY_PREFERRED_BEARER2		RPacketService::EBearerCircuitSwitched

#define DPCKTTSY_ATTACH_MODE1			RPacketService::EAttachWhenPossible
#define DPCKTTSY_ATTACH_MODE2			RPacketService::EAttachWhenNeeded

#define DPCKTTSY_NETWORK_MODE1			RPacketService::EReleaseMode97_98
#define DPCKTTSY_NETWORK_MODE2			RPacketService::EReleaseMode99

#define DPCKTTSY_NUMBER_OF_NIFS					(2)
#define DPCKTTSY_NIF_NAME_LENGTH				(20)
#define DPCKTTSY_NIF_CONTEXT1			_L("NifContext1")
#define DPCKTTSY_NIF_CONTEXT2			_L("NifContext2")
#define DPCKTTSY_NUMBER_OF_CONTEXTS_IN_NIF1		(1)
#define DPCKTTSY_NUMBER_OF_CONTEXTS_IN_NIF2		(2)
#define DPCKTTSY_CONTEXT1_IN_NIF1		_L("Context1InNif1")
#define DPCKTTSY_CONTEXT1_IN_NIF2		_L("Context1InNif2")
#define DPCKTTSY_CONTEXT2_IN_NIF2		_L("Context2InNif2")
#define DPCKTTSY_INTERNAL_CONTEXT		RPacketService::EInternalContext
#define DPCKTTSY_EXTERNAL_CONTEXT		RPacketService::EExternalContext

//**********************************************************************************
// Testing MBMS APIs
//**********************************************************************************
#define DPCKTTSY_MBMS_NETWORKSERVICE_SUPPORTED				EMbmsSupported 
#define DPCKTTSY_MBMS_NETWORKSERVICE_NOT_SUPPORTED			EMbmsNotSupported
#define DPCKTTSY_MBMS_NETWORKSERVICE_UNKNOWN				EMbmsSupportUnknown
													
#define DPCKTTSY_MBMS_ATTEMPT_GSMATTACH_TRUE				(TBool)ETrue 
#define DPCKTTSY_MBMS_ATTEMPT_GSMATTACH_FALSE			    (TBool)EFalse

#define DPCKTTSY_MBMS_SERVICELIST_COUNT_ADD		    		3
#define DPCKTTSY_MBMS_SERVICELIST_COUNT_REMOVE				1

#define DPCKTTSY_MBMS_ACTION_ADD			   				EAddEntries
#define DPCKTTSY_MBMS_ACTION_REMOVE			   				ERemoveEntries
#define DPCKTTSY_MBMS_ACTION_REMOVEALL		   				ERemoveAllEntries

#define DPCKTTSY_MBMS_SERVICEMODE_ONE						KBroadcast
#define DPCKTTSY_MBMS_ACCESSBEARER_ONE						E2GBearer
#define DPCKTTSY_MBMS_AVAILABILITYSTATUS_ONE				EMbmsAvailabilityUnknown
#define DPCKTTSY_MBMS_MCC_ONE								01
#define DPCKTTSY_MBMS_MNC_ONE								01
#define DPCKTTSY_MBMS_SERVICEID_ONE							01

#define DPCKTTSY_MBMS_SERVICEMODE_TWO						KBroadcast
#define DPCKTTSY_MBMS_ACCESSBEARER_TWO						E3GBearer
#define DPCKTTSY_MBMS_AVAILABILITYSTATUS_TWO				EMbmsUnavailable
#define DPCKTTSY_MBMS_MCC_TWO								02
#define DPCKTTSY_MBMS_MNC_TWO								22
#define DPCKTTSY_MBMS_SERVICEID_TWO							222

#define DPCKTTSY_MBMS_SERVICEMODE_THREE						KBroadcast
#define DPCKTTSY_MBMS_ACCESSBEARER_THREE					E2GBearer
#define DPCKTTSY_MBMS_AVAILABILITYSTATUS_THREE				EMbmsAvailable
#define DPCKTTSY_MBMS_MCC_THREE								03
#define DPCKTTSY_MBMS_MNC_THREE								33
#define DPCKTTSY_MBMS_SERVICEID_THREE						333

#define DPCKTTSY_MBMS_SERVICEMODE_INVALID					KSelectedBroadcast
#define DPCKTTSY_MBMS_ACCESSBEARER_INVALID					E3GBearer
#define DPCKTTSY_MBMS_AVAILABILITYSTATUS_INVALID			EMbmsUnavailable
#define DPCKTTSY_MBMS_MCC_INVALID							0
#define DPCKTTSY_MBMS_MNC_INVALID							0
#define DPCKTTSY_MBMS_SERVICEID_INVALID						0

#define DPCKTTSY_MBMS_SERVICEPRIORITY_ONE 					EMbmsHighServicePriority

#define DPCKTTSY_MBMS_CONTEXT_STATUS_RECEIVING				RPacketContext::EStatusReceiving
#define DPCKTTSY_MBMS_CONTEXT_STATUS_ACTIVE					RPacketContext::EStatusActive

#define DPCKTTSY_MBMS_ACTION_ONE      						 EAddEntries
#define DPCKTTSY_MBMS_SESSIONID_ONE   						 12
#define DPCKTTSY_MBMS_SESSIONID_INVALID 					 200
#define DPCKTTSY_MBMS_MONITORSERVICE_NUMBER_OF_CONTEXTS	     5
#define DPCKTTSY_MBMS_MONITORSERVICE_MAX_NUMBER_OF_CONTEXTS  10

#define DPCKTTSY_MBMS_ACTIVESERVICE_NUMBER_OF_CONTEXTS		 6
#define DPCKTTSY_MBMS_ACTIVESERVICE_MAX_NUMBER_OF_CONTEXTS   11

#define DPCKTTSY_CONTEXT_STATUS_RECEIVING						RPacketContext::EStatusReceiving
#define DPCKTTSY_MBMS_SESSION_FLAG_TRUE							1
#define DPCKTTSY_MBMS_DELAY										200000
#define DPCKTTSY_STANDARD_DELAY									100000

#define DPCKTTSY_MBMS_CONTEXT_SESSION_ID_COUNT					3
#define DPCKTTSY_MBMS_CONTEXT_SESSION_ID_ONE					10000
#define DPCKTTSY_MBMS_CONTEXT_SESSION_ID_TWO					20000
#define DPCKTTSY_MBMS_CONTEXT_SESSION_ID_THREE					30000

//**********************************************************************************
// Testing RPacketContext
//**********************************************************************************
/**
@internalComponent
*/
#define DPCKTTSY_CONTEXT_STATUS1		RPacketContext::EStatusSuspended
#define DPCKTTSY_CONTEXT_STATUS2		RPacketContext::EStatusActive

#define DPCKTTSY_BYTES_SENT1			(TUint32)(100000)
#define DPCKTTSY_OVERFLOW_COUNTER_SENT1	(TUint32)(100)
#define DPCKTTSY_BYTES_RCVD1			(TUint32)(400000)
#define DPCKTTSY_OVERFLOW_COUNTER_RCVD1	(TUint32)(400)

#define DPCKTTSY_BYTES_SENT2			(TUint32)(200000)
#define DPCKTTSY_OVERFLOW_COUNTER_SENT2	(TUint32)(200)
#define DPCKTTSY_BYTES_RCVD2			(TUint32)(800000)
#define DPCKTTSY_OVERFLOW_COUNTER_RCVD2	(TUint32)(400)

#define DPCKTTSY_BYTES_SENT3			(TUint32)(400000)
#define DPCKTTSY_OVERFLOW_COUNTER_SENT3	(TUint32)(400)
#define DPCKTTSY_BYTES_RCVD3			(TUint32)(200000)
#define DPCKTTSY_OVERFLOW_COUNTER_RCVD3	(TUint32)(400)

#define DPCKTTSY_LAST_ERROR_CAUSE		(TInt)(0xEF4EFFFE)  // extended error + core error

#define DPCKTTSY_NUMBER_OF_CONTEXTS		(1)
#define DPCKTTSY_MAX_NUMBER_OF_CONTEXTS (5)

#define DPCKTTSY_CSY					_L("ECUART")
#define DPCKTTSY_PORT					_L("COMM::1")
#define DPCKTTSY_CSY_ASYNC				_L("IRCOMM")
#define DPCKTTSY_PORT_ASYNC				_L("COMM::2")
#define DPCKTTSY_CHANNEL_ID				_L("CHANNEL_ID:1")

#define DPCKTTSY_RCVD_GRANULARITY		(TInt)(0x1000)
#define DPCKTTSY_SENT_GRANULARITY		(TInt)(0x1000)

#define DPCKTTSY_NUMBER_OF_PACKET_FILTERS			(2)

#define DPCKTTSY_FILTER_ID1							(1)
#define DPCKTTSY_EVALUATION_PRECEDENCE_INDEX1		(0)

#define DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_0			(TUint8) 0xCF
#define DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_1			(TUint8) 0xFF
#define DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_2			(TUint8) 0xFF
#define DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_3			(TUint8) 0x11

#define DPCKTTSY_DOMAIN_SUBNET_MASK1_0				(TUint8) 0xC0
#define DPCKTTSY_DOMAIN_SUBNET_MASK1_1				(TUint8) 0xC1
#define DPCKTTSY_DOMAIN_SUBNET_MASK1_2				(TUint8) 0xF2
#define DPCKTTSY_DOMAIN_SUBNET_MASK1_3				(TUint8) 0x51

#define DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER1		(6)
#define DPCKTTSY_MINIMUM_SOURCE_PORT_NUMBER1		(2)
#define DPCKTTSY_MAXIMUM_SOURCE_PORT_NUMBER1		(65000)
#define DPCKTTSY_MINIMUM_DESTINATION_PORT_NUMBER1	(8)
#define DPCKTTSY_MAXIMUM_DESTINATION_PORT_NUMBER1	(65100)
#define DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1	(0x3F8C)

#define DPCKTTSY_FILTER_ID2							(2)
#define DPCKTTSY_EVALUATION_PRECEDENCE_INDEX2		(255)

#define DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_0			(TUint8) 0x21
#define DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_1			(TUint8) 0xD2
#define DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_2			(TUint8) 0x3F
#define DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_3			(TUint8) 0x1A

#define DPCKTTSY_DOMAIN_SUBNET_MASK2_0				(TUint8) 0x2A
#define DPCKTTSY_DOMAIN_SUBNET_MASK2_1				(TUint8) 0x2B
#define DPCKTTSY_DOMAIN_SUBNET_MASK2_2				(TUint8) 0xFF
#define DPCKTTSY_DOMAIN_SUBNET_MASK2_3				(TUint8) 0x3F

#define DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER2		(8)
#define DPCKTTSY_SECURITY_PARAMETER_INDEX2			(0x0F88F000)
#define DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS2	(0x2F77)
#define DPCKTTSY_FLOW_LABEL							(0x0F11F000)

#define DPCKTTSY_PRIMARY_CONTEXT_NAME			_L("PrimaryContextName")

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
#define DPCKTTSY_HSDPA_CATEGORY			            (TUint)(5)
#define DPCKTTSY_HSUPA_CATEGORY	                    (TUint)(6)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW


//**********************************************************************************
// Testing RPacketContext
//**********************************************************************************

//**********************************************************************************
// Context Configuration
//**********************************************************************************
/**
@internalComponent
*/
#define DPCKTTSY_PDP_TYPE1					RPacketContext::EPdpTypeIPv4
#define DPCKTTSY_ACCESS_POINT1				_L8("ACME INTERNET PROVIDERS LTD")
#define DPCKTTSY_PDP_ADDRESS1				_L8("i-MODE GATEWAY")
#define DPCKTTSY_COMPRESSION1				RPacketContext::KPdpDataCompression
#define DPCKTTSY_ANONYMOUS_ACCESS1			RPacketContext::ENotRequired
#define DPCKTTSY_EGPRS_REQUIRED				TRUE
#define DPCKTTSY_USER_NAME1					_L8("ONYEARA")
#define DPCKTTSY_PASSWORD1					_L8("1234")
#define DPCKTTSY_PROTOCOL_CONFIG1			_L8("ARBITRARY INFORMATION")
#define DPCKTTSY_PROTOCOL_TYPE1_0			RPacketContext::EProtocolNone
#define DPCKTTSY_PROTOCOL_TYPE1_1			RPacketContext::EProtocolPAP
#define DPCKTTSY_PROTOCOL_TYPE1_2			RPacketContext::EProtocolCHAP

#define DPCKTTSY_PDP_TYPE2					RPacketContext::EPdpTypeIPv6
#define DPCKTTSY_ACCESS_POINT2				_L8("SYMBIAN EMPLOYEE INTRANET")
#define DPCKTTSY_PDP_ADDRESS2				_L8("A WAP PORTAL")
#define DPCKTTSY_COMPRESSION2				RPacketContext::KPdpHeaderCompression
#define DPCKTTSY_ANONYMOUS_ACCESS2			RPacketContext::ERequired
#define DPCKTTSY_EGPRS_NOTREQUIRED			FALSE
#define DPCKTTSY_USER_NAME2					_L8("SYMBIAN EMPLOYEE X")
#define DPCKTTSY_PASSWORD2					_L8("1111")
#define DPCKTTSY_PROTOCOL_CONFIG2			_L8("YET MORE ARBITRARY INFORMATION")
#define DPCKTTSY_PROTOCOL_TYPE2_0			RPacketContext::EProtocolNone
#define DPCKTTSY_PROTOCOL_TYPE2_1			RPacketContext::EProtocolPAP
#define DPCKTTSY_PROTOCOL_TYPE2_2			RPacketContext::EProtocolCHAP
#define DPCKTTSY_NWICONTEXT_FALSE			FALSE
#define DPCKTTSY_NWICONTEXT_TRUE			TRUE
#define DPCKTTSY_PFICONTEXT_SIG				(RPacketContext::ESignalling)
#define DPCKTTSY_PFICONTEXT_SMS				(RPacketContext::ESms)

#define DPCKTTSY_DATACOMPRESSION_NONE		RPacketContext::ENoDataCompression
#define DPCKTTSY_DATACOMPRESSION_DEFAULT 	RPacketContext::EManufacturerDefaultDataCompression
#define DPCKTTSY_DATACOMPRESSION_EV42		RPacketContext::EV42
#define DPCKTTSY_DATACOMPRESSION_EV44		RPacketContext::EV44

#define DPCKTTSY_HEADERCOMPRESSION_NONE		RPacketContext::ENoHeaderCompression
#define DPCKTTSY_HEADERCOMPRESSION_DEFAULT 	RPacketContext::EManufacturerDefaultHeaderCompression
#define DPCKTTSY_HEADERCOMPRESSION_ERFC1144	RPacketContext::ERfc1144
#define DPCKTTSY_HEADERCOMPRESSION_ERFC2507	RPacketContext::ERfc2507
#define DPCKTTSY_HEADERCOMPRESSION_EROHC	RPacketContext::ERohc

#define DPCKTTSY_CONNECTION_RATE 			55000
//**********************************************************************************
// Testing RPacketQoS
//**********************************************************************************
/**
@internalComponent
*/
#define DPCKTTSY_QOS_CONFIG1			RPacketQoS::EUnspecifiedReliabilityClass

//**********************************************************************************
// QoS Configuration
//**********************************************************************************
/**
@internalComponent
*/
#define DPCKTTSY_PROFILE_NAME				_L("QoSProfile1")
#define DPCKTTSY_DELAY_MIN1					RPacketQoS::EDelayClass1
#define DPCKTTSY_DELAY_REQ1					RPacketQoS::EDelayClass2
#define DPCKTTSY_MEAN_THROUGHPUT_MIN1		RPacketQoS::EMeanThroughput200
#define DPCKTTSY_MEAN_THROUGHPUT_REQ1		RPacketQoS::EMeanThroughput2000
#define DPCKTTSY_PEAK_THROUGHPUT_MIN1		RPacketQoS::EPeakThroughput16000
#define DPCKTTSY_PEAK_THROUGHPUT_REQ1		RPacketQoS::EPeakThroughput64000
#define DPCKTTSY_PRECEDENCE_MIN1			RPacketQoS::EPriorityLowPrecedence
#define DPCKTTSY_PRECEDENCE_REQ1			RPacketQoS::EPriorityMediumPrecedence
#define DPCKTTSY_RELIABILITY_MIN1			RPacketQoS::EReliabilityClass1
#define DPCKTTSY_RELIABILITY_REQ1			RPacketQoS::EReliabilityClass2

#define DPCKTTSY_DELAY_NEG2					RPacketQoS::EDelayClass2
#define DPCKTTSY_MEAN_THROUGHPUT_NEG2		RPacketQoS::EMeanThroughput100
#define DPCKTTSY_PEAK_THROUGHPUT_NEG2		RPacketQoS::EPeakThroughput8000
#define DPCKTTSY_PRECEDENCE_NEG2			RPacketQoS::EPriorityMediumPrecedence
#define DPCKTTSY_RELIABILITY_NEG2			RPacketQoS::EReliabilityClass2

#define DPCKTTSY_PRECEDENCE					(RPacketQoS::EPriorityMediumPrecedence|\
												RPacketQoS::EPriorityLowPrecedence)
#define DPCKTTSY_DELAY						(RPacketQoS::EDelayClass2|\
												RPacketQoS::EDelayClass3|\
												RPacketQoS::EDelayClass4)
#define DPCKTTSY_RELIABILITY				RPacketQoS::EReliabilityClass2
#define DPCKTTSY_PEAK_THROUGHPUT			(RPacketQoS::EPeakThroughput8000|\
												RPacketQoS::EPeakThroughput16000)
#define DPCKTTSY_MEAN_THROUGHPUT			RPacketQoS::EMeanThroughput100
#define DPCKTTSY_ANONYMOUS_ACCESS			RPacketContext::ERequired

#define DPCKTTSY_TRAFFIC_CLASS_MIN			RPacketQoS::ETrafficClassStreaming
#define DPCKTTSY_TRAFFIC_CLASS_REQ			RPacketQoS::ETrafficClassConversational

#define DPCKTTSY_DELIVERY_ORDER_MIN			RPacketQoS::EDeliveryOrderNotRequired
#define DPCKTTSY_DELIVERY_ORDER_REQ			RPacketQoS::EDeliveryOrderRequired

#define DPCKTTSY_ERRONEOUS_SDU_DELIVERY_MIN	RPacketQoS::EErroneousSDUDeliveryNotRequired
#define DPCKTTSY_ERRONEOUS_SDU_DELIVERY_REQ	RPacketQoS::EErroneousSDUDeliveryRequired

#define DPCKTTSY_MAX_SDU_SIZE_MIN			(10)
#define DPCKTTSY_MAX_SDU_SIZE_REQ			(1500)

#define DPCKTTSY_MAX_UPLINK_BITRATE_MIN		(1000)
#define DPCKTTSY_MAX_UPLINK_BITRATE_REQ		(2000)
#define DPCKTTSY_MAX_DOWNLINK_BITRATE_MIN	(1000)
#define DPCKTTSY_MAX_DOWNLINK_BITRATE_REQ	(2000)

#define DPCKTTSY_BER_MAX					RPacketQoS::EBERFivePerHundred
#define DPCKTTSY_BER_REQ					RPacketQoS::EBERFivePerThousand

#define DPCKTTSY_SDU_ERROR_RATIO_MAX		RPacketQoS::ESDUErrorRatioOnePerTen
#define DPCKTTSY_SDU_ERROR_RATIO_REQ		RPacketQoS::ESDUErrorRatioOnePerHundred

#define DPCKTTSY_TRFFC_HNDLG_PRIORITY_MIN	RPacketQoS::ETrafficPriority1
#define DPCKTTSY_TRFFC_HNDLG_PRIORITY_REQ	RPacketQoS::ETrafficPriority2

#define DPCKTTSY_TRANSFER_DELAY_MAX			(20)
#define DPCKTTSY_TRANSFER_DELAY_REQ			(10)

#define DPCKTTSY_GRNT_UPLINK_BITRATE_MIN	(1000)
#define DPCKTTSY_GRNT_UPLINK_BITRATE_REQ	(2000)
#define DPCKTTSY_GRNT_DOWNLINK_BITRATE_MIN	(1000)
#define DPCKTTSY_GRNT_DOWNLINK_BITRATE_REQ	(2000)

#define DPCKTTSY_TRAFFIC_CLASS_NEG			RPacketQoS::ETrafficClassConversational
#define DPCKTTSY_DELIVERY_ORDER_NEG			RPacketQoS::EDeliveryOrderRequired
#define DPCKTTSY_ERRONEOUS_SDU_DELIVERY_NEG	RPacketQoS::EErroneousSDUDeliveryRequired
#define DPCKTTSY_MAX_SDU_SIZE_NEG			(1500)
#define DPCKTTSY_MAX_UPLINK_BITRATE_NEG		(2000)
#define DPCKTTSY_MAX_DOWNLINK_BITRATE_NEG	(2000)
#define DPCKTTSY_BER_NEG					RPacketQoS::EBERFivePerThousand
#define DPCKTTSY_SDU_ERROR_RATIO_NEG		RPacketQoS::ESDUErrorRatioOnePerHundred
#define DPCKTTSY_TRFFC_HNDLG_PRIORITY_NEG	RPacketQoS::ETrafficPriority2
#define DPCKTTSY_TRANSFER_DELAY_NEG			(10)
#define DPCKTTSY_GRNT_UPLINK_BITRATE_NEG	(2000)
#define DPCKTTSY_GRNT_DOWNLINK_BITRATE_NEG	(2000)

#define DPCKTTSY_TRAFFIC_CLASS				(RPacketQoS::ETrafficClassConversational|\
												RPacketQoS::ETrafficClassStreaming)
#define DPCKTTSY_DELIVERY_ORDER				(RPacketQoS::EDeliveryOrderRequired|\
												RPacketQoS::EDeliveryOrderNotRequired)
#define DPCKTTSY_ERRONEOUS_SDU_DELIVERY		(RPacketQoS::EErroneousSDUDeliveryRequired|\
												RPacketQoS::EErroneousSDUDeliveryNotRequired)
#define DPCKTTSY_BER						(RPacketQoS::EBERFivePerHundred|\
												RPacketQoS::EBERFivePerThousand)
#define DPCKTTSY_SDU_ERROR_RATIO			(RPacketQoS::ESDUErrorRatioOnePerTen|\
												RPacketQoS::ESDUErrorRatioOnePerHundred)
#define DPCKTTSY_TRFFC_HNDLG_PRIORITY		(RPacketQoS::ETrafficPriority1|\
												RPacketQoS::ETrafficPriority2)

#define DPCKTTSY_TFT_NO_FILTERS				(1)

#define DPCKTTSY_SRC_STATS					(RPacketQoS::ESourceSpeech)

#define DPCKTTSY_SRC_STATSDESC				(RPacketQoS::ESourceStatisticsDescriptorSpeech)
#define DPCKTTSY_SIG_IND					(ETrue)

#define DPCKTTSY_AUTH_TOKEN					(_L8("TESTXX"))
#define DPCKTTSY_COMP_NUMBER1				(022)
#define DPCKTTSY_IPFLOW_NUMBER1				(033)
#define DPCKTTSY_COMP_NUMBER2				(003)
#define DPCKTTSY_IPFLOW_NUMBER2				(004)

#endif
