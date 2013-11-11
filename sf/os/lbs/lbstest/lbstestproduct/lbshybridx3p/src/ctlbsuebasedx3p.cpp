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

// LBS includes. 
#include <lbs.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbsuebasedx3p.h"
#include <lbs/test/tlbsutils.h>
#include <lbs/test/lbsnetprotocolproxy.h>
#include "argutils.h"

/**
Static Constructor
*/
CT_LbsUEBasedX3P* CT_LbsUEBasedX3P::New(CT_LbsHybridX3PServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsUEBasedX3P* testStep = new CT_LbsUEBasedX3P(aParent);
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
CT_LbsUEBasedX3P::CT_LbsUEBasedX3P(CT_LbsHybridX3PServer& aParent) : CT_LbsHybridX3PStep(aParent)
	{
	SetTestStepName(KLbsUEBasedX3P);
	}

void CT_LbsUEBasedX3P::ConstructL()
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
CT_LbsUEBasedX3P::~CT_LbsUEBasedX3P()
	{
	iTransmitter.Close();
	iServer.Close();
	delete iProxy;
	}

TVerdict CT_LbsUEBasedX3P::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("CT_LbsUEBasedX3P::doTestStepL()"));

	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);
	
	const TInt KTimeOut = 30*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
	const TInt KSmallTimeOut = 3*1000*1000;
		
	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KAdviceSystemStatusTimeout) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status));
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	
//Initiate X3P start
	// TransmitPosition()
	_LIT(KThirdParty,"+4407463842101"); 
	const TInt KPriority= 1;
	TLbsTransmitPositionOptions options(TTimeIntervalMicroSeconds(50*1000*1000));
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
	TInt cleanupCnt;
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

	// RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
	TLbsAsistanceDataGroup dataGroup;
	CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup));
	TESTL(dataGroup == EAssistanceDataNone);

	// here we wait for either of the asynchrnous requestst to complete
	User::WaitForRequest(refPosStatus,transPosStatus); 
	// but now we do expect that only the REF position asyn request to complete
	TESTL(refPosStatus==KErrNone);
	TESTL(transPosStatus.Int() == KRequestPending);

	TESTL(iProxy->WaitForResponse(KAlpha2Timeout) == ENetMsgRespondLocationRequest);	
	getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason == KErrNone);
	// Test position is the same as in the ini file data fed to the GPS module
	// $update,1,2,51.5015,-0.105,50,2,3*
	TPosition gpsPos;
	getPositionInfo->GetPosition(gpsPos);
	TESTL(gpsPos.Latitude()==51.5015 && gpsPos.Longitude()==-0.105 && gpsPos.Altitude()==50 && gpsPos.HorizontalAccuracy()==2 && gpsPos.VerticalAccuracy()==3); 
	CleanupStack::PopAndDestroy(cleanupCnt); //getSessionId, getPositionInfo
					
// Network Result Notification Start
	// ProcessLocationUpdate()
	TPositionInfo gpsPosInfo;
	gpsPosInfo.SetPosition(gpsPos);
	gpsPosInfo.SetUpdateType(EPositionUpdateGeneral);
	gpsPosInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
	gpsPosInfo.SetPositionModeReason(EPositionModeReasonNone);
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &gpsPosInfo);
// Network Result Notification Stop
	
// Session Complete Start
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
	
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceNone;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);
// Session Complete Stop

	// the REF position request has completed, so now, after injecting the FNP and Session Complete
	// we expect that the other request to complete with KErrNone
	User::WaitForRequest(transPosStatus);  

	TESTL(transPosStatus==KErrNone);
	    
	return TestStepResult();
	}
