// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#if !defined(__LOG_H__)
#define __LOG_H__

#if 1
#include "guqos_log.h"
#else

#if !defined(_DEBUG) || EPOC_SDK < 0x06000000
#  define LOG(a)
#else
#  define LOG(a) a 
#  define _LOG

#include <flogger.h>


_LIT(KGUQoSLogFolder,"guqos");
_LIT(KGUQoSLogFile,"guqos.txt");  

class Log
	{
public:
	static inline void Write(const TDesC& aDes);
	static inline void Printf(TRefByValue<const TDesC> aFmt, ...);
	static inline void Printf(TRefByValue<const TDesC8> aFmt, ...);
	static inline void HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen);
	};

inline void Log::Write(const TDesC& aDes)
{
	RFileLogger::Write(KGUQoSLogFolder(), KGUQoSLogFile(), EFileLoggingModeAppend, aDes);
}
	

inline void Log::Printf(TRefByValue<const TDesC> aFmt, ...)
{
	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KGUQoSLogFolder(), KGUQoSLogFile(), EFileLoggingModeAppend, aFmt, list);
}

inline void Log::Printf(TRefByValue<const TDesC8> aFmt, ...)
{
	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KGUQoSLogFolder(), KGUQoSLogFile(), EFileLoggingModeAppend, aFmt, list);
}

inline void Log::HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
{
	RFileLogger::HexDump(KGUQoSLogFolder(), KGUQoSLogFile(), EFileLoggingModeAppend, aHeader, aMargin, aPtr, aLen);
}

#endif // !defined(_DEBUG) || EPOC_SDK < 0x06000000
#endif //1

#endif //__LOG_H__
