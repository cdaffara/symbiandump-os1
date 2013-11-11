//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  This file implements the CCsyDebugLogger class.
// *               This class provides the functionality to log TSY debug messages to a file.
// *               The log file is located at \epoc32\wins\c\logs\etel.
//

// CsyDebugLogger.cpp
/*
Copies from RefLog.cpp
MAF move back to using RefLog
*/

// MAF we need to move to using RefLog.cpp - because we are duplicating code
// fixes to the code that TapRoot have copied from us.

#include <e32svr.h>

#include "CsyGlobals.h"
#include "CsyDebugLogger.h"

#ifdef _DEBUG // Only use the following code when in Debug mode

_LIT8(KDateFormat, "%02d.%02d:%4d");
_LIT8(KTimeFormat, "%02d.%02d:%02d:%06d ");

class TNoOverflow : public TDes8Overflow
/**
 * This class is used to avoid panics when logging anything that's too long.
 */
	{
public:
	virtual void Overflow(TDes8& /*aDes*/)
		{
		}
	};

void CCsyDebugLogger::WriteHeader()
/**
 * Write the log header.
 */	
	{
	TBuf8<64> buf;
	TTime now;
	now.UniversalTime();
	TDateTime dateTime;
	dateTime = now.DateTime();
	// TDateTimes Day() and Month() start at 0
	buf.Format(KDateFormat, dateTime.Day() + 1, dateTime.Month() + 1,
		dateTime.Year());

#ifndef __DEBUGSERIALPORT__
	// overwrite log file
	RFileLogger logger;
	TInt ret = logger.Connect();
	if (ret == KErrNone)
		{
		logger.SetDateAndTime(EFalse, EFalse);
		logger.CreateLog(KCsyLogDir, KCsyLogFile, EFileLoggingModeOverwrite);
		logger.Write(KCsyLogLine);
		logger.CloseLog();
		logger.Close();
		}
#endif

	CCsyDebugLogger::Write(_L8(" ")); // please leave this separator in
	CCsyDebugLogger::WriteFormat(_L8("Date: %S"), &buf);
	CCsyDebugLogger::WriteFormat(_L8("CSY 3GPP 27.010 version %d.%02d  (v8.0 build:%d)"),
			KCSY_Gsm0710MajorVersionNumber,
			KCSY_Gsm0710MinorVersionNumber,
			KCSY_Gsm0710BuildVersionNumber);
	}

void CCsyDebugLogger::Write(const TDesC8& aText)
/**
 * Write an 8-bit descriptor to the log file.
 * @param aDebugLevel Debug level of log request
 * @param aText is the text to write to the log file
 * @return void
 */	
	{
	TBuf8<KLogBufferSize> buf;
	TTime now;
	now.UniversalTime();
	TDateTime dateTime;
	dateTime = now.DateTime();
	buf.Format(KTimeFormat, dateTime.Hour(), dateTime.Minute(), 
			   dateTime.Second(), dateTime.MicroSecond());

	if (aText.Length() <= (buf.MaxLength() - buf.Length()))
		{
		buf.Append(aText);
		}
	else
		{
		buf.Append(aText.Left(buf.MaxLength() - buf.Length()));
		}

#ifdef __DEBUGSERIALPORT__

	TBuf<KLogBufferSize> buf2;
	// TEMP: Do not print single % since appear as format char to RDebug::Print
	for (TInt i = 0; i < buf.Length(); i++)
		{
		if (buf[i] == '%')
			buf2.Append(_L("."));
		else
			buf2.Append(buf[i]);
		}

	RDebug::Print(buf2);

#else
	// log to our own file
	RFileLogger logger;
	TInt ret = logger.Connect();
	if (ret == KErrNone)
		{
		logger.SetDateAndTime(EFalse, EFalse);
		logger.CreateLog(KCsyLogDir, KCsyLogFile, EFileLoggingModeAppend);
		logger.Write(buf);
		logger.CloseLog();
		logger.Close();
		}
#endif
	}

void CCsyDebugLogger::WriteFormat(TRefByValue<const TDesC8> aFmt, ...)
/**
 * Write an 8-bit format list to the log file.
 * @param aDebugLevel Debug level of log request
 * @param aFmt is the variable arguments
 * @return void
 */	
	{
	VA_LIST list;
	VA_START(list,aFmt);

	TBuf8<KLogBufferSize> buf;
	TNoOverflow overflow;
	buf.AppendFormatList(aFmt, list, &overflow);
	Write(buf);
	}

#endif // _DEBUG
