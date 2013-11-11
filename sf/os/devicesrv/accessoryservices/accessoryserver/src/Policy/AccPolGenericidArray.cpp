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
* Description:  Public, Accessory Policy Generic ID Array 
*
*/



// INCLUDE FILES
#include    <AccPolGenericIDArray.h>
#include    <AccPolGenericID.h>
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

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TAccPolGenericIDArray::TAccPolGenericIDArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolGenericIDArray::TAccPolGenericIDArray()
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::TAccPolGenericIDArray()" );
    TAccPolGenericID emptyInstance;
    for ( TInt i(0); i < KMaxGenericIDInstances; ++i)
        {
        iGenericIDArray[i] = emptyInstance;
        }
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::TAccPolGenericIDArray() - return" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDArray::GetGenericIDL
// Returns a pointer to the Generic ID instance from certain index of the array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TAccPolGenericID TAccPolGenericIDArray::GetGenericIDL(
         const TInt aIndex ) const  // Index of the Generic ID instance in the array 
                                    // that is requested
    {
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::GetGenericIDL() %d", aIndex );

    if ( aIndex >= KMaxGenericIDInstances || aIndex < 0)
        {
        // Out of bounds
        COM_TRACE_( "[AccFW: ACCPOLICY] Given index out of bounds - leaving" );
        User::Leave( KErrGeneral );
        }

    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::GetGenericIDL() - return:" );

    return iGenericIDArray[aIndex];
    }
        
// -----------------------------------------------------------------------------
// TAccPolGenericIDArray::Count
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccPolGenericIDArray::Count() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::Count()" );
    TInt ret( 0 );

    for( TInt i(0) ; i<KMaxGenericIDInstances ; ++i)
        {
        if( KErrNotFound != iGenericIDArray[i].UniqueID() )
            {
            ret++;
            }
        }

    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericIDArray::Count() - return %d", ret );
    return ret;
    }

//  End of File
