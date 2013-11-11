// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Central place for nice debug-type macros & functions
// 
//

#ifndef DEBUG_H
#define DEBUG_H

#undef __FLOGGING__
#ifdef _DEBUG
#define __FLOGGING__
#endif

#ifdef __SECURITY_ON__
#undef __SECURITY_ON__
#endif
//#define __SECURITY_ON__

#include <flogger.h>
#include <e32std.h>


extern TInt sdp_debug_level;


#include <f32file.h>


// Use level 0 for production, 1 for verification, >=2 for debug
// 0	Errors
// 1	Warnings
// 2	Function calls
// 3	Key values
// 4	Full packet logging
// 5	Insane

#ifdef __FLOGGING__

_LIT(KLogFile,"sdplog.txt");
_LIT(KLogDirFullName,"c:\\logs\\bt\\");
_LIT(KLogDir,"BT");
const TInt KLogIndexOffset=5;

#define SDP_DEBUG(n, arg) {if(sdp_debug_level >= (n)){arg ;}}

#define FLOG(a) {FPrint(a);}  
#define FTRACE(a) {a;}

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
//	TBuf<32> logFile(KLogFile);
//	logFile[KLogIndexOffset]=TText('0'+(TInt)Dll::Tls());
//	RFileLogger::WriteFormat(KLogDir, logFile, EFileLoggingModeAppend, aFmt, list);
	RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list);
	}

inline void FHex(const TUint8* aPtr, TInt aLen)
	{
	RFileLogger::HexDump(KLogDir, KLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
	}

inline void FHex(const TDesC8& aDes)
	{
	FHex(aDes.Ptr(), aDes.Length());
	}

#else // i.e. __FLOGGING__ undefined

#ifdef _DEBUG // to quell linker warnings
#define SDP_DEBUG(n, arg) {FDummy();}
#define FLOG(a) {FDummy();}
#define FTRACE(a) {FDummy();}
inline void FDummy()
	{
	RFileLogger dummy;
	}
#else
#define SDP_DEBUG(n, arg)
#define FLOG(a)
#define FTRACE(a)
#endif

#endif //	__FLOGGING__


#endif // DEBUG_H
