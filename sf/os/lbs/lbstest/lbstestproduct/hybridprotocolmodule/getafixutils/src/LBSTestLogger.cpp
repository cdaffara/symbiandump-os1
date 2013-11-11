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

// Component
#include "LBSTestLogger.h"

const TInt KLbsTestLogMaxBufSize = 150;
_LIT(KTimeFormat, "%H:%T:%S.%C");
_LIT(KLbsTestLogFolder, "lbs");
_LIT(KLbsTestLogFile,"lbstest.txt");
_LIT(KLbsTestLogError, "Error: ");
_LIT(KLbsTestLogWarning, "Warning: ");
_LIT(KLbsTestLogSep," | ");

const TInt KLbsHexDumpWidth=16;
_LIT(KLbsFirstFormatString,"%04x : ");
_LIT(KLbsSecondFormatString,"%02x ");
_LIT(KLbsThirdFormatString,"%c");
_LIT(KLbsThreeSpaces,"   ");
_LIT(KLbsTwoSpaces,"  ");
const TText KLbsFullStopChar='.';

//#define ENABLE_LBS_DEV_LOGGER_RDEBUG
//-----------------------------------------------------------------------------
// LbsTestLogger
//-----------------------------------------------------------------------------
EXPORT_C void LbsTestLogger::Initialize()
	{
	TInt stk; 
	TInt* stkPtr = &stk;
	Dll::SetTls(stkPtr);
	}


/** Static function, one parameter, overwrite the previous log
@param aFmt TDes string reference 
@internalTechnology
@released
*/
EXPORT_C void LbsTestLogger::OverWrite(const TDesC16& aFmt)
	{
	TInt stackInt;
	
	RFileLogger logger;
	TInt err = logger.Connect();
	
	if (err == KErrNone)
		{
		TBuf16<KLbsTestLogMaxBufSize> txt;
		CreateLogTxt(ELogNormal, ELogP5, &stackInt, txt);
		txt.Append(aFmt);
		
		logger.CreateLog(KLbsTestLogFolder, KLbsTestLogFile, EFileLoggingModeOverwrite);
		logger.SetDateAndTime(EFalse, EFalse);
		logger.Write(txt);
		
		#ifdef ENABLE_LBS_TEST_LOGGER_RDEBUG
		txt.Append(_L("\n\r"));
		RDebug::RawPrint(txt);
		#endif		
		}
		
	logger.Close();
	}

/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt TDes string reference 
@internalTechnology
@released
*/
EXPORT_C void LbsTestLogger::Write(TLbsTestLogType aType, TLbsTestLogPriority aPrior, TRefByValue<const TDesC16> aFmt, ...)
	{
	TInt stackInt;
	
	RFileLogger logger;
	TInt err = logger.Connect();
	
	if (err == KErrNone)
		{
		VA_LIST list;
		VA_START(list, aFmt);
		TBuf16<KLbsTestLogMaxBufSize> strList;
		strList.FormatList(aFmt, list);
		VA_END(list);
		TBuf16<KLbsTestLogMaxBufSize> txt;
		
		CreateLogTxt(aType, aPrior, &stackInt, txt);
		txt.Append(strList.Left(KLbsTestLogMaxBufSize - txt.Length()));

		logger.CreateLog(KLbsTestLogFolder, KLbsTestLogFile, EFileLoggingModeAppend);
		logger.SetDateAndTime(EFalse, EFalse);
		logger.Write(txt);
		
		#ifdef ENABLE_LBS_TEST_LOGGER_RDEBUG
		txt.Append(_L("\n\r"));
		RDebug::RawPrint(txt);
		#endif		
		}
		
	logger.Close();
	}

/** Static function to dump the hex data
@param aPrior Log entry priority
@param aPtr TUnit8 pointer to hex data
@param aLen length of hex data

*/
EXPORT_C void LbsTestLogger::HexDump(TLbsTestLogPriority aPrior, const TUint8 *aPtr, TInt aLen)
	{
	TInt stackInt;
	
	if (aPtr==NULL)     // nothing to do
		return;

	RFileLogger logger;
	TInt err = logger.Connect();
	logger.CreateLog(KLbsTestLogFolder, KLbsTestLogFile, EFileLoggingModeAppend);
	logger.SetDateAndTime(EFalse, EFalse);
	
	// based on RFileLogger
	TBuf<KLbsTestLogMaxBufSize> buf;
	TBuf8<KLbsTestLogMaxBufSize> temp;
	TInt i=0;
	TBuf8<KLbsTestLogMaxBufSize> prefix;
	CreateLogTxt(ELogNormal, aPrior, &stackInt, prefix);
	
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
		
		#ifdef ENABLE_LBS_TEST_LOGGER_RDEBUG
		buf.Append(_L("\n\r"));
		RDebug::RawPrint(buf);
		#endif				
		
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


*/
void LbsTestLogger::CreateLogTxt(TLbsTestLogType aType, TLbsTestLogPriority aPrior, TInt* aStackPtr, TDes16& aBuf)
	{
	TTime currentTime;
	currentTime.UniversalTime();
	TBuf16<32> cTimeStr;
	TRAPD(err,currentTime.FormatL(cTimeStr, KTimeFormat));
	if (err)
		{
		User::Panic(KLbsTestLogger, KErrNoMemory);
		}
	
	TFileName dirName = RProcess().FileName();
	TInt16 pos = dirName.LocateReverse('\\') + 1;
	TPtr16 fileName = dirName.MidTPtr(pos);
	TUint64 procId = RProcess().Id().Id();
	
	aBuf.Append(cTimeStr);
	aBuf.Append(KLbsTestLogSep);

	aBuf.Append(fileName);
	aBuf.Append(KLbsTestLogSep);
	
	aBuf.AppendFormat(_L16("%LX"),procId);
	aBuf.Append(KLbsTestLogSep);
	
	aBuf.AppendFormat(_L16("P%d"),aPrior);
	aBuf.Append(KLbsTestLogSep);
		
	TUint32 stackUsage = reinterpret_cast<TUint32>(Dll::Tls());
	stackUsage = stackUsage - reinterpret_cast<TUint32>(aStackPtr);
	aBuf.AppendFormat(_L16("%08X(%05d)"), aStackPtr, stackUsage);
	aBuf.Append(KLbsTestLogSep);

	if (aType == ELogError)
		{
		aBuf.Append(KLbsTestLogError);
		}
	else if (aType == ELogWarning)
		{
		aBuf.Append(KLbsTestLogWarning);
		}
	}

/** Static function, one parameter, overwrite the previous log
@param aFmt TDes string reference 


*/
EXPORT_C void LbsTestLogger::OverWrite(const TDesC8& aFmt)
	{
	TInt stackInt;
	
	RFileLogger logger;
	TInt err = logger.Connect();
	
	if (err == KErrNone)
		{
		TBuf8<KLbsTestLogMaxBufSize> txt;
		CreateLogTxt(ELogNormal, ELogP5, &stackInt, txt);
		txt.Append(aFmt);

		logger.CreateLog(KLbsTestLogFolder, KLbsTestLogFile, EFileLoggingModeOverwrite);
		logger.SetDateAndTime(EFalse, EFalse);
		logger.Write(txt);
		
		#ifdef ENABLE_LBS_TEST_LOGGER_RDEBUG
		txt.Append(_L("\n\r"));
		RDebug::RawPrint(txt);
		#endif				
		}
		
	logger.Close();
	}

/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt Log entry


*/
EXPORT_C void LbsTestLogger::Write(TLbsTestLogType aType, TLbsTestLogPriority aPrior, TRefByValue<const TDesC8> aFmt, ...)
	{
	TInt stackInt;
	
	RFileLogger logger;
	TInt err = logger.Connect();
	
	if (err == KErrNone)
		{
		VA_LIST list;
		VA_START(list, aFmt);
		TBuf8<KLbsTestLogMaxBufSize> strList;
		strList.FormatList(aFmt, list);
		VA_END(list);

		TBuf8<KLbsTestLogMaxBufSize> txt;
		CreateLogTxt(aType, aPrior, &stackInt, txt);
		txt.Append(strList.Left(KLbsTestLogMaxBufSize - txt.Length()));

		logger.CreateLog(KLbsTestLogFolder, KLbsTestLogFile, EFileLoggingModeAppend);
		logger.SetDateAndTime(EFalse, EFalse);
		logger.Write(txt);
		
		#ifdef ENABLE_LBS_TEST_LOGGER_RDEBUG
		txt.Append(_L("\n\r"));
		RDebug::RawPrint(txt);
		#endif				
		}
		
	logger.Close();
	}

/** private function, create common log text
@param aPrior Log entry priority
@param aBuf The log prefix buffer

*/
void LbsTestLogger::CreateLogTxt(TLbsTestLogType aType, TLbsTestLogPriority aPrior, TInt* aStackPtr, TDes8& aBuf)
	{
	TTime currentTime;
	currentTime.UniversalTime();
	TBuf<32> cTimeStr;
	TRAPD(err, currentTime.FormatL(cTimeStr, KTimeFormat));
	if (err)
		{
		User::Panic(KLbsTestLogger, KErrNoMemory);
		}
	TBuf8<32> cTimeStr8;
	cTimeStr8.Copy(cTimeStr);
	
	TFileName dirName = RProcess().FileName();
	TInt pos = dirName.LocateReverse('\\') + 1;
	TPtr fileName = dirName.MidTPtr(pos);
	TUint64 procId = RProcess().Id().Id();
	
	aBuf.Append(cTimeStr8);
	aBuf.Append(KLbsTestLogSep);

	aBuf.Append(fileName);
	aBuf.Append(KLbsTestLogSep);
	
	aBuf.AppendFormat(_L8("%LX"),procId);
	aBuf.Append(KLbsTestLogSep);
	
	aBuf.AppendFormat(_L8("P%d"),aPrior);
	aBuf.Append(KLbsTestLogSep);
	
	TUint32 stackUsage = reinterpret_cast<TUint32>(Dll::Tls());
	stackUsage = stackUsage - reinterpret_cast<TUint32>(aStackPtr);
	aBuf.AppendFormat(_L8("%08X(%05d)"), aStackPtr, stackUsage);

	aBuf.Append(KLbsTestLogSep);
	
	if (aType == ELogError)
		{
		aBuf.Append(KLbsTestLogError);
		}
	else if (aType == ELogWarning)
		{
		aBuf.Append(KLbsTestLogWarning);
		}
	}
