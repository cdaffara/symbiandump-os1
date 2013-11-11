/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  An implementation class for issuing Mediator Service commands.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorCommandInitiatorBody.h"
#include    "MediatorServerClient.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

CMediatorCommandInitiatorBody::CMediatorCommandInitiatorBody(
                MMediatorCommandResponseObserver* aObserver )
    : CActive( EPriorityStandard ),
      iObserver( aObserver ),
      iCommandDataPtr( NULL, 0 ),
      iCategoryBuffer( iCategory ),
      iStatusBuffer( iResponseStatus ),
      iCommandBuffer( iCommand )
    {
    }

void CMediatorCommandInitiatorBody::ConstructL()
    {
    CActiveScheduler::Add( this );
    User::LeaveIfError( iMediatorServer.Connect() );
    ResetDataBufferL( KMaxCommandData ); 
    }

CMediatorCommandInitiatorBody* CMediatorCommandInitiatorBody::NewL(
                MMediatorCommandResponseObserver* aObserver )
    {
    CMediatorCommandInitiatorBody* self 
                = new( ELeave ) CMediatorCommandInitiatorBody( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorCommandInitiatorBody::~CMediatorCommandInitiatorBody()
    {
    Cancel();
    iMediatorServer.Close();
    delete iCommandData;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandInitiatorBody::DoCancel
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorCommandInitiatorBody::DoCancel()
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorCommandInitiatorBody::DoCancel\n")));
    iMediatorServer.Cancel();  
    }





// -----------------------------------------------------------------------------
// CMediatorCommandInitiatorBody::IssueCommandL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorCommandInitiatorBody::IssueCommand( TUid aDomain,
                                                  TUid aCategory, 
                                                  TInt aCommandId,
                                                  TVersion aVersion, 
                                                  const TDesC8& aData )
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorCommandInitiatorBody::IssueCommandL\n"))); 
    
    // Check that we are not already active
    if ( IsActive() )
        {
        ERROR_LOG(_L("[Mediator] CMediatorCommandIntiatorBody::IssueCommand: IsActive returned KErrInUse\n"));
        return KErrInUse;
        }
    
    // Initiate response waiting before command, otherwise there will be a concurrency problem
    // with clients that issue response in command handler
    WaitForCommandResponse();
    
    // Send the command to Mediator                                 
    TInt status = iMediatorServer.IssueCommand( aDomain, 
                                                aCategory,
                                                aCommandId, 
                                                aVersion,
                                                aData );
    
    // Cancel command waiting request
    if ( status != KErrNone )
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorCommandIntiatorBody::IssueCommand: status=%d\n"), status ) );
        // Cancel wait for response in server
        iMediatorServer.CancelCommand ( aDomain, aCategory, aCommandId );
        Cancel();
        }
    
    return status;
    }
    
// -----------------------------------------------------------------------------
// CMediatorCommandInitiatorBody::CancelCommand
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorCommandInitiatorBody::CancelCommand( TUid aDomain, 
                                                        TUid aCategory, 
                                                        TInt aCommandId )
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorCommandInitiatorBody::CancelCommand\n")));
    
    // Send the cancellation
    iMediatorServer.CancelCommand( aDomain, aCategory, aCommandId );
    Cancel(); // cancel also the pending request, so that subsequent commands will be served 
    
    }
// -----------------------------------------------------------------------------
// CMediatorCommandInitiatorBody::RunL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorCommandInitiatorBody::RunL()
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorCommandInitiatorBody::RunL status %d\n"), iStatus.Int() )); 
    
    // in case of error, no callbacks
    if ( iStatus.Int() >= 0 && iObserver )
        {
        // Check the parameter data size. If bigger than expected, fetch it synchronously
        TInt dataSize = iStatus.Int();
        if ( dataSize > iCommandDataPtr.MaxLength() )
            {
            // Reset data buffer for bigger size
            ResetDataBufferL( dataSize );
            
            // Fetch data from Mediator
            iMediatorServer.FetchParameterData( iCommandDataPtr );
            }
        iObserver->CommandResponseL( iCategory.iDomain,
                                     iCategory.iCategory,
                                     iCommand.iCommandId, 
                                     iResponseStatus,
                                     *iCommandData );                                  
        }
    }

// -----------------------------------------------------------------------------
// CMediatorCommandInitiatorBody::RunError
// This will be called when there is a leave from ResetDataBufferL() or CommandResponseL() from RunL
// 
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CMediatorCommandInitiatorBody::RunError( TInt aError )
#else
TInt CMediatorCommandInitiatorBody::RunError( TInt /*aError*/ )
#endif //_DEBUG
	{
	ERROR_TRACE(Print(_L("[Mediator Server]\t CMediatorCommandInitiatorBody::RunError called with Error %d\n"), aError ));
	//Ignore the error.
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CMediatorCommandInitiatorBody::WaitForCommandResponse
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorCommandInitiatorBody::WaitForCommandResponse()
    {
    // To be sure no duplicate requests are raised
    if ( IsActive() )
        {
        return;
        }
    iMediatorServer.WaitForCommandResponse( iStatus,
                                            iCategoryBuffer,
                                            iCommandBuffer,
                                            iCommandDataPtr,
                                            iStatusBuffer );
    SetActive();                                            
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponder::ResetDataBufferL
// Starts to receive commands asynchronously
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMediatorCommandInitiatorBody::ResetDataBufferL( TInt aSize )
    {
    if ( iCommandData )
        {
        delete iCommandData;
        iCommandData = NULL;
        }
    iCommandData = HBufC8::NewL( aSize );
    iCommandDataPtr.Set( iCommandData->Des() );
       
    }


//  End of File  
