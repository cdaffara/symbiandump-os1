/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handler for NotifyProcessCommands and responses
*
*/


// INCLUDE FILES
#include "ASYProxyCommandObserver.h"
#include "ASYProxyCommsService.h"
#include "ASYProxyTRRecord.h"
#include "acc_debug.h"
#include <e32svr.h>
#include <s32mem.h>
#include <accpolobjectcon.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::CASYProxyCommandObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxyCommandObserver::CASYProxyCommandObserver():
    CActive( CActive::EPriorityStandard ),
    iObjectBuf( NULL ),
    iObjectBufPtr( NULL, 0, 0 )
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::CASYProxyCommandObserver()" );

    CActiveScheduler::Add( this );
    iAllowResponse = KErrNone;

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::CASYProxyCommandObserver" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::ConstructL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::ConstructL()" );
        
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::ConstructL() - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::ConstructL(
                               CASYProxyCommsService* aAsyProxyCommsSrvc )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::ConstructL(0x%x)", aAsyProxyCommsSrvc );

    ConstructL();

    iAsyProxyCommsSrvc = aAsyProxyCommsSrvc;

    if ( !iAsyProxyCommsSrvc )
        {
        SetActive();
        iStatus = KRequestPending;
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyCommandObserver* CASYProxyCommandObserver::NewL(
                          CASYProxyCommsService* aAsyProxyCommsSrvc )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::NewL(0x%x)", aAsyProxyCommsSrvc );

    CASYProxyCommandObserver* self = new ( ELeave ) CASYProxyCommandObserver();
    CleanupStack::PushL( self );
    self->ConstructL( aAsyProxyCommsSrvc );
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::NewL - return 0x%x", self );

    return self;
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyCommandObserver* CASYProxyCommandObserver::NewL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::NewL()" );

    CASYProxyCommandObserver* self = new ( ELeave ) CASYProxyCommandObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::NewL - return 0x%x", &self );

    return self;
    }

// Destructor
CASYProxyCommandObserver::~CASYProxyCommandObserver()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::~CASYProxyCommandObserver()" );

    Cancel();
    
    if ( iAsyProxyCommsSrvc && iAsyProxyCommsSrvc->iAsyCmdHndlr )
        {
        delete iAsyProxyCommsSrvc->iAsyCmdHndlr;
        iAsyProxyCommsSrvc->iAsyCmdHndlr = 0;
        }

    if( iObjectBuf )
        {
        delete iObjectBuf;
        iObjectBuf = NULL;
        }
        
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::~CASYProxyCommandObserver" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::MarkForFinalDestruction
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::MarkForFinalDestruction ()
    {
    iInDestructionPhase = ETrue;
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::DoCancel
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::DoCancel()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoCancel()" );

    if ( iAsyProxyCommsSrvc )
        {
        iAsyProxyCommsSrvc->iAsyComms.CancelNotifyProcessCommand();
    
        if ( !iInDestructionPhase )
            {
            COM_TRACE_( "[AccFW:AsyProxy] Proxy Command Observation is Cancelled while Destruction is not in Progress" );
        #ifdef _DEBUG
            TRAPD( errUpdate, iAsyProxyCommsSrvc->UpdateAsyCommsStackL() );
            TRAPD( errCreate, iAsyProxyCommsSrvc->CreateAsyCommandHandlerL( this ) );
            
            COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoCancel - errUpdate == %d", errUpdate );
            COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoCancel - errCreate == %d", errCreate );
        #else
            TRAP_IGNORE( iAsyProxyCommsSrvc->UpdateAsyCommsStackL() );
            TRAP_IGNORE( iAsyProxyCommsSrvc->CreateAsyCommandHandlerL( this ) );
        #endif
            }
        
        iAsyProxyCommsSrvc->Destroy( KErrNotFound ); // Transaction Id is not known know
        }
        
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoCancel - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::IssueRequestL
// Asynchronously calls NotifyProcessCommand of the client
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::IssueRequestL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::IssueRequestL()" );

    iObjectBuf = CBufFlat::NewL( KAccSrvObjectBaseStreamBufGranularity );   
    iObjectBuf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
    iObjectBufPtr.Set( iObjectBuf->Ptr( 0 ) );
    iAsyProxyCommsSrvc->iAsyComms.NotifyProcessCommand(
                                  iStatus,
                                  iAsyProxyTRRecord->iParams,
                                  iObjectBufPtr );                                  
    iStatus = KRequestPending;                              
    SetActive();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::IssueRequestL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::RunL
// Calls ProcessCommandL of ASYProxy handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::RunL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::RunL()" );

    TInt func( iStatus.Int() );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::RunL - func == %d", func );

    // NotifyProcessCommand() has been completed with KErrNone
    if ( func == KErrNone ||
         func == KErrOverflow ) //KErrOverflow means that iObjectBuf is too small
        {
        
        TInt timeOutTrId( KErrNone );
        timeOutTrId = iAsyProxyTRRecord->TimeOutTrId();        
        
        if( timeOutTrId )
            {
            COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::RunL - timeout transaction id == %d", timeOutTrId );	
            DoDestroy( timeOutTrId );	            
            }	        
        else
            {
        	  COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::RunL() - no timeout" );
            }
                        
        // Creates a new CASYProxyCommandObserver objects
        // Adds it to ASYProxyCommsStack
        // Launches a new IssueRequest() for just created observer objects
        iAsyProxyCommsSrvc->UpdateAsyCommsStackL();

        // Create new transaction
        iAsyProxyCommsSrvc->CreateAsyCommandHandlerL( this );

        if( ECmdGetObjectValue == iAsyProxyTRRecord->CmdId() ||
            ECmdSetObjectValue == iAsyProxyTRRecord->CmdId() )
            {            
            
            if( func == KErrOverflow )
                {
                //buf size was not enough.
                //Read the new buffer size and get the buffer again
                API_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::RunL - 2nd attempt" );
                RBufReadStream rStrm;
                (void) rStrm.Open( *iObjectBuf );
                CleanupClosePushL( rStrm );    
                TInt newBufferSize( rStrm.ReadInt32L() );
                CleanupStack::PopAndDestroy( &rStrm );

                iObjectBuf->Reset();
                iObjectBuf->ResizeL( newBufferSize );
                TPtr8 ptr( iObjectBuf->Ptr( 0 ) );

                iAsyProxyCommsSrvc->iAsyComms.GetBuffer( ptr );                   
                }
            
	            iAsyProxyCommsSrvc->iAsyCmdHndlr->ProcessCommandL( iAsyProxyTRRecord->CmdId(),
                                                                   iAsyProxyTRRecord->AsyCmdParams());
            }
        else
            {
            // These must came from server:
            //     TProcessCmdId
            //     TASYCmdParams
            iAsyProxyCommsSrvc->iAsyCmdHndlr->ProcessCommandL(
                                         iAsyProxyTRRecord->CmdId(),
                                         iAsyProxyTRRecord->AsyCmdParams() );
            }
        }

    // NotifyProcessCommand() has been completed with KErrCancel
    else if ( func == KErrCancel )
        {
        // Do not care about the transaction id
        // All transactions will be canceled
        DoDestroy( KDeleteAllRecords );
        CActiveScheduler::Stop();
        }

    // else just do nothing more

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::RunL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::RunError
// Returns KErrNone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyCommandObserver::RunError( TInt aError )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::RunError(%d)", aError );

     // Avoid Panic in CActiveScheduler
    aError = KErrNone;

    if( iObjectBuf )
        {
        delete iObjectBuf;
        iObjectBuf = NULL;
        }
    //
    // If process command fails, mark the record.
    //
    TInt trId = iAsyProxyTRRecord->TrId();
    DoDestroy( trId );
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::RunError - return KErrNone " );

    return aError;
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::DoProcessResponseL
// Overloaded method for processing responses of TBool -type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::DoProcessResponseL(
                               const TAccValueTypeTBool aCmdResponse,
                               const TInt aErrCode )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL(%d)", aCmdResponse );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL(%d)", aErrCode );
    
    TInt trId = iAsyProxyTRRecord->TrId();
    DoDestroy( trId );
    
    AllowResponseL();
    iAsyProxyCommsSrvc->iAsyComms.ProcessResponse(
                                  trId,
                                  aCmdResponse,
                                  aErrCode );
    

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::DoProcessResponseL
// Overloaded method for processing responses of TInt -type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::DoProcessResponseL(
                               const TAccValueTypeTInt aCmdResponse,
                               const TInt aErrCode )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL(%d)", aCmdResponse );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL(%d)", aErrCode );

    TInt trId = iAsyProxyTRRecord->TrId();
    DoDestroy( trId );
    
    AllowResponseL();
    iAsyProxyCommsSrvc->iAsyComms.ProcessResponse(
                                  trId,
                                  aCmdResponse,
                                  aErrCode );


    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::DoProcessResponseL
// Overloaded method for processing responses TDesC8 -type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::DoProcessResponseL(
                               const TDesC8& aCmdResponse,
                               const TInt aErrCode )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL(%S)", &aCmdResponse );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL(%d)", aErrCode );

    TInt trId = iAsyProxyTRRecord->TrId();    
    DoDestroy( trId );
    
    AllowResponseL();
    iAsyProxyCommsSrvc->iAsyComms.ProcessResponse(
                                  trId,
                                  aCmdResponse,
                                  aErrCode );


    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::DoProcessResponseL
// Overloaded method for processing responses TDesC8 -type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::DoProcessResponseL( const CAccPolObjectCon& aObjectCon, const TInt aErrCode )
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL()");
    
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL(%d)", aErrCode );
    
    AllowResponseL();
    TInt trId = iAsyProxyTRRecord->TrId();
    DoDestroy( trId );

    if ( iObjectBuf )
        {
		    //Reset buffer
		    iObjectBuf->Reset();    
		    if ( ECmdGetObjectValue == iAsyProxyTRRecord->CmdId() )	
		        {
		        iObjectBuf->ResizeL( KAccSrvObjectBaseStreamBufMaxSize );
		        //Externalize aValue to aBuf
		        RBufWriteStream wStrm;    
		        (void) wStrm.Open( *iObjectBuf );
		        CleanupClosePushL( wStrm );    
		        aObjectCon.ExternalizeL( wStrm );
		        wStrm.CommitL();         
		        CleanupStack::PopAndDestroy( &wStrm );
		       }
		       else
		       {
		        COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL() - Nothing do to!");
		       }
		    
		    // Release allocated buffer
        TPtr8 ptr( iObjectBuf->Ptr(0) );   
        iAsyProxyCommsSrvc->iAsyComms.ProcessResponse( trId, 
                                                       &ptr,
                                                       aErrCode ); 
        delete iObjectBuf;
    	  iObjectBuf = NULL;
		    }
    else
        {
        iAsyProxyCommsSrvc->iAsyComms.ProcessResponse( trId, 
                                                       NULL,
                                                       aErrCode ); 
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoProcessResponseL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::DoObjectContainerFromProxyL
// Overloaded method for processing responses TDesC8 -type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::DoObjectContainerFromProxyL( CAccPolObjectCon& aObjectCon ) const
    {    
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoObjectContainerFromProxyL()");	
    RBufReadStream rStrm;
	  (void) rStrm.Open( *iObjectBuf );
	  CleanupClosePushL( rStrm );    
	  aObjectCon.InternalizeL( rStrm );
    CleanupStack::PopAndDestroy( &rStrm );	 
    
    //
    //Data is fetched from proxy, reset buffer.
    //
    iObjectBuf->Reset();       
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyCommandObserver::DoObjectContainerFromProxyL()");
    }    
// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::AllowResponseL
// Leaves or updates iAllowResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::AllowResponseL()
    {
    User::LeaveIfError( iAllowResponse );
    iAllowResponse = KErrAbort;
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::DoDestroy
// Calls Destroy if allowed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::DoDestroy( TInt aTrId )
    {
    iAsyProxyCommsSrvc->Destroy( aTrId );
    }

// -----------------------------------------------------------------------------
// CASYProxyCommandObserver::InitializeRef
// Initializes the iAsyProxyTRRecord
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyCommandObserver::InitializeRef(
                               CASYProxyTRRecord* aAsyProxyTRRecord )
    {
    iAsyProxyTRRecord = aAsyProxyTRRecord;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
