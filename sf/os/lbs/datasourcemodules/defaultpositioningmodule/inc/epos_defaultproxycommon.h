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



#ifndef EPOS_DEFAULTPROXYCOMMON_H
#define EPOS_DEFAULTPROXYCOMMON_H

#include <e32std.h>
#include <flogger.h>

//Panic category
_LIT( KPosDefaultProxyPanic, "EPosDefault" );

//Panic code
enum 
    {
    EDefaultProxyPanic_PosInfoNotNull       = 1,
    EDefaultProxyPanic_SimualtaneousLR      = 2,
    EDefaultProxyPanic_PosInfoSizeMismatch  = 3,
    EDefaultProxyPanic_PsyNotFoundInStateManager = 4,
    EDefaultProxyPanic_PsyNotFoundInPsyListHandler = 5,
    EDefaultProxyPanic_PsyNotFoundInExtGpsPsyMinitor = 6,
    EDefaultProxyPanic_GlobalObjectNotConstructed = 7
    };

//Default proxy panic
#define DefaultProxyPanic(x) User::Panic( KPosDefaultProxyPanic, x )


// Logging macros
#ifdef _DEBUG

_LIT( KLoggingFolder, "DPsy" );
_LIT( KLoggingFile, "log.txt" );
_LIT( KLoggingFullName, "c:\\logs\\DPsy\\log.TXT" );

#define TRACETEXT( S1 )         { RFileLogger::Write( KLoggingFolder(),\
KLoggingFile(), EFileLoggingModeAppend, S1 ); }

#define TRACESTRING( S1 )       { \
    _LIT( tempLogDes, S1 ); \
    RFileLogger::Write( KLoggingFolder(), KLoggingFile(), \
    EFileLoggingModeAppend, tempLogDes() ); \
    }

#define TRACESTRING2( S1, S2 )  { \
    _LIT( tempLogDes, S1 ); \
    RFileLogger::WriteFormat( KLoggingFolder(), KLoggingFile(), \
    EFileLoggingModeAppend, TRefByValue < const TDesC>( tempLogDes() ), S2 ); \
    }

#else   // _DEBUG
#define CLEARTRACELOG 
#define TRACETEXT( S1 )
#define TRACESTRING( S1 )
#define TRACESTRING2( S1, S2 )
#endif  // _DEBUG


#endif      // EPOS_DEFAULTPROXYCOMMON_H  
            
// End of File
