// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TSYLogger

// system inlcude
#include <flogger.h>		// for FLogger

// user include
#include "mslogger.h"		// Header file for this source file

// const defination
_LIT(KLogFolder,"etel");
_LIT(KLogFileName,"PhoneTsy.txt");
_LIT8(KTimeFormat,"%02d:%02d:%02d.%03d ");

const TInt KTSYLogBufferSize=400;

void TSYLogger::Write(const TDesC8& aText)
	{
	//
	// Assemble time stamp and callers text into one string
	TBuf8<KTSYLogBufferSize> buf;
	TTime now;
	now.UniversalTime();
	TDateTime t(now.DateTime());
	buf.Format(KTimeFormat,t.Hour(),t.Minute(),t.Second(),t.MicroSecond());
	buf.Append(aText);

	//
	// Open log, write text to log and close log
	RFileLogger log;
	if(KErrNone==log.Connect())
		{
		log.CreateLog(KLogFolder,KLogFileName,EFileLoggingModeAppend);
		log.SetDateAndTime(EFalse,EFalse);
		log.Write(buf);
		log.CloseLog();
		log.Close();
		}
	}


void TSYLogger::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);

	//
	// Assemble time stamp and callers text into one string
	TBuf8<KTSYLogBufferSize> buf;
	TTime now;
	now.UniversalTime();
	TDateTime t(now.DateTime());
	buf.Format(KTimeFormat,t.Hour(),t.Minute(),t.Second(),t.MicroSecond());
	buf.AppendFormatList(aFmt,list);

	//
	// Open log, write text to log and close log
	RFileLogger log;
	if(KErrNone==log.Connect())
		{
		log.CreateLog(KLogFolder,KLogFileName,EFileLoggingModeAppend);
		log.SetDateAndTime(EFalse,EFalse);
		log.Write(buf);
		log.CloseLog();
		log.Close();
		}
	}

