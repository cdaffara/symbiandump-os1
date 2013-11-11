// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the class implementation for the "SUPL MOLR Terminal Based Futile Special Feature" test.
// 
//

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbssuplmolrtbfutilesp.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

const TInt KTimeOut = 60*1000*1000;

CT_LbsSuplMolrTbFutileSp* CT_LbsSuplMolrTbFutileSp::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsSuplMolrTbFutileSp* testStep = new CT_LbsSuplMolrTbFutileSp(aParent);
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


CT_LbsSuplMolrTbFutileSp::CT_LbsSuplMolrTbFutileSp(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsSuplMolrTbFutileSp);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsSuplMolrTbFutileSp::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	
	User::LeaveIfError(iServer.Connect());
	
	iProxy = CNetProtocolProxy::NewL();	
	iPosSrvWatcher = CPosServerWatcher::NewLC(iPositioner, *this);
	CleanupStack::Pop(iPosSrvWatcher);
	}


CT_LbsSuplMolrTbFutileSp::~CT_LbsSuplMolrTbFutileSp()
	{
	delete iPosSrvWatcher;
	delete iProxy;
	iServer.Close();
	}


TVerdict CT_LbsSuplMolrTbFutileSp::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridMOLRFutile::doTestStepL()"));
		
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	// Start Test

	// This test will perform 2 MOLRs sessions, for the first MOLR the integration module will return an futile update,
	// the second MOLR is performed with an accurate update.
	// The aim of the test is to ensure the futile update from session 1 is not returned during session 2, with the
	// special feature KLbsSpecialFeatureIntermediateFutileUpdate switched on.

	// >> AdviceSystemStatus(0) - this is sent once after LBS startup.
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);

	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// Carryout the first Molr, where the A-GPS integration returns futile updates.
	Perform_Molr(KPositionCalculationFutile);

	// Carryout the second Molr, where the A-GPS integration returns valid updates.
	Perform_Molr(KErrNone);

	return TestStepResult();
	}


/** Performs an MOLR as the SUPL protocol module would. This differs from an normal MOLR because
a 2nd network location request is sent to LBS as a result of the SUPL_RESPONSE() msg.

The "SUPL_---" msgs indicate supl network traffic which would occur with a real SUPL Protocol Module.
	
During each MOLR the following location requests are made.

Request No	From
----------	----
1			Loc server.
2			Network.
3			Network.
4			Loc server.

@param   aExpectedUpdateErr	The error sent to the network in the "RespondLocationRequest "msg.
*/
void CT_LbsSuplMolrTbFutileSp::Perform_Molr(TInt aExpectedUpdateErr)
	{

	iState = EInitializing;
	
	// Open sub-session.
	User::LeaveIfError(iPositioner.Open(iServer));
	CleanupClosePushL(iPositioner);

	// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KDefaultMaxFixTime));
	iPositioner.SetUpdateOptions(posOpts);

	// Client NotifyPositionUpdate() - req 1.
	iPosSrvWatcher->IssueNotifyPositionUpdate();	

		
	// >> RequestSelfLocation()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation); 

	// Process the response.
	TLbsNetSessionId* 					sessionId = NULL;
	TLbsNetPosRequestOptionsAssistance*	opts = NULL;

	TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);
	iSessionId.SetSessionNum(sessionId->SessionNum());
	iSessionId.SetSessionOwner(sessionId->SessionOwner());
	CleanupStack::PopAndDestroy(cleanupCnt);
	sessionId = NULL;


	
	// << ProcessStatusUpdate(EServiceSelfLocation)
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);


	//																SUPL_START() -->

	//																SUPL_RESPONSE() <--

	// << ProcessLocationRequest(TBMode), invoked from the SUPL_RESPONSE() - req 2.
	TBool emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	TLbsNetPosRequestQuality quality = ArgUtils::Quality(); 
	TLbsNetPosRequestMethod method = ArgUtils::RequestTerminalBasedMethod();
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

	// >> RequestAssistanceData(0) - integ mod assistance data req for req 2.
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	TLbsAsistanceDataGroup dataMask;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);


	//																SUPL_POS_INIT() -->

	//																SUPL_POS(Assistance Data) <--
	//																SUPL_POS(Ack) -->
	
	// << ProcessLocationUpdate(SessionId, RefPosition)
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

	// << ProcessAssistanceData()
	dataMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); // assistanceData
		
	//																SUPL_POS(Location request) <--

	// << ProcessLocationRequest(TBMode) - req 3.
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

	// >> RequestAssistanceData(0)  - integ mod assistance data req for req 3.
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);


	// Reterive the ref pos app side.
	CheckForObserverEventTestsL(KTimeOut, *this);
		
	// Client NotifyPositionUpdate() - req 4.
	iPosSrvWatcher->IssueNotifyPositionUpdate();
		
	// >> RequestAssistanceData(0) - integ mod assistance data req for req 4.
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
		

	// >> RespondLocationRequest()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest); 

	TPositionInfo* posInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &posInfo);
	TESTL(posInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass));
	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == aExpectedUpdateErr);


	//																SUPL_POS(GPSPos) -->
	//																SUPL_END() <--

	// <<  ProcessLocationUpdate - return terminal calculated pos for the final network position.
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, posInfo);

	CleanupStack::PopAndDestroy(cleanupCnt); // sessionId, posInfo - for previous GetArgsLC() was still using posInfo.


	// During the futile Molr, req 4 will be re-issued by the A-GPS manager because the update from the A-GPS integ module satisfies
	// the NRH but the location server still has an outstanding request. For the normal MOLR both the NRH and location server are satisfied.
	if (aExpectedUpdateErr == KPositionCalculationFutile)
		{
		// Integ mod assistance data req for re-issue of req 4.
		TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
		cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
		TESTL(dataMask == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);
		}


	// << ProcessSessionComplete(SessionId, KErrNone)	
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// << ENetMsgProcessStatusUpdate()
	serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);


	// Reterive the A-GPS pos app side.
	CheckForObserverEventTestsL(KTimeOut, *this);		
	TESTL(iState == EGpsLocReceived);

	// Close sub-session.
	CleanupStack::PopAndDestroy(&iPositioner);
	}


// MPosServerObserver
void CT_LbsSuplMolrTbFutileSp::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	// Shouldn't get this call.
	TEST(EFalse);

	ReturnToTestStep();
	}

void CT_LbsSuplMolrTbFutileSp::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
			
	// Verify position.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

	// Expecting ref position.
	if (iState == EInitializing)
		{
		// Verify error, mode and position data.
		TEST(aErr == KErrNone);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		
		iState = ERefLocReceived;
		}
		
	// Expecting A-GPS position.
	else if (iState == ERefLocReceived)
		{
		// Verify mode.
		TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
		
		iState = EGpsLocReceived;
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}

