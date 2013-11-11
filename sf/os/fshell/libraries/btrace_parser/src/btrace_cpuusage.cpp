// btrace_cpuusage.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "btrace_parser.h"

MBtraceCpuUsageObserver::TCpuUsage::TCpuUsage(const TBtraceThreadId& aId)
	: iId(aId), iNumFastTicks(0)
	{
	}

MBtraceCpuUsageObserver::TCpuUsage::TCpuUsage(const TBtraceThreadId& aId, const TBtraceTickCount& aTickCount)
	: iId(aId), iNumFastTicks(0), iSwitchedInAt(aTickCount)
	{
	}

EXPORT_C CBtraceCpuUsage* CBtraceCpuUsage::NewL(CBtraceReader& aReader, CBtraceContext& aContext)
	{
	CBtraceCpuUsage* self = new(ELeave) CBtraceCpuUsage(aReader, aContext);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBtraceCpuUsage::~CBtraceCpuUsage()
	{
	while (iCpuUsageNotifs.Count())
		{
		iCpuUsageNotifs[0].Close();
		iCpuUsageNotifs.Remove(0);
		}
	iCpuUsageNotifs.Close();
	iContextSwitchNotifs.Close();
	iThreadIdleNotifs.Close();
	iReader.RemoveObserver(BTrace::ECpuUsage, *this);
	iReader.RemoveObserver(KAmTraceCategory, *this);
	}

EXPORT_C void CBtraceCpuUsage::NotifyCpuUsageL(TUint32 aNanoTickCount, TTimeIntervalMicroSeconds32 aPeriod, MBtraceCpuUsageObserver& aObserver)
	{
	NotifyCpuUsageL(aNanoTickCount, aPeriod, aObserver, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceCpuUsage::NotifyCpuUsageL(TUint32 aNanoTickCount, TTimeIntervalMicroSeconds32 aPeriod, MBtraceCpuUsageObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	RCpuUsageNotif notif(aNanoTickCount, TBtraceUtils::MicroSecondsToNanoTicks(aPeriod), aObserver, aPersistence);
	User::LeaveIfError(iCpuUsageNotifs.Append(notif));
	}
	
EXPORT_C void CBtraceCpuUsage::NotifyContextSwitchL(const TBtraceThreadId& aId, MBtraceCpuUsageObserver& aObserver)
	{
	TContextSwitchNotif notif(aId, aObserver);
	User::LeaveIfError(iContextSwitchNotifs.InsertInUnsignedKeyOrderAllowRepeats(notif));
	}

EXPORT_C void CBtraceCpuUsage::NotifyThreadIdleL(const TBtraceThreadId& aId, const TBtraceTickCount& aFromTickCount, TTimeIntervalMicroSeconds32 aPeriod, MBtraceCpuUsageObserver& aObserver)
	{
	NotifyThreadIdleL(aId, aFromTickCount, aPeriod, aObserver, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceCpuUsage::NotifyThreadIdleL(const TBtraceThreadId& aId, const TBtraceTickCount& aFromTickCount, TTimeIntervalMicroSeconds32 aPeriod, MBtraceCpuUsageObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	{
	TThreadIdleNotif notif(aId, aFromTickCount, TBtraceUtils::MicroSecondsToNanoTicks(aPeriod), aObserver, aPersistence);
	User::LeaveIfError(iThreadIdleNotifs.InsertInUnsignedKeyOrderAllowRepeats(notif));
	}
	
EXPORT_C void CBtraceCpuUsage::CancelNotifyThreadIdle(MBtraceCpuUsageObserver& aObserver)
	{
	for (TInt i = iThreadIdleNotifs.Count()-1; i>=0; --i)
		{
		if (iThreadIdleNotifs[i].iObserver == &aObserver)
			{
			iThreadIdleNotifs.Remove(i);
			}
		}
	}

CBtraceCpuUsage::CBtraceCpuUsage(CBtraceReader& aReader, CBtraceContext& aContext)
	: iReader(aReader), iContext(aContext)
	{
	}

void CBtraceCpuUsage::ConstructL()
	{
	iReader.AddObserverL(BTrace::ECpuUsage, *this);
	iReader.AddObserverL(KAmTraceCategory, *this); // To ensure we get heartbeat and sync frames, which are needed to flush through and force TestThreadIdlenessL to be called
	}

void CBtraceCpuUsage::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if (aFrame.iCategory == BTrace::ECpuUsage)
		{
		switch (aFrame.iSubCategory)
			{
			case BTrace::ENewThreadContext:
				{
				++iNumProcessedFrames;
				const TBtraceThreadId* btraceThreadId = iContext.FindThread(aFrame.iThreadContext);
//				__ASSERT_ALWAYS(btraceThreadId, Panic(EBtpPanicUnknownCpuContext));
				if (btraceThreadId)
					{
					HandleContextSwitchL(aFrame.iTickCount, *btraceThreadId);
					}
				else
					{
					iReader.Log(_L("BTrace::ENewThreadContext: Unknown thread context 0x%08x\r\n"), aFrame.iThreadContext);
					}
				break;
				}
			default:
				{
				// Ignore anything we don't know about.
				++iNumIgnoredFrames;
				break;
				}
			}
		}

	TestCpuUsagePeriodL(aFrame.iTickCount);
	TestThreadIdlenessL(aFrame.iTickCount);
	}

CBtraceCpuUsage::RCpuUsageNotif::RCpuUsageNotif(TUint32 aStartTickCount, TUint32 aNumNanoTicks, MBtraceCpuUsageObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iStartTickCount(aStartTickCount), iNumNanoTicks(aNumNanoTicks), iObserver(&aObserver), iPersistence(aPersistence)
	{
	}

void CBtraceCpuUsage::RCpuUsageNotif::RCpuUsageNotif::Close()
	{
	iUsage.Close();
	}

CBtraceCpuUsage::TContextSwitchNotif::TContextSwitchNotif(const TBtraceThreadId& aId)
	: iId(aId), iObserver(NULL)
	{
	}

CBtraceCpuUsage::TContextSwitchNotif::TContextSwitchNotif(const TBtraceThreadId& aId, MBtraceCpuUsageObserver& aObserver)
	: iId(aId), iObserver(&aObserver)
	{
	}

CBtraceCpuUsage::TThreadIdleNotif::TThreadIdleNotif(const TBtraceThreadId& aId)
	: iId(aId), iNumNanoTicks(0), iObserver(NULL), iEverScheduled(EFalse), iPersistence(ENotificationOneShot)
	{
	}

CBtraceCpuUsage::TThreadIdleNotif::TThreadIdleNotif(const TBtraceThreadId& aId, const TBtraceTickCount& aFromTickCount, TUint aNumNanoTicks, MBtraceCpuUsageObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iId(aId), iNumNanoTicks(aNumNanoTicks), iObserver(&aObserver), iLastSwitchedOut(aFromTickCount), iEverScheduled(EFalse), iPersistence(aPersistence)
	{
	}

void CBtraceCpuUsage::HandleContextSwitchL(const TBtraceTickCount& aTickCount, const TBtraceThreadId& aNewBtraceThreadId)
	{
	TBool relevant(EFalse);

	if (iLastBtractThreadId.Value() != 0)
		{
		// Update CPU usage notifications for the thread that has been switched out.
		for (TInt i = (iCpuUsageNotifs.Count() - 1); i >= 0; --i)
			{
			RCpuUsageNotif& notif = iCpuUsageNotifs[i];
			TInt pos = notif.iUsage.FindInUnsignedKeyOrder(MBtraceCpuUsageObserver::TCpuUsage(iLastBtractThreadId));
			if (pos >= 0)
				{
				MBtraceCpuUsageObserver::TCpuUsage& usage = notif.iUsage[pos];
				usage.iNumFastTicks += aTickCount.IntervalInFastTicks(usage.iSwitchedInAt);
				relevant = ETrue;
				}
			}
		}
	
	// Update CPU usage notifications to store this tick count at which this thread was switched in.
	for (TInt i = (iCpuUsageNotifs.Count() - 1); i >= 0; --i)
		{
		relevant = ETrue;
		RCpuUsageNotif& notif = iCpuUsageNotifs[i];
		TInt pos = notif.iUsage.FindInUnsignedKeyOrder(aNewBtraceThreadId);
		if (pos == KErrNotFound)
			{
			MBtraceCpuUsageObserver::TCpuUsage cpuUsage(aNewBtraceThreadId, aTickCount);
			TInt err = notif.iUsage.InsertInUnsignedKeyOrder(cpuUsage);
			__ASSERT_ALWAYS(err == KErrNone, Panic(EBtpPanicFailedToInsertCpuUsageObject));
			}
		else
			{
			notif.iUsage[pos].iSwitchedInAt = aTickCount;
			}
		}

	// Inform context switch observers that are interested in switches from the previous thread.
	TInt pos = iContextSwitchNotifs.SpecificFindInUnsignedKeyOrder(TContextSwitchNotif(iLastBtractThreadId), EArrayFindMode_First);
	if (pos >= 0)
		{
		const TInt numNotifs = iContextSwitchNotifs.Count();
		for (TInt i = 0; i < numNotifs; ++i)
			{
			const TContextSwitchNotif& thisNotif = iContextSwitchNotifs[i];
			if (thisNotif.iId == iLastBtractThreadId)
				{
				relevant = ETrue;
				thisNotif.iObserver->HandleContextSwitchL(aTickCount, iLastBtractThreadId, MBtraceCpuUsageObserver::EFromThisThread);
				}
			else
				{
				break;
				}
			}
		}

	// Inform context switch observers that are interested in switches to this new thread.
	pos = iContextSwitchNotifs.SpecificFindInUnsignedKeyOrder(TContextSwitchNotif(aNewBtraceThreadId), EArrayFindMode_First);
	if (pos >= 0)
		{
		const TInt numNotifs = iContextSwitchNotifs.Count();
		for (TInt i = pos; i < numNotifs; ++i)
			{
			const TContextSwitchNotif& thisNotif = iContextSwitchNotifs[i];
			if (thisNotif.iId == aNewBtraceThreadId)
				{
				relevant = ETrue;
				thisNotif.iObserver->HandleContextSwitchL(aTickCount, aNewBtraceThreadId, MBtraceCpuUsageObserver::EToThisThread);
				}
			else
				{
				break;
				}
			}
		}

	// Update thread idle notifications' last switched out time.
	pos = iThreadIdleNotifs.SpecificFindInUnsignedKeyOrder(TThreadIdleNotif(iLastBtractThreadId), EArrayFindMode_First);
	if (pos >= 0)
		{
		const TInt numNotifs = iThreadIdleNotifs.Count();
		for (TInt i = pos; i < numNotifs; ++i)
			{
			TThreadIdleNotif& thisNotif = iThreadIdleNotifs[i];
			if (thisNotif.iId == iLastBtractThreadId)
				{
				relevant = ETrue;
				thisNotif.iLastSwitchedOut = aTickCount;
				}
			else
				{
				break;
				}
			}
		}

	// Update thread idle notifications' ever scheduled.
	pos = iThreadIdleNotifs.SpecificFindInUnsignedKeyOrder(TThreadIdleNotif(aNewBtraceThreadId), EArrayFindMode_First);
	if (pos >= 0)
		{
		const TInt numNotifs = iThreadIdleNotifs.Count();
		for (TInt i = pos; i < numNotifs; ++i)
			{
			TThreadIdleNotif& thisNotif = iThreadIdleNotifs[i];
			if (thisNotif.iId == aNewBtraceThreadId)
				{
				relevant = ETrue;
				thisNotif.iEverScheduled = ETrue;
				}
			else
				{
				break;
				}
			}
		}

	iLastBtractThreadId = aNewBtraceThreadId;

	if (relevant)
		{
		++iNumRelevantFrames;
		}
	}

TInt CompareUsage(const MBtraceCpuUsageObserver::TCpuUsage& aFirst,const MBtraceCpuUsageObserver::TCpuUsage& aSecond)
	{
	if (aFirst.iNumFastTicks > aSecond.iNumFastTicks)
		{
		return -1;
		}
	else if (aFirst.iNumFastTicks < aSecond.iNumFastTicks)
		{
		return 1;
		}
	else
		{
		return 0;
		}
	}

void CBtraceCpuUsage::TestCpuUsagePeriodL(const TBtraceTickCount& aTickCount)
	{
	for (TInt i = (iCpuUsageNotifs.Count() - 1); i >= 0; --i)
		{
		RCpuUsageNotif& thisNotif = iCpuUsageNotifs[i];
		if ((aTickCount.iNano - thisNotif.iStartTickCount) >= thisNotif.iNumNanoTicks)
			{
			thisNotif.iUsage.Sort(TLinearOrder<MBtraceCpuUsageObserver::TCpuUsage>(CompareUsage));
			TArray<MBtraceCpuUsageObserver::TCpuUsage> array(thisNotif.iUsage.Array());
			thisNotif.iObserver->HandleCpuUsageL(aTickCount, array);
			if (thisNotif.iPersistence == ENotificationOneShot)
				{
				thisNotif.Close();
				iCpuUsageNotifs.Remove(i);
				}
			}
		}
	}

void CBtraceCpuUsage::TestThreadIdlenessL(const TBtraceTickCount& aTickCount)
	{
	for (TInt i = (iThreadIdleNotifs.Count() - 1); i >= 0; --i)
		{
		const TThreadIdleNotif& thisNotif = iThreadIdleNotifs[i];
		//iReader.Log(_L("CBtraceCpuUsage::TestThreadIdlenessL: now %d, lastswitch %d, remaining %d\r\n"), aTickCount.iNano, thisNotif.iLastSwitchedOut.iNano, thisNotif.iNumNanoTicks - (aTickCount.iNano - thisNotif.iLastSwitchedOut.iNano));
		if ((aTickCount.iNano > thisNotif.iLastSwitchedOut.iNano) && ((aTickCount.iNano - thisNotif.iLastSwitchedOut.iNano) >= thisNotif.iNumNanoTicks))
			{
			MBtraceCpuUsageObserver* observer = thisNotif.iObserver;
			TBtraceTickCount lastSwitchedOut = thisNotif.iLastSwitchedOut;
			TBtraceThreadId threadId = thisNotif.iId;
			MBtraceCpuUsageObserver::TIdleType idleType = thisNotif.iEverScheduled ? MBtraceCpuUsageObserver::EScheduledAtLeastOnce : MBtraceCpuUsageObserver::ENeverScheduled;

			if (thisNotif.iPersistence == ENotificationOneShot)
				{
				iThreadIdleNotifs.Remove(i);
				}

			observer->HandleThreadIdleL(lastSwitchedOut, threadId, idleType);
			}
		}
	}
