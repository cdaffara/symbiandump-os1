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

#ifndef BT_DEBUG_H
#define BT_DEBUG_H

#define __FLOGGING__
#undef __FLOGGING__

#include <e32std.h>

_LIT(KLogFile,"btlog0.txt");
const TInt KLogIndexOffset=5;
_LIT(KLogDirFullName,"c:\\logs\\bt\\");
_LIT(KLogDir,"BT");

#include <f32file.h>
#include <flogger.h>

#define FLOG(a) {FPrint(a);}
#define FTRACE(a) {a;}


#ifdef __FLOGGING__
//#define MEMTRACE
inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf<32> logFile(KLogFile);
	logFile[KLogIndexOffset]=TText('0'+(TInt)Dll::Tls());
	RFileLogger::WriteFormat(KLogDir, logFile, EFileLoggingModeAppend, aFmt, list);
#ifdef MEMTRACE
	TInt size;
	User::Heap().AllocSize(size);
	
	RFileLogger::WriteFormat(KLogDir, logFile, EFileLoggingModeAppend, _L("Mem usage: %d high: %d"), size, User::Heap().Size());
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

#else // i.e. __FLOGGING__ undefined

inline void FPrint(const TRefByValue<const TDesC> /*aFmt*/,...) 
	{
	}

inline void FHex(const TUint8* /*aPtr*/, TInt /*aLen*/)
	{
	}

inline void FHex(const TDesC8& /*aDes*/)
	{
	}

#endif //	__FLOGGING__


#endif // __BT_DEBUG_H__
