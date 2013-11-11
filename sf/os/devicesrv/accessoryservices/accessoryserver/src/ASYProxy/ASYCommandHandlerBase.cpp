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
* Description:  Pure virtual command handler base class for ASY
*
*/


// INCLUDE FILES
#include "ASYProxyCommandObserver.h"
#include "acc_debug.h"
#include <AsyCommandHandlerBase.h>
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
// CASYCommandHandlerBase::CASYCommandHandlerBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CASYCommandHandlerBase::CASYCommandHandlerBase()
    {
    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::CASYCommandHandlerBase()" );
    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::CASYCommandHandlerBase" );
    }

// -----------------------------------------------------------------------------
// CASYCommandHandlerBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYCommandHandlerBase::ConstructL(
        CASYMainServiceBase* aAsyMainServiceBase,
        CASYProxyCommandObserver* aASYProxyCommandObserver )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::ConstructL(0x%x)", aASYProxyCommandObserver );

    iASYProxyCommandObserver = aASYProxyCommandObserver;
    iAsyMainServiceBase = aAsyMainServiceBase;

    COM_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYCommandHandlerBase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYCommandHandlerBase* CASYCommandHandlerBase::NewL(
                        CASYMainServiceBase* aAsyMainServiceBase,
                        CASYProxyCommandObserver* aASYProxyCommandObserver,
                        TUid aImplementationUidAsyHandler
                        )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::NewL(0x%x)", aASYProxyCommandObserver );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::NewL - Before REComSession::CreateImplementationL - 0x%x", aImplementationUidAsyHandler.iUid );

    TAny* tmpEcom = NULL;

#ifdef _DEBUG

    TRAPD( createErr, tmpEcom = REComSession::CreateImplementationL(
                                              aImplementationUidAsyHandler,
                                              _FOFF( CASYCommandHandlerBase,
                                              iDtor_ID_Key ) ) );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::NewL - After REComSession::CreateImplementationL - createErr == %d", createErr );
    User::LeaveIfError( createErr );

#else

    tmpEcom = REComSession::CreateImplementationL(
                            aImplementationUidAsyHandler,
                            _FOFF( CASYCommandHandlerBase,
                            iDtor_ID_Key ) );

#endif // _DEBUG

    CASYCommandHandlerBase* theChosenOne = NULL;

    if ( tmpEcom )
        {
        theChosenOne = reinterpret_cast <CASYCommandHandlerBase*> ( tmpEcom );
        theChosenOne->ConstructL( aAsyMainServiceBase, aASYProxyCommandObserver );
        }

    COM_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::NewL - return 0x%x", &theChosenOne );
    return theChosenOne;
    }

// Destructor
EXPORT_C CASYCommandHandlerBase::~CASYCommandHandlerBase()
    {
    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::~CASYCommandHandlerBase()" );

    REComSession::DestroyedImplementation(iDtor_ID_Key);

    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::~CASYCommandHandlerBase" );
    }

// -----------------------------------------------------------------------------
// CASYCommandHandlerBase::ProcessCommandL
// ASY implements
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CASYCommandHandlerBase::ProcessCommandL(
                                      const TProcessCmdId /*aCommand*/,
                                      const TASYCmdParams& /*aCmdParams*/ )
    {
    // Pure virtual method
    }
    
// -----------------------------------------------------------------------------
// CASYCommandHandlerBase::ProcessResponseL
// Overloaded method for response of TBool -type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CASYCommandHandlerBase::ProcessResponseL(
                                      const TAccValueTypeTBool aCmdResponse,
                                      const TInt aErrCode )
    {
    API_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL( %d )", aCmdResponse );
    API_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL( %d )", aErrCode );

    iASYProxyCommandObserver->DoProcessResponseL( aCmdResponse, aErrCode );

    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYCommandHandlerBase::ProcessResponseL
// Overloaded method for response of TInt -type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CASYCommandHandlerBase::ProcessResponseL(
                                      const TAccValueTypeTInt aCmdResponse,
                                      const TInt aErrCode )
    {
    API_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL( %d )", aCmdResponse );
    API_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL( %d )", aErrCode );

    iASYProxyCommandObserver->DoProcessResponseL( aCmdResponse, aErrCode );

    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYCommandHandlerBase::ProcessResponseL
// Overloaded method for response of TDesC8 -type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CASYCommandHandlerBase::ProcessResponseL(
                                      const TDesC8& aCmdResponse,
                                      const TInt aErrCode )
    {
#ifdef _DEBUG  
    TBuf8<KTraceMaxSize> buf;
    TInt size( aCmdResponse.Size() );
    if( size > KTraceMaxSize )
        {
        size = KTraceMaxSize;//COM_TRACE_RAW_1 supports KTraceMaxSize
        }
    buf.Append(_L("[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL(): "));
    buf.Append( aCmdResponse.Ptr(), size );
    COM_TRACE_RAW_( buf );
#endif // _DEBUG    
    API_TRACE_1( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL( %d )", aErrCode );

    iASYProxyCommandObserver->DoProcessResponseL( aCmdResponse, aErrCode );

    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYCommandHandlerBase::ProcessResponseL
// Overloaded method for response of CAccPolObjectBase -type
// -----------------------------------------------------------------------------
//
EXPORT_C void CASYCommandHandlerBase::ProcessResponseL( const CAccPolObjectCon& aObjectCon, const TInt aErrCode )
    {    

    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL()" );

    iASYProxyCommandObserver->DoProcessResponseL( aObjectCon, aErrCode );

    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::ProcessResponseL - return void" );

    }
    
// -----------------------------------------------------------------------------
// CASYCommandHandlerBase::ObjectConDataFromProxyL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
EXPORT_C void CASYCommandHandlerBase::ObjectConDataFromProxyL( CAccPolObjectCon& aObjectCon ) const
    {
	  API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::ObjectConDataFromProxyL()" );

    iASYProxyCommandObserver->DoObjectContainerFromProxyL( aObjectCon );

    API_TRACE_( "[AccFW:AsyProxy] CASYCommandHandlerBase::ObjectConDataFromProxyL - return void" );
    }
    
// -----------------------------------------------------------------------------
// CASYCommandHandlerBase::ASYMainServiceBase
// Returns pointer of CASYMainServiceBase -object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CASYMainServiceBase* CASYCommandHandlerBase::ASYMainServiceBase()
    {
    return iAsyMainServiceBase;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
