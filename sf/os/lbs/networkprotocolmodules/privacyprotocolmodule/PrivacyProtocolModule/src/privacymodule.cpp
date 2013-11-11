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
// privacymodule.h
// This file provides the class implementation of the ECOM plug-in 
// interface for the LBS Privacy Protocol Module.
// 
//

#include <ecom/implementationproxy.h>
#include "lbsdevloggermacros.h"
#include <lbs/lbsadmin.h>
#include "privacymodule.h"
#include "lbsprivacyserverreturncodes.h"


/** The unique ID of this plug-in implementation.
This corresponds to the implementation UID specified in the .rss file
for this protocol module.
*/
const TInt KPluginUidValue = 0x1028373C;


/** 
Static public constructor.

@param aObserver Plug-in parameters, which includes a reference to this protocol module's observer.
@return A new instance of the CPrivacyProtocol class.
*/  
CPrivacyProtocol* CPrivacyProtocol::NewL(TAny* aParam)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::NewL() Begin\n");
	LBSLOG(ELogP2, "CPrivacyProtocol::NewL");

	TLbsNetProtocolModuleParams* param = reinterpret_cast<TLbsNetProtocolModuleParams*>(aParam);
	CPrivacyProtocol* self = new (ELeave) CPrivacyProtocol(param->Observer());
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	LBSLOG(ELogP1, "CPrivacyProtocol::NewL() End\n");
	return self;
	}


/** 
Standard destructor.
*/  
CPrivacyProtocol::~CPrivacyProtocol()
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::~CPrivacyProtocol() Begin\n");
	LBSLOG(ELogP2, "CPrivacyProtocol::~CPrivacyProtocol");
	delete iRequestBuffer;
	delete iServer;	
	LBSLOG(ELogP1, "CPrivacyProtocol::~CPrivacyProtocol() End\n");
	}


/** 
Request to cancel a previous privacy verification request.

@param aCancelInfo The information required to cancel the request.
*/	
void CPrivacyProtocol::CancelVerifyLocationRequest(CCancelPrivacyRequestInfo* aCancelInfo)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::CancelVerifyLocationRequest() Begin\n");
	TInt requestId = aCancelInfo->GetRequestId();
	TInt cancelReason = aCancelInfo->CancelReason();

	// Complete the session
	CPrivacyRequestInfo* request = iRequestBuffer->FindRequest(requestId);
	if (request)
		{
		TLbsNetSessionId sessionId(TUid::Uid(KPluginUidValue),requestId);
		iObserver.ProcessSessionComplete(sessionId,cancelReason);
		request->SetIsSessionCompleteAutomatic(EFalse);
		}
	
	// Cancel any waiting requests with this ID in the buffer
	iRequestBuffer->CancelAnyOutstandingRequestById(requestId, cancelReason);

	// We're done with this now, so delete
	delete aCancelInfo;
	
	LBSLOG(ELogP1, "CPrivacyProtocol::CancelVerifyLocationRequest() End\n");
	}


/** 
Request to get the next session ID to use for a new privacy request.

@param aSessionId Reference to a TLbsNetSessionId to populate with the new session ID.
*/	
void CPrivacyProtocol::GetNextSessionId(TLbsNetSessionId& aSessionId)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::GetNextSessionId() Begin\n");
	aSessionId = iNextSessionId;
	if (iNextSessionId.SessionNum() == KMaxTInt)
		{
		iNextSessionId.SetSessionNum(0);
		}
	iNextSessionId.IncrSession();
	LBSLOG(ELogP1, "CPrivacyProtocol::GetNextSessionId() End\n");
	}


/** 
Send a privacy notification request to the LBS sub-system.

@param aRequestInfo The information about this request.
*/	
void CPrivacyProtocol::NotifyLocationRequest(CPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::NotifyLocationRequest() Begin\n");
	ProcessRequest(aRequestInfo);
	LBSLOG(ELogP1, "CPrivacyProtocol::NotifyLocationRequest() End\n");
	}


/** 
Notifies the LBS sub-system that a previous privacy verification request timed out in the network.

@param aRequestInfo The information about this request.
*/	
void CPrivacyProtocol::NotifyVerificationTimeout(CPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::NotifyVerificationTimeout() Begin\n");
	ProcessRequest(aRequestInfo);
	LBSLOG(ELogP1, "CPrivacyProtocol::NotifyVerificationTimeout() End\n");
	}


/** 
Sends a privacy verification request to the LBS sub-system.

@param aRequestInfo The information about this request.
*/	
void CPrivacyProtocol::VerifyLocationRequest(CPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::VerifyLocationRequest() Begin\n");
	ProcessRequest(aRequestInfo);
	LBSLOG(ELogP1, "CPrivacyProtocol::VerifyLocationRequest() End\n");
	}

/** 
Sends a privacy request to the LBS sub-system.

@param aRequestInfo The information about this request.
*/	
void CPrivacyProtocol::PrivacyLocationRequest(CPrivacyRequestInfo* aRequestInfo)
	{
    LBSLOG(ELogP7, "<-S CPrivacyProtocol::PrivacyLocationRequest(aRequestInfo)\n");
	ProcessRequest(aRequestInfo);
	}

/**
Completes the privacy request session. This is part of the call made from the client side, so 
The client gets notified of this.

@param aRequestId The session of the privacy request to be completed
@param aReason The reason for which is completed
*/
void CPrivacyProtocol::CompletePrivacyRequest(TUint32 aRequestId, TInt aReason)
	{
    LBSLOG(ELogP7, "<-S CPrivacyProtocol::CompletePrivacyRequest(aRequestId, aReason)\n");
    LBSLOG2(ELogP7, "\t > TLbsPrivacyRequestId aRequestId = %d\n", aRequestId);
    LBSLOG2(ELogP7, "\t > TLbsPrivacyRequestId aReason = %d\n", aReason);
	
	CPrivacyRequestInfo* request = iRequestBuffer->FindRequest(aRequestId);
	if (request)
		{
		// Need to complete the message for LBS
		TLbsNetSessionId sessionId(TUid::Uid(KPluginUidValue),aRequestId);
		iObserver.ProcessSessionComplete(sessionId,aReason);
		request->SetIsSessionCompleteAutomatic(EFalse);

		// Complete the request if it is awaiting a response.
		if (request->IsResponseRequired())
			{
		    LBSLOG(ELogP7, "->A Client::RunL(aResponse)\n");
		    LBSLOG2(ELogP7, "\t < TInt aResponse = %d\n", KRequestCancelledByClient);
			request->CompleteRequest(KRequestCancelledByClient);
			}
		
		// Remove the request and delete
		iRequestBuffer->RemoveRequest(request);
		delete request;
		}

	LBSLOG(ELogP1, "CPrivacyProtocol::CompletePrivacyRequest() End\n");
	}

/**
Signals that a client has disconnected from the server.

Normally the disconnect will be because the client has closed
the session, but it may also be because the client panicked. 

We must clean up any outstanding requests the client had here.

@param aSession Session object for the client that disconnected.
*/
void CPrivacyProtocol::SessionDisconnected(const CSession2* aSession)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::SessionDisconnected() Begin\n");

	// Complete the session
	CPrivacyRequestInfo* request = iRequestBuffer->FindRequest(aSession);
	if (request)
		{
		TLbsNetSessionId sessionId(TUid::Uid(KPluginUidValue),request->GetRequestId());
		iObserver.ProcessSessionComplete(sessionId,KErrCancel);
		request->SetIsSessionCompleteAutomatic(EFalse);
		
		// Remove the request and delete
		iRequestBuffer->RemoveRequest(request);
		delete request;
		}
	
	// Check if any buffered requests need to cancelled
	iRequestBuffer->CancelAnyOutstandingRequestBySession(aSession);

	LBSLOG(ELogP1, "CPrivacyProtocol::SessionDisconnected() End\n");	
	}


/** 
Called by the Network Gateway in response to our privacy requests.

@param aSessionId The ID of the session.
@param aResponse Response information for this privacy session.
*/	
void CPrivacyProtocol::RespondPrivacyRequest(const TLbsNetSessionId& aSessionId, const TLbsPrivacyResponse& aResponse)
	{
	// Locate the request in the buffer
	CPrivacyRequestInfo* request = iRequestBuffer->FindRequest(aSessionId.SessionNum());
	if (request)
		{
		// Complete the active request if it is awaiting a response
		if (request->IsResponseRequired())
			{
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
		
		if(request->IsSessionCompleteAutomatic())
			{
			// Notify network gateway that this session is complete
			iObserver.ProcessSessionComplete(aSessionId,KErrNone);
			request->SetIsSessionCompleteAutomatic(EFalse);

			// Remove the request and delete
			iRequestBuffer->RemoveRequest(request);
			delete request;
			}
		}
	else
		{
		LBSLOG(ELogP2, "CPrivacyProtocol::RespondPrivacyRequest() Unexpected response\n");
		}
	}


//---------------------------------------------------------------------------
// The following Network Protocol Module SPI methods are are not supported, 
// as this module is only concerned with privacy requests:
//---------------------------------------------------------------------------


/**
Send a response to a location request to the network.
@see CLbsNetworkProtocolBase::RespondLocationRequest()
*/
void CPrivacyProtocol::RespondLocationRequest(const TLbsNetSessionId& /*aSessionId*/, TInt /*aReason*/, 
												const TPositionInfoBase& /*aPosInfo*/)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::RespondPrivacyRequest()\n");
	}


/**
Send a request for GPS assistance data to the network.
@see CLbsNetworkProtocolBase::RequestAssistanceData()
*/
void CPrivacyProtocol::RequestAssistanceData(TLbsAsistanceDataGroup /*aDataRequestMask*/)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::RespondPrivacyRequest()\n");
	}


/**
Send a request for an approximate location based on the current network cell to the network.
@see CLbsNetworkProtocolBase::RequestNetworkLocation()
*/
void CPrivacyProtocol::RequestNetworkLocation(const TLbsNetSessionId& /*aSessionId*/,
							const TLbsNetPosRequestOptionsBase& /*aOptions*/)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::RespondPrivacyRequest()\n");
	}


/**
Tell the protocol module to finish the current network based location request.
@see CLbsNetworkProtocolBase::CancelNetworkLocation()
*/
void CPrivacyProtocol::CancelNetworkLocation(const TLbsNetSessionId& /*aSessionId*/,
							TInt /*aReason*/)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::RespondPrivacyRequest()\n");
	}


/** 
Request self locate.
@see CLbsNetworkProtocolBase::RequestSelfLocation()
*/
void CPrivacyProtocol::RequestSelfLocation(const TLbsNetSessionId& /*aSessionId*/,
							const TLbsNetPosRequestOptionsBase& /*aOptions*/)	
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::RespondPrivacyRequest()\n");
	}


/**
Cancel request for self locate.
@see CLbsNetworkProtocolBase::CancelSelfLocation()
*/
void CPrivacyProtocol::CancelSelfLocation(const TLbsNetSessionId& /*aSessionId*/,
							TInt /*aReason*/)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::RespondPrivacyRequest()\n");
	}


/**
Lbs System Status information.
@see CLbsNetworkProtocolBase::AdviceSystemStatus()
*/
void CPrivacyProtocol::AdviceSystemStatus(TLbsSystemStatus /*aStatus*/)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::RespondPrivacyRequest()\n");
	}


/**
Send a request to send the handsets location to a third party to the network.
@see CLbsNetworkProtocolBase::RequestTransmitLocation()
*/
void CPrivacyProtocol::RequestTransmitLocation(const TLbsNetSessionId& /*aSessionId*/,
								const TDesC& /*aDestination*/, TInt /*aPriority*/)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::RespondPrivacyRequest()\n");
	}


/**
Tell the protocol module to end the current send location to third party request.
@see CLbsNetworkProtocolBase::CancelTransmitLocation()
*/
void CPrivacyProtocol::CancelTransmitLocation(const TLbsNetSessionId& /*aSessionId*/, TInt /*aReason*/)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::RespondPrivacyRequest()\n");
	}


/** 
Default constructor.

@param aObserver Reference to this protocol module's observer.
*/
CPrivacyProtocol::CPrivacyProtocol(MLbsNetworkProtocolObserver& aObserver) : 
	iObserver(aObserver)
	{
	LBSLOG(ELogP2, "CPrivacyProtocol::CPrivacyProtocol Begin");
	
	// Seed the intial requestId from the current time.
	TTime now;
    now.UniversalTime();
    const TInt KPosRequestIdShift = 14;
    TInt64 requestId64 = now.Int64() >> KPosRequestIdShift;
    TUint low = I64LOW(requestId64);
    TInt requestId = ~(~low | 0x80000000);
    iNextSessionId = TLbsNetSessionId(TUid::Uid(KPluginUidValue),
    								  requestId);
    
    LBSLOG(ELogP2, "CPrivacyProtocol::CPrivacyProtocol End");
	}


/** 
Second stage private constructor.
*/  
void CPrivacyProtocol::ConstructL()
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::ConstructL() Begin\n");
	iRequestBuffer = CPrivacyRequestBuffer::NewL();

	iServer = CPrivacyServer::NewL(*this);
	
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);
	
	
	// Read admin setting for maximum number of external locate requests
	TUint maxExternalLocateRequests = KLbsDefaultMaximumExternalLocateRequests;
	TInt err = admin->Get(KLbsSettingMaximumExternalLocateRequests, maxExternalLocateRequests);
	if (err != KErrNone)
		{
		LBSLOG_ERR2(ELogP4, "Failed to get KLbsSettingMaximumExternalLocateRequests (err %d)", err);
		}
	iMaxSimultaneousActiveRequests = maxExternalLocateRequests;	
	LBSLOG2(ELogP1, "Using KLbsSettingMaximumExternalLocateRequests = %d", iMaxSimultaneousActiveRequests);

	
	// Read admin setting for shutdown delay
	TUint shutdownDelay = KLbsDefaultPrivacyShutdownDelay;
	err = admin->Get(KLbsSettingPrivacyShutdownDelay, shutdownDelay);
	if (err != KErrNone)
		{
		LBSLOG_ERR2(ELogP4, "Failed to get KLbsSettingPrivacyShutdownDelay (err %d)", err);
		}
	LBSLOG2(ELogP1, "Using KLbsSettingPrivacyShutdownDelay = %d", shutdownDelay);
	iServer->SetShutdownDelay(shutdownDelay);
	
	
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
	LBSLOG(ELogP1, "CPrivacyProtocol::ConstructL() End\n");
	}


/** 
Submit a privacy request for processing.

Adds the request to the buffer, and starts processing it if possible.

Note: There may be a limit on the number of requests allowed to be active simultaneously, if this
limit is reached then the request will be buffered for processing later.

@param aRequest The request info to process.
*/	
void CPrivacyProtocol::ProcessRequest(CPrivacyRequestInfo* aRequest)
	{
	LBSLOG(ELogP1, "CPrivacyProtocol::ProcessRequest() Begin\n");

	if (iRequestBuffer->NumRequests() >= iMaxSimultaneousActiveRequests)
		{
		// Maximum number of requests has been reached
		LBSLOG_WARN(ELogP1, "CPrivacyProtocol::ProcessRequest() Request limit reached\n");
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
		LBSLOG_WARN2(ELogP1, "CPrivacyProtocol::ProcessRequest() Error buffering request (%d)\n", err);
		if (aRequest->IsResponseRequired())
			{
			aRequest->CompleteRequest(err);
			}
		delete aRequest;
		return;
		}

	// Send the request to the network gateway
	LBSLOG(ELogP1, "CPrivacyProtocol::ProcessRequest() Sending request\n");

	TLbsNetSessionId sessionId(TUid::Uid(KPluginUidValue),aRequest->GetRequestId());
	
	TLbsNetPosRequestPrivacy requestPrivacy;
	aRequest->GetRequestPrivacy(requestPrivacy);
			
	TLbsExternalRequestInfo2 requestInfo;
	aRequest->GetRequestInfo(requestInfo);
	
#ifdef ENABLE_LBS_DEV_LOGGER
    LBSLOG(ELogP7, "->A MLbsNetworkProtocolObserver::ProcessPrivacyRequest(sessionId, EFalse, requestPrivacy, requestInfo) LbsPrivacyProtocolModule\n");
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

    iObserver.ProcessPrivacyRequest(sessionId, EFalse, requestPrivacy, requestInfo);
    
    LBSLOG(ELogP1, "CPrivacyProtocol::ProcessRequest() End\n");
	}

/** 
Defines the plug-in interface implementation UIDs, required by ECOM.
*/
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KPluginUidValue, CPrivacyProtocol::NewL)
    };


/** 
Exported function for ECOM to retrieve implementations table.
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }



