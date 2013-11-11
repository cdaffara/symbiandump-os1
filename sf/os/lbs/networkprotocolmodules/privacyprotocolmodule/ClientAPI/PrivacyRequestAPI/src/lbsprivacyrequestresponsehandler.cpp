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
// Class implementation of CPrivacyRequestResponseHandler
// 
//


// INCLUDE FILES
#include "lbsprivacyclient.h"
#include "lbsprivacyrequestresponsehandler.h"
#include "lbsprivacyserverreturncodes.h"
#include "lbsdevloggermacros.h"

//-----------------------------------------------------------------------------
// CPrivacyRequestResponseHandler
//-----------------------------------------------------------------------------

// Default constructor 
 CPrivacyRequestResponseHandler::CPrivacyRequestResponseHandler(CLbsPrivacyRequestImpl& aParent, 
		                       RPrivacyServer& aPrivacyServer,
		                       MLbsPrivacyRequestObserver& aObserver) 
    
    : CActive(EPriorityStandard),iParent(aParent), iPrivacyServer(aPrivacyServer), iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

  // Static constructor
 CPrivacyRequestResponseHandler* CPrivacyRequestResponseHandler::NewLC(CLbsPrivacyRequestImpl& aParent, 
                               RPrivacyServer& aPrivacyServer,
                               MLbsPrivacyRequestObserver& aObserver)
    {
    CPrivacyRequestResponseHandler* self = new (ELeave) CPrivacyRequestResponseHandler(aParent, aPrivacyServer,
    		                                          aObserver);
    CleanupStack::PushL(self);
    return self;
    }

 // Destructor
 CPrivacyRequestResponseHandler::~CPrivacyRequestResponseHandler()
    {
    Cancel();
    }

void CPrivacyRequestResponseHandler::NewPrivacyRequest(TUint32& aRequestId, const TLbsExternalRequestInfo& aRequestInfo, 
		const TLbsNetPosRequestPrivacy& aPrivacyRequest)
	{
    iPrivacyServer.NewPrivacyRequest(aRequestId, aRequestInfo, aPrivacyRequest, iStatus);
    SetActive();
	iRequestId = aRequestId;
	}

void CPrivacyRequestResponseHandler::RepeatPrivacyRequest(TUint32 aRequestId, const TLbsExternalRequestInfo& aRequestInfo, 
		const TLbsNetPosRequestPrivacy& aPrivacyRequest)
	{
	iRequestId = aRequestId;
    iPrivacyServer.RepeatPrivacyRequest(aRequestId, aRequestInfo, aPrivacyRequest, iStatus);
    SetActive();
	}

/**
  * From CActive. Called when verification is complete
  */
 void CPrivacyRequestResponseHandler::RunL()
    {
    TInt response = iStatus.Int();
    LBSLOG(ELogP9, "<-A CPrivacyRequestResponseHandler::RunL() LbsPrivacyProtocolModule\n");
    LBSLOG2(ELogP8, "\t > TInt iStatus = %d\n", response);

    if(response == KRequestCancelledByClient)
    	{ // If client cancelled then we don't need to call back
    	return;
    	}
    
    if(response >= 0)
    	{
    	iObserver.OnPrivacyResponse(iRequestId, response, KErrNone);
    	}
    else
    	{
    	iObserver.OnPrivacyResponse(iRequestId, 
    				MLbsPrivacyRequestObserver::EPrivacyResponseRejected, response);
    	}
    iParent.RemoveHandler(this);
    delete this;
    }

/**
* From CActive. Cancels the outstanding verification request.
*/
 void CPrivacyRequestResponseHandler::DoCancel()
    {
	// Cancel this request in the server
	iPrivacyServer.CompleteRequest(iRequestId, KErrCancel);
    }

/**
* From CActive. Calls this function if RunL() function leaves
*/
 TInt CPrivacyRequestResponseHandler::RunError(TInt aError)
    {
    return aError;
    }
