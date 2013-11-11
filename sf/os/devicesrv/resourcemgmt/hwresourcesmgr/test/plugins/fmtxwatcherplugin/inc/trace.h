/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef FMTXWATCHERDEBUG_H
#define FMTXWATCHERDEBUG_H

#include <e32base.h>
#include <e32debug.h>


#ifdef _DEBUG
    #define FMTX_WATCHER_INFO_LOG_ENABLED
    #define FMTX_WATCHER_ERROR_LOG_ENABLED
#endif // _DEBUG


// Info logging
#ifdef FMTX_WATCHER_INFO_LOG_ENABLED

 #define INFO_LOG( aMsg ) { _LIT( KMsg, aMsg ); RDebug::Print( KMsg ); }

 #define INFO_LOG1( aMsg, aArg1 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1 ); }

 #define INFO_LOG2( aMsg, aArg1, aArg2 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2 ); }

 #define INFO_LOG3( aMsg, aArg1, aArg2, aArg3 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2, aArg3 ); }

// Function log object
_LIT8( KFuncNameTerminator, "(" );
_LIT8( KFuncLeavePattern, "L" );

class TFuncLog
    {
public:
    static void Cleanup( TAny* aPtr )
        {
        TFuncLog* self = static_cast< TFuncLog* >( aPtr );
        self->iLeft = ETrue;
        RDebug::Printf( "%S-LEAVE", &self->iFunc ); // Leave detected
        }

    inline TFuncLog( const char* aFunc ) :
            iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
            iLeft( EFalse ),
            iCanLeave( EFalse )
        {
        TInt pos( iFunc.Find( KFuncNameTerminator ) );
        if( pos != KErrNotFound )
            {
            iFunc.Set( iFunc.Left( pos ) );
            TInt patLen( KFuncLeavePattern().Length() );
            iCanLeave = iFunc.Length() > patLen &&
                !iFunc.Right( patLen ).Compare( KFuncLeavePattern );
            }
        RDebug::Printf( "%S-START", &iFunc );
        }

    inline ~TFuncLog()
        {
        if ( !iLeft )
            {
            if ( iCanLeave )
                {
                CleanupStack::Pop( this ); // Pop the cleanup item
                }
            RDebug::Printf( "%S-END", &iFunc ); // Normally finished
            }
        }

    TPtrC8 iFunc;
    TBool iLeft;
    TBool iCanLeave;
    };

 #define FUNC_LOG\
  TFuncLog _fl( __PRETTY_FUNCTION__ );\
  TCleanupItem _flCi( TFuncLog::Cleanup, &_fl );\
  if ( _fl.iCanLeave ) { CleanupStack::PushL( _flCi ); }

 #define TIMESTAMP( aCaption )\
    {\
    TTime t;\
    t.UniversalTime();\
    RDebug::Printf( "%s %d us", aCaption, I64INT( t.Int64() ) );\
    }

#else // FMTX_WATCHER_INFO_LOG_ENABLED

 #define INFO_LOG( aMsg )

 #define INFO_LOG1( aMsg, aArg1 )

 #define INFO_LOG2( aMsg, aArg1, aArg2 )

 #define INFO_LOG3( aMsg, aArg1, aArg2, aArg3 )

 #define FUNC_LOG

 #define TIMESTAMP( aCaption )

#endif // FMTX_WATCHER_INFO_LOG_ENABLED


// Error logging
#ifdef FMTX_WATCHER_ERROR_LOG_ENABLED

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

#else // FMTX_WATCHER_ERROR_LOG_ENABLED

 #define ERROR_LOG( aMsg )

 #define ERROR_LOG1( aMsg, aArg1 )

 #define ERROR_LOG2( aMsg, aArg1, aArg2 )

 #define ERROR_LOG3( aMsg, aArg1, aArg2, aArg3 )

 // Remove compiler warning
 #define LOG_IF_ERROR( aErr, aMsg ) ( aErr ) = ( aErr );

 #define LOG_IF_ERROR1( aErr, aMsg, aArg1 )  ( aErr ) = ( aErr );

 #define LOG_IF_ERROR2( aErr, aMsg, aArg1, aArg2 )  ( aErr ) = ( aErr );

 #define LOG_IF_ERROR3( aErr, aMsg, aArg1, aArg2, aArg3 ) ( aErr ) = ( aErr );

#endif // FMTX_WATCHER_ERROR_LOG_ENABLED


#endif // FMTXWATCHERDEBUG_H

// End of File
