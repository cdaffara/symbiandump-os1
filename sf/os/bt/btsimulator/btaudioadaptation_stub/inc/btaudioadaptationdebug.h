/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Debug definitions for BT Audio Adaptation 
*
*/


#ifndef BT_AUDIO_ADAPTATION_DEBUG_H 
#define BT_AUDIO_ADAPTATION_DEBUG_H 

#include <f32file.h>


#ifdef _DEBUG

// Print options

#define KPRINTERROR		0x00000001 // Print error
#define KPRINTFTRACE	0x00000002 // Print function trace
#define KPRINTSTATE		0x00000004 // Print state machine infos

const TInt KDebugMask = KPRINTERROR	| KPRINTFTRACE | KPRINTSTATE ; 


// Un-comment/comment this definition to enable/disable memory tracing
//#define MEMTRACE

#ifdef __WINS__

// File logging for WINS
#define __FLOGGING__

#else

// Logging with RDebug for target HW
#define __CLOGGING__

#endif //__WINS__

#endif // _DEBUG


// ==================================================================
// File logging
// ==================================================================
//
#if  defined(__FLOGGING__)

#include <e32std.h>

_LIT(KLogFile,"BTAudioAdaptationLog.txt");
_LIT(KLogDirFullName,"c:\\logs\\bt\\");
_LIT(KLogDir,"BT");

#include <f32file.h>
#include <flogger.h>

#define FLOG(a) { FPrint(a); }
  
#define FTRACE(a) { a; }

#define BT_AUDIO_ADAPTATION_TRACE_OPT(a,p) {if((KDebugMask)&(a))p;}




inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list);
#ifdef MEMTRACE
    TInt size;
    User::Heap().AllocSize(size);   
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, _L("[BTAudioAdaptation]\t MEM\tAllocated from heap: %d B. Total heap size: %d B"), size, User::Heap().Size());
#endif
    }


inline void FHex(const TUint8* aPtr, TInt aLen)
    {
    RFileLogger::HexDump(KLogDir, KLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
    }


inline void FHex(const TDesC8& aDes)
    {
    FHex(aDes.Ptr(), aDes.Length());
    }


// ==================================================================
// RDebug logging
// ==================================================================
//
#elif defined(__CLOGGING__)

#include <e32svr.h>

#define FLOG(a) { RDebug::Print(a);  }

#define FHex(a)

#define FTRACE(a) { a; }

#define BT_AUDIO_ADAPTATION_TRACE_OPT(a,p) {if((KDebugMask)&(a))p;}


inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    TInt tmpInt = VA_ARG(list, TInt);
    TInt tmpInt2 = VA_ARG(list, TInt);
    TInt tmpInt3 = VA_ARG(list, TInt);
    VA_END(list);
    RDebug::Print(aFmt, tmpInt, tmpInt2, tmpInt3);
    }


// ==================================================================
// No loggings --> Reduced binary size
// ==================================================================
//
#else // if no __FLOGGING__ and no __CLOGGING__

#define FLOG(a)

#define FTRACE(a)

#define FHex(a)

#define BT_AUDIO_ADAPTATION_TRACE_OPT(a,p)

#endif // ...loggings 

#endif // BTAUDIOADAPTATION_DEBUG_H

// End of File
