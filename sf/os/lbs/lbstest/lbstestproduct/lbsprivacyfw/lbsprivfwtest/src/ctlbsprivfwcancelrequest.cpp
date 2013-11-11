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

#include "ctlbsprivfwcancelrequest.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwCancelRequest::~CT_LbsPrivFwCancelRequest()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::~CT_LbsPrivFwCancelRequest()"));
	}
	

CT_LbsPrivFwCancelRequest::CT_LbsPrivFwCancelRequest()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}

CT_LbsPrivFwCancelRequest* CT_LbsPrivFwCancelRequest::NewL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwCancelRequest* me = new(ELeave)CT_LbsPrivFwCancelRequest();
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwCancelRequest::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	CT_LbsPrivFwBaseRequest::ConstructL(aProxy, aNetPrivacy, aPrivacyObserver, aPrivacyHandler, aStep);
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::ConstructL()"));

	iCancelBehaviour = EReqCancelNone;
	//iNetworkTimeout = KDefaultNetworkPrivacyTimeout; //30s
	iDecision = CPosNetworkPrivacy::EDecisionNotAvailable;
	iCancelReason = CPosNetworkPrivacy::ECancelReasonNotAvailable;
	}

void CT_LbsPrivFwCancelRequest::SetRequestType(TRequestType aRequestType)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::SetRequestType()"));
	iRequestType = aRequestType;
	}

void CT_LbsPrivFwCancelRequest::SetCancelBehaviour(TReqCancelBehaviour aCancelBehaviour)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::SetCancelBehaviour()"));
	iCancelBehaviour = aCancelBehaviour;
	}

void CT_LbsPrivFwCancelRequest::SetCancelReason(CPosNetworkPrivacy::TCancelReason aCancelReason)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::SetCancelReason()"));
	iCancelReason = aCancelReason;
	}

void CT_LbsPrivFwCancelRequest::SetDecision(CPosNetworkPrivacy::TRequestDecision aDecision)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::SetDecision()"));
	iDecision = aDecision;
	}

void CT_LbsPrivFwCancelRequest::SetDesiredResponse(TResponseType aResponseType)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::SetDesiredResponse()"));
	iDesiredResponse = aResponseType;
	}


void CT_LbsPrivFwCancelRequest::Start()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::Start()"));
	//Set Connection Timeout value
	//iNetPrivacy->SetConnectionTimeout(iNetworkTimeout);
	
	if(iRequestType == ERequestVerify)
		{
		iNetPrivacy->VerifyLocationRequestL(*iRequestInfo, iRequestId, *iApiObserver, iDecision);
		}
	else if(iRequestType == ERequestNotify)
		{
		iNetPrivacy->NotifyLocationRequestL(*iRequestInfo, iRequestId);
		}
	else
		{
		ASSERT(FALSE);
		}

	iState = EReqSubmitted;

	//This might not be correct behaviour as desired - Previous request should be still waiting 
	if(iCancelBehaviour == EReqCancelBeforeDialog /*&& iRequestType == ERequestVerifyCancel*/)
		{
		iNetPrivacy->CancelVerifyLocationRequest(iRequestId, iCancelReason);
		iRequestType = ERequestNotify;
		}
	}

void CT_LbsPrivFwCancelRequest::ProcessPrivApiData(TInt aRequestId, TInt aCompletionCode)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::ProcessPrivApiData()"));

	//This function should not be called when request is cancelled - We might find a better way to do it...:-(
	if(iCancelBehaviour == EReqCancelAfterDialog || iCancelBehaviour == EReqCancelBeforeDialog) 
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwCancelRequest::ProcessPrivApiData() - Test Failed - Function shouldn't be called"));
		iState = EReqFailed;
		iStep->SetTestStepResult(EFail);
		return;
		}
	
	if(iCancelBehaviour == EReqCancelAfterComplete)
		{
		iNetPrivacy->CancelVerifyLocationRequest(aRequestId, iCancelReason); //Note the use of aRequestId (Thought aRequestId should be same as iRequestId)
		}
		
	// Validate
	CT_LbsPrivFwBaseRequest::ProcessPrivApiData(aRequestId, aCompletionCode);
	// Not much here, validation mostly done in base
	if((aCompletionCode == KErrAccessDenied) && (iDesiredResponse == EResponseTypeRejected))
		{
		if(iState != EReqRejected)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwCancelRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
			iStep->SetTestStepResult(EFail);
			}
		iState = EReqValidated;
		}
	else if((aCompletionCode == KErrNone) && (iDesiredResponse == EResponseTypeAccepted))
		{
		if(iState != EReqAccepted)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwCancelRequest::ProcessPrivApiData() - Test Failed - Invalid state"));
			iStep->SetTestStepResult(EFail);
			}
		iState = EReqValidated;
		}
	else
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwCancelRequest::ProcessPrivApiData() - Test Failed - Actual completion code does not match desired completion code"));
		iStep->SetTestStepResult(EFail);
		}
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);	
        }
	}

void CT_LbsPrivFwCancelRequest::ProcessPrivNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwCancelRequest::ProcessPrivNotifierData()"));
	
	//if(!(iState == EReqCancelled && iPrivacyHandler == EPrivacyHandleByNotifier))
	if(iState == EReqSubmitted)
		{
		// Validate Parameters in Base class
		CT_LbsPrivFwBaseRequest::ProcessPrivNotifierData(aRequestId, aPrivNotifierData);
		}

	//if(!(iState == EReqSubmitted || iState == EReqCancelled))
	//	{
	//	ERR_PRINTF1(_L("CT_LbsPrivFwCancelRequest::ProcessPrivNotifierData() - Test Failed - Invalid state"));
	//	iStep->SetTestStepResult(EFail);
	//	}

	if(iState == EReqSubmitted)
		{
		//Do cancel based on scenario
		if(iCancelBehaviour == EReqCancelAfterDialog)
			{
			iNetPrivacy->CancelVerifyLocationRequest(iRequestId, iCancelReason);
			if(iPrivacyHandler != EPrivacyHandleByController)
				{
				iState = EReqCancelled;	
				}
			else
				{
				iState = EReqValidated;
			    if(iStep->NeedSessionTerminate())
			        {				
                    INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
                    TerminateSessionL(iRequestId);
			        }
				}
			}
		else if(iCancelBehaviour == EReqCancelNotifyAfterDialog)
			{
			iNetPrivacy->CancelVerifyLocationRequest(iRequestId, iCancelReason);
			}
		else if(iCancelBehaviour == EReqCancelInvalidRequestId)
			{
			iNetPrivacy->CancelVerifyLocationRequest(iRequestId+10, iCancelReason); //May be we can find a better way of doing it
			}
		if(iCancelBehaviour == EReqCancelBeforeDialog) //We do not have any way to find if request is Notification request - TBD
			{
			iState = EReqValidated;
		    if(iStep->NeedSessionTerminate())
		        {			
                INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
                TerminateSessionL(iRequestId);	
		        }
			}

		//SEND RESPONSE TO THE NOTIFIER		
		TTimeIntervalMicroSeconds delay(iResponseDelay);
		// Send response
		// Warning, When request Ids match, use iRequestId instead of aRequestId
		iProxy->SendNotifierResponse(iRequestType, aRequestId, iDesiredResponse, delay);

		if(iState == EReqSubmitted)
			{
			if(iRequestType == ERequestVerify)
				{
				if(iDesiredResponse == EResponseTypeAccepted)
					{
					iState = EReqAccepted;
					}
				else if (iDesiredResponse == EResponseTypeRejected)
					{
					iState = EReqRejected;
					}
				}
			else
				{
				iState = EReqValidated;
			    if(iStep->NeedSessionTerminate())
			        {				
                    INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
                    TerminateSessionL(iRequestId);
			        }
				}
			}
		}
		
	else if(iState == EReqCancelled)
		{
		//Check if Notfier received Cancel information with Proper Cancel Reason
		//Note: Notifier always sends cancel reason EPosCancelReasonNotAvailable
		if(iPrivacyHandler != EPrivacyHandleByNotifier) // ---TBD
			{
			if(aPrivNotifierData.VerifyCancelReason() != iCancelReason)
				{
				ERR_PRINTF1(_L("CT_LbsPrivFwCancelRequest::ProcessPrivNotifierData() - Test Failed - CancelReason does not match"));
				iStep->SetTestStepResult(EFail);
				iState = EReqFailed;
				}
			}
		iState = EReqValidated;	
	    if(iStep->NeedSessionTerminate())
	        {		
            INFO_PRINTF2(_L("Terminating Session with iRequest id 0x%x"),iRequestId);
            TerminateSessionL(iRequestId);
	        }
		}
	}

void CT_LbsPrivFwCancelRequest::ProcessPrivRequestComplete(TInt aRequestId, TInt aCompletionCode)
	{
	//We have to check completion code (based on Cancel behaviour)
	TBool		error = EFalse;
	TBuf<256> 	errorMessage;
	switch(iCancelBehaviour)
		{
		case EReqCancelInvalidRequestId:
		case EReqCancelNotifyAfterDialog:
		case EReqCancelAfterComplete:
			{
			if(!(aCompletionCode == KErrNone || aCompletionCode == KErrCancel))
				{
				error = ETrue;
				errorMessage.Format(_L("CT_LbsPrivFwCancelRequest::ProcessPrivRequestComplete() - Completion code should be KErrNone or KErrCancel, not %d - Test Failed"), aCompletionCode);
				}
			}
			break;
			
//		case EReqCancelBeforeDialog: - Its difficult to find when request actually got cancelled
		case EReqCancelAfterDialog:
			{
			if( iCancelReason == CPosNetworkPrivacy::ECancelReasonTimeout )
				{
				if(aCompletionCode != KErrTimedOut)	
					{
					error = ETrue;
					errorMessage.Format(_L("CT_LbsPrivFwCancelRequest::ProcessPrivRequestComplete() - Completion code should be KErrTimedOut - Test Failed"));
					}
				} 
			else
				{
				if(aCompletionCode != KErrCancel) 
					{
					error = ETrue;
					errorMessage.Format(_L("CT_LbsPrivFwCancelRequest::ProcessPrivRequestComplete() - Completion code should be KErrCancel - Test Failed"));
					}
				}
			}
			break;
			
		default:
			break;
		}
	
	if (iPrivacyHandler != EPrivacyHandleByController)
		{
		error = ETrue;
		errorMessage.Format(_L("CT_LbsPrivFwCancelRequest::ProcessPrivRequestComplete() - Test Failed - Callback should not be invoked for this privacy handler"));
		}
		
	else if(iState != EReqValidated)
		{
		error = ETrue;
		errorMessage.Format(_L("CT_LbsPrivFwCancelRequest::ProcessPrivRequestComplete() - Test Failed - Invalid state"));
		}

	if(error)
		{
		ERR_PRINTF1(errorMessage);
		ERR_PRINTF2(_L("CT_LbsPrivFwCancelRequest::iState = %d"),iState);
		iStep->SetTestStepResult(EFail);
		iState = EReqFailed;
		return;
		}
		
	iState = EReqCtrlFinal;
    if(iStep->NeedSessionTerminate())
        {	
        INFO_PRINTF1(_L("Terminating Session"));
        TerminateSessionL(aRequestId);
        }
	}

TBool CT_LbsPrivFwCancelRequest::IsRequestComplete()
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
	
