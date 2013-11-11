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

#include "ctlbsprivfwnotifyrequest.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwNotifyRequest::~CT_LbsPrivFwNotifyRequest()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwNotifyRequest::~CT_LbsPrivFwNotifyRequest()"));
	}
	

CT_LbsPrivFwNotifyRequest::CT_LbsPrivFwNotifyRequest()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}


CT_LbsPrivFwNotifyRequest* CT_LbsPrivFwNotifyRequest::NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwNotifyRequest* me = new(ELeave)CT_LbsPrivFwNotifyRequest();
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwNotifyRequest::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	CT_LbsPrivFwBaseRequest::ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	INFO_PRINTF1(_L("CT_LbsPrivFwNotifyRequest::ConstructL()"));
	}


void CT_LbsPrivFwNotifyRequest::Start()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwNotifyRequest::Start()"));
	
	// Start the notify request.
	iNetPrivacy->NotifyLocationRequestL(*iRequestInfo, iRequestId);
	iState = EReqSubmitted;
	}




void CT_LbsPrivFwNotifyRequest::ProcessPrivApiData(TInt /*aRequestId*/, TInt /*aCompletionCode*/)
	{
	// This should never happen during a notification
	ERR_PRINTF1(_L("CT_LbsPrivFwNotifyRequest::ProcessPrivApiData() - Privacy API received unexpected callback during Notification request"));
	iStep->SetTestStepResult(EFail);
	}

void CT_LbsPrivFwNotifyRequest::ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwNotifyRequest::ProcessPrivNotifierData()"));
	// Validate Parameters in Base class
	CT_LbsPrivFwBaseRequest::ProcessPrivNotifierData(aRequestId, aPrivNotifierData);
	if(iState != EReqSubmitted)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwNotifyRequest::ProcessPrivRequestComplete() - Failed: Invalid state"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}

	iState = EReqCompleted;
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF2(_L("Terminating Session with iRequestId 0x%x"), iRequestId);
        TerminateSessionL(iRequestId);
        }
	}

void CT_LbsPrivFwNotifyRequest::ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode)
	{
	(void)aRequestId;
	(void)aCompletionCode;
	
	if(iState != EReqCompleted)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwNotifyRequest::ProcessPrivRequestComplete() - Failed: Invalid state"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}
	iState = EReqCtrlFinal;
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF2(_L("Terminating Session with iRequestId 0x%x)"), iRequestId);
        TerminateSessionL(iRequestId);    
        }
	}

TBool CT_LbsPrivFwNotifyRequest::IsRequestComplete()
	{
	if (iPrivacyHandler == EPrivacyHandleByController)
		{
		// Controller has one extra state
		return ((iState == EReqCtrlFinal) || (iState == EReqFailed));
		}
	else 
		{
		return ((iState == EReqCompleted) || (iState == EReqFailed));
		}
	}
