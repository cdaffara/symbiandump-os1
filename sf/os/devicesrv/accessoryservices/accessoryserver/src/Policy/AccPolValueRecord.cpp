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
* Description:  Public, Accessory Policy Value Record
*
*/


// INCLUDE FILES
#include <accpolobjectcon.h>
#include "AccPolValueRecord.h"
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
// TAccPolValueRecord::TAccPolValueRecord
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//    
EXPORT_C TAccPolValueRecord::TAccPolValueRecord() : 
    iValue( 0 ),
    iValueRef( KNullDesC8 ),
    iType( EAPVNone ),
    iLocation( EAPVNotDefined )
    {
	API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::TAccPolValueRecord() default" );
    }
    
EXPORT_C TAccPolValueRecord::TAccPolValueRecord( 
    TInt aValue, 
	TAccPolValueType aType, 
	TAccPolValueLocation aLocation ) :
	iValue( aValue ),
    iValueRef( KNullDesC8 ),
    iType( aType ),
    iLocation( aLocation )
    {
    API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::TAccPolValueRecord()" );   
    }
		                                 
EXPORT_C TAccPolValueRecord::TAccPolValueRecord( 
    TDesC8& aValue,
	TAccPolValueLocation aLocation ) :
	iValue( 0 ),
	iValueRef( aValue ),
    iType( EAPVDes8 ),
    iLocation( aLocation )
    {
    API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::TAccPolValueRecord() desc" );
    }
    
EXPORT_C TAccPolValueRecord::TAccPolValueRecord(     
    TAccPolValueLocation aLocation,
    CAccPolObjectCon* aValue ) :
    iValue( iValue = (TInt) aValue ),
    iValueRef( KNullDesC8 ),
    iType( EAPVObject ),
    iLocation( aLocation )
    {        
    API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::TAccPolValueRecord() CAccPolObjectBase" ); 
    }    

// -----------------------------------------------------------------------------
// TAccPolValueRecord::Value
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccPolValueRecord::Value() const
    {
    API_TRACE_1( "[ACCPOLICY] TAccPolValueRecord::Value() return %d", iValue );
	return iValue;
    }

// -----------------------------------------------------------------------------
// TAccPolValueRecord::SetValue
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolValueRecord::SetValue( TInt aValue )
    {
    API_TRACE_1( "[ACCPOLICY] TAccPolValueRecord::SetValue(%d)", aValue );
	iValue = aValue;
	API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::SetValue() return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolValueRecord::GetValueL
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolValueRecord::GetValueL( TDesC8& aValue ) const
    {
    API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::GetValueL()" );
    aValue = iValueRef;
    API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::GetValueL return void" );
    }
    
// -----------------------------------------------------------------------------
// TAccPolValueRecord::ValueRef()
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& TAccPolValueRecord::ValueRef()
    {
    API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::ValueRef()" );
    API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::ValueRef - return" );
    return iValueRef;   
    }
    
// -----------------------------------------------------------------------------
// TAccPolValueRecord::SetValue
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolValueRecord::SetValue( const TDesC8& aValue )
    {
    iValueRef.Set( aValue );
    }
		
// -----------------------------------------------------------------------------
// TAccPolValueRecord::Type
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolValueType TAccPolValueRecord::Type() const
	{
    API_TRACE_1( "[ACCPOLICY] TAccPolValueRecord::Type() return %d", iType );
	return iType;	
	}

// -----------------------------------------------------------------------------
// TAccPolValueRecord::SetType
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolValueRecord::SetType( TAccPolValueType aType )
    {
    API_TRACE_1( "[ACCPOLICY] TAccPolValueRecord::SetType(%d)", aType );
	iType = aType;
	API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::SetType - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolValueRecord::Location
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolValueLocation TAccPolValueRecord::Location() const
	{
    API_TRACE_1( "[ACCPOLICY] TAccPolValueRecord::Location() return %d", iLocation );
	return iLocation;	
	}

// -----------------------------------------------------------------------------
// TAccPolValueRecord::SetLocation
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolValueRecord::SetLocation( TAccPolValueLocation aLocation )
    {
    API_TRACE_1( "[ACCPOLICY] TAccPolValueRecord::SetLocation(%d)", aLocation );
	iLocation = aLocation;
	API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::SetLocation - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolValueRecord::SetL
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolValueRecord::SetL( 
    TInt aValue, 
	TAccPolValueType aType, 
	TAccPolValueLocation aLocation )
    {
    API_TRACE_3( "[ACCPOLICY] TAccPolValueRecord::SetL(value=%d type=%d location=%d)", aValue, aType, aLocation );
        
    iValue    = aValue;
    iType     = aType;
    iLocation = aLocation;
    
    API_TRACE_( "[ACCPOLICY] TAccPolValueRecord::SetL - return void" );
    }
		                    
//  End of File  
