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


#ifndef DISKNOTIFYHANDLERDEBUG_H
#define DISKNOTIFYHANDLERDEBUG_H


//  INCLUDES
#include <e32base.h>
#include <e32debug.h>

// Function trace
//#define DISK_NOTIFY_HANDLER_FUNC_LOG

// Informative trace
//#define DISK_NOTIFY_HANDLER_INFO_LOG

// Error trace
//#define DISK_NOTIFY_HANDLER_ERROR_LOG

#ifdef _DEBUG
 #ifndef DISK_NOTIFY_HANDLER_ERROR_LOG
  #define DISK_NOTIFY_HANDLER_ERROR_LOG
 #endif // DISK_NOTIFY_HANDLER_ERROR_LOG
#endif // _DEBUG


// Function logging
#ifdef DISK_NOTIFY_HANDLER_FUNC_LOG

// Function log object
_LIT8( KFuncNameTerminator, "(" );
_LIT8( KFuncLeavePattern, "L" );

class TFuncLog
    {
public:
    static void Cleanup( TAny* aPtr )
        {
        // Leave detected
        TFuncLog* self = static_cast< TFuncLog* >( aPtr );
        self->iLeft = ETrue;
        if ( self->iLogClient )
            {
            TName name( RThread().Name() );
            _LIT( KMsg, "Client=%S-LEAVE" );
            RDebug::Print( KMsg, &name );
            }
        RDebug::Printf( "%S-LEAVE", &self->iFunc );
        }

    inline TFuncLog( const char* aFunc, TBool aLogClient ) :
            iFunc( aFunc ? _S8( aFunc ) : _S8("") ),
            iLogClient( aLogClient ),
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
        if ( iLogClient )
            {
            TName name( RThread().Name() );
            _LIT( KMsg, "Client=%S-START" );
            RDebug::Print( KMsg, &name );
            }
        }

    inline ~TFuncLog()
        {
        if ( !iLeft )
            {
            if ( iCanLeave )
                {
                CleanupStack::Pop( this ); // Pop the cleanup item
                }
            // Normally finished
            if ( iLogClient )
                {
                TName name( RThread().Name() );
                _LIT( KMsg, "Client=%S-END" );
                RDebug::Print( KMsg, &name );
                }
            RDebug::Printf( "%S-END", &iFunc );
            }
        }

    TPtrC8 iFunc;
    TBool iLogClient;
    TBool iLeft;
    TBool iCanLeave;
    };

 #define FUNC_LOG\
  TFuncLog _fl( __PRETTY_FUNCTION__, EFalse );\
  TCleanupItem _flCi( TFuncLog::Cleanup, &_fl );\
  if ( _fl.iCanLeave ) { CleanupStack::PushL( _flCi ); }

 #define FUNC_LOG_WITH_CLIENT_NAME\
  TFuncLog _fl( __PRETTY_FUNCTION__, ETrue );\
  TCleanupItem _flCi( TFuncLog::Cleanup, &_fl );\
  if ( _fl.iCanLeave ) { CleanupStack::PushL( _flCi ); }

#else // DISK_NOTIFY_HANDLER_FUNC_LOG

 #define FUNC_LOG

 #define FUNC_LOG_WITH_CLIENT_NAME

#endif // DISK_NOTIFY_HANDLER_FUNC_LOG

// Informative logging
#ifdef DISK_NOTIFY_HANDLER_INFO_LOG

 #define INFO_LOG( aMsg ) { _LIT( KMsg, aMsg ); RDebug::Print( KMsg ); }

 #define INFO_LOG1( aMsg, aArg1 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1 ); }

 #define INFO_LOG2( aMsg, aArg1, aArg2 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2 ); }

 #define INFO_LOG3( aMsg, aArg1, aArg2, aArg3 )\
    { _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aArg1, aArg2, aArg3 ); }

#else // DISK_NOTIFY_HANDLER_INFO_LOG

 #define INFO_LOG( aMsg )

 #define INFO_LOG1( aMsg, aArg1 )

 #define INFO_LOG2( aMsg, aArg1, aArg2 )

 #define INFO_LOG3( aMsg, aArg1, aArg2, aArg3 )

#endif // DISK_NOTIFY_HANDLER_INFO_LOG


// Error logging
#ifdef DISK_NOTIFY_HANDLER_ERROR_LOG

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

#else // DISK_NOTIFY_HANDLER_ERROR_LOG

 #define ERROR_LOG( aMsg )

 #define ERROR_LOG1( aMsg, aArg1 )

 #define ERROR_LOG2( aMsg, aArg1, aArg2 )

 #define ERROR_LOG3( aMsg, aArg1, aArg2, aArg3 )

 #define LOG_IF_ERROR( aErr, aMsg )

 #define LOG_IF_ERROR1( aErr, aMsg, aArg1 )

 #define LOG_IF_ERROR2( aErr, aMsg, aArg1, aArg2 )

 #define LOG_IF_ERROR3( aErr, aMsg, aArg1, aArg2, aArg3 )

#endif // DISK_NOTIFY_HANDLER_ERROR_LOG


#endif // DISKNOTIFYHANDLERDEBUG_H

// End of File
