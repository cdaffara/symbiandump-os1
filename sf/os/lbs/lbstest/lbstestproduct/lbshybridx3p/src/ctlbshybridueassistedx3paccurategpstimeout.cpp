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
// LBS includes. 
// 
//

#include <lbs.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbshybridueassistedx3paccurategpstimeout.h"
#include <lbs/test/tlbsutils.h>
#include <lbs/test/lbsnetprotocolproxy.h>
#include "argutils.h"

/**
Static Constructor
*/
CT_LbsHybridUEAssistedX3PAccurateGPSTimeout* CT_LbsHybridUEAssistedX3PAccurateGPSTimeout::New(CT_LbsHybridX3PServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEAssistedX3PAccurateGPSTimeout* testStep = new CT_LbsHybridUEAssistedX3PAccurateGPSTimeout(aParent);
	if (testStep)
		{
		TInt err = KErrNone;

		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}
	return testStep;
	}

/**
 * Constructor
 */
CT_LbsHybridUEAssistedX3PAccurateGPSTimeout::CT_LbsHybridUEAssistedX3PAccurateGPSTimeout(CT_LbsHybridX3PServer& aParent) : CT_LbsHybridX3PStep(aParent)
	{
	SetTestStepName(KLbsHybridUEAssistedX3PAccurateGPSTimeout);
	}

void CT_LbsHybridUEAssistedX3PAccurateGPSTimeout::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridX3PStep::ConstructL();
	User::LeaveIfError(iServer.Connect());
	User::LeaveIfError(iTransmitter.Open(iServer));
	iProxy = CNetProtocolProxy::NewL();
	}

/**
 * Destructor
 */
CT_LbsHybridUEAssistedX3PAccurateGPSTimeout::~CT_LbsHybridUEAssistedX3PAccurateGPSTimeout()
	{
	iTransmitter.Close();
	iServer.Close();
	delete iProxy;
	}

TVerdict CT_LbsHybridUEAssistedX3PAccurateGPSTimeout::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("CT_LbsHybridUEAssistedX3PAccurateGPSTimeout::doTestStepL()"));

	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);
	
	const TInt KTimeOut = 30*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
	const TInt KSmallTimeOut = 3*1000*1000; 
			
	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KAdviceSystemStatusTimeout) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

//Initiate X3P start
	// TransmitPosition()
	_LIT(KThirdParty,"+4407463842101"); 
	const TInt KPriority= 6;
	TLbsTransmitPositionOptions options(TTimeIntervalMicroSeconds(500*1000*1000));
	TRequestStatus refPosStatus=KRequestPending;
	TRequestStatus transPosStatus=KRequestPending;
	TPositionInfo refPosInfo;
	TPositionInfo transPosInfo;
	iTransmitter.SetTransmitOptions(options);
	iTransmitter.TransmitPosition(KThirdParty, KPriority, refPosStatus, refPosInfo, transPosStatus, transPosInfo);
	
	// RequestTransmitLocation()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestTransmitLocation);
	TBufC16<14> thirdParty(KThirdParty);
	TPtr16 ptr = thirdParty.Des(); 
	HBufC16* getThirdParty = NULL;
	TLbsNetSessionId* getSessionId = NULL;
	TInt getPriority(0);
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestTransmitLocation, &getSessionId, &getThirdParty, &getPriority); 
	TESTL(ptr.Compare(*getThirdParty)==KErrNone);	
	TESTL(getPriority == KPriority);
	iSessionId = *getSessionId; //session ID is initialised by LBS
	CleanupStack::PopAndDestroy(cleanupCnt);

	// ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceTransmitThirdParty;		
	iProxy->CallL(ENetMsgProcessStatusUpdate, &service);
//End Initiate

//Reference Position Notification Start	
	// ProcessLocationUpdate()
	refPosInfo = ArgUtils::ReferencePositionInfo();
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);
//Reference Position Notification End

	
//Assistance Data Notification Start
	// ProcessAssistanceData()
	TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(1); //assistanceData
// Assistance Data Notification End
	
// Network Location Request Start
	// ProcessLocationRequest()
	const TBool emergency(EFalse);
	TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2(); 
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();	
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
// Network Location Request Stop

	//Start the timer
	TTime timerStart;
	timerStart.HomeTime();
	
	// RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
	TLbsAsistanceDataGroup dataGroup;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
	TESTL(dataGroup == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	User::WaitForRequest(refPosStatus, transPosStatus); 
	TESTL(refPosStatus==KErrNone);

	//Find the time elapsed from timer
	TTimeIntervalMicroSeconds microseconds;
 	TTime timerStop;
 	timerStop.HomeTime();
 	microseconds = timerStop.MicroSecondsFrom(timerStart); 
	TInt64 timeElapsed = microseconds.Int64();
						
/*** NRH's Alpha2 timer expires. We enter Hybrid mode.***/
	//Test that we do not get response before alpha2 has expired
	TESTL(iProxy->WaitForResponse(KAlpha2Timeout-timeElapsed-KDelta) == ENetMsgTimeoutExpired); 
	TESTL(iProxy->WaitForResponse(2*KDelta) == ENetMsgRespondLocationRequest); 	
	getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason == KErrNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// no need for looping, we assume that the next update location from GPS will give accurate fix
	quality = ArgUtils::Quality(); 
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

	// RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
	TESTL(dataGroup == EAssistanceDataNone);		
	CleanupStack::PopAndDestroy(cleanupCnt);
	
	//	GPS positions meets required accuracy. This is sent immediately to protocol module
	// The ini file should contain accurate gps fix for this test case to work
	TESTL(iProxy->WaitForResponse(KTTimeout) == ENetMsgRespondLocationRequest);	
	getSessionId = NULL;
	getReason = KErrNone;
	getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason == KErrNone);
	// Test position is the same as in the ini file data fed to the GPS module
	// $update,1,2,51.5015,-0.105,50,2,3*
	TPosition gpsPos;
	getPositionInfo->GetPosition(gpsPos);
	TESTL(gpsPos.Latitude()==51.5015 && gpsPos.Longitude()==-0.105 && gpsPos.Altitude()==50 && gpsPos.HorizontalAccuracy()==2 && gpsPos.VerticalAccuracy()==3); 
	CleanupStack::PopAndDestroy(cleanupCnt);

// Network Result Notification Start
	// Add extra delay to simulate the Facility timing out 	
	User::After(2*KTimeOut);
	// Test that LBS is not sending any response to the Protocol Module
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgTimeoutExpired); 
// Network Result Notification Stop
	
// Session Complete Start
	// The network times out and pass an error to the Protocol Module, in this test, we use KErrTimedOut
	// The protocol module passes the error to the client app
	reason = KErrTimedOut; 
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceNone;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);
// Session Complete Stop

	User::WaitForRequest(transPosStatus); 
	// We test the error returned from the network
	TESTL(transPosStatus==KErrTimedOut);
	return TestStepResult();
	}






