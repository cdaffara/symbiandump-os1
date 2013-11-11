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

/**
 @file
 @internalTechnology
*/

#ifndef MEMORY_USAGE_LOGGER_H
#define MEMORY_USAGE_LOGGER_H


#include <e32base.h>
#include <flogger.h>

/**
*/
NONSHARABLE_CLASS(CMemoryUsageLogger) : public CBase
	{
public:
	IMPORT_C static CMemoryUsageLogger* NewLC();
	~CMemoryUsageLogger();

	IMPORT_C void Cancel();
	
private:
	CMemoryUsageLogger();
	void ConstructL();
	
	void WriteToLog();
	void WriteMaxHeapToLog();
	
	static TInt PeriodicCallback(TAny* aSelf);
	
private:
	CPeriodic* iPeriodic;
	TInt iInterval;
	
	RProcess iProcess;
	RHeap& iHeap;
	TInt iStackSize;
	TInt iMaxHeapTotal;
	TInt iMaxChunkTotal;
	TTime iStartTime;
	RFileLogger iLogger;
	TFileName iFileName;
	};


#ifdef ENABLE_LBS_MEM_LOGGER

#define LBSMEMLOG_BEGIN()	{ CMemoryUsageLogger* memoryUsageLogger = CMemoryUsageLogger::NewLC(); }

#define LBSMEMLOG_END()		{ CleanupStack::PopAndDestroy(1); }

#else // ENABLE_LBS_MEM_LOGGER

#define LBSMEMLOG_BEGIN()

#define LBSMEMLOG_END()

#endif // ENABLE_LBS_MEM_LOGGER

#endif	//MEMORY_USAGE_LOGGER_H
