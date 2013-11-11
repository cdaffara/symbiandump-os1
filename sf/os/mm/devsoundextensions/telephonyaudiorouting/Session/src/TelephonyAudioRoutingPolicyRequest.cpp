/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Active object for each asynchronous request in RTelephonyAudioRoutingSession.
*				 Notifies MTelephonyAudioRoutingPolicyObserver upon request completion.
*
*/




// INCLUDE FILES
#include "TelephonyAudioRoutingPolicyRequest.h"
#include "MTelephonyAudioRoutingPolicyObserver.h"
#include "TelephonyAudioRoutingClientServer.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingPolicyRequest::CTelephonyAudioRoutingPolicyRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
    
CTelephonyAudioRoutingPolicyRequest::CTelephonyAudioRoutingPolicyRequest( 
	RTelephonyAudioRoutingManagerSession& aManagerSession, 
	MTelephonyAudioRoutingPolicyObserver& aObserver, 
	CTelephonyAudioRoutingManager& aAudioRouting, 
	TTelAudRtngServRqst aRequest )
	:	CActive(EPriorityStandard),
		iManagerSession(aManagerSession),
		iPolicyObserver(aObserver),
		iAudioRoutingManager(aAudioRouting),
		iRequest(aRequest)
	{
	  
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingPolicyRequest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingPolicyRequest::ConstructL()
    {
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingPolicyRequest::ConstructL"));    
	CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingPolicyRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
   
CTelephonyAudioRoutingPolicyRequest* CTelephonyAudioRoutingPolicyRequest::NewL(
	RTelephonyAudioRoutingManagerSession& aManagerSession,
	MTelephonyAudioRoutingPolicyObserver& aObserver,
	CTelephonyAudioRoutingManager& aAudioRouting,
	TTelAudRtngServRqst aRequest )
    {
	CTelephonyAudioRoutingPolicyRequest* self = new( ELeave ) CTelephonyAudioRoutingPolicyRequest(aManagerSession, aObserver, aAudioRouting, aRequest);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }

// Destructor
CTelephonyAudioRoutingPolicyRequest::~CTelephonyAudioRoutingPolicyRequest()
    {
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingPolicyRequest::~CTelephonyAudioRoutingPolicyRequest"));    
	Cancel();

    }

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingPolicyRequest::CompleteRequest
// This method completes the request status and sets the object active
// to provide asynchronous behavior.
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingPolicyRequest::CompleteRequest(
	TTelAudRtngServRqst aRequest,
	TInt aError )
    {
	iRequest = aRequest;
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat, aError);
	SetActive();
    }

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingPolicyRequest::DoCancel
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingPolicyRequest::DoCancel()
    {
	iManagerSession.CancelRequest(iRequest);
    }

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingPolicyRequest::RunL
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingPolicyRequest::RunL()
    {
    TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingPolicyRequest::RunL with %d "), iStatus.Int());
	CTelephonyAudioRouting::TAudioOutput aResponse;
	
	switch( iStatus.Int() )
	{
	case ETelAudRtngServOutputChangeRequested:
		{

		aResponse = (iManagerSession.AudioOutputPkg())(); 


		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingPolicyRequest::RunL, Calling OutputChangeRequested With: %d "), aResponse);		         
        iPolicyObserver.OutputChangeRequested(iAudioRoutingManager, aResponse);
        iManagerSession.MonitorOutputChangeRequest();
		}
		break;
						
	default:
		{
		User::Panic(_L("TelephonyAudioRouting"), KErrGeneral );
		break;
		}
	}
    }

//  End of File

