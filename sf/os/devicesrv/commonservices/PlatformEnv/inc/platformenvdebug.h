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
* Description:  Debug definitions
*
*/


#ifndef PLATFORMENVDEBUG_H
#define PLATFORMENVDEBUG_H


//  INCLUDES
#include <e32debug.h>

// Function trace
// #define PLATFORM_ENV_FUNC_LOG

// Informative trace
// #define PLATFORM_ENV_INFO_LOG

// Error trace
// #define PLATFORM_ENV_ERROR_LOG

// Client trace (used with function trace)
#define PLATFORM_ENV_CLIENT_LOG

#ifdef _DEBUG
 #ifndef PLATFORM_ENV_ERROR_LOG
  #define PLATFORM_ENV_ERROR_LOG
 #endif // PLATFORM_ENV_ERROR_LOG
#endif // _DEBUG


// Function logging
#ifdef PLATFORM_ENV_FUNC_LOG

_LIT8( KFuncNameTerminator, "(" );
_LIT8( KFuncLeavePatternL, "L" );
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
        RDebug::Printf( "%S-LEAVE", &self->iFunc ); // Leave detected
        }

    inline void LogClientStart()
        {
#ifdef PLATFORM_ENV_CLIENT_LOG
        if ( iFlags & KFuncLogClient )
            {
            _LIT( KMsg, "Client=%S-START" );
            TName name( RThread().Name() );
            RDebug::Print( KMsg, &name );
            }
#endif // PLATFORM_ENV_CLIENT_LOG
        }

    inline void LogClientEnd()
        {
#ifdef PLATFORM_ENV_CLIENT_LOG
        if ( iFlags & KFuncLogClient )
            {
            _LIT( KMsg, "Client=%S-END" );
            TName name( RThread().Name() );
            RDebug::Print( KMsg, &name );
            }
#endif // PLATFORM_ENV_CLIENT_LOG
        }

    // For non leaving and L methods
    inline TFuncLog( const char* aFunc, TUint aFlags ) :
            iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
            iFlags( aFlags ),
            iCleanupItem( Cleanup, this ),
            iLCPtr( NULL )
        {
        TInt pos( iFunc.Find( KFuncNameTerminator ) );
        if( pos != KErrNotFound )
            {
            iFunc.Set( iFunc.Left( pos ) );
            if( !iFunc.Right( KFuncLeavePatternL().Length() ).Compare( KFuncLeavePatternL ) )
                {
                iFlags |= KFuncCanLeave;
                CleanupStack::PushL( iCleanupItem ); // Ignore warnings
                }
            }
        RDebug::Printf( "%S-START", &iFunc );
        LogClientStart();
        }

    // For LC methods only
    inline TFuncLog( const char* aFunc, TUint aFlags, TAny** aLCPtr ) :
            iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
            iFlags( aFlags ),
            iCleanupItem( Cleanup, this ),
            iLCPtr( NULL )
        {
        TInt pos( iFunc.Find( KFuncNameTerminator ) );
        if( pos != KErrNotFound )
            {
            iFunc.Set( iFunc.Left( pos ) );
            iFlags |= KFuncCanLeave;
            iLCPtr = aLCPtr;
            CleanupStack::PushL( iCleanupItem ); // Ignore warnings
            }
        RDebug::Printf( "%S-START", &iFunc );
        LogClientStart();
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
            LogClientEnd();
            RDebug::Printf( "%S-END", &iFunc ); // Normally finished
            }
        }

private: // Data
    TPtrC8 iFunc;
    TUint iFlags;
    TCleanupItem iCleanupItem;
    TAny** iLCPtr;
    };
    
 #define FUNC_LOG TFuncLog _fl( __PRETTY_FUNCTION__, 0 );
 
 #define FUNC_LOG_LC( ptr ) TFuncLog _fl( __PRETTY_FUNCTION__, 0, (TAny**)&ptr );
 
 #define FUNC_LOG_WITH_CLIENT_NAME TFuncLog _fl( __PRETTY_FUNCTION__, KFuncLogClient );
 
 #define FUNC_LOG_WITH_CLIENT_NAME_LC( ptr )\
    TFuncLog _fl( __PRETTY_FUNCTION__, KFuncLogClient, (TAny**)&ptr );

#else // PLATFORM_ENV_FUNC_LOG

 #define FUNC_LOG
 
 #define FUNC_LOG_LC( ptr )
 
 #define FUNC_LOG_WITH_CLIENT_NAME
 
 #define FUNC_LOG_WITH_CLIENT_NAME_LC( ptr )

#endif // PLATFORM_ENV_FUNC_LOG

// Informative logging
#ifdef PLATFORM_ENV_INFO_LOG

 #define INFO_LOG( aMsg ) { _LIT( KMsg, aMsg ); RDebug::Print( KMsg ); }

 #define INFO_LOG1( aMsg, aArg1 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1 ); }

 #define INFO_LOG2( aMsg, aArg1, aArg2 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2 ); }

 #define INFO_LOG3( aMsg, aArg1, aArg2, aArg3 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2, aArg3 ); }

#else // PLATFORM_ENV_INFO_LOG

 #define INFO_LOG( aMsg )

 #define INFO_LOG1( aMsg, aArg1 )

 #define INFO_LOG2( aMsg, aArg1, aArg2 )

 #define INFO_LOG3( aMsg, aArg1, aArg2, aArg3 )

#endif // PLATFORM_ENV_INFO_LOG


// Error logging
#ifdef PLATFORM_ENV_ERROR_LOG

 #define ERROR_LOG( aMsg ) { _LIT( KMsg, aMsg ); RDebug::Print( KMsg ); }

 #define ERROR_LOG1( aMsg, aArg1 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1 ); }

 #define ERROR_LOG2( aMsg, aArg1, aArg2 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2 ); }

 #define ERROR_LOG3( aMsg, aArg1, aArg2, aArg3 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2, aArg3 ); }

 #define LOG_IF_ERROR( aErr, aMsg )\
    if ( ( aErr ) != KErrNone )\
        { _LIT( KMsg, aMsg ); RDebug::Print( KMsg ); }

 #define LOG_IF_ERROR1( aErr, aMsg, aArg1 )\
    if ( ( aErr ) != KErrNone )\
        { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1 ); }
        
 #define LOG_IF_ERROR2( aErr, aMsg, aArg1, aArg2 )\
    if ( ( aErr ) != KErrNone )\
        { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2 ); }

 #define LOG_IF_ERROR3( aErr, aMsg, aArg1, aArg2, aArg3 )\
    if ( ( aErr ) != KErrNone )\
        { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2, aArg3 ); }

#else // PLATFORM_ENV_ERROR_LOG

 #define ERROR_LOG( aMsg )

 #define ERROR_LOG1( aMsg, aArg1 )

 #define ERROR_LOG2( aMsg, aArg1, aArg2 )

 #define ERROR_LOG3( aMsg, aArg1, aArg2, aArg3 )

 #define LOG_IF_ERROR( aErr, aMsg )

 #define LOG_IF_ERROR1( aErr, aMsg, aArg1 )

 #define LOG_IF_ERROR2( aErr, aMsg, aArg1, aArg2 )

 #define LOG_IF_ERROR3( aErr, aMsg, aArg1, aArg2, aArg3 )

#endif // PLATFORM_ENV_ERROR_LOG


#endif // PLATFORMENVDEBUG_H

// End of File
