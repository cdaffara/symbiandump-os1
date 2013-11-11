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
* Description:  Handler for ASY instance
*
*/


// INCLUDE FILES
#include "ASYProxy.h"
#include "ASYProxyAsyHandler.h"
#include "acc_debug.h"
#include <AsyServiceUids.h>
#include <e32svr.h>
#include <e32def.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KMinASYHeapSize      = 0x2000;
const TInt KDefaultASYHeapSize  = 0x8000;
const TInt KMaxASYHeapSize      = 0x8000;

const TInt KMinASYStackSize     = 0x1000;
const TInt KDefaultASYStackSize = 0x2000;
const TInt KMaxASYStackSize     = 0x4000;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CASYProxyASYHandler::CASYProxyASYHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxyASYHandler::CASYProxyASYHandler(
      CASYProxy* aASYProxy,
      const TInt aIndex,
      const TUid aHandlerUid,
      const CImplementationInformation& aImplementationInformation )

    : iHandlerUid( aHandlerUid ),
      iASYProxy( aASYProxy ),    
      iASYState( EASYProxyASYReset ),
      iIndex( aIndex ),
      iHeapValue( KDefaultASYHeapSize ),
      iStackValue( KDefaultASYStackSize )
    {
    COM_TRACE_4( "[AccFW:AsyProxy] CASYProxyASYHandler::CASYProxyASYHandler(0x%x, %d, 0x%x, 0x%x )", aASYProxy, aIndex, aHandlerUid.iUid, &aImplementationInformation );
    
    iImplementationInformation = const_cast<CImplementationInformation*>
                                           ( &aImplementationInformation );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyASYHandler::CASYProxyASYHandler( 0x%x )", &iImplementationInformation );

    SetMemoryValues();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::CASYProxyASYHandler" );
    }

// -----------------------------------------------------------------------------
// CASYProxyASYHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyASYHandler::ConstructL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::ConstructL()" );

    LoadASYL();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyASYHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyASYHandler* CASYProxyASYHandler::NewL(
        CASYProxy* aASYProxy,
        const TInt aIndex,
        const TUid aHandlerUid,
        const CImplementationInformation& aImplementationInformation )
    {
    COM_TRACE_4( "[AccFW:AsyProxy] CASYProxyASYHandler::CASYProxyASYHandler(0x%x, %d, 0x%x, 0x%x )", aASYProxy, aIndex, aHandlerUid.iUid, &aImplementationInformation );

    CASYProxyASYHandler* self = new ( ELeave ) CASYProxyASYHandler(
                                               aASYProxy,
                                               aIndex,
                                               aHandlerUid,
                                               aImplementationInformation
                                               );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyASYHandler::NewL - return 0x%x", self );

    return self;
    }

// Destructor
CASYProxyASYHandler::~CASYProxyASYHandler()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::~CASYProxyASYHandler()" );

    if ( iASYServiceMain )
        {
        // Cancel main service
        iASYServiceMain->Cancel();
        delete iASYServiceMain;
        iASYServiceMain = NULL;
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::~CASYProxyASYHandler" );
    }

// -----------------------------------------------------------------------------
// CASYProxyASYHandler::SetMemoryValues
// Sets heap and stack memory values for a new thread
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyASYHandler::SetMemoryValues()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::SetMemoryValues()" );

    TBuf8<KMaxImplementationInfoDataLength> bufOpaqData;
    bufOpaqData = iImplementationInformation->OpaqueData();

    TInt tmpHeapMemValue( 0 );
    TInt tmpStackMemValue( 0 );

    _LIT8( KMatchHeapAndStack, "Heap=0x????|Stack=0x????" );
    _LIT8( KMatchHeap,         "Heap=0x????" );
    _LIT8( KMatchStackAndHeap, "Stack=0x????|Heap=0x????" );
    _LIT8( KMatchStack,        "Stack=0x????" );

    _LIT8(KHeap,"Heap=0x");
    _LIT8(KStack,"Stack=0x");

    if ( ( KErrNotFound != bufOpaqData.Match( KMatchHeapAndStack ) ) ||
         ( KErrNotFound != bufOpaqData.Match( KMatchHeap ) ) ||
         ( KErrNotFound != bufOpaqData.Match( KMatchStackAndHeap ) ) ||
         ( KErrNotFound != bufOpaqData.Match( KMatchStack ) ) )
        {
        tmpHeapMemValue = ReadMemoryItem( bufOpaqData, KHeap );
        tmpStackMemValue = ReadMemoryItem( bufOpaqData, KStack );

        if ( ( tmpHeapMemValue != KErrGeneral ) &&
             ( tmpStackMemValue != KErrGeneral ) )
            {

            if ( ( tmpHeapMemValue > KMinASYHeapSize) &&
                 ( tmpHeapMemValue < KMaxASYHeapSize ) )
                {
                iHeapValue = tmpHeapMemValue;
                }

            if ( ( tmpStackMemValue > KMinASYStackSize) &&
                 ( tmpStackMemValue < KMaxASYStackSize ) )
                {
                iStackValue = tmpStackMemValue;
                }
            }
        }
    else
        {
        COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::SetMemoryValues - Opaque data was empty or in wrong format." );
        }

    COM_TRACE_2( "[AccFW:AsyProxy] CASYProxyASYHandler::SetMemoryValues(iHeapValue == 0x%x, iStackValue == 0x%x)", iHeapValue, iStackValue );
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::SetMemoryValues - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyASYHandler::ReadMemoryItem
// Sets heap and stack memory values for a new thread
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyASYHandler::ReadMemoryItem( const TDesC8& aOpaqData,
                                          const TDesC8& aMemKey )
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::ReadMemoryItem( TDesC8, TDesC8 )" );

    TPtrC8 ptrValue;
    TChar tmpChar;
    TLex8 lex;
    TBool hDigitsOK( ETrue );
    TUint memValue( 0 );
    const TInt KValueLength( 4 );
    TInt valueBeg(0);
    TInt valueFind( 0 );
    TInt retValue( KErrGeneral );

    valueFind = aOpaqData.Find( aMemKey );
    if ( KErrNotFound != valueFind )
        {
        valueBeg = valueFind + aMemKey.Length();
        ptrValue.Set( aOpaqData.Mid( valueBeg, KValueLength ) );

        for ( TInt i=0; i < KValueLength; i++ )
            {
            tmpChar = ptrValue[i];
            if ( !tmpChar.IsHexDigit() )
                {
                hDigitsOK = EFalse;
                COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::ReadMemoryItem - Opaque data was in wrong format." );
                break;
                }
            }

        if ( hDigitsOK )
            {
            lex.Assign( ptrValue );
            if ( !lex.Val( memValue, EHex ) )
                {
                retValue = memValue;
                }
            }
        }
    else
        {
        retValue = 0;
        }

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyASYHandler::ReadMemoryItem - return 0x%x", retValue );

    return retValue;
    }

// -----------------------------------------------------------------------------
// CASYProxyASYHandler::LoadAsysL
// Creates a new CASYProxyMainService object and orders it to start a new thread
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyASYHandler::LoadASYL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::LoadASYL()" );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyASYHandler::LoadASYL - iIndex == %d", iIndex );
    COM_TRACE_2( "[AccFW:AsyProxy] CASYProxyASYHandler::LoadASYL[%d] - ImplementationUid == 0x%x", iIndex, iImplementationInformation->ImplementationUid() );
    COM_TRACE_2( "[AccFW:AsyProxy] CASYProxyASYHandler::LoadASYL[%d] - iHandlerUid == 0x%x", iIndex, iHandlerUid );
    COM_TRACE_2( "[AccFW:AsyProxy] CASYProxyASYHandler::LoadASYL[%d] - dllUid == display_name == %S", iIndex, &iImplementationInformation->DisplayName() );

    COM_TRACE_2( "[AccFW:AsyProxy] CASYProxyASYHandler::LoadASYL[%d] - iHeapValue == 0x%x", iIndex, iHeapValue );
    COM_TRACE_2( "[AccFW:AsyProxy] CASYProxyASYHandler::LoadASYL[%d] - iStackValue == 0x%x", iIndex, iStackValue );

    iASYServiceMain = CASYProxyMainService::NewL( this );

    iASYServiceMain->StartThreadL(
                     iImplementationInformation->ImplementationUid(),
                     iHandlerUid,
                     iHeapValue,
                     iStackValue );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::LoadASYL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyASYHandler::UpdateASYStateL
// Updates the status information of ASY
// Informs ASYProxy about changes of status
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyASYHandler::UpdateASYStateL( TASYProxyASYState aASYState )
    {
    COM_TRACE_2( "[AccFW:AsyProxy] CASYProxyASYHandler::UpdateASYStateL(iIndex == %d, aAsyState == %d)", iIndex, aASYState );

    iASYState = aASYState;

    if ( aASYState == EASYProxyASYConnectionReady )
        {
        iASYProxy->LoadCompleteL( iIndex );
        }

    else if ( aASYState == EASYProxyASYThreadKills )
        {
        iASYProxy->ThreadKills( iIndex );
        }

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::UpdateASYStateL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyASYHandler::CurrentState
// Returns the current status of ASY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TASYProxyASYState CASYProxyASYHandler::CurrentState() const
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::CurrentState()" );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyASYHandler::CurrentState - iIndex == %d", iIndex );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyASYHandler::CurrentState - return %d", iASYState );
    return iASYState;
    }

// -----------------------------------------------------------------------------
// CASYProxyASYHandler::Index
// Returns the index of this ASY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyASYHandler::Index() const
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyASYHandler::Index()" );
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyASYHandler::Index - return %d", iIndex );
    return iIndex;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
