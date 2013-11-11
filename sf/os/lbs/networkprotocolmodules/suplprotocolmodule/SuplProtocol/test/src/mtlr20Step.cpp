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
 @file mtlr20Step.cpp
 @internalTechnology
*/
#include "mtlr20Step.h"
#include "te_suplprotocolsuitedefs.h"

/**
 Destructor
 */
Cmtlr20Step::~Cmtlr20Step()

	{
	}

/**
 Constructor
 */
Cmtlr20Step::Cmtlr20Step()
	{
	SetTestStepName(Kmtlr20Step);
	}

/**
 @return - TVerdict code
 Override of base class virtual
 */
TVerdict Cmtlr20Step::doTestStepPreambleL()
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


/** Perform Cmtlr20Step test step.
This test verifies that the SUPL Protocol Module correctly handles 
an MT-LR Network requested Locate Terminal Based sequence (assistance data
delivered via an RRLP payload).

@return TVerdict test result code
*/
TVerdict Cmtlr20Step::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMTLR rejected by user + connection inmediately available)"));
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
	INFO_PRINTF1(_L("\tLBS -> PrivacyResponse (EPrivacyResponseRejected)"));	
	CLbsNetworkProtocolBase::TLbsPrivacyResponse privacyResponse = CLbsNetworkProtocolBase::EPrivacyResponseRejected;
	iModule->RespondPrivacyRequest(iGatewayObserver->SessionIdValue(), privacyResponse, 0);

	// LBS delivers a Location Response with KErrAccessDenied after privacy is rejected.
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest (KErrAccessDenied)"));
	TPositionInfo mobilePosInfo;
	iModule->RespondLocationRequest(iGatewayObserver->SessionIdValue(), KErrAccessDenied, mobilePosInfo);

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
    
	// Check the Connection Manager receives a request to send a SUPL END 
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplEndSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL END -> NET"));

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
TVerdict Cmtlr20Step::doTestStepPostambleL()
	{
	delete iModule;

	delete iSuplPush;
	delete iSuplInitGenerator;

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}


/**
Overrides the pure virtual MTe_LbsSuplSmsTriggerSenderObserver::OnMessageSent.
Receives the result of the message sending.
	
@param aError [In] The error code or KErrNone if successful.

@see MTe_LbsSuplSmsTriggerSenderObserver::OnMessageSent
@see CTe_LbsSuplSmsTriggerSender::SendMessage
*/
void Cmtlr20Step::OnSuplInitComplete(TLbsSuplPushChannel /*aChannel*/, TLbsSuplPushRequestId /*aReqId*/, TInt aError, TInt /*aReserved*/)
	{
	iSmsSendError = aError;
	if (iSmsSendError != KErrNone)
		{
		User::Invariant();
		}
	}

