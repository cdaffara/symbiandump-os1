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

#ifndef SYMBIAN_ENABLE_NETWORKING_PERFMETRICS
 #define SYMBIAN_ENABLE_NETWORKING_PERFMETRICS              // ensure a decleration exists (for real or stubbed definitions)
#endif

#define SYMBIAN_NETWORKING_PERFMETRICS_SUPPRESS_WARNING     // suppress warning
#include <comms-infras/cfperfmetrics.h>
#include <comms-infras/commsdebugutility.h>

_LIT(KPerfMetricPanic, "NetPerfMetric");
enum TPerfMetricPanics 
	{ 
	EPerfStoreAlreadyCreated,
	ETooManyClients,
    EStubbedImplementationInvoked,
	};

#if defined(SYMBIAN_NETWORKING_PERFMETRICS)
#if defined(__FLOG_ACTIVE)
using namespace CommsFW;

_LIT8(KMajorPerfTag, "Networking");
_LIT8(KMinorPerfTag, "Perf");

CPerfMetricStore::CPerfMetricStore(TInt aPriority, TTimeIntervalMicroSeconds32 aPeriod)
: CPeriodic(aPriority),
  iPeriod(aPeriod)
	{
	}

EXPORT_C CPerfMetricStore* CPerfMetricStore::NewL(TInt aPriority, TTimeIntervalMicroSeconds32 aInterval)
	{
	__ASSERT_ALWAYS(Context() == NULL, User::Panic(KPerfMetricPanic, EPerfStoreAlreadyCreated));
	CPerfMetricStore* self = new(ELeave) CPerfMetricStore(aPriority, aInterval);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	Dll::SetTls(self);
	return self;
	}

CPerfMetricStore::~CPerfMetricStore()
	{
	if(iLogger)
		{
		iLogger->Close();
		delete iLogger;
		}
	Dll::SetTls(NULL);
	}
	
EXPORT_C void CPerfMetricStore::Destroy()
	{
	CPerfMetricStore* self = Context();
	if(self)
		{
		delete self;
		}
	}

CPerfMetricStore* CPerfMetricStore::Context()
	{
	return static_cast<CPerfMetricStore*>(Dll::Tls());
	}

EXPORT_C void CPerfMetricStore::AddClient(TAny* aClientId, CPerfMetricStore::TFnWriteLog aCallBack)
	{
	CPerfMetricStore* self = Context();
	if(!self)
		{
		TRAP_IGNORE(self = CPerfMetricStore::NewL(CPerfMetricStore::KPriority, CPerfMetricStore::KPeriod));
		}
	if(self)
		{
		self->DoAddClient(aClientId, aCallBack);
		}
	}

void CPerfMetricStore::DoAddClient(TAny* aClientId, CPerfMetricStore::TFnWriteLog aCallBack)
	{
	if(!IsAdded())
		{
		CActiveScheduler::Add(this);
		Start(iPeriod, iPeriod, TCallBack(Pulse, this));
		}
	if(iNumClients < KMaxClients)
		{
		TLogClient& client = iClients[iNumClients++];
		client.iId = aClientId;
		client.iCallBack = aCallBack;
		}
	}

EXPORT_C void CPerfMetricStore::RemoveClient(TAny* aClientId)
	{
	CPerfMetricStore* self = Context();
	if(self)
		{
		self->DoRemoveClient(aClientId);

        if(self->iNumClients == 0)
		    {
		    delete self;
		    }
		}
	}

void CPerfMetricStore::DoRemoveClient(TAny* aClientId)
	{
	TInt i = 0;
	while(i < iNumClients)
		{
		if(iClients[i].iId == aClientId)
			{
			break;
			}
		++i;
		}
	if(i < iNumClients)
		{
		--iNumClients;
		if(i < iNumClients)
			{
			Mem::Move(&iClients[i], &iClients[i + 1], (iNumClients - i) * sizeof(TLogClient));
			}
		}
	}

TInt CPerfMetricStore::Pulse(TAny* aSelf)
	{
	return static_cast<CPerfMetricStore*>(aSelf)->DoPulse();
	}

TInt CPerfMetricStore::DoPulse()
	{
	if(!iLogger)
		{
		iLogger = new RFileLogger;
		if(!iLogger)
			{
			return 0;
			}
		}
	RFileLogger& logger = *iLogger;
	if(logger.Handle() == 0)
		{
		if(logger.Connect() == KErrNone)
			{
			logger.SetLogTags(KMajorPerfTag, KMinorPerfTag);
			}
		}
	if(logger.Handle() != 0)
		{
		TBuf8<KLogBufferSize> buf;
		class TLogOverflow : public TDes8Overflow
			{
		public:
			void Overflow(TDes8& /*aDes*/) 
				{ }
			};

		TLogOverflow overflow;
		for(int i = 0; i < iNumClients; ++i)
			{
			TLogClient& client = iClients[i];
			TBool moreToWrite;
			do
				{
				buf.SetLength(0);
				moreToWrite = client.iCallBack(client.iId, buf, &overflow);
				logger.Write(buf);
				} while(moreToWrite);
			}
		}
	return 0;
	}

#else  // __FLOG_ACTIVE
 // built with PerfMetrics enabled but logging disabled; doesn't work
 //#pragma message("SYMBIAN_NETWORKING_PERFMETRICS defined, but __FLOG_ACTIVE not defined, thus perf metrics disabled")
#endif // __FLOG_ACTIVE
#endif // SYMBIAN_NETWORKING_PERFMETRICS

#if !defined(SYMBIAN_NETWORKING_PERFMETRICS) || !defined(__FLOG_ACTIVE)
// stubbed defs to keep .def import library from complaining about missing exports
using namespace CommsFW;

EXPORT_C CPerfMetricStore* CPerfMetricStore::NewL(TInt /*aPriority*/, TTimeIntervalMicroSeconds32 /*aInterval*/) 
    {
    User::Panic(KPerfMetricPanic, EStubbedImplementationInvoked);
    return NULL;    // to keep compiler quiet
    };
EXPORT_C void CPerfMetricStore::Destroy() {};
EXPORT_C void CPerfMetricStore::AddClient(TAny* /*aClientId*/, TFnWriteLog /*aCallBack*/) {};
EXPORT_C void CPerfMetricStore::RemoveClient(TAny* /*aClientId*/) {};

#endif // SYMBIAN_NETWORKING_PERFMETRICS


