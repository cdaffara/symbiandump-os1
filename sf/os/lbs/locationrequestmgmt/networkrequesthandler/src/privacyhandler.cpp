// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <lbs/lbsadmin.h>

#include "lbsdevloggermacros.h"
#include <lbs/lbslocclasstypes.h>
#include "lbsprivacynotifier.h"
#include "privacyhandlerobserver.h"
#include "privacyhandler.h"
#include "lbsprivacycontrollerdata.h"
#include "LbsInternalInterface.h"
#include "lbsqualityprofile.h"
#ifdef SYMBIAN_LOCATION_PRIVACY_V2
	#include "privacyadvancednotifierhandler.h"
#endif


const TInt KErrLbsNotifierUnknown = KErrUnknown; // for now....

//===================================================================================================
CPrivacyHandler* CPrivacyHandler::CreateL(MPrivacyHandlerObserver* aObserver,
                                          CLbsAdmin& aLbsAdmin,
                                          RLbsNetworkRegistrationStatus& aNetRegStatus)
	{
	CPrivacyHandler* implementation = 0;

	
	// Find out whether we're referring notification/verification to a notifier 
	// or a controller.
	CLbsAdmin::TPrivacyHandler privacyHandler;
	aLbsAdmin.Get(KLbsSettingPrivacyHandler, privacyHandler);

	switch(privacyHandler)
		{
		case CLbsAdmin::EPrivacyHandleByNotifier:
			{
			implementation = CPrivacyNotifierHandler::NewL(aLbsAdmin,
														   aNetRegStatus);
			break;
			}
		case CLbsAdmin::EPrivacyHandleByController:
			{
			implementation = CPrivacyControllerHandler::NewL(aLbsAdmin,
															 aNetRegStatus);
			break;
			}
#ifdef SYMBIAN_LOCATION_PRIVACY_V2
		case CLbsAdmin::EPrivacyHandleByAdvancedNotifier:
			{
			implementation = CPrivacyAdvancedNotifierHandler::NewL(aLbsAdmin,
																   aNetRegStatus);
			break;
			}
#endif
		default:
			{
			User::Leave(KErrLbsNotifierUnknown);
			break;
			}
		}
	implementation->RegisterObserver(aObserver);    
	return(implementation);
	}

CPrivacyHandler::CPrivacyHandler(CLbsAdmin& aLbsAdmin,
								 RLbsNetworkRegistrationStatus& aNetRegStatus)
: CActive(EPriorityStandard),
	iLbsAdmin(aLbsAdmin),
	iNetRegStatus(aNetRegStatus)
	{		
	}

/* Get the current value for the admin setting for external locate.

This functions checks the network registration (i.e. 'roaming') status
and reads either the 'home' or 'network' admin setting as appropriate. 
*/
void CPrivacyHandler::GetExternalLocateAdminSetting(const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, CLbsAdmin::TExternalLocateService& aExternalLocateService)
	{
	CLbsAdmin::TExternalLocateService								serviceStatus(CLbsAdmin::EExternalLocateOff);
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus	netRegStatus(RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown);	

	// Rread the network regisration to determine if currently roaming.
	TInt err = iNetRegStatus.GetNetworkRegistrationStatus(netRegStatus);
	if (err == KErrNone)
		{
		// Determine service status based on the normal MT-LR admin settings.
		if (aSessionType == TLbsNetworkEnumInt::EServiceMobileTerminated)
			{
			switch (netRegStatus)
				{
				case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
					{
					err = iLbsAdmin.Get(KLbsSettingHomeExternalLocate, serviceStatus);
					break;
					}
				case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
				case RLbsNetworkRegistrationStatus::ENotRegistered:
					{
					err = iLbsAdmin.Get(KLbsSettingRoamingExternalLocate, serviceStatus);
					break;
					}		
				case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
				default:
					{
					LBSLOG_WARN2(ELogP4, "Unrecognised TLbsNetworkRegistrationStatus (%d), defaulting to EExternalLocateOff", netRegStatus);
					break;
					}
				}
			}
			
		// Determine service status based on the network induced MT-LR admin settings.
		else if (aSessionType == TLbsNetworkEnumInt::EServiceNetworkInduced)
			{
			switch (netRegStatus)
				{
				case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
					{
					err = iLbsAdmin.Get(KLbsSettingHomeNetworkInducedLocate, serviceStatus);
					break;
					}
				case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
				case RLbsNetworkRegistrationStatus::ENotRegistered:
					{
					err = iLbsAdmin.Get(KLbsSettingRoamingNetworkInducedLocate, serviceStatus);
					break;
					}		
				case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
				default:
					{
					LBSLOG_WARN2(ELogP4, "Unrecognised TLbsNetworkRegistrationStatus (%d), defaulting to EExternalLocateOff", netRegStatus);
					break;
					}
				}
			}

		else
			{
			LBSLOG_WARN2(ELogP4, "Unsupported TLbsNetworkEnumInt::TLbsNetProtocolServiceInt (%d), defaulting to EExternalLocateOff", aSessionType);
			}
		}

	else
		{
		LBSLOG_WARN2(ELogP4, "Failed to get TExternalLocateService, couldn't read roaming status (err %d), defaulting to EExternalLocateOff", err);		
		}

	// Error during admin get. 
	if (err)
		{
		serviceStatus = CLbsAdmin::EExternalLocateOff;
		}
	
	aExternalLocateService = serviceStatus;
	}

/** Get the default privacy response based on the admin setting.
 */
void CPrivacyHandler::GetPrivacyTimeoutAction(
		TLbsNetworkEnumInt::TLbsPrivacyResponseInt& aResponse, 
		const TLbsNetPosRequestPrivacyInt& aRequestPrivacy)
	{
	CLbsAdmin::TPrivacyTimeoutAction privacyTimeoutAction;
	iLbsAdmin.Get(KLbsSettingPrivacyTimeoutAction, privacyTimeoutAction);
	
	switch(privacyTimeoutAction)
		{
		case CLbsAdmin::EPrivacyTimeoutReject:
			{
			aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			break;
			}
		
		case CLbsAdmin::EPrivacyTimeoutNetworkDefined:
			{
			//Use the network defined action to decide whether to reject/accept
			switch(aRequestPrivacy.RequestAction())
				{
				case TLbsNetPosRequestPrivacyInt::ERequestActionAllow:
					{
					aResponse = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
					break;
					}
				case TLbsNetPosRequestPrivacyInt::ERequestActionReject:
				case TLbsNetPosRequestPrivacyInt::ERequestActionNotUsed:
					{
					aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
					break;
					}
				}
			break;
			}
		default:
			{
			aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			break;
			}
		}	
	}

//
// CPrivacyRequest
//
CPrivacyRequest::CPrivacyRequest()
	{
	}

CPrivacyRequest::~CPrivacyRequest()
	{
	}

CPrivacyRequest* CPrivacyRequest::NewL()
	{
	CPrivacyRequest* self = new (ELeave) CPrivacyRequest;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CPrivacyRequest::ConstructL()
	{
	}

CPrivacyRequest::TPrivReqState CPrivacyRequest::State() const
	{
	return iState;
	}

void CPrivacyRequest::SetState(CPrivacyRequest::TPrivReqState aState)
	{
	iState = aState;
	}

const TLbsNetSessionIdInt& CPrivacyRequest::SessionId() const
	{
	return iSessionId;
	}

void CPrivacyRequest::SetSessionId(const TLbsNetSessionIdInt& aSessionId)
	{
	iSessionId = aSessionId;
	}

const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt& CPrivacyRequest::SessionType() const
	{
	return iSessionType;
	}

void CPrivacyRequest::SetSessionType(const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt& aSessionType)
	{
	iSessionType = aSessionType;
	}

const TLbsExternalRequestInfo& CPrivacyRequest::RequestInfo() const
	{
	return iRequestInfo;
	}

void CPrivacyRequest::SetRequestInfo(const TLbsExternalRequestInfo& aRequestInfo)
	{
	Mem::Copy(&iRequestInfo, &aRequestInfo, aRequestInfo.ClassSize());
	}

const TLbsNetPosRequestPrivacyInt& CPrivacyRequest::RequestPrivacy() const
	{
	return iRequestPrivacy;
	}

void CPrivacyRequest::SetRequestPrivacy(const TLbsNetPosRequestPrivacyInt& aRequestPrivacy)
	{
	iRequestPrivacy = aRequestPrivacy;
	}

TBool CPrivacyRequest::IsEmergency() const
	{
	return iIsEmergency;
	}

void CPrivacyRequest::SetIsEmergency(TBool aIsEmergency)
	{
	iIsEmergency = aIsEmergency;
	}

TTime CPrivacyRequest::StartTime() const
	{
	return iStartTime;
	}

void CPrivacyRequest::SetStartTime()
	{
	iStartTime.UniversalTime();
	}

/** Comparison function for RPointerArray<CPrivacyRequest>::Find()
 */
TBool CPrivacyRequest::IsSessionIdEqual(
		const TLbsNetSessionIdInt* aSessionId,
		const CPrivacyRequest& aItem)
	{
	return (*aSessionId == aItem.SessionId());
	}
	
//
// CPrivacyNotifierHandler
//
CPrivacyNotifierHandler::CPrivacyNotifierHandler(CLbsAdmin& aLbsAdmin,
												 RLbsNetworkRegistrationStatus& aNetRegStatus) : 
	CPrivacyHandler(aLbsAdmin, aNetRegStatus)
    {
	iPrivacyResponseOutstanding = EFalse;
    }
    
CPrivacyNotifierHandler* CPrivacyNotifierHandler::NewL(CLbsAdmin& aLbsAdmin,
													   RLbsNetworkRegistrationStatus& aNetRegStatus)
    {
	CPrivacyNotifierHandler* self = new (ELeave)CPrivacyNotifierHandler(aLbsAdmin,
																		aNetRegStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
    }

/**
*/
void CPrivacyNotifierHandler::ConstructL()
	{
	CActiveScheduler::Add(this);
	iPrivacyResponseTimer = CLbsCallbackTimer::NewL(*this);
	}

/**
 * Called when the response timer expires.
 * An accept or reject of the original query is reported to the NRH 
 * depending on the request action parameter received with the original request.
 * @param aTimerId always 1 (we only use one timer in this subcomponent)
 */
void CPrivacyNotifierHandler::OnTimerEventL(TInt aTimerId)
	{
	iNotifier->Cancel();
	switch (aTimerId)
		{
		case 1:
			{
			// The privacy notifier has timed out without a response being
			// received. Generate a response as required depending upon
			// the LbsAdmin setting: TPrivacyTimeoutAction
			CPrivacyRequest* request = iRequestBuffer[0];
			TLbsNetworkEnumInt::TLbsPrivacyResponseInt response;
			GetPrivacyTimeoutAction(response, request->RequestPrivacy());
			
			iObserver->OnRespondNetworkLocationRequest(
					request->SessionId(), response, KErrNone);

			// Delete the current request and start the next one in the buffer
			iRequestBuffer.Remove(0);
			delete request; 
          	SendNextPrivacyRequest();
			break;
			}
		default:
			// do nothing
			break;	
		}
	}
	
TInt CPrivacyNotifierHandler::OnTimerError(TInt /*aTimerId*/, 
											TInt aError)
	{
	// Not used - OnTimerEventL doesn't generate any error.
	return(aError);
	}


void CPrivacyNotifierHandler::ProcessNetworkLocationRequest(
		TLbsNetSessionIdInt aSessionId,
		const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType, 		
        const TLbsExternalRequestInfo&  aRequestInfo, 
        const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
        TBool aIsEmergency)
           
	{
	BufferPrivacyRequest(aSessionId, aSessionType, aRequestInfo, aNetPosRequestPrivacy, aIsEmergency);
	
	if (!IsPrivacyRequestActive())
		{
		SendNextPrivacyRequest();
		}
	}
void CPrivacyNotifierHandler::ProcessNetworkPositionUpdate(TLbsNetSessionIdInt /*aSessionId*/, 
                                           const TPositionInfo& /*aPosInfo*/)
    {
    }
		
void CPrivacyNotifierHandler::ProcessRequestComplete(TLbsNetSessionIdInt aSessionId, TInt /*aReason*/)
    {
    // If the current request is still in the buffer here, 
    // it is because it is still waiting for a reponse.
    // So, we need to cancel the notifier and clean up.
    if (iRequestBuffer.Count() > 0
    	&& aSessionId == iRequestBuffer[0]->SessionId())
    	{
    	CPrivacyRequest* request = iRequestBuffer[0];
    	
    	// If the completed request is the currently active one,
    	// may need to cancel the notifer and timer as well.
		if (iNotifier)
			{
			iNotifier->Cancel();
			}
		
		iPrivacyResponseTimer->Cancel();
		
		// Delete the completed request
		iRequestBuffer.Remove(0);
		delete request; 
    	}
    }

void CPrivacyNotifierHandler::RegisterObserver(MPrivacyHandlerObserver* aObserver)
    {
    iObserver = aObserver;	
    }
    
CPrivacyNotifierHandler::~CPrivacyNotifierHandler()
    {	
    if(iNotifier)
    	{
    	iNotifier->Cancel();
    	}
    delete iNotifier;
    iRequestBuffer.ResetAndDestroy();
	iPrivacyResponseTimer->Cancel();
	delete iPrivacyResponseTimer;
    Cancel();
    }

// From MLbsPrivacyNotifierObserver
void CPrivacyNotifierHandler::OnNotificationDialogResponse(TInt aErr , 
	                         const TLbsPrivacyNotifierResponse& aResponse)
	{
	iPrivacyResponseTimer->Cancel();
	iPrivacyResponseOutstanding = EFalse;
	
	CPrivacyRequest* request = iRequestBuffer[0];
	if(aErr == KErrNone)
		{
		switch(aResponse)
			{
			case EResponseAccepted:
				{
				iObserver->OnRespondNetworkLocationRequest(request->SessionId(), 
						TLbsNetworkEnumInt::EPrivacyResponseAccepted, KErrNone);				
				break;
				}
				
			case EResponseTimedOut:
				{
				iNotifier->Cancel();
				TLbsNetworkEnumInt::TLbsPrivacyResponseInt response;
				GetPrivacyTimeoutAction(response, request->RequestPrivacy());
				iObserver->OnRespondNetworkLocationRequest(
									request->SessionId(), response, KErrNone);
				break; // case EResponseTimedOut
				}

			default:				
			case EResponseRejected:
				{
				iObserver->OnRespondNetworkLocationRequest(request->SessionId(), 
						TLbsNetworkEnumInt::EPrivacyResponseRejected, KErrNone);				
				break;
				}
			}
		}
	else			
	// Treat any error as an 'unknown' response
		{
		iObserver->OnRespondNetworkLocationRequest(request->SessionId(), 
					TLbsNetworkEnumInt::EPrivacyResponseUnknown, KErrArgument);
		}
	
	// Delete the current request and start the next one in the buffer
	iRequestBuffer.Remove(0);
	delete request; 
   	SendNextPrivacyRequest();
	}

/* Return ETrue if we are currently waiting on the RNotifier dialog.

Uses the response timer, because this should only be running
when we are waiting for the RNotifier.
*/
TBool CPrivacyNotifierHandler::IsPrivacyRequestActive()
	{
	return (iPrivacyResponseTimer->IsActive());
	}

void CPrivacyNotifierHandler::WaitForPrivacyResponse()
	{
	// Start a timeout timer, in case the privacy notifier doesn't reply.
	iPrivacyResponseTimer->Cancel();
	TUint privacyVerifyTimeoutInMilliSeconds = 0;		
	// Note: the following should never fail as we create a default setting in case of absence
	iLbsAdmin.Get(KLbsSettingPrivacyAppTimeout, privacyVerifyTimeoutInMilliSeconds);	
	TTimeIntervalMicroSeconds privacyVerifyTimeout = privacyVerifyTimeoutInMilliSeconds * 1000;
	if (privacyVerifyTimeout > 0)
		{
		iPrivacyResponseTimer->EventAfter(privacyVerifyTimeout, 1);		
		}	
	// flag to show we're waiting for a response
	iPrivacyResponseOutstanding = ETrue;
	}

void CPrivacyNotifierHandler::SendNextPrivacyRequest()
	{
	// May need to loop over multiple *notify* requests,
	// because we don't wait for a response for those.	
	while (iRequestBuffer.Count() > 0 && !IsPrivacyRequestActive())
		{
		CPrivacyRequest* request = iRequestBuffer[0];
		
		TRequiredPrivacyAction notifierAction(ERequiredPrivacyActionNone);
		TBool waitForResponse(ETrue);
		TLbsNetworkEnumInt::TLbsPrivacyResponseInt response(TLbsNetworkEnumInt::EPrivacyResponseAccepted);
		TInt responseReason = KErrNone;
		
		CLbsAdmin::TExternalLocateService externalLocate(CLbsAdmin::EExternalLocateOff);
		GetExternalLocateAdminSetting(request->SessionType(), externalLocate);

		// ERequestAdviceStealth is not supported, so reject any such request
		if(request->RequestPrivacy().RequestAdvice() == 
			TLbsNetPosRequestPrivacyInt::ERequestAdviceStealth)
			{
			notifierAction = ERequiredPrivacyActionNone;
			waitForResponse = EFalse;
			response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			responseReason = KErrNotSupported;
			}
		else
			{
			TLbsNetPosRequestPrivacyInt privacy = request->RequestPrivacy();
			GetRequiredNotificationAction(	request->IsEmergency(), 
											externalLocate,
											notifierAction, 
											waitForResponse, 
											response,
											privacy);
			request->SetRequestPrivacy(privacy);
			}
		
		// If required, raise a notification/verification
		TInt displayErr = KErrNone;
		if(iNotifier && iNotifierAction != notifierAction)
			{
	    	iNotifier->Cancel();
			delete iNotifier;	
			iNotifier = NULL;	
			}
			
		// Start a notify or verify notifier depending on the type of the privacy request.
		if(iNotifier == NULL)
			{
			switch(notifierAction)
				{
				case ERequiredPrivacyActionNotify:
					{
					TRAPD(err, iNotifier = CLbsPrivacyNotifier::NewL(EPrivacyDialogNotification));
					if(err == KErrNone)
					    {
                        iNotifier->SetObserver(this);
					    }
					break;
					}

				case ERequiredPrivacyActionVerify:
					{
					TRAPD(err, iNotifier = CLbsPrivacyNotifier::NewL(EPrivacyDialogVerification));
					if(err == KErrNone)
					    {
                        iNotifier->SetObserver(this);
					    }
					break;
					}
			
				default:
				case ERequiredPrivacyActionNone:
					{
					// do nothing
					break;
					}
				}
			}
			
		iNotifierAction = notifierAction;
		if(iNotifier)
			{
			TRAPD(displayLeaveErr, iNotifier->DisplayL(displayErr, request->RequestInfo()));
			// If there is an error, treat the notification as having returned an
			// 'unknown' response.
			if(displayLeaveErr != KErrNone || displayErr != KErrNone)
				{
				response = TLbsNetworkEnumInt::EPrivacyResponseUnknown;
				waitForResponse = EFalse;
				}	
			}
		
		// If the privacy controller application is responsible for accepting
		// or rejecting the privacy request, wait for its response
		if (waitForResponse)
			{
			WaitForPrivacyResponse();		
			}
		else
			{
			// Send an immediate response
			iObserver->OnRespondNetworkLocationRequest(request->SessionId(), response, responseReason);
			
			// Remove the request since we don't need to wait for the response.
			iRequestBuffer.Remove(0);
			delete request; 
         	}
		}
	}

void CPrivacyNotifierHandler::SendPrivacyResponse()
	{
	}
	
TInt CPrivacyNotifierHandler::BufferPrivacyRequest(
		const TLbsNetSessionIdInt& aSessionId,
		const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,		
		const TLbsExternalRequestInfo& aRequestInfo,
		const TLbsNetPosRequestPrivacyInt& aRequestPrivacy,
		TBool aIsEmergency)
	{
	CPrivacyRequest* requestData = NULL;
	TRAPD(err, requestData = CPrivacyRequest::NewL());
	if (err == KErrNone)
		{
		requestData->SetSessionId(aSessionId);
		requestData->SetSessionType(aSessionType);
		requestData->SetRequestInfo(aRequestInfo);
		requestData->SetRequestPrivacy(aRequestPrivacy);
		requestData->SetIsEmergency(aIsEmergency);
		requestData->SetStartTime();
		err = iRequestBuffer.Append(requestData);
		}
	
	return err;
	}

void CPrivacyNotifierHandler::RemovePrivacyRequestFromBuffer(
		const TLbsNetSessionIdInt& aSessionId)
	{
	TInt index = iRequestBuffer.Find(aSessionId, CPrivacyRequest::IsSessionIdEqual);
	while (KErrNotFound != index)
		{
		CPrivacyRequest* reqData = iRequestBuffer[index];
		iRequestBuffer.Remove(index);
		delete reqData; 
    	index = iRequestBuffer.Find(aSessionId, CPrivacyRequest::IsSessionIdEqual);
		}
	}
	
void CPrivacyNotifierHandler::OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& /* aRequestId */, 
                            TLbsNetworkEnumInt::TLbsPrivacyResponseInt /* aRequestResult */,
                            TInt /*aResponseReason*/)
	{
	}
	
/**
 * Unused for Notifier - cancel from the notifier is reported via 
 * MLbsPrivacyNotifierObserver::OnNotificationDialogResponse.
 */
void CPrivacyNotifierHandler::OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& /*aRequestId*/)
	{
	}

void CPrivacyNotifierHandler::RunL()
	{
	}
	
void CPrivacyNotifierHandler::DoCancel()
	{
	}
	
void CPrivacyNotifierHandler::SetServerObserver(MLbsSessionObserver* /*aNrhServer*/)
    {
    // Unused for notifier
    }
// A privacy request must be accepted or rejected, based on a
// combination of iPrivacyRequest->RequestPrivacy() settings, LbsAdmin settings
// and whether it is an emergency request.
//

void CPrivacyNotifierHandler::GetRequiredNotificationAction(
									TBool aIsEmergency,
									CLbsAdmin::TExternalLocateService aExternalLocate,
									TRequiredPrivacyAction& aPrivacyAction, 
									TBool& aTimeoutRequired,
									TLbsNetworkEnumInt::TLbsPrivacyResponseInt& aResponse,
									TLbsNetPosRequestPrivacyInt& aPrivacy
									)
	{
	
    if (aIsEmergency)
    	{
    	TBool silent = (aPrivacy.RequestAdvice() == TLbsNetPosRequestPrivacyInt::ERequestAdviceSilent);
		aTimeoutRequired = EFalse; // always the case for emergency - it's only ever notified
										
    	// Decide whether to accept or reject an emergency request.
    	switch (aPrivacy.RequestAction())
    		{
    		case TLbsNetPosRequestPrivacyInt::ERequestActionAllow:
    			{
				aResponse = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
				if(silent)
					{
    				aPrivacyAction = ERequiredPrivacyActionNone;										
					}
				else
					{
    				aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
    				aPrivacyAction = ERequiredPrivacyActionNotify;
					}
    			break;
    			}
			case TLbsNetPosRequestPrivacyInt::ERequestActionReject:
				{
				aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
				if(silent)
					{
    				aPrivacyAction = ERequiredPrivacyActionNone;										
					}
				else
					{
    				aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
    				aPrivacyAction = ERequiredPrivacyActionNotify;					
					}
    			break;
    			}
    		default:
    			{
	    		LBSLOG2(ELogP4, "CPrivCntrllrHandler::ProcessNetworkLocationRequest :  Unknown RequestAction() (0x%x), rejecting privacy request", 
	    						 aPrivacy.RequestAction());    			
    			aPrivacyAction = ERequiredPrivacyActionNone;
				aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
    			break;
    			}
    		}
    		// For the privacy notifier, if the original advice is not
    		// notify or verify, don't do anything
    	}
    else
    	{
    	// Decide wether the privacy controller application should be notified,
    	// and if not, whether the request should be accepted or rejected.
	    switch (aExternalLocate)
	    	{
	    	case CLbsAdmin::EExternalLocateOn:
	    		{
				// Whether we wait for the privacy controller application response
				// depends on the RequestAdvice() from the network
				switch (aPrivacy.RequestAdvice())
					{
					case TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify:
						{
    					aPrivacyAction = ERequiredPrivacyActionVerify;
						aTimeoutRequired = ETrue;
						break;
						}
					case TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify:
						{
    					aPrivacyAction = ERequiredPrivacyActionNotify;
						aTimeoutRequired = EFalse;
						if(aPrivacy.RequestAction() == TLbsNetPosRequestPrivacyInt::ERequestActionAllow)
							{
							aResponse = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
							}
						else
							{
							aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
							}
						break;
						}
					case TLbsNetPosRequestPrivacyInt::ERequestAdviceSilent:
						{
						// Always accept these types of privacy request for the controller - and 
						// tell the controller about them. For the notifier, just do what it says, 
						// without any notification
						// Either way, no timer is needed
						aTimeoutRequired = EFalse;
						aPrivacyAction = ERequiredPrivacyActionNone;
						if(aPrivacy.RequestAction() == TLbsNetPosRequestPrivacyInt::ERequestActionAllow)
							{
							aResponse = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
							}
						else
							{
							aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
							}
						break;
						}
					default:
						{
						// Error: Unknown RequestAdvice() value. Send a 
						// 'reject' back to the network
			    		LBSLOG2(ELogP4, "CPrivCntrllrHandler::ProcessNetworkLocationRequest : \
			    						 Unknown RequestAdvice() (0x%x), \
			    						 rejecting privacy request", 
			    						 aPrivacy.RequestAdvice());
    					aPrivacyAction = ERequiredPrivacyActionNone;
						aTimeoutRequired = EFalse;
						aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
						break;
						}
					}
	    		break;
	    		}
	    	case CLbsAdmin::EExternalLocateOnButAlwaysVerify:
	    		{
    			aPrivacyAction = ERequiredPrivacyActionVerify;
	    		aTimeoutRequired = ETrue;
	    		aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify);
	    		break;
	    		}
	    	case CLbsAdmin::EExternalLocateOff:
	    		{
    			aPrivacyAction = ERequiredPrivacyActionNone;
	    		aTimeoutRequired = EFalse;
	    		aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
	    		break;
	    		}
	    	case CLbsAdmin::EExternalLocateOffButNotify:
	    		{
    			aPrivacyAction = ERequiredPrivacyActionNotify;
	    		aTimeoutRequired = EFalse;
	    		aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
	    		aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
	    		aPrivacy.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionReject);
	    		break;
	    		}
	    	case CLbsAdmin::EExternalLocateUnknown:
	    	default:
	    		{
	    		LBSLOG2(ELogP4, "CPrivCntrllrHandler::ProcessNetworkLocationRequest : \
	    						 Unknown Admin setting (0x%x), \
	    						 rejecting privacy request", aExternalLocate);
    			aPrivacyAction = ERequiredPrivacyActionNone;
	    		aTimeoutRequired = EFalse;
	    		aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
	    		break;
	    		}
	    	}
    	
    	}
	}

//================================================================================
/**
 * The CPrivacyControllerHandler class handles privacy requests when the
 * configuration specifies that a Privacy Controller Application is 
 * present, rather than a Notifier Handler.
 * Its main task is to pass on messages to the Privacy Controller using 
 * he NRH server. Communications between this class and the NRH server 
 * are mediated by internally-defined properties
 */
CPrivacyControllerHandler::CPrivacyControllerHandler(CLbsAdmin& aLbsAdmin,
													 RLbsNetworkRegistrationStatus& aNetRegStatus)  :
	CPrivacyHandler(aLbsAdmin, aNetRegStatus)
    {
	iSessionActive = EFalse;
	iIsEmergency = EFalse;
	iAlwaysVerify = EFalse;
	TInt err = iLbsAdmin.Get(KLbsSettingBehaviourMode, iLbsBehaviourMode);
	if (err != KErrNone)
		{
		iLbsBehaviourMode = CLbsAdmin::ELbsBehaviourCustom1;
		}	
    }
CPrivacyControllerHandler::~CPrivacyControllerHandler()
	{
	iRequestBuffer.ResetAndDestroy();
	iPrivacyResponseTimer->Cancel();
	delete iPrivacyResponseTimer;
	delete iEmergencyPrivReq;
	}
    
CPrivacyControllerHandler* CPrivacyControllerHandler::NewL(CLbsAdmin& aLbsAdmin,
														   RLbsNetworkRegistrationStatus& aNetRegStatus)
    {
	CPrivacyControllerHandler* self = new (ELeave)CPrivacyControllerHandler(aLbsAdmin,
																			aNetRegStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
    }
    
void CPrivacyControllerHandler::ConstructL()
	{
	CActiveScheduler::Add(this);
	iPrivacyResponseTimer = CLbsCallbackTimer::NewL(*this);
	iEmergencyPrivReq = CPrivacyRequest::NewL();
	}

/**
 * Passes on a privacy request to the server.
 * @param aSessionId identified the location update session within the LBS System
 * @param aRequestInfo 	data about the source of the request
 * @param aNetPosRequestPrivacy defines what notification is necessary (e.g.
 *                              notify or confirm). 
 */
void CPrivacyControllerHandler::ProcessNetworkLocationRequest(TLbsNetSessionIdInt aSessionId, 
												const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
												const TLbsExternalRequestInfo&  aRequestInfo, 
												const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
												TBool aIsEmergency)
	{	
	CPrivacyRequest* privReq = NULL;
	if (aIsEmergency)
	    {
	    privReq = iEmergencyPrivReq;
	    }
	else
	    {
	    TRAPD(err, privReq = CPrivacyRequest::NewL());
	    if(err != KErrNone)
	        {
            // We can do nothing here
            return;
	        }
	    }
	privReq->SetIsEmergency(aIsEmergency);
	privReq->SetSessionId(aSessionId);
	privReq->SetSessionType(aSessionType);
	privReq->SetRequestInfo(aRequestInfo);
	// If the admin setting timeout strategy is reject
	TLbsNetworkEnumInt::TLbsPrivacyResponseInt adminTimeoutAction;
	GetPrivacyTimeoutAction(adminTimeoutAction, aNetPosRequestPrivacy);
	if((adminTimeoutAction == TLbsNetworkEnumInt::EPrivacyResponseRejected)
		&& (aNetPosRequestPrivacy.RequestAdvice() == TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify))
		{
		// We should update the request timeout action so it is also reject
		TLbsNetPosRequestPrivacyInt rejectNetPosRequestPrivacy;
		rejectNetPosRequestPrivacy.SetRequestAdvice(aNetPosRequestPrivacy.RequestAdvice());
		rejectNetPosRequestPrivacy.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionReject);
		privReq->SetRequestPrivacy(rejectNetPosRequestPrivacy);
		}
	else
		{
		// Otherwise keep it the same
		privReq->SetRequestPrivacy(aNetPosRequestPrivacy);
		}
	privReq->SetStartTime();
	TInt err = iRequestBuffer.Append(privReq);
	    if(err != KErrNone)
	        {
	    	LBSLOG2(ELogP4, "CPrivacyControllerHandler::ProcessNetworkLocationRequest : iRequestBuffer.Append failed with - %d error",err); 
	        }
	iAlwaysVerify = EFalse;	
	
	if (iNrhServer == 0)
		{
		if (privReq->IsEmergency())
			{
			// No handler to ProcessNetworkLocationRequest - just continue with processing emergency.
			TLbsNetworkEnumInt::TLbsPrivacyResponseInt response(TLbsNetworkEnumInt::EPrivacyResponseAccepted);    
			iObserver->OnRespondNetworkLocationRequest(privReq->SessionId(), response, KErrNone);
			privReq->SetState(CPrivacyRequest::EPrivReqStateResponseSent);
			return;			
			}
		else
			{
			// No handler to ProcessNetworkLocationRequest - respond depending on type:
			// Verify - reject
			// Notify - default action
			TLbsNetworkEnumInt::TLbsPrivacyResponseInt response;
			TInt responseReason = KErrNone;
			switch (privReq->RequestPrivacy().RequestAdvice())
				{
				case TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify:
					{
					switch (privReq->RequestPrivacy().RequestAction())
			    		{
			    		case TLbsNetPosRequestPrivacyInt::ERequestActionAllow:
			    			{
			    			response = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
			    			break;
			    			}
			    		case TLbsNetPosRequestPrivacyInt::ERequestActionReject:
			    			{
			    			response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			    			break;
			    			}
			    		default:
			    			{
			    			response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			    			responseReason = KErrArgument;
			    			break;
			    			}
			    		}
					break;
					}
				case TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify:
					{
					response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
					responseReason = KErrServerBusy;
					break;
					}
				default:
					{
					response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
					responseReason = KErrArgument;
					break;
					}
				}
			iObserver->OnRespondNetworkLocationRequest(privReq->SessionId(), response, responseReason);
			privReq->SetState(CPrivacyRequest::EPrivReqStateResponseSent);
			return;
			}
		}

    // The new privacy request must be accepted or rejected, based on a
    // combination of iPrivacyRequest->RequestPrivacy() settings, LbsAdmin settings
    // and whether it is an emergency request.
    //
    // For a normal privacy request, the privacy controller application usually
    // has the responsibility for accepting or rejecting it. (Although this depends
    // on the LbsAdmin settings). 
    // NOTE (1): Any non-emergency request with RequestAdvice() of ERequestAdviceStealth 
	//           will be rejected, since this is not currently supported by LBS.
	// NOTE (2): If the current quality profile Id (admin setting) refers to an invalid
	//           quality profile then any non-ememrgency request will be rejected.
    //
    // For an emergency privacy request, the privacy controller application
    // is always notified of the request, but it has no say over whether
    // the request is accepted or not. The request is accepted if the 
    // RequestAction() is ERequestActionAllow, otherwise it will be rejected.
    
    TBool notifyController(EFalse);
	TBool waitForResponse(ETrue);
	TLbsNetworkEnumInt::TLbsPrivacyResponseInt response(TLbsNetworkEnumInt::EPrivacyResponseAccepted);    
	TInt responseReason = KErrNone;
	TLbsNetPosRequestPrivacyInt privacy = privReq->RequestPrivacy();
    
    if (aIsEmergency)
    	{
    	// Decide whether to accept or reject an emergency request.
    	switch (privacy.RequestAction())
    		{
    		case TLbsNetPosRequestPrivacyInt::ERequestActionAllow:
    			{
    			privacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
    			notifyController = ETrue;
				waitForResponse = EFalse;
				response = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
    			break;
    			}
    		case TLbsNetPosRequestPrivacyInt::ERequestActionReject:
    			{
    			privacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
    			notifyController = ETrue;
				waitForResponse = EFalse;
				response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
    			break;
    			}
    		default:
    			{
	    		LBSLOG2(ELogP4, "CPrivCntrllrHandler::ProcessNetworkLocationRequest : \
	    						 Unknown RequestAction() (0x%x), \
	    						 rejecting privacy request", 
	    						 privacy.RequestAction());
    			notifyController = EFalse;
				waitForResponse = EFalse;
				response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
				responseReason = KErrArgument;
    			break;
    			}
    		}
    	
		privReq->SetRequestPrivacy(privacy);	
    		
    	}
    else
    	{
    	// Decide wether the privacy controller application should be notified,
    	// and if not, whether the request should be accepted or rejected.
    	CLbsAdmin::TExternalLocateService externalLocate(CLbsAdmin::EExternalLocateOff);
		GetExternalLocateAdminSetting(privReq->SessionType(), externalLocate);
	    switch (externalLocate)
	    	{
	    	case CLbsAdmin::EExternalLocateOn:
	    		{
				// Whether we wait for the privacy controller application response
				// depends on the RequestAdvice() from the network
				switch (privacy.RequestAdvice())
					{
					case TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify:
						{
						notifyController = ETrue;
						waitForResponse = ETrue;
						break;
						}
					case TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify:
						{
						switch(privacy.RequestAction())
							{
							case TLbsNetPosRequestPrivacyInt::ERequestActionReject:
								{
								// automatically reject and inform user
								notifyController = ETrue;
								waitForResponse = EFalse;
								response = TLbsNetworkEnumInt::EPrivacyResponseRejected;								
								}
								break;
							case TLbsNetPosRequestPrivacyInt::ERequestActionAllow:
								{
								// Always accept these types of privacy request
								notifyController = ETrue;
								waitForResponse = EFalse;
								response = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
								}
								break;
							case TLbsNetPosRequestPrivacy::ERequestActionNotUsed:
							default:
								{
								// Error: Unknown RequestAdvice() value. Send a 
								// 'reject' back to the network
					    		LBSLOG2(ELogP4, "CPrivCntrllrHandler::ProcessNetworkLocationRequest : \
					    						 Unknown TLbsRequestAction() (0x%x), \
					    						 rejecting privacy request", 
					    						 privacy.RequestAdvice());
					    		notifyController = EFalse;
								waitForResponse = EFalse;
								response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
								responseReason = KErrArgument;
								}
								break;
							}
						break;
						}
					case TLbsNetPosRequestPrivacyInt::ERequestAdviceSilent:
						{
						// Always accept these types of privacy request
						notifyController = ETrue;
						waitForResponse = EFalse;
						// For 'vanilla' lbs we should reject if network action = reject
						if((iLbsBehaviourMode == CLbsAdmin::ELbsBehaviourModeOriginal) && (privacy.RequestAction() == TLbsNetPosRequestPrivacyInt::ERequestActionReject))
							{
							response = TLbsNetworkEnumInt::EPrivacyResponseRejected;							
							}
						else	// Always accept these types of privacy request for 'custom1' behaviour
							{
							response = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
							privacy.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionAllow);							
							}						
						break;
						}
					case TLbsNetPosRequestPrivacyInt::ERequestAdviceStealth:
						{
						notifyController = EFalse;
						waitForResponse = EFalse;
						response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
						responseReason = KErrNotSupported;
						break;
						}
					default:
						{
						// Error: Unknown RequestAdvice() value. Send a 
						// 'reject' back to the network
			    		LBSLOG2(ELogP4, "CPrivCntrllrHandler::ProcessNetworkLocationRequest : \
			    						 Unknown RequestAdvice() (0x%x), \
			    						 rejecting privacy request", 
			    						 privacy.RequestAdvice());
			    		notifyController = EFalse;
						waitForResponse = EFalse;
						response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
						responseReason = KErrArgument;
						break;
						}
					}
	    		break;
	    		}
	    	case CLbsAdmin::EExternalLocateOnButAlwaysVerify:
	    		{
	    		iAlwaysVerify = ETrue; 	    		
	    		notifyController = ETrue;
	    		waitForResponse = ETrue;
	    		privacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify);
	    		break;
	    		}
	    	case CLbsAdmin::EExternalLocateOff:
	    		{
	    		notifyController = EFalse;
	    		waitForResponse = EFalse;
	    		response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
	    		break;
	    		}
	    	case CLbsAdmin::EExternalLocateOffButNotify:
	    		{
	    		notifyController = ETrue;
	    		waitForResponse = EFalse;
	    		response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
	    		privacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
	    		privacy.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionReject);
	    		break;
	    		}
	    	case CLbsAdmin::EExternalLocateUnknown:
	    	default:
	    		{
	    		LBSLOG2(ELogP4, "CPrivCntrllrHandler::ProcessNetworkLocationRequest : \
	    						 Unknown Admin setting (0x%x), \
	    						 rejecting privacy request", externalLocate);
	    		notifyController = EFalse;
	    		waitForResponse = EFalse;
	    		response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
	    		responseReason = KErrArgument;
	    		break;
	    		}
	    	}
	    privReq->SetRequestPrivacy(privacy);	
    	
    	// ERequestAdviceStealth is currently not implemented/supported,
    	// so regardless of the admin settings, etc. reject all requests
    	// with RequestAdvice() of ERequestAdviceStealth.
    	if (privReq->RequestPrivacy().RequestAdvice() == TLbsNetPosRequestPrivacyInt::ERequestAdviceStealth)
    		{
			notifyController = EFalse;
			waitForResponse = EFalse;
			response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			responseReason = KErrNotSupported;
    		}
    	
		// For 'custom1' behaviour, always reject the request if the current quality profile is invalid,
    	// For 'vanilla' we should allow for no profile in use (quality will be taken from request params)
    	// note that the profileid is not used here, we just verify whether it exists.
    	// When the location request arrives the quality settings from the profile are read and used
	    TLbsQualityProfileId profileId(KLbsNullQualityProfileId);
	    TInt err = iLbsAdmin.Get(KLbsSettingQualityProfileExternalLocate, profileId);
	    if (err == KErrNone)	// there should always be a profile setting of some kind in admin
	    	{
	    	if(!((iLbsBehaviourMode == CLbsAdmin::ELbsBehaviourModeOriginal) && (profileId == KLbsNoProfileInUseQualityProfileId)))
	    		{
		    	TQualityProfile qualityProfile;
	    		err = LbsQualityProfile::GetQualityProfileById(profileId, qualityProfile);
	    		}
	       	}
		
		if (err != KErrNone)	// no profile specified in admin, or profile in use and profile doesn't exist in profile ini file
			{
			notifyController = EFalse;
			waitForResponse = EFalse;
			response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			}
    	}

	// If required, send a notification to the privacy controller application.
	if (notifyController)
		{
		iNrhServer->ProcessNetworkLocationRequest(aSessionId, privReq->SessionType(), aRequestInfo, 
												  privReq->RequestPrivacy(), 
												  aIsEmergency);
		iSessionActive = ETrue;
		iRefPosReported = EFalse;
		}
	
	// If the privacy controller application is responsible for accepting
	// or rejecting the privacy request, wait for its response
	if (waitForResponse)
		{
		privReq->SetState(CPrivacyRequest::EPrivReqStateWaitPrivacyResponse);
		
		// Start a timeout timer, in case the privacy controller application
		// doesn't reply.
		CancelResponseTimer();
		StartResponseTimer();		
		}
	else
		{
		// Send an immediate response
		iObserver->OnRespondNetworkLocationRequest(privReq->SessionId(), response, responseReason);
		// If it's being rejected straight off, make sure any 
		// 'request completes' which come along later aren't passed on, UNLESS
		// the controller is to be told about the request (e.g. when the
		// external locate is EExternalLocateOffButNotify.
		if(response == TLbsNetworkEnumInt::EPrivacyResponseRejected && !notifyController)
			{
			iSessionActive = EFalse;
			iIsEmergency = EFalse;
			privReq->SetState(CPrivacyRequest::EPrivReqStateCompleted);
			}
		else
			{
			privReq->SetState(CPrivacyRequest::EPrivReqStateResponseSent);
			}
		}
	}

/**
 * Not used. Here for symmetry with the Notifier Handler class
 */
void CPrivacyControllerHandler::RunL()
	{
	}	
void CPrivacyControllerHandler::DoCancel()
	{
	}

/**
 * Called when a location update has been received. The data is passed on 
 * to the server.
 * @param aSessionId identifies the session within the LBS System
 * @param aPositionInfo position data to be passed on to the 
 *                      Privacy Controller
 */
void CPrivacyControllerHandler::ProcessNetworkPositionUpdate(
		TLbsNetSessionIdInt aSessionId, 
		const TPositionInfo& aPositionInfo)
	{
	// If there is no privacy handler, ignore the position update.
	if (iNrhServer == 0)
		{
		return;
		}
	
	CPrivacyRequest* privReq = FindPrivacyRequest(aSessionId, EFalse);
	if (privReq == NULL)
		{
		// Not matching request found, so return.
		return;
		}

	// For terminal assisted mode there may be several update requests.
	// We don't want to bombard the privacy controller with all of these, 
	// so just send the first one. Position updates that aren't flagged
	// with PositionMode() of ETechnologyNetwork are  OK - these are 'real' 
	// updates which the privacy controller wants to know about.
	if(aPositionInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork)
		{
		iRefPosReported = ETrue;
		}
		
	if (iNrhServer != 0)
		{
		iNrhServer->ProcessNetworkPositionUpdate(
			privReq->SessionId(), 
			aPositionInfo);
		}

	}
	
/**
 * Called when a session complete message has been received. 
 * The data is passed on to the server.
 * @param aSessionId identifies the session within the LBS System
 * @param aReason reason for termination of the session
 */
void CPrivacyControllerHandler::ProcessRequestComplete(
		TLbsNetSessionIdInt aSessionId, 
		TInt aReason)
	{
	CPrivacyRequest* privReq = FindPrivacyRequest(aSessionId, ETrue);
	if (privReq == NULL)
		{
		// Not matching request found, so return.
		return;
		}

	if (privReq->State() != CPrivacyRequest::EPrivReqStateCompleted)
		{
		// Notify the privacy controller of the session complete message
		if (iNrhServer != 0)
			{
			iNrhServer->ProcessRequestComplete(privReq->SessionId(), aReason);	
			}
		privReq->SetState(CPrivacyRequest::EPrivReqStateCompleted);
		}
	
	// Delete the completed request.
	// do not delete emergency buffer - not onheap
	if(privReq != iEmergencyPrivReq)
	    {
	    delete privReq; 
	    }
	// Cancel the response timer, then re-start if any outstanding requests.
	CancelResponseTimer();
	StartResponseTimer();
	}

/**
 * Called when the server passes on a response to a privacy query.
 * Message wtaxchers and the timer are stopped, and the response is passed
 * on to the Privacy and Location Handler subcomponent.
 * @param aSessionId identifies the session within the LBS System
 * @param aRequestResult response to the query
 */
void CPrivacyControllerHandler::OnRespondNetworkLocationRequest(
		const TLbsNetSessionIdInt& aRequestId, 
		TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
		TInt aResponseReason)
	{
	CPrivacyRequest* privReq = FindPrivacyRequest(aRequestId, EFalse);
	if (privReq)
		{
		if (privReq->State() == CPrivacyRequest::EPrivReqStateWaitPrivacyResponse)
			{
			iObserver->OnRespondNetworkLocationRequest(
						privReq->SessionId(), aRequestResult, aResponseReason);
			privReq->SetState(CPrivacyRequest::EPrivReqStateResponseSent);
			}

		CancelResponseTimer();
		StartResponseTimer();
		}
	}
	
/**
 * Called when the server passes on cancellation of a privacy query by thet 
 * Privacy Controller.
 * Message wtaxchers and the timer are stopped, and the cancellation
 * is passed on to the Privacy and Location Handler subcomponent.
 * @param aSessionId identifies the session within the LBS System
 */
void CPrivacyControllerHandler::OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId)
	{
	CPrivacyRequest* privReq = FindPrivacyRequest(aRequestId, EFalse);
	if (privReq)
		{
		// Cancel/Restart the timeout timer.
		CancelResponseTimer();
		StartResponseTimer();
		
		// Send cancel to the network.
		iObserver->OnCancelNetworkLocationRequest(privReq->SessionId());
		}
	}

void CPrivacyControllerHandler::StartResponseTimer()
	{
	if (iPrivacyResponseTimer->IsActive())
		{
		// Timer is currently active, don't start again.
		return;
		}
	
	// Find the earliest request that is waiting for a response.
	// (Buffer should be in order of time started, so earliest request
	// should always be the one nearest the start of the buffer.
	const TInt count = iRequestBuffer.Count();
	TUint privacyVerifyTimeoutInMilliSeconds = 0;		
	// Read timeout from admin (nb: should never fail as we create a default setting in case of absence)
	iLbsAdmin.Get(KLbsSettingPrivacyAppTimeout, privacyVerifyTimeoutInMilliSeconds);
	TTimeIntervalMicroSeconds timeout(privacyVerifyTimeoutInMilliSeconds*1000);	
	for (TInt i = 0; i < count; i++)
		{
		if (iRequestBuffer[i]->State() == CPrivacyRequest::EPrivReqStateWaitPrivacyResponse)
			{
			// Pending request was found; start the timer.
			CPrivacyRequest* privReq = iRequestBuffer[i];
			
			// Calculate the time at which the request would timeout.			
			TTime endTime = privReq->StartTime() + timeout;
			
			// Start the timer.
			iPrivacyResponseTimer->EventAtUTC(endTime, 1);
			
			break;
			}
		}
	}

void CPrivacyControllerHandler::CancelResponseTimer()
	{
	iPrivacyResponseTimer->Cancel();
	}

/**
 * Called when the response timer expires.
 * An accept or reject of the original query is reported to the NRH 
 * depending on the request action parameter received with the original request.
 * @param aTimerId always 1 (we only use one timer in this subcomponent)
 */
void CPrivacyControllerHandler::OnTimerEventL(TInt aTimerId)
	{
	// 1) Get time 'now'
	// 2) search through all requests to see if 'now' > 'start' + timeout
	// 3) For each request found in 2), send default response
	// 4) Need to remove the response from the buffer??
	// 5) Search through requests to find earliest 'start'
	// 6) If 5) finds a request, re-start timer for it. 
	//    (need to calculate revised interval).
	
	switch (aTimerId)
		{
		case 1:
			{
			// Find the earliest request that is waiting for a response.
			// (Buffer should be in order of time started, so earliest request
			// should always be the one nearest the start of the buffer.
			const TInt count = iRequestBuffer.Count();
			for (TInt i = 0; i < count; i++)
				{
				if (iRequestBuffer[i]->State() == CPrivacyRequest::EPrivReqStateWaitPrivacyResponse)
					{
					// Found a request in the correct state, assume
					// this is the one which has timed out.
					CPrivacyRequest* privReq = iRequestBuffer[i];
					
					// The privacy controller has timed out without a response being
					// received. Generate a response as required depending upon
					// the LbsAdmin setting: TPrivacyTimeoutAction
					TLbsNetworkEnumInt::TLbsPrivacyResponseInt response;
					GetPrivacyTimeoutAction(response, privReq->RequestPrivacy());	
					iObserver->OnRespondNetworkLocationRequest(privReq->SessionId(), response, KErrNone);
					
					privReq->SetState(CPrivacyRequest::EPrivReqStateResponseSent);
					
					// Re-start the timer if there are more outstanding requests.
					StartResponseTimer();
					break;
					}
				}
			break;
			}
		default:
			// do nothing
			break;	
		}
	}
	
TInt CPrivacyControllerHandler::OnTimerError(TInt /*aTimerId*/, 
											TInt aError)
	{
	// Not used - OnTimerEventL doesn't generate any error.
	return(aError);
	}
	
/**
 * Establish a connection with a server subsession (there is only ever one)
 */
void CPrivacyControllerHandler::SetServerObserver(MLbsSessionObserver* aNrhServer)
    {
    iNrhServer = aNrhServer;
    }

/**
 * Register the observer of this subcomponent (the Privacy and Location Request 
 * Handler)
 */
void CPrivacyControllerHandler::RegisterObserver(MPrivacyHandlerObserver* aObserver)
    {
    iObserver = aObserver;	
    }

TBool CPrivacyControllerHandler::IsSessionIdMatch(
		const CPrivacyRequest& aReq1, 
		const CPrivacyRequest& aReq2)
	{
	CPrivacyRequest& req1(const_cast<CPrivacyRequest&>(aReq1));
	CPrivacyRequest& req2(const_cast<CPrivacyRequest&>(aReq2));
	
	return (req1.SessionId().SessionOwner() == req2.SessionId().SessionOwner()
			&& req1.SessionId().SessionNum() == req2.SessionId().SessionNum());
	}

CPrivacyRequest* CPrivacyControllerHandler::FindPrivacyRequest(
		const TLbsNetSessionIdInt& aSessionId,
		TBool aRemoveRequest)
	{
	CPrivacyRequest* privReq = NULL;
	CPrivacyRequest* matchReq = NULL;
	TRAPD(err, matchReq = CPrivacyRequest::NewL());
	if (KErrNone == err)
		{
		matchReq->SetSessionId(aSessionId);
		TInt index = iRequestBuffer.Find(matchReq, IsSessionIdMatch);
		if (KErrNotFound != index)
			{
			privReq = iRequestBuffer[index];
			if (aRemoveRequest)
				{
				iRequestBuffer.Remove(index);
				}
			}
		delete matchReq;
		}
	return privReq;
	}

/** Get the default privacy response based on the admin setting.
 */
void CPrivacyControllerHandler::GetPrivacyTimeoutAction(
		TLbsNetworkEnumInt::TLbsPrivacyResponseInt& aResponse, 
		const TLbsNetPosRequestPrivacyInt& aRequestPrivacy)
	{
	CLbsAdmin::TPrivacyTimeoutAction privacyTimeoutAction;
	iLbsAdmin.Get(KLbsSettingPrivacyTimeoutAction, privacyTimeoutAction);
	
	switch(privacyTimeoutAction)
		{
		case CLbsAdmin::EPrivacyTimeoutReject:
			{
			aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			break;
			}

		case CLbsAdmin::EPrivacyTimeoutNetworkDefined:
			{
			// For 'vanilla' we shouldn't trust the network-specified action if admin setting says "always verify"			
			if(aRequestPrivacy.RequestAction() == TLbsNetPosRequestPrivacyInt::ERequestActionReject ||
					aRequestPrivacy.RequestAction() == TLbsNetPosRequestPrivacyInt::ERequestActionNotUsed ||
					(iLbsBehaviourMode == CLbsAdmin::ELbsBehaviourModeOriginal && iAlwaysVerify))
				{
				aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;				
				}
			else
				{
				aResponse = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
				}
			break;
			}
		default:
			{
			aResponse = TLbsNetworkEnumInt::EPrivacyResponseRejected;
			break;
			}
		}	
	}
