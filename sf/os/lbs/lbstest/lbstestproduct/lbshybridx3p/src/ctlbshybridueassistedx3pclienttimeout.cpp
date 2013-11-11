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
#include "ctlbshybridueassistedx3pclienttimeout.h"
#include <lbs/test/tlbsutils.h>
#include <lbs/test/lbsnetprotocolproxy.h>
#include "argutils.h"

/**
Static Constructor
*/
CT_LbsHybridUEAssistedX3PClientTimeout* CT_LbsHybridUEAssistedX3PClientTimeout::New(CT_LbsHybridX3PServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEAssistedX3PClientTimeout* testStep = new CT_LbsHybridUEAssistedX3PClientTimeout(aParent);
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
CT_LbsHybridUEAssistedX3PClientTimeout::CT_LbsHybridUEAssistedX3PClientTimeout(CT_LbsHybridX3PServer& aParent) : CT_LbsHybridX3PStep(aParent)
	{
	SetTestStepName(KLbsHybridUEAssistedX3PClientTimeout);
	}


void CT_LbsHybridUEAssistedX3PClientTimeout::ConstructL()
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
CT_LbsHybridUEAssistedX3PClientTimeout::~CT_LbsHybridUEAssistedX3PClientTimeout()
	{
	iTransmitter.Close();
	iServer.Close();
	delete iProxy;
	}



TVerdict CT_LbsHybridUEAssistedX3PClientTimeout::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("CT_LbsHybridUEAssistedX3PClientTimeout::doTestStepL()"));

	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);
	
	const TInt KTimeOut = 30*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
	const TInt KSmallTimeOut = 3*1000*1000; 
	const TInt64 KTransmitTimeOut = 70*1000*1000;
	
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
	const TInt KPriority= 1;
	TTimeIntervalMicroSeconds transmitTimeOut(KTransmitTimeOut);
	TLbsTransmitPositionOptions options(transmitTimeOut);
	TRequestStatus refPosStatus=KRequestPending;
	TRequestStatus transPosStatus=KRequestPending;
	TPositionInfo refPosInfo;
	TPositionInfo transPosInfo;
	iTransmitter.SetTransmitOptions(options);
	iTransmitter.TransmitPosition(KThirdParty, KPriority, refPosStatus, refPosInfo, transPosStatus, transPosInfo);

	//Start the transmission timer
	TTime transmitTimerStart;
	transmitTimerStart.HomeTime();
	
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
	TInt reason(KErrNone);
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(1); //assistanceData
// Assistance Data Notification End
	

// Network Location Request Start
	// ProcessLocationRequest()
	const TBool emergency(EFalse);
	TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2(); 
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();	
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
// Network Location Request End

	//Start the timer
	TTime timerStart;
	timerStart.HomeTime();
	
	// RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
	TLbsAsistanceDataGroup dataGroup;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
	TESTL(dataGroup == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// we wait for either to complete but we are expecting
	// for only the REF pos request to complete
	User::WaitForRequest(refPosStatus, transPosStatus);
	TESTL(refPosStatus==KErrNone);
	TESTL(transPosStatus.Int() == KRequestPending);

	//Find the time elapsed from timer
	TTimeIntervalMicroSeconds microseconds;
 	TTime timerStop;
 	timerStop.HomeTime();
 	microseconds = timerStop.MicroSecondsFrom(timerStart); 
	TInt64 timeElapsed = microseconds.Int64();
						
/*** NRH's Alpha2 timer expires. We enter Hybrid mode.***/
	//Test that we do not get response before alpha2 has expired
	// The way the test works is to check first that the response is not received too early, 
	// that is, not within the period (t - delta).
	// Then it checks the response IS received within the period t + delta (so 2 * delta
	// after the previous check).
	// If both these tests are satisfied, then the test passes.
	// There is a problem though. Because the protocol module involves lots of
	// state transitions for any request, and the priority of its active object is set low to avoid 
	// hogging system resources, it occasionally happens that a delay creeps in before the
	// request gets as far as the NRH. 700ms has been observed.
	// This means that the WaitForResponse() here, which takes its startpoint from 
	// the issuing of the request, sometimes times out even though the NRH, left for a bit longer,
	// would time out correctly and return the expected response.
	// We could just change delta to a large enough value (say, 1 sec) to cover this, but
	// this might end up concealing a real problem if it happened that a consistent delay was 
	// introduced for some other reason.
	// So the solution is to behave as described above, but in the case where the response is not 
	// received within (t + delta), we wait for a further second. If the response is
	// still not received, then the test fails. If it is received in this extra period,
	// the test result is set to EInconclusive, to indicate there's something we're
	// not happy with, without its being a complete failure. If the test consistently 
	// finishes with an EInconclusive result, then that is an indication that it ought 
	// to be looked at
	TInt extraDelta = 4.0*1000*1000; // 1 sec
	TESTL(iProxy->WaitForResponse(KAlpha2Timeout-timeElapsed-KDelta) == ENetMsgTimeoutExpired); 
	TNetProtocolResponseType result = iProxy->WaitForResponse(KDelta*2);
	if(result != ENetMsgRespondLocationRequest)
		{
		INFO_PRINTF3(_L("********WARNING - EXPECTED %d RECEIVED %d, Checking to see if it arrives at all...."), ENetMsgRespondLocationRequest, result);
		// Didn't get it. See if it arrives within the extra period

		result = iProxy->WaitForResponse(extraDelta);
		if(result == ENetMsgRespondLocationRequest)
			{
			// Got it eventually. Can't say it passed, but don't worry too much.
			SetTestStepResult(EInconclusive);
			}
		else
			{
			// Didn't get it at all. Fail.
			ERR_PRINTF1(_L("********ERROR - EXPECTED MESSAGE NOT RECEIVED"));
			User::Leave(result);				
			}
		}
		
	getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getPositionInfo->PositionClassType()==EPositionGpsMeasurementInfoClass);	
	TESTL(getReason == KErrNone); 
	CleanupStack::PopAndDestroy(cleanupCnt);

	quality = ArgUtils::Quality(); 
 	TTime transmitTimerStop;
 	transmitTimerStop.HomeTime();
 	TInt64 transmitTimeElapsed = transmitTimerStop.MicroSecondsFrom(transmitTimerStart).Int64();
 	while((transmitTimeElapsed + KTTimeout + KDelta + extraDelta)<=KTransmitTimeOut)
		{
		iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
		
		timerStart.HomeTime();
		
		// RequestAssistanceData(0)
		TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
		cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
		TESTL(dataGroup == EAssistanceDataNone);		
		CleanupStack::PopAndDestroy(cleanupCnt);
	
	 	timerStop.HomeTime();
 		microseconds = timerStop.MicroSecondsFrom(timerStart); 
		timeElapsed = microseconds.Int64();	
		
		TESTL(iProxy->WaitForResponse(KTTimeout-timeElapsed-KDelta) == ENetMsgTimeoutExpired); 
		TNetProtocolResponseType result = iProxy->WaitForResponse(KDelta*2);
		if(result != ENetMsgRespondLocationRequest)
			{
			INFO_PRINTF3(_L("********WARNING - EXPECTED %d RECEIVED %d, Checking to see if it arrives at all...."), ENetMsgRespondLocationRequest, result);
			// Didn't get it. See if it arrives within the extra period

			result = iProxy->WaitForResponse(extraDelta);
			if(result == ENetMsgRespondLocationRequest)
				{
			 	timerStop.HomeTime();
				microseconds = timerStop.MicroSecondsFrom(timerStart); 
				TInt64 hadToWait = microseconds.Int64();
				INFO_PRINTF2(_L("********OK - MESSAGE RECEIVED AFTER %d"),hadToWait);
				// Got it eventually. Can't say it passed, but don't worry too much.
				SetTestStepResult(EInconclusive);
				}
			else
				{
				// Didn't get it at all. Fail.
				ERR_PRINTF1(_L("********ERROR - EXPECTED MESSAGE NOT RECEIVED"));
				User::Leave(result);				
				}
			}

		getSessionId = NULL;
		getReason = KErrNone;
		getPositionInfo = NULL;

		cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
		TESTL(getReason == KErrNone);
		TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
		TESTL(getPositionInfo->PositionClassType()==EPositionGpsMeasurementInfoClass);	
		CleanupStack::PopAndDestroy(cleanupCnt);
		
		transmitTimerStop.HomeTime();
	 	transmitTimeElapsed = transmitTimerStop.MicroSecondsFrom(transmitTimerStart).Int64();
		} 
	
/*** Client X3P Timer Expires ***/
	getSessionId = NULL;
	getReason = KErrNone;
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgCancelTransmitLocation); 
	cleanupCnt = iProxy->GetArgsLC(ENetMsgCancelTransmitLocation, &getSessionId, &getReason);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason == KErrTimedOut); 
	CleanupStack::PopAndDestroy(cleanupCnt);
			
// Network Result Notification Start
	// ProcessLocationUpdate()
	//Final Network Position is the reference position
	refPosInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

// Session Complete Start
	reason = KErrNone; 
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceNone;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);
// Session Complete End

	User::WaitForRequest(transPosStatus); 
	// We test the error returned from the network
	TESTL(transPosStatus==KPositionQualityLoss); 
		
	return TestStepResult();
	}
