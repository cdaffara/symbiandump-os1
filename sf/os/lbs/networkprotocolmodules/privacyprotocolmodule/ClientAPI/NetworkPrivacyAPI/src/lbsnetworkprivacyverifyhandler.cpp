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
// Class implementation of CNetworkPrivacyVerifyHandler
// 
//


// INCLUDE FILES
#include "lbsprivacyclient.h"
#include "lbsnetworkprivacyverifyhandler.h"

//-----------------------------------------------------------------------------
// CNetworkPrivacyVerifyHandler
//-----------------------------------------------------------------------------

// Default constructor 
 CNetworkPrivacyVerifyHandler::CNetworkPrivacyVerifyHandler(
		                       RPrivacyServer& aPrivacyServer,
                               MPosVerificationObserver& aClientObserver,
                               MPosVerificationObserver& aBufferObserver) 
    
    : CActive(EPriorityStandard),iPrivacyServer(aPrivacyServer),
     iClientObserver(aClientObserver),iBufferObserver(aBufferObserver),
     iCancelReason(KErrNone)
    {
    CActiveScheduler::Add(this);
    }

  // Static constructor
 CNetworkPrivacyVerifyHandler* CNetworkPrivacyVerifyHandler::NewL(
                               RPrivacyServer& aPrivacyServer,
                               MPosVerificationObserver& aClientObserver,
                               MPosVerificationObserver& aBufferObserver)
    {
    CNetworkPrivacyVerifyHandler* self = new (ELeave) CNetworkPrivacyVerifyHandler(aPrivacyServer,
    		                                          aClientObserver,aBufferObserver);
    return self;
    }

 // Destructor
 CNetworkPrivacyVerifyHandler::~CNetworkPrivacyVerifyHandler()
    {
    CancelRequest(KErrCancel);
    }

/**
 * Start the verification request.
 * @param aRequestId Will contain the request id on return. 
 * @param aRequestInfo  The requestInfo that should be used for verification.
 * @param aRequestAction The request action in case a network timeout occurs.
 */
 void CNetworkPrivacyVerifyHandler::VerifyLocationRequest(
               TInt& aRequestId,TLbsExternalRequestInfo2& aRequestInfo,
               TLbsNetPosRequestPrivacy& aNetPosRequestPrivacy)
    {
    iPrivacyServer.VerifyLocationRequest(aRequestId,aRequestInfo,aNetPosRequestPrivacy,iStatus);
    SetActive();
    iRequestId = aRequestId;
    }
    
 /**
 Cancel this verification request.
 
 This function sends a cancel to the server to cancel the outstanding
 verification request.
 
 Note that when the server completes the verification request, the
 observer callback should *not* be called. 
 
 @param aCancelReason The reason code for the cancel.
 */
 void CNetworkPrivacyVerifyHandler::CancelRequest(TInt aCancelReason)
 	{
 	// Store the cancel reason
 	iCancelReason = aCancelReason;
 	
 	// Actual cancel is done in DoCancel(), so just call CActive::Cancel().
 	Cancel();
 	}

 /**
 * Retrieves the request id of the location request which is being verified.
 * @return The request id.
 */
 TInt CNetworkPrivacyVerifyHandler::RequestId()const
	{
	return iRequestId;
	}

 /**
  * From CActive. Called when verification is complete
  */
 void CNetworkPrivacyVerifyHandler::RunL()
    {
    TInt result = iStatus.Int();
    
    // Notify the client this request was completed,
    // but only if it wasn't cancelled by the client.
    if((result == KErrNone) && (iCancelReason == KErrNone))
    	{
    	iClientObserver.HandleVerifyComplete(iRequestId, result);
    	}
    //If the verification complete reason other than KErrNone, Notify the client 
    //this request was completed with KErrAccessDenied
    if(result != KErrNone)
        {
    	iClientObserver.HandleVerifyComplete(iRequestId,KErrAccessDenied);
    	}
    
    iBufferObserver.HandleVerifyComplete(iRequestId, result);
    }

/**
* From CActive. Cancels the outstanding verification request.
*/
 void CNetworkPrivacyVerifyHandler::DoCancel()
    {    
    // Cancel this request in the server
    TInt cancelReason(KErrCancel);
    if (iCancelReason != KErrNone)
    	{
    	cancelReason = iCancelReason;
    	}
    iPrivacyServer.CancelVerifyLocationRequest(iRequestId, cancelReason);
    }

/**
* From CActive. Calls this function if RunL() function leaves
*/
 TInt CNetworkPrivacyVerifyHandler::RunError(TInt aError)
    {
    return aError;
    }
