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
//

/**
@file 
@internalTechnology
*/

#ifndef GUARD_SYMBIAN_CFPERFMETRICS_H
#define GUARD_SYMBIAN_CFPERFMETRICS_H

#ifdef SYMBIAN_ENABLE_NETWORKING_PERFMETRICS
 #define SYMBIAN_NETWORKING_PERFMETRICS
#endif // SYMBIAN_NETWORKING_PERFMETRICS

#if defined SYMBIAN_NETWORKING_PERFMETRICS && !defined _DEBUG && !defined SYMBIAN_NETWORKING_PERFMETRICS_SUPPRESS_WARNING
 #pragma message ("WARNING: Use of performance logging in a non-debug build")
#endif

#include <e32base.h>

class RFileLogger;


namespace CommsFW 
//CommsFW namespace doxygen is supplied in cfmsgs.h. The namespace is @publishedPartner, @released
{
/** performance logging - data stored, but not serialised/dumped until after a specified time
@class CPerfMetricStore 
@internalTechnology
*/
NONSHARABLE_CLASS(CPerfMetricStore) : private CPeriodic
	{
public:
	enum 
		{ 
		KMaxClients = 32, 
		KPriority = EPriorityHigh + 1,
		KPeriod = 30 * 1000 * 1000,
		};
	typedef TBool (*TFnWriteLog)(TAny* aClientId, TDes8& aBuffer, TDes8Overflow* aOverflowHandler);

	IMPORT_C static CPerfMetricStore* NewL(TInt aPriority, TTimeIntervalMicroSeconds32 aInterval);
	IMPORT_C static void Destroy();
	~CPerfMetricStore();

	IMPORT_C static void AddClient(TAny* aClientId, TFnWriteLog aCallBack);
	IMPORT_C static void RemoveClient(TAny* aClientId);

private:
	CPerfMetricStore(TInt aPriority, TTimeIntervalMicroSeconds32 aPeriod);
	static TInt Pulse(TAny* aSelf);
	TInt DoPulse();
	static CPerfMetricStore* Context();
	void DoAddClient(TAny* aClientId, CPerfMetricStore::TFnWriteLog aCallBack);
	void DoRemoveClient(TAny* aClientId);
private:
	struct TLogClient
		{
		TAny* iId;
		TFnWriteLog iCallBack;
		};
	TLogClient iClients[KMaxClients];
	TInt iNumClients;
	TTimeIntervalMicroSeconds32 iPeriod;
	RFileLogger* iLogger;
	}; // CPerfMetricStore
} // CommsFW

#endif // GUARD_SYMBIAN_CFPERFMETRICS_H

