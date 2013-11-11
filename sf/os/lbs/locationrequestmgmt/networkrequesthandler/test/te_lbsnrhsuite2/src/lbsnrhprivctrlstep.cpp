// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CTestStep derived implementation of tests for the Network Request Handler
// Privacy Notifier.
// 
//

/**
 @file lbsnrhprivacyadvancednotifierstep.cpp
*/

#include "lbsnrhprivctrlstep.h"
#include "te_lbsnrhsuite2defs.h"
#include "lbsnrhngmsgs.h"

const TLbsNetSessionIdInt KDummySessionId;

const TUid KLbsPrivacyProtocolModuleUid = { 0x1028373C };


/**
 * Destructor
 */
CLbsNrhPrivCtrlStep::~CLbsNrhPrivCtrlStep()
	{
	}

/**
 * Constructor
 */
CLbsNrhPrivCtrlStep::CLbsNrhPrivCtrlStep()
	{
	SetTestStepName(KLbsNrhPrivCtrlStep);
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNrhPrivCtrlStep::doTestStepPreambleL()
	{      
	// Common initialisation  
	CTe_LbsNrhSuiteStepBase::doTestStepPreambleL();
	
	iNgChannel = CNgChannel::NewL(Logger());
	iPrivCtrlChannel = CPrivCtrlChannel::NewL(Logger());
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CLbsNrhPrivCtrlStep::doTestStepL()
	{
	if (TestStepResult()==EPass)
		{		
		Test001L();
		
		Test002L();
		
		Test003L();
		}
		
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNrhPrivCtrlStep::doTestStepPostambleL()
	{	
	delete iNgChannel;
	delete iPrivCtrlChannel;
	
	CTe_LbsNrhSuiteStepBase::doTestStepPostambleL();

	return TestStepResult();
	}

/* Single privacy request & response
*/
void CLbsNrhPrivCtrlStep::Test001L()
	{
	// Send privacy (MT-LR) request
	TLbsNetSessionIdInt sessionId(KLbsPrivacyProtocolModuleUid, 1);
	TLbsNetMtLrRequestMsg* privReqMsg = CreatePrivacyVerificationMsgLC(sessionId);
	iNgChannel->SendMessageAndWait(*privReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(privReqMsg);

	// Wait for response from Privacy Controller API
	iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(CPrivCtrlChannel::EDataResponsePrivReq == 
			iPrivCtrlChannel->GetNextResponseType());
	TUint requestId;
	MLbsPrivacyObserver::TNotificationType notificationType;
	TLbsExternalRequestInfo reqInfo;
	iPrivCtrlChannel->GetNextResponseAsPrivacyRequestL(
			requestId, 
			notificationType,
			reqInfo);
	
	// Send response to privacy request
	iPrivCtrlChannel->SendPrivacyResponse(
			requestId, 
			CLbsPrivacyController::ERequestAccepted,
			KDefaultTimeout);
	
	// Wait for response to reach NG
	iNgChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(TLbsNetInternalMsgBase::EPrivacyResponse == 
			iNgChannel->GetNextResponseType());
	TLbsNetMtLrReponseMsg privacyResponse(KDummySessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted, KErrNone);
	iNgChannel->GetNextResponseL(privacyResponse);
	TESTL(privacyResponse.Response() == TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
	// Complete request
	TLbsNetSessionCompleteMsg* sessCompMsg = CreateSessionCompeleteMsgLC(
												sessionId, KErrNone);
	iNgChannel->SendMessageAndWait(*sessCompMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(sessCompMsg);

	// Wait for complete at Privacy Controller
	iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
	CPrivCtrlChannel::TPrivCtrlResponseType type = iPrivCtrlChannel->GetNextResponseType();
	TESTL(CPrivCtrlChannel::EDataResponseReqComplete == 
			iPrivCtrlChannel->GetNextResponseType());
	TInt reason;
	iPrivCtrlChannel->GetNextResponseAsRequestCompleteL(
			requestId,
			reason);	
	}

/* Double simultaneous privacy request & response
*/
void CLbsNrhPrivCtrlStep::Test002L()
	{
	// Send privacy (MT-LR) request
	TLbsNetSessionIdInt sessionId(KLbsPrivacyProtocolModuleUid, 1);
	TLbsNetMtLrRequestMsg* privReqMsg = CreatePrivacyVerificationMsgLC(sessionId);
	iNgChannel->SendMessageAndWait(*privReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(privReqMsg);
	
	// Send second privacy request
	TLbsNetSessionIdInt sessionId2(KLbsPrivacyProtocolModuleUid, 2);
	privReqMsg = CreatePrivacyVerificationMsgLC(sessionId2);
	iNgChannel->SendMessageAndWait(*privReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(privReqMsg);

	// Wait for response from Privacy Controller API
	iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(CPrivCtrlChannel::EDataResponsePrivReq == 
			iPrivCtrlChannel->GetNextResponseType());
	TUint requestId;
	MLbsPrivacyObserver::TNotificationType notificationType;
	TLbsExternalRequestInfo reqInfo;
	iPrivCtrlChannel->GetNextResponseAsPrivacyRequestL(
			requestId, 
			notificationType,
			reqInfo);

	// Wait for 2nd response from Privacy Controller API
	iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(CPrivCtrlChannel::EDataResponsePrivReq == 
			iPrivCtrlChannel->GetNextResponseType());
	TUint requestId2;
	iPrivCtrlChannel->GetNextResponseAsPrivacyRequestL(
			requestId2, 
			notificationType,
			reqInfo);
	
	// Send response to privacy request
	iPrivCtrlChannel->SendPrivacyResponse(
			requestId, 
			CLbsPrivacyController::ERequestAccepted,
			KDefaultTimeout);
	
	// Wait for response to reach NG
	iNgChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(TLbsNetInternalMsgBase::EPrivacyResponse == 
			iNgChannel->GetNextResponseType());
	TLbsNetMtLrReponseMsg privacyResponse(KDummySessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted, KErrNone);
	iNgChannel->GetNextResponseL(privacyResponse);
	TESTL(privacyResponse.Response() == TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
	// Send response to 2nd privacy request
	iPrivCtrlChannel->SendPrivacyResponse(
			requestId2, 
			CLbsPrivacyController::ERequestRejected,
			KDefaultTimeout);

	// Wait for 2nd response to reach NG
	iNgChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(TLbsNetInternalMsgBase::EPrivacyResponse == 
			iNgChannel->GetNextResponseType());
	iNgChannel->GetNextResponseL(privacyResponse);
	TESTL(privacyResponse.Response() == TLbsNetworkEnumInt::EPrivacyResponseRejected);
	
	// Complete request
	TLbsNetSessionCompleteMsg* sessCompMsg = CreateSessionCompeleteMsgLC(
												sessionId, KErrNone);
	iNgChannel->SendMessageAndWait(*sessCompMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(sessCompMsg);

	// 2nd complete request
	sessCompMsg = CreateSessionCompeleteMsgLC(sessionId2, KErrNone);
	iNgChannel->SendMessageAndWait(*sessCompMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(sessCompMsg);
	
	// Wait for complete at Privacy Controller
	iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
	CPrivCtrlChannel::TPrivCtrlResponseType type = iPrivCtrlChannel->GetNextResponseType();
	TESTL(CPrivCtrlChannel::EDataResponseReqComplete == 
			iPrivCtrlChannel->GetNextResponseType());
	TInt reason;
	iPrivCtrlChannel->GetNextResponseAsRequestCompleteL(
			requestId,
			reason);

	// Wait for 2nd complete at Privacy Controller
	iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(CPrivCtrlChannel::EDataResponseReqComplete == 
			iPrivCtrlChannel->GetNextResponseType());
	TInt reason2;
	iPrivCtrlChannel->GetNextResponseAsRequestCompleteL(
			requestId2,
			reason2);
	}
	
/* Multiple privacy request & response, exceeds the maximum number allowed.

The NRH should reject any privacy requests that it cannot buffer.
E.g. if max privacy request setting is 5, then the 6th, 7th, etc. 
should be rejected.
*/
void CLbsNrhPrivCtrlStep::Test003L()
	{
	const TInt KMaxNumSimultaneousPrivacyRequests(10);
	
	// Send privacy (MT-LR) request
	for (TInt i = 0; i < KMaxNumSimultaneousPrivacyRequests; i++)
		{
		TLbsNetSessionIdInt sessionId(KLbsPrivacyProtocolModuleUid, (i + 1));
		TLbsNetMtLrRequestMsg* privReqMsg = CreatePrivacyVerificationMsgLC(sessionId);
		iNgChannel->SendMessageAndWait(*privReqMsg, KDefaultTimeout);
		CleanupStack::PopAndDestroy(privReqMsg);
		}

	// Wait for response from Privacy Controller API
	for (TInt i = 0; i < KMaxNumSimultaneousPrivacyRequests; i++)
		{
		iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
		TESTL(CPrivCtrlChannel::EDataResponsePrivReq == 
				iPrivCtrlChannel->GetNextResponseType());
		TUint requestId;
		MLbsPrivacyObserver::TNotificationType notificationType;
		TLbsExternalRequestInfo reqInfo;
		iPrivCtrlChannel->GetNextResponseAsPrivacyRequestL(
				requestId, 
				notificationType,
				reqInfo);
		}
	
	// Send response to privacy request
	for (TInt i = 0; i < KMaxNumSimultaneousPrivacyRequests; i++)
		{
		TUint requestId(i + 1);
		iPrivCtrlChannel->SendPrivacyResponse(
				requestId, 
				CLbsPrivacyController::ERequestAccepted,
				KDefaultTimeout);
	
		// Wait for response to reach NG
		iNgChannel->WaitForResponseL(KDefaultTimeout);
		TESTL(TLbsNetInternalMsgBase::EPrivacyResponse == 
				iNgChannel->GetNextResponseType());
		TLbsNetMtLrReponseMsg privacyResponse(KDummySessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted, KErrNone);
		iNgChannel->GetNextResponseL(privacyResponse);
		TESTL(privacyResponse.Response() == TLbsNetworkEnumInt::EPrivacyResponseAccepted);
		}
	
	// Complete request
	for (TInt i = 0; i < KMaxNumSimultaneousPrivacyRequests; i++)
		{
		TLbsNetSessionIdInt sessionId(KLbsPrivacyProtocolModuleUid, (i + 1));
		TLbsNetSessionCompleteMsg* sessCompMsg = CreateSessionCompeleteMsgLC(
													sessionId, KErrNone);
		iNgChannel->SendMessageAndWait(*sessCompMsg, KDefaultTimeout);
		CleanupStack::PopAndDestroy(sessCompMsg);
		}
	
	// Wait for complete at Privacy Controller
	for (TInt i = 0; i < KMaxNumSimultaneousPrivacyRequests; i++)
		{
		iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
		CPrivCtrlChannel::TPrivCtrlResponseType type = iPrivCtrlChannel->GetNextResponseType();
		TESTL(CPrivCtrlChannel::EDataResponseReqComplete == 
				iPrivCtrlChannel->GetNextResponseType());
		TInt reason;
		TUint requestId;
		iPrivCtrlChannel->GetNextResponseAsRequestCompleteL(
				requestId,
				reason);
		}
	}

