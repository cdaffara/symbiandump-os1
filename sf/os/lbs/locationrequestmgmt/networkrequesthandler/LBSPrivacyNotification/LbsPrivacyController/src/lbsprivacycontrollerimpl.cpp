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
// client side exmaple of how to use the abstract server framework
// 
//

#include <e32base.h>
#include <e32debug.h>
#include <lbs/lbslocclasstypes.h>
#include "lbsdevloggermacros.h"
#include "lbsprivacycontrollerimpl.h"
#include "lbsprocessuiddefs.h"
#include "messageutils.h"
#include "lbsnrhserverdata.h"
#include "lbsnrhmessageenums.h"
#include "lbssatellite.h"
#include <lbs/lbsgpsmeasurement.h>

/* The Privacy Controller Implementation hides the details of the 
 * Privacy Controller from the class CLbsPrivacyController, which uses only the
 * published API.
 *
 * The privacy comntroller is implemented here as a client of the Network
 * Request Handler, which includes a server to handle requests from this 
 * component.
 *
 * Generic operation on thet client side is implemented by the
 * code in lbsloccommon\serverframework.
  */
RNrhServer::RNrhServer()
	{
	// nothing to do
	}
 	
	
RNrhServer::~RNrhServer()
	{
	// nothing to do
	}

TVersion RNrhServer::Version()
	{
	return TVersion(KNrhServerMajorVersionNumber,
					KNrhServerMinorVersionNumber,
					KNrhServerBuildVersionNumber);
	}

TInt RNrhServer::StartServer(TServerStartParams& /*aParams*/)
	{
	// Start LBS from LbsRoot    
    TRAPD(err, StartLbsL());
    return (err);
	}

void RNrhServer::StartLbsL()
	{
    RLbsSystemController systemController;
    RProcess process;
    CleanupClosePushL(process);
	systemController.OpenL(process.SecureId());
	CleanupClosePushL(systemController);
	
	TInt startErr = systemController.RequestSystemStartup();
	if (startErr != ELbsStartupSuccess)
		{
		User::Leave(KErrCouldNotConnect);
		}
	
	CleanupStack::PopAndDestroy(2, &process);
	}

//------------------------------------------------------------	

RNrhSession::RNrhSession()
	{
	// nothing to do
	}
	
RNrhSession::~RNrhSession()
	{
	// nothing to do
	}
	

/**
 * Transmits to the server a response to a privacy query.
 * The message is synchronous, as no response is expected.
 *
 * @param aSessionId identified the session to the network request handler to
 *                   allow matching of request and response.
 * @param aResult	indicates whether the request is accepted or rejected
 * @return		KErrNone or an error code as for SendReceived
 */
TInt RNrhSession::SendPrivacyResponse(TLbsNetSessionIdInt aSessionId,
					CLbsPrivacyController::TRequestVerificationResult aResult)
	{
	TInt err;
	
	TPrivacyResponseParams privacyResponseParams;
	privacyResponseParams.iSessionId = aSessionId;
	privacyResponseParams.iResult = aResult;
		
	TPckgBuf<TPrivacyResponseParams> privacyResponseParamsBuf(privacyResponseParams);
	TIpcArgs args(&privacyResponseParamsBuf);

	err = SendReceive(ENrhPrivacyControllerResponse, args);
			
	return(err);
	}

/**
 * Transmits to the server a cancel to a privacy query.
 * The message is synchronous, as no response is expected.
 *
 * @param aSessionId identified the session to the network request handler to
 *                   allow matching of request and response.
 * @return		KErrNone or an error code as for SendReceived
 */
TInt RNrhSession::SendPrivacyCancel(TLbsNetSessionIdInt aSessionId)
	{
	// Synchronous - we don't expect a response

	TInt err;
	
	TPrivacyCancelParams privacyCancelParams;
	privacyCancelParams.iSessionId = aSessionId;
		
	TPckgBuf<TPrivacyCancelParams> privacyCancelParamsBuf(privacyCancelParams);
	TIpcArgs args(&privacyCancelParamsBuf);

	err = SendReceive(ENrhPrivacyControllerCancel, args);
			
	return(err);
	}
/**
 * Issues an uotstanding request to the server, which is completed 
 * when there is a message for the Pribvacy Controller (Request, Location Update
 * or Complete).
 * With the message, pointers to a buffer for each of the types of message
 * data is passed. The appropraite one is filled when a message is 
 * available, and the relevant one is indicated by the completion code for the
 * status parameter.
 *
 * @param aTrs   TRequestStatus 
 */
void RNrhSession::RegisterForServerMessages(TRequestStatus& aTrs)
	{
	// Have to send it with space for whatever data the server wants to send 
	// back - could be privacy request, location update or session complete
	TIpcArgs args(	&iPrivacyRequestParamsBuf,
					&iLocationUpdateParamsBuf,
					&iLbsSessionCompleteParamsBuf);
	
	SendReceive(ENrhPrivacyControllerRegister, args, aTrs);
	}

/**
 * Cancels the outstanding request issued by RegisterForServerMessages()
 *
 */	
TInt RNrhSession::CancelRequest()
	{
	TInt err;
	TIpcArgs args(TIpcArgs::ENothing);
	err = SendReceive(ENrhPrivacyControllerCancelRegister, args);
	return err;	
	}

//-----------------------------------------------------------

CLbsPrivacyControllerImpl::CLbsPrivacyControllerImpl (MLbsPrivacyObserver* aObserver)
: 	CActive(EPriorityStandard),
	iObserver(aObserver)
	{
	
	}

CLbsPrivacyControllerImpl* CLbsPrivacyControllerImpl::NewL(MLbsPrivacyObserver* aObserver)	
	{
	CLbsPrivacyControllerImpl* self = new (ELeave) CLbsPrivacyControllerImpl(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
	}
	
CLbsPrivacyControllerImpl::~CLbsPrivacyControllerImpl ()
	{
	if(iSession.SubSessionHandle() != 0)
		{
		if(IsActive())
			{
			iSession.CancelRequest();	
			}
		iSession.Close();
		}
	if(iServer.Handle() != 0)
		{
		iServer.Close();
		}
	if(IsActive())
		{
		Cancel();		
		}

#if defined(SYMBIAN_LOCATION_PRIVACY_V2)
	delete iCloseDownRequestor;
#endif // SYMBIAN_LOCATION_PRIVACY_V2
	}
  
void CLbsPrivacyControllerImpl ::ConstructL()
	{
	CActiveScheduler::Add(this);
	
	/**
	 * See the ServerFramework code for the meaning of this code
	 */
	iParams.SetServerFileName(KNrhServerFileName);
	iParams.SetServerName(KNrhServerName);
	iParams.SetNumberOfServerSlots(-1); // Use messages from global pool
	iParams.SetServerUids(KNullUid, KNullUid, KLbsNetRequestHandlerUid);
	
	TInt err;
	err = iServer.Connect(iParams);
	User::LeaveIfError(err);
	
	TNrhPrivacyControllerData data;
	data.iVersion = iObserver->Version();
	TNrhSubSessionType type(ENrhSubSessionPrivacyController);
	err = iSession.Open<TNrhSubSessionType, TNrhPrivacyControllerData>(
						iServer, 
						type,
						data);
	User::LeaveIfError(err);	
	
	// Issue an outstanding request to be told of a privacy query.
	iSession.RegisterForServerMessages(iStatus);
	SetActive();

#if defined(SYMBIAN_LOCATION_PRIVACY_V2)
	// If the privacy controller application is using the 
	// MLbsPrivacyObserver2 observer, then register for 
	// close down events from LbsRoot.
	if (iObserver->Version().iMajor == KLbsPrivacyObserverVersion2Major)
		{
		iCloseDownRequestor = CLbsCloseDownRequestDetector::NewL(
								this,
								RProcess().SecureId());
		}
#endif // SYMBIAN_LOCATION_PRIVACY_V2
	}


/**
 * Respond to a privacy query
 */	
void CLbsPrivacyControllerImpl::RespondNetworkLocationRequest(TUint aRequestId, 
			CLbsPrivacyController::TRequestVerificationResult aResult)
	{
	// Send the data to the server.
	
	// Set the session number even if it's not the one we received. 
	// The NRH will detect any discrepancy.
	//
	// This does though take away most of the point of changing the session Id
	// from being a TUint to a TLbsNetSessionIdInt where it also has the UID of
	// the originator - due to the privacy controller API being published, 
	// the UID gets lost, and it has to be assumed when a response is received that 
	// it is the same as when we sent it.
	iSessionId.SetSessionNum(aRequestId);
	
	iSession.SendPrivacyResponse(iSessionId,aResult);
	}
/**
 * Rejects a privacy query
 */	
void CLbsPrivacyControllerImpl::CancelNetworkLocationRequest(TUint aRequestId)
	{
	iSessionId.SetSessionNum(aRequestId);
	
	iSession.SendPrivacyCancel(iSessionId);	
	}

/**
 * Called when the outstanding request is completed. 
 * This indicates that a message has been received from the server,
 * so the type is identified and the data interpreted and 
 * handled accordingly.
 * The request is then reisuued.
 */
void CLbsPrivacyControllerImpl::RunL()
	{

	TInt serverResponseType = iStatus.Int();
			
	switch(serverResponseType)
		{
		case ENrhServerMessageAck:
		// Do nothing - just acknowledges a request.
			{
			break;
			}
		
		case ENrhServerPrivacyRequest:
			{
			HandlePrivacyRequest();
			break;
			}
		
		case ENrhServerLocationUpdate:
			{
			HandleLocationUpdate();
			break;
			}
		
		case ENrhServerLbsSessionComplete:
			{
			HandleLbsSessionComplete();
			break;
			}		
		}
		
											
	// Wait for another request
	// Unless the error is KErrServerTerminated, which
	// means the NRH has shut down.
	if (serverResponseType != KErrServerTerminated)
		{
		iSession.RegisterForServerMessages(iStatus);
		SetActive();	
		}
	}
	
void CLbsPrivacyControllerImpl::DoCancel()
	{
	}	
	
/**
 * Called when a privacy request has been received.
 * Find out what sort of request it is, extract the data
 * which specifies how to inform the user, and pass it on to the observer
 */	
void CLbsPrivacyControllerImpl::HandlePrivacyRequest()
	{

	
	TPrivacyRequestParams privacyRequestParams;
	privacyRequestParams = iSession.iPrivacyRequestParamsBuf();
	iSessionId = privacyRequestParams.iSessionId;
	
	/*
	 * Need to translate the notification and timeout parameters from TLbsNetPosRequestPrivacyInt
	 * values to a TNotificationType value.
	 */
	MLbsPrivacyObserver::TNotificationType notificationType;
	TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt requestAdvice = 
						privacyRequestParams.iRequestPrivacy.RequestAdvice();
	TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt requestAction = 
						privacyRequestParams.iRequestPrivacy.RequestAction();
		
	
	switch(requestAdvice)
		{
		case TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify:
		case TLbsNetPosRequestPrivacyInt::ERequestAdviceSilent:
			{
			if(requestAction == TLbsNetPosRequestPrivacyInt::ERequestActionAllow)
				{
				notificationType = MLbsPrivacyObserver::ENotifyLocationAccepted;
				}
			else
				{
				// Covers the 'unused' option as well, though that should never
				// get this far
				notificationType = MLbsPrivacyObserver::ENotifyLocationRejected;					
				}
			break;
			}

		case TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify:
			{
			if(requestAction == TLbsNetPosRequestPrivacyInt::ERequestActionAllow)
				{
				notificationType = MLbsPrivacyObserver::ENotifyAndVerifyLocationAcceptedIfNoReponse;
				}
			else
				{
				// Covers the 'unused' option as well, though that should never
				// get this far
				notificationType = MLbsPrivacyObserver::ENotifyAndVerifyLocationRejectedIfNoReponse;
				}
			break;
			}

		default:
			{
			// No other value should get this far.
			User::Invariant();
			break;
			}
		}
	
	LBSLOG(ELogP9, "<-A MLbsPrivacyObserver::ProcessNetworkLocationRequest() lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TUint iSessionId.SessionNum()  = %u\n", iSessionId.SessionNum());
	LBSLOG2(ELogP9, "  > TNotificationType notificationType	  = %d\n", notificationType);
	LBSLOG(ELogP9, "  > TLbsExternalRequestInfo privacyRequestParams.iRequestInfo  =\n");
	LBSLOG_TLBSEXTERNALREQ(privacyRequestParams.iRequestInfo);
	iObserver->ProcessNetworkLocationRequest(iSessionId.SessionNum(), 
											privacyRequestParams.iRequestInfo, 
											notificationType);
	}	
	
/**
 * Called when a location update has been received.
 * Extract the position data, and pass it on to the observer
 */	
void CLbsPrivacyControllerImpl::HandleLocationUpdate()
	{
	TLocationUpdateParams locationUpdateParams;
	locationUpdateParams = iSession.iLocationUpdateParamsBuf();
	
	LBSLOG(ELogP9, "<-A MLbsPrivacyObserver::ProcessNetworkPositionUpdate() lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TUint locationUpdateParams.iSessionId.SessionNum()  = %u\n", locationUpdateParams.iSessionId.SessionNum());
	LBSLOG(ELogP9, "  > TPositionInfo locationUpdateParams.iPositionInfo  = \n");
	LBSLOG_TPOSITIONINFOBASE(locationUpdateParams.iPositionInfo);
	iObserver->ProcessNetworkPositionUpdate(
								locationUpdateParams.iSessionId.SessionNum(),
								locationUpdateParams.iPositionInfo);
	}
	
/**
 * Called when a session complete message has been received.
 * Extract the data, which consists of the session id and the
 * reason for completing the session, and pass it on to the observer
 */	
void CLbsPrivacyControllerImpl::HandleLbsSessionComplete()
	{
	TLbsSessionCompleteParams sessionCompleteParams;
	sessionCompleteParams = iSession.iLbsSessionCompleteParamsBuf();
	
	LBSLOG(ELogP9, "<-A MLbsPrivacyObserver::ProcessRequestComplete() lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TUint sessionCompleteParams.iSessionId.SessionNum()  = %u\n", sessionCompleteParams.iSessionId.SessionNum());
	LBSLOG2(ELogP9, "  > TInt sessionCompleteParams.iReason	  = %d\n", sessionCompleteParams.iReason);
	iObserver->ProcessRequestComplete(
								sessionCompleteParams.iSessionId.SessionNum(),
								sessionCompleteParams.iReason);
	}

/** Callback from LbsRoot to tell this process to stop.

CLbsPrivacyControllerImpl should only register for this shutdown callback
if the MLbsPrivacyObserver2 version of the privacy controller observer
was used.

This callback will be triggered by LbsRoot when it is shutting down the 
LBS system. We must call ProcessCloseDownNotification to tell the 
privacy controller application to shutdown.
*/
void CLbsPrivacyControllerImpl::OnProcessCloseDown()
	{
#if defined(SYMBIAN_LOCATION_PRIVACY_V2)
	MLbsPrivacyObserver2* observer = static_cast<MLbsPrivacyObserver2*>(iObserver);
	observer->ProcessCloseDownNotification(KErrNone);
#endif // SYMBIAN_LOCATION_PRIVACY_V2
	}

