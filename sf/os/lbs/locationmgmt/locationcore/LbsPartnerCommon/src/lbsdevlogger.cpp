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
#include "lbsdevlogger.h"


const TInt KLbsDevLogMaxBufSize = 300;
_LIT(KTimeFormat, "%H:%T:%S.%C");
_LIT(KLbsDevLogFolder, "lbs");
_LIT(KLbsDevLogFile,"lbs.txt");
_LIT(KLbsDevLogError, "Error: ");
_LIT(KLbsDevLogWarning, "Warning: ");
_LIT(KLbsDevLogSep," | ");

const TInt KLbsHexDumpWidth=16;
_LIT(KLbsFirstFormatString,"%04x : ");
_LIT(KLbsSecondFormatString,"%02x ");
_LIT(KLbsThirdFormatString,"%c");
_LIT(KLbsThreeSpaces,"   ");
_LIT(KLbsTwoSpaces,"  ");
const TText KLbsFullStopChar='.';

#ifdef ENABLE_LBS_DEV_LOGGER_RDEBUG
_LIT(KLbsNextLine,"\n\r");
#endif

// Declare structures which will hold variables on the heap.
typedef struct
	{
	TTime currentTime;
	TBuf16<32> cTimeStr;
	TFileName dirName;
	TInt16 pos;
	TUint64 procId;
	TUint32 stackUsage;
	} 
TCreateLog16;

typedef struct
	{
	TTime currentTime;
	TBuf8<32> cTimeStr8;
	TBuf16<32> cTimeStr;
	TFileName dirName;
	TInt pos;
	TUint64 procId;
	TUint32 stackUsage;
	} 
TCreateLog8;

typedef struct
	{
	RThread localThread;
	RProcess localProcess;	
	TFileName fName;
	TInt16 pos;
	}
TMessageProcessName;

//#define ENABLE_LBS_DEV_LOGGER_RDEBUG
//-----------------------------------------------------------------------------
// LbsDevLogger
//-----------------------------------------------------------------------------

/** Static function, one parameter, overwrite the previous log
@param aFmt TDes string reference 
*/
void LbsDevLogger::OverWriteL(const TDesC16& aFmt)
	{
	TInt stackInt = 0;
	
	// Grab some memory off the heap, give up if no memory available.
	RFileLogger logger;
	CleanupClosePushL(logger);
	
	RBuf16 txt;
	CleanupClosePushL(txt);
	
	User::LeaveIfError(txt.Create(KLbsDevLogMaxBufSize));

	User::LeaveIfError(logger.Connect());
	
	CreateLogTxt(ELogNormal, ELogP5, &stackInt, txt);
	txt.Append(aFmt.Left(KLbsDevLogMaxBufSize - txt.Length()));
	
	logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeOverwrite);
	logger.SetDateAndTime(EFalse, EFalse);
	logger.Write(txt);
	
	#ifdef ENABLE_LBS_DEV_LOGGER_RDEBUG
	txt.Append(KLbsNextLine);
	RDebug::RawPrint(txt);
	#endif		


	// Free up heap space
	CleanupStack::PopAndDestroy(&txt);
	CleanupStack::PopAndDestroy(&logger);
	}

/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt TDes string reference 
*/
void LbsDevLogger::WriteL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC16> aFmt, ...)
	{
	TInt stackInt;
	
	// Grab space on the heap for variables we are going to need here.
	RFileLogger logger;
	
	CleanupClosePushL(logger);

	User::LeaveIfError(logger.Connect());	
	
	VA_LIST list;
	VA_START(list, aFmt);
	RBuf16 strList;
	CleanupClosePushL(strList);
	User::LeaveIfError(strList.Create(KLbsDevLogMaxBufSize));

	strList.FormatList(aFmt, list);
	VA_END(list);

	RBuf16 txt;
	CleanupClosePushL(txt);
	User::LeaveIfError(txt.Create(KLbsDevLogMaxBufSize));
	
	CreateLogTxt(aType, aPrior, &stackInt, txt);
	txt.Append(strList.Left(KLbsDevLogMaxBufSize - txt.Length()));

	logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeAppend);
	logger.SetDateAndTime(EFalse, EFalse);
	logger.Write(txt);
	
	#ifdef ENABLE_LBS_DEV_LOGGER_RDEBUG
	txt.Append(KLbsNextLine);
	RDebug::RawPrint(txt);
	#endif

	CleanupStack::PopAndDestroy(&txt);
	CleanupStack::PopAndDestroy(&strList);
	CleanupStack::PopAndDestroy(&logger);
	}

/** Static function to dump the hex data
@param aPrior Log entry priority
@param aPtr TUnit8 pointer to hex data
@param aLen length of hex data
*/
void LbsDevLogger::HexDumpL(TLbsLogPriority aPrior, const TUint8 *aPtr, TInt aLen)
	{
	TInt stackInt = 0;
	
	if (!aPtr)     // nothing to do
		return;

	RFileLogger logger;
	CleanupClosePushL(logger);
	
	User::LeaveIfError(logger.Connect());
	logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeAppend);
	logger.SetDateAndTime(EFalse, EFalse);
	
	RBuf buf;
	CleanupClosePushL(buf);
	
	User::LeaveIfError(buf.Create(KLbsDevLogMaxBufSize));
	
	RBuf8 temp;
	CleanupClosePushL(temp);
	User::LeaveIfError(temp.Create(KLbsDevLogMaxBufSize));		
	
	RBuf8 prefix;
	CleanupClosePushL(prefix);
	User::LeaveIfError(prefix.Create(KLbsDevLogMaxBufSize));
	
	CreateLogTxt(ELogNormal, aPrior, &stackInt, prefix);
	
	TInt i=0;
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
			
		#ifdef ENABLE_LBS_DEV_LOGGER_RDEBUG
		buf.Append(KLbsNextLine);
		RDebug::RawPrint(buf);
		#endif				
			
		buf.SetLength(0);
		temp.SetLength(0);
		aLen-=n;
		i+=n;
		}

	CleanupStack::PopAndDestroy(&prefix);
	CleanupStack::PopAndDestroy(&temp);	
	CleanupStack::PopAndDestroy(&buf);
	CleanupStack::PopAndDestroy(&logger);
	}   
	
/** private function, create common log text
@param aPrior Log entry priority
@param aBuf The log prefix buffer


*/
void LbsDevLogger::CreateLogTxt(TLbsLogType aType, TLbsLogPriority aPrior, TInt* aStackPtr, TDes16& aBuf)
	{
	// Grab space on the heap for all the varibles we are going to need here.
	TCreateLog16 vars;

	vars.currentTime.UniversalTime();
	TRAPD(err, vars.currentTime.FormatL(vars.cTimeStr, KTimeFormat));
	if (err)
		{
		User::Panic(KLbsDevLogger, KErrNoMemory);
		}
	
	vars.dirName = RProcess().FileName();	
	vars.pos = vars.dirName.LocateReverse('\\') + 1;
	TPtr16 fileName = vars.dirName.MidTPtr(vars.pos);
	vars.procId = RProcess().Id();
	
	aBuf.Append(vars.cTimeStr);
	aBuf.Append(KLbsDevLogSep);

	aBuf.Append(fileName);
	aBuf.Append(KLbsDevLogSep);
	
	aBuf.AppendFormat(_L16("%LX"),vars.procId);
	aBuf.Append(KLbsDevLogSep);
	
	aBuf.AppendFormat(_L16("P%d"),aPrior);
	aBuf.Append(KLbsDevLogSep);

	aBuf.AppendFormat(_L16("%08X"), aStackPtr);
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

/** Static function, one parameter, overwrite the previous log
@param aFmt TDes string reference 
*/
void LbsDevLogger::OverWriteL(const TDesC8& aFmt)
	{
	TInt stackInt = 0;
	
	RFileLogger logger;
	CleanupClosePushL(logger);

	User::LeaveIfError(logger.Connect());
	
	RBuf8 txt;
	CleanupClosePushL(txt);
	User::LeaveIfError(txt.Create(KLbsDevLogMaxBufSize));

	CreateLogTxt(ELogNormal, ELogP5, &stackInt, txt);
	txt.Append(aFmt.Left(KLbsDevLogMaxBufSize - txt.Length()));

	logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeOverwrite);
	logger.SetDateAndTime(EFalse, EFalse);
	logger.Write(txt);
		
	#ifdef ENABLE_LBS_DEV_LOGGER_RDEBUG
	txt.Append(KLbsNextLine);
	RDebug::RawPrint(txt);
	#endif

	CleanupStack::PopAndDestroy(&txt);
	CleanupStack::PopAndDestroy(&logger);
	}

/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt Log entry
*/
void LbsDevLogger::WriteL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC8> aFmt, ...)
	{
	TInt stackInt;
	
	RFileLogger logger;
	CleanupClosePushL(logger);

	User::LeaveIfError(logger.Connect());

	VA_LIST list;
	VA_START(list, aFmt);

	RBuf8 strList;
	CleanupClosePushL(strList);
	User::LeaveIfError(strList.Create(KLbsDevLogMaxBufSize*10));
	
	strList.FormatList(aFmt, list);
	VA_END(list);

	RBuf8 txt;
	CleanupClosePushL(txt);
	User::LeaveIfError(txt.Create(KLbsDevLogMaxBufSize));
			
	
	CreateLogTxt(aType, aPrior, &stackInt, txt);
	txt.Append(strList.Left(KLbsDevLogMaxBufSize - txt.Length()));

	logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeAppend);
	logger.SetDateAndTime(EFalse, EFalse);
	logger.Write(txt);
				
	#ifdef ENABLE_LBS_DEV_LOGGER_RDEBUG
	txt.Append(KLbsNextLine);
	RDebug::RawPrint(txt);
	#endif

	CleanupStack::PopAndDestroy(&txt);
	CleanupStack::PopAndDestroy(&strList);
	CleanupStack::PopAndDestroy(&logger);
	}

/** private function, create common log text
@param aPrior Log entry priority
@param aBuf The log prefix buffer

*/
void LbsDevLogger::CreateLogTxt(TLbsLogType aType, TLbsLogPriority aPrior, TInt* aStackPtr, TDes8& aBuf)
	{
	// Grab space on the heap for all the varibles we are going to need here.
	TCreateLog8 vars;

	vars.currentTime.UniversalTime();	
	TRAPD(err, vars.currentTime.FormatL(vars.cTimeStr, KTimeFormat));
	if (err)
		{
		User::Panic(KLbsDevLogger, KErrNoMemory);
		}
	vars.cTimeStr8.Copy(vars.cTimeStr);
	
	vars.dirName = RProcess().FileName();
	vars.pos = vars.dirName.LocateReverse('\\') + 1;
	TPtr fileName = vars.dirName.MidTPtr(vars.pos);
	vars.procId = RProcess().Id();
	
	aBuf.Append(vars.cTimeStr8);
	aBuf.Append(KLbsDevLogSep);

	aBuf.Append(fileName);
	aBuf.Append(KLbsDevLogSep);
	
	aBuf.AppendFormat(_L8("%LX"),vars.procId);
	aBuf.Append(KLbsDevLogSep);
	
	aBuf.AppendFormat(_L8("P%d"),aPrior);
	aBuf.Append(KLbsDevLogSep);
	
	aBuf.AppendFormat(_L8("%08X"), aStackPtr);
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




/**
 This function uses the message to get the process and thread information in
 order to get the process name for the log file
 @param aMessage Client\server message about to be completed
 @param aName Contains the filename of the process to which this message belongs
**/
EXPORT_C void LbsDevLogger::GetMessageProcessName(const RMessage2& aMessage, TFileName& aName)
	{
	TMessageProcessName vars;

	//Get the thread and process information
	if (aMessage.Client(vars.localThread) != KErrNone)
		return;
		
	vars.localThread.Process(vars.localProcess);

	aName = vars.localProcess.FileName();
	vars.pos = aName.LocateReverse('\\') + 1;
	aName = aName.Mid(vars.pos);
	vars.localProcess.Close();
	vars.localThread.Close();
	}

EXPORT_C void LbsDevLogger::OverWrite(const TDesC8& aFmt)
	{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	TRAP_IGNORE(OverWriteL(aFmt));
	delete cleanupStack;
	}

EXPORT_C void LbsDevLogger::OverWrite(const TDesC16& aFmt)
	{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	TRAP_IGNORE(OverWriteL(aFmt));
	delete cleanupStack;
	}
		
EXPORT_C void LbsDevLogger::Write(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC16> aFmt, ...)
	{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	VA_LIST list;
	VA_START(list, aFmt);
	TRAP_IGNORE(WriteL(aType, aPrior, aFmt, list));
	VA_END(list);
	delete cleanupStack;
	}


void LbsDevLogger::WriteListL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC8> aFmt, VA_LIST list)
	{
	RBuf8 strList;
	strList.CleanupClosePushL();
	User::LeaveIfError(strList.Create(KLbsDevLogMaxBufSize*10));
	strList.FormatList(aFmt, list);
	WriteL(aType, aPrior, aFmt, list);
	CleanupStack::PopAndDestroy();
	}

EXPORT_C void LbsDevLogger::Write(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC8> aFmt, ...)
	{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();

	VA_LIST list;
	VA_START(list, aFmt);

	TRAP_IGNORE(WriteListL(aType, aPrior, aFmt, list));

	VA_END(list);
	delete cleanupStack;
	}

EXPORT_C void LbsDevLogger::HexDump(TLbsLogPriority aPrior, const TUint8 *aPtr, TInt aLen)
	{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	TRAP_IGNORE(HexDumpL(aPrior, aPtr, aLen););
	delete cleanupStack;
	}


/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt Log entry
*/
void LbsDevLogger::WriteL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC8> aFmt, VA_LIST list)
	{
	TInt stackInt;
	
	RFileLogger logger;
	CleanupClosePushL(logger);

	User::LeaveIfError(logger.Connect());
	//VA_START(list, aFmt);

	RBuf8 strList;
	CleanupClosePushL(strList);
	User::LeaveIfError(strList.Create(KLbsDevLogMaxBufSize*10));
	
	strList.FormatList(aFmt, list);
	//VA_END(list);

	RBuf8 txt;
	CleanupClosePushL(txt);
	User::LeaveIfError(txt.Create(KLbsDevLogMaxBufSize));
			
	
	CreateLogTxt(aType, aPrior, &stackInt, txt);
	txt.Append(strList.Left(KLbsDevLogMaxBufSize - txt.Length()));

	logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeAppend);
	logger.SetDateAndTime(EFalse, EFalse);
	logger.Write(txt);
				
	#ifdef ENABLE_LBS_DEV_LOGGER_RDEBUG
	txt.Append(KLbsNextLine);
	RDebug::RawPrint(txt);
	#endif

	CleanupStack::PopAndDestroy(&txt);
	CleanupStack::PopAndDestroy(&strList);
	CleanupStack::PopAndDestroy(&logger);
	}

/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt TDes string reference 
*/
void LbsDevLogger::WriteL(TLbsLogType aType, TLbsLogPriority aPrior, TRefByValue<const TDesC16> aFmt, VA_LIST list)
	{
	TInt stackInt;
	
	// Grab space on the heap for variables we are going to need here.
	RFileLogger logger;
	
	CleanupClosePushL(logger);

	User::LeaveIfError(logger.Connect());	
	
	//VA_START(list, aFmt);
	RBuf16 strList;
	CleanupClosePushL(strList);
	User::LeaveIfError(strList.Create(KLbsDevLogMaxBufSize));

	strList.FormatList(aFmt, list);
	//VA_END(list);

	RBuf16 txt;
	CleanupClosePushL(txt);
	User::LeaveIfError(txt.Create(KLbsDevLogMaxBufSize));
	
	CreateLogTxt(aType, aPrior, &stackInt, txt);
	txt.Append(strList.Left(KLbsDevLogMaxBufSize - txt.Length()));

	logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeAppend);
	logger.SetDateAndTime(EFalse, EFalse);
	logger.Write(txt);
	
	#ifdef ENABLE_LBS_DEV_LOGGER_RDEBUG
	txt.Append(KLbsNextLine);
	RDebug::RawPrint(txt);
	#endif

	CleanupStack::PopAndDestroy(&txt);
	CleanupStack::PopAndDestroy(&strList);
	CleanupStack::PopAndDestroy(&logger);
	}

