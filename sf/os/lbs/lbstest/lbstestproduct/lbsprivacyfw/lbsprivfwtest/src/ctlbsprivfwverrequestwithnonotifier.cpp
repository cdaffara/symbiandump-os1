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
 @file ctlbsprivfwverrequestwithnonotifier.cpp
*/

#include "ctlbsprivfwverrequestwithnonotifier.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwVerRequestWithNoNotifier::~CT_LbsPrivFwVerRequestWithNoNotifier()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerRequestWithNoNotifier::~CT_LbsPrivFwVerRequestWithNoNotifier()"));
	}
	

CT_LbsPrivFwVerRequestWithNoNotifier::CT_LbsPrivFwVerRequestWithNoNotifier()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}


CT_LbsPrivFwVerRequestWithNoNotifier* CT_LbsPrivFwVerRequestWithNoNotifier::NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwVerRequestWithNoNotifier* me = new(ELeave)CT_LbsPrivFwVerRequestWithNoNotifier();
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwVerRequestWithNoNotifier::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	CT_LbsPrivFwBaseRequest::ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	INFO_PRINTF1(_L("CT_LbsPrivFwVerRequestWithNoNotifier::ConstructL()"));
	}


void CT_LbsPrivFwVerRequestWithNoNotifier::Start()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerRequestWithNoNotifier::Start()"));
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


void CT_LbsPrivFwVerRequestWithNoNotifier::SetDecision(CPosNetworkPrivacy::TRequestDecision aDecision)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerRequestWithNoNotifier::SetDecision()"));
	iDecision = aDecision;
	}


void CT_LbsPrivFwVerRequestWithNoNotifier::SetDesiredResponse(TResponseType aResponseType)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerRequestWithNoNotifier::SetDesiredResponse()"));
	iDesiredResponse = aResponseType;
	}

void CT_LbsPrivFwVerRequestWithNoNotifier::ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwVerRequestWithNoNotifier::ProcessPrivApiData()"));
	// Validate
	CT_LbsPrivFwBaseRequest::ProcessPrivApiData(aRequestId, aCompletionCode);
	// Not much here, validation mostly done in base
	if(aCompletionCode == KErrAccessDenied) //All requests rejected if there is no notifier
		{
		if(iState != EReqSubmitted)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwVerRequestWithNoNotifier::ProcessPrivApiData() - Test Failed - Invalid state"));
			iStep->SetTestStepResult(EFail);
			iState = EReqFailed;
			}
		iState = EReqValidated;
		}
	else
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwVerRequestWithNoNotifier::ProcessPrivApiData() - Test Failed - Actual completion code does not match desired completion code"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);
        }
	}

void CT_LbsPrivFwVerRequestWithNoNotifier::ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
	{
	(void)aRequestId;
	(void)aPrivNotifierData;
	// This should never happen during a notification with no notifier available
	ERR_PRINTF1(_L("CT_LbsPrivFwNotRequestWithNoNotifier::ProcessPrivNotifierData() - Notifier received unexpected callback during Notification request"));
	iStep->SetTestStepResult(EFail);
	iState = EReqFailed;
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
        TerminateSessionL(iRequestId);	
        }
	}

void CT_LbsPrivFwVerRequestWithNoNotifier::ProcessPrivRequestComplete(TInt aRequestId, TInt /*aCompletionCode*/)
	{
	ERR_PRINTF1(_L("CT_LbsPrivFwVerRequestWithNoNotifier::ProcessPrivRequestComplete() - Test Failed - Callback should not be invoked for this privacy handler"));
	iStep->SetTestStepResult(EFail);
	iState = EReqFailed;
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);
        }
	}

TBool CT_LbsPrivFwVerRequestWithNoNotifier::IsRequestComplete()
	{
	return ((iState == EReqValidated) || (iState == EReqFailed));
	}
