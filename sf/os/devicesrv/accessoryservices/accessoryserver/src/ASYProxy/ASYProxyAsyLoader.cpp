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
* Description:  Loader of ASYs
*
*/


// INCLUDE FILES
#include "ASYProxy.h"
#include "ASYProxyAsyHandler.h"
#include "acc_debug.h"
#include <e32svr.h>

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
// CASYProxyAsyLoader::CASYProxyAsyLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxyAsyLoader::CASYProxyAsyLoader(
                    CASYProxy* aAsyProxy ) :
                        CActive( CActive::EPriorityStandard ),
                        iListRet( EFalse ),
                        iArrayItemCounter( 0 ),
                        iStepToDo( EASYLoadingStepListInit ),
                        iAsyProxy( aAsyProxy )

    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyAsyLoader::CASYProxyAsyLoader(0x%x)", &aAsyProxy );

    CActiveScheduler::Add( this );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::CASYProxyAsyLoader" );
    }

// -----------------------------------------------------------------------------
// CASYProxyAsyLoader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyAsyLoader::ConstructL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::ConstructL()" );

    iTimer = CASYProxyLoaderTimer::NewL( this );
    iTimer->StartTimer();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyAsyLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyAsyLoader* CASYProxyAsyLoader::NewL(
                    CASYProxy* aAsyProxy )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyAsyLoader::NewL(0x%x)", aAsyProxy );

    CASYProxyAsyLoader* self = new (ELeave) CASYProxyAsyLoader( aAsyProxy );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyAsyLoader::NewL - return 0x%x", self );

    return self;
    }

// Destructor
CASYProxyAsyLoader::~CASYProxyAsyLoader()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::~CASYProxyAsyLoader()" );

    Cancel();
    Cleanup();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::~CASYProxyAsyLoader" );
    }

// -----------------------------------------------------------------------------
// CASYProxyAsyLoader::DoCancel
// Cancel of Active object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyAsyLoader::DoCancel()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::DoCancel()" );

    // Cancel is called only from destructor
    // Cleanup is called from destructor

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::DoCancel - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyAsyLoader::DoCancel
// Deletes own class members
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyAsyLoader::Cleanup()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::Cleanup()" );

    delete iTimer;
    iTimer = NULL;

    iImplUidsHandlerArray.Reset();
    iServiceImplInfoArray.ResetAndDestroy();
    iHandlerImplInfoArray.ResetAndDestroy();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::Cleanup - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyAsyLoader::RunL
// Loading has been splitted to five section
// Each of them are under time out control.
// - case ListImplementations
// - case CheckListValues
// - case OrderImplementations
// - case LoadASY
// - case LoadingOver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyAsyLoader::RunL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::RunL()" );

    User::LeaveIfError( iStatus.Int() );
    
    if ( iTimer && iTimer->IsActive() )
        {
        NextStep();
        switch ( iStepToDo )
            {
            case EASYLoadingStepListImplementations :
                    ListImplementationsL();
                    SetActive();
                    break;

            case EASYLoadingStepOrderImplementations :
                    ArrangeImplementationsL();
                    SetActive();
                    break;

            case EASYLoadingStepLoadASY :
                    DoIssueRequestL();
                    SetActive();
                    break;

            case EASYLoadingOver :
                    break;

            default :
                    // Nothing to do. Assert if it comes here.
                    TRACE_ASSERT_ALWAYS;
                    break;
            }
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::RunL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyAsyLoader::RunError
// Returns KErrNone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyAsyLoader::RunError( TInt aError )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyAsyLoader::RunError(%d)", aError );

     // Avoid Panic in CActiveScheduler
    aError = KErrNone;

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::RunError - return KErrNone" );

    return aError;
    }

// -----------------------------------------------------------------------------
// CASYProxyAsyLoader::IssueRequest
// Starts the loading process of ASY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyAsyLoader::IssueRequest()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::IssueRequest()" );

    SetActive();
    TRequestStatus* ptrStat = &iStatus;
    *ptrStat = KRequestPending;

    User::RequestComplete( ptrStat, KErrNone );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::IssueRequest - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyAsyLoader::DoIssueRequestL
// Last step of loading process of ASY
// Creates own CASYProxyASYHandler -object for each ASY to continue it's loading
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyAsyLoader::DoIssueRequestL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::DoIssueRequest()" );
    
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyAsyLoader::DoIssueRequestL() - iArrayItemCounter == %d", iArrayItemCounter ); 

    if ( iArrayItemCounter < iServiceImplInfoArray.Count() )
        {
        TRequestStatus* ptrStat = &iStatus;
        *ptrStat = KRequestPending;

        CASYProxyASYHandler* createdAccHandler = NULL;

        // Starts new ASY creating
        TRAPD( err, createdAccHandler = CASYProxyASYHandler::NewL(
                iAsyProxy,
                iArrayItemCounter,
                iImplUidsHandlerArray.operator[]( iArrayItemCounter ),
                *iServiceImplInfoArray[ iArrayItemCounter ]
                ) );

        COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyAsyLoader::DoIssueRequest() - err == %d", err );
		if ( createdAccHandler )
			{
			CleanupStack::PushL ( createdAccHandler );
	        iAsyProxy->iAccHandlerArray.AppendL( createdAccHandler );
	        iArrayItemCounter++;
	        CleanupStack::Pop ( createdAccHandler );
			}
					
        User::RequestComplete( ptrStat, err );
        }
     else 
        {
        User::Leave( KErrArgument );
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::DoIssueRequest - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::ListImplementationsL
// List implementations for ASY by using RECom library
//    - List service parts to iServiceImplInfoArray
//    - List handler parts to iHandlerImplInfoArray
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyAsyLoader::ListImplementationsL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::ListImplementationsL()" );

    TRequestStatus* ptrStat = &iStatus;
    *ptrStat = KRequestPending;

    TBufC8<KMaxImplementationInfoDataLength>
            ASYPluginMainServiceName( ( TText8* ) KACCESSORYPLUGINAPIMAINSERVICE );

    TBufC8<KMaxImplementationInfoDataLength>
            ASYPluginHandlerName    ( ( TText8* ) KACCESSORYPLUGINAPIHANDLER );

     // ASY interface_uid
    TUid pluginInterfaceUid;
    pluginInterfaceUid.iUid = KACCESSORYPLUGINAPIUID;

    TEComResolverParams resolverParam;

    // MainService
    // -----------------------------------------------------
    resolverParam.SetDataType( ASYPluginMainServiceName );
    REComSession::ListImplementationsL( pluginInterfaceUid, 
                                        resolverParam, 
                                        iServiceImplInfoArray );

    // Handler
    // -----------------------------------------------------
    resolverParam.SetDataType( ASYPluginHandlerName );
    REComSession::ListImplementationsL( pluginInterfaceUid, 
                                        resolverParam, 
                                        iHandlerImplInfoArray );

    iListRet = ( iServiceImplInfoArray.Count() > 0 ) ? ETrue : EFalse;
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyAsyLoader::ListImplementationsL - count == %d", iServiceImplInfoArray.Count() );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyAsyLoader::ListImplementationsL - iListRet == %d", iListRet );

    User::RequestComplete( ptrStat, KErrNone );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::ListImplementationsL - return void");
    }

// -----------------------------------------------------------------------------
// CASYProxy::ArrangeImplementationsL
// Arranges the listed implementations
// 1. Find "Idle ASYs" ( i.e. ASY has only service part and not handler part )
//    - Service parts are in iServiceImplInfoArray
//    - Handler parts are in iHandlerImplInfoArray
// 2. Move idle ASYs to last ones within iServiceImplInfoArray
// 3. Find Service / Handler pairs
//    - Service parts are still in iServiceImplInfoArray
//    - Handler parts implementation Uids added to iImplUidsHandlerArray
// 4. Add zero values to iImplUidsHandlerArray until it's length is same as
//    length of iServiceImplInfoArray
// 5. Create CASYProxyASYHandler -array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyAsyLoader::ArrangeImplementationsL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::ArrangeImplementationsL()" );

    TRequestStatus* ptrStat = &iStatus;
    *ptrStat = KRequestPending;

    RArray<TInt> onlyServicesIndexArray;
    CleanupClosePushL( onlyServicesIndexArray );

    TBuf<KMaxImplementationInfoDataLength>  bufDisplayNameService;
    TBuf<KMaxImplementationInfoDataLength>  bufDisplayNameHandler;

    // Find implementation indexes of idle ASYs
    // ----------------------------------------------------------------
    TInt lastServiceIndex = iServiceImplInfoArray.Count() - 1;
    TInt lastHandlerIndex = iHandlerImplInfoArray.Count() - 1;

    for ( TInt i=0; i<=lastServiceIndex; i++ )
        { 
        TBool found( EFalse );
        bufDisplayNameService = iServiceImplInfoArray.operator[]( i )->DisplayName();
        for ( TInt j=0; j<=lastHandlerIndex; j++ )
            {
            bufDisplayNameHandler = iHandlerImplInfoArray.operator[]( j )->DisplayName();
            if ( KErrNotFound != bufDisplayNameService.Match( bufDisplayNameHandler ) )
                {
                found = ETrue;
                break; // Break j loop
                }
            }

        if ( found == EFalse )
            {
            onlyServicesIndexArray.AppendL( i );
            }
        }

    // ASYProxy must know the "real" ASYs count
    iAsyProxy->SetASYsCount( iServiceImplInfoArray.Count() - onlyServicesIndexArray.Count() );

    // Move idle ASYs to last ones within iServiceImplInfoArray
    // ----------------------------------------------------------------
    for ( TInt k = onlyServicesIndexArray.Count() - 1; k>=0; k-- )
        {
        CImplementationInformation *implementationInformation =
            iServiceImplInfoArray.operator[]( onlyServicesIndexArray.operator[]( k ) );

        iServiceImplInfoArray.Remove( onlyServicesIndexArray.operator[]( k ) );
        iServiceImplInfoArray.AppendL( implementationInformation );
        }

    // Find Service / Handler pairs
    // ----------------------------------------------------------------
    for ( TInt i=0; i<=lastServiceIndex; i++ )
        {
        bufDisplayNameService = iServiceImplInfoArray.operator[]( i )->DisplayName();
        for ( TInt j=0; j<=lastHandlerIndex; j++ )
            {
            bufDisplayNameHandler = iHandlerImplInfoArray.operator[]( j )->DisplayName();
            if ( KErrNotFound != bufDisplayNameService.Match( bufDisplayNameHandler ) )
                {
                TUid implementUid = iHandlerImplInfoArray.operator[]( j )->ImplementationUid();

                User::LeaveIfError( iImplUidsHandlerArray.Append( implementUid ) );
                }
            }
        }

    // Add zero -values to iImplUidsHandlerArray for the last idle ASYs
    // ----------------------------------------------------------------
    TUid zeroUid;
    zeroUid.iUid = ZEROHANDLERUID;
    while ( iImplUidsHandlerArray.Count() < iServiceImplInfoArray.Count() )
        {
        iImplUidsHandlerArray.AppendL( zeroUid );
        }

    User::RequestComplete( ptrStat, KErrNone );
    CleanupStack::PopAndDestroy( &onlyServicesIndexArray );
    // only ServicesIndexArray.Close() was called;

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::ArrangeImplementationsL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::NextStep
// Increases the loading process control splitter variable
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyAsyLoader::NextStep()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::NextStep()" );

    switch ( iStepToDo )
        {
        case EASYLoadingStepListInit :
            iStepToDo = EASYLoadingStepListImplementations;
            break;
        case EASYLoadingStepListImplementations :
            iStepToDo = iListRet ? EASYLoadingStepOrderImplementations : EASYLoadingOver;
            break;
        case EASYLoadingStepOrderImplementations :
            iStepToDo = EASYLoadingStepLoadASY;
            break;
        case EASYLoadingStepLoadASY :
            if ( iArrayItemCounter == iServiceImplInfoArray.Count() )
                {
                iStepToDo = EASYLoadingOver;
                }
            break;
        default :
            iStepToDo = EASYLoadingOver;
            break;
        }

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyAsyLoader::NextStep - iStepToDo == %d", iStepToDo );
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyAsyLoader::NextStep - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
