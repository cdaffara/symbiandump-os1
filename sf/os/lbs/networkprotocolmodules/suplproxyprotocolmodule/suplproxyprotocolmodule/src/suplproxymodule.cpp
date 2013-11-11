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
// suplproxymodule.cpp
// This file provides the class implementation of the ECOM plug-in 
// interface for the LBS Supl Proxy Protocol Module.
// 
//

#include <ecom/implementationproxy.h>
#include "lbsdevloggermacros.h"
#include <lbs/lbsadmin.h>
#include <lbscommon.h>

#include <lbspositioninfo.h>
#include "suplproxymodule.h"
#include "lbssuplproxyserverreturncodes.h"
#include <lbs/lbsnetclasstypes.h>  
#include <lbs/epos_eomasuplposerrors.h>

#include <lbs/lbsassistancereferencelocation.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <centralrepository.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbssatellite.h>

/** The unique ID of this plug-in implementation.
 This corresponds to the implementation UID specified in the .rss file
 for this protocol module.
 */
const TInt KPluginUidValue = 0x102871EC;

//Uid of the supl proxy protocol module dll
const TInt KSPPModuleDllUid = 0x102871ED;

/** The UID for the central repository file used by the supl proxy protocol module */
const TUid KLbsSuplProxyProtocolModuleCenRepUid = {0x102871ED};

/** The entry in cen rep where the reference location source UID is placed */
const TUint32 KLbsRefLocationSource = 0x00000001;

/** The entry in cen rep where the TI plugin UID is placed */
const TUint32 KLbsTiPluginImpl = 0x00000002;

/** 
 Static public constructor.

 @param aObserver Plug-in parameters, which includes a reference to this protocol module's observer.
 @return A new instance of the CSuplProxyProtocol class.
 */
CSuplProxyProtocol* CSuplProxyProtocol::NewL(TAny* aParam)
    {
    LBSLOG(ELogP1, "CSuplProxyProtocol::NewL() Begin\n");

    TLbsNetProtocolModuleParams* param =
            reinterpret_cast<TLbsNetProtocolModuleParams*> (aParam);
    CSuplProxyProtocol* self = new (ELeave) CSuplProxyProtocol(static_cast<MLbsNetworkProtocolObserver2&>(param->Observer()));
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    LBSLOG(ELogP1, "CSuplProxyProtocol::NewL() End\n");

    return self;
    }

/** 
 Standard destructor.
 */
CSuplProxyProtocol::~CSuplProxyProtocol()
    {
    LBSLOG(ELogP1, "CSuplProxyProtocol::~CSuplProxyProtocol() Begin\n");
    delete iRequestBuffer;
    delete iServer;
    if (iSuplTiApiWrapper)
        {
        iSuplTiApiWrapper->CloseSession();
        delete iSuplTiApiWrapper;
        }
    iLbsNetSessions.ResetAndDestroy();
    LBSLOG(ELogP1, "CSuplProxyProtocol::~CSuplProxyProtocol() End\n");
    }

/** 
Request to cancel a previous privacy verification request.

@param aCancelInfo The information required to cancel the request.
*/	
void CSuplProxyProtocol::CancelVerifyLocationRequest(CCancelSuplProxyPrivacyRequestInfo* aCancelInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::CancelVerifyLocationRequest() Begin\n");

	TInt requestId = aCancelInfo->GetRequestId();
	TInt cancelReason = aCancelInfo->CancelReason();

	// Complete the session
	CSuplProxyPrivacyRequestInfo* request = iRequestBuffer->FindRequest(requestId);
	if (request)
		{
		TLbsNetSessionId sessionId(TUid::Uid(KPluginUidValue),requestId);
		LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", sessionId.SessionNum());
		iObserver.ProcessSessionComplete(sessionId,cancelReason);	
		request->SetIsSessionCompleteAutomatic(EFalse);
		}
	
	// Cancel any waiting requests with this ID in the buffer
	iRequestBuffer->CancelAnyOutstandingRequestById(requestId, cancelReason);

    //Remove the positioning session object from the list. 
    //[added during a privacy session to help differentiate an MTLR &
    //triggered session at a later stage]
    TInt index = iLbsNetSessions.Find(requestId,CLbsNetSession::IsSessionNumMatch);
    if (index >= 0) //has already been added during an earlier verifylocationrequest
        {
        CLbsNetSession* netSession = iLbsNetSessions[index];
        LBSLOG2(ELogP1, "CSuplProxyProtocol::CancelVerifyLocationRequest() Removing sppm session object 0x%x to array\n", requestId);
        iLbsNetSessions.Remove(index);
        delete netSession;
        }
    
	// We're done with this now, so delete
	delete aCancelInfo;
	
	LBSLOG(ELogP1, "CSuplProxyProtocol::CancelVerifyLocationRequest() End\n");
	}


/** 
Request to get the next session ID to use for a new privacy request.

@param aSessionId Reference to a TLbsNetSessionId to populate with the new session ID.
*/	
void CSuplProxyProtocol::GetNextSessionId(TLbsNetSessionId& aSessionId)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::GetNextSessionId() Begin\n");

	aSessionId = iNextSessionId;
	if (iNextSessionId.SessionNum() == KMaxTInt)
		{
		iNextSessionId.SetSessionNum(0);
		}
	iNextSessionId.IncrSession();
	LBSLOG(ELogP1, "CSuplProxyProtocol::GetNextSessionId() End\n");
	}


/** 
Send a privacy notification request to the LBS sub-system.

@param aRequestInfo The information about this request.
*/	
void CSuplProxyProtocol::NotifyLocationRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifyLocationRequest() Begin\n");
	ProcessRequest(aRequestInfo);
	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifyLocationRequest() End\n");
	}

/** 
Notifies the LBS sub-system that a previous privacy verification request timed out in the network.

@param aRequestInfo The information about this request.
*/	
void CSuplProxyProtocol::NotifyVerificationTimeout(CSuplProxyPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifyVerificationTimeout() Begin\n");

	ProcessRequest(aRequestInfo);

	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifyVerificationTimeout() End\n");
	}

/** 
Sends a privacy verification request to the LBS sub-system.

@param aRequestInfo The information about this request.
*/	
void CSuplProxyProtocol::VerifyLocationRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::VerifyLocationRequest() Begin\n");

	ProcessRequest(aRequestInfo);

	LBSLOG(ELogP1, "CSuplProxyProtocol::VerifyLocationRequest() End\n");
	}

/** 
Sends a privacy request to the LBS sub-system.

@param aRequestInfo The information about this request.
*/	
void CSuplProxyProtocol::PrivacyLocationRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::PrivacyLocationRequest() Begin\n");

	ProcessRequest(aRequestInfo);

	LBSLOG(ELogP1, "CSuplProxyProtocol::PrivacyLocationRequest() End\n");
	}

/**
Completes the privacy request session. This is part of the call made from the client side, so 
The client gets notified of this.

@param aRequestId The session of the privacy request to be completed
@param aReason The reason for which is completed
*/
void CSuplProxyProtocol::CompletePrivacyRequest(TUint32 aRequestId, 
												TInt aReason)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::CompletePrivacyRequest() Begin\n");
	
	CSuplProxyPrivacyRequestInfo* request = iRequestBuffer->FindRequest(aRequestId);
	if (request)
		{
		// Need to complete the message for LBS
		TLbsNetSessionId sessionId(TUid::Uid(KPluginUidValue),aRequestId);

		if(aReason != KErrNone)	// only terminate the session in anomalous situations. In other cases we expect a location request
			{
            LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", sessionId.SessionNum());
			iObserver.ProcessSessionComplete(sessionId,aReason);	 
			request->SetIsSessionCompleteAutomatic(EFalse);
			
			//Remove the positioning session data object - incase it had been added
			//[added during a privacy session to help differentiate an MTLR &
			//triggered session at a later stage]
           TInt requestId = request->GetRequestId();
           TInt index = iLbsNetSessions.Find(requestId, CLbsNetSession::IsSessionNumMatch);
           if (index >= 0)
               {
               CLbsNetSession* netSession = iLbsNetSessions[index];
               LBSLOG2(ELogP1, "CSuplProxyProtocol::CompletePrivacyRequest() Removing sppm session object 0x%x to array\n", aRequestId);
               iLbsNetSessions.Remove(index);
               delete netSession;               
               }//if
			}//if
		
		// Complete the request if it is awaiting a response.

		if (request->IsResponseRequired())
			{
			request->CompleteRequest(KRequestCancelledByClient);
			}
		
		// Remove the request and delete
		iRequestBuffer->RemoveRequest(request);
		delete request;
		}

	LBSLOG(ELogP1, "CSuplProxyProtocol::CompletePrivacyRequest() End\n");
	}

/**
Signals that a client has disconnected a privacy session from the server.

Normally the disconnect will be because the client has closed the privacy session, but it may also 
be because the client panicked. 

We must clean up any outstanding requests the client had here.

@param aSession Session object for the client that disconnected.
*/
void CSuplProxyProtocol::PrivacySessionDisconnected(const CSession2* aSession, TInt aReason)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::PrivacySessionDisconnected() Begin\n");

	// Complete the privacy session
	CSuplProxyPrivacyRequestInfo* request = iRequestBuffer->FindRequest(aSession);
	if (request)
		{
		TInt requestId = request->GetRequestId();
		TLbsNetSessionId sessionId(TUid::Uid(KPluginUidValue),requestId);

		if(aReason != KErrNone) // only terminate the session in anomalous situations. In other cases we expect a location request
	            {
                LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", sessionId.SessionNum());
	            iObserver.ProcessSessionComplete(sessionId,aReason);     
	            request->SetIsSessionCompleteAutomatic(EFalse);
	            
	            //Remove the positioning session data object - incase it had been added
	            //[added during a privacy session to help differentiate an MTLR &
	            //triggered session at a later stage]
	           TInt index = iLbsNetSessions.Find(requestId, CLbsNetSession::IsSessionNumMatch);
	           if (index >= 0)
	               {
	               CLbsNetSession* netSession = iLbsNetSessions[index];
	               LBSLOG2(ELogP1, "CSuplProxyProtocol::PrivacySessionDisconnected() Removing sppm session object 0x%x to array\n", requestId);
	               iLbsNetSessions.Remove(index);
	               delete netSession;	              
	               }//if
	            }//if
		}
	
	// Check if any buffered requests need to be cancelled
	iRequestBuffer->CancelAnyOutstandingRequestBySession(aSession);

    iRequestBuffer->RemoveRequest(request);
    delete request;
	LBSLOG(ELogP1, "CSuplProxyProtocol::PrivacySessionDisconnected() End\n");	
	}


void CSuplProxyProtocol::EndPrivacySession(TInt aRequestId, TInt aReason)
	{
	TLbsNetSessionId sessionId(TUid::Uid(KPluginUidValue),aRequestId);
	LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", sessionId.SessionNum());
	iObserver.ProcessSessionComplete(sessionId,aReason);
	}

/** 
Called by the Network Gateway in response to our privacy requests.

@param aSessionId The ID of the session.
@param aResponse Response information for this privacy session.
*/	
void CSuplProxyProtocol::RespondPrivacyRequest(const TLbsNetSessionId& aSessionId, const TLbsPrivacyResponse& aResponse, TInt /*aReason*/)
	{
	
	LBSLOG(ELogP1, "CSuplProxyProtocol::RespondPrivacyRequest() Begin\n");
	
	// Locate the request in the buffer
	CSuplProxyPrivacyRequestInfo* request = iRequestBuffer->FindRequest(aSessionId.SessionNum());
	if (request)
		{
		// Complete the active request if it is awaiting a response
		if (request->IsResponseRequired())
			{
			// If this is a verification being accepted, timestamp the positioning session object. 
			// If a notification arrives shortly from the same requestor we will not create a new positioning session object
			// since that notification is simply the SUPL FW's post-verification notification for the same privacy request.
			TInt index = iLbsNetSessions.Find(aSessionId,CLbsNetSession::IsSessionMatch);
            if (index >= 0) 
                {
                TTime currentTime;
                currentTime.UniversalTime();
                CLbsNetSession* netSession = iLbsNetSessions[index];
                netSession->SetResponseTimeStamp(currentTime);
                }
            
			if(request->ConvertResponseCode())
				{
                request->CompleteRequest((aResponse == EPrivacyResponseAccepted) ? KErrNone : KErrAccessDenied);
				}
			else
				{
				request->CompleteRequest(aResponse);
				}
			// Only need to respond once, so clear the flag.
			request->SetIsResponseRequired(EFalse);
			}
		
		// If privacy request was not accepted then close the LBS session
		if(request->IsSessionCompleteAutomatic() && (EPrivacyResponseAccepted != aResponse))
			{
			// Notify network gateway that this session is complete
            LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", aSessionId.SessionNum());
			iObserver.ProcessSessionComplete(aSessionId, KErrNone);		
			request->SetIsSessionCompleteAutomatic(EFalse);

			// Remove the request and delete
			iRequestBuffer->RemoveRequest(request);
			delete request;
			
	        //Remove the positioning session object from the list. 
            //[added during a privacy session to help differentiate an MTLR &
            //triggered session at a later stage]
			TInt index = iLbsNetSessions.Find(aSessionId,CLbsNetSession::IsSessionMatch);
            if (index >= 0) 
                {
                CLbsNetSession* netSession = iLbsNetSessions[index];
                LBSLOG2(ELogP1, "CSuplProxyProtocol::RespondPrivacyRequest() Removing sppm session object 0x%x to array\n", aSessionId);
                iLbsNetSessions.Remove(index);
                delete netSession;                
                }
            }
		}
	else
		{
		LBSLOG(ELogP2, "CSuplProxyProtocol::RespondPrivacyRequest() Unexpected response\n");
		}

	LBSLOG(ELogP1, "CSuplProxyProtocol::RespondPrivacyRequest() End\n");
	}

//---------------------------------------------------------------------------
// SPI methods for Positioning Requests:
//---------------------------------------------------------------------------

/**
Send a response to a location request to the network.
@see CLbsNetworkProtocolBase::RespondLocationRequest()
*/
void CSuplProxyProtocol::RespondLocationRequest(const TLbsNetSessionId& aSessionId, TInt aReason, 
												const TPositionInfoBase& aPosInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::RespondLocationRequest() Begin\n");

	// This class needs to be able to deal with multiple sessions (MOLR/MTLR) at once 
	// and each session can have an outstanding request 
	// we need to complete the right one here =)
	
    // Find and complete the outstanding location request
    TInt index = iLbsNetSessions.Find(aSessionId,CLbsNetSession::IsSessionMatch);
    if (index >= 0)
        {
        LBSLOG(ELogP1, "CSuplProxyProtocol - found matching request\n");
        
        CLbsNetSession* netSession = iLbsNetSessions[index];
        
        switch (netSession->Type())
            {
            case CLbsNetSession::EPosRequestTypeGetPosition:
                {
                LBSLOG(ELogP1, "Outstanding EPosRequestTypeGetPosition request\n");
                // assert there really is a message to complete
                __ASSERT_DEBUG(!netSession->Msg().IsNull(),User::Invariant());      
                if(aReason == KErrNone)
                    {
					// Fill in what we have of the position structure that was passed to us
					TInt clientPosSize = netSession->Msg().GetDesLength(0);
					__ASSERT_DEBUG(clientPosSize >= 0,User::Invariant()); 
					TInt posInfoSize = aPosInfo.PositionClassSize();
					TInt copySize = (clientPosSize < posInfoSize? clientPosSize : posInfoSize);
					if(copySize > 0)
					    {
                        HBufC8* buffer = HBufC8::New(copySize);
                        if(buffer)
                            {
                            TPtr8 ptr = buffer->Des();
                            const TUint8* FromAddr = reinterpret_cast<const TUint8*>(&aPosInfo);
                            ptr.Copy(FromAddr, copySize);
                            TInt err = netSession->Msg().Write(0, buffer->Des(), 0);	
                            __ASSERT_DEBUG(err == KErrNone, User::Invariant());
                            delete buffer;
                            }
					    }
                    }
                
                netSession->Msg().Complete(aReason);
                break;
                }
            case CLbsNetSession::EPosRequestTypeRrlpGetLocation:
                {
                LBSLOG(ELogP1, "Outstanding EPosRequestTypeRrlpGetLocation request\n");
                __ASSERT_DEBUG(netSession->PosProtocolResponseObserver()!=NULL,User::Invariant());
                // passed back to the appropriate subsession for encoding and returning to client in pospayload
                netSession->PosProtocolResponseObserver()->HandleRrlpLocationResponse(aReason, aPosInfo);
                break;
                }
            case CLbsNetSession::EPosRequestTypeSuplInfoAssData:
                {
                LBSLOG(ELogP1, "Outstanding EPosRequestTypeSuplInfoAssData request\n");
                // the location request was issued simply to receive the currently required Assistance Data Mask
                __ASSERT_DEBUG(netSession->PosProtocolResponseObserver()!=NULL,User::Invariant());                
                // inform the suplproxyinforetriever expecting an update of the required assistance data mask
                // of the error
                netSession->PosProtocolResponseObserver()->HandleSetAssistanceDataMask(aReason, netSession->GetAssistanceDataMask());
                break;
                }
            case CLbsNetSession::EPosRequestTypeNone:
                {
                LBSLOG(ELogP1, "Outstanding EPosRequestTypeNone request\n");
                // if no matching request is found we ignore it since the request 
                // may have been cancelled 
                break;
                }
            default:
                {
                LBSLOG(ELogP1, "Outstanding request bad type\n");
                // something has gone wrong
                __ASSERT_DEBUG(EFalse,User::Invariant());
                }
            } // switch (netSession->Type())

        // the outstanding request has been processed
        netSession->SetType(CLbsNetSession::EPosRequestTypeNone);
        } // if (index >= 0)

	LBSLOG(ELogP1, "CSuplProxyProtocol::RespondLocationRequest() End\n");

	}

/**
 Send a request for GPS assistance data to the network. LBS does not currently send valid infomation 
 in the sessionIdArray. 
 
 @see CLbsNetworkProtocolBase::RequestAssistanceData()
 */
void CSuplProxyProtocol::RequestAssistanceData(TLbsAsistanceDataGroup aDataRequestMask, const TLbsNetSessionIdArray& /* aSessionIdArray */)
    {
    LBSLOG(ELogP1, "CSuplProxyProtocol::RequestAssistanceData() Begin\n");

    if (aDataRequestMask != EAssistanceDataNone)
        {
        // if mask is non-zero and we have any outstanding location requests, 
        // need to complete them with KErrOMASuplPosInfo here.
        CompleteMatchingRequest(CLbsNetSession::EPosRequestTypeGetPosition,KErrOMASuplPosInfo);
        }
    //Store the details reg the latest assistance data request in all the session objects
    for (TInt index = iLbsNetSessions.Count() - 1; index >= 0; index--)
        {
        //Update the masks for all the sessions that have started a positioning session.
        //ie. all sessions except MTLRs that are currently processing privacy session.
        //This exception is required as we create an entry for MTLRs in iLbsNetSessions 
        //in advance during its priv session 
        if (iLbsNetSessions[index]->PosSessionStarted())
            {
            //reset the flag relating to the outstanding assistance data info from LBS
            if (iLbsNetSessions[index]->Type() == CLbsNetSession::EPosRequestTypeSuplInfoAssData)
                {
                iLbsNetSessions[index]->SetType(CLbsNetSession::EPosRequestTypeNone);
                }
            //Update all the sessions with the latest assistance data requested.
            iLbsNetSessions[index]->SetAssistanceDataMask(aDataRequestMask);

            __ASSERT_DEBUG(iLbsNetSessions[index]->PosProtocolResponseObserver()!=NULL,User::Invariant());
            //Inform the observer (expecting a location response) that a fresh assistance data request has arrived.
            //The subsession inturn updates "CSuplProxySuplInfoRetriever" which uses this latest mask to complete the outstanding
            //GetSuplInfoL.
            iLbsNetSessions[index]->PosProtocolResponseObserver()->HandleSetAssistanceDataMask(
                    KErrNone, aDataRequestMask);
            }
        }
	LBSLOG(ELogP1, "CSuplProxyProtocol::RequestAssistanceData() End\n");
	}

/**
 RequestNetworkLocation called by the NetworkGateway to start a MOLR-TA/CellBased
 positioning.
 
 @params aSessionId The session id used to identify the queued request 
 @params aOptions   Parameters for MOLR-TA/CellBased
 @see CLbsNetworkProtocolBase::RequestNetworkLocation()
 @see CLbsSuplTiApi::RequestLocation()
 */
void CSuplProxyProtocol::RequestNetworkLocation(
        const TLbsNetSessionId& aSessionId,
        const TLbsNetPosRequestOptionsBase& aOptions)
    {
    LBSLOG(ELogP1, "CSuplProxyProtocol::RequestNetworkLocation() Begin\n");
	
	if(!iSuplTiApiWrapper)
    	{
		//Error, TI Plugin not available.  Complete the request with error
		LBSLOG_ERR(ELogP1, "Error, TI Plugin not available, not able to complete request");
		LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", aSessionId.SessionNum());
		iObserver.ProcessSessionComplete(aSessionId, KErrNotReady);
		return;
    	}

	const TLbsNetPosRequestOptions reqParams =
			static_cast<const TLbsNetPosRequestOptions&> (aOptions);
		
	CLbsNetSession* netSession = new CLbsNetSession(aSessionId, MLbsNetworkProtocolObserver2::EServiceNetworkLocation);
    if(netSession)
    	{
		// Store the requested quality parameter
		TLbsNetPosRequestQuality quality;
		reqParams.GetRequestQuality(quality);
		netSession->SetQuality(quality);
		
		// Add this request to the queue of outstanding requests
		LBSLOG2(ELogP1, "CSuplProxyProtocol::RequestNetworkLocation() Adding sppm session object 0x%x to array\n", aSessionId);
		TInt err = iLbsNetSessions.Append(netSession);
		if(KErrNone != err)
		    {
		    LBSLOG(ELogP1, "CSuplProxyProtocol::RequestNetworkLocation() - iLbsNetSessions.Append failed!!\n");
            delete netSession;
		    }
    	}

    //Create the prioritised list of positioning methods
    TLbsNetPosRequestMethod posReqMethods;
    TLbsNetPosMethod posMethod[2];
    //1. Terminal Assisted and 2. CellBased positioning
    posMethod[0].SetPosMethod(KLbsPositioningMeansGps,
            (TPositionModuleInfo::ETechnologyNetwork
                    | TPositionModuleInfo::ETechnologyAssisted));
    posMethod[1].SetPosMethod(KLbsPositioningMeansCell,
            TPositionModuleInfo::ETechnologyNetwork);
    posReqMethods.SetPosMethods(posMethod, 2);

    //Inform LBS of the start of an MOLR - TA/CellBased
    StatusUpdate(MLbsNetworkProtocolObserver2::EServiceNetworkLocation, ETrue);

    //Call the Terminal Initiation API
	LBSLOG(ELogP9, "->A   CLbsSuplTiApi::RequestLocation() SUPL-FW\n");
	LBSLOG(ELogP9, " > const TLbsNetSessionId aSessionId = \n");
	LBSLOG_SESSIONID(aSessionId);
    LBSLOG(ELogP9, " > TLbsNetPosRequestOptionsBase aOptions = \n");	
	LBSLOG_POSREQUESTOPTIONS(reqParams);	
    LBSLOG(ELogP9, " > TLbsNetPosRequestMethod aMethod = \n");
    LBSLOG_POSREQUESTMETHOD(posReqMethods);	
    iSuplTiApiWrapper->RequestLocation(aSessionId, aOptions, posReqMethods);

    LBSLOG(ELogP1, "CSuplProxyProtocol::RequestNetworkLocation() End\n");

    }


/**
Called by the NetworkGateway to cancel a MOLR-TA/CellBased request. Calls cancel on the TI 
interface and complete the queued request.
@params aSessionId The session id used to identify the queued request
@params aReason    Reason for cancelling the request

@see CLbsSuplTiApi::CancelRequest()
*/
void CSuplProxyProtocol::CancelNetworkLocation(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::CancelNetworkLocation() Begin\n");
	
	//Call the TI Api
    LBSLOG(ELogP9, "->A CLbsSuplTiApi::CancelRequest() SUPL-FW\n");
    LBSLOG(ELogP9, " > TLbsNetSessionId aSessionId = \n"); 
    LBSLOG_SESSIONID(aSessionId);       
	TInt err = iSuplTiApiWrapper->CancelRequest(aSessionId);
	
    TInt index = iLbsNetSessions.Find(aSessionId, CLbsNetSession::IsSessionMatch);
    if (index >= 0)
        {
        iLbsNetSessions[index]->SetSessionCompleteErr(aReason);
        }
	LBSLOG(ELogP1, "CSuplProxyProtocol::CancelNetworkLocation() End\n");
	}


/** 
 Request self locate called by the network gateway to start a MOLR-TB.
 
 @params aSessionId The session id used to identify the queued request 
 @params aOptions   Parameters for MOLR-TB
 @see CLbsNetworkProtocolBase::RequestSelfLocation()
 @see CLbsSuplTiApi::RequestLocation()
 */
void CSuplProxyProtocol::RequestSelfLocation(	
        const TLbsNetSessionId& aSessionId,
        const TLbsNetPosRequestOptionsBase& aOptions)
    {
    LBSLOG(ELogP1, "CSuplProxyProtocol::RequestSelfLocation() Begin\n");

    //Check before casting to the TLbsNetPosRequestOptionsAssistance derived class
    if (aOptions.ClassType() != ELbsNetPosRequestOptionsAssistanceClass)
        {
        LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", aSessionId.SessionNum());
        iObserver.ProcessSessionComplete(aSessionId, KErrNone);
        }
	 else if(!iSuplTiApiWrapper)
		{
		//Error, TI Plugin not available.  Complete the request with error
		LBSLOG_ERR(ELogP1, "Error, TI Plugin not available, not able to complete request");
		LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", aSessionId.SessionNum());
		iObserver.ProcessSessionComplete(aSessionId, KErrNotReady);
		}
    else
        {

        //Parameters for the SelfLocation request
        const TLbsNetPosRequestOptionsAssistance
                reqParams =
                        static_cast<const TLbsNetPosRequestOptionsAssistance&> (aOptions);
        //Determine the assistance data requested by LBS
        TLbsAssistanceDataGroup reqDataMask = reqParams.DataRequestMask();

        //If no assistance data is requested
        if (reqDataMask == EAssistanceDataNone)
            {
            LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", aSessionId.SessionNum());
            iObserver.ProcessSessionComplete(aSessionId, KErrNone);
            }
        else
            {
            TBool validPosMethod = EFalse;
            
            //Create the prioritised list of supported positioning methods
            TLbsNetPosRequestMethod posReqMethods;
            TLbsNetPosMethod posMethod[1];
            switch (reqParams.PosMode())
                {
                case (TPositionModuleInfo::ETechnologyTerminal
                        | TPositionModuleInfo::ETechnologyAssisted):
                case (TPositionModuleInfo::ETechnologyTerminal
                        | TPositionModuleInfo::ETechnologyAssisted
                        | TPositionModuleInfo::ETechnologyNetwork):
                    {
                    // 'Only TB' or 'TA combined with TB'
                    // Set the preferred mode to be TB
                    posMethod[0].SetPosMethod(
                            KLbsPositioningMeansGps,
                            (TPositionModuleInfo::ETechnologyTerminal
                                    | TPositionModuleInfo::ETechnologyAssisted));
                    posReqMethods.SetPosMethods(posMethod, 1);
                    validPosMethod = ETrue;
                    break;
                    }
                case (TPositionModuleInfo::ETechnologyNetwork
                        | TPositionModuleInfo::ETechnologyAssisted):
                case (TPositionModuleInfo::ETechnologyTerminal):
                    {
                    //'Only TA' or 'Only autonomous' 
                    LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", aSessionId.SessionNum());
                    iObserver.ProcessSessionComplete(aSessionId, KErrNone);
                    break;
                    }
                default:
                    {
                    LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", aSessionId.SessionNum());
                    iObserver.ProcessSessionComplete(aSessionId, KErrArgument);
                    }
                }

            if (validPosMethod)
                {
                CLbsNetSession* netSession = new CLbsNetSession(aSessionId,MLbsNetworkProtocolObserver2::EServiceSelfLocation);
                if(netSession)
                	{
					// Store the assistance data requested
					netSession->SetAssistanceDataMask(reqDataMask);
	
					// Store the requested quality parameter
					TLbsNetPosRequestQuality quality;
					reqParams.GetRequestQuality(quality);
					netSession->SetQuality(quality);
	
					// Store the positioning method
					netSession->SetPosMethod(posReqMethods);

	                // Add this request to the queue of outstanding requests
					LBSLOG2(ELogP1, "CSuplProxyProtocol::RequestSelfLocation() Adding sppm session object 0x%x to array\n", aSessionId);
	                TInt err = iLbsNetSessions.Append(netSession);
	                if(KErrNone != err)
	                    {
		    			LBSLOG(ELogP1, "CSuplProxyProtocol::RequestSelfLocation() - iLbsNetSessions.Append failed!!\n");
	                    delete netSession;
	                    return;
	                    }
                	}
                
                //Inform LBS of the start of an MOLR - TB 
                StatusUpdate(MLbsNetworkProtocolObserver2::EServiceSelfLocation, ETrue);

                //Call the Terminal Initiation API
                LBSLOG(ELogP9, " ->A CLbsSuplTiApi::RequestLocation() SUPL-FW\n");                
                LBSLOG(ELogP9, " > TLbsNetSessionId aSessionId = \n");
                LBSLOG_SESSIONID(aSessionId);
                LBSLOG(ELogP9, " > TLbsNetPosRequestOptionsBase aOptions = \n");                
                LBSLOG_REQUESTOPTIONSASSISTANCE(reqParams);   
                LBSLOG(ELogP9, " > TLbsNetPosRequestMethod aMethod = \n");
                LBSLOG_POSREQUESTMETHOD(posReqMethods); 

                iSuplTiApiWrapper->RequestLocation(aSessionId, aOptions,
                        posReqMethods);
                }
            }
        }
    LBSLOG(ELogP1, "CSuplProxyProtocol::RequestSelfLocation() End\n");
    }


/**
Called by the NetworkGateway to cancel a MOLR-TB request. Calls cancel on the TI 
interface and complete the queued request.
@params aSessionId The session id used to identify the queued request
@params aReason    Reason for cancelling the request

@see CLbsSuplTiApi::CancelRequest()
*/
void CSuplProxyProtocol::CancelSelfLocation(const TLbsNetSessionId& aSessionId,
							TInt aReason)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::CancelSelfLocation() Begin\n");
	//Call the TI Api

    LBSLOG(ELogP9, "->A CLbsSuplTiApi::CancelRequest() SUPL-FW\n");
    LBSLOG(ELogP9, " > TLbsNetSessionId aSessionId = \n");  
    LBSLOG_SESSIONID(aSessionId);       	
	TInt err = iSuplTiApiWrapper->CancelRequest(aSessionId);

    TInt index = iLbsNetSessions.Find(aSessionId, CLbsNetSession::IsSessionMatch);
    if (index >= 0)
        {
        iLbsNetSessions[index]->SetSessionCompleteErr(aReason);
        }
	LBSLOG(ELogP1, "CSuplProxyProtocol::CancelSelfLocation() End\n");
	}


/** Inform LBS of status update.
 This is called when a new operation starts or when a current operation ends.

 @param aService Indicates the type of service that has started or stopped
 @param aIsOperationStarting ETrue if operation is starting/EFalse otherwise
 */
void CSuplProxyProtocol::StatusUpdate(
        const MLbsNetworkProtocolObserver2::TLbsNetProtocolService& aService,
        const TBool& aIsOperationStarting)
    {
    LBSLOG(ELogP1, "CSuplProxyProtocol::StatusUpdate() Begin\n");
    if (aIsOperationStarting)
        {
        // The start of a new operation
        iActiveServiceMask |= aService;
        }
    else
        {
        // An operation has ended
        iActiveServiceMask &= ~aService;
        }

    iObserver.ProcessStatusUpdate(iActiveServiceMask);
    LBSLOG(ELogP1, "CSuplProxyProtocol::StatusUpdate() End\n");
    }

/**
Lbs System Status information.
@see CLbsNetworkProtocolBase::AdviceSystemStatus()
*/
void CSuplProxyProtocol::AdviceSystemStatus(TLbsSystemStatus /*aStatus*/)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::AdviceSystemStatus() Begin And End\n");
	}

/**
Send a request to send the handsets location to a third party to the network.
@see CLbsNetworkProtocolBase::RequestTransmitLocation()
*/
void CSuplProxyProtocol::RequestTransmitLocation(const TLbsNetSessionId& /*aSessionId*/,
								const TDesC& /*aDestination*/,
								TInt /*aPriority*/,
								const TLbsNetPosRequestOptionsBase& /*aOptions*/)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::RequestTransmitLocation() v2 Begin and End\n");
	}

/**
Tell the protocol module to end the current send location to third party request.
@see CLbsNetworkProtocolBase::CancelTransmitLocation()
*/
void CSuplProxyProtocol::CancelTransmitLocation(const TLbsNetSessionId& /*aSessionId*/, TInt /*aReason*/)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::CancelTransmitLocation() Begin and End\n");
	}


/** 
Default constructor.

@param aObserver Reference to this protocol module's observer.
*/
CSuplProxyProtocol::CSuplProxyProtocol(MLbsNetworkProtocolObserver2& aObserver) :
    iObserver(aObserver), iActiveServiceMask(
            MLbsNetworkProtocolObserver2::EServiceNone)
    {
    LBSLOG(ELogP1, "CSuplProxyProtocol::CSuplProxyProtocol Begin");

    // Seed the intial requestId from the current time.
    TTime now;
    now.UniversalTime();
    const TInt KPosRequestIdShift = 14;
    TInt64 requestId64 = now.Int64() >> KPosRequestIdShift;
    TUint low = I64LOW(requestId64);
    TInt requestId = ~(~low | 0x80000000);
    iNextSessionId = TLbsNetSessionId(TUid::Uid(KPluginUidValue),
    								  requestId);
    
    LBSLOG(ELogP1, "CSuplProxyProtocol::CSuplProxyProtocol End");
	}


/** 
Second stage private constructor.
*/  
void CSuplProxyProtocol::ConstructL()
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::ConstructL() Begin\n");
	TInt err = KErrNone;

    CRepository* cenRep = CRepository::NewLC(KLbsSuplProxyProtocolModuleCenRepUid);
	TInt refLocSourceId(KErrNone);
    TInt result = cenRep->Get(KLbsRefLocationSource, refLocSourceId);
	if(KErrNone != result)
		{
		refLocSourceId = 0;
		}
    LBSLOG2(ELogP9, "  > TUid refLocSrcId  = 0x%08X\n", refLocSourceId);
	TInt suplTiPluginImplUid;
    User::LeaveIfError(cenRep->Get(KLbsTiPluginImpl, suplTiPluginImplUid));
	CleanupStack::PopAndDestroy(cenRep);

	iRequestBuffer = CSuplProxyPrivacyRequestBuffer::NewL();

	iServer = CSuplProxyServer::NewL(*this,*this,refLocSourceId); 

    LBSLOG(ELogP9, "->A   CLbsSuplTiApi::NewL() SUPL-FW\n");
    LBSLOG2(ELogP9, " > Uid = 0x%08X\n", suplTiPluginImplUid);
    TRAP(err, iSuplTiApiWrapper = CLbsSuplTiApi::NewL(*this, TUid::Uid(suplTiPluginImplUid)));
    if(err != KErrNone)
    	{
		LBSLOG_ERR2(ELogP1, "Failed to load the Terminal Initiation API Plugin (error: %d)", err);
    	}

	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);
		
	// Read admin setting for maximum number of external locate requests
	TUint maxExternalLocateRequests = KLbsDefaultMaximumExternalLocateRequests;
	err = admin->Get(KLbsSettingMaximumExternalLocateRequests, maxExternalLocateRequests);
	if (err != KErrNone)
		{
		LBSLOG_ERR2(ELogP4, "Failed to get KLbsSettingMaximumExternalLocateRequests (err %d)", err);
		}
	iMaxSimultaneousActiveRequests = maxExternalLocateRequests;	
	LBSLOG2(ELogP1, "Using KLbsSettingMaximumExternalLocateRequests = %d", iMaxSimultaneousActiveRequests);
	
	// Read admin setting for maximum number of network sessions
	TUint maxNetworkSessions = KLbsDefaultPrivacyMaxNetworkSessions;
	err = admin->Get(KLbsSettingPrivacyMaxNetworkSessions, maxNetworkSessions);
	if (err != KErrNone)
		{
		LBSLOG_ERR2(ELogP4, "Failed to get KLbsSettingPrivacyMaxNetworkSessions (err %d)", err);
		}
	LBSLOG2(ELogP1, "Using KLbsSettingPrivacyMaxNetworkSessions = %d", maxNetworkSessions);
	iServer->SetMaxNetworkSessions(maxNetworkSessions);
	
	
	CleanupStack::PopAndDestroy(admin);
	LBSLOG(ELogP1, "CSuplProxyProtocol::ConstructL() End\n");
	}

/** 
 Submit a privacy request for processing.

 Adds the request to the buffer, and starts processing it if possible.

 Note: There may be a limit on the number of requests allowed to be active simultaneously, if this
 limit is reached then the request will be buffered for processing later.

 @param aRequest The request info to process.
 */
void CSuplProxyProtocol::ProcessRequest(CSuplProxyPrivacyRequestInfo* aRequest)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::ProcessRequest() Begin\n");

	if (iRequestBuffer->NumRequests() >= iMaxSimultaneousActiveRequests)
		{
		// Maximum number of requests has been reached
		LBSLOG_WARN(ELogP1, "CSuplProxyProtocol::ProcessRequest() Request limit reached\n");
		if (aRequest->IsResponseRequired())
			{
			aRequest->CompleteRequest(KErrServerBusy);
			}
		delete aRequest;
		return;
		}

	// Add the request to the buffer
	TRAPD(err, iRequestBuffer->AddRequestL(aRequest));
	if (err != KErrNone)
		{
		// A problem occured and the request could not be added to the buffer
		LBSLOG_WARN2(ELogP1, "CSuplProxyProtocol::ProcessRequest() Error buffering request (%d)\n", err);
		if (aRequest->IsResponseRequired())
			{
			aRequest->CompleteRequest(err);
			}
		delete aRequest;
		return;
		}

	// Send the request to the network gateway
	LBSLOG(ELogP1, "CSuplProxyProtocol::ProcessRequest() Sending request\n");

	TInt requestId = aRequest->GetRequestId();
	TLbsNetSessionId sessionId(TUid::Uid(KPluginUidValue),requestId);

	//Create a positioning session data object here to help differentiate an MTLR against
	//a triggered session at a later stage.
    TInt index = iLbsNetSessions.Find(requestId, CLbsNetSession::IsSessionNumMatch);
    if (index < 0)  // we don't know about this subsession yet. 
        { 
		TLbsExternalRequestInfo2 extReqInfo;
		aRequest->GetRequestInfo(extReqInfo);       
        // If we've had a verification accepted recently from the same requester 
        // then we assume that it is just a post-verification notification 
        // and don't create another positioning session object
        if(aRequest->IsResponseRequired() || !HasRecentMatchingVerificationResponse(extReqInfo))
        	{
			CLbsNetSession* netSession = new CLbsNetSession(sessionId, MLbsNetworkProtocolObserver::EServiceMobileTerminated);
			if(netSession)
				{
				netSession->SetExtRequestInfo(extReqInfo);
				LBSLOG2(ELogP1, "CSuplProxyProtocol::ProcessRequest() Adding sppm session object 0x%x to array\n", sessionId);
				TInt err = iLbsNetSessions.Append(netSession);
				if (err != KErrNone)
					{
					// A problem occured and the request could not be added to the buffer
					LBSLOG_WARN2(ELogP1, "CSuplProxyProtocol::ProcessRequest() - iLbsNetSessions.Append failed!! - (%d)\n", err);
					if (aRequest->IsResponseRequired())
						{
						aRequest->CompleteRequest(err);
						}
                    delete netSession;
					delete aRequest;
					return;
					}
				}
        	}
        }

	TLbsNetPosRequestPrivacy requestPrivacy;
	aRequest->GetRequestPrivacy(requestPrivacy);
			
	TLbsExternalRequestInfo2 requestInfo;
	aRequest->GetRequestInfo(requestInfo);
	
	// force any requests with a "silent" request type to have request advice: silent
	if (requestInfo.RequestType() >= TLbsExternalRequestInfo::ERequestSingleShotSilent && 
	    requestInfo.RequestType() <= TLbsExternalRequestInfo::ERequestAreaEventSilent &&
	    requestPrivacy.RequestAdvice() != TLbsNetPosRequestPrivacy::ERequestAdviceSilent)
	    {
	    LBSLOG(ELogP1, "CSuplProxyProtocol::ProcessRequest() Forcing request advice = Silent\n");
	    requestPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent);
	    }
	
#if 0
    LBSLOG(ELogP7, "->A MLbsNetworkProtocolObserver::ProcessPrivacyRequest(sessionId, EFalse, requestPrivacy, requestInfo) LbsSuplProxyProtocolModule\n");
    LBSLOG2(ELogP7, "\t < TLbsPrivacyRequestId sessionId = %d\n", aRequest->GetRequestId());
    LBSLOG(ELogP7, "\t < TLbsNetPosRequestPrivacy requestPrivacy = \n");
    LBSLOG2(ELogP7, "\t\t < TLbsRequestAdvice iAdvice = %d\n", requestPrivacy.RequestAdvice());
    LBSLOG2(ELogP7, "\t\t < TLbsRequestAction iAction = %d\n", requestPrivacy.RequestAction());
    LBSLOG(ELogP7, "\t < TLbsExternalRequestInfo2 requestInfo = \n");

    TBuf16<KLbsMaxRequesterIdSize2 - KLbsMaxRequesterIdSize/2> requesterId; // Same length as TLbsExternalRequestInfo2::iRequesterIdExt
    requestInfo.GetRequesterId(requesterId);
    LBSLOG2(ELogP7, "\t\t < TLbsRequesterId iRequesterId = %S\n", &requesterId);
       
    TBuf16<KLbsMaxClientNameSize2 - KLbsMaxClientNameSize/2> clientName; // / Same length as TLbsExternalRequestInfo2::iClientNameExt
    requestInfo.GetClientName(clientName);
    LBSLOG2(ELogP7, "\t\t < TLbsClientName iClientName = %S\n", &clientName);

    TBuf8<KLbsMaxClientExternalIdSize> clientExternalId;
    requestInfo.GetClientExternalId(clientExternalId);
    LBSLOG2(ELogP7, "\t\t < TLbsClientExternalId iClientExternalId = %S\n", &clientExternalId);
    
    LBSLOG2(ELogP7, "\t\t < TRequestType iRequestType = %d\n", requestInfo.RequestType());
    LBSLOG2(ELogP7, "\t\t < TRequestSource iRequestSource = %d\n", requestInfo.RequestSource());
    LBSLOG2(ELogP7, "\t\t < TNetworkType iNetworkType = %d\n", requestInfo.NetworkType());
    LBSLOG2(ELogP7, "\t\t < TFormatIndicator iRequesterFormat = %d\n", requestInfo.RequesterIdFormat());
    LBSLOG2(ELogP7, "\t\t < TFormatIndicator iClientFormat = %d\n", requestInfo.ClientNameFormat());
//  LBSLOG2(ELogP7, "\t\t < TCodingScheme iRequesterCoding = %d\n", requestInfo.RequesterIdCodingScheme());
//  LBSLOG2(ELogP7, "\t\t < TCodingScheme iClientCoding = %d\n", requestInfo.ClientNameCodingScheme());
#endif

    LBSLOG2(ELogP1, "-->ProcessPrivacyRequest(0x%x)\n", sessionId.SessionNum());
    iObserver.ProcessPrivacyRequest(sessionId, EFalse, requestPrivacy, requestInfo);
    
    LBSLOG(ELogP1, "CSuplProxyProtocol::ProcessRequest() End\n");
	}

/** Complete the Session following the receipt of SUPL_END from the network
 @param aRequestId - The ID from the subsession [Used to identify the netsession from the stored list]
 @see   MLbsNetworkProtocolObserver2
 */
TInt CSuplProxyProtocol::NotifySessionEnd(TInt aRequestId)
    {
	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifySessionEnd() Begin\n");

    TInt err = KErrNone;

    TInt index = iLbsNetSessions.Find(aRequestId,
            CLbsNetSession::IsSessionNumMatch);
    if (index >= 0)
        {
        CLbsNetSession* netSession = iLbsNetSessions[index];

        //Send ProcessSessionComplete to LBS
        if (netSession->Service()
                != MLbsNetworkProtocolObserver2::EServiceNetworkLocation)
            {
            //Complete sessions other than Terminal assisted / cellbased
            err = CompleteMatchingSession(aRequestId, netSession->SessionCompleteErr());
            }
        else
            {
            //For Terminal Assisted / CellBased request [ie. corresp to RequestNetworkLocation()]
              if (netSession->SessionCompleteStatus())
                  {
                  //The Position has been obtained through the TI API. So
                  //send ProcessSessionComplete to LBS.
                  err = CompleteMatchingSession(aRequestId, netSession->SessionCompleteErr());                  
                  }
              else
                  {
                  //Position through the TI Api not received yet. So wait
                  //until we receive RequestComplete from TI API to send ProcessSessionComplete to LBS.
                  netSession->SetSessionCompleteStatus(ETrue);
                  }//if
            }//if(netSession->Service()..)
        }//if(index >= 0)

	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifySessionEnd() End\n");

    return err;
    }

/** 
 * 
 */
TInt CSuplProxyProtocol::NotifySubSessionClose(TInt aRequestId)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifySubSessionClose() Begin\n");
    
	TInt err = KErrNotFound;
  
    TInt index = iLbsNetSessions.Find(aRequestId, CLbsNetSession::IsSessionNumMatch);
    
    if (index >= 0)
        {
        //Remove the stored object containing the session details from the list
        CLbsNetSession* netSession = iLbsNetSessions[index];
        LBSLOG2(ELogP1, "CSuplProxyProtocol::NotifySubSessionClose() Removing sppm session object 0x%x to array\n", aRequestId);
        iLbsNetSessions.Remove(index);
        delete netSession;        
        err = KErrNone;
        }

	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifySubSessionClose() End\n");

    return err;
	}

/** 
 * SUPL FW has created a new subsession (MOLR / MTLR). 
 * In the case of SUPL 1.0 MOLR we will already be aware of it from RequestSelfLocate
 * In the case of MTLR we will already be aware of it from PrivacyRequest
@param aRequestId The request id that is used to identify the session.
@param aObserver  The handle to the observer (subsession) for relaying an lbs rrlp location response/new assistance data req.
 */
void CSuplProxyProtocol::NotifySubSessionOpen(MPosProtocolResponseObserver* aObserver, 		// lrm - now leaves, change name!
        TInt aRequestId)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifySubSessionOpen() Begin\n");

	// add this subsession to our list if it's not already there
    TInt index = iLbsNetSessions.Find(aRequestId, CLbsNetSession::IsSessionNumMatch);
    if (index < 0)	// we don't know about this subsession yet, must be a SUPL 2.0 triggered MOLR
        {
        TLbsNetSessionId sessionId;
        sessionId.SetSessionNum(aRequestId);
        sessionId.SetSessionOwner(TUid::Uid(KPluginUidValue));	// NG verifies that the owner passed in ProcessLocationRequest() is a valid PM UID.

        CLbsNetSession* netSession = new CLbsNetSession(sessionId, MLbsNetworkProtocolObserver2::EServiceTriggeredMolr);
        if(netSession)
        	{
			//Store the observer (subsession) for forwarding future requests/updates
			netSession->SetPosProtocolResponseObserver(aObserver);
			netSession->SetPosSessionStarted(ETrue);
			netSession->SetSessionStarted();
			LBSLOG2(ELogP1, "CSuplProxyProtocol::NotifySubSessionOpen() Adding sppm session object 0x%x to array\n", sessionId);
			TInt err = iLbsNetSessions.Append(netSession);
            if(KErrNone != err)
                   {
	    		   LBSLOG(ELogP1, "CSuplProxyProtocol::NotifySubSessionOpen() - iLbsNetSessions.Append failed!!\n");
                   delete netSession;
                   }
        	}
        //Update LBS of the currently active services including this new session type
        StatusUpdate(MLbsNetworkProtocolObserver2::EServiceTriggeredMolr,ETrue);
        }
    else
        {
        //Store the observer (subsession) for forwarding future requests/updates
        iLbsNetSessions[index]->SetPosProtocolResponseObserver(aObserver);
        iLbsNetSessions[index]->SetPosSessionStarted(ETrue);
        iLbsNetSessions[index]->SetSessionStarted();
        
        if (iLbsNetSessions[index]->Service()
                & MLbsNetworkProtocolObserver2::EServiceMobileTerminated)
            {
            //Update LBS of the currently active services including this new session type
            //Preceeding privacy request "present" => MTLR 
            StatusUpdate(MLbsNetworkProtocolObserver2::EServiceMobileTerminated,ETrue);
            }
        }

	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifySubSessionOpen() End\n");
	}

/** 
* SUPL FW is reusing an inactive (following SessionEnd()) pos session for a new SUPL session through 
* re-initialization. Note: SessionEnd doesn't delete the subsession object and hence reinitialization
* doesn't involve creating a new subsession object.  
@param aRequestId The request id that is used to identify the session.
@param aObserver  The handle to the observer (the reinitialized subsession) for relaying an lbs rrlp location response/new assistance data req.
 */
void CSuplProxyProtocol::NotifySubSessionReInit(MPosProtocolResponseObserver* aObserver, TInt aRequestId)
    {
	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifySubSessionReInit() Begin\n");

    //The only difference when the SUPL FW reuses an inactive session is that NotifySubSessionOpen wouldn't get called. 
    //So the steps done in NotifySubSessionOpen should also be done when CSuplProxyPosSubSession 
    //is reused.
    NotifySubSessionOpen(aObserver,aRequestId);

	LBSLOG(ELogP1, "CSuplProxyProtocol::NotifySubSessionReInit() End\n");

    }

/** Handle a direct (at SUPL level) location request from the network 
Send location request to protocol module's observer (lbs subsystem)
@param aMessage	contains a TPositionInfo& (arg 2, actually a TPositionSatelliteInfo) to be filled with requested position
@param aRequestId	the request id from the subsession
@see MLbsNetworkProtocolObserver2::ProcessLocationRequest() 
*/
TInt CSuplProxyProtocol::NetworkGetPosition(const RMessage2& aMessage, TInt aRequestId)	
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::NetworkGetPosition() Begin\n");

	TInt err = KErrNone;
	
    TInt index = iLbsNetSessions.Find(aRequestId, CLbsNetSession::IsSessionNumMatch);
    
    // Find the session in our list.
	if (index >= 0)
		{
		CLbsNetSession* netSession = iLbsNetSessions[index];
		
		TBool firstCall = !(netSession->GetPositionCalled());
		netSession->SetGetPositionCalled();
		
		// complete with error for SUPL 1.0 (should only be called for MOLR in SUPL 1.0, not MTLR)
		// SUPL 1.0 case is identified by the fact that SUPLFW has previously done a GetSuplInfoL(all data, inc assistance data)
		// whereas in SUPL 2.0 triggered case we will only have had a GetSuplInfoL(capabilities)		
		if(firstCall && netSession->AssistanceDataMaskRequested())
			{
			// SUPL 1.0, complete with KErrOMASuplPosInfo - position not returned
			aMessage.Complete(KErrOMASuplPosInfo);
			}
		else	
			{
			// SUPL 2.0 (triggered) MOLR/MTLR
			// store the msg for later completion
			netSession->SetMsg(aMessage);
			netSession->SetType(CLbsNetSession::EPosRequestTypeGetPosition);	
			
			// aSession - The Id of the location session within the LBS sub-system. Stored in the netsession when new sppm subsession opened (or before then when RequestSelfLocate called for SUPL 1.0 MOLR)
			// aEmergency - always False (emergencies always over control plane)
			// aService - type of location request eg EServiceSelfLocation. EServiceTriggeredMolr is used for SUPL 2.0 (triggered) MOLR requests
			// aQuality - The network-requested quality of service (QoS) of the location calculation 
			//			- for (triggered) MOLR 
			//			- for (triggered) MTLR
			//          - these are set to null values, indicating that the NRH must load QoP parameters from admin quality profiles.
			// aMethod 	- can work with the assumption that it is Terminal Based, as the SUPL Framework will only call GetPositionL in Terminal Based
			//			- should have already been set for session via SetPosMethodAndAllowedCapabilities() or in RequestSelfLocation() for SUPL 1.0 MOLR
			//			- default to TB if not
			
			TLbsNetPosRequestMethod method;
			method = netSession->PosMethod();
			if(method.NumPosMethods() == 0) // not set, use TB
				{
				TLbsNetPosMethod posMethods[1];
				posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);	
				method.SetPosMethods(posMethods, 1);
				}
			
			// specifying quality parameters that force NRH to load and use the appropriate quality profile
			TRealX defaultAccuracy;
            defaultAccuracy.SetNaN();           // NaN indicates that the NRH should load and use appropriate quality profile
			const TInt KDefaultMaxFixTime = 0;	// 0 indicates that the NRH should load and use appropriate quality profile
			const TInt KDefaultMaxAge = 0;		// 0 means we want a new position			
			TLbsNetPosRequestQuality quality;
			quality.SetMinHorizontalAccuracy(defaultAccuracy);		
			quality.SetMinVerticalAccuracy(defaultAccuracy);		
			quality.SetMaxFixAge(KDefaultMaxAge);					
			quality.SetMaxFixTime(KDefaultMaxFixTime);				
			
			LBSLOG2(ELogP1, "-->ProcessLocationRequest(0x%x)\n", netSession->SessionId());
			iObserver.ProcessLocationRequest(netSession->SessionId(), EFalse, netSession->Service(), quality, method);
			}
		}
	else	// we don't know anything about this session, complete with an error
		{
		err = KErrNotFound;
		}

	LBSLOG(ELogP1, "CSuplProxyProtocol::NetworkGetPosition() End\n");

	return err;
	}

/** Handle a location request from the network at the positioning protocol level
Send location request to protocol module's observer (lbs subsystem)
@param aRequestId	the request id from the subsession
@see MLbsNetworkProtocolObserver2::ProcessLocationRequest() 
*/
TInt CSuplProxyProtocol::RrlplLocationRequest(const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod, TInt aRequestId)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::RrlplLocationRequest() Begin\n");

	TInt err = KErrNone;
	// find the matching lbs net session
	TInt index = iLbsNetSessions.Find(aRequestId, CLbsNetSession::IsSessionNumMatch);
	
	if (index < 0)
		{
		err = KErrNotFound;	// no matching session for this requestid
		}
	else
		{
		CLbsNetSession* netSession = iLbsNetSessions[index];
		netSession->SetType(CLbsNetSession::EPosRequestTypeRrlpGetLocation);	

		__ASSERT_DEBUG(netSession->Service() != MLbsNetworkProtocolObserver2::EServiceNone, User::Invariant());
		LBSLOG2(ELogP1, "-->ProcessLocationRequest(0x%x)\n", netSession->SessionId());
		iObserver.ProcessLocationRequest(netSession->SessionId(), EFalse, netSession->Service(), aQuality, aPosMethod);
		}

	LBSLOG(ELogP1, "CSuplProxyProtocol::RrlplLocationRequest() End\n");

	return err;
	}
/** Send assistance data obtained from the network to the protocol module's observer (lbs subsystem).
*If reference location is present, retrieve it and send it to LBS through ProcessLocationUpdate. 
@param aDataReportMask The mask indicating the reported assistance data 	
@param aAssistanceData The assistance data to be returned (if aReason == KErrNone)
@param aReason     KErrNone [if the requested assistance data is available]/Error code otherwise
@param aRequestId  the request id used to search for the session
*/
void CSuplProxyProtocol::ProcessAssistanceData(
        const TLbsAsistanceDataGroup& aDataReportMask,
        const RLbsAssistanceDataBuilderSet& aAssistanceData, TInt aReason, TInt aRequestId)
    {
	LBSLOG(ELogP1, "CSuplProxyProtocol::ProcessAssistanceData() Begin\n");
    
     //To use the extended version of extended version of ProcessAssistanceData in "MLbsNetworkProtocolObserver2"  
     //(NetworkGateway)
     const TLbsNetSessionIdArray dummySessionIdArray;
  
     TInt index = iLbsNetSessions.Find(aRequestId,CLbsNetSession::IsSessionNumMatch);
     if (index >= 0)
        {
        if (aReason == KErrNone)
            {
            // Extract Reference Location from the reported assistance data if present and 
            // send it to LBS as a Location Update.
            //
            if (EAssistanceDataReferenceLocation == (aDataReportMask & EAssistanceDataReferenceLocation))
                {
                RReferenceLocationBuilder* refLocBuilderPtr;
                RLbsAssistanceDataBuilderSet& data = const_cast<RLbsAssistanceDataBuilderSet&>(aAssistanceData);
                if (KErrNone == data.GetDataBuilder(refLocBuilderPtr))
                    {
                    if (refLocBuilderPtr->IsDataAvailable())
                        {
                        // Create a Ref Loc Reader with data from the Builder
                        RReferenceLocationReader refLocReader;
                        TInt err = KErrNone;
                        TRAP(err,refLocReader.OpenL());
                        if (KErrNone == err)
                            {
                            refLocReader.DataBuffer() = refLocBuilderPtr->DataBuffer();
                            
                            // Obtain position info from the Ref Loc Reader and set it in a TPositionInfo
                            if(refLocReader.FieldExists(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide))
                                {
                                REllipsoidPointAltitudeEllipsoideReader ellipsoidReader;
                                refLocReader.GetFieldReader(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, ellipsoidReader);
                                TPositionInfo positionInfo;
                                TPosition position;
                                TUint tempUint;
                                TInt  tempInt;
                                TReal64 latitudeDegrees;
                                TReal64 longitudeDegrees;
                                TReal32 altitudeMeters;
                                TReal aux = 0;
                                TReal32 uncertainty;

                                position.SetCurrentTime();
                                // Get latitude and convert to degrees
                                ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::ELatitudeSign, tempInt);
                                ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::ELatitude, tempUint);
                                latitudeDegrees = (tempUint * 90.0) / 8388608.0; //*90/2^23 ; ETSI TS 123.032
                                latitudeDegrees *= (tempInt == TEllipsoidPointAltitudeEllipsoide::ESouth)? -1: 1;
                                
                                // Get longitude and convert to degreees
                                ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::ELongitude, tempInt);
                                longitudeDegrees = (tempInt * 45.0) / 2097152.0; //*360/2^24; ETSI TS 123.032

                                // Get altitude (meters) and apply sign
                                ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EAltitudeDirection, tempInt);
                                ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EAltitude, tempUint);
                                altitudeMeters = tempUint; // ETSI TS 123.032
                                altitudeMeters *= (tempInt == TEllipsoidPointAltitudeEllipsoide::EDepth)? -1: 1;
                                
                                position.SetCoordinate(latitudeDegrees, longitudeDegrees, altitudeMeters);

                                // Get both horizontal uncertainties, convert to meters and set Horizontal Accuracy to the largest
                                ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMajor, tempUint);
                                Math::Pow(aux, 1.1, tempUint);
                                uncertainty = 10*(aux - 1); // ETSI TS 123.032 : C(((1+x)^k) -1);  x = 0.1; C= 10
                                ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMinor, tempUint);
                                Math::Pow(aux, 1.1, tempUint);
                                uncertainty = uncertainty > (10*(aux - 1))? uncertainty : (10*(aux - 1));
                            
                                position.SetHorizontalAccuracy(uncertainty);
                                
                                // Get vertical uncertainty, convert to meters and set Vertical Accuracy
                                ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintyAltitude, tempUint);
                                Math::Pow(aux,1.025, tempUint);
                                uncertainty = 45*(aux - 1); //ETSI TS 123.032; C(((1+x)^k) -1); C=45; x=0.025
                                position.SetVerticalAccuracy(uncertainty);

                                positionInfo.SetPosition(position);

                                const TPositionModuleId id = {KSPPModuleDllUid};
                                positionInfo.SetModuleId(id);
                                positionInfo.SetUpdateType(EPositionUpdateGeneral);
                                positionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);
                                positionInfo.SetPositionModeReason(EPositionModeReasonNone);
                                
                                refLocReader.Close();
                            
                                // Send the reference position to LBS
                                LBSLOG2(ELogP1, "-->ProcessLocationUpdate(0x%x)\n", iLbsNetSessions[index]->SessionId());
                                iObserver.ProcessLocationUpdate(iLbsNetSessions[index]->SessionId(),positionInfo);
                                }//if - position from ref loc reader
                            }//if - open ref loc reader
                        }//if - data available
                    }//if - GetDataBuilder
                }//if - ref loc bit set
            }// if - (aReason == KErrNone)
        }
    
    //Pass on the assistance data to LBS.
    LBSLOG2(ELogP1, "-->ProcessAssistanceData(0x%x)\n", aDataReportMask);
    iObserver.ProcessAssistanceData(aDataReportMask, aAssistanceData,
            aReason, dummySessionIdArray);
    //Reset the assistance data fields
    if (aReason == KErrNone)
        {
        for (TInt index = iLbsNetSessions.Count() - 1; index >= 0; index--)
            {
            //Update the masks for all the sessions that have started a positioning session.
            //ie. all sessions except MTLRs that are currently processing privacy session.
            //this is required as we create an entry for MTLRs in iLbsNetSessions during the priv session 
            if (iLbsNetSessions[index]->PosSessionStarted())
                {
                TLbsAssistanceDataGroup assistanceDataMask = iLbsNetSessions[index]->GetAssistanceDataMask();
                //Clear the fields corresp to the assistance data items that we have just received
                assistanceDataMask &= ~aDataReportMask;
                iLbsNetSessions[index]->SetAssistanceDataMask(assistanceDataMask);
                }
            }
        }

	LBSLOG(ELogP1, "CSuplProxyProtocol::ProcessAssistanceData() End\n");
    }

/* Stores the positioning method requested by the network if we haven't already decided on one
 * 
 */
void CSuplProxyProtocol::SetPosMethod(const TLbsNetPosRequestMethod& aMethod, TInt aRequestId)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::SetPosMethod() Begin\n");

	// find the matching lbs net session
	TInt index = iLbsNetSessions.Find(aRequestId, CLbsNetSession::IsSessionNumMatch);
	if (index >= 0)
		{
		CLbsNetSession* netSession = iLbsNetSessions[index];
		TLbsNetPosRequestMethod method = netSession->PosMethod();
		
        netSession->SetPosMethod(aMethod);
		}

	LBSLOG(ELogP1, "CSuplProxyProtocol::SetPosMethod() End\n");
	}

/** Cancels a direct (at SUPL level) location request from the network 
@param aMessage
@param aRequestId	the request id from the subsession
*/
TInt CSuplProxyProtocol::CancelNetworkGetPosition(const RMessage2& /*aMessage*/ , TInt aRequestId)	
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::CancelNetworkGetPosition() Begin\n");

	TInt err = KErrNone;
	CompleteMatchingRequest(aRequestId,CLbsNetSession::EPosRequestTypeGetPosition,KErrCancel);

	LBSLOG(ELogP1, "CSuplProxyProtocol::CancelNetworkGetPosition() End\n");

	return err;
	}

/** Gets the current positioning capabilities of the device from the LBS subsystem 
	For example this might include terminal based GPS but not terminal assisted.
	
@param aPosCaps	[out] capabilities supported
 */
void CSuplProxyProtocol::GetCurrentPosCapabilities(TLbsNetPosCapabilities& aPosCaps)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::GetCurrentPosCapabilities() Begin\n");

	iObserver.GetCurrentCapabilities(aPosCaps);

	LBSLOG(ELogP1, "CSuplProxyProtocol::GetCurrentPosCapabilities() End\n");

	}

TInt CSuplProxyProtocol::GetAssistanceDataMask(TInt aRequestId)
	{
	LBSLOG(ELogP1, "CSuplProxyProtocol::GetAssistanceDataMask() Begin\n");

	TInt err = KErrNotFound;
	// find the matching lbs net session
	TInt index = iLbsNetSessions.Find(aRequestId, CLbsNetSession::IsSessionNumMatch);
	
	
	if (index >= 0)
		{
		CLbsNetSession* netSession = iLbsNetSessions[index];
		
		LBSLOG2(ELogP1, "request type = 0x%x (EServiceMobileTerminated = 0x04)\n", netSession->Service());
		
		if ((netSession->Service() & MLbsNetworkProtocolObserver2::EServiceNetworkLocation)
                || (netSession->Service() & MLbsNetworkProtocolObserver2::EServiceMobileTerminated))
            {
	        //In the case of MOLR-TA & MTLR, inject a processlocationrequest to LBS.
            // LBS will respond with RequestAssistanceData() (with assistance data mask)
            // or RespondLocationRequest() (with error)
            LBSLOG(ELogP1, "ProcessLocationRequest()-->LBS to get AD\n");
            //To infer at a later stage that this location req has been issued just to retrieve
            //the assistance data mask.
            netSession->SetType(CLbsNetSession::EPosRequestTypeSuplInfoAssData);
            iObserver.ProcessLocationRequest(netSession->SessionId(),
                    EFalse,netSession->Service(),netSession->Quality(),netSession->PosMethod());
		    }
		else
		    {
	        //In the case of MOLR-TB, update the observer of the assistance 
	        //data mask stored in a data object [obtained during RequestSelfLocation or 
            //set through a more recent assistancedatarequest for another session]		    
            netSession->PosProtocolResponseObserver()->HandleSetAssistanceDataMask(KErrNone, netSession->GetAssistanceDataMask());
		    }
		err = KErrNone;
		netSession->SetAssistanceDataMaskRequested();
        }
	LBSLOG(ELogP1, "CSuplProxyProtocol::GetAssistanceDataMask() End\n");

	return err;
	}

/** Callback from the SuplTIApi through the MLbsSuplTiObserver interface
@param aReason is the return/error code for the completed location request.
@param aSessionId is the id of the completed request.
@see MLbsSuplTiObserver
@see CLbsSuplTiApi
@see MLbsNetworkProtocolObserver2
 */
void CSuplProxyProtocol::RequestComplete(TInt aReason,
        const TLbsNetSessionId& aSessionId)
    {
	LBSLOG(ELogP1, "CSuplProxyProtocol::RequestComplete() Begin\n");
	
    TInt index = iLbsNetSessions.Find(aSessionId,
            CLbsNetSession::IsSessionMatch);

    if (index >= 0)
        {
        //Nothing to be done in the case of MOLR-TerminalBased. Session complete
        //will be sent to LBS when NotifySessionEnd gets called.

        //In the case of MOLR-TerminalAssisted / CellBased location request 
        //[ie. corresp to RequestNetworkLocation()] do the following:
        if (iLbsNetSessions[index]->Service()
                & MLbsNetworkProtocolObserver2::EServiceNetworkLocation)
            {
            TInt err = aReason;
            if (err == KErrNone)
                {
                // TODO Note: For the time being it has been agreed that we retain TPositionInfo.
                // The SUPL FW supports two data types when requesting position (TPositionInfo)
                // and HPositionGenericInfo [provides extra info - horizontal, vertical velocity].
                // But we can't pass in HPositionGenericInfo using ProcessLocationUpdate and hence
                // some conversion has to be done. HPositionGenericInfo has to be used only if we 
                // need the velocity information.

                //Read the position from the Terminal Initiation API
                TPositionInfo posInfo;
                
				// Log the call to GetPosition and the session id which is an input parameter
				LBSLOG(ELogP9, "->S   CLbsSuplTiApi::GetPosition() SUPL-FW\n");
				LBSLOG(ELogP9, " > const TLbsNetSessionId aSessionId = \n");
				LBSLOG_SESSIONID(aSessionId);

                err = iSuplTiApiWrapper->GetPosition(aSessionId, posInfo);

				// log the position output from supl TI Api
				LBSLOG(ELogP9, " < TPositionInfo posInfo = \n");
				LBSLOG_TPOSITIONINFOBASE(posInfo);
                
                if (err == KErrNone)
                    {
                    //Send the network calculated position to the Network Gateway
                    LBSLOG2(ELogP1, "-->ProcessLocationUpdate(0x%x)\n", aSessionId.SessionNum());
                    iObserver.ProcessLocationUpdate(aSessionId, posInfo);
                    }//if
                }//if
            if (!iLbsNetSessions[index]->IsSessionStarted() || iLbsNetSessions[index]->SessionCompleteStatus())
                {
                //Send ProcessSessionComplete to LBS as SessionEnd has already been
                //received.
                err = CompleteMatchingSession(aSessionId,iLbsNetSessions[index]->SessionCompleteErr());
                }
            else
                {
                //SessionEnd not received yet. So wait until we receive NotifySessionEnd
                //to send ProcessSessionComplete to LBS.
                iLbsNetSessions[index]->SetSessionCompleteErr(err);
                iLbsNetSessions[index]->SetSessionCompleteStatus(ETrue);
                }//if
            }//if  MOLR-TA/CellBased

        //If there is no "other" MOLR outstanding when RequestComplete() is called then
        //call CloseSession() on the TI API.
        TBool otherMOLRSession = EFalse;
        for (TInt sessionIndex = iLbsNetSessions.Count()-1; sessionIndex>=0; sessionIndex-- )
            {
            //Check for any MOLR-TB [EServiceSelfLocation] / MOLR-TA [EServiceNetworkLocation]
            if ( (iLbsNetSessions[sessionIndex]->Service() == 
                    MLbsNetworkProtocolObserver2::EServiceSelfLocation) ||
                    (iLbsNetSessions[sessionIndex]->Service() == 
                       MLbsNetworkProtocolObserver2::EServiceNetworkLocation) )
                {
                otherMOLRSession = ETrue;
                }
            }//for - all sessions

        if (!otherMOLRSession)
            {
            iSuplTiApiWrapper->CloseSession();
            // Log the call to CloseSession()
            LBSLOG(ELogP9, "->S   CLbsSuplTiApi::CloseSession() SUPL-FW\n");
            } 
        }//if - a matching session object is found
	LBSLOG(ELogP1, "CSuplProxyProtocol::RequestComplete() End\n");
    }


/** Utility function to complete a request at a particular index in the queue
with the given error code.

@params aIndex      The index within the queue of the request to be completed
@params aErrorCode  The completion code for the request
*/
void CSuplProxyProtocol::CompleteRequest(TInt aIndex,TInt aErrorCode)
    {
	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteRequest() Begin\n");

    CLbsNetSession* netSession = iLbsNetSessions[aIndex];
    netSession->Msg().Complete(aErrorCode);
    netSession->SetType(CLbsNetSession::EPosRequestTypeNone);

	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteRequest() End\n");

    }

/** Utility function to search through the queue of outstanding requests for a particular
type of asynchronous request [see @CLbsNetSession::TRequesttype] and complete the matching
entries with the given error code

@params aRequestType - The type of outstanding asynchronous request [eg. EPosRequestTypeGetSuplInfo ]
@params aErrorCode   - Return code
@return KErrNone if the request is found and completed, KErrNotFound otherwise
*/
TInt CSuplProxyProtocol::CompleteMatchingRequest(CLbsNetSession::TRequesttype aRequestType, 
        TInt aErrorCode)
    {
	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteMatchingRequest() Begin\n");

    TInt err = KErrNotFound;

    for (TInt index = iLbsNetSessions.Count() - 1; index >= 0; index--)
        {
        CLbsNetSession* netSession = iLbsNetSessions[index];
        if (netSession->Type() == aRequestType)
            {
            CompleteRequest(index, aErrorCode);
            err = KErrNone;
            }
        }
    // if no request found it's too late to cancel, do nothing

	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteMatchingRequest() End\n");

    return err;
    }

/** Utility function to find a request in the queue based on its request id and request type 
and then complete it with the given error code

@params: aRequestId   The ID of the request to be completed
@params: TRequesttype The type of async request that is outstanding
@params: aErrorCode   The error/reason code for completing the request
@return: KErrNone if the request is found and completed, KErrNotFound otherwise
*/
TInt CSuplProxyProtocol::CompleteMatchingRequest(TInt aRequestId,CLbsNetSession::TRequesttype aRequestType,
        TInt aErrorCode)
    {
	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteMatchingRequest()-v3 Begin\n");


    TInt err = KErrNotFound;
    
    const TInt requestId = aRequestId;
    TInt index = iLbsNetSessions.Find(requestId, CLbsNetSession::IsSessionNumMatch);
    
    if (index >= 0 && iLbsNetSessions[index]->Type() == aRequestType)
        {
        CompleteRequest(index,aErrorCode);
        err = KErrNone;
        }

	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteMatchingRequest()-v3 End\n");

    // if no request found it's too late, do nothing
    return err;
    }


/** Utility function to search for a session in the queue based on the session number 
and complete it.

@params: aRequestId   The sesssion number of the session to be completed
@params: aReason      The reason for the completion
@return: KErrNone if the request is found and completed, KErrNotFound otherwise
@see MLbsNetworkProtocolObserver2::ProcessSessionComplete
*/
TInt CSuplProxyProtocol::CompleteMatchingSession(TInt aRequestId, TInt aReason)
    {
	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteMatchingSession() Begin\n");

    TInt err = KErrNotFound;
    
    const TInt requestId = aRequestId;
    TInt index = iLbsNetSessions.Find(requestId, CLbsNetSession::IsSessionNumMatch);
    
    if (index >= 0)
        {
        const TLbsNetSessionId netSessionId = iLbsNetSessions[index]->SessionId();
        
        //Inform LBS that the session is to be completed
        LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", netSessionId.SessionNum());
        iObserver.ProcessSessionComplete(netSessionId,aReason);
        
        StatusUpdate(iLbsNetSessions[index]->Service(),EFalse);
        
        CLbsNetSession* netSession = iLbsNetSessions[index];
        LBSLOG2(ELogP1, "CSuplProxyProtocol::CompleteMatchingSession() Removing sppm session object 0x%x to array\n", requestId);
        iLbsNetSessions.Remove(index);
        delete netSession;        
        err = KErrNone;
        }

	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteMatchingSession() End\n");

    return err;
    }


/** Utility function to search for a session in the queue based on the session id 
and complete it.

@params: aSessionId   The ID of the session to be completed
@params: aProcess     The process that is active or queued in network protocol module.
@params: aReason      The reason for the completion
@return: KErrNone if the request is found and completed, KErrNotFound otherwise
@see MLbsNetworkProtocolObserver2::TLbsNetProtocolService
@see MLbsNetworkProtocolObserver2::ProcessSessionComplete
*/
TInt CSuplProxyProtocol::CompleteMatchingSession(const TLbsNetSessionId& aSessionId,
        TInt aReason)
    {
	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteMatchingSession()-v2 Begin\n");

    TInt err = KErrNotFound;
    
    TInt index = iLbsNetSessions.Find(aSessionId, CLbsNetSession::IsSessionMatch);
    
    if (index >= 0)
        {

        //Inform LBS that the session is to be completed
        LBSLOG2(ELogP1, "-->ProcessSessionComplete(0x%x)\n", aSessionId.SessionNum());
        iObserver.ProcessSessionComplete(aSessionId,aReason);

        StatusUpdate(iLbsNetSessions[index]->Service(),EFalse);
      
        CLbsNetSession* netSession = iLbsNetSessions[index];
        LBSLOG2(ELogP1, "CSuplProxyProtocol::CompleteMatchingSession(sessionId) Removing sppm session object 0x%x to array\n", aSessionId);
        iLbsNetSessions.Remove(index);
        delete netSession;
        err = KErrNone;
        }

	LBSLOG(ELogP1, "CSuplProxyProtocol::CompleteMatchingSession()-v2 End\n");

    return err;
    }

/* Utility function to search for a recently accepted Verification Request from the same requestor

 */
TBool CSuplProxyProtocol::HasRecentMatchingVerificationResponse(TLbsExternalRequestInfo2& aExtRequestInfo)
	{
	TBool found = EFalse;
	
	TLbsRequesterId2 requestorIdtoMatch;
	TLbsClientName2 clientNametoMatch;

	aExtRequestInfo.GetRequesterId(requestorIdtoMatch);
	aExtRequestInfo.GetClientName(clientNametoMatch);
	
	for (TInt index = iLbsNetSessions.Count() - 1; index >= 0; index--)	
		{

		TTime now;
		now.UniversalTime();
		TTime responseTimeStamp = iLbsNetSessions[index]->GetResponseTimeStamp();
		TTimeIntervalSeconds interval;
		
		const TInt KMaxSecondsSinceRecent = 2;
		now.SecondsFrom(responseTimeStamp, interval);
		
		TBool recent = interval.Int() < KMaxSecondsSinceRecent;
		TLbsExternalRequestInfo2 acceptedExtRequestInfo = iLbsNetSessions[index]->GetExtRequestInfo();
	    TLbsRequesterId2 acceptedRequesterId;
	    TLbsClientName2 acceptedClientName;		

		acceptedExtRequestInfo.GetRequesterId(acceptedRequesterId);
		TBool foundExtString = EFalse;
		TBool extStringMatch = ETrue;
		if(acceptedRequesterId.Length() > 0)
			{
			foundExtString = ETrue;
			if(acceptedRequesterId.Compare(requestorIdtoMatch) != 0)
				{
				extStringMatch = EFalse;
				}
			}
		acceptedExtRequestInfo.GetClientName(acceptedClientName);
		if(acceptedClientName.Length() > 0)
			{
			foundExtString = ETrue;
			if(acceptedClientName.Compare(clientNametoMatch) != 0)
				{
				extStringMatch = EFalse;
				}
			}		
		if(iLbsNetSessions[index]->Service() == MLbsNetworkProtocolObserver2::EServiceMobileTerminated)
		    {
		    //As iLbsNetSessions contains both MOLR and MTLR entries, assert only for MTLRs
		    __ASSERT_DEBUG(foundExtString, User::Invariant());	// SUPL Spec dictates that either RequestorID or ClientName must exist
		    (void) foundExtString; // suppress compiler warning if not in debug
		    }
		if(recent && extStringMatch)
			{
			found = ETrue;
			break;	// no need to continue for loop once we get a match
			}
		}
		
	return found;
	}


/** 
Defines the plug-in interface implementation UIDs, required by ECOM.
*/
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KPluginUidValue, CSuplProxyProtocol::NewL)
    };


/** 
Exported function for ECOM to retrieve implementations table.
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

