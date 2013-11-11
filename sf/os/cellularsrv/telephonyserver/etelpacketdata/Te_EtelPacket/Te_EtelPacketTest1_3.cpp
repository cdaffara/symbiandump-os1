// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTest.cpp
// This contains EtelPacket TestCase Test 1.1(RPAcketContext functionality)
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTest1_3.h"
#include "Te_etelpckt_defs.h"

#include <es_sock.h>
#include <e32svr.h>
#include "etelQoS.h"

// constructor
CEtelPacketTest1_3::CEtelPacketTest1_3()
{
	// store the name of this test case
	SetTestStepName(_L("QoS"));
}

// destructor
CEtelPacketTest1_3::~CEtelPacketTest1_3()
{
}

// do Test step 1.1
enum TVerdict CEtelPacketTest1_3::doTestStepL( void )
{
	RPhone phone;
	TInt ret=phone.Open(iTelServer,DPCKTTSY_PHONE_NAME);
	CHECKPOINT(ret,KErrNone,CHP_OPEN_PHONE);

	RPacketService gprs;
	ret=gprs.Open(phone);
    CHECKPOINT(ret,KErrNone,CHP_SRVS_CASE("A.1"));

	RPacketContext gprsContext;
	TName contextName;
	ret=gprsContext.OpenNewContext(gprs, contextName);
	CHECKPOINT(ret,KErrNone,CHP_CNTXT_CASE("B.1"));

	RPacketQoS	gprsQoS;
	TName qosName;
	ret = gprsQoS.OpenNewQoS(gprsContext, qosName);
	CHECKPOINT(ret,KErrNone,CHP_QOS_CASE("C.1"));
	CHECKPOINT(qosName.Compare(DPCKTTSY_QOS_NAME),KErrNone,CHP_QOS_CASE("C.1"));
	// local variables used throughout the gprs tests
	TRequestStatus reqStatus;
	TRequestStatus notifyStatus;
	// post a notification
	RPacketQoS::TQoSGPRSNegotiated aNotifyQoS;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> aNotifyQoSPckg(aNotifyQoS);

	gprsQoS.NotifyProfileChanged(notifyStatus, aNotifyQoSPckg);
	User::WaitForRequest(notifyStatus);
	CHECKPOINT(notifyStatus.Int(),KErrNone,CHP_QOS_CASE("C.7"));
	CHECKPOINT(aNotifyQoS.iDelay , DPCKTTSY_DELAY_MIN1,CHP_QOS_CASE("C.7"));
	CHECKPOINT(aNotifyQoS.iMeanThroughput , DPCKTTSY_MEAN_THROUGHPUT_MIN1,CHP_QOS_CASE("C.7"));
	CHECKPOINT(aNotifyQoS.iPeakThroughput , DPCKTTSY_PEAK_THROUGHPUT_MIN1,CHP_QOS_CASE("C.7"));
	CHECKPOINT(aNotifyQoS.iPrecedence , DPCKTTSY_PRECEDENCE_MIN1,CHP_QOS_CASE("C.7"));
	CHECKPOINT(aNotifyQoS.iReliability , DPCKTTSY_RELIABILITY_MIN1,CHP_QOS_CASE("C.7"));
	//fix for defect MPO-576M6R: added cancel case 
	gprsQoS.NotifyProfileChanged(notifyStatus, aNotifyQoSPckg);
	gprsQoS.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(notifyStatus);
	CHECKPOINT(notifyStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3"));
	// Get QoS GPRS Capabilities
	RPacketQoS::TQoSCapsGPRS aQoSCaps;
	TPckg<RPacketQoS::TQoSCapsGPRS> aQoSCapsPckg(aQoSCaps);
	
	gprsQoS.GetProfileCapabilities(reqStatus, aQoSCapsPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_QOS_CASE("C.6"));
	CHECKPOINT(aQoSCaps.ExtensionId(),TPacketDataConfigBase::KConfigGPRS,CHP_QOS_CASE("C.6"));
	CHECKPOINT(aQoSCaps.iDelay,DPCKTTSY_DELAY,CHP_QOS_CASE("C.6"));
	CHECKPOINT(aQoSCaps.iMean,DPCKTTSY_MEAN_THROUGHPUT,CHP_QOS_CASE("C.6"));
	CHECKPOINT(aQoSCaps.iPeak,DPCKTTSY_PEAK_THROUGHPUT,CHP_QOS_CASE("C.6"));
	CHECKPOINT(aQoSCaps.iPrecedence,DPCKTTSY_PRECEDENCE,CHP_QOS_CASE("C.6"));
	CHECKPOINT(aQoSCaps.iReliability,DPCKTTSY_RELIABILITY,CHP_QOS_CASE("C.6"));
	//fix for defect MPO-576M6R: added cancel case 
	gprsQoS.GetProfileCapabilities(reqStatus, aQoSCapsPckg);
	gprsQoS.CancelAsyncRequest(EPacketQoSGetProfileCaps);
	User::WaitForRequest(notifyStatus);
	CHECKPOINT(notifyStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3"));
	// Set QoS Profile Params
	RPacketQoS::TQoSGPRSRequested aQoSReqConfig;
	TPckg<RPacketQoS::TQoSGPRSRequested> aQoSReqPckg(aQoSReqConfig);

	aQoSReqConfig.iMinDelay = DPCKTTSY_DELAY_MIN1;
	aQoSReqConfig.iMinMeanThroughput = DPCKTTSY_MEAN_THROUGHPUT_MIN1;
	aQoSReqConfig.iMinPeakThroughput = DPCKTTSY_PEAK_THROUGHPUT_MIN1;
	aQoSReqConfig.iMinPrecedence = DPCKTTSY_PRECEDENCE_MIN1;
	aQoSReqConfig.iMinReliability = DPCKTTSY_RELIABILITY_MIN1;
	aQoSReqConfig.iReqDelay = DPCKTTSY_DELAY_REQ1;
	aQoSReqConfig.iReqMeanThroughput = DPCKTTSY_MEAN_THROUGHPUT_REQ1;
	aQoSReqConfig.iReqPeakThroughput = DPCKTTSY_PEAK_THROUGHPUT_REQ1;
	aQoSReqConfig.iReqPrecedence = DPCKTTSY_PRECEDENCE_REQ1;
	aQoSReqConfig.iReqReliability = DPCKTTSY_RELIABILITY_REQ1;

	gprsQoS.SetProfileParameters(reqStatus, aQoSReqPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_QOS_CASE("C.4"));
	//fix for defect MPO-576M6R: added cancel case 
	gprsQoS.SetProfileParameters(reqStatus, aQoSReqPckg);
	gprsQoS.CancelAsyncRequest(EPacketQoSSetProfileParams);
	User::WaitForRequest(notifyStatus);
	CHECKPOINT(notifyStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3"));
	// Get QoS Profile Params
	RPacketQoS::TQoSGPRSNegotiated aQoSNegConfig;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> aQoSNegPckg(aQoSNegConfig);

	gprsQoS.GetProfileParameters(reqStatus, aQoSNegPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_QOS_CASE("C.5"));
	CHECKPOINT(aQoSNegConfig.ExtensionId(),TPacketDataConfigBase::KConfigGPRS,CHP_QOS_CASE("C.5"));
	CHECKPOINT(aQoSNegConfig.iDelay,DPCKTTSY_DELAY_NEG2,CHP_QOS_CASE("C.5"));
	CHECKPOINT(aQoSNegConfig.iMeanThroughput,DPCKTTSY_MEAN_THROUGHPUT_NEG2,CHP_QOS_CASE("C.5"));
	CHECKPOINT(aQoSNegConfig.iPeakThroughput,DPCKTTSY_PEAK_THROUGHPUT_NEG2,CHP_QOS_CASE("C.5"));
	CHECKPOINT(aQoSNegConfig.iPrecedence,DPCKTTSY_PRECEDENCE_NEG2,CHP_QOS_CASE("C.5"));
	CHECKPOINT(aQoSNegConfig.iReliability,DPCKTTSY_RELIABILITY_NEG2,CHP_QOS_CASE("C.5"));
	//fix for defect MPO-576M6R: added cancel case 
	gprsQoS.GetProfileParameters(reqStatus, aQoSNegPckg);
	gprsQoS.CancelAsyncRequest(EPacketQoSGetProfileParams);
	User::WaitForRequest(notifyStatus);
	CHECKPOINT(notifyStatus.Int(),KErrCancel,CHP_QOS_CASE("C.3"));

	//Fix for defect MPO-576M6R: Added call to RPacket::OpenExistingQos()
	RPacketQoS	gprsQoS2;
	ret = gprsQoS2.OpenExistingQoS(gprsContext, qosName);
	CHECKPOINT(ret,KErrNone,CHP_QOS_CASE("C.2"));
	
	gprsQoS.Close();
	gprsQoS2.Close();
	gprsContext.Close();
	gprs.Close();
	phone.Close();

	return TestStepResult();
}
