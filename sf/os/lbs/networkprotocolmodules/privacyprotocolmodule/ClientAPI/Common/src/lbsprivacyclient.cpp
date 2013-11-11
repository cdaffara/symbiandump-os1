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
// Class implementation of RPrivacyServer
// 
//

/**
 @file
 @internalTechnology
 @released
*/

// INCLUDE FILES
#include <e32std.h>
#include "lbsprivacyclient.h"
#include "lbsprivacyservermsgenums.h"   
#include "lbssystemcontroller.h"
#include "lbsdevloggermacros.h"

const TInt KPosNumberOfTrials = 16;

// The name of the server
_LIT(KPrivacyServerName,"LBSPrivacyServer");

/*
 * C++ default constructor 
*/
RPrivacyServer::RPrivacyServer()
    : RSessionBase(),iRequestInfoPkg(iRequestInfo),
      iNetPosRequestPrivacyPkg(iNetPosRequestPrivacy),
	  iRequestIdPkg(iRequestId), 
	  iRequestIdUint32Pkg(iRequestIdUint32)
    {
    }


/*
 * Destructor 
*/
RPrivacyServer::~RPrivacyServer()
    {
    }

/** 
 * Opens a connection with the server. This method must be called
 * before any other operation.
 * @return KErrNone if the operation was successful, otherwise
 * one of the system-wide error codes.
 */

TInt RPrivacyServer::Connect(TLbsProxyApiType aApiType)
    {
    TInt err = KErrNone;
        
    // Attempt to start the server and create a session
	for (TInt retry = 0; retry < KPosNumberOfTrials; retry++)
		{
		TVersion version;
		if (aApiType == ELbsProxyApiTypeLocal)
			{
			version = TVersion(KLbsLocalPrivacyAPIVersionMajor,
								KLbsLocalPrivacyAPIVersionMinor,
								KLbsLocalPrivacyAPIVersionBuild);
			}
		else 
			{
			version = TVersion(KLbsNetworkPrivacyAPIVersionMajor,
						   		KLbsNetworkPrivacyAPIVersionMinor,
						   		KLbsNetworkPrivacyAPIVersionBuild);
			}
		
		err = CreateSession(KPrivacyServerName, version);
		if (err == KErrNone)
			{
			break; // We have a session
			}
		// Server already started
		if (err == KErrAlreadyExists)
			{
			err = KErrNone; //Server already started 
			break;
			}	
				 
		// Can't create a session
		if ((err == KErrNotFound) || (err == KErrServerTerminated))
			{
			// Start Lbs 
			// We're not interested in the error code returned as we must retry KPosNumberOfTrials times anyway
		    StartLbsSystem();
			}	
		}// for
	return err;
	}

/**
* Verifies that a requesting remote party is allowed to do a
* request.
* @param aRequestId An id for the request.
* @param aRequestInfo the requestInfo that should be used for
*  verification.
* @param aRequestAction The timeout decision in case a network timeout
* occurs.
* @param aStatus is set to KErrNone if the request is allowed and
* KErrAccessDenied if it is not.
*/
void RPrivacyServer::VerifyLocationRequest(TInt& aRequestId,TLbsExternalRequestInfo2& aRequestInfo,
		                                            TLbsNetPosRequestPrivacy& aNetPosRequestPrivacy,
		                                            TRequestStatus& aStatus)
	{
	iRequestInfo = aRequestInfo;
	
	iNetPosRequestPrivacy = aNetPosRequestPrivacy;
    	
	RSemaphore semaphore;

	TInt err = semaphore.CreateGlobal(KNullDesC, 0, EOwnerThread);
	if(err != KErrNone)
		{
		aStatus = KRequestPending;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
 
	TIpcArgs args(&iRequestIdPkg,&iRequestInfoPkg,&iNetPosRequestPrivacyPkg, semaphore);

	SendReceive(ELbsPrivacyServerVerifyLocation,args,aStatus);

	semaphore.Wait();
	semaphore.Close();
	aRequestId = iRequestId;
	}

/**
 * Cancels an outstanding verification request.
 * @param aRequestId An Id for the request. 
 * @param aCancelReason A cancel reason.
 * @return KErrNone, if the send operation is successful; KErrServerTerminated, 
 * if the server no longer present; KErrServerBusy, if there are no message slots available; 
 * KErrNoMemory, if there is insufficient memory available.
 */
TInt RPrivacyServer::CancelVerifyLocationRequest(TInt aRequestId , TInt aCancelReason)
	{
	TIpcArgs args(aRequestId, aCancelReason);
	return (SendReceive(ELbsPrivacyServerCancelVerifyLocationRequest, args));
    }
/**
 * Notifies the user that a service or user has made a location request.
 * This notification is used for requests that don't have been verified
 * by the privacy server.
 * @param aRequestInfo the requestInfo that should be used for notification.
 * @param aRequestId An id for the request.
 * @return KErrNone, if the send operation is successful; KErrServerTerminated, 
 * if the server no longer present; KErrServerBusy, if there are no message slots available; 
 * KErrNoMemory, if there is insufficient memory available.
 */
 
TInt RPrivacyServer::NotifyLocationRequest(TLbsExternalRequestInfo2& aRequestInfo, TInt& aRequestId,
                                                    TLbsNetPosRequestPrivacy& aNetPosRequestPrivacy)
    {
 	
	TPckg<TLbsExternalRequestInfo2> requestInfoPkg(aRequestInfo);
    TPckg<TLbsNetPosRequestPrivacy> netPosRequestPrivacyPkg(aNetPosRequestPrivacy);
	TPckg<TInt> pkgReqId(aRequestId);

	TIpcArgs args(&requestInfoPkg,&pkgReqId,&netPosRequestPrivacyPkg);
	return(SendReceive(ELbsPrivacyServerNotifyLocation, args));
    }
/**
 * Called to report that a verification timed out.
 * @param aRequestInfo the requestInfo that should be used for verification.
 * @param aRequestId The request id.
 * @param aRequestAction The reason for a notification.
 * @return KErrNone, if the send operation is successful; KErrServerTerminated, 
 * if the server no longer present; KErrServerBusy, if there are no message slots available; 
 * KErrNoMemory, if there is insufficient memory available.
 */
 
TInt RPrivacyServer::NotifyVerificationTimeout(TLbsExternalRequestInfo2& aRequestInfo,TInt aRequestId,
                                                        TLbsNetPosRequestPrivacy& aNetPosRequestPrivacy)
    {
    TPckg<TLbsExternalRequestInfo2> requestInfoPkg(aRequestInfo);
    TPckg<TLbsNetPosRequestPrivacy> netPosRequestPrivacyPkg(aNetPosRequestPrivacy);
    TIpcArgs args(&requestInfoPkg,aRequestId,&netPosRequestPrivacyPkg);
    return (SendReceive(ELbsPrivacyServerNotifyVerificationTimeout, args));
    }

/**
 * Sends an asynchronous privacy request to the Privacy Protocol Module. The method waits
 * for the server to write the request ID parameter and then returns.
 * 
 * @param aRequestID The request id
 * @param aRequestInfo The details of the request
 * @param aPrivacyRequest The actual request, the type and preferred action
 * @param aStatus The TRequestStatus that should be completed
 */
void RPrivacyServer::NewPrivacyRequest(TUint32& aRequestId, const TLbsExternalRequestInfo& aRequestInfo, 
		const TLbsNetPosRequestPrivacy& aPrivacyRequest, TRequestStatus& aStatus)
	{
	Mem::Copy(&iRequestInfo, &aRequestInfo, aRequestInfo.ClassSize());
	
	iNetPosRequestPrivacy = aPrivacyRequest;
	
	RSemaphore semaphore;

	TInt err = semaphore.CreateGlobal(KNullDesC, 0, EOwnerThread);
	if(err != KErrNone)
		{
		aStatus = KRequestPending;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
 
	TIpcArgs args(&iRequestIdUint32Pkg,&iRequestInfoPkg,&iNetPosRequestPrivacyPkg, semaphore);

	// The IPC Message is sent with a different ordinal for local and network requests
	// so that different capability checking can be done depending on the source of the request
	if(aRequestInfo.RequestSource() == TLbsExternalRequestInfo::ERequestSourceLocal)
		{
		SendReceive(ELbsPrivacyServerNewPrivacyRequestLocal,args,aStatus);
		}
	else
		{
		SendReceive(ELbsPrivacyServerNewPrivacyRequestNetwork,args,aStatus);
		}

	semaphore.Wait();
	semaphore.Close();
	aRequestId = iRequestIdUint32;
	}

/**
 * Sends an asynchronous privacy request to the Privacy Protocol Module.
 * 
 * @param aRequestID The request id
 * @param aRequestInfo The details of the request
 * @param aPrivacyRequest The actual request, the type and preferred action
 * @param aStatus The TRequestStatus that should be completed
 */
void RPrivacyServer::RepeatPrivacyRequest(TUint32 aRequestId, const TLbsExternalRequestInfo& aRequestInfo, 
		const TLbsNetPosRequestPrivacy& aPrivacyRequest, TRequestStatus& aStatus)
	{
	Mem::Copy(&iRequestInfo, &aRequestInfo, aRequestInfo.ClassSize());
	
	iNetPosRequestPrivacy = aPrivacyRequest;
 
	TIpcArgs args(aRequestId,&iRequestInfoPkg,&iNetPosRequestPrivacyPkg);
	// The IPC Message is sent with a different ordinal for local and network requests
	// so that different capability checking can be done depending on the source of the request
	if(aRequestInfo.RequestSource() == TLbsExternalRequestInfo::ERequestSourceLocal)
		{
		SendReceive(ELbsPrivacyServerRepeatPrivacyRequestLocal,args,aStatus);
		}
	else
		{
		SendReceive(ELbsPrivacyServerRepeatPrivacyRequestNetwork,args,aStatus);
		}
	}

/**
 * Tells the Privacy Protocol Module the request session is finished.
 * 
 * @param aRequestId The ID of the request session to complete
 * @param aReason The reason for which the session is ended
 */
TInt RPrivacyServer::CompleteRequest(TUint32 aRequestId, TInt aReason)
	{
	TIpcArgs args(aRequestId, aReason);
	return (SendReceive(ELbsPrivacyServerCompleteRequest, args));
	}

/**
*Close the connection to the server
*/
void RPrivacyServer::Close()
    {
    RSessionBase::Close();
    }

/* Start the LBS system
 ** open the system controller and issue the Lbsroot startup request
*/
TInt RPrivacyServer::StartLbsSystem()
	{
	RLbsSystemController systemController;
	
	TRAPD(startErr, systemController.OpenL(KLbsNetworkPrivacyApiUid));
	
	if(startErr == KErrNone)
	    {
        startErr = systemController.RequestSystemStartup();
        systemController.Close();
	    }
	
    return (startErr);
	}

