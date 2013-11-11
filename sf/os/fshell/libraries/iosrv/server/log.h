// log.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __LOG_H__
#define __LOG_H__

#include <e32base.h>
#include <f32file.h>
#include "server.h"

#ifdef _DEBUG
//#define IOSRV_LOGGING
#endif

#ifdef IOSRV_LOGGING

#ifdef IOSRV_LOGGING_USES_CLOGGER // This is defined in the mmp
#include <fshell/clogger.h>
#endif

#define LOG(x) x
#define OBJ_NAME(x) TName objName((x).Name())

class CIoLog : public CBase
	{
public:
	static CIoLog* NewL(RFs& aFs);
	~CIoLog();
	static void StartServiceLC(const RMsg& aMessage);
	static void LogCompletion(const RMsg& aMessage, TInt aError);
	static void Write(const TDesC& aData);
	static void Printf(TRefByValue<const TDesC> aFmt, ...);
	static const TDesC* StringifyError(TInt aError);
	static const TDesC* StringifyOpcode(TInt aOpCode);
private:
	CIoLog(RFs& aFs);
	void ConstructL();
	static CIoLog& Self();
	static void EndService(TAny* aSelf);
private:
	RFs& iFs;
#ifdef IOSRV_LOGGING_USES_CLOGGER
	RClogger iClogger;
#else
	RFile iFile;
	TBuf8<0x100> iNarrowBuf;
#endif
	TBuf<0x100> iScratchBuf;
	TBool iInServiceL;
	};

#if !defined(__WINS__) || defined (EKA2)
extern CIoLog* gLog;
#endif

#else

#define LOG(x)
#define OBJ_NAME(x)

#endif // IOSRV_LOGGING

#endif // __LOG_H__
