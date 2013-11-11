// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//




// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>
#include "a3f_trace_heap.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;
TUint32* 	TraceHeap::iTraceMask = NULL;
TUint32* 	TraceHeap::iTraceLevel = NULL;
TBool*      TraceHeap::iApiOnly = NULL;
RHeap* 		TraceHeap::iTraceHeap = NULL;


// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TraceHeap::CreateL(const TUint aDefaultMask)
    {
    // check that the heap is not already created
	if ( TraceHeap::iTraceHeap )
	    {
	    return;
	    }
	    
	// create a heap private to the owning process (heap can also be created as global)
	TraceHeap::iTraceHeap = UserHeap::ChunkHeap( NULL, KMinHeapSize, KMinHeapSize, KMinHeapGrowBy );
	__ASSERT_ALWAYS( TraceHeap::iTraceHeap, User::Leave(KErrGeneral));
	
	// create iTrace... variables
	TraceHeap::iTraceMask = (TUint32*)TraceHeap::iTraceHeap->AllocL( sizeof(TUint32) );
	*TraceHeap::iTraceMask = aDefaultMask;
    TraceHeap::iTraceLevel = (TUint32*)TraceHeap::iTraceHeap->AllocL( sizeof(TUint32) );
	*TraceHeap::iTraceLevel = 0;		
    TraceHeap::iApiOnly = (TBool*)TraceHeap::iTraceHeap->AllocL( sizeof(TBool) );
	*TraceHeap::iApiOnly = EFalse;		
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TraceHeap::TraceMask()
	{
	return *TraceHeap::iTraceMask;
	}

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TraceHeap::SetTraceMask(
	const TUint32 aTraceMask )
	{
    if ( !TraceHeap::iTraceHeap )
        {
        TRAP_IGNORE(TraceHeap::CreateL());   
        }
	*TraceHeap::iTraceMask = aTraceMask;
	}

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TraceHeap::IsMaskOn(
	const TUint32 aMask )
	{
	return ( *TraceHeap::iTraceMask & aMask );
	}

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TraceHeap::SetLevel( const TUint32 aTraceLevel )
    {
    *TraceHeap::iTraceLevel = aTraceLevel;
    }
    
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool TraceHeap::IsBelowLevel( const TUint32 aLevel )
    {
    return (*TraceHeap::iTraceLevel > aLevel);
    }
    
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
EXPORT_C TUint32 TraceHeap::TraceLevel()
    {
    return *TraceHeap::iTraceLevel;
    }
    
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool TraceHeap::IsApiOnly()
    {
    return *iApiOnly;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
EXPORT_C void TraceHeap::SetApiOnly( const TBool aApiOnly)
    {
    *iApiOnly = aApiOnly;
    }

//  End of File  

