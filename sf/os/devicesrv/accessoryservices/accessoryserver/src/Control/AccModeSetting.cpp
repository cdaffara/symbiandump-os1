/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Support class used to deliver accessory settings related
*        parameters.
*
*/



// INCLUDE FILES
#include "acc_debug.h"
#include <AccModeSetting.h>

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
// TAccModeSetting::TAccModeSetting
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TAccModeSetting::TAccModeSetting()
    {
    API_TRACE_( "[AccFW:Client] TAccModeSetting::TAccModeSetting()" );

    iId = 0;
    iType = EAccModeSettingNotSet;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::SetId
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccModeSetting::SetId( const TUint32 aId )
    {
    API_TRACE_1( "[AccFW:Client] TAccModeSetting::SetId(%d)", aId  );

    iId = aId;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::SetTInt
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccModeSetting::SetTInt( const TInt aValue )
    {
    API_TRACE_1( "[AccFW:Client] TAccModeSetting::SetTInt(%d)", aValue );

    iValue.Num( aValue );
    iType = EAccModeSettingInt;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::SetTBool
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccModeSetting::SetTBool( const TBool aValue )
    {
    API_TRACE_1( "[AccFW:Client] TAccModeSetting::SetTBool(%d)", aValue );

    if( aValue )
        {
        _LIT( KTrue, "1" );
        TBuf<1> temp;
        temp.Append( KTrue );
        iValue = temp;
        }
    else
        {
        _LIT( KFalse, "0" );
        TBuf<1> temp;
        temp.Append( KFalse );
        iValue = temp;
        }

    iType = EAccModeSettingBool;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::SetTDes
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccModeSetting::SetTDes( const TDesC& aValue )
    {
    API_TRACE_( "[AccFW:Client] TAccModeSetting::SetTDes()" );

    if( KMaxAccModeSetting < aValue.Length() )
      {
      API_TRACE_1( "[AccFW:Client] TAccModeSetting::SetTDes() parameter aValue=%d too long", aValue.Length() );
        return KErrArgument;
      }

    iValue = aValue;
    iType = EAccModeSettingDes;

    API_TRACE_1( "[AccFW:Client] TAccModeSetting::SetTDes() returns %d", KErrNone );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::Type
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TAccModeSettingType TAccModeSetting::Type() const
    {
    API_TRACE_1( "[AccFW:Client] TAccModeSetting::Type() - returns %d", iType );

    return iType;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::Id
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TAccModeSetting::Id() const
    {
    API_TRACE_1( "[AccFW:Client] TAccModeSetting::GetId() - returns %d", iId );

    return iId;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::GetTInt
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccModeSetting::GetTInt( TInt& aValue ) const
    {
    if( iType != EAccModeSettingInt )
        {
        API_TRACE_1( "[AccFW:Client] TAccModeSetting::GetTInt() - returns %d", KErrNotFound );
        return KErrNotFound;
        }

    TLex parse( iValue );
    parse.Val( aValue );

    API_TRACE_1( "[AccFW:Client] TAccModeSetting::GetTInt() - aValue=%d", aValue );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::GetTBool
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccModeSetting::GetTBool( TBool& aValue ) const
    {
    if( iType != EAccModeSettingBool )
        {
        API_TRACE_1( "[AccFW:Client] TAccModeSetting::GetTBool() - returns %d", KErrNotFound );
        return KErrNotFound;
        }

    TInt temp( 0 );
    TLex parse( iValue );
    parse.Val( temp );
    aValue = static_cast<TBool>( temp );

    API_TRACE_1( "[AccFW:Client] TAccModeSetting::GetTBool() - aValue=%d", aValue );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// TAccModeSetting::GetTDes
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccModeSetting::GetTDes( TDes& aValue ) const
    {
    if( iType != EAccModeSettingDes )
        {
        API_TRACE_1( "[AccFW:Client] TAccModeSetting::GetTDes() - returns %d", KErrNotFound );
        return KErrNotFound;
        }

    aValue = iValue;

    API_TRACE_( "[AccFW:Client] TAccModeSetting::GetTDes() - return KErrNone" );

    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File

