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
* Description:  Private, Accessory Policy Generic ID Array Accessor
*
*/


// INCLUDE FILES
#include    "AccPolGenericIDArrayAccessor.h"
#include    "AccPolGenericIDAccessor.h"
#include    "acc_debug.h"

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
// TAccPolGenericIDArrayAccessor::TAccPolGenericIDArrayAccessor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TAccPolGenericIDArrayAccessor::TAccPolGenericIDArrayAccessor()
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::TAccPolGenericIDArrayAccessor()" );
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::TAccPolGenericIDArrayAccessor() - return void" );
    }
    
// -----------------------------------------------------------------------------
// TAccPolGenericIDArrayAccessor::ReplaceArrayL
// Replace the member array with the given one.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDArrayAccessor::ReplaceArrayL( 
    TAccPolGenericIDArray& aGenericIDArray, 
    const RArray<TAccPolGenericID>& aNewArray )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::ReplaceArrayL()");
    TInt newArrayCount( aNewArray.Count() );
    if (  newArrayCount > KMaxGenericIDInstances )
        {
        // Array size greater than maximum size
        COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::ReplaceArrayL() new array size greater than maximum size, leaving" );
        User::Leave( KErrOverflow );
        }
    else
        {
        TInt i( 0 );
        for ( ; i < newArrayCount; ++i )
        {
            aGenericIDArray.iGenericIDArray[i] = aNewArray[i];
            }
            TAccPolGenericID emptyID;
            while ( i < KMaxGenericIDInstances )
                {
                aGenericIDArray.iGenericIDArray[i] = emptyID;
                ++i;
                }
        }
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::ReplaceArrayL() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDArrayAccessor::CopyGenericIDArray
// Copy generic ID array instance to another.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDArrayAccessor::CopyGenericIDArray( 
    const TAccPolGenericIDArray& aSource, 
    TAccPolGenericIDArray& aTarget )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::CopyGenericIDArrayL()" );

    for ( TInt i( 0 ); i < KMaxGenericIDInstances; ++i )
        {
        aTarget.iGenericIDArray[i] = aSource.iGenericIDArray[i];
        }

    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::CopyGenericIDArrayL() - return void" );
    }
    

EXPORT_C void TAccPolGenericIDArrayAccessor::RemoveIndexFromGenericIDArray( 
    TAccPolGenericIDArray& aGenericIDArray, 
    TInt  aIndex) 
    { 
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::RemoveIndexFromGenericIDArray()" ); 
    
    TInt iArrayCount( aGenericIDArray.Count() );    
    TInt i(aIndex);
    
    if( (iArrayCount > 0) && (i >= 0) ) 
    	{
	    for ( ; i < (iArrayCount-1); ++i ) 
	        { 
	        aGenericIDArray.iGenericIDArray[i] = aGenericIDArray.iGenericIDArray[i+1]; 
	        } 
    	TAccPolGenericID emptyID; 
    	aGenericIDArray.iGenericIDArray[i] = emptyID; 
    	} 

    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::RemoveIndexFromGenericIDArray() - return void" ); 
    } 



// -----------------------------------------------------------------------------
// TAccPolGenericIDArrayAccessor::FindWithHWDeviceIDL
// Look for a Generic ID from the array with device ID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccPolGenericIDArrayAccessor::FindWithHWDeviceIDL( 
    const TAccPolGenericIDArray& aGenericIDArray, 
    const TUint64& aHWDeviceID )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::FindWithHWDeviceIDL()" );

    TInt retIndex( KErrNotFound );
    TInt count( aGenericIDArray.Count() );
    
    for ( TInt i( 0 ); i < count; ++i )
        {
        if (  aGenericIDArray.GetGenericIDL( i ).HWDeviceID() == aHWDeviceID )
            {
            retIndex = i;
            break; // from for loop
            }
        }

    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::FindWithHWDeviceIDL() - return %d", retIndex );
    return retIndex;
    }
    
// -----------------------------------------------------------------------------
// TAccPolGenericIDArrayAccessor::FindWithDeviceAddressL
// Look for a Generic ID from the array with Device Address.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccPolGenericIDArrayAccessor::FindWithDeviceAddressL( 
    const TAccPolGenericIDArray& aGenericIDArray, 
    const TUint64& aDeviceAddress )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::FindWithDeviceAddressL()" );
    
    TInt retIndex( KErrNotFound );
    TInt count( aGenericIDArray.Count() );

    for ( TInt i( 0 ); i < count; ++i )
        {
        if (  aGenericIDArray.GetGenericIDL( i ).DeviceAddress() == aDeviceAddress )
            {
            retIndex = i;
            break; // from for loop
            }
        }

    COM_TRACE_1( "AccFW: [ACCPOLICY] TAccPolGenericIDArrayAccessor::FindWithDeviceAddressL() - return %d", retIndex );
    return retIndex;
    }
            
// -----------------------------------------------------------------------------
// TAccPolGenericIDArrayAccessor::FindWithUniqueIDL
// Look for a Generic ID from the array with Unique ID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccPolGenericIDArrayAccessor::FindWithUniqueIDL( 
    const TAccPolGenericIDArray& aGenericIDArray, 
    TInt aUnigueID )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::FindWithUniqueIDL()" );

    TInt retIndex( KErrNotFound );
    TInt count( aGenericIDArray.Count() );
    
    for ( TInt i( 0 ); i < count; ++i )
        {
        if (  aGenericIDArray.GetGenericIDL( i ).UniqueID() == aUnigueID )
            {
            retIndex = i;
            break; // from for loop
            }
        }

    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericIDArrayAccessor::FindWithUniqueIDL() - return %d", retIndex );
    return retIndex;
    }

//  End of File  

