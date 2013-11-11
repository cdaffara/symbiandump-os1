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
 @file mtlr13Step.cpp
 @internalTechnology
*/
#include "mtlr13Step.h"
#include "te_suplprotocolsuitedefs.h"

/**
 Destructor
 */
Cmtlr13Step::~Cmtlr13Step()

	{
	}

/**
 Constructor
 */
Cmtlr13Step::Cmtlr13Step()
	{
	SetTestStepName(Kmtlr13Step);
	}

/**
 @return - TVerdict code
 Override of base class virtual
 */
TVerdict Cmtlr13Step::doTestStepPreambleL()
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


/** Perform Cmtlr13Step test step.

This test verifies that the SUPL Protocol Module correctly handles 
a SUPL INIT with SLP Mode element set to NON-Proxy.

Expected behaviour is for the message to be silently dropped.

@return TVerdict test result code
*/
TVerdict Cmtlr13Step::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMTLR - SLP MODE is NON-PROXY (expect it to be silently dropped      "));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));	
	

	// Initiate MT-LR
	// Generate received SUPL INIT message (test message content #0)
	TPtr8 messagePtr(iReceiveBuffer.Des());
	TInt err = iSuplInitGenerator->GenerateSuplInitL(1, messagePtr);
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
TVerdict Cmtlr13Step::doTestStepPostambleL()
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
void Cmtlr13Step::OnSuplInitComplete(TLbsSuplPushChannel /*aChannel*/, TLbsSuplPushRequestId /*aReqId*/, TInt aError, TInt /*aReserved*/)
	{
	iSmsSendError = aError;
	if (iSmsSendError != KErrNone)
		{
		User::Invariant();
		}
	}

