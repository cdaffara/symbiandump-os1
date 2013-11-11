/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Tracing macros.
*
*/


#ifndef TRACE_H
#define TRACE_H

#include "traceconfiguration.hrh"
#include "tracedefs.h"

#ifdef TRACE_INTO_FILE
#include <flogger.h> // RFileLogger
#else
#include <e32debug.h> // RDebug
#endif

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
//

/**
* Prefix error trace
*/
#define _PREFIX_ERROR( aMsg ) _PREFIX_TRACE( "[ERROR: %d]: " L##aMsg )

/**
* Prefix info trace.
*/
#define _PREFIX_INFO( aMsg ) _PREFIX_TRACE( "[INFO]: " L##aMsg )

/**
* Define needed directories if TRACE_INTO_FILE macro in use
*/
#ifdef TRACE_INTO_FILE

    _LIT( KDir, "sensordatacompensator" );
    _LIT( KFile, "sensordatacompensator_log.txt" );
    _LIT( KFullPath, "c:\\logs\\sensordatacompensator\\" );

#endif

//-----------------------------------------------------------------------------
// Error trace macros
//-----------------------------------------------------------------------------
//
#ifdef ERROR_TRACE

    /**
    * Error trace definitions.
    */
    #ifdef TRACE_INTO_FILE

        #define ERROR( aErr, aMsg )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, \
                    _PREFIX_ERROR( aMsg ), aErr );\
                }\
            }
        #define ERROR_1( aErr, aMsg, aP1 )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, \
                    _PREFIX_ERROR( aMsg ), aErr, aP1 );\
                }\
            }
        #define ERROR_2( aErr, aMsg, aP1, aP2 )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, \
                    _PREFIX_ERROR( aMsg ), aErr, aP1, aP2 );\
                }\
            }
        #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, \
                    _PREFIX_ERROR( aMsg ), aErr, aP1, aP2, aP3 );\
                }\
            }

    #else//TRACE_INTO_FILE not defined

        #define ERROR( aErr, aMsg )\
            {\
            if( aErr < KErrNone )\
                {\
                RDebug::Print( _PREFIX_ERROR( aMsg ), aErr );\
                }\
            }
        #define ERROR_1( aErr, aMsg, aP1 )\
            {\
            if( aErr < KErrNone )\
                {\
                RDebug::Print( _PREFIX_ERROR( aMsg ), aErr, aP1 );\
                }\
            }
        #define ERROR_2( aErr, aMsg, aP1, aP2 )\
            {\
            if( aErr < KErrNone )\
                {\
                RDebug::Print( _PREFIX_ERROR( aMsg ), aErr, aP1, aP2 );\
                }\
            }
        #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )\
            {\
            if( aErr < KErrNone )\
                {\
                RDebug::Print( _PREFIX_ERROR( aMsg ), aErr, aP1, aP2, aP3 );\
                }\
            }

    #endif//TRACE_INTO_FILE

    #define ERROR_GEN( aMsg ) ERROR( KErrGeneral, aMsg )
    #define ERROR_GEN_1( aMsg, aP1 ) ERROR_1( KErrGeneral, aMsg, aP1 )
    #define ERROR_GEN_2( aMsg, aP1, aP2 ) ERROR_2( KErrGeneral, aMsg, aP1, aP2 )
    #define ERROR_GEN_3( aMsg, aP1, aP2, aP3 ) ERROR_3( KErrGeneral, aMsg, aP1, aP3 )

    #define ERROR_PARAM(_p) _p

    #define TRAPD_ERR( aErr, aStmt ) TRAPD( aErr, aStmt )
    #define TRAP_ERR( aErr, aStmt ) TRAP( aErr, aStmt )

    #define TRAP_AND_LEAVE(_s,_t) \
        { TRAPD(_e,_s); ERROR(_e,_t); User::LeaveIfError(_e); }

#else//ERROR_TRACE not defined

    #define ERROR( aErr, aMsg )
    #define ERROR_1( aErr, aMsg, aP1 )
    #define ERROR_2( aErr, aMsg, aP1, aP2 )
    #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )
    #define ERROR_GEN( aMsg )
    #define ERROR_GEN_1( aMsg, aP1 )
    #define ERROR_GEN_2( aMsg, aP1, aP2 )
    #define ERROR_GEN_3( aMsg, aP1, aP2, aP3 )

    #define ERROR_PARAM(_p)

    #define TRAPD_ERR( aErr, aStmt ) TRAP_IGNORE( aStmt )
    #define TRAP_ERR( aErr, aStmt )  TRAP_IGNORE( aStmt )

    #define TRAP_AND_LEAVE(_s,_t) { _s; }

#endif//ERROR_TRACE

//-----------------------------------------------------------------------------
// Info trace macros
//-----------------------------------------------------------------------------
//
#ifdef INFO_TRACE

    /**
    * Info log message definitions.
    */
    #ifdef TRACE_INTO_FILE

        #define INFO( aMsg )\
            {\
            RFileLogger::Write( KDir, KFile, EFileLoggingModeAppend, \
                _PREFIX_INFO( aMsg ) );\
            }
        #define INFO_1( aMsg, aP1 )\
            {\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, \
                _PREFIX_INFO( aMsg ), aP1 );\
            }
        #define INFO_2( aMsg, aP1, aP2 )\
            {\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, \
                _PREFIX_INFO( aMsg ), aP1, aP2 );\
            }
        #define INFO_3( aMsg, aP1, aP2, aP3 )\
            {\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, \
                _PREFIX_INFO( aMsg ), aP1, aP2, aP3 );\
            }
        #define INFO_4( aMsg, aP1, aP2, aP3, aP4 )\
            {\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, \
                _PREFIX_INFO( aMsg ), aP1, aP2, aP3, aP4 );\
            }
        #define INFO_5( aMsg, aP1, aP2, aP3, aP4, aP5 )\
            {\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, \
                _PREFIX_INFO( aMsg ), aP1, aP2, aP3, aP4, aP5 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define INFO( aMsg )\
            {\
            RDebug::Print( _PREFIX_INFO( aMsg ) );\
            }
        #define INFO_1( aMsg, aP1 )\
            {\
            RDebug::Print( _PREFIX_INFO( aMsg ), aP1 );\
            }
        #define INFO_2( aMsg, aP1, aP2 )\
            {\
            RDebug::Print( _PREFIX_INFO( aMsg ), aP1, aP2 );\
            }
        #define INFO_3( aMsg, aP1, aP2, aP3 )\
            {\
            RDebug::Print( _PREFIX_INFO( aMsg ), aP1, aP2, aP3 );\
            }
        #define INFO_4( aMsg, aP1, aP2, aP3, aP4 )\
            {\
            RDebug::Print( _PREFIX_INFO( aMsg ), aP1, aP2, aP3, aP4 );\
            }
        #define INFO_5( aMsg, aP1, aP2, aP3, aP4, aP5 )\
            {\
            RDebug::Print( _PREFIX_INFO( aMsg ), aP1, aP2, aP3, aP4, aP5 );\
            }

    #endif//TRACE_INTO_FILE

    #define INFO_PARAM( aParam ) aParam

#else//INFO_TRACE not defined

    #define INFO( aMsg )
    #define INFO_1( aMsg, aP1 )
    #define INFO_2( aMsg, aP1, aP2 )
    #define INFO_3( aMsg, aP1, aP2, aP3 )
    #define INFO_4( aMsg, aP1, aP2, aP3, aP4 )
    #define INFO_5( aMsg, aP1, aP2, aP3, aP4, aP5 )
    #define INFO_PARAM( aParam )

#endif//INFO_TRACE

//-----------------------------------------------------------------------------
// Trace current client thread name and process id
//-----------------------------------------------------------------------------
//
#ifdef CLIENT_TRACE

    #define CLIENT( aMessage )\
        {\
        RThread thread;\
        TInt err = aMessage.Client( thread );\
        if( err == KErrNone )\
            {\
            RProcess process;\
            err = thread.Process( process );\
            if( err == KErrNone )\
                {\
                TPtrC thredName( thread.Name() );\
                TUid processUid( process.SecureId() );\
                INFO_2( "Current client process UID: [%x], thread name: [%S]",\
                    processUid,\
                    &thredName );\
                }\
            process.Close();\
            }\
        thread.Close();\
        }

#else

    #define CLIENT( aMessage )

#endif

//-----------------------------------------------------------------------------
// Function trace macros
//-----------------------------------------------------------------------------
//
#ifdef FUNC_TRACE

    #include <e32base.h> // TCleanupItem

    /**
    * Function logging definitions.
    */
    #ifdef TRACE_INTO_FILE

        #define FUNC( aMsg, aP1 )\
            {\
            TPtrC8 trace( _S8( aMsg ) );\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend, trace, aP1 );\
            }\

    #else//TRACE_INTO_FILE not defined

        #define FUNC( aMsg, aP1 )\
            {\
            RDebug::Printf( aMsg, aP1 );\
            }\

    #endif//TRACE_INTO_FILE

    /**
    * Function trace helper class.
    *
    * NOTE:
    * LC -methods cannot be trapped. Therefore if LC -method leaves
    * END trace is used instead of LEAVE trace.
    * If you have an idea how to round this problem please tell.
    */
    _LIT8( KFuncNameTerminator, "(" );
    _LIT8( KFuncLeavePatternL, "L" );
    class TFuncLog
        {
        public:
            static void Cleanup( TAny* aPtr )
                {
                TFuncLog* self = static_cast< TFuncLog* >( aPtr );
                self->iLeft = ETrue;
                FUNC( _PREFIX_CHAR("%S-LEAVE"), &self->iFunc ); // Leave detected
                }
            inline TFuncLog( const char* aFunc ) :
                    iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
                    iLeft( EFalse ),
                    iCleanupItem( Cleanup, this ),
                    iCanLeave( EFalse )
                {
                TInt pos( iFunc.Find( KFuncNameTerminator ) );
                if( pos != KErrNotFound )
                    {
                    iFunc.Set( iFunc.Left( pos ) );
                    iCanLeave = !iFunc.Right(
                        KFuncLeavePatternL().Length() ).Compare( KFuncLeavePatternL );
                    if ( iCanLeave )
                        {
                        CleanupStack::PushL( iCleanupItem ); // Ignore warnings
                        }
                    }
                FUNC( _PREFIX_CHAR("%S-START"), &iFunc );
                }

            inline ~TFuncLog()
                {
                if ( !iLeft )
                    {
                    if ( iCanLeave )
                        {
                        CleanupStack::Pop( this ); // Pop the cleanup item
                        }
                    FUNC( _PREFIX_CHAR("%S-END"), &iFunc ); // Normally finished
                    }
                }

        private: // Data
            TPtrC8 iFunc;
            TBool iLeft;
            TCleanupItem iCleanupItem;
            TBool iCanLeave;
        };
    #define FUNC_LOG TFuncLog _fl( __PRETTY_FUNCTION__ );

#else//FUNC_TRACE not defined

    #define FUNC_LOG

#endif//FUNC_TRACE

//-----------------------------------------------------------------------------
// Timestamp trace macros
//-----------------------------------------------------------------------------
//
#ifdef TIMESTAMP_TRACE

    #define PREFIX_TIMESTAMP( aCaption ) _PREFIX_TRACE_2("[TIMESTAMP] (%d:%02d:%02d.%06d us) ",aCaption)
    #define CURRENT_TIME( aDt ) TDateTime aDt; { TTime t; t.HomeTime(); aDt = t.DateTime(); }
    #define EXTRACT_TIME( aDt ) aDt.Hour(), aDt.Minute(), aDt.Second(), aDt.MicroSecond()

    #ifdef TRACE_INTO_FILE

        #define TIMESTAMP( aCaption )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ) );\
            }

        #define TIMESTAMP_1( aCaption, aP1 )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1 );\
            }

        #define TIMESTAMP_2( aCaption, aP1, aP2 )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1,aP2 );\
            }

        #define TIMESTAMP_3( aCaption, aP1, aP2, aP3 )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( KDir, KFile, EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1,aP2,aP3 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define TIMESTAMP( aCaption )\
            {\
            CURRENT_TIME( dt );\
            RDebug::Print( PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ) );\
            }

        #define TIMESTAMP_1( aCaption, aP1 )\
            {\
            CURRENT_TIME( dt );\
            RDebug::Print( PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1 );\
            }

        #define TIMESTAMP_2( aCaption, aP1, aP2 )\
            {\
            CURRENT_TIME( dt );\
            RDebug::Print( PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1,aP2 );\
            }

        #define TIMESTAMP_3( aCaption, aP1, aP2, aP3 )\
            {\
            CURRENT_TIME( dt );\
            RDebug::Print( PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1,aP2,aP3 );\
            }

    #endif//TRACE_INTO_FILE

#else//TIMESTAMP_TRACE not defined

    #define TIMESTAMP( aCaption )
    #define TIMESTAMP_1( aCaption, aP1 )
    #define TIMESTAMP_2( aCaption, aP1, aP2 )
    #define TIMESTAMP_3( aCaption, aP1, aP2, aP3 )

#endif//TIMESTAMP_TRACE

#ifdef _DEBUG

    #include <e32debug.h> // RDebug

    static void Panic( const TDesC8& aFileName, const TInt aLineNum )
        {
        TPath name;
        name.Copy( aFileName );
        RDebug::Print( _L( "Assertion failed in file=%S, line=%d" ), &name, aLineNum );
        User::Invariant();
        }

    #define ASSERT_ALWAYS_TRACE Panic( _L8(__FILE__), __LINE__ );
    #define ASSERT_TRACE( _s ) if ( !( _s ) ) { ASSERT_ALWAYS_TRACE; }

#else // _DEBUG

    #define ASSERT_ALWAYS_TRACE
    #define ASSERT_TRACE( _s )

#endif // _DEBUG

#define TRACE_MATRIX( aMsg, aMatrix ) \
    INFO( aMsg ); \
    INFO( "------------------------------------------------" ); \
    INFO_3( "       [%f %f %f]", (TReal)(aMatrix)[0], (TReal)(aMatrix)[1], (TReal)(aMatrix)[2] ); \
    INFO_3( "Matrix [%f %f %f]", (TReal)(aMatrix)[3], (TReal)(aMatrix)[4], (TReal)(aMatrix)[5] ); \
    INFO_3( "       [%f %f %f]", (TReal)(aMatrix)[6], (TReal)(aMatrix)[7], (TReal)(aMatrix)[8] ); \
    INFO( "------------------------------------------------" );

#endif // TRACE_H
