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

#include "ctlbsprivfwcomplextimeoutrequest.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwComplexTimeoutRequest::~CT_LbsPrivFwComplexTimeoutRequest()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::~CT_LbsPrivFwComplexTimeoutRequest()"));
	}
	

CT_LbsPrivFwComplexTimeoutRequest::CT_LbsPrivFwComplexTimeoutRequest()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}


CT_LbsPrivFwComplexTimeoutRequest* CT_LbsPrivFwComplexTimeoutRequest::NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwComplexTimeoutRequest* me = new(ELeave)CT_LbsPrivFwComplexTimeoutRequest();
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwComplexTimeoutRequest::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	CT_LbsPrivFwBaseRequest::ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	INFO_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::ConstructL()"));

	iResponseDelay = 0;
	iNetworkTimeout = 30; //30s
	}


void CT_LbsPrivFwComplexTimeoutRequest::Start()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::Start()"));
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



void CT_LbsPrivFwComplexTimeoutRequest::SetNetworkTimeout(TUint aNetworkTimeout)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::SetNetworkTimeout()"));
	iNetworkTimeout = aNetworkTimeout;
	}

void CT_LbsPrivFwComplexTimeoutRequest::SetDecision(CPosNetworkPrivacy::TRequestDecision aDecision)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::SetDecision()"));
	iDecision = aDecision;
	}

void CT_LbsPrivFwComplexTimeoutRequest::SetTimeoutDecision(CPosNetworkPrivacy::TRequestDecision aTimeoutDecision)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::SetDecision()"));
	iTimeoutDecision = aTimeoutDecision;
	}

void CT_LbsPrivFwComplexTimeoutRequest::SetDesiredResponse(TResponseType aResponseType)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::SetDesiredResponse()"));
	iDesiredResponse = aResponseType;
	}

void CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivApiData()"));
	// Validate
	CT_LbsPrivFwBaseRequest::ProcessPrivApiData(aRequestId, aCompletionCode);

	// Perform usual validation here
	if((aCompletionCode == KErrAccessDenied) && (iDesiredResponse == EResponseTypeRejected))
		{
		if(iState != EReqRejected)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
			iStep->SetTestStepResult(EFail);
			iState = EReqFailed;
		    if(iStep->NeedSessionTerminate())
		        {			
                INFO_PRINTF1(_L("Terminating Session"));
                TerminateSessionL(aRequestId);
		        }
			return;
			}
		}
	else if((aCompletionCode == KErrNone) && (iDesiredResponse == EResponseTypeAccepted))
		{
		if(iState != EReqAccepted)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
			iStep->SetTestStepResult(EFail);
			iState = EReqFailed;
		    if(iStep->NeedSessionTerminate())
		        {			
                INFO_PRINTF1(_L("Terminating Session"));
                TerminateSessionL(aRequestId);
		        }
			return;
			}
		}
	else
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivApiData() - Test Failed - Actual completion code does not match desired completion code"));
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
        if(iStep->NeedSessionTerminate())
            {
            INFO_PRINTF1(_L("Terminating Session"));
            TerminateSessionL(aRequestId);
            }
		return;
		}
	
	
	if(iDecision == iTimeoutDecision)
		{
		// If none of the tests above failed  then the response is correct
		// If reponses do not differ, test is over
		if(iPrivacyHandler != EPrivacyHandleByController)
			{
			iState = EReqValidated;
		    if(iStep->NeedSessionTerminate())
		        {			
                INFO_PRINTF1(_L("Terminating Session"));
                TerminateSessionL(aRequestId);
		        }
			}
		else
			{
			iState = EReqTempValidated;
			}
		}
	else
		{
		// If timeout and reponse differ, do not shut down
		iState = EReqTempValidated;
		// Instead, perform a timeout
		iNetPrivacy->NotifyVerificationTimeoutL(*iRequestInfo, iRequestId, iTimeoutDecision);
		}
	}


void CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivNotifierData()"));
	
	// Hasn't been timed out yet, respond
	if(iState == EReqSubmitted)
		{
		// Validate Parameters in Base class
		CT_LbsPrivFwBaseRequest::ProcessPrivNotifierData(aRequestId, aPrivNotifierData);
		iProxy->SendNotifierResponse(ERequestVerify, aRequestId, iDesiredResponse, iResponseDelay);
		if(iDesiredResponse == EResponseTypeAccepted)
			{
			iState = EReqAccepted;
			}
		else if (iDesiredResponse == EResponseTypeRejected)
			{
			iState = EReqRejected;
			}
		}
	// We've been timed out, the responses must differ - check that and repond
	else if((iState == EReqTempValidated) && (iDecision != iTimeoutDecision))
		{
		// If qnn Respond here
		switch(iPrivacyHandler)
			{
			case EPrivacyHandleByQnN:
				{
				iProxy->SendNotifierResponse(ERequestNotify, aRequestId, EResponseTypeAccepted, iResponseDelay);
				iState = EReqValidated;
			    if(iStep->NeedSessionTerminate())
			        {				
                    INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
                    TerminateSessionL(iRequestId);
			        }
				break;
				}
			case EPrivacyHandleByNotifier:
				{
				//iState = EReqTempNotifierValidated; //TBC::::BRAJ
				iState = EReqValidated;
			    if(iStep->NeedSessionTerminate())
			        {				
                    INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
                    TerminateSessionL(iRequestId);
			        }
				break;
				}
			case EPrivacyHandleByController:
				{
				iState = EReqCtrlValidated;
				break;
				}
			}
		}
	else if(iState == EReqTempCtrlValidated)
		{
		iState = EReqFinalCtrlValidated;
		}
	else if (iState == EReqTempNotifierValidated)
		{
		iState = EReqValidated;
	    if(iStep->NeedSessionTerminate())
	        {		
            INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
            TerminateSessionL(iRequestId);
	        }
		}
	else if(iState == EReqAccepted || iState == EReqRejected)
		{
		//Do nothing
		}
	// Neither of our options applied. Something has gone wrong
	else
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivNotifierData() - Test Failed - Invalid state"));
		iStep->SetTestStepResult(EFail);
		iState = EReqTestFailed;
	    if(iStep->NeedSessionTerminate())
	        {		
            INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
            TerminateSessionL(iRequestId);
	        }
		}	
	}


void CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode)
	{
	if((iPrivacyHandler == EPrivacyHandleByController) 
			&& (iState == EReqTempValidated)
			&& (aCompletionCode == KErrNone))
		{
		if(iDecision != iTimeoutDecision)
			{
			iState = EReqTempCtrlValidated;
			}
		else
			{
			iState = EReqCtrlFinal;
		    if(iStep->NeedSessionTerminate())
		        {			
                INFO_PRINTF1(_L("Terminating Session"));
                TerminateSessionL(aRequestId);
		        }
			}
		
		}
	else if ((iPrivacyHandler == EPrivacyHandleByController) 
			&& (iState == EReqFinalCtrlValidated)
			&& (aCompletionCode == KErrNone))
		{
		iState = EReqCtrlFinal;
	    if(iStep->NeedSessionTerminate())
	        {		
            INFO_PRINTF1(_L("Terminating Session"));
            TerminateSessionL(aRequestId);
	        }
		}
	else	
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwComplexTimeoutRequest::ProcessPrivRequestComplete() - Test Failed"));
		iStep->SetTestStepResult(EFail);
		iState = EReqTestFailed;
	    if(iStep->NeedSessionTerminate())
	        {		
            INFO_PRINTF1(_L("Terminating Session"));
            TerminateSessionL(aRequestId);
	        }
		return;
		}
	}

TBool CT_LbsPrivFwComplexTimeoutRequest::IsRequestComplete()
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
