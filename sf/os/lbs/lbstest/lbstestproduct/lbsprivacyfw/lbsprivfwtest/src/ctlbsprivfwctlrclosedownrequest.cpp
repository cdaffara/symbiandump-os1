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
 @file ctlbsprivfwctlrclosedownrequest.cpp
*/

#include "ctlbsprivfwctlrclosedownrequest.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwCtlrCloseDownRequest::~CT_LbsPrivFwCtlrCloseDownRequest()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCtlrCloseDownRequest::~CT_LbsPrivFwCtlrCloseDownRequest()"));
	}
	

CT_LbsPrivFwCtlrCloseDownRequest::CT_LbsPrivFwCtlrCloseDownRequest()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}


CT_LbsPrivFwCtlrCloseDownRequest* CT_LbsPrivFwCtlrCloseDownRequest::NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwCtlrCloseDownRequest* me = new(ELeave)CT_LbsPrivFwCtlrCloseDownRequest();
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwCtlrCloseDownRequest::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	CT_LbsPrivFwBaseRequest::ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	INFO_PRINTF1(_L("CT_LbsPrivFwCtlrCloseDownRequest::ConstructL()"));
	}


void CT_LbsPrivFwCtlrCloseDownRequest::Start()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCtlrCloseDownRequest::Start()"));
	iNetPrivacy->NotifyLocationRequestL(*iRequestInfo, iRequestId);
	iState = EReqSubmitted;
	}


void CT_LbsPrivFwCtlrCloseDownRequest::ProcessPrivApiData(TInt /*aRequestId*/, TInt /*aCompletionCode*/)
	{
	// This should never happen during a notification
	ERR_PRINTF1(_L("CT_LbsPrivFwCtlrCloseDownRequest::ProcessPrivApiData() - Privacy API received unexpected callback during Notification request"));
	iStep->SetTestStepResult(EFail);
	}

void CT_LbsPrivFwCtlrCloseDownRequest::ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCtlrCloseDownRequest::ProcessPrivNotifierData()"));
	// Validate Parameters in Base class
	CT_LbsPrivFwBaseRequest::ProcessPrivNotifierData(aRequestId, aPrivNotifierData);
	if(iState != EReqSubmitted)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwCtlrCloseDownRequest::ProcessPrivRequestComplete() - Failed: Invalid state"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}
	iState = EReqReceived;
	// Send response
	// Warning, When request Ids match, use iRequestId instead of aRequestId
	iProxy->SendNotifierResponse(ERequestNotify, aRequestId, EResponseTypeAccepted, iResponseDelay);
	iState = EReqCompleted;
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
        TerminateSessionL(iRequestId);
        }
	}

void CT_LbsPrivFwCtlrCloseDownRequest::ProcessPrivRequestComplete(TInt aRequestId, TInt /*aCompletionCode*/)
	{
	if(iState != EReqCompleted)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwCtlrCloseDownRequest::ProcessPrivRequestComplete() - Failed: Invalid state"));
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

void CT_LbsPrivFwCtlrCloseDownRequest::ProcessPrivNotiferCtrlCloseDown()
	{
	if (iPrivacyHandler != EPrivacyHandleByController)
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwCtlrCloseDownRequest::ProcessPrivNotiferCtrlCloseDown() - Test Failed - Callback should not be invoked for this privacy handler"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		}
	else
		{
		iState = EReqCtrlShutDown;
		}
	}

TBool CT_LbsPrivFwCtlrCloseDownRequest::IsRequestComplete()
	{
	if (iPrivacyHandler == EPrivacyHandleByController)
		{
		// Controller has one extra state
		return ((iState == EReqCtrlShutDown) || (iState == EReqFailed));
		}
	else 
	    {
		return ((iState == EReqCompleted) || (iState == EReqFailed));
		}
	}
