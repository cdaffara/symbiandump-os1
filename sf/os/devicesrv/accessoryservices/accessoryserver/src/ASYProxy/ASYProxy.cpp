/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Polymorphic interface to Accessory Server
*
*/


// INCLUDE FILES
#include "ASYProxyAsyHandler.h"
#include "ASYProxy.h"
#include <ecom/ecom.h>
#include "AsyProxyObserver.h"
#include "acc_debug.h"

#include <e32svr.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ASYProxy -dll construction function
EXPORT_C CASYProxyAPI* NewAccessoryHandlerProxyL();

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NewAccessoryHandlerProxyL Creates instance of CASYProxy
// Returns: self: Pointer to created instance
// -----------------------------------------------------------------------------
//
EXPORT_C CASYProxyAPI* NewAccessoryHandlerProxyL()
    {
    API_TRACE_( "[AccFW:AsyProxy] NewAccessoryHandlerProxy()" );

    CASYProxy* self = new ( ELeave ) CASYProxy();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    API_TRACE_1( "[AccFW:AsyProxy] NewAccessoryHandlerProxy - return 0x%x", self );

    return (CASYProxyAPI*) self;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CASYProxy::CASYProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxy::CASYProxy()
    : iRealASYsCount( -1 ), iLoadedRealASYsCount( 0 ), iLoadedASYsCount( 0 )
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::CASYProxy()" );
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::CASYProxy" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxy::ConstructL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::ConstructL()" );

#ifdef _DEBUG

    TRAPD( openErr, REComSession::OpenL() )
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxy::ConstructL - REComSession::OpenL() == %d", openErr );
    User::LeaveIfError( openErr );

#else

    REComSession::OpenL();

#endif // _DEBUG

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::ConstructL - return void" );
    }

// Destructor
CASYProxy::~CASYProxy()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::~CASYProxy()" );

    DestroyLoader();
    iAccHandlerArray.ResetAndDestroy();

    // RECom isn't needed any more
    REComSession::FinalClose();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::~CASYProxy" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::LoadASYs
// Launches ASYLoader to start loading ASYs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxy::LoadASYs()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::LoadASYs()" );

    iAsyLoader->IssueRequest();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::LoadASYs - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::InitializeL
// Sets pointer for observer to make possible callbacks
// Creates a new CASYProxyAsyLoader -object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxy::InitializeL( MASYProxyObserver* aAccServerObserver )
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::InitializeL()" );

    iAccServerObserver = aAccServerObserver;
    iAsyLoader = CASYProxyAsyLoader::NewL( this );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::InitializeL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::ASYInfo
// Returns implementation information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const CImplementationInformation* CASYProxy::ASYInfo( TInt aIndex ) const
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxy::ASYInfo(%d)", aIndex );

    if ( iAccHandlerArray.Count() )
        {

        COM_TRACE_1( "[AccFW:AsyProxy] CASYProxy::ASYInfo - return 0x%x", &iAccHandlerArray.operator[]( aIndex )->iImplementationInformation );

        return iAccHandlerArray.operator[]( aIndex )->iImplementationInformation;
        }

     else
        {

        COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::ASYInfo - return NULL" );

        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CASYProxy::ASYState
// Returns state of ASY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TASYProxyASYState CASYProxy::ASYState( TInt aIndex ) const
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxy::ASYState(%d)", aIndex );
    
    if ( iAccHandlerArray.Count() )
        {

        COM_TRACE_1( "[AccFW:AsyProxy] CASYProxy::ASYState - return %d", iAccHandlerArray.operator[]( aIndex )->CurrentState() );

        return iAccHandlerArray.operator[]( aIndex )->CurrentState();
        }

    else 
        {

        COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::ASYState - return TASYProxyASYStateReset");

        return EASYProxyASYReset;
        }
    }

// -----------------------------------------------------------------------------
// CASYProxy::SetASYsCount
// Sets ASYs count
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxy::SetASYsCount( TInt aCount )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxy::SetASYsCount(%d)", aCount );
    
    iRealASYsCount = aCount;

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::SetASYsCount - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::ASYCount
// Returns count of ASYs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxy::ASYsCount() const
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::ASYsCount()" );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxy::ASYsCount - return %d", iRealASYsCount );

    return iRealASYsCount;
    }

// -----------------------------------------------------------------------------
// CASYProxy::LoadCompleteL
// Inform observer that loading is completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxy::LoadCompleteL( const TInt aASYIndex )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxy::LoadCompleteL(%d)", aASYIndex );

    iLoadedASYsCount++;

    if ( aASYIndex <= ASYsCount() - 1 )
        {
        iLoadedRealASYsCount++;

        TBool lastASYComplete =
              ( iLoadedRealASYsCount == ASYsCount() ) ? ETrue : EFalse;

        iAccServerObserver->LoadCompleteL( aASYIndex, lastASYComplete );
        }

    DestroyLoader();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::LoadCompleteL" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::ThreadKills
// Inform observer that loading is completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxy::ThreadKills( const TInt aASYIndex )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxy::ThreadKills(%d)", aASYIndex );

    iAccServerObserver->ThreadKills( aASYIndex );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::ThreadKills" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::LoadTimeOutL
// Inform observer that loading is timed out
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxy::LoadTimeOutL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::LoadTimeOutL()" );

    if ( ( iLoadedRealASYsCount < ASYsCount() ) || ( ASYsCount() < 0 ) )
        {
        iAccServerObserver->LoadingTimedOutL();
        }

    DestroyLoader();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::LoadTimeOutL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxy::DestroyLoader
// Deletes the ASY loader instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxy::DestroyLoader()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::DestroyLoader()" );
    
    TInt handlerCount( 0 );
    handlerCount = iAccHandlerArray.Count();

    if ( iAsyLoader && iLoadedASYsCount == handlerCount )
        {
        delete iAsyLoader;
        iAsyLoader = 0;
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxy::DestroyLoader" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
