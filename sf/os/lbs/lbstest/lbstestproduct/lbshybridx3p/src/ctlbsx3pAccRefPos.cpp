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
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbsx3pAccRefPos.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

/**
Static Constructor
*/
CT_LbsX3pAccRefPos* CT_LbsX3pAccRefPos::New(CT_LbsHybridX3PServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsX3pAccRefPos* testStep = new CT_LbsX3pAccRefPos(aParent);
	if (testStep)
		{
		TInt err(KErrNone);

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
CT_LbsX3pAccRefPos::CT_LbsX3pAccRefPos(CT_LbsHybridX3PServer& aParent) : CT_LbsHybridX3PStep(aParent)
	{
	SetTestStepName(KLbsX3pAccurateReferencePosition);
	}


void CT_LbsX3pAccRefPos::ConstructL()
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
CT_LbsX3pAccRefPos::~CT_LbsX3pAccRefPos()
	{
	iTransmitter.Close();
	iServer.Close();
	delete iProxy;
	}



TVerdict CT_LbsX3pAccRefPos::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("CT_LbsX3pAccRefPos::doTestStepL()"));

	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);
	
	const TInt KTimeOut = 30*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
		
	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KAdviceSystemStatusTimeout) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	// nothing is placed on cleanupstack for ENetMsgGetCurrentCapabilitiesResponse
	CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status));
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);

	
//Initiate X3P start
	// TransmitPosition()
	_LIT(KThirdParty,"+4407463842101"); 
	const TInt KPriority(6);
	TLbsTransmitPositionOptions options(TTimeIntervalMicroSeconds(50*1000*1000));
	TRequestStatus refPosStatus=KRequestPending;
	TRequestStatus transPosStatus=KRequestPending;
	TPositionInfo clientRefPosInfo;
	TPositionInfo transPosInfo;
	iTransmitter.SetTransmitOptions(options);
	iTransmitter.TransmitPosition(KThirdParty, KPriority, refPosStatus, clientRefPosInfo, transPosStatus, transPosInfo);
	
	// RequestTransmitLocation()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestTransmitLocation);
	TBufC16<14> thirdParty(KThirdParty);
	TPtr16 ptr = thirdParty.Des(); 
	HBufC16* getThirdParty = NULL;
	TLbsNetSessionId* getSessionId = NULL;
	TInt getPriority(0);
	TInt stackCnt = 
		iProxy->GetArgsLC(ENetMsgRequestTransmitLocation, &getSessionId, &getThirdParty, &getPriority); 
	TESTL(ptr.Compare(*getThirdParty)==KErrNone);	
	TESTL(getPriority == KPriority);
	iSessionId = *getSessionId; //session ID is initialised by LBS
	CleanupStack::PopAndDestroy(stackCnt);//getSessionID,getThirdParty

	// ProcessStatusUpdate()
	const MLbsNetworkProtocolObserver::TLbsNetProtocolService service 
		= MLbsNetworkProtocolObserver::EServiceTransmitThirdParty;		
	iProxy->CallL(ENetMsgProcessStatusUpdate, &service);
//End Initiate


//Reference Position Notification Start	
	// ProcessLocationUpdate()
	TPositionInfo refPosInfo;
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


// Network Result Notification Start
	// ProcessLocationUpdate()
	//Final Network Position = ref position
	refPosInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);
// Network Result Notification Stop

	
// Session Complete Start
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceNone;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);
// Session Complete Stop

	// Allow session complete to be processed by LBS.
	TInt delta = 4 * 1000 * 1000; // 4 secs.
	TESTL(iProxy->WaitForResponse(delta) == ENetMsgTimeoutExpired);

	// Complete client request.
	User::WaitForRequest(refPosStatus, transPosStatus); 

	// Verify client ref pos.
	TESTL(clientRefPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
	TESTL(refPosStatus == KErrNone);
	
	// Verify client final pos.
	TESTL(transPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
	TESTL(transPosStatus == KErrNone);

		
	return TestStepResult();
	}






