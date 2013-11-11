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
* Description:  Declaration and implementation of CCF trace macros
*
*/

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

#ifndef CCFTRACE_H
#define CCFTRACE_H

#ifdef _DEBUG
#include <e32svr.h>

#ifdef __WINS__
void TraceAssertFunc( const TDesC8& aFile, TInt aLine );
    
#define DECLARE_TRACE_ASSERT \
void TraceAssertFunc( const TDesC8& aFile, TInt aLine ) \
    { \
    TBuf<256> file; \
    file.Copy( aFile ); \
    TBuf<32> date; \
    TBuf<32> time; \
    date.Copy( _L8( __DATE__ ) ); \
    time.Copy( _L8( __TIME__ ) ); \
    RDebug::Print( _L( "Assertion failed: file=%s, line=%d, compiled=%s %s" ), file.PtrZ(), aLine, date.PtrZ(), time.PtrZ() ); \
    }
#else
#define DECLARE_TRACE_ASSERT
#endif

#ifdef __WINS__
#define TRACE_ASSERT( a ) if ( !(a) ) TraceAssertFunc( _L8( __FILE__ ), __LINE__ )
#define TRACE_ASSERT_FALSE() TraceAssertFunc( _L8( __FILE__ ), __LINE__ )
#else
#define TRACE_ASSERT( a ) if ( !(a) ) RDebug::Print( _L( "Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__ " " __TIME__ ), __LINE__ )
#define TRACE_ASSERT_FALSE() RDebug::Print( _L( "Assertion failed: file=" __FILE__ ", line=%d, compiled=" __DATE__ " " __TIME__ ), __LINE__ )
#endif

#define TEST_CASE_TRACE( a ) \
    RDebug::Print a; \
    Logger().Write a; \
    Logger().EndLine()

#ifdef COMPONENT_TRACE_FLAG
#define COMPONENT_TRACE( a ) RDebug::Print a 
#else
#define COMPONENT_TRACE( a )
#endif

#else
#define TRACE_ASSERT( a )
#define TRACE_ASSERT_FALSE()
#define COMPONENT_TRACE( a )
#define DECLARE_TRACE_ASSERT
#define TEST_CASE_TRACE( a )
#endif
#endif