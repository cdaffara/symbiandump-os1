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
* Description:  Defines trace macros for Orientation SSY
*
*/


#ifndef __SSYTRACE_H__
#define __SSYTRACE_H__


// INCLUDES
#include <e32debug.h>

// CONSTANTS
enum TSsyTraceGroup
    {
    ENotrace  = 0x00000000, // No traces    
    EMust     = 0x00000001, // must traces, always needed
    EError    = 0x00000002, // all error situation traces
    EFlow     = 0x00000004, // execution flow traces
    EExtended = 0x00000008  // extended traces, better to have than not    
    };

#ifdef __WINS__ // orientation ssy generates a lot of trace which slows emulator down
const TUint32 KSsyActiveTraceMask = 
      ENotrace
    + EError
    + EMust;   
#else
const TUint32 KSsyActiveTraceMask = 
      ENotrace
    + EFlow
    + EError
    + EMust
    + EExtended;   
#endif

#undef AUTO_ORIENTAION_TEST

// MACRO DEFINITIONS
#ifdef _DEBUG

    #define SSY_LOCATION( traceMask )               if ( traceMask & EError )                            \
                                                        {                                                \
                                                        RDebug::Printf( "ORIENTATIONSSY:File:%s, Function:%s, Line:%u",  __FILE__, __FUNCTION__, __LINE__ );  \
                                                        }                                                \

    #define SSY_TRACE_IN()                          if ( KSsyActiveTraceMask & EFlow )                   \
                                                        {                                                \
                                                        RDebug::Printf( "ORIENTATIONSSY:=> %s",  __FUNCTION__ );\
                                                        }                                                \

    #define SSY_TRACE_OUT()                         if ( KSsyActiveTraceMask & EFlow )                   \
                                                        {                                                \
                                                        RDebug::Printf( "ORIENTATIONSSY:<= %s",  __FUNCTION__ );\
                                                        }                                                \

    #define SSY_TRACE( traceMask, msg, args... )    if ( traceMask & KSsyActiveTraceMask )               \
                                                        {                                                \
                                                        SSY_LOCATION( traceMask );                       \
                                                        RDebug::Print( _L(msg), ##args );                \
                                                        }                                                \

    #define SSY_TRACE_TRAP(_r, _s)   TRAPD( _r, _s );              \
                                     if ( _r != KErrNone )         \
                                         {                         \
                                         SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Registering channels to the client failed with error code %d", _r );     \
                                         User::Leave( _r );        \
                                         }                         \

#else
    #define SSY_TRACE_IN()
    #define SSY_TRACE_OUT()
    #define SSY_TRACE( traceMask, msg, args... )    
    #define SSY_TRACE_TRAP(_r, _s)     _s;
#endif

#define ASSERT_ALWAYS_SSY( condition )      if ( !(condition) ) RDebug::Printf( "ORIENTATIONSSY:ASSERTION:File:%s, Function:%s, Line:%u",  __FILE__, __FUNCTION__, __LINE__ ); \
                                                    __ASSERT_ALWAYS( condition, User::Panic( _L( "SSY:ASSERTION" ), __LINE__ ) );   \

#define ASSERT_DEBUG_SSY( condition )       if ( !(condition) ) RDebug::Printf( "ORIENTATIONSSY:DEBUG ASSERTION:File:%s, Function:%s, Line:%u",  __FILE__, __FUNCTION__, __LINE__ ); \
                                                    __ASSERT_DEBUG( condition, User::Panic( _L( "SSY:ASSERTION" ), __LINE__ ) );    \


#endif // __SSYTRACE_H__

// End of File
