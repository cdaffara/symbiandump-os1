// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
 
*/

#include <e32base.h>
#include <e32debug.h>
#include <flogger.h>

// Component
#include "supldevlogger.h"


const TInt KSuplDevLogMaxBufSize = 300;
_LIT(KTimeFormat, "%H:%T:%S.%C");
_LIT(KSuplDevLogFolder, "lbs");
_LIT(KSuplDevLogFile,"lbs.txt");
_LIT(KSuplDevLogError, "Error: ");
_LIT(KSuplDevLogWarning, "Warning: ");
_LIT(KSuplDevLogSep," | ");

const TInt KSuplHexDumpWidth=16;
_LIT(KSuplFirstFormatString,"%04x : ");
_LIT(KSuplSecondFormatString,"%02x ");
_LIT(KSuplThirdFormatString,"%c");
_LIT(KSuplThreeSpaces,"   ");
_LIT(KSuplTwoSpaces,"  ");
const TText KSuplFullStopChar='.';

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

//#define ENABLE_SUPL_DEV_LOGGER_RDEBUG
//-----------------------------------------------------------------------------
// SuplDevLogger
//-----------------------------------------------------------------------------

/** Static function, one parameter, overwrite the previous log
@param aFmt TDes string reference 
*/
EXPORT_C void SuplDevLogger::OverWrite(const TDesC16& aFmt)
	{
	TInt stackInt = 0;
	
	// Grab some meory off the heap, give up if no memory available.
	RFileLogger *logger = new RFileLogger;
	if (logger == NULL)
		return;

	RBuf16 txt;
	TInt err = txt.Create(KSuplDevLogMaxBufSize);
	if (err != KErrNone)
		{
		delete logger;
		return;
		}

	err = logger->Connect();
	if (err == KErrNone)		
		{
		CreateLogTxt(ELogNormal, ELogP5, &stackInt, txt);
		txt.Append(aFmt.Left(KSuplDevLogMaxBufSize - txt.Length()));
		
		logger->CreateLog(KSuplDevLogFolder, KSuplDevLogFile, EFileLoggingModeOverwrite);
		logger->SetDateAndTime(EFalse, EFalse);
		logger->Write(txt);
		
		#ifdef ENABLE_SUPL_DEV_LOGGER_RDEBUG
		txt.Append(_L("\n\r"));
		RDebug::RawPrint(txt);
		#endif		
		}
	logger->Close();

	// Free up heap space
	txt.Close();
	delete logger;
	}

/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt TDes string reference 
*/
EXPORT_C void SuplDevLogger::Write(TSuplLogType aType, TSuplLogPriority aPrior, TRefByValue<const TDesC16> aFmt, ...)
	{
	TInt stackInt = 0;
	
	// Grab space on the heap for varibles we are going to need here.
	RFileLogger *logger = new RFileLogger;
	if (logger == NULL)
		{
		return;
		}

	TInt err = logger->Connect();	
	if (err == KErrNone)
		{
		VA_LIST list;
		VA_START(list, aFmt);
		RBuf16 strList;
		err = strList.Create(KSuplDevLogMaxBufSize);

		if (err == KErrNone)
			{
			strList.FormatList(aFmt, list);
			VA_END(list);

			RBuf16 txt;
			err = txt.Create(KSuplDevLogMaxBufSize);
			if (err == KErrNone)
				{
				CreateLogTxt(aType, aPrior, &stackInt, txt);
				txt.Append(strList.Left(KSuplDevLogMaxBufSize - txt.Length()));

				logger->CreateLog(KSuplDevLogFolder, KSuplDevLogFile, EFileLoggingModeAppend);
				logger->SetDateAndTime(EFalse, EFalse);
				logger->Write(txt);
				
				#ifdef ENABLE_SUPL_DEV_LOGGER_RDEBUG
				txt.Append(_L("\n\r"));
				RDebug::RawPrint(txt);
				#endif

				txt.Close();
				}
			strList.Close();
			}
		}
		
	logger->Close();
	delete logger;
	}

/** Static function to dump the hex data
@param aPrior Log entry priority
@param aPtr TUnit8 pointer to hex data
@param aLen length of hex data
*/
EXPORT_C void SuplDevLogger::HexDump(TSuplLogPriority aPrior, const TUint8 *aPtr, TInt aLen)
	{
	TInt stackInt = 0;
	
	if (aPtr==NULL)     // nothing to do
		return;

	RFileLogger* logger = new RFileLogger;
	if (logger == NULL)
		return;

	TInt err = logger->Connect();
	logger->CreateLog(KSuplDevLogFolder, KSuplDevLogFile, EFileLoggingModeAppend);
	logger->SetDateAndTime(EFalse, EFalse);
	
	RBuf buf;
	err = buf.Create(KSuplDevLogMaxBufSize);
	if (err == KErrNone)
		{
		RBuf8 temp;
		err= temp.Create(KSuplDevLogMaxBufSize);		
		if (err == KErrNone)
			{
			RBuf8 prefix;
			if (prefix.Create(KSuplDevLogMaxBufSize) == KErrNone)
				{
				CreateLogTxt(ELogNormal, aPrior, &stackInt, prefix);
				
				TInt i=0;
				while (aLen>0)
					{
					TInt n=(aLen>KSuplHexDumpWidth ? KSuplHexDumpWidth : aLen);
					buf.Copy(prefix);
					buf.AppendFormat(KSuplFirstFormatString,i);
					
					TInt j;
					for (j=0; j<n; j++)
						buf.AppendFormat(KSuplSecondFormatString,aPtr[i+j]);
						
					while (j++<KSuplHexDumpWidth)
						buf.Append(KSuplThreeSpaces);
						
					buf.Append(KSuplTwoSpaces);
					for (j=0; j<n; j++)
						buf.AppendFormat(KSuplThirdFormatString,(aPtr[i+j]<32 || aPtr[i+j]>126) ? KSuplFullStopChar : aPtr[i+j]);

					logger->Write(buf);
					
					#ifdef ENABLE_SUPL_DEV_LOGGER_RDEBUG
					buf.Append(_L("\n\r"));
					RDebug::RawPrint(buf);
					#endif				
					
					buf.SetLength(0);
					temp.SetLength(0);
					aLen-=n;
					i+=n;
					}
				prefix.Close();
				}
			temp.Close();
			}
		buf.Close();
		}  //end if buf created ok	

	logger->Close();
	delete logger;
	}   
	
/** private function, create common log text
@param aPrior Log entry priority
@param aBuf The log prefix buffer
@internalTechnology
*/
void SuplDevLogger::CreateLogTxt(TSuplLogType aType, TSuplLogPriority aPrior, TInt* aStackPtr, TDes16& aBuf)
	{
	// Grab space on the heap for all the varibles we are going to need here.
	TCreateLog16* vars = new TCreateLog16;
	if (vars == NULL)
		return;

	vars->currentTime.UniversalTime();
	TRAPD(err, vars->currentTime.FormatL(vars->cTimeStr, KTimeFormat));
	if (err)
		{
		User::Panic(KSuplDevLogger, KErrNoMemory);
		}
	
	vars->dirName = RProcess().FileName();	
	vars->pos = vars->dirName.LocateReverse('\\') + 1;
	TPtr16 fileName = vars->dirName.MidTPtr(vars->pos);
	vars->procId = RProcess().Id();
	
	aBuf.Append(vars->cTimeStr);
	aBuf.Append(KSuplDevLogSep);

	aBuf.Append(fileName);
	aBuf.Append(KSuplDevLogSep);
	
	aBuf.AppendFormat(_L16("%LX"),vars->procId);
	aBuf.Append(KSuplDevLogSep);
	
	aBuf.AppendFormat(_L16("P%d"),aPrior);
	aBuf.Append(KSuplDevLogSep);

	aBuf.AppendFormat(_L16("%08X"), aStackPtr);
	aBuf.Append(KSuplDevLogSep);

	if (aType == ELogError)
		{
		aBuf.Append(KSuplDevLogError);
		}
	else if (aType == ELogWarning)
		{
		aBuf.Append(KSuplDevLogWarning);
		}

	delete vars;
	}

/** Static function, one parameter, overwrite the previous log
@param aFmt TDes string reference 
*/
EXPORT_C void SuplDevLogger::OverWrite(const TDesC8& aFmt)
	{
	TInt stackInt = 0;
	
	RFileLogger* logger = new RFileLogger;
	if (logger == NULL)
		return;

	TInt err = logger->Connect();
	if (err == KErrNone)
		{
		RBuf8 txt;
		err = txt.Create(KSuplDevLogMaxBufSize);

		if (err == KErrNone)
			{
			CreateLogTxt(ELogNormal, ELogP5, &stackInt, txt);
			txt.Append(aFmt.Left(KSuplDevLogMaxBufSize - txt.Length()));

			logger->CreateLog(KSuplDevLogFolder, KSuplDevLogFile, EFileLoggingModeOverwrite);
			logger->SetDateAndTime(EFalse, EFalse);
			logger->Write(txt);
			
			#ifdef ENABLE_SUPL_DEV_LOGGER_RDEBUG
			txt.Append(_L("\n\r"));
			RDebug::RawPrint(txt);
			#endif

			txt.Close();
			}
		} // end if logger connect ok
		
	logger->Close();
	delete logger;
	}

/** Static function, one parameter
@param aPrior Log entry priority
@param aFmt Log entry
*/
EXPORT_C void SuplDevLogger::Write(TSuplLogType aType, TSuplLogPriority aPrior, TRefByValue<const TDesC8> aFmt, ...)
	{
	TInt stackInt = 0;
	
	RFileLogger* logger = new RFileLogger;
	if (logger == NULL)
		return;

	TInt err = logger->Connect();
	if (err == KErrNone)
		{
		VA_LIST list;
		VA_START(list, aFmt);

		RBuf8 strList;
		err = strList.Create(KSuplDevLogMaxBufSize);
		if (err == KErrNone)
			{
			strList.FormatList(aFmt, list);
			VA_END(list);

			RBuf8 txt;
			err = txt.Create(KSuplDevLogMaxBufSize);
			
			if (err == KErrNone)
				{
				CreateLogTxt(aType, aPrior, &stackInt, txt);
				txt.Append(strList.Left(KSuplDevLogMaxBufSize - txt.Length()));

				logger->CreateLog(KSuplDevLogFolder, KSuplDevLogFile, EFileLoggingModeAppend);
				logger->SetDateAndTime(EFalse, EFalse);
				logger->Write(txt);
				
				#ifdef ENABLE_SUPL_DEV_LOGGER_RDEBUG
				txt.Append(_L("\n\r"));
				RDebug::RawPrint(txt);
				#endif

				txt.Close();
				}

			strList.Close();
			} // end if strList memory alloc'd ok
			
		} // end if logger connected ok

	logger->Close();
	delete logger;
	}

/** private function, create common log text
@param aPrior Log entry priority
@param aBuf The log prefix buffer
@internalTechnology
*/
void SuplDevLogger::CreateLogTxt(TSuplLogType aType, TSuplLogPriority aPrior, TInt* aStackPtr, TDes8& aBuf)
	{
	// Grab space on the heap for all the varibles we are going to need here.
	TCreateLog8* vars = new TCreateLog8;
	if (vars == NULL)
		{
		return;
		}

	vars->currentTime.UniversalTime();	
	TRAPD(err, vars->currentTime.FormatL(vars->cTimeStr, KTimeFormat));
	if (err)
		{
		User::Panic(KSuplDevLogger, KErrNoMemory);
		}
	vars->cTimeStr8.Copy(vars->cTimeStr);
	
	vars->dirName = RProcess().FileName();
	vars->pos = vars->dirName.LocateReverse('\\') + 1;
	TPtr fileName = vars->dirName.MidTPtr(vars->pos);
	vars->procId = RProcess().Id();
	
	aBuf.Append(vars->cTimeStr8);
	aBuf.Append(KSuplDevLogSep);

	aBuf.Append(fileName);
	aBuf.Append(KSuplDevLogSep);
	
	aBuf.AppendFormat(_L8("%LX"),vars->procId);
	aBuf.Append(KSuplDevLogSep);
	
	aBuf.AppendFormat(_L8("P%d"),aPrior);
	aBuf.Append(KSuplDevLogSep);
	
	aBuf.AppendFormat(_L8("%08X"), aStackPtr);
	aBuf.Append(KSuplDevLogSep);
	
	if (aType == ELogError)
		{
		aBuf.Append(KSuplDevLogError);
		}
	else if (aType == ELogWarning)
		{
		aBuf.Append(KSuplDevLogWarning);
		}

	delete vars;
	}

/**
 This function uses the message to get the process and thread information in
 order to get the process name for the log file
 @param aMessage Client\server message about to be completed
 @param aName Contains the filename of the process to which this message belongs
**/
EXPORT_C void SuplDevLogger::GetMessageProcessName(const RMessage2& aMessage, TFileName& aName)
	{
	TMessageProcessName* vars = new TMessageProcessName;
	if (vars == 0)
		return;

	//Get the thread and process information
	if (aMessage.Client(vars->localThread) != KErrNone)
		return;

	vars->localThread.Process(vars->localProcess);

	aName = vars->localProcess.FileName();
	vars->pos = aName.LocateReverse('\\') + 1;
	aName = aName.Mid(vars->pos);

	delete vars;

	return;
	}
