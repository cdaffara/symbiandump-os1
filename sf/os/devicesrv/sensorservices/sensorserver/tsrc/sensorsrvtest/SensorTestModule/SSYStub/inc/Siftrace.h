/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration and implementation of SIF trace macros
*
*/

#ifndef SIFTRACE_H
#define SIFTRACE_H

// Additional Lint configuration options are defined here.
//lint -e1714 Member function not referenced.
//   - CTFTestTimer, CTFActiveTest, copy constructors, assignment operators
//lint -e1526 Member function not defined.
//   - Copy constructors, assignment operators
//lint -e714  Symbol not referenced
//   - uid, LibEntryL
//lint -e1711 Class has a virtual function but is not inherited
//   - Inherited in the project that implements the test cases
//lint -e755  Macro not referenced
//   - Some trace macros
//lint -e769  Enumeration constant not referenced
//   - Panic codes are not referenced in WINS
//lint -e1716 Virtual member function not referenced
//   - Referenced b the project that implements the test cases
//lint -esym(960, 69) Variable number of arguments
//   - Log writer functions contain ellipsis
//lint -esym(960, 54) Null statement by itself
//   - When COMPONENT_TRACE is not enabled
//lint -esym(960, 59) Left brace expected for if, else, for, do and while
//   - TRACE_ASSERT macro
//lint -e1924 C-style case
//   - From _L macros
//lint -e666 Expression with side effects
//   - From TEST_CASE_TRACE

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

// Global enumeration constant not referenced
//lint -e769

// Virtual member not referenced
//lint -e1716

// Expression with side effects passed to macro
//lint -e666

// Enumeration not referenced
//lint -e758

// Declaration could be moved from header to module
//lint -e759


// CONSTANTS
// MACROS
#ifdef _DEBUG

    inline void TraceAssertFunc( const TDesC8& aFile, TInt aLine )
        {
        HBufC* buffer = HBufC::New( aFile.Length() + 1 );
        HBufC* date = HBufC::New( 32 );
        HBufC* time = HBufC::New( 32 );
        if ( buffer != NULL && date != NULL && time != NULL )
            {
            buffer->Des().Copy( aFile );
            date->Des().Copy( _L8( __DATE__ ) );
            time->Des().Copy( _L8( __TIME__ ) );
            RDebug::Print( _L( "Assertion failed: file=%s, line=%d, compiled=%s %s" ), buffer->Des().PtrZ(), aLine, date->Des().PtrZ(), time->Des().PtrZ() );
            }
        else
            {
            RDebug::Print( _L( "Assertion and memory allocation failed" ) );
            }
        delete buffer;
        delete date;
        delete time;
        }

    inline void ThisFileFunc( const TDesC8& aFile )
        {
        HBufC* buffer = HBufC::New( aFile.Length() + 1 );
        HBufC* date = HBufC::New( 32 );
        HBufC* time = HBufC::New( 32 );
        if ( buffer != NULL && date != NULL && time != NULL )
            {
            buffer->Des().Copy( aFile );
            date->Des().Copy( _L8( __DATE__ ) );
            time->Des().Copy( _L8( __TIME__ ) );
            RDebug::Print( _L( "File=%s, compiled=%s %s" ), buffer->Des().PtrZ(), date->Des().PtrZ(), time->Des().PtrZ() );
            }
        delete buffer;
        delete date;
        delete time;
        }

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

	#ifdef TEST_CASE_TRACE_FLAG

		#define TEST_CASE_TRACE( a ) RDebug::Print a 
	
	#else
	
		#define TEST_CASE_TRACE( a )
		
	#endif //#ifdef TEST_CASE_TRACE_FLAG

    #ifdef API_TRACE_FLAG

        #define API_TRACE( a ) RDebug::Print a

    #else //#ifdef API_TRACE_FLAG

        #define API_TRACE( a )

    #endif //#ifdef API_TRACE_FLAG

#else // #ifdef _DEBUG

    #define TRACE_ASSERT( a )
    #define TRACE_ASSERT_RETURN( a ) a
    #define TRACE_ASSERT_ALWAYS
	#define TEST_CASE_TRACE( a )
    #define COMPONENT_TRACE( a )
    #define API_TRACE( a )
    #define COMPONENT_TRACE_THIS_FILE

#endif //#ifdef _DEBUG



// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

#endif //#ifndef ccftrace.h

//  End of File
