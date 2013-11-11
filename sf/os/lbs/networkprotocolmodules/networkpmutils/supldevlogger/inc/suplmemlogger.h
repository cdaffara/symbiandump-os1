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
//

/**
 @file
 @internalTechnology
 
*/

#ifndef SUPL_MEMORY_USAGE_LOGGER_H
#define SUPL_MEMORY_USAGE_LOGGER_H


#include <e32base.h>
#include <flogger.h>

/**
*/
NONSHARABLE_CLASS(CSuplMemoryUsageLogger) : public CBase
	{
public:
	IMPORT_C static CSuplMemoryUsageLogger* NewLC();
	~CSuplMemoryUsageLogger();

	IMPORT_C void Cancel();
	
private:
	CSuplMemoryUsageLogger();
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


#ifdef ENABLE_SUPL_MEM_LOGGER

#define SUPLMEMLOG_BEGIN()	{ CSuplMemoryUsageLogger* memoryUsageLogger = CSuplMemoryUsageLogger::NewLC(); }

#define SUPLMEMLOG_END()		{ CleanupStack::PopAndDestroy(1); }

#else // ENABLE_SUPL_MEM_LOGGER

#define SUPLMEMLOG_BEGIN()

#define SUPLMEMLOG_END()

#endif // ENABLE_SUPL_MEM_LOGGER

#endif	//SUPL_MEMORY_USAGE_LOGGER_H
