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
// Privacy handler for Location Update requests
// 
//

#include <e32base.h>

#include <lbs/lbslocclasstypes.h>
#include "lbsdevloggermacros.h"

#include "privacyadvancednotifierhandler.h"
#include "EPos_CPosPrivManager.h"
#include "EPos_CPosDialogCtrl.h"
#include "EPos_PosCommonPrivacyResources.h"


// Special 'invalid session' SessionId.
const TLbsNetSessionIdInt KInvalidSessionId(TUid::Uid(0), 0);

//
// CPrivacyAdvancedRequest
//

CPrivacyAdvancedRequest* CPrivacyAdvancedRequest::NewL()
	{
	CPrivacyAdvancedRequest* self = new (ELeave) CPrivacyAdvancedRequest;
	return self;
	}

//
// CPrivacyAdvancedNotifierHandler
//
CPrivacyAdvancedNotifierHandler::CPrivacyAdvancedNotifierHandler(CLbsAdmin& aLbsAdmin,
												 RLbsNetworkRegistrationStatus& aNetRegStatus) : 
	CPrivacyHandler(aLbsAdmin, aNetRegStatus)
	{
	CActiveScheduler::Add(this);
	iHighestSessionId = KInvalidSessionId;
	}
    
CPrivacyAdvancedNotifierHandler* CPrivacyAdvancedNotifierHandler::NewL(CLbsAdmin& aLbsAdmin,
													   RLbsNetworkRegistrationStatus& aNetRegStatus)
	{
	CPrivacyAdvancedNotifierHandler* self = new (ELeave) CPrivacyAdvancedNotifierHandler(aLbsAdmin,
																		aNetRegStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
	}
    
/**
*/
void CPrivacyAdvancedNotifierHandler::ConstructL()
	{
	User::LeaveIfError(PosCommonPrivacyResources::Install());
	iPrivacyDialogController = CPosDialogCtrl::InstanceL();
    PosCommonPrivacyResources::SetDialogCtrl(iPrivacyDialogController);
	iPrivacyManager = CPosPrivManager::NewL();
	}

/**
*/
CPrivacyAdvancedNotifierHandler::~CPrivacyAdvancedNotifierHandler()
	{
	Cancel();
	iRequestBuffer.ResetAndDestroy();
	delete iPrivacyManager;
	delete iPrivacyDialogController;
	PosCommonPrivacyResources::Uninstall();
	}

/**
*/
void CPrivacyAdvancedNotifierHandler::RunL()
	{
	// Send response to current privacy request.
	SendPrivacyResponse();
	
	// If there is another request in the buffer, send it.
	SendNextPrivacyRequest();
	}

/**
*/
void CPrivacyAdvancedNotifierHandler::DoCancel()
	{
	// Cancel the verification dialog if it is
	// currently being displayed.
	if (iRequestBuffer.Count() > 0)
		{
		CPrivacyAdvancedRequest* requestData = iRequestBuffer[0];
		if (requestData->RequestPrivacy().RequestAdvice() 
			== TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify)
			{
			iPrivacyManager->CancelVerify();
			}
		
		// Remove the cancelled session.
		iRequestBuffer.Remove(0);
		delete requestData;
		}
	}

/**
*/
void CPrivacyAdvancedNotifierHandler::RegisterObserver(MPrivacyHandlerObserver* aObserver)
    {
    iObserver = aObserver;	
    }

/** Receive a privacy request from the network and pass on to the Q&N notifier.

This function receives the privacy request from the network and converts
it into a verify or notify request for the Privacy Q&N API.

Currently only one active request is supported. If a new request arrives
when the previous one is still running, then the previous request
will be cancelled before the new request is sent to the Q&N API.

*/
void CPrivacyAdvancedNotifierHandler::ProcessNetworkLocationRequest(TLbsNetSessionIdInt aSessionId, 
										const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt /*aSessionType*/,
                                         const TLbsExternalRequestInfo&  aRequestInfo, 
                       					const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
                       					TBool aIsEmergency)
           
	{
	// TODO:
	// 1) Convert the data types into the data types used by the CPosPrivManager
	// 2) Store the data in a single class, tied to the session Id?
	// 3) Issue a VerifyL or NotifyL as required
	// 3) Set this AO as 'active' and wait for RunL to be triggered.

	
	// Record the highest session Id so far.
	// We need this in case we need to check if a new request
	// is actually a repeat of an old request.
	if ((aSessionId.SessionNum() > iHighestSessionId.SessionNum()) &&
	    (aNetPosRequestPrivacy.RequestAdvice() == TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify))
		{
		iHighestSessionId = aSessionId;
		}

	// Add this new request to the buffer
	BufferPrivacyRequest(aSessionId, aRequestInfo, aNetPosRequestPrivacy, aIsEmergency);
	
	// If there is no current request outstanding, send it now
	if (!IsPrivacyRequestActive())
		{
		SendNextPrivacyRequest();
		}
	}

/**
*/	
void CPrivacyAdvancedNotifierHandler::ProcessNetworkPositionUpdate(TLbsNetSessionIdInt /*aSessionId*/, 
                                           const TPositionInfo& /*aPosInfo*/)
    {
    // Position updates are not supported by the Privacy Q&N API so ignore 
    // this position update.
    }


/**
*/  
void CPrivacyAdvancedNotifierHandler::ProcessRequestComplete(TLbsNetSessionIdInt aSessionId, 
															 TInt aReason)
	{
	// How we interpret this session complete depends on what
	// state the request for aSessionId is in, and the value
	// of aReason.
	//
	// Generally:
	// KErrNone - Should only happen after we've sent a reply.
	// KErrCancel - Generic cancel for a session, usually happens before 
	//				we have sent a reply (but not always).
	// KErrTimedOut - Cancel because of a network-side timeout. Usually
	//				  happens before we have sent a reply (but not always).
	// Other error code - Completely unexpected - at the moment these are
	//					  ignored and the session is ended without any other
	//					  action.
	TInt index = iRequestBuffer.Find(aSessionId, CPrivacyAdvancedRequest::IsSessionIdEqual);
	if (index >= 0)
		{
		CPrivacyAdvancedRequest* request(iRequestBuffer[index]);
		request->SetState(CPrivacyRequest::EPrivReqStateCompleted);
		
		if (index == 0)
			{
			// If the request is the currently active one then
			// we have to cancel the dialog.
			switch (aReason)
				{
				case KErrNone:
					{
					// Normal end of a session. Don't need to send anything more
					// to the Q&N API.
					break;
					}
				case KErrTimedOut:
					{
					if (request->RequestPrivacy().RequestAdvice() == TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify)
						{
						// Network-side timeout, tell the Q&N API about this.
						TRAP_IGNORE(iPrivacyManager->CancelVerifyL(EPosCancelReasonTimeout));
						}
					break;
					}
				case KErrCancel:
					{
					if (request->RequestPrivacy().RequestAdvice() == TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify)
						{
						// Generic network-side cancel, tell the Q&N API about this.
						iPrivacyManager->CancelVerify();
						}
					break;
					}
				default:
					{
					// Ignore any other error code. Don't send
					// anything else to the Q&N API.
					break;
					}
				}

			// Delete the cancelled request
			iRequestBuffer.Remove(index);
			delete request;
			}
		else
			{
			// If the request has not yet been sent to the dialog,
			// then what to do will depend on what type of privacy
			// request it is.
			//
			// Notification: Just remove it from the list.
			// Verification: If the timeout action is 'accept', turn it into
			//               a notification for the user. Else just remove.
			switch (request->RequestPrivacy().RequestAdvice())
				{
				case TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify:
					{
					// Delete the cancelled request
					iRequestBuffer.Remove(index);
					delete request;
					break;
					}
				case TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify:
					{
					if (request->RequestPrivacy().RequestAction() == TLbsNetPosRequestPrivacyInt::ERequestActionAllow)
						{
						// Change the request into a notification and move to after any pending verifications
						TLbsNetPosRequestPrivacyInt privacy(request->RequestPrivacy());
						privacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
						request->SetRequestPrivacy(privacy);
						
						TPosRequestData data(request->RequestData());
						data.iRequestDecision = EPosDecisionAccepted;
						data.iNotificationReason = EPosDecisionByRequestSource;
						request->SetRequestData(data);
						
						DeferNotification(index);
						}
					else
						{
						// Delete the cancelled request
						iRequestBuffer.Remove(index);
						delete request;
						}
					break;
					}
				default:
					{
					// Should never get here
					break;
					}
				}
			}
		}

	// If there is no current request outstanding, start the next one.
	if (!IsPrivacyRequestActive())
		{
		SendNextPrivacyRequest();
		}

	CPrivacyAdvancedRequest* activeRequest = NULL;
	if (iRequestBuffer.Count() > 0)
		{
		activeRequest = iRequestBuffer[0];	
		}
	
	if (activeRequest
		&& aSessionId == activeRequest->SessionId())
		{

		
		// Delete the cancelled request
		iRequestBuffer.Remove(0);
		delete activeRequest;
		
		// Start the next request in the buffer
		SendNextPrivacyRequest();
		}
	}
	
/**
*/
void CPrivacyAdvancedNotifierHandler::OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& /* aRequestId */, 
                            TLbsNetworkEnumInt::TLbsPrivacyResponseInt /* aRequestResult */,
                            TInt /*aResponseReason*/)
	{
	// Unused for advanced notifier
	}

/**
*/
void CPrivacyAdvancedNotifierHandler::OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& /*aRequestId*/)
	{
	// Unused for advanced notifier
	}

/**
*/	
void CPrivacyAdvancedNotifierHandler::SetServerObserver(MLbsSessionObserver* /*aNrhServer*/)
	{
	// Unused for advanced notifier
	}

TBool CPrivacyAdvancedNotifierHandler::IsPrivacyRequestActive()
	{
	return (IsActive());
	}

void CPrivacyAdvancedNotifierHandler::SendNextPrivacyRequest()
	{
	if (iRequestBuffer.Count() == 0)
		{
		// No request to actually send!
		return;
		}
	
	// Send a verify or notify request to the Q&N API depending on the
	// RequestAdvice().
	CPrivacyAdvancedRequest* activeRequest = iRequestBuffer[0];
	switch (activeRequest->RequestPrivacy().RequestAdvice())
		{
		case TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify:
			{
			SendNotificationRequest(*activeRequest);
			break;
			}
		case TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify:
			{    		
            SendVerificationRequest(*activeRequest);
			break;
			}
		case TLbsNetPosRequestPrivacyInt::ERequestAdviceSilent:
		    {
		    // The request was for no notification/no verification.
		    // These are handled as per the admin setting.
		    CLbsAdmin::TExternalLocateService externalLocate(CLbsAdmin::EExternalLocateOff);
		    GetExternalLocateAdminSetting(activeRequest->SessionType(), externalLocate);
		    
		    switch (externalLocate)
		        {
		        case CLbsAdmin::EExternalLocateOn:
		            {
		            // Accept the request with no notification/verification.
                    iObserver->OnRespondNetworkLocationRequest(
                          activeRequest->SessionId(), 
                          TLbsNetworkEnumInt::EPrivacyResponseAccepted,
                          KErrArgument);
		            break;
		            }
		        case CLbsAdmin::EExternalLocateOnButAlwaysVerify:
		            {
		            SendVerificationRequest(*activeRequest);
		            break;
		            }

		        case CLbsAdmin::EExternalLocateOffButNotify:
		            {
		            // Notify the user about the request.
		            SendNotificationRequest(*activeRequest);
	                // Reject the request.
                    iObserver->OnRespondNetworkLocationRequest(
                            activeRequest->SessionId(), 
                            TLbsNetworkEnumInt::EPrivacyResponseRejected,
                            KErrNotSupported);
                    break;
		            }
		        		        
		        case CLbsAdmin::EExternalLocateOff:
		        case CLbsAdmin::EExternalLocateUnknown:
		        default:
		            {
		            // This advice type is not supported.
		            // Our response is to reject such requests.
		            iObserver->OnRespondNetworkLocationRequest(
		                    activeRequest->SessionId(), 
		                    TLbsNetworkEnumInt::EPrivacyResponseRejected,
		                    KErrNotSupported);
		            break;
		            }
		        }
		    
		    break;
		    }
		case TLbsNetPosRequestPrivacyInt::ERequestAdviceStealth:
			{
			// This advice type is not supported. Our response
			// is to reject such requests.
			iObserver->OnRespondNetworkLocationRequest(
					activeRequest->SessionId(), 
					TLbsNetworkEnumInt::EPrivacyResponseRejected,
					KErrNotSupported);
			break;
			}
		case TLbsNetPosRequestPrivacyInt::ERequestAdviceNotUsed:
		default:
			{
			// These Advice types are not expected arguments. 
			// Our response is to reject them.
			iObserver->OnRespondNetworkLocationRequest(
					activeRequest->SessionId(), 
					TLbsNetworkEnumInt::EPrivacyResponseRejected,
					KErrArgument);
			break;
			}
		}
	}

/**
 * Issues the notification request.
 */  
void CPrivacyAdvancedNotifierHandler::SendNotificationRequest(CPrivacyAdvancedRequest& aActiveRequest)
    {
    TRAPD(err, iPrivacyManager->NotifyL(
            iStatus,
            aActiveRequest.RequestInfo(),
            aActiveRequest.RequestData(),
            aActiveRequest.SessionId().SessionNum()));
    if (KErrNone == err)
        {
        // Set AO active and wait for result.
        SetActive();
        }
    else
        {
        // Error starting the notifier; reject the privacy request.
        iObserver->OnRespondNetworkLocationRequest(
                aActiveRequest.SessionId(), 
                TLbsNetworkEnumInt::EPrivacyResponseRejected,
                KErrGeneral);
        }
    }

/**
 * Issues the verification request.
 */ 
void CPrivacyAdvancedNotifierHandler::SendVerificationRequest(CPrivacyAdvancedRequest& aActiveRequest)
    {
    TRAPD(err, iPrivacyManager->VerifyL(
            iStatus,
            aActiveRequest.RequestInfo(),
            aActiveRequest.RequestData(),
            aActiveRequest.SessionId().SessionNum()));
    if (KErrNone == err)
        {
        // Set AO active and wait for result.
        SetActive();
        }
    else
        {
        // Error starting the notifier; reject the privacy request.
        iObserver->OnRespondNetworkLocationRequest(
                aActiveRequest.SessionId(), 
                TLbsNetworkEnumInt::EPrivacyResponseRejected,
                KErrGeneral);
        }
    }

void CPrivacyAdvancedNotifierHandler::SendPrivacyResponse()
	{
	if (iRequestBuffer.Count() == 0)
		{
		return;
		}
	
	// Process the result from a notification or verification request.	
	CPrivacyAdvancedRequest* activeRequest = iRequestBuffer[0];
	if (activeRequest->State() != CPrivacyRequest::EPrivReqStateCompleted)
		{		
		switch (activeRequest->RequestPrivacy().RequestAdvice())
			{
			case TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify:
				{
				// Convert the response into the internal LBS enum
				TLbsNetworkEnumInt::TLbsPrivacyResponseInt response;
				if (iStatus.Int() == KErrNone)
					{
					response = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
					}
				else
					{
					response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
					}
			
				// Pass the verification response to the network.
				iObserver->OnRespondNetworkLocationRequest(
						activeRequest->SessionId(), response, KErrNone);
				break;
				}
			case TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify:
				{
				// Return the default response for notify requests
				TLbsNetworkEnumInt::TLbsPrivacyResponseInt response;
				if (activeRequest->RequestPrivacy().RequestAction() == TLbsNetPosRequestPrivacyInt::ERequestActionAllow)
					{
					response = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
					}
				else
					{
					response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
					}
				
				// For notification, simply return the default action.
				iObserver->OnRespondNetworkLocationRequest(
						activeRequest->SessionId(), response, KErrNone);
				break;
				}
			default:
				{
				// Other types of response are not expected, so
				// just ignore them???
				break;
				}
			}
		}
		
	// Delete the finished request
	iRequestBuffer.Remove(0);
	delete activeRequest;
	}

/*
 * Buffers requests. Verifications always take priority over notifications (ie are inserted in front of them)
 * If a notification is at the front of the queue (ie has already been sent) then we leave it there (it will complete almost instantly) 
 * 
 */
TInt CPrivacyAdvancedNotifierHandler::BufferPrivacyRequest(
		const TLbsNetSessionIdInt& aSessionId,
		const TLbsExternalRequestInfo& aRequestInfo,
		const TLbsNetPosRequestPrivacyInt& aRequestPrivacy,
		TBool aIsEmergency)
	{
	TBool isVerification = EFalse;
	// create the new request
	CPrivacyAdvancedRequest* request = NULL;
	TRAPD(err, request = CPrivacyAdvancedRequest::NewL());
	if (err == KErrNone)
		{
		request->SetSessionId(aSessionId);
		request->SetRequestInfo(aRequestInfo);
		request->SetRequestPrivacy(aRequestPrivacy);
		request->SetIsEmergency(aIsEmergency);
		request->SetStartTime();
		
		TPosRequestData data;
		ResetRequestData(data);
		switch (request->RequestPrivacy().RequestAdvice())
			{
			case TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify:
				{
				data.iRequestDecision = EPosDecisionAccepted;
				
				// If the session Id is less than the previous highest session Id
				// then assume that it is for a request that has already been processed.
				// (This should only happen if a client has called
				// NotifyVerificationTimeout() on the Network Privacy API.)
				// We should use a different notification reason for these
				// types of request.
				if (request->SessionId().SessionNum() <= iHighestSessionId.SessionNum())
					{
					data.iNotificationReason = EPosVerificationTimeout;
					data.iRequestDecision = (aRequestPrivacy.RequestAction() == TLbsNetPosRequestPrivacyInt::ERequestActionAllow) ? EPosDecisionAccepted : EPosDecisionRejected;
					}
				else
					{
					data.iNotificationReason = EPosDecisionByRequestSource;
					}
				break;
				}
			case TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify:
				{
				ConvertActionToTPosRequestDecision(
					request->RequestPrivacy().RequestAction(), 
    				data.iTimeoutStrategy);
				isVerification = ETrue;
				break;
				}
			default:
				{
				// Should never get here; ignore for now.
				break;
				}
			}
		request->SetRequestData(data);
		
		// add it to the buffer
		if(isVerification)  // verification gets inserted before pending notifications
		    {
		    err = InsertVerification(request);
		    }
		else  // notifications just get appended on the end
		    {
		    err = iRequestBuffer.Append(request);
		    }
		}
	
	return err;
	}

/*
 * If the notification at the given index is in front of any verifications 
 * then it is moved to immediately after the last verification.
 */
TInt CPrivacyAdvancedNotifierHandler::DeferNotification(TInt aIndex)
    {
    TInt err = KErrNone;
    TInt lastVerification = aIndex;
    
    for( TInt i = aIndex + 1; i < iRequestBuffer.Count(); i++ )
        {
        if(iRequestBuffer[i]->RequestPrivacy().RequestAdvice() == TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify)
            {
            lastVerification = i;
            }
        }
    
    // there are pending verifications after the notification, so move it back
    if(lastVerification > aIndex)
        {
        CPrivacyAdvancedRequest* request = iRequestBuffer[aIndex];
        if(lastVerification + 1 < iRequestBuffer.Count())
            {
            err = iRequestBuffer.Insert(request, lastVerification + 1);
            }
        else
            {
            err = iRequestBuffer.Append(request);
            }
        iRequestBuffer.Remove(aIndex);
        }
    
    return err;
    
    }

/*
 * Inserts a verification request into the request queue ahead of pending notifications
 * nb: the first request, which has already been issued, is left in place
 */
TInt CPrivacyAdvancedNotifierHandler::InsertVerification(CPrivacyAdvancedRequest* aRequest)
{
    TInt err;
    
    for( TInt i = 1; i < iRequestBuffer.Count(); i++ )
        {
        if ( iRequestBuffer[i]->RequestPrivacy().RequestAdvice() == TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify)
            {
            return iRequestBuffer.Insert(aRequest,i);
            }
        }
    
    // there were no notifications, just append on the end
    err = iRequestBuffer.Append(aRequest);

    return err;
}

void CPrivacyAdvancedNotifierHandler::RemovePrivacyRequestFromBuffer(
		const TLbsNetSessionIdInt& aSessionId)
	{
	TInt index = iRequestBuffer.Find(aSessionId, CPrivacyAdvancedRequest::IsSessionIdEqual);
	while (KErrNotFound != index)
		{
		CPrivacyRequest* reqData = iRequestBuffer[index];
		iRequestBuffer.Remove(index);
		delete reqData;
		
		index = iRequestBuffer.Find(aSessionId, CPrivacyAdvancedRequest::IsSessionIdEqual);
		}
	}

/** Reset the members of aRequestData to their default values.
*/
void CPrivacyAdvancedNotifierHandler::ResetRequestData(TPosRequestData& aRequestData)
	{	
    aRequestData.iRequestSource = EPosRequestSourceNetwork;
    aRequestData.iTimeoutStrategy = EPosDecisionNotAvailable;
    aRequestData.iRequestDecision = EPosDecisionNotAvailable;
    aRequestData.iNotificationReason = EPosNotificationReasonNotAvailable;
    aRequestData.iCancelReason = EPosCancelReasonNotAvailable;
	}

/**
*/
void CPrivacyAdvancedNotifierHandler::ConvertActionToTPosRequestDecision(
		TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt aAction,
    	TPosRequestDecision& aDecision)
	{	
	switch (aAction)
		{
		case TLbsNetPosRequestPrivacyInt::ERequestActionAllow:
			{
			aDecision = EPosDecisionAccepted;
			break;
			}
		case TLbsNetPosRequestPrivacyInt::ERequestActionReject:
			{
			aDecision = EPosDecisionRejected;
			break;
			}
		case TLbsNetPosRequestPrivacyInt::ERequestActionNotUsed:
		default:
			{
			aDecision = EPosDecisionNotAvailable;
			break;
			}
		}
	}
