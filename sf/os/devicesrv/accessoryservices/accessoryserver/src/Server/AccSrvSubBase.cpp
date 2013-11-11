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
* Description:  Server-side sub-session base class
*
*/



// INCLUDE FILES
#include "AccSrvSubBase.h"
#include "AccSrvMainSession.h"
#include "AccClientServerMessages.h"
#include "AccSrvSubBase.h"
#include "AccPolGenericIDAccessor.h"
#include "AccPolGenericIDArrayAccessor.h"
#include "acc_debug.h"

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
// CAccSrvSubBase::CAccSrvSubBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubBase::CAccSrvSubBase( CASYProxyAPI* aProxy,
                                CAccSrvConnectionController* aConnectionController,
                                CAccSrvServerModel* aServerModel ) :
    iConnectionController( aConnectionController ),
    iServerModel( aServerModel ),
    iProxy( aProxy )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::CAccSrvSubBase()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::CAccSrvSubBase - return" );
    }

// Destructor
CAccSrvSubBase::~CAccSrvSubBase()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::~CAccSrvSubBase()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubBase::Write
// -----------------------------------------------------------------------------
//
void CAccSrvSubBase::Write( const RMessage2& aMessage, TInt aFlag )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubBase::Write(%d)", aFlag );

    TInt flags( 0 );
    TInt err( KErrNone );
    TPckg<TInt> flagsPckg( flags );

    err = aMessage.Read( KAccServParamPosThird, flagsPckg );//It's allways argument number 2 which is used in client side
    
    if( KErrNone == err)
        {
        flags = flags & ( ~aFlag );//Turn bit number aFlag to zero
        aMessage.Write( KAccServParamPosThird, flagsPckg );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::Write - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubBase::CompleteMessageL
//
// If this base class implementation is called, something is wrong.
// Function is defined here to avoid unnecessary implementation
// in derived classes.
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubBase::CompleteMessageL( TMsgID /*aMsgID*/,
                                        TInt /*aErrorCode*/,
                                        TInt /*aUniqueID*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::CompleteMessageL()" );

    TRACE_ASSERT_ALWAYS;
    User::Leave( KErrGeneral );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::CompleteMessageL - return EFalse" );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CAccSrvSubBase::CompleteProcessCommandL
//
// If this base class implementation is called, something is wrong.
// Function is defined here to avoid unnecessary implementation
// in derived classes.
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubBase::CompleteProcessCommandL( TProcessCmdId /*aCmdn*/,
                                               TInt /*aTrId*/,
                                               TASYCommandParamRecord& /*aASYCommandParamRecord*/,
                                               TPtr8* /*aPtrBuf*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::CompleteProcessCommandL()" );

    TRACE_ASSERT_ALWAYS;
    User::Leave( KErrGeneral );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::CompleteProcessCommandL - return EFalse" );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CAccSrvSubBase::HandleValueMessageL
//
// If this base class implementation is called, something is wrong.
// Function is defined here to avoid unnecessary implementation
// in derived classes.
// -----------------------------------------------------------------------------
//
void CAccSrvSubBase::HandleValueMessageL( TMsgID /*aMsgID*/,
                                          TAccValueTypeTBool /*aCmdResponse*/,
                                          TInt /*aErrorCode*/,
                                          TInt /*aTrId*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::HandleValueMessageL() - TBool" );

    TRACE_ASSERT_ALWAYS;
    User::Leave( KErrGeneral );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::HandleValueMessageL - TBool - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubBase::HandleValueMessageL
//
// If this base class implementation is called, something is wrong.
// Function is defined here to avoid unnecessary implementation
// in derived classes.
// -----------------------------------------------------------------------------
//
void CAccSrvSubBase::HandleValueMessageL( TMsgID /*aMsgID*/,
                                          TAccValueTypeTInt /*aCmdResponse*/,
                                          TInt /*aErrorCode*/,
                                          TInt /*aTrId*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::HandleValueMessageL() - TInt" );

    TRACE_ASSERT_ALWAYS;
    User::Leave( KErrGeneral );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::HandleValueMessageL - TInt - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubBase::HandleValueMessageL
//
// If this base class implementation is called, something is wrong.
// Function is defined here to avoid unnecessary implementation
// in derived classes.
// -----------------------------------------------------------------------------
//
void CAccSrvSubBase::HandleValueMessageL( TMsgID /*aMsgID*/,
                                          TDesC8& /*aCmdResponse*/,
                                          TInt /*aErrorCode*/,
                                          TInt /*aTrId*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::HandleValueMessageL() - TDesC8t" );

    TRACE_ASSERT_ALWAYS;
    User::Leave( KErrGeneral );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::HandleValueMessageL - TDesC8 - return void" );
    }

void CAccSrvSubBase::HandleValueMessageL( TMsgID /*aMsgID*/,
                                          TInt /*aTrId*/, 
                                          TPtr8* /*aPtrBuf*/,
                                          TInt /*aErrorCode*/ )
{
	  COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::HandleValueMessageL() - Object" );

    TRACE_ASSERT_ALWAYS;
    User::Leave( KErrGeneral );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBase::HandleValueMessageL - Object - return void" );
}                                          
                                          
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
