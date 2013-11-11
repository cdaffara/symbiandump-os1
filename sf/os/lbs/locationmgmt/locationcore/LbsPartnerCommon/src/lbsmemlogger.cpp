// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <flogger.h>
#include <hal.h>
#include "lbsmemlogger.h"


/** Time interval between the first and second updates. */
const TInt KTimerInitialDelay = 1000000;

/** Time interval between the second and third and all the other updates. */
const TInt KTimerInitialInterval = 2000000;

const TInt KLbsDevLogMaxBufSize = 256;

_LIT(KLbsDevLogFolder, "lbs");
_LIT(KHeader, "TIME\tUID\tSTACK SIZE\tHEAP TOTAL\tHEAP AVAIL\tCHUNK TOTAL\tCHUNK AVAIL\tCELLS TOTAL\tCELLS AVAIL\tH/S TOTAL\tRAM TOTAL\tRAM AVAIL");
_LIT(KLogFormat, "%d\t%x\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d");

EXPORT_C CMemoryUsageLogger* CMemoryUsageLogger::NewLC()
	{
	CMemoryUsageLogger* self = new (ELeave) CMemoryUsageLogger();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMemoryUsageLogger::CMemoryUsageLogger() :
	iHeap(User::Heap())
	{
	}

CMemoryUsageLogger::~CMemoryUsageLogger()
	{	
	// Cancel and delete periodic timer
	iPeriodic->Cancel();
	delete iPeriodic;
	
	// Close iLogger handle
	iLogger.Write(KNullDesC8);
	iLogger.Close();

	// Write the max params to a separate file
	WriteMaxHeapToLog();
	
	// Close RProcess handle
	iProcess.Close();
	}

void CMemoryUsageLogger::ConstructL()
	{	
	//create periodic timer object
	iPeriodic = CPeriodic::NewL(CActive::EPriorityHigh);
	
	//get process name
	TFileName processFilename(iProcess.FileName());
	
	//create log file name out of process name
	TInt pos = processFilename.LocateReverse(TChar('\\'));
	iFileName = processFilename.Mid(pos + 1);
	iFileName = iFileName.Mid(0, iFileName.Length() - 3);
	iFileName.Append(_L("txt"));
	
	//connect to file iLogger
	User::LeaveIfError(iLogger.Connect());
	
	//get total stack size
	RThread currentThread;
	TThreadStackInfo stackInfo;
	currentThread.StackInfo(stackInfo);
	iStackSize = stackInfo.iBase - stackInfo.iLimit;
	
	//create log file
	iLogger.CreateLog(KLbsDevLogFolder, iFileName, EFileLoggingModeAppend);
	
	//write header to log file
	iLogger.SetDateAndTime(ETrue, ETrue);
	iLogger.Write(KNullDesC8);
	iLogger.SetDateAndTime(EFalse, EFalse);
	iLogger.WriteFormat(KHeader);
	
	//setup time stamp for calculating time passed
	iStartTime.UniversalTime();
	
	//first write to log file
	WriteToLog();
	
	//start periodic timer
	iPeriodic->Start(KTimerInitialDelay, KTimerInitialInterval, TCallBack(PeriodicCallback, this));
	}

EXPORT_C void CMemoryUsageLogger::Cancel()
	{
	iPeriodic->Cancel();
	}

TInt CMemoryUsageLogger::PeriodicCallback(TAny* aSelf)
	{
	CMemoryUsageLogger* self = static_cast<CMemoryUsageLogger*>(aSelf);
	self->WriteToLog();
	return KErrNone;
	}

void CMemoryUsageLogger::WriteToLog()
	{
	//seconds passed since start of application
	TTime currentTime;
	TTimeIntervalSeconds seconds;
	currentTime.UniversalTime();
	currentTime.SecondsFrom(iStartTime, seconds);
	
	if (seconds.Int() <= 60)
		{
		TInt heapTotal = 0;
		TInt heapAvail = 0;
		TInt chunkTotal = 0;
		TInt chunkAvail = 0;
		TInt cellsTotal = 0;
		TInt cellsAvail = 0;
		TInt heapStackTotal = 0;
		TInt ramTotal = 0;
		TInt ramAvail = 0;
		
		//get system memory info from hardware abstraction layer
		HAL::Get(HAL::EMemoryRAM, ramTotal);
		HAL::Get(HAL::EMemoryRAMFree, ramAvail);
		
		//get process UID
		TSecureId processUid(iProcess.SecureId());
		
		//get various heap and chunk memory sizes
		iHeap.AllocSize(heapTotal);
		if (heapTotal > iMaxHeapTotal)
			{
			iMaxHeapTotal = heapTotal;
			}
		iHeap.Available(heapAvail);
		chunkTotal = iHeap.Size();
		chunkAvail = chunkTotal - heapTotal;
		if (chunkTotal > iMaxChunkTotal)
			{
			iMaxChunkTotal = chunkTotal;
			}
		
		//get cells info
		cellsTotal = iHeap.Count(cellsAvail);
		
		//sum up the total heap and stack sizes
		heapStackTotal = heapTotal + iStackSize;
		
		//create log text and write to log file
		TBuf16<KLbsDevLogMaxBufSize> logData;
		logData.Format(KLogFormat, seconds.Int(), processUid.iId, iStackSize, heapTotal, heapAvail, chunkTotal, chunkAvail, cellsTotal, cellsAvail, heapStackTotal, ramTotal, ramAvail);
		iLogger.Write(logData);
		}
	}

void CMemoryUsageLogger::WriteMaxHeapToLog()
	{
	_LIT(KLogMaxHeapFormat, "%d, %d");

	//connect to file iLogger
	TInt err = iLogger.Connect();
	if (KErrNone == err)
		{
		//create new log file name out of existing
		TInt pos = iFileName.LocateReverse(TChar('.'));
		if (KErrNotFound != pos)
			{
			iFileName.Insert(pos, _L("_max_ram"));

			//create log file
			iLogger.CreateLog(KLbsDevLogFolder, iFileName, EFileLoggingModeOverwrite);
			
			//write header to log file
			iLogger.SetDateAndTime(EFalse, EFalse);

			//create log text and write to log file
			TBuf16<KLbsDevLogMaxBufSize> logData;
			logData.Format(KLogMaxHeapFormat, iMaxHeapTotal, iMaxChunkTotal);
			iLogger.Write(logData);
			iLogger.Write(KNullDesC8);
			}
		iLogger.Close();
		}
	}

