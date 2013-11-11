// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// System
// 
//

#include <e32base.h>
#include <e32debug.h>
#include <flogger.h>
#include <f32file.h>

// Component
#include "lbstestlogger.h"

const TInt KLbsDevLogMaxBufSize = 256;
_LIT(KTimeFormat, "%H:%T:%S.%C");
_LIT(KLogPath, "\\logs\\LbsTestLogs\\");
_LIT(KLbsDevLogFolder, "LbsTestLogs");
_LIT(KLbsDevLogFile,"lbstest.txt");
_LIT(KLbsDevLogError, "Error: ");
_LIT(KLbsDevLogWarning, "Warning: ");
_LIT(KLbsDevLogSep," | ");
_LIT(KLbsDevLog, "LbsDevLog");


const TInt KLbsHexDumpWidth=16;
_LIT(KLbsFirstFormatString,"%04x : ");
_LIT(KLbsSecondFormatString,"%02x ");
_LIT(KLbsThirdFormatString,"%c");
_LIT(KLbsThreeSpaces,"   ");
_LIT(KLbsTwoSpaces,"  ");
const TText KLbsFullStopChar='.';

//-----------------------------------------------------------------------------
// LbsTestLogger
//-----------------------------------------------------------------------------

void LbsTestLogger::CreateLogDir()
	{
	TInt err;
	
	// Create the directory
	RFs fs;
	err = fs.Connect();
	if(KErrNone == err)
		{
		err = fs.MkDirAll(KLogPath);
		fs.Close();	
		}
	}
	

/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt TDes string reference 
@internalTechnology
@released
*/
void LbsTestLogger::Write(TLogType aType, TLogPriority aPrior, TRefByValue<const TDesC16> aFmt, ...)
	{
	RFileLogger logger;
	TInt err = logger.Connect();
	
	if (err == KErrNone)
		{
		VA_LIST list;
		VA_START(list, aFmt);
		TBuf16<KLbsDevLogMaxBufSize> strList;
		strList.FormatList(aFmt, list);
		VA_END(list);
		TBuf16<KLbsDevLogMaxBufSize> txt;
		
		CreateLogTxt(aType, aPrior, txt);
		txt.Append(strList.Left(KLbsDevLogMaxBufSize - txt.Length()));

		CreateLogDir();
		
		logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeAppend);
		logger.SetDateAndTime(EFalse, EFalse);
		logger.Write(txt);

		}
		
	logger.Close();
	}

/** Static function to dump the hex data
@param aPrior Log entry priority
@param aPtr TUnit8 pointer to hex data
@param aLen length of hex data
@internalTechnology
@released
*/
void LbsTestLogger::HexDump(TLogPriority aPrior, const TUint8 *aPtr, TInt aLen)
	{
	if (aPtr==NULL)     // nothing to do
		return;

	RFileLogger logger;
	TInt err = logger.Connect();
	CreateLogDir();	
	logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeAppend);
	logger.SetDateAndTime(EFalse, EFalse);
	
	// based on RFileLogger
	TBuf<KLbsDevLogMaxBufSize> buf;
	TBuf8<KLbsDevLogMaxBufSize> temp;
	TInt i=0;
	TBuf8<KLbsDevLogMaxBufSize> prefix;
	CreateLogTxt(ELogNormal, aPrior, prefix);
	
	while (aLen>0)
		{
		TInt n=(aLen>KLbsHexDumpWidth ? KLbsHexDumpWidth : aLen);
		buf.Copy(prefix);
		buf.AppendFormat(KLbsFirstFormatString,i);
		
		TInt j;
		for (j=0; j<n; j++)
			buf.AppendFormat(KLbsSecondFormatString,aPtr[i+j]);
			
		while (j++<KLbsHexDumpWidth)
			buf.Append(KLbsThreeSpaces);
			
		buf.Append(KLbsTwoSpaces);
		for (j=0; j<n; j++)
			buf.AppendFormat(KLbsThirdFormatString,(aPtr[i+j]<32 || aPtr[i+j]>126) ? KLbsFullStopChar : aPtr[i+j]);

		logger.Write(buf);

		buf.SetLength(0);
		temp.SetLength(0);
		aLen-=n;
		i+=n;
		}

	logger.Close();
	}
	
/** private function, create common log text
@param aPrior Log entry priority
@param aBuf The log prefix buffer
@internalTechnology
@released
*/
void LbsTestLogger::CreateLogTxt(TLogType aType, TLogPriority aPrior, TDes16& aBuf)
	{
	TTime currentTime;
	currentTime.UniversalTime();
	TBuf16<32> cTimeStr;
	TInt err = KErrNone;
	TRAP(err, currentTime.FormatL(cTimeStr, KTimeFormat);)
	if(err)
		User::Panic(KLbsDevLog, err);
	
	TFileName dirName = RProcess().FileName();
	TInt16 pos = dirName.LocateReverse('\\') + 1;
	TPtr16 fileName = dirName.MidTPtr(pos);
	TUint64 procId = RProcess().Id().Id();
	
	aBuf.Append(cTimeStr);
	aBuf.Append(KLbsDevLogSep);

	aBuf.Append(fileName);
	aBuf.Append(KLbsDevLogSep);
	
	//aBuf.AppendFormat(_L16("%LX"),procId);
	//aBuf.Append(KLbsDevLogSep);
	
	aBuf.AppendFormat(_L16("P%d"),aPrior);
	aBuf.Append(KLbsDevLogSep);
		
	if (aType == ELogError)
		{
		aBuf.Append(KLbsDevLogError);
		}
	else if (aType == ELogWarning)
		{
		aBuf.Append(KLbsDevLogWarning);
		}
	}


/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt Log entry
@internalTechnology
@released
*/
void LbsTestLogger::Write(TLogType aType, TLogPriority aPrior, TRefByValue<const TDesC8> aFmt, ...)
	{
	RFileLogger logger;
	TInt err = logger.Connect();
	
	if (err == KErrNone)
		{
		VA_LIST list;
		VA_START(list, aFmt);
		TBuf8<KLbsDevLogMaxBufSize> strList;
		strList.FormatList(aFmt, list);
		VA_END(list);

		TBuf8<KLbsDevLogMaxBufSize> txt;
		CreateLogTxt(aType, aPrior, txt);
		txt.Append(strList.Left(KLbsDevLogMaxBufSize - txt.Length()));

		CreateLogDir();
		logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeAppend);
		logger.SetDateAndTime(EFalse, EFalse);
		logger.Write(txt);

		}
		
	logger.Close();
	}

/** private function, create common log text
@param aPrior Log entry priority
@param aBuf The log prefix buffer
@internalTechnology
@released
*/
void LbsTestLogger::CreateLogTxt(TLogType aType, TLogPriority aPrior, TDes8& aBuf)
	{
	TTime currentTime;
	currentTime.UniversalTime();
	TBuf<32> cTimeStr;
	TInt err = KErrNone;
	TRAP(err, currentTime.FormatL(cTimeStr, KTimeFormat);)
	if(err)
		User::Panic(KLbsDevLog, err);
	
	TBuf8<32> cTimeStr8;
	cTimeStr8.Copy(cTimeStr);
	
	TFileName dirName = RProcess().FileName();
	TInt pos = dirName.LocateReverse('\\') + 1;
	TPtr fileName = dirName.MidTPtr(pos);
	TUint64 procId = RProcess().Id().Id();
	
	aBuf.Append(cTimeStr8);
	aBuf.Append(KLbsDevLogSep);

	aBuf.Append(fileName);
	aBuf.Append(KLbsDevLogSep);
	
	//aBuf.AppendFormat(_L8("%LX"),procId);
	//aBuf.Append(KLbsDevLogSep);
	
	aBuf.AppendFormat(_L8("P%d"),aPrior);
	aBuf.Append(KLbsDevLogSep);
	
	if (aType == ELogError)
		{
		aBuf.Append(KLbsDevLogError);
		}
	else if (aType == ELogWarning)
		{
		aBuf.Append(KLbsDevLogWarning);
		}
	}
