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
* Description:  Declaration of DSY test tool traces
*
*/


#ifndef DSYTESTTOOLTRACE_H
#define DSYTESTTOOLTRACE_H



// INCLUDES
#include <e32svr.h>

// PC-LINT OPTIONS
// Removes null statement not in line by itself warnings from 
// COMPONENT_TRACE macros
//lint -esym(960,54)

// Removes "area too small" pointer cast warnings.
//lint -e826

// Member function not referenced (copy constructors / assignment operators)
//lint -e1714

// Member function not defined (copy constructors / assignment operators)
//lint -e1526

// Global macro not referenced
//lint -e755

// Symbol not referenced (uid)
//lint -e714

// Variable number of arguments
//lint -esym(960, 69)


// CONSTANTS
// MACROS
#ifdef _DEBUG

    void TraceAssertFunc( const TDesC8& aFile, TInt aLine );

    #define TRACE_ASSERT( a ) if ( !( a ) ) TraceAssertFunc( _L8( __FILE__ ), __LINE__ )
    #define TRACE_ASSERT_ALWAYS TraceAssertFunc( _L8( __FILE__ ), __LINE__ )

    #ifdef COMPONENT_TRACE_FLAG

        #define COMPONENT_TRACE( a ) RDebug::Print a 

    #else // #ifdef COMPONENT_TRACE_FLAG

        #define COMPONENT_TRACE( a )

    #endif //#ifdef COMPONENT_TRACE_FLAG

#else // #ifdef _DEBUG

    #define TRACE_ASSERT( a )
    #define TRACE_ASSERT_ALWAYS
    #define COMPONENT_TRACE( a )

#endif //#ifdef _DEBUG



// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS



#endif //#ifndef DSYTESTTOOLTRACE_H


//  End of File
