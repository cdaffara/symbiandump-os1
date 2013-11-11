/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Tracing macros for SysLangUtil module.
*
*/


#ifndef __SYSLANGUTILTRACE_H_
#define __SYSLANGUTILTRACE_H_

#ifdef _DEBUG
    #include <e32cmn.h>

    void AssertTraceFunc( const TDesC8& aFile, TInt aLine );
    void TraceFunc( TRefByValue<const TDesC16> aFmt, ... );

    #define ASSERT_ALWAYS_TRACE     AssertTraceFunc( _L8( __FILE__ ), __LINE__ )
    #define ASSERT_TRACE( a )       if( !( a ) ) { ASSERT_ALWAYS_TRACE; }
    #define ERROR_TRACE_1( a, b )   TraceFunc( _L( a ), b )
    #define ERROR_TRACE_2( a, b, c )   TraceFunc( _L( a ), b,c )
#else
    #define ASSERT_ALWAYS_TRACE
    #define ASSERT_TRACE( a )
    #define ERROR_TRACE_1( a, b )
	#define ERROR_TRACE_2( a, b, c )
#endif

#if defined _DEBUG && defined __SYSLANG_TRACE__
    void FuncEntryTrace( const TDesC8& aFunction, TBool aPrintCaller );
    void FuncExitTrace( const TDesC8& aFunction );
    void FuncExitTrace( const TDesC8& aFunction, const TInt aReturnValue );

    #define FUNC_NAME _L8( __PRETTY_FUNCTION__ )

    #define FUNC_ENTRY_TRACE            FuncEntryTrace( FUNC_NAME, EFalse )
    #define API_FUNC_ENTRY_TRACE        FuncEntryTrace( FUNC_NAME, ETrue )
    #define FUNC_EXIT_TRACE             FuncExitTrace( FUNC_NAME )
    #define FUNC_EXIT_RET_TRACE( a )    FuncExitTrace( FUNC_NAME, a )
#else
    #define FUNC_ENTRY_TRACE
    #define API_FUNC_ENTRY_TRACE
    #define FUNC_EXIT_TRACE
    #define FUNC_EXIT_RET_TRACE( a )
#endif

#endif // __SYSLANGUTILTRACE_H_
