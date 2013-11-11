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
 * Description:  Public, Accessory Policy Name Value Record
 *
 */

// INCLUDE FILES
#include "AccPolNameValueRecord.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TAccPolNameValueRecord::TAccPolNameValueRecord
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolNameValueRecord::TAccPolNameValueRecord()
    {
    API_TRACE_( "[ACCPOLICY] TAccPolNameValueRecord::TAccPolNameValueRecord() default" );
    }

EXPORT_C TAccPolNameValueRecord::TAccPolNameValueRecord( const TAccPolNameRecord aNameRecord,
    const TAccPolValueRecord aValueRecord ):
        iNameRecord( aNameRecord ),
        iValueRecord( aValueRecord )
    {
    API_TRACE_( "[ACCPOLICY] TAccPolNameValueRecord::TAccPolNameValueRecord() with name and value record" );
    }

EXPORT_C TAccPolNameValueRecord::TAccPolNameValueRecord( const TUint32& aName,
    TInt aValue,
    TAccPolValueType aType,
    TAccPolValueLocation aLocation ) :
        iValueRecord( aValue, aType, aLocation )
    {
    API_TRACE_( "[ACCPOLICY] TAccPolNameValueRecord::TAccPolNameValueRecord()" );
    iNameRecord.SetName( aName );
    }

EXPORT_C TAccPolNameValueRecord::TAccPolNameValueRecord( const TUint32& aName,
    TDesC8& aValue,
    TAccPolValueLocation aLocation ) :
        iValueRecord( aValue, aLocation )
    {
    API_TRACE_( "[ACCPOLICY] TAccPolNameValueRecord::TAccPolNameValueRecord() desc" );
    iNameRecord.SetName( aName );
    }

EXPORT_C TAccPolNameValueRecord::TAccPolNameValueRecord( const TUint32& aName ) :
    iValueRecord()
    {
    API_TRACE_( "[ACCPOLICY] TAccPolNameValueRecord::TAccPolNameValueRecord() no value" );
    iNameRecord.SetName( aName );
    }

EXPORT_C TAccPolNameValueRecord::TAccPolNameValueRecord( const TUint32& aName,
    TAccPolValueLocation aLocation,
    CAccPolObjectCon* aValue ) :
        iValueRecord( aLocation, aValue )
    {
    API_TRACE_( "[ACCPOLICY] TAccPolNameValueRecord::TAccPolNameValueRecord() CAccPolObjectCon" );
    iNameRecord.SetName( aName );
    }

// -----------------------------------------------------------------------------
// TAccPolNameValueRecord::NameRecord
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolNameRecord TAccPolNameValueRecord::NameRecord() const
    {
    API_TRACE_( "[ACCPOLICY] TAccPolNameValueRecord::NameRecord() return" );
    return iNameRecord;
    }

// -----------------------------------------------------------------------------
// TAccPolNameValueRecord::ValueRecord
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolValueRecord TAccPolNameValueRecord::ValueRecord() const
    {
    API_TRACE_( "[ACCPOLICY] TAccPolNameValueRecord::ValueRecord() return" );
    return iValueRecord;
    }

//  End of File  
