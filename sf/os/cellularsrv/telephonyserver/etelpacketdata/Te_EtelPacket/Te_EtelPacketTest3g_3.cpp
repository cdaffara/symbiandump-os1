// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTest3G_3.cpp
// JetStream Packet data tests.
// This file tests the 3G (WCDMA) Packet Service Quality of Service (QoS) functionality: 
// TestCase 3G.3 
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"
#include <es_sock.h>
#include <e32svr.h>
#include "etelQoS.h"


#include "testdef.h"

#include "Te_EtelPacketTest3g_3.h"
#include "Te_etelpckt_defs.h"

CEtelPacketTest3G_3::CEtelPacketTest3G_3()
/** Constructor
 *  Stores the name of this test case.
 */
{
	SetTestStepName(_L("3GPacketQoS"));
}


CEtelPacketTest3G_3::~CEtelPacketTest3G_3()
/**
 * Destructor
 */
{
}

enum TVerdict CEtelPacketTest3G_3::doTestStepL( void )
/**
 * Test step 3G.3 calls the Packet Service QoS methods (RPacketQoS) using the 
 * Release 99/Release 4 QoS structures and classes.
 */
{
	RPhone pcktPhone;
	TInt ret=pcktPhone.Open(iTelServer,DPCKTTSY_PHONE_NAME);
	CHECKPOINT_EX(ret==KErrNone,CHP_OPEN_PHONE);

	RPacketService wcdmaService;
	ret=wcdmaService.Open(pcktPhone);
    CHECKPOINT(ret,KErrNone,CHP_SRVS_CASE("A.3G1"));

// Open a Primary Context	
	RPacketContext wcdmaContext;
	TName contextName;
	ret=wcdmaContext.OpenNewContext(wcdmaService, contextName);
	CHECKPOINT(ret,KErrNone,CHP_CNTXT_CASE("B.3G1"));

// Open a Rel99/Rel4 Quality of Service (QoS) profile
	RPacketQoS	r99r4QoS;
	TName qosName;
	ret = r99r4QoS.OpenNewQoS(wcdmaContext, qosName);
	CHECKPOINT(ret,KErrNone,CHP_QOS_CASE("C.3G1"));
	CHECKPOINT(qosName.Compare(DPCKTTSY_QOS_NAME),KErrNone,CHP_QOS_CASE("C.3G1"));

	RPacketQoS aQos;
	ret = aQos.OpenExistingQoS(wcdmaContext, qosName);
	CHECKPOINT(ret,KErrNone,CHP_QOS_CASE("C.3G2"));
	aQos.Close();

// Local TRequestStatus variables used in the tests
	TRequestStatus reqStatus;
	TRequestStatus notifyStatus;

// Post a 'Profile Changed' notification. 
// Note that although the "Traffic handling priority" is set, it is not required for the 
// "Conversational" Traffic class.   
	RPacketQoS::TQoSR99_R4Negotiated aNotifyQoS;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> aNotifyQoSPckg(aNotifyQoS);

	r99r4QoS.NotifyProfileChanged(notifyStatus, aNotifyQoSPckg);
	User::WaitForRequest(notifyStatus);

	CHECKPOINT(notifyStatus.Int(),KErrNone,CHP_QOS_CASE("C.3G7"));
	
	CHECKPOINT(aNotifyQoS.ExtensionId(),TPacketDataConfigBase::KConfigRel99Rel4,CHP_QOS_CASE("C.3G7"));

	CHECKPOINT(aNotifyQoS.iTrafficClass, DPCKTTSY_TRAFFIC_CLASS_MIN, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iDeliveryOrderReqd, DPCKTTSY_DELIVERY_ORDER_MIN, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iDeliverErroneousSDU, DPCKTTSY_ERRONEOUS_SDU_DELIVERY_MIN, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iMaxSDUSize, DPCKTTSY_MAX_SDU_SIZE_MIN, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iMaxRate.iUplinkRate, DPCKTTSY_MAX_UPLINK_BITRATE_MIN, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iMaxRate.iDownlinkRate, DPCKTTSY_MAX_DOWNLINK_BITRATE_MIN, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iBER, DPCKTTSY_BER_MAX, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iSDUErrorRatio, DPCKTTSY_SDU_ERROR_RATIO_MAX, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iTrafficHandlingPriority, DPCKTTSY_TRFFC_HNDLG_PRIORITY_MIN, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iTransferDelay, DPCKTTSY_TRANSFER_DELAY_MAX, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iGuaranteedRate.iUplinkRate, DPCKTTSY_GRNT_UPLINK_BITRATE_MIN, CHP_QOS_CASE("C.3G7"));
	CHECKPOINT(aNotifyQoS.iGuaranteedRate.iDownlinkRate, DPCKTTSY_GRNT_DOWNLINK_BITRATE_MIN, CHP_QOS_CASE("C.3G7"));


// Cancel Request	
	r99r4QoS.NotifyProfileChanged(notifyStatus, aNotifyQoSPckg);
	r99r4QoS.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(notifyStatus);
	CHECKPOINT(notifyStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3G7"));

// Get the Rel99/Rel4 QoS  Capabilities
	RPacketQoS::TQoSCapsR99_R4 aQoSCaps;
	TPckg<RPacketQoS::TQoSCapsR99_R4> aQoSCapsPckg(aQoSCaps);
	
	r99r4QoS.GetProfileCapabilities(reqStatus, aQoSCapsPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_QOS_CASE("C.3G6"));

	CHECKPOINT(aQoSCaps.ExtensionId(),TPacketDataConfigBase::KConfigRel99Rel4,CHP_QOS_CASE("C.3G6"));
	CHECKPOINT(aQoSCaps.iTrafficClass,DPCKTTSY_TRAFFIC_CLASS,CHP_QOS_CASE("C.3G6"));
	CHECKPOINT(aQoSCaps.iDeliveryOrderReqd,DPCKTTSY_DELIVERY_ORDER,CHP_QOS_CASE("C.3G6"));
	CHECKPOINT(aQoSCaps.iDeliverErroneousSDU,DPCKTTSY_ERRONEOUS_SDU_DELIVERY,CHP_QOS_CASE("C.3G6"));
	CHECKPOINT(aQoSCaps.iBER,DPCKTTSY_BER,CHP_QOS_CASE("C.3G6"));
	CHECKPOINT(aQoSCaps.iSDUErrorRatio,DPCKTTSY_SDU_ERROR_RATIO,CHP_QOS_CASE("C.3G6"));
	CHECKPOINT(aQoSCaps.iTrafficHandlingPriority,DPCKTTSY_TRFFC_HNDLG_PRIORITY,CHP_QOS_CASE("C.3G6"));

// Cancel Request	
	r99r4QoS.GetProfileCapabilities(reqStatus, aQoSCapsPckg);
	r99r4QoS.CancelAsyncRequest(EPacketQoSGetProfileCaps);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3G6"));

// Set Rel99/R4 QoS Profile Parameters
	RPacketQoS::TQoSR99_R4Requested aQoSReqConfig;
	TPckg<RPacketQoS::TQoSR99_R4Requested> aQoSReqPckg(aQoSReqConfig);

	aQoSReqConfig.iMinTrafficClass = DPCKTTSY_TRAFFIC_CLASS_MIN;
	aQoSReqConfig.iMinDeliveryOrderReqd = DPCKTTSY_DELIVERY_ORDER_MIN;
	aQoSReqConfig.iMinDeliverErroneousSDU = DPCKTTSY_ERRONEOUS_SDU_DELIVERY_MIN;
	aQoSReqConfig.iMinAcceptableMaxSDUSize = DPCKTTSY_MAX_SDU_SIZE_MIN;
	aQoSReqConfig.iMinAcceptableMaxRate.iUplinkRate = DPCKTTSY_MAX_UPLINK_BITRATE_MIN;
	aQoSReqConfig.iMinAcceptableMaxRate.iDownlinkRate = DPCKTTSY_MAX_DOWNLINK_BITRATE_MIN;
	aQoSReqConfig.iMaxBER = DPCKTTSY_BER_MAX;
	aQoSReqConfig.iMaxSDUErrorRatio = DPCKTTSY_SDU_ERROR_RATIO_MAX;
	aQoSReqConfig.iMaxTransferDelay = DPCKTTSY_TRANSFER_DELAY_MAX;
	aQoSReqConfig.iMinGuaranteedRate.iUplinkRate = DPCKTTSY_GRNT_UPLINK_BITRATE_MIN;
	aQoSReqConfig.iMinGuaranteedRate.iDownlinkRate = DPCKTTSY_GRNT_DOWNLINK_BITRATE_MIN;
	
	aQoSReqConfig.iReqTrafficClass = DPCKTTSY_TRAFFIC_CLASS_REQ;
	aQoSReqConfig.iReqDeliveryOrderReqd = DPCKTTSY_DELIVERY_ORDER_REQ;
	aQoSReqConfig.iReqDeliverErroneousSDU = DPCKTTSY_ERRONEOUS_SDU_DELIVERY_REQ;
	aQoSReqConfig.iReqMaxSDUSize = DPCKTTSY_MAX_SDU_SIZE_REQ;
	aQoSReqConfig.iReqMaxRate.iUplinkRate = DPCKTTSY_MAX_UPLINK_BITRATE_REQ;
	aQoSReqConfig.iReqMaxRate.iDownlinkRate = DPCKTTSY_MAX_DOWNLINK_BITRATE_REQ;
	aQoSReqConfig.iReqBER = DPCKTTSY_BER_REQ;
	aQoSReqConfig.iReqSDUErrorRatio = DPCKTTSY_SDU_ERROR_RATIO_REQ;
	aQoSReqConfig.iReqTransferDelay = DPCKTTSY_TRANSFER_DELAY_REQ;
	aQoSReqConfig.iReqGuaranteedRate.iUplinkRate = DPCKTTSY_GRNT_UPLINK_BITRATE_REQ;
	aQoSReqConfig.iReqGuaranteedRate.iDownlinkRate = DPCKTTSY_GRNT_DOWNLINK_BITRATE_REQ;
	
	r99r4QoS.SetProfileParameters(reqStatus, aQoSReqPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_QOS_CASE("C.3G4"));

// Cancel Request	
	r99r4QoS.SetProfileParameters(reqStatus, aQoSReqPckg);
	r99r4QoS.CancelAsyncRequest(EPacketQoSSetProfileParams);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3G4"));


// Get Rel99/R4 QoS Profile Parameters
	RPacketQoS::TQoSR99_R4Negotiated aQoSNegConfig;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> aQoSNegPckg(aQoSNegConfig);

	r99r4QoS.GetProfileParameters(reqStatus, aQoSNegPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_QOS_CASE("C.3G5"));

	CHECKPOINT(aQoSNegConfig.ExtensionId(), TPacketDataConfigBase::KConfigRel99Rel4, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iTrafficClass, DPCKTTSY_TRAFFIC_CLASS_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iDeliveryOrderReqd, DPCKTTSY_DELIVERY_ORDER_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iDeliverErroneousSDU, DPCKTTSY_ERRONEOUS_SDU_DELIVERY_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iMaxSDUSize, DPCKTTSY_MAX_SDU_SIZE_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iMaxRate.iUplinkRate, DPCKTTSY_MAX_UPLINK_BITRATE_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iMaxRate.iDownlinkRate, DPCKTTSY_MAX_DOWNLINK_BITRATE_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iBER, DPCKTTSY_BER_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iSDUErrorRatio, DPCKTTSY_SDU_ERROR_RATIO_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iTransferDelay, DPCKTTSY_TRANSFER_DELAY_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iGuaranteedRate.iUplinkRate, DPCKTTSY_GRNT_UPLINK_BITRATE_NEG, CHP_QOS_CASE("C.3G5"));
	CHECKPOINT(aQoSNegConfig.iGuaranteedRate.iDownlinkRate, DPCKTTSY_GRNT_DOWNLINK_BITRATE_NEG, CHP_QOS_CASE("C.3G5"));

// Cancel Request	
	r99r4QoS.GetProfileParameters(reqStatus, aQoSNegPckg);
	r99r4QoS.CancelAsyncRequest(EPacketQoSGetProfileParams);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3G5"));

// Post a 'Profile Changed' notification for R5. 

	RPacketQoS::TQoSR5Negotiated aNotifyQoSR5;
	TPckg<RPacketQoS::TQoSR5Negotiated> aNotifyQoSR5Pckg(aNotifyQoSR5);

	r99r4QoS.NotifyProfileChanged(notifyStatus, aNotifyQoSR5Pckg);
	User::WaitForRequest(notifyStatus);

	CHECKPOINT(notifyStatus.Int(),KErrNone,CHP_QOS_CASE("C.3G8"));
	
	CHECKPOINT(aNotifyQoSR5.ExtensionId(),TPacketDataConfigBase::KConfigRel5,CHP_QOS_CASE("C.3G8"));

	CHECKPOINT(!aNotifyQoSR5.iSignallingIndication, KErrNone, CHP_QOS_CASE("C.3G8"));
	CHECKPOINT(aNotifyQoSR5.iSourceStatisticsDescriptor, DPCKTTSY_SRC_STATSDESC, CHP_QOS_CASE("C.3G8"));
	
// Cancel Request	
	r99r4QoS.NotifyProfileChanged(notifyStatus, aNotifyQoSR5Pckg);
	r99r4QoS.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(notifyStatus);
	CHECKPOINT(notifyStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3G8"));

// Set R5 QoS Profile Parameters
	RPacketQoS::TQoSR5Requested aQoSR5ReqConfig;
	TPckg<RPacketQoS::TQoSR5Requested> aQoSR5ReqPckg(aQoSR5ReqConfig);

	aQoSR5ReqConfig.iSignallingIndication = DPCKTTSY_SIG_IND;
	aQoSR5ReqConfig.iSourceStatisticsDescriptor = DPCKTTSY_SRC_STATSDESC;
		
	r99r4QoS.SetProfileParameters(reqStatus, aQoSR5ReqPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_QOS_CASE("C.3G9"));

// Cancel Request	
	r99r4QoS.SetProfileParameters(reqStatus, aQoSR5ReqPckg);
	r99r4QoS.CancelAsyncRequest(EPacketQoSSetProfileParams);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3G9"));
	
	r99r4QoS.Close();
	wcdmaContext.Close();
	wcdmaService.Close();
	pcktPhone.Close();

	return TestStepResult();
}
