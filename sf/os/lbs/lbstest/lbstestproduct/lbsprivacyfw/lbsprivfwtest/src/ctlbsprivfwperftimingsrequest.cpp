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
 @file ctlbsprivfwperftimingsrequest.cpp
*/

#include "ctlbsprivfwperftimingsrequest.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwPerfTimingsRequest::~CT_LbsPrivFwPerfTimingsRequest()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::~CT_LbsPrivFwPerfTimingsRequest()"));
	}
	

CT_LbsPrivFwPerfTimingsRequest::CT_LbsPrivFwPerfTimingsRequest()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}


CT_LbsPrivFwPerfTimingsRequest* CT_LbsPrivFwPerfTimingsRequest::NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwPerfTimingsRequest* me = new(ELeave)CT_LbsPrivFwPerfTimingsRequest();
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwPerfTimingsRequest::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	CT_LbsPrivFwBaseRequest::ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	INFO_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::ConstructL()"));
	}


void CT_LbsPrivFwPerfTimingsRequest::Start()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::Start()"));
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


void CT_LbsPrivFwPerfTimingsRequest::SetDecision(CPosNetworkPrivacy::TRequestDecision aDecision)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::SetDecision()"));
	iDecision = aDecision;
	}


void CT_LbsPrivFwPerfTimingsRequest::SetDesiredResponse(TResponseType aResponseType)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::SetDesiredResponse()"));
	iDesiredResponse = aResponseType;
	}

void CT_LbsPrivFwPerfTimingsRequest::ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::ProcessPrivApiData()"));
	// Validate
	CT_LbsPrivFwBaseRequest::ProcessPrivApiData(aRequestId, aCompletionCode);
	// Not much here, validation mostly done in base
	if((aCompletionCode == KErrAccessDenied) && (iDesiredResponse == EResponseTypeRejected))
		{
		if(iState != EReqSubmitted)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
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
		if(iState != EReqSubmitted)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::ProcessPrivApiData() - Test Failed - Error codes should have been rejected"));
			iStep->SetTestStepResult(EFail);
			iState = EReqFailed;
			}
		iState = EReqValidated;
		}
	else if((aCompletionCode == KErrNone) && (iDesiredResponse == EResponseTypeAccepted))
		{
		if(iState != EReqSubmitted)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
			iStep->SetTestStepResult(EFail);
			iState = EReqFailed;
			}
		iState = EReqValidated;
		}
	else
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::ProcessPrivApiData() - Test Failed - Actual completion code does not match desired completion code"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);
        }
	}

void CT_LbsPrivFwPerfTimingsRequest::ProcessPrivNotifierData(TInt /*aRequestId*/, TLbsTestRequestInfo& /*aPrivNotifierData*/)
	{
	// This should never happen during the performance timing tests
	ERR_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::ProcessPrivNotifierData() - Privacy Notifier Data received unexpected callback during verification request"));
	iStep->SetTestStepResult(EFail);
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
        TerminateSessionL(iRequestId);
        }
	}

void CT_LbsPrivFwPerfTimingsRequest::ProcessPrivRequestComplete(TInt aRequestId, TInt /*aCompletionCode*/)
	{
	if (iPrivacyHandler != EPrivacyHandleByController)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::ProcessPrivRequestComplete() - Test Failed - Callback should not be invoked for this privacy handler"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}
	
	if(iState != EReqValidated)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwPerfTimingsRequest::ProcessPrivRequestComplete() - Test Failed - Invalid state"));
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

TBool CT_LbsPrivFwPerfTimingsRequest::IsRequestComplete()
	{
	return ((iState == EReqValidated) || (iState == EReqFailed));
	}
