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
 @file ctlbsprivfwvnotrequestwithnonotifier.cpp
*/

#include "ctlbsprivfwnotrequestwithnonotifier.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwNotRequestWithNoNotifier::~CT_LbsPrivFwNotRequestWithNoNotifier()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwNotRequestWithNoNotifier::~CT_LbsPrivFwNotRequestWithNoNotifier()"));
	}
	

CT_LbsPrivFwNotRequestWithNoNotifier::CT_LbsPrivFwNotRequestWithNoNotifier()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}


CT_LbsPrivFwNotRequestWithNoNotifier* CT_LbsPrivFwNotRequestWithNoNotifier::NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwNotRequestWithNoNotifier* me = new(ELeave)CT_LbsPrivFwNotRequestWithNoNotifier();
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwNotRequestWithNoNotifier::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	CT_LbsPrivFwBaseRequest::ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	INFO_PRINTF1(_L("CT_LbsPrivFwNotRequestWithNoNotifier::ConstructL()"));
	}


void CT_LbsPrivFwNotRequestWithNoNotifier::Start()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwNotRequestWithNoNotifier::Start()"));
	iNetPrivacy->NotifyLocationRequestL(*iRequestInfo, iRequestId);
	iState = EReqSubmitted;
	}


void CT_LbsPrivFwNotRequestWithNoNotifier::ProcessPrivApiData(TInt aRequestId, TInt /*aCompletionCode*/)
	{
	// This should never happen during a notification
	ERR_PRINTF1(_L("CT_LbsPrivFwNotRequestWithNoNotifier::ProcessPrivApiData() - Privacy API received unexpected callback during Notification request"));
	iStep->SetTestStepResult(EFail);
	iState = EReqFailed;
	
	if(iStep->NeedSessionTerminate())
	    {
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);
	    }
	}

void CT_LbsPrivFwNotRequestWithNoNotifier::ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
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

void CT_LbsPrivFwNotRequestWithNoNotifier::ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode)
	{
	(void)aRequestId;
	(void)aCompletionCode;
	
	// This should never happen during a notification with no notifier available
	ERR_PRINTF1(_L("CT_LbsPrivFwNotRequestWithNoNotifier::ProcessPrivRequestComplete() - Notifier received unexpected callback during Notification request"));
	iStep->SetTestStepResult(EFail);
	iState = EReqFailed;
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);
        }
	}

TBool CT_LbsPrivFwNotRequestWithNoNotifier::IsRequestComplete()
	{
	return ((iState == EReqSubmitted) || (iState == EReqFailed));
	
	}
