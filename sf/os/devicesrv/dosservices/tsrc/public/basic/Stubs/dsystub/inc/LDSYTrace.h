/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Licensee DSY stub traces
*
*/


#ifndef LDSYTRACE_H
#define LDSYTRACE_H

// INCLUDES
#include <e32svr.h>

// PC-LINT OPTIONS
// Removes null statement not in line by itself warnings from 
// COMPONENT_TRACE macros
//lint -esym(960,54)

// Removes "area too small" pointer cast warnings.
//lint -e826



// CONSTANTS
// MACROS
#ifdef _DEBUG

    void TraceAssertFunc( const TDesC8& aFile, TInt aLine );

    #ifdef COMPONENT_TRACE_FLAG

        #define COMPONENT_TRACE_THIS_FILE ThisFileFunc( _L8( __FILE__ ) )

    #else //#ifdef COMPONENT_TRACE_FLAG

        #define COMPONENT_TRACE_THIS_FILE

    #endif //#ifdef COMPONENT_TRACE_FLAG

    #define TRACE_ASSERT( a ) if ( !( a ) ) TraceAssertFunc( _L8( __FILE__ ), __LINE__ )
    #define TRACE_ASSERT_RETURN( a ) if ( !( ( a ) == KErrNone ) ) TraceAssertFunc( _L8( __FILE__ ), __LINE__ )
    #define TRACE_ASSERT_ALWAYS TraceAssertFunc( _L8( __FILE__ ), __LINE__ )

    #ifdef COMPONENT_TRACE_FLAG

        #define COMPONENT_TRACE( a ) RDebug::Print a 

    #else // #ifdef COMPONENT_TRACE_FLAG

        #define COMPONENT_TRACE( a )

    #endif //#ifdef COMPONENT_TRACE_FLAG

#else // #ifdef _DEBUG

    #define TRACE_ASSERT( a )
    #define TRACE_ASSERT_RETURN( a ) a
    #define TRACE_ASSERT_ALWAYS
    #define COMPONENT_TRACE( a )
    #define COMPONENT_TRACE_THIS_FILE

#endif //#ifdef _DEBUG



// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS



#endif //#ifndef LDSYTRACE_H



//  End of File
