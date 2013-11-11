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
* Description:  Macro definitions for tracing and debugging purposes.
*
*/


#ifndef ACC_DEBUG_H
#define ACC_DEBUG_H

#ifdef _DEBUG

// INCLUDES
#include <e32svr.h>

// CONSTANTS
_LIT( KComponent, "[AccFW:Component]" );
_LIT( KThisFile,  "[AccFW:Component] - Trace this file: %s, line: %d, compiled: %s %s" );
_LIT( KAssertion, "[AccFW:Component] - Assertion failed: File: %s, line: %d, compiled: %s %s" );
_LIT( KPanic,     "[AccFW:Component] - Panic occurred: File: %s, line: %d, compiled: %s %s" );
_LIT( KMystic,    "[AccFW:Component] - Mystic failure: File: %s, line: %d, compiled: %s %s" );
_LIT8( KDATE, __DATE__ );
_LIT8( KTIME, __TIME__ );

const TInt KTraceMaxSize = 256;

// DATA TYPES
enum TTraceType
    {
    ETraceInit,
    ETraceAssert,
    ETracePanic
    };

// INLINE FUNCTIONS

    // -----------------------------------------------------------------------------
    // ThisFileFunc
    // -----------------------------------------------------------------------------
    inline void ThisFileFunc( const TDesC8& aFile,
                              TInt aLine,
                              TTraceType aType = ETraceInit )
        {
        HBufC* fileBuf = HBufC::New( aFile.Length() + 1 );
        HBufC* dateBuf = HBufC::New( 32 );
        HBufC* timeBuf = HBufC::New( 32 );

        if ( fileBuf != NULL && dateBuf != NULL && timeBuf != NULL )
            {
            fileBuf->Des().Copy( aFile );
            timeBuf->Des().Copy( KTIME );
            dateBuf->Des().Copy( KDATE );

            if ( aType == ETraceInit )
                {
                RDebug::Print( KThisFile,
                               fileBuf->Des().PtrZ(),
                               aLine,
                               dateBuf->Des().PtrZ(),
                               timeBuf->Des().PtrZ() );
                }

            else if ( aType == ETraceAssert )
                {
                RDebug::Print( KAssertion,
                               fileBuf->Des().PtrZ(),
                               aLine,
                               dateBuf->Des().PtrZ(),
                               timeBuf->Des().PtrZ() );
                }

            else if ( aType == ETracePanic )
                {
                RDebug::Print( KPanic,
                               fileBuf->Des().PtrZ(),
                               aLine,
                               dateBuf->Des().PtrZ(),
                               timeBuf->Des().PtrZ() );
                }

            else
                {
                RDebug::Print( KMystic,
                               fileBuf->Des().PtrZ(),
                               aLine,
                               dateBuf->Des().PtrZ(),
                               timeBuf->Des().PtrZ() );
                }
            }

        else
            {
            RDebug::Print( _L( "Assertion and memory allocation failed!" ) );
            }

        delete fileBuf;
        delete dateBuf;
        delete timeBuf;
        }

    // -----------------------------------------------------------------------------
    // TraceAssertFunc
    // -----------------------------------------------------------------------------
    inline void TraceAssertFunc( const TDesC8& aFile, TInt aLine )
        {
        ThisFileFunc( aFile, aLine, ETraceAssert );
        }

    // -----------------------------------------------------------------------------
    // TracePanicFunc
    // -----------------------------------------------------------------------------
    inline void TracePanicFunc( const TDesC8& aFile, TInt aLine )
        {
        ThisFileFunc( aFile, aLine, ETracePanic );
        User::Panic( KComponent, KErrGeneral );
        }

// MACROS
    #define PANIC_IF_FALSE( a ) if ( !( a ) )\
            TracePanicFunc( TPtrC8( ( TText8* ) __FILE__), __LINE__ )

    #define PANIC_IF_TRUE( a ) if ( ( a ) )\
            TracePanicFunc( TPtrC8( ( TText8* ) __FILE__), __LINE__ )

    #define PANIC_ALWAYS\
            TracePanicFunc( TPtrC8( ( TText8* ) __FILE__), __LINE__ )

    // -----------------------------------------------------------------------------
    // COMPONENT_TRACE_FLAG
    // -----------------------------------------------------------------------------
    #ifdef COMPONENT_TRACE_FLAG

        #define COMPONENT_TRACE_THIS_FILE\
            ThisFileFunc( TPtrC8( ( TText8* ) __FILE__), __LINE__ )

        #define COM_TRACE_( AAA ) do\
            { _LIT( logStr, AAA ); RDebug::Print( logStr ); } while ( 0 )

        #define COM_TRACE_1( AAA, BBB ) do\
             { _LIT( logStr, AAA ); RDebug::Print( logStr, BBB ); } while ( 0 )

        #define COM_TRACE_2( AAA, BBB, CCC ) do\
             { _LIT( logStr, AAA ); RDebug::Print( logStr, BBB, CCC ); } while ( 0 )

        #define COM_TRACE_3( AAA, BBB, CCC, DDD ) do\
             { _LIT( logStr, AAA ); RDebug::Print( logStr, BBB, CCC, DDD ); } while ( 0 )

        #define COM_TRACE_4( AAA, BBB, CCC, DDD, EEE ) do\
             { _LIT( logStr, AAA ); RDebug::Print( logStr, BBB, CCC, DDD, EEE ); } while ( 0 )

        #define COM_TRACE_RAW_( AAA ) do\
            { RDebug::RawPrint( AAA ); } while ( 0 )
            
        #define COM_TRACE_RAW_1( AAA, BBB ) do\
             { _LIT( logStr, AAA ); TBuf<KTraceMaxSize> buffer; buffer.Append( logStr ); buffer.Append( BBB );\
             RDebug::RawPrint( buffer ); } while ( 0 )
        
    #else

        #define COMPONENT_TRACE_THIS_FILE

        #define COM_TRACE_( AAA )
        #define COM_TRACE_1( AAA, BBB )
        #define COM_TRACE_2( AAA, BBB, CCC )
        #define COM_TRACE_3( AAA, BBB, CCC, DDD )
        #define COM_TRACE_4( AAA, BBB, CCC, DDD, EEE )
        #define COM_TRACE_RAW_( AAA )
        #define COM_TRACE_RAW_1( AAA, BBB )

    #endif // COMPONENT_TRACE_FLAG

    #define TRACE_ASSERT( a ) if ( !( a ) )\
            TraceAssertFunc( TPtrC8( ( TText8* ) __FILE__), __LINE__ )

    #define TRACE_ASSERT_RETURN( a ) if ( !( ( a ) == KErrNone ) )\
            TraceAssertFunc( TPtrC8( ( TText8* ) __FILE__), __LINE__ )

    #define TRACE_ASSERT_ALWAYS\
            TraceAssertFunc( TPtrC8( ( TText8* ) __FILE__ ), __LINE__ )

    // -----------------------------------------------------------------------------
    // API_TRACE_FLAG
    // -----------------------------------------------------------------------------
    #ifdef API_TRACE_FLAG

        #define API_TRACE_( AAA ) do\
            { _LIT( logStr, AAA ); RDebug::Print( logStr ); } while ( 0 )

        #define API_TRACE_1( AAA, BBB ) do\
             { _LIT( logStr, AAA ); RDebug::Print( logStr, BBB ); } while ( 0 )

        #define API_TRACE_2( AAA, BBB, CCC ) do\
             { _LIT( logStr, AAA ); RDebug::Print( logStr, BBB, CCC ); } while ( 0 )

        #define API_TRACE_3( AAA, BBB, CCC, DDD ) do\
             { _LIT( logStr, AAA ); RDebug::Print( logStr, BBB, CCC, DDD ); } while ( 0 )

        #define API_TRACE_4( AAA, BBB, CCC, DDD, EEE ) do\
             { _LIT( logStr, AAA ); RDebug::Print( logStr, BBB, CCC, DDD, EEE ); } while ( 0 )
             
        #define API_TRACE_RAW_1( AAA, BBB ) do\
             { _LIT( logStr, AAA ); TBuf<KTraceMaxSize> buffer; buffer.Append( logStr ); buffer.Append( BBB );\
             RDebug::RawPrint( buffer ); } while ( 0 )

    #else

        #define API_TRACE_( AAA )
        #define API_TRACE_1( AAA, BBB )
        #define API_TRACE_2( AAA, BBB, CCC )
        #define API_TRACE_3( AAA, BBB, CCC, DDD )
        #define API_TRACE_4( AAA, BBB, CCC, DDD, EEE )
        #define API_TRACE_RAW_1( AAA, BBB )

    #endif // API_TRACE_FLAG

#else // _DEBUG

    #define TRACE_ASSERT( a )
    #define TRACE_ASSERT_RETURN( a ) a
    #define TRACE_ASSERT_ALWAYS

    #define COM_TRACE_( AAA )
    #define COM_TRACE_1( AAA, BBB )
    #define COM_TRACE_2( AAA, BBB, CCC )
    #define COM_TRACE_3( AAA, BBB, CCC, DDD )
    #define COM_TRACE_4( AAA, BBB, CCC, DDD, EEE )
    #define COM_TRACE_RAW_( AAA )
    #define COM_TRACE_RAW_1( AAA, BBB )

    #define API_TRACE_( AAA )
    #define API_TRACE_1( AAA, BBB )
    #define API_TRACE_2( AAA, BBB, CCC )
    #define API_TRACE_3( AAA, BBB, CCC, DDD )
    #define API_TRACE_4( AAA, BBB, CCC, DDD, EEE )
    #define API_TRACE_RAW_1( AAA, BBB )

    #define COMPONENT_TRACE_THIS_FILE

    #define PANIC_IF_FALSE( a )
    #define PANIC_IF_TRUE( a )
    #define PANIC_ALWAYS

#endif // _DEBUG

#endif // ACC_DEBUG_H

// End of File
