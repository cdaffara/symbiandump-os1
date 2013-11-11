// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//



//  INCLUDE FILES
#include "cmmtsyfax.h"
#include "cmmfaxext.h"


// ======== MEMBER FUNCTIONS ========

CMmTsyFax::CMmTsyFax(
    CMmFaxExt* aFaxExt ) 
    :  iFaxExt( aFaxExt )
    
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_CTOR_1 "TSY: CMmTsyFax::CMmTsyFax" );
    }

void CMmTsyFax::ConstructL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_CONSTRUCTL_1, "TSY: CMmTsyFax::ConstructL");
    
    iTerminate     = EFalse;
    iDataDirection = EUnknown;

    //  Used to be that CTsyFax could not be constructed before a call had
    //  been initiated i.e. the fax server was started and the call had a 
    //  pointer to CFaxSession. Now, to solve the problem of the fax progress
    //  being only available after phase C, RFax can be opened before a call
    //  has begun to connect. So in the event of this happening, each function
    //  of CTsyFax must check that the fax server has been started.
    if ( iFaxExt->GetFaxSession() )
        {
        RFax::TProgress* faxProgress = iFaxExt->iMmCall->
          CreateFaxProgressChunk();
        if ( !faxProgress )
            {
            User::Leave( KErrEtelFaxChunkNotCreated );
            }
        }
    }

CMmTsyFax* CMmTsyFax::NewL(
        CMmFaxExt* aFaxExt )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_NEWL_1, "TSY: CMmTsyFax::NewL");
    //  Completes successfully if fax call has already been dialled creating
    //  a CFaxSession instance, and is not called if that is not true.
    CMmTsyFax* fax = new ( ELeave ) CMmTsyFax( aFaxExt );

    CleanupClosePushL( *fax );
    fax->ConstructL();
    CleanupStack::Pop();
    
    return fax;
    }

CMmTsyFax::~CMmTsyFax()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_DTOR_1, "TSY: CMmTsyFax::~CMmTsyFax" );

	if( iFaxExt )
		{
		iFaxExt->iFax = NULL;
		iFaxExt = NULL;
		}
    }

// ---------------------------------------------------------------------------
// CMmTsyFax::CloseFax
// Utility to cleanup stack. Called if NewL fails and CleanupStack unwinds.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmTsyFax::CloseFax(
        TAny* aObj )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_CLOSEFAX_1, "TSY: CMmTsyFax::CloseFax");
    
    ( ( CObject* )aObj )->Close();
    }

// ---------------------------------------------------------------------------
// CMmTsyFax::DeregisterNotification
// Deregisters notification in use.
// Returns: Error value
// ---------------------------------------------------------------------------
//
TInt CMmTsyFax:: DeregisterNotification(
        const TInt /*aIpc*/ )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, DEREGISTERNOTIFICATION_1, "TSY: CMmTsyFax::DeregisterNotification");
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmTsyFax::Init
// Fax initialisation. Etel server calls this.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmTsyFax::Init()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_INIT_1, "TSY: CMmTsyFax::Init");
    }

// ---------------------------------------------------------------------------
// CMmTsyFax::Read
// Read fax data into descriptor, asynchronously.
// Returns: None
// ---------------------------------------------------------------------------
//
TInt CMmTsyFax::Read(
        const TTsyReqHandle aTsyReqHandle, 
        TDes8* aDes )                      
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_READ_1, "TSY: CMmTsyFax::Read : %d" , aTsyReqHandle);

    TInt errorCode( KErrNone );

    if ( !iFaxExt->GetFaxSession()
         || iFaxExt->GetCallStatus() != RCall::EStatusConnected )
        {
        // not ready
        ReqCompleted( aTsyReqHandle, KErrNotReady );
        }
    else
        {
        if( iTerminate )
          {
          // terminated
          errorCode = KErrGeneral;
          }
        else
          {
          // Prevent the machine powering down due to inactivity during
          // a long fax receive.
          User::ResetInactivityTime();
          iDataDirection = EReceiveData;
          CFaxSession* session = iFaxExt->GetFaxSession();
          // Request handle for CMmFaxCompletion object.
          iFaxExt->ConfigureCompletion( aTsyReqHandle, this );
          session->RxFaxData( *aDes );
          }
        }
    return errorCode;

    }
    
// ---------------------------------------------------------------------------
// CMmTsyFax::RegisterNotification
// Registers notification in use.
// Returns: Error value
// ---------------------------------------------------------------------------
//
TInt CMmTsyFax::RegisterNotification
        (
        const TInt /*aIpc*/ 
        )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_REGISTERNOTIFICATION_1, "TSY: CMmTsyFax::RegisterNotification");
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// CMmTsyFax::ReqModeL
// Returns: Request mode
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmTsyFax::ReqModeL(
        const TInt aIpc ) 
    {
    return CFaxBase::ReqModeL( aIpc );
    }

// ---------------------------------------------------------------------------
// CMmTsyFax::Terminate
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmTsyFax::Terminate()
	{    
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_TERMINATE_1, "TSY: CMmTsyFax::Terminate");
	iTerminate = ETrue;
	}

// ---------------------------------------------------------------------------
// CMmTsyFax::TerminateFaxSession
// Cancel read/write operation on CMmFaxCompletion object
// Returns: Error value
// ---------------------------------------------------------------------------
//
TInt CMmTsyFax::TerminateFaxSession(
        const TTsyReqHandle aTsyReqHandle ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_TERMINATEFAXSESSION_1, "TSY: CMmTsyFax::TerminateFaxSession Handle: %d", aTsyReqHandle);
    
    CFaxSession* session = iFaxExt->GetFaxSession();

    if ( !session )
        {
        ReqCompleted( aTsyReqHandle, KErrNotReady );
        } 
    else
        {
        session->Cancel();
        // This will cancel ongoig read/write operation on CMmFaxCompletion
        // object.
        iFaxExt->CompleteOperation( KErrCancel );
        //This completes this method and upper ongoing operation.
        ReqCompleted( aTsyReqHandle,KErrNone );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmTsyFax::WaitForEndOfPage
// Ask fax server to send notification of end page of fax.
// Returns: Error value
// ---------------------------------------------------------------------------
//
TInt CMmTsyFax::WaitForEndOfPage(
        const TTsyReqHandle aTsyReqHandle ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_WAITFORENDOFPAGE_1, "TSY: CMmTsyFax::WaitForEndOfPage  Handle:%d", aTsyReqHandle);

    if ( !iFaxExt->GetFaxSession() 
         || iFaxExt->GetCallStatus() != RCall::EStatusConnected )
        {
        // not ready
        ReqCompleted( aTsyReqHandle, KErrNotReady );
        }
    else
        {
        switch ( iDataDirection )
            {
            case EReceiveData:
                iFaxExt->ConfigureCompletion( aTsyReqHandle, this );
                iFaxExt->GetFaxSession()->RxPostPage();
                break;
            case ESendData:
                iFaxExt->ConfigureCompletion( aTsyReqHandle, this );
                iFaxExt->GetFaxSession()->TxPostPage();
                break;
            default:
                ReqCompleted( aTsyReqHandle, KErrUnknown );
                break;
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmTsyFax::Write
// Send fax data in descriptor to fax module, asynchronously.
// Returns: Error value
// ---------------------------------------------------------------------------
//
TInt CMmTsyFax::Write(
        const TTsyReqHandle aTsyReqHandle, 
        TDesC8* aDes )                     
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMTSYFAX_WRITE_1, "TSY: CMmTsyFax::Write : %d" , aTsyReqHandle);

    TInt errorCode( KErrNone );

    if ( !iFaxExt->GetFaxSession() 
         || iFaxExt->GetCallStatus() != RCall::EStatusConnected )
        {
        // not ready
        ReqCompleted( aTsyReqHandle, KErrNotReady );
        errorCode = KErrNone;
        }
    else
        {
        if( iTerminate )
            {
            errorCode = KErrGeneral;
            }
        else
            {
            // Prevent the machine powering down due to inactivity during a 
            // long fax send.
            User::ResetInactivityTime();
            iDataDirection = ESendData;
            CFaxSession* session = iFaxExt->GetFaxSession();
            // Request handle for CMmFaxCompletion object.
            iFaxExt->ConfigureCompletion( aTsyReqHandle, this );
            session->TxFaxData( *aDes );
            }
        }
    return errorCode;
    }

// ================= OTHER EXPORTED FUNCTIONS ===============================

//  End of File 
