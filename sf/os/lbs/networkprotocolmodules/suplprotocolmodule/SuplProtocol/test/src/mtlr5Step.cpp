// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file mtlr5Step.cpp
 @internalTechnology
*/
#include "mtlr5Step.h"
#include "te_suplprotocolsuitedefs.h"

/**
 Destructor
 */
Cmtlr5Step::~Cmtlr5Step()

	{
	}

/**
 Constructor
 */
Cmtlr5Step::Cmtlr5Step()
	{
	SetTestStepName(Kmtlr5Step);
	}

/**
 @return - TVerdict code
 Override of base class virtual
 */
TVerdict Cmtlr5Step::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();
	 
	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CSuplGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	// components for generating incoming SMS SUPL INIT
	iSuplPush = CLbsSuplPush::NewL(ELbsSuplPushChannelSMS, *this);
	iSuplInitGenerator = CSuplInitGenerator::NewL();

	return TestStepResult();
	}


/** Perform Cmtlr5Step test step.

This test checks that the Protocol Module handles the case in which LBS requests further
assistance data.

2 data items are requested - one is included in the rrlp assistance data message, and one 
is included in the rrlp measure position request. 

@return TVerdict test result code
*/
TVerdict Cmtlr5Step::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMTLR basic procedure - not all requested assistance data delivered..."));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));	
	

	// Initiate MT-LR
	// Generate received SUPL INIT message (test message content #0)
	TPtr8 messagePtr(iReceiveBuffer.Des());
	TInt err = iSuplInitGenerator->GenerateSuplInitL(0, messagePtr);
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// Inject SUPL INIT using the SUPL Push API
	INFO_PRINTF1(_L("\tLBS (SuplPush) -> OnSuplInit()"));
	TLbsSuplPushRequestId reqId = 12345;
	TPtrC8 message(messagePtr);
	iSuplPush->SuplInit(reqId, message, 0);
	
	// Check Gateway receives a Privacy request and Location Request
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest()"));
	if (EFail == CheckGatewayCallbackL(CSuplGatewayObserver::EProcessLocationRequest) ||
		!iGatewayObserver->IsPrivReqReceived() ||
		MLbsNetworkProtocolObserver::EServiceMobileTerminated != iGatewayObserver->LocType() )
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}


	// LBS delivers privacy response
	INFO_PRINTF1(_L("\tLBS -> RespondPrivacyRequest ()"));	
	CLbsNetworkProtocolBase::TLbsPrivacyResponse privacyResponse = CLbsNetworkProtocolBase::EPrivacyResponseAccepted;
	iModule->RespondPrivacyRequest(iGatewayObserver->SessionIdValue(), privacyResponse, 0);
	
	// Check Connection Manager receives a request for connecting
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::EConnectReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t ConnectionRequest -> NET"));


	// Simulate the connection is up (inject that event)
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Connected <- NET"));
    iNetworkObserver->InjectConnectedIndication(iNetworkObserver->SessionId());
    

	// LBS Requests assistance data
	INFO_PRINTF1(_L("\tLBS -> RequestAssistanceData ()"));	
	// Request both sets of assitance data. 
	// They'll be delivered in different SUPL POS messages later on in the test.
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataBadSatList;
	TLbsAsistanceDataGroup dataRequestMask2 = EAssistanceDataIonosphericModel;
	TLbsAsistanceDataGroup dataRequestMask = dataRequestMask1|dataRequestMask2;
	TLbsNetSessionIdArray dummyIdArray;	
	iModule->RequestAssistanceData(dataRequestMask, dummyIdArray);
	
	
	// Check Connection Manager receives a request to send a SUPL POS INIT
	// with the assistance data mask requested by the gateway
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosInitSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS INIT -> NET"));


	// Inject a SUPL POS with some of the Assistance data requested
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Assitance Data <- NET"));
	CSuplMessageBase* suplPos = BuildSuplPosAssitDataL(dataRequestMask1, EFalse);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplPos);	


	// Check gateway receives the assistance data types requested.
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessAssistanceData) ||
		(dataRequestMask1 & iGatewayObserver->AssistanceDataSetMask() != dataRequestMask1))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	

	// Check the Connection Manager receives a request to send a SUPL POS (ack to assistance data)
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Assistance Data Ack -> NET"));
	
	// Inject a SUPL POS (RRLP Measure Position Request) - does not contains missing assistance data
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Measure Position Request <- NET"));
	suplPos = BuildSuplPosRequestL(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted, dataRequestMask2);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplPos);	

	// Check gateway receives the assistance data types requested.
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessAssistanceData) ||
		(dataRequestMask2 & iGatewayObserver->AssistanceDataSetMask() != dataRequestMask2))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// Check gateway receives a location request (due to the RRLP Measure Position Request)
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceMobileTerminated != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	

	// Fake LBS sending results (position) to SPM
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	TReal64 latitude = 10;
	TReal64 longitude = 0;
	TReal32 HorAccuracy = 10;
	TPositionInfo mobilePosInfo;
	TTime utcTime;
	utcTime.UniversalTime();
	TCoordinate coor(latitude,longitude);
	TPosition mobilePos(TLocality(coor,HorAccuracy),utcTime);
	mobilePosInfo.SetPosition(mobilePos);
	iModule->RespondLocationRequest(iGatewayObserver->SessionIdValue(),KErrNone,mobilePosInfo);
	

	// Check the Connection Manager receives a request to send a SUPL POS (RRLP Measure Position Response)
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Measure Position Response -> NET"));


	// Inject a SUPL END (without position)
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL END <- NET"));
	CSuplMessageBase* suplEnd = BuildSuplEndL(EFalse);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplEnd);
	
	
	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessSessionComplete))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));

	// Check Connection Manager receives a disconnection request
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::EDisconnectReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t DisconnectRequest -> NET"));
	
	// Check if more observer activity takes place
	if (iGatewayObserver->IsMoreObserverActivity() ||
	iNetworkObserver->IsMoreObserverActivity())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("- END -"));
	
	SetTestStepResult(EPass);
	return TestStepResult();
	
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmtlr5Step::doTestStepPostambleL()
	{
	delete iModule;

	delete iSuplPush;
	delete iSuplInitGenerator;

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}


/**
Overrides the pure virtual MLbsSuplPushObserver::OnSuplInitComplete.
Receives the result of the message sending.
	
@param aError [In] The error code or KErrNone if successful.

@see MTe_LbsSuplSmsTriggerSenderObserver::OnMessageSent
@see CTe_LbsSuplSmsTriggerSender::SendMessage
*/
void Cmtlr5Step::OnSuplInitComplete(TLbsSuplPushChannel /*aChannel*/, TLbsSuplPushRequestId /*aReqId*/, TInt aError, TInt /*aReserved*/)
	{
	iSmsSendError = aError;
	if (iSmsSendError != KErrNone)
		{
		User::Invariant();
		}
	}

