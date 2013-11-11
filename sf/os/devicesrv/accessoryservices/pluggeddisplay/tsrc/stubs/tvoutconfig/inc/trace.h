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
 * Description: Tracing macros, 2nd generation
 *
 */

#ifndef TRACE_H
#define TRACE_H

#include "traceconfiguration.hrh"
#include "tracedefs.h"
#include <e32debug.h> // RDebug
#if defined TRACE_INTO_FILE || defined LOG_INTO_FILE
    #include <flogger.h> // RFileLogger
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
* Helper for using _LIT with macros.
*/
#define _CREATE_LIT(a,b) _LIT(a,b)

/**
* Define needed if TRACE_INTO_FILE macro in use
*/
#ifdef TRACE_INTO_FILE
    _CREATE_LIT( KTraceFile, TRACE_FILE );
#endif

/**
* Define needed directories if LOG_INTO_FILE macro in use
*/
#ifdef LOG_INTO_FILE
    _CREATE_LIT( KLogFile, LOG_FILE );
#endif

/**
* Helpers for tracing into file.
*/
#if defined TRACE_INTO_FILE || defined LOG_INTO_FILE

const TText KBackSlash = '\\';

static TPtrC DriveAndPath( const TDesC& aFullPath )
    {
    TInt pos( aFullPath.LocateReverse( KBackSlash ) );
    return ( pos >= 0 && pos < aFullPath.Length() ? aFullPath.Left( pos ) : KNullDesC() );
    }

static TPtrC NameAndExt( const TDesC& aFullPath )
    {
    TInt pos( aFullPath.LocateReverse( KBackSlash ) );
    return ( pos >= 0 && pos < aFullPath.Length() ? aFullPath.Mid( pos + 1 ) : KNullDesC() );
    }

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
                RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                    EFileLoggingModeAppend,\
                    _PREFIX_ERROR( aMsg ), aErr );\
                }\
            }
        #define ERROR_1( aErr, aMsg, aP1 )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                    EFileLoggingModeAppend,\
                    _PREFIX_ERROR( aMsg ), aErr, aP1 );\
                }\
            }
        #define ERROR_2( aErr, aMsg, aP1, aP2 )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                    EFileLoggingModeAppend,\
                    _PREFIX_ERROR( aMsg ), aErr, aP1, aP2 );\
                }\
            }
        #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )\
            {\
            if( aErr < KErrNone )\
                {\
                RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                    EFileLoggingModeAppend,\
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
            RFileLogger::Write( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend,\
                _PREFIX_INFO( aMsg ) );\
            }
        #define INFO_1( aMsg, aP1 )\
            {\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend,\
                _PREFIX_INFO( aMsg ), aP1 );\
            }
        #define INFO_2( aMsg, aP1, aP2 )\
            {\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend,\
                _PREFIX_INFO( aMsg ), aP1, aP2 );\
            }
        #define INFO_3( aMsg, aP1, aP2, aP3 )\
            {\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend,\
                _PREFIX_INFO( aMsg ), aP1, aP2, aP3 );\
            }
        #define INFO_4( aMsg, aP1, aP2, aP3, aP4 )\
            {\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend,\
                _PREFIX_INFO( aMsg ), aP1, aP2, aP3, aP4 );\
            }
        #define INFO_5( aMsg, aP1, aP2, aP3, aP4, aP5 )\
            {\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend,\
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

    #define _PREFIX_CLIENT( aMsg ) _PREFIX_TRACE( "[CLIENT: sid [0x%x] thread [%S]] " L##aMsg )

    #ifdef TRACE_INTO_FILE

        #define CLIENT_TRACE_2( aMsg, aP1, aP2 )\
            {\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend, _PREFIX_CLIENT(aMsg), aP1, aP2 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define CLIENT_TRACE_2( aMsg, aP1, aP2 )\
            {\
            RDebug::Print( _PREFIX_CLIENT(aMsg), aP1, aP2 );\
            }

    #endif//TRACE_INTO_FILE

    /**
    * Traces current secure id and thread
    */
    #define CURRENT_CLIENT( aMsg )\
        {\
        TName name( RThread().Name() );\
        TSecureId sid( RThread().SecureId() );\
        CLIENT_TRACE_2( aMsg, sid.iId, &name );\
        }

    /**
    * Traces secure id and thread from RMessage2
    */
    #define CLIENT( aRMessage2 )\
        {\
        RThread thread;\
        TInt err = aRMessage2.Client( thread );\
        if( err == KErrNone )\
            {\
            TName name( thread.Name() );\
            TSecureId sid( thread.SecureId() );\
            CLIENT_TRACE_2( "", sid.iId, &name );\
            }\
        thread.Close();\
        }

    /**
    * Traces secure id and thread from RMessage2
    */
    #define CLIENT_1( aRMessage2, aMsg )\
        {\
        RThread thread;\
        TInt err = aRMessage2.Client( thread );\
        if( err == KErrNone )\
            {\
            TName name( thread.Name() );\
            TSecureId sid( thread.SecureId() );\
            CLIENT_TRACE_2( aMsg, sid.iId, &name );\
            }\
        thread.Close();\
        }

#else

    #define CURRENT_CLIENT( aMsg )
    #define CLIENT( aRMessage2 )
    #define CLIENT_1( aRMessage2, aMsg )

#endif

//-----------------------------------------------------------------------------
// Trace current heap usage
//-----------------------------------------------------------------------------
//
#ifdef HEAP_TRACE

    #define _PREFIX_HEAP( aMsg ) _PREFIX_TRACE( "[HEAP: %d bytes] " L##aMsg )

    #ifdef TRACE_INTO_FILE

        #define HEAP( aMsg )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace );\
            }
        #define HEAP_1( aMsg, aP1 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1 );\
            }
        #define HEAP_2( aMsg, aP1, aP2 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2 );\
            }
        #define HEAP_3( aMsg, aP1, aP2, aP3 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2, aP3 );\
            }
        #define HEAP_4( aMsg, aP1, aP2, aP3, aP4 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend, _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2, aP3, aP4 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define HEAP( aMsg )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace );\
            }
        #define HEAP_1( aMsg, aP1 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1 );\
            }
        #define HEAP_2( aMsg, aP1, aP2 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2 );\
            }
        #define HEAP_3( aMsg, aP1, aP2, aP3 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2, aP3 );\
            }
        #define HEAP_4( aMsg, aP1, aP2, aP3, aP4 )\
            {\
            TInt totalAllocSpace = 0;\
            User::AllocSize( totalAllocSpace );\
            RDebug::Print( _PREFIX_HEAP( aMsg ), totalAllocSpace, aP1, aP2, aP3, aP4 );\
            }

    #endif//TRACE_INTO_FILE

#else//HEAP_TRACE not defined

    #define HEAP( aMsg )
    #define HEAP_1( aMsg, aP1 )
    #define HEAP_2( aMsg, aP1, aP2 )
    #define HEAP_3( aMsg, aP1, aP2, aP3 )
    #define HEAP_4( aMsg, aP1, aP2, aP3, aP4 )

#endif//HEAP_TRACE


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
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend, trace, aP1 );\
            }

        #define FUNC_2( aMsg, aP1, aP2, aP3 )\
            {\
            TPtrC8 trace( _S8( aMsg ) );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend, trace, aP1, aP2, aP3 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define FUNC( aMsg, aP1 )\
            {\
            RDebug::Printf( aMsg, aP1 );\
            }

        #define FUNC_2( aMsg, aP1, aP2, aP3 )\
            {\
            RDebug::Printf( aMsg, aP1, aP2, aP3 );\
            }

    #endif//TRACE_INTO_FILE

    /**
    * Function trace helper class.
    */
    const TText KFuncNameTerminator = '(';
    const TText KFuncLeaveIndicator = 'L';
    const TInt KFuncCanLeave = 0x1;
    const TInt KFuncLeft = 0x2;
    const TInt KFuncLogClient = 0x4;
    
    class TFuncLog
        {
    public:
        static void Cleanup( TAny* aPtr )
            {
            TFuncLog* self = static_cast< TFuncLog* >( aPtr );
            self->iFlags |= KFuncLeft;
            self->LogLeave();
            }
    
        inline void LogStart()
            {
            if ( iFlags & KFuncLogClient )
                {
                TBuf8<KMaxName> name8;
                name8.Copy( RThread().Name() );
                TSecureId sid( RThread().SecureId() );
                FUNC_2( _PREFIX_TRACE8("%S-START(sid [0x%x] thread [%S])"),
                    &iFunc, sid.iId, &name8 );
                }
            else
                {
                FUNC( _PREFIX_TRACE8("%S-START"), &iFunc );
                }
            }

        inline void LogLeave()
            {
            if ( iFlags & KFuncLogClient )
                {
                TBuf8<KMaxName> name8;
                name8.Copy( RThread().Name() );
                TSecureId sid( RThread().SecureId() );
                FUNC_2( _PREFIX_TRACE8("%S-LEAVE(sid [0x%x] thread [%S])"),
                    &iFunc, sid.iId, &name8 );
                }
            else
                {
                FUNC( _PREFIX_TRACE8("%S-LEAVE"), &iFunc );
                }
            }
    
        inline void LogEnd()
            {
            if ( iFlags & KFuncLogClient )
                {
                TBuf8<KMaxName> name8;
                name8.Copy( RThread().Name() );
                TSecureId sid( RThread().SecureId() );
                FUNC_2( _PREFIX_TRACE8("%S-END(sid [0x%x] thread [%S])"),
                    &iFunc, sid.iId, &name8 );
                }
            else
                {
                FUNC( _PREFIX_TRACE8("%S-END"), &iFunc );
                }
            }
    
        // For non leaving and L methods
        inline TFuncLog( const char* aFunc, TUint aFlags ) :
                iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
                iFlags( aFlags ),
                iCleanupItem( Cleanup, this ),
                iLCPtr( NULL )
            {
            TInt pos( iFunc.Locate( KFuncNameTerminator ) );
            if( pos >= 0 && pos < iFunc.Length() )
                {
                iFunc.Set( iFunc.Left( pos ) );
                TInt len( iFunc.Length() );
                if( len > 0 && iFunc[ len - 1 ] == KFuncLeaveIndicator )
                    {
                    iFlags |= KFuncCanLeave;
                    CleanupStack::PushL( iCleanupItem ); // Ignore warnings
                    }
                }
            LogStart();
            }
    
        // For LC methods only
        inline TFuncLog( const char* aFunc, TUint aFlags, TAny** aLCPtr ) :
                iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
                iFlags( aFlags ),
                iCleanupItem( Cleanup, this ),
                iLCPtr( NULL )
            {
            TInt pos( iFunc.Locate( KFuncNameTerminator ) );
            if( pos >= 0 && pos < iFunc.Length() )
                {
                iFunc.Set( iFunc.Left( pos ) );
                iFlags |= KFuncCanLeave;
                iLCPtr = aLCPtr;
                CleanupStack::PushL( iCleanupItem ); // Ignore warnings
                }
            LogStart();
            }
    
        inline ~TFuncLog()
            {
            if ( !( iFlags & KFuncLeft ) )
                {
                if ( iFlags & KFuncCanLeave )
                    {
                    if ( iLCPtr && *iLCPtr)
                        {
                        CleanupStack::Pop(); // Pop LC ptr first
                        CleanupStack::Pop( this ); // Pop the cleanup item
                        CleanupStack::PushL( *iLCPtr ); // Push LC ptr back
                        }
                    else
                        {
                        CleanupStack::Pop( this ); // Pop the cleanup item
                        }
                    }
                LogEnd(); // Normally finished
                }
            }
    
    private: // Data
        TPtrC8 iFunc;
        TUint iFlags;
        TCleanupItem iCleanupItem;
        TAny** iLCPtr;
        };

    /*
    * Use with non leaving and L methods
    *
    * CMyClass* CMyClass::DoL()
    *   {
    *   FUNC_LOG;
    *   CMyClass* ret = new (ELeave) CMyClass();
    *   ...
    *   return ret;
    *   }
    *
    * CMyClass* CMyClass::Do()
    *   {
    *   FUNC_LOG;
    *   return iMyClass;
    *   }
    *
    */
    #define FUNC_LOG TFuncLog _fl( __PRETTY_FUNCTION__, 0 );

    /*
    * Use with LC methods
    *
    * CMyClass* CMyClass::DoLC()
    *   {
    *   CMyClass* ret = NULL;
    *   FUNC_LOG_LC( ret );
    *   ...
    *   return ret;
    *   }
    */
    #define FUNC_LOG_LC( aPtr ) TFuncLog _fl( __PRETTY_FUNCTION__, 0, (TAny**)&aPtr );

#ifdef CLIENT_TRACE

    /*
    * Used like FUNC_LOG. Prints client info useful for client side executed API methods.
    */
    #define FUNC_LOG_WITH_CLIENT TFuncLog _fl( __PRETTY_FUNCTION__, KFuncLogClient );

    /*
    * Used like FUNC_LOG_LC. Prints client info useful for client side executed API methods.
    */ 
    #define FUNC_LOG_WITH_CLIENT_LC( aPtr )\
        TFuncLog _fl( __PRETTY_FUNCTION__, KFuncLogClient, (TAny**)&aPtr );

#else //CLIENT_TRACE not defined

    #define FUNC_LOG_WITH_CLIENT FUNC_LOG
    #define FUNC_LOG_WITH_CLIENT_LC( aPtr ) FUNC_LOG_LC( aPtr )

#endif //CLIENT_TRACE

#else//FUNC_TRACE not defined

    #define FUNC_LOG
    #define FUNC_LOG_LC( ptr )
    #define FUNC_LOG_WITH_CLIENT
    #define FUNC_LOG_WITH_CLIENT_LC( ptr )

#endif//FUNC_TRACE

//-----------------------------------------------------------------------------
// Timestamp trace macros
//-----------------------------------------------------------------------------
//
#ifdef TIMESTAMP_TRACE

    #define PREFIX_TIMESTAMP( aCaption )\
        _PREFIX_TRACE_2("[TIMESTAMP] (%d:%02d:%02d.%06d us) ",aCaption)
    #define CURRENT_TIME( aDt ) TDateTime aDt; { TTime t; t.HomeTime(); aDt = t.DateTime(); }
    #define EXTRACT_TIME( aDt ) aDt.Hour(), aDt.Minute(), aDt.Second(), aDt.MicroSecond()

    #ifdef TRACE_INTO_FILE

        #define TIMESTAMP( aCaption )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ) );\
            }

        #define TIMESTAMP_1( aCaption, aP1 )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1 );\
            }

        #define TIMESTAMP_2( aCaption, aP1, aP2 )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                 EFileLoggingModeAppend,\
                PREFIX_TIMESTAMP(aCaption),EXTRACT_TIME( dt ),aP1,aP2 );\
            }

        #define TIMESTAMP_3( aCaption, aP1, aP2, aP3 )\
            {\
            CURRENT_TIME( dt );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend,\
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

//-----------------------------------------------------------------------------
// Trace assert macros
//-----------------------------------------------------------------------------
//
#ifdef _DEBUG

    #ifdef TRACE_INTO_FILE

        #define PANIC_TRACE_2( aMsg, aP1, aP2 )\
            {\
            TPtrC8 trace( _S8( aMsg ) );\
            RFileLogger::WriteFormat( DriveAndPath( KTraceFile ), NameAndExt( KTraceFile ),\
                EFileLoggingModeAppend, trace, aP1, aP2 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define PANIC_TRACE_2( aMsg, aP1, aP2 )\
            {\
            RDebug::Printf( aMsg, aP1, aP2 );\
            }

    #endif //TRACE_INTO_FILE not define

    static void Panic( const TDesC8& aFileName, const TInt aLineNum )
        {
        PANIC_TRACE_2( "Assertion failed in file=%S, line=%d", &aFileName, aLineNum );
        User::Invariant();
        }

    #define ASSERT_ALWAYS_TRACE Panic( _L8(__FILE__), __LINE__ );
    #define ASSERT_TRACE( _s ) if ( !( _s ) ) { ASSERT_ALWAYS_TRACE; }

#else // _DEBUG

    #define ASSERT_ALWAYS_TRACE
    #define ASSERT_TRACE( _s )

#endif // _DEBUG

//-----------------------------------------------------------------------------
// Event logging macros
//-----------------------------------------------------------------------------
//
#ifdef LOG_INTO_FILE

    #define _PREFIX_LOG( aMsg ) _PREFIX_TRACE( "[LOG] " L##aMsg )

    class TLogObjectAppend
        {
    public:
        inline TLogObjectAppend()
            {
            if ( iFlogger.Connect() == KErrNone )
                {
                iFlogger.CreateLog( DriveAndPath(KLogFile), NameAndExt(KLogFile),
                    EFileLoggingModeAppend );
                }
            }
        inline ~TLogObjectAppend()
            {
            iFlogger.Close();
            }
    public:
        RFileLogger iFlogger;
        };

   class TLogObjectNew
        {
    public:
        inline TLogObjectNew()
            {
            if ( iFlogger.Connect() == KErrNone )
                {
                iFlogger.CreateLog( DriveAndPath(KLogFile), NameAndExt(KLogFile),
                    EFileLoggingModeOverwrite );
                }
            }
        inline ~TLogObjectNew()
            {
            iFlogger.Close();
            }
    public:
        RFileLogger iFlogger;
        };

    /*
    * Use LOG_OBJECT as a member in class declaration or in a method needing logging support
    * before using any other log macros
    *
    * DO NOT enter semicolon after this macro when used in class declaration to avoid compiler
    * warning.
    */
    #define LOG_OBJECT TLogObjectAppend _floggerObject;

    /*
    * Use LOG_OBJECT_NEW as a member in class declaration or in a method needing logging support
    * before using any other log macros. Macro overwrites the existing log file.
    *
    * DO NOT enter semicolon after this macro when used in class declaration to avoid compiler
    * warning.
    */
    #define LOG_OBJECT_NEW TLogObjectNew _floggerObject;

    /*
    * Use LOG macros to write log file
    */
    #define LOG( aMsg ) _floggerObject.iFlogger.WriteFormat( _PREFIX_LOG( aMsg ) );

    #define LOG_1( aMsg, aP1 ) _floggerObject.iFlogger.WriteFormat( _PREFIX_LOG( aMsg ), aP1 );

    #define LOG_2( aMsg, aP1, aP2 ) _floggerObject.iFlogger.WriteFormat( _PREFIX_LOG( aMsg ), aP1, aP2 );

    #define LOG_3( aMsg, aP1, aP2, aP3 ) _floggerObject.iFlogger.WriteFormat( _PREFIX_LOG( aMsg ), aP1, aP2, aP3 );

    #define LOG_4( aMsg, aP1, aP2, aP3, aP4 )\
        _floggerObject.iFlogger.WriteFormat( _PREFIX_LOG( aMsg ), aP1, aP2, aP3, aP4 );

    #define LOG_5( aMsg, aP1, aP2, aP3, aP4, aP5 )\
        _floggerObject.iFlogger.WriteFormat( _PREFIX_LOG( aMsg ), aP1, aP2, aP3, aP4, aP5 );

#else //LOG_INTO_FILE not defined

    #define LOG_OBJECT
    #define LOG_OBJECT_NEW
    #define LOG( aMsg )
    #define LOG_1( aMsg, aP1 )
    #define LOG_2( aMsg, aP1, aP2 )
    #define LOG_3( aMsg, aP1, aP2, aP3 )
    #define LOG_4( aMsg, aP1, aP2, aP3, aP4 )
    #define LOG_5( aMsg, aP1, aP2, aP3, aP4, aP5 )

#endif //LOG_INTO_FILE

#endif // TRACE_H
