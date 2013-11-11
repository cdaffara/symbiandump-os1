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
 @file ctlbsprivfwverifyrequest.cpp
*/

#include "ctlbsprivfwverifyrequest.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwVerifyRequest::~CT_LbsPrivFwVerifyRequest()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::~CT_LbsPrivFwVerifyRequest()"));
	}
	

CT_LbsPrivFwVerifyRequest::CT_LbsPrivFwVerifyRequest()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}


CT_LbsPrivFwVerifyRequest* CT_LbsPrivFwVerifyRequest::NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwVerifyRequest* me = new(ELeave)CT_LbsPrivFwVerifyRequest();
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwVerifyRequest::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	CT_LbsPrivFwBaseRequest::ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	INFO_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ConstructL()"));
	}


void CT_LbsPrivFwVerifyRequest::Start()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::Start()"));
	// If we have a default decision - send it to the API
	if(iDecision == CPosNetworkPrivacy::EDecisionNotAvailable)
		{
		iNetPrivacy->VerifyLocationRequestL(*iRequestInfo, iRequestId, *iApiObserver);
		}
	else
		{
		iNetPrivacy->VerifyLocationRequestL(*iRequestInfo, iRequestId, *iApiObserver, iDecision);
		}
	iState = EReqSubmitted;
	}


void CT_LbsPrivFwVerifyRequest::SetDecision(CPosNetworkPrivacy::TRequestDecision aDecision)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::SetDecision()"));
	iDecision = aDecision;
	}


void CT_LbsPrivFwVerifyRequest::SetDesiredResponse(TResponseType aResponseType)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::SetDesiredResponse()"));
	iDesiredResponse = aResponseType;
	}

void CT_LbsPrivFwVerifyRequest::ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivApiData()"));
	// Validate
	CT_LbsPrivFwBaseRequest::ProcessPrivApiData(aRequestId, aCompletionCode);
	// Not much here, validation mostly done in base
	if((aCompletionCode == KErrAccessDenied) && (iDesiredResponse == EResponseTypeRejected))
		{
		if(iState != EReqRejected)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
			iStep->SetTestStepResult(EFail);
			iState = EReqFailed;
			}
		iState = EReqValidated;
		}
	else if((aCompletionCode == KErrAccessDenied) && ( //Check that error codes cause rejected requests
									(iDesiredResponse == EResponseTypeTimedOut) ||
									(iDesiredResponse == EResponseTypeNoMemory) ||
									(iDesiredResponse == EResponseTypeNotfound) ))
		{
		if(iState != EReqRejected)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivApiData() - Test Failed - Error codes should have been rejected"));
			iStep->SetTestStepResult(EFail);
			iState = EReqFailed;
			}
		iState = EReqValidated;
		}
	else if((aCompletionCode == KErrNone) && (iDesiredResponse == EResponseTypeAccepted))
		{
		if(iState != EReqAccepted)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
			iStep->SetTestStepResult(EFail);
			iState = EReqFailed;
			}
		iState = EReqValidated;
		}
	else
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivApiData() - Test Failed - Actual completion code does not match desired completion code"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);
        }
	}

void CT_LbsPrivFwVerifyRequest::ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivNotifierData()"));
	// Validate Parameters in Base class
	
	if(iState == EReqAccepted || iState == EReqRejected || iState == EReqValidated)
		{
		return;
		}
	
	//TMessageType msgType = aPrivNotifierData.MessageType();
	//if(msgType == EMessageVerifyCancelRequest)
	//	{
	//	return;
	//	}
	
	CT_LbsPrivFwBaseRequest::ProcessPrivNotifierData(aRequestId, aPrivNotifierData);

	if(iState != EReqSubmitted)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivNotifierData() - Test Failed - Invalid state"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
	    if(iStep->NeedSessionTerminate())
	        {		
            INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
            TerminateSessionL(iRequestId);
	        }
		}
	// Send response
	// Warning, When request Ids match, use iRequestId instead of aRequestId
	iProxy->SendNotifierResponse(ERequestVerify, aRequestId, iDesiredResponse, iResponseDelay);
	if(iDesiredResponse == EResponseTypeAccepted)
		{
		iState = EReqAccepted;
		}
	else if ((iDesiredResponse == EResponseTypeRejected) ||
			 (iDesiredResponse == EResponseTypeTimedOut) ||
			 (iDesiredResponse == EResponseTypeNoMemory) ||
			 (iDesiredResponse == EResponseTypeNotfound) )
		{
		iState = EReqRejected;
		}

	}

void CT_LbsPrivFwVerifyRequest::ProcessPrivRequestComplete(TInt aRequestId, TInt /*aCompletionCode*/)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivRequestComplete()"));
	
	if (iPrivacyHandler != EPrivacyHandleByController)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivRequestComplete() - Test Failed - Callback should not be invoked for this privacy handler"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}
	
	if(iState != EReqValidated)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwVerifyRequest::ProcessPrivRequestComplete() - Test Failed - Invalid state"));
		ERR_PRINTF2(_L("CT_LbsPrivFwVerifyRequest::iState = %d"),iState);
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}
	
	iState = EReqCtrlFinal;
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);	
        }
	}

TBool CT_LbsPrivFwVerifyRequest::IsRequestComplete()
	{
	if (iPrivacyHandler == EPrivacyHandleByController)
		{
		// Controller has one extra state
		return ((iState == EReqCtrlFinal) || (iState == EReqFailed));
		}
	else 
		{
		return ((iState == EReqValidated) || (iState == EReqFailed));
		}
	}
