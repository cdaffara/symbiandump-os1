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

/**
 @file ctlbsprivfwcancelrequest.cpp
*/

#include "ctlbsprivfwtimeoutrequest.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwTimeoutRequest::~CT_LbsPrivFwTimeoutRequest()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::~CT_LbsPrivFwTimeoutRequest()"));
	}
	

CT_LbsPrivFwTimeoutRequest::CT_LbsPrivFwTimeoutRequest()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}


CT_LbsPrivFwTimeoutRequest* CT_LbsPrivFwTimeoutRequest::NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwTimeoutRequest* me = new(ELeave)CT_LbsPrivFwTimeoutRequest();
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwTimeoutRequest::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	CT_LbsPrivFwBaseRequest::ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::ConstructL()"));

	iResponseDelay = 0;
	iNetworkTimeout = 30; //30s
	}


void CT_LbsPrivFwTimeoutRequest::Start()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::Start()"));
	//Set Connection Timeout value
	//iNetPrivacy->SetConnectionTimeout(iNetworkTimeout * 1000000);
	if(iBehaviourType == EReqInvalidTimeoutId)
		{
        INFO_PRINTF2(_L("NotifyVerificationTimeoutL(0x%x)"), iRequestId);
		iNetPrivacy->NotifyVerificationTimeoutL(*iRequestInfo, iRequestId, CPosNetworkPrivacy::EDecisionRejected);
		iState = EReqSubmitted;
		return;
		}
	// If we have a default decision - send it to the API
	if(iDecision == CPosNetworkPrivacy::EDecisionNotAvailable)
		{
        INFO_PRINTF2(_L("VerifyLocationRequestL(0x%x)"), iRequestId);
		iNetPrivacy->VerifyLocationRequestL(*iRequestInfo, iRequestId, *iApiObserver);
		}
	else
		{
        INFO_PRINTF2(_L("VerifyLocationRequestL(0x%x)"), iRequestId);
		iNetPrivacy->VerifyLocationRequestL(*iRequestInfo, iRequestId, *iApiObserver, iDecision);
		}
	iState = EReqSubmitted;
	}



void CT_LbsPrivFwTimeoutRequest::SetNetworkTimeout(TUint aNetworkTimeout)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::SetNetworkTimeout()"));
	iNetworkTimeout = aNetworkTimeout;
	}

void CT_LbsPrivFwTimeoutRequest::SetDecision(CPosNetworkPrivacy::TRequestDecision aDecision)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::SetDecision()"));
	iDecision = aDecision;
	}

void CT_LbsPrivFwTimeoutRequest::SetDesiredResponse(TResponseType aResponseType)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::SetDesiredResponse()"));
	iDesiredResponse = aResponseType;
	}

void CT_LbsPrivFwTimeoutRequest::SetBehaviourType(TReqBehaviourType aBehaviourType)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::SetBehaviourType()"));
	iBehaviourType = aBehaviourType;
	}

void CT_LbsPrivFwTimeoutRequest::SetRequestId(TInt aRequestId)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::SetRequestId()"));
	iRequestId = aRequestId;
	if(iRequestId < 1)
		{
		iNetPrivacy->SetBadRequestId();
		}
	}

void CT_LbsPrivFwTimeoutRequest::ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::ProcessPrivApiData()"));
	// Validate
	CT_LbsPrivFwBaseRequest::ProcessPrivApiData(aRequestId, aCompletionCode);
	// Not much here, validation mostly done in base

	if(iPrivacyHandler != EPrivacyHandleByQnN) // If handler is Ctrl or Notifier
		{
		// Decision should match our timeout strategy
		if((aCompletionCode == KErrAccessDenied) && 
				((iDecision == CPosNetworkPrivacy::EDecisionRejected) || (iDecision == CPosNetworkPrivacy::EDecisionNotAvailable)))
			{
			if(iState != EReqRejected)
				{
				ERR_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
				iStep->SetTestStepResult(EFail);
				iState = EReqTestFailed;
				return;
				}
			}
		// Decision should match our timeout strategy
		else if((aCompletionCode == KErrNone) && (iDecision == CPosNetworkPrivacy::EDecisionAccepted))
			{
			if(iState != EReqAccepted)
				{
				ERR_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
				iStep->SetTestStepResult(EFail);
				iState = EReqTestFailed;
				return;
				}
			}
		else
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::ProcessPrivApiData() - Test Failed - Actual completion code does not match desired completion code"));
			iStep->SetTestStepResult(EFail);
			iState = EReqTestFailed;
			return;
			}
		}
	else // If handler is QNN
		{
		 // QNN Timeout should always result in rejection
		if((aCompletionCode != KErrAccessDenied) || (iState != EReqRejected))
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
			iStep->SetTestStepResult(EFail);
			iState = EReqTestFailed;
			return;
			}
		}
	// If none of the tests above failed then the response is correct
	iState = EReqValidated;
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);	
        }
	}


void CT_LbsPrivFwTimeoutRequest::ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::ProcessPrivNotifierData()"));
	
	if ((iState == EReqAccepted) || (iState == EReqRejected) || (iState == EReqValidated))
		{
		return;
		}
	else if(iState != EReqSubmitted)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::ProcessPrivNotifierData() - Test Failed - Invalid state"));
		iStep->SetTestStepResult(EFail);
		iState = EReqTestFailed;
		return;
		}
	
	// Validate Parameters in Base class
	CT_LbsPrivFwBaseRequest::ProcessPrivNotifierData(aRequestId, aPrivNotifierData);
	if(iBehaviourType == EReqInvalidTimeoutId)
		{
		iProxy->SendNotifierResponse(ERequestNotify, aRequestId, EResponseTypeAccepted, iResponseDelay);
		iState = EReqValidated;
	    if(iStep->NeedSessionTerminate())
	        {		
            INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
            TerminateSessionL(iRequestId);	
	        }
		return;
		}


	if(iPrivacyHandler == EPrivacyHandleByQnN)
		{
		//send a timeout response with err EResponseTypeTimedOut
		iProxy->SendNotifierResponse(ERequestVerify, aRequestId, EResponseTypeTimedOut, iResponseDelay);
		iState = EReqRejected;
		}
	else
		{
		//Ctrl and Notifier usually send no response - the system should use our timeout strategy
		if(iDecision == CPosNetworkPrivacy::EDecisionAccepted)
			{
			iState = EReqAccepted;
			}
		else if ((iDecision == CPosNetworkPrivacy::EDecisionRejected) || (iDecision == CPosNetworkPrivacy::EDecisionNotAvailable))
			{
			iState = EReqRejected;
			}
		
		if(iBehaviourType == EReqRespondAfterComplete)
			{
			User::After(20000000);
			iProxy->SendNotifierResponse(ERequestVerify, aRequestId, iDesiredResponse, iResponseDelay);
			}
		else if(iBehaviourType == EReqSendTimeoutError)
			{
			// This test is only valid for the notifier
			ASSERT(iPrivacyHandler == EPrivacyHandleByNotifier);
			// Send back a response claiming we have timed out
			iProxy->SendNotifierResponse(ERequestVerify, aRequestId, EResponseTypeTimedOut, iResponseDelay);
			}
		
		}	
	}


void CT_LbsPrivFwTimeoutRequest::ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode)
	{
	if((iPrivacyHandler != EPrivacyHandleByController) 
			|| (iState != EReqValidated)
			|| (aCompletionCode != KErrNone))
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::ProcessPrivRequestComplete() - Test Failed"));
		iStep->SetTestStepResult(EFail);
		iState = EReqTestFailed;
		return;
		}
	if(iBehaviourType == EReqRespondAfterComplete)
		{
		iProxy->SendNotifierResponse(ERequestVerify, aRequestId, iDesiredResponse, iResponseDelay);
		}
	iState = EReqCtrlFinal;	
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);
        }
	}

TBool CT_LbsPrivFwTimeoutRequest::IsRequestComplete()
	{
	if (iPrivacyHandler == EPrivacyHandleByController)
		{
		// Controller has one extra state
		return ((iState == EReqCtrlFinal) || (iState == EReqTestFailed));
		}
	else 
		{
		return ((iState == EReqValidated) || (iState == EReqTestFailed));
		}
	}
