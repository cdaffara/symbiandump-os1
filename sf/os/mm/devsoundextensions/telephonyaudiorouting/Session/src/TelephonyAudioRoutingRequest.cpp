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
*				 Notifies MTelephonyAudioObserver upon request completion.
*
*/




// INCLUDE FILES
#include "TelephonyAudioRoutingRequest.h"
#include "MTelephonyAudioRoutingObserver.h"
#include "TelephonyAudioRoutingClientServer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingRequest::CTelephonyAudioRoutingRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTelephonyAudioRoutingRequest::CTelephonyAudioRoutingRequest(
	RTelephonyAudioRoutingSession& aSession,
	MTelephonyAudioRoutingObserver& aObserver,
	CTelephonyAudioRouting& aAudioRouting,
	TTelAudRtngServRqst aRequest )
	:	CActive(EPriorityStandard),
		iSession(aSession),
		iObserver(aObserver),
		iAudioRouting(aAudioRouting),
		iRequestOpcode(aRequest)
	{
	  
    }
    

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingRequest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingRequest::ConstructL()
    {
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest::ConstructL"));    
	CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTelephonyAudioRoutingRequest* CTelephonyAudioRoutingRequest::NewL(
	RTelephonyAudioRoutingSession& aSession,
	MTelephonyAudioRoutingObserver& aObserver,
	CTelephonyAudioRouting& aAudioRouting,
	TTelAudRtngServRqst aRequest )
    {
	CTelephonyAudioRoutingRequest* self = new( ELeave ) CTelephonyAudioRoutingRequest(aSession, aObserver, aAudioRouting, aRequest);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }
    
// Destructor
CTelephonyAudioRoutingRequest::~CTelephonyAudioRoutingRequest()
    {
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest::~CTelephonyAudioRoutingRequest"));       

	Cancel();

    }

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingRequest::DoCancel
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingRequest::DoCancel()
    {
	iSession.CancelRequest(iRequestOpcode);
    }

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingRequest::RunL 
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingRequest::RunL()
    {
    TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest::RunL with: %d"), iStatus.Int());

	TPckgBuf<TInt> errPkgBuf;
	TInt error = KErrNone;

	switch( iStatus.Int() )
	{
	case ETelAudRtngServSetOutputComplete:
		{
 		TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t RunL: Handling SetOutputComplete"));		
    	errPkgBuf = iSession.ErrPkg();
    	error = errPkgBuf(); 
    	if (error != KErrNone)
    	{
    		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest: SetOutput Completed with an ERROR: %d"),error); 
    		iSession.ResetShowNoteMode(); // don't allow retrieval if errored   		
    	}
    	else  // Set iSession's showNoteMode if there's no error
    	{
    		iSession.SetShowNoteMode();	
    	}
    
       	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest: Will Call Observer's SetOutputComplete..."));  			
 		iObserver.SetOutputComplete(iAudioRouting, error );       
		}
		break;
		
	case ETelAudRtngServOutputChanged:
		{	
		TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t RunL: Handling OutputChanged"));
    	iSession.SetShowNoteMode(); // Set flag any time adaptation initiates output change
    			
		TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest: Call Observer's OutputChanged..."));
    	iSession.NotifyIfOutputChanged();		
       	iObserver.OutputChanged(iAudioRouting);
    	}
		break;
		
	case ETelAudRtngServAvailableOutputsChanged:
		{
		TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest: Call Observer's AvailableOutputsChanged..."));
    	iSession.NotifyIfAvailableOutputsChanged();		
		iObserver.AvailableOutputsChanged(iAudioRouting);
		}
		break;
		
	case KErrPermissionDenied:
		{
		TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest: ERROR: Permission Denied..."));
  		iObserver.SetOutputComplete(iAudioRouting, KErrPermissionDenied );  
  	//	User::Leave(KErrPermissionDenied);
		}
		break;
								
	case KErrCancel:
    TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest: KErrCancel"));	
		iObserver.SetOutputComplete(iAudioRouting, KErrCancel );      
		break;
				
	default:
		{
	    TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingRequest::RunL: Default - Error Case!! "));
		User::Panic(_L("TelephonyAudioRouting"), KErrGeneral );
		break;
		}
	}
    }

//  End of File

