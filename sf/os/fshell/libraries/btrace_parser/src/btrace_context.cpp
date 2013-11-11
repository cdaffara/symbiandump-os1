// btrace_context.cpp
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

#include <e32debug.h>
#include "btrace_parser.h"


_LIT(KUnknown, "unknown");


EXPORT_C TBtraceIdBase::TBtraceIdBase()
	: iId(0)
	{
	}

EXPORT_C TBtraceIdBase::TBtraceIdBase(TUint aId)
	: iId(aId)
	{
	}

EXPORT_C TBtraceIdBase::TBtraceIdBase(const TBtraceIdBase& aId)
	: iId(aId.Value())
	{
	}

EXPORT_C void TBtraceIdBase::Set(TUint aValue)
	{
	iId = aValue;
	}

EXPORT_C TUint TBtraceIdBase::Value() const
	{
	return iId;
	}

EXPORT_C TBool TBtraceIdBase::operator==(const TBtraceIdBase& aId) const
	{
	return (aId.iId == iId);
	}

EXPORT_C TBtraceThreadId::TBtraceThreadId()
	{
	}

EXPORT_C TBtraceThreadId::TBtraceThreadId(TUint aId)
	: TBtraceIdBase(aId)
	{
	}

EXPORT_C TBtraceThreadId::TBtraceThreadId(const TBtraceThreadId& aId)
	: TBtraceIdBase(aId)
	{
	}

EXPORT_C TBtraceProcessId::TBtraceProcessId()
	{
	}

EXPORT_C TBtraceProcessId::TBtraceProcessId(TUint aId)
	: TBtraceIdBase(aId)
	{
	}

EXPORT_C TBtraceProcessId::TBtraceProcessId(const TBtraceProcessId& aId)
	: TBtraceIdBase(aId)
	{
	}

EXPORT_C TBtraceWindowGroupId::TBtraceWindowGroupId()
	{
	}

EXPORT_C TBtraceWindowGroupId::TBtraceWindowGroupId(TUint aId)
	: TBtraceIdBase(aId)
	{
	}

EXPORT_C TBtraceWindowGroupId::TBtraceWindowGroupId(const TBtraceWindowGroupId& aId)
	: TBtraceIdBase(aId)
	{
	}

EXPORT_C void MBtraceContextObserver::HandleThreadSeenL(const TBtraceTickCount&, const TBtraceThreadId&, TUint)
	{
	Panic(EBtpPanicUndefinedCallBack);
	}

EXPORT_C void MBtraceContextObserver::HandleThreadGoneL(const TBtraceTickCount&, const TBtraceThreadId&, TUint)
	{
	Panic(EBtpPanicUndefinedCallBack);
	}

EXPORT_C void MBtraceContextObserver::HandleThreadExitL(const TBtraceTickCount&, const TBtraceThreadId&, TExitType, TInt, const TDesC&, TUint)
	{
	Panic(EBtpPanicUndefinedCallBack);
	}

EXPORT_C void MBtraceContextObserver::HandleWindowGroupSeenL(const TBtraceTickCount&, const TBtraceWindowGroupId&, TUint)
	{
	Panic(EBtpPanicUndefinedCallBack);
	}

EXPORT_C CBtraceContext* CBtraceContext::NewL(CBtraceReader& aReader, TMode aMode)
	{
	CBtraceContext* self = new(ELeave) CBtraceContext(aReader, aMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBtraceContext::~CBtraceContext()
	{
	iWindowGroupSeenNotifs.Close();
	iThreadExitNotifs.Close();
	iThreadGoneNotifs.Close();
	iThreadSeenNotifs.Close();
	iWindowGroups.Close();
	iThreads.Close();
	iProcesses.Close();
	iReader.RemoveObserver(BTrace::EThreadIdentification, *this);
	iReader.RemoveObserver(KAmTraceCategory, *this);
	}


TBool CBtraceContext::ThreadMatchesId(const TThread& aLeft, const TThread& aRight)
	{
	return (aLeft.iId == aRight.iId);
	}

TBool CBtraceContext::ThreadMatchesKernelId(const TThread& aLeft, const TThread& aRight)
	{
	return (aLeft.iKernelId == aRight.iKernelId);
	}

TBool CBtraceContext::ThreadMatchesNThreadAddress(const TThread& aLeft, const TThread& aRight)
	{
	return (aLeft.iNThreadAddress == aRight.iNThreadAddress);
	}

TBool CBtraceContext::ProcessMatchesId(const TProcess& aLeft, const TProcess& aRight)
	{
	return (aLeft.iId == aRight.iId);
	}

TBool CBtraceContext::ProcessMatchesDProcessAddress(const TProcess& aLeft, const TProcess& aRight)
	{
	return (aLeft.iDProcessAddress == aRight.iDProcessAddress);
	}

TBool CBtraceContext::WindowGroupMatchesId(const TWindowGroup& aLeft, const TWindowGroup& aRight)
	{
	return (aLeft.iId == aRight.iId);
	}

TBool CBtraceContext::WindowGroupMatchesThreadId(const TWindowGroup& aLeft, const TWindowGroup& aRight)
	{
	return (aLeft.iThreadId == aRight.iThreadId);
	}

TBool CBtraceContext::WindowGroupMatchesWServId(const TWindowGroup& aLeft, const TWindowGroup& aRight)
	{
	return (aLeft.iWindowGroupId == aRight.iWindowGroupId);
	}

EXPORT_C const TDesC& CBtraceContext::ThreadName(const TBtraceThreadId& aId) const
	{
	TThread findKey(0, 0, KNullDesC8);
	findKey.iId = aId;
	TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesId));
	if (pos >= 0)
		{
		return iThreads[pos].iName;
		}
	else
		{
		return KUnknown;
		}
	}

EXPORT_C void CBtraceContext::GetFullThreadName(const TBtraceThreadId& aId, TDes& aFullName) const
	{
	TThread findKey(0, 0, KNullDesC8);
	findKey.iId = aId;
	TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesId));
	if (pos >= 0)
		{
		const TThread& thread = iThreads[pos];
		TProcess findKey(thread.iOwningProcess);
		TInt processPos = iProcesses.Find(findKey, TIdentityRelation<TProcess>(ProcessMatchesDProcessAddress));
		__ASSERT_ALWAYS(processPos >= 0, Panic(EBtpPanicFailedToFindProcess));
		aFullName.Zero();
		aFullName.Append(ProcessName(iProcesses[processPos].iId));
		aFullName.Append(_L("::"));
		aFullName.Append(thread.iName);
		}
	else
		{
		aFullName = KUnknown;
		}
	}

EXPORT_C TThreadId CBtraceContext::ThreadId(const TBtraceThreadId& aId) const
	{
	TThread findKey(0, 0, KNullDesC8);
	findKey.iId = aId;
	TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesId));
	__ASSERT_ALWAYS(pos >= 0, Panic(EBtpPanicThreadIdNotFound));
	return iThreads[pos].iKernelId;
	}

EXPORT_C const TDesC& CBtraceContext::ProcessName(const TBtraceProcessId& aId) const
	{
	TProcess findKey(0);
	findKey.iId = aId;
	TInt pos = iProcesses.Find(findKey, TIdentityRelation<TProcess>(ProcessMatchesId));
	__ASSERT_ALWAYS(pos >= 0, Panic(EBtpPanicProcessNameNotFound));
	return iProcesses[pos].iName;
	}

EXPORT_C TInt CBtraceContext::WindowGroupId(const TBtraceWindowGroupId& aId) const
	{
	TWindowGroup findKey(0, 0, KNullDesC);
	findKey.iId = aId;
	TInt pos = iWindowGroups.Find(findKey, TIdentityRelation<TWindowGroup>(WindowGroupMatchesId));
	__ASSERT_ALWAYS(pos >= 0, Panic(EBtpPanicWindowGroupIdNotFound));
	return iWindowGroups[pos].iWindowGroupId;
	}

EXPORT_C const TDesC& CBtraceContext::WindowGroupName(const TBtraceWindowGroupId& aId) const
	{
	TWindowGroup findKey(0, 0, KNullDesC);
	findKey.iId = aId;
	TInt pos = iWindowGroups.Find(findKey, TIdentityRelation<TWindowGroup>(WindowGroupMatchesId));
	__ASSERT_ALWAYS(pos >= 0, Panic(EBtpPanicWindowGroupIdNotFound));
	return iWindowGroups[pos].iName;
	}

EXPORT_C const TBtraceThreadId* CBtraceContext::FindThread(TUint32 aNThreadAddress) const
	{
	TThread findKey(aNThreadAddress, 0, KNullDesC8);
	TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesNThreadAddress));
	if (pos >= 0)
		{
		return &iThreads[pos].iId;
		}
	else
		{
		return NULL;
		}
	}

EXPORT_C const TBtraceThreadId* CBtraceContext::FindThread(const TThreadId& aId) const
	{
	TThread findKey(0, 0, KNullDesC8);
	findKey.iKernelId = TUint(aId);
	TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesKernelId));
	if (pos >= 0)
		{
		return &iThreads[pos].iId;
		}
	else
		{
		return NULL;
		}
	}

EXPORT_C const TBtraceWindowGroupId* CBtraceContext::FindWindowGroup(TInt aWServWgId) const
	{
	TWindowGroup findKey(aWServWgId);
	TInt pos = iWindowGroups.Find(findKey, TIdentityRelation<TWindowGroup>(WindowGroupMatchesWServId));
	if (pos >= 0)
		{
		return &iWindowGroups[pos].iId;
		}
	else
		{
		return NULL;
		}
	}

EXPORT_C void CBtraceContext::FindThreadsL(const TDesC& aPattern, RArray<TBtraceThreadId>& aThreads) const
	{
	TInt count = iThreads.Count();
	while (--count >= 0)
		{
		const TThread& thread = iThreads[count];
		TFullName name;
		GetFullThreadName(thread.iId, name);
		if (name.MatchF(aPattern) != KErrNotFound)
			{
			aThreads.AppendL(thread.iId);
			}
		}
	}

EXPORT_C void CBtraceContext::FindWindowGroupsL(const TDesC& aPattern, RArray<TBtraceWindowGroupId>& aWindowGroups) const
	{
	TInt count = iWindowGroups.Count();
	while (--count >= 0)
		{
		const TWindowGroup& windowGroup = iWindowGroups[count];
		if (windowGroup.iName.MatchF(aPattern) != KErrNotFound)
			{
			aWindowGroups.AppendL(windowGroup.iId);
			}
		}
	}

EXPORT_C void CBtraceContext::FindWindowGroupsByThreadName(const TDesC& aPattern, RArray<TBtraceWindowGroupId>& aWindowGroups) const
	{
	TInt count = iThreads.Count();
	while (--count >= 0)
		{
		const TThread& thread = iThreads[count];
		TFullName name;
		GetFullThreadName(thread.iId, name);
		if (name.MatchF(aPattern) != KErrNotFound)
			{
			for (TInt i = (iWindowGroups.Count() - 1); i >= 0; --i)
				{
				const TWindowGroup& thisWindowGroup = iWindowGroups[i];
				if (thisWindowGroup.iThreadId.Id() == thread.iKernelId)
					{
					aWindowGroups.AppendL(thisWindowGroup.iId);
					}
				}
			}
		}
	}

EXPORT_C void CBtraceContext::NotifyThreadSeenL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId)
	{
	NotifyThreadSeenL(aPattern, aObserver, aId, ENotificationOneShot);
	}

EXPORT_C void CBtraceContext::NotifyThreadSeenL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence)
	{
	iThreadSeenNotifs.AppendL(TThreadSeenNotif(aId, aPattern, aObserver, aPersistence));
	}

EXPORT_C void CBtraceContext::CancelNotifyThreadSeen(MBtraceContextObserver& aObserver)
	{
	for (TInt i = (iThreadSeenNotifs.Count() - 1); i >= 0; --i)
		{
		if (&iThreadSeenNotifs[i].iObserver == &aObserver)
			{
			iThreadSeenNotifs.Remove(i);
			}
		}
	}

EXPORT_C void CBtraceContext::NotifyThreadGoneL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId)
	{
	return NotifyThreadGoneL(aPattern, aObserver, aId, ENotificationOneShot);
	}
	
EXPORT_C void CBtraceContext::NotifyThreadGoneL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence)
	{
	iThreadGoneNotifs.AppendL(TThreadGoneNotif(aId, aPattern, aObserver, aPersistence));
	}

EXPORT_C void CBtraceContext::CancelNotifyThreadGone(MBtraceContextObserver& aObserver)
	{
	for (TInt i = (iThreadGoneNotifs.Count() - 1); i >= 0; --i)
		{
		if (&iThreadGoneNotifs[i].iObserver == &aObserver)
			{
			iThreadGoneNotifs.Remove(i);
			}
		}
	}

EXPORT_C void CBtraceContext::NotifyThreadExitL(const TDesC& aPattern, TUint aExitTypes, TInt* aReason, const TDesC* aCategory, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence)
	{
	iThreadExitNotifs.AppendL(TThreadExitNotif(aId, aPattern, aExitTypes, aReason, aCategory, aObserver, aPersistence));
	}

EXPORT_C void CBtraceContext::CancelNotifyThreadExit(MBtraceContextObserver& aObserver)
	{
	for (TInt i = (iThreadExitNotifs.Count() - 1); i >= 0; --i)
		{
		if (&iThreadExitNotifs[i].iObserver == &aObserver)
			{
			iThreadExitNotifs.Remove(i);
			}
		}
	}

EXPORT_C void CBtraceContext::NotifyWindowGroupSeenL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence)
	{
	iWindowGroupSeenNotifs.AppendL(TWindowGroupSeenNotif(aId, aPattern, TWindowGroupSeenNotif::EWindowGroupName, aObserver, aPersistence));
	}

EXPORT_C void CBtraceContext::NotifyWindowGroupSeenByThreadNameL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence)
	{
	iWindowGroupSeenNotifs.AppendL(TWindowGroupSeenNotif(aId, aPattern, TWindowGroupSeenNotif::EThreadName, aObserver, aPersistence));
	}

EXPORT_C void CBtraceContext::CancelNotifyWindowGroupSeen(MBtraceContextObserver& aObserver)
	{
	for (TInt i = (iWindowGroupSeenNotifs.Count() - 1); i >= 0; --i)
		{
		if (&iWindowGroupSeenNotifs[i].iObserver == &aObserver)
			{
			iWindowGroupSeenNotifs.Remove(i);
			}
		}
	}

CBtraceContext::CBtraceContext(CBtraceReader& aReader, TMode aMode)
	: iMode(aMode), iReader(aReader), iNextBtraceThreadId(KMaxTUint), iNextBtraceProcessId(KMaxTUint)
	{
	}

void CBtraceContext::ConstructL()
	{
	iReader.AddObserverL(BTrace::EThreadIdentification, *this, CBtraceReader::EIncludeSynchronizationFrames);
	iReader.AddObserverL(KAmTraceCategory, *this);
	}

TUint CBtraceContext::GetNextThreadId()
	{
	TUint id = iNextBtraceThreadId--;
	__ASSERT_ALWAYS(iNextBtraceThreadId > 0, Panic(EBtpPanicBtraceThreadIdOverflow));
	return id;
	}

TUint CBtraceContext::GetNextProcessId()
	{
	TUint id = iNextBtraceProcessId--;
	__ASSERT_ALWAYS(iNextBtraceProcessId > 0, Panic(EBtpPanicBtraceProcessIdOverflow));
	return id;
	}

TUint CBtraceContext::GetNextWindowGroupId()
	{
	TUint id = iNextBtraceWindowGroupId--;
	__ASSERT_ALWAYS(iNextBtraceWindowGroupId > 0, Panic(EBtpPanicBtraceWindowGroupIdOverflow));
	return id;
	}

void CBtraceContext::SeenL(const TThread& aThread, const TBtraceTickCount& aTickCount)
	{
	TFullName name;
	GetFullThreadName(aThread.iId, name);
	for (TInt i = (iThreadSeenNotifs.Count() - 1); i >= 0; --i)
		{
		const TThreadSeenNotif& thisNotif = iThreadSeenNotifs[i];
		if (name.MatchF(thisNotif.iPattern) != KErrNotFound)
			{
			MBtraceContextObserver& observer = thisNotif.iObserver;
			TUint id = thisNotif.iId;
			if (thisNotif.iPersistence == ENotificationOneShot)
				{
				iThreadSeenNotifs.Remove(i);
				}
			observer.HandleThreadSeenL(aTickCount, aThread.iId, id);
			}
		}
	}

void CBtraceContext::GoneL(const TThread& aThread, const TBtraceTickCount& aTickCount)
	{
	TFullName name;
	GetFullThreadName(aThread.iId, name);
	for (TInt i = (iThreadGoneNotifs.Count() - 1); i >= 0; --i)
		{
		const TThreadGoneNotif& notif = iThreadGoneNotifs[i];
		if (name.MatchF(notif.iPattern) != KErrNotFound)
			{
			MBtraceContextObserver& observer = notif.iObserver;
			TUint id = notif.iId;
			if (notif.iPersistence == ENotificationOneShot)
				{
				iThreadGoneNotifs.Remove(i);
				}
			observer.HandleThreadGoneL(aTickCount, aThread.iId, id);
			}
		}
	}

void CBtraceContext::ExitedL(const TThread& aThread, TExitType aExitType, TInt aReason, const TDesC& aCategory, const TBtraceTickCount& aTickCount)
	{
	TFullName name;
	GetFullThreadName(aThread.iId, name);
	for (TInt i = (iThreadExitNotifs.Count() - 1); i >= 0; --i)
		{
		const TThreadExitNotif& notif = iThreadExitNotifs[i];
		if (notif.Matches(name, aExitType, aReason, aCategory))
			{
			MBtraceContextObserver& observer = notif.iObserver;
			TUint id = notif.iId;
			if (notif.iPersistence == ENotificationOneShot)
				{
				iThreadExitNotifs.Remove(i);
				}
			observer.HandleThreadExitL(aTickCount, aThread.iId, aExitType, aReason, aCategory, id);
			}
		}
	}

void CBtraceContext::SeenL(const TWindowGroup& aWindowGroup, const TBtraceTickCount& aTickCount)
	{
	for (TInt i = (iWindowGroupSeenNotifs.Count() - 1); i >= 0; --i)
		{
		const TWindowGroupSeenNotif& thisNotif = iWindowGroupSeenNotifs[i];
		if (thisNotif.iPatternType == TWindowGroupSeenNotif::EWindowGroupName)
			{
			if (aWindowGroup.iName.MatchF(thisNotif.iPattern) != KErrNotFound)
				{
				MBtraceContextObserver& observer = thisNotif.iObserver;
				TUint id = thisNotif.iId;
				if (thisNotif.iPersistence == ENotificationOneShot)
					{
					iWindowGroupSeenNotifs.Remove(i);
					}
				observer.HandleWindowGroupSeenL(aTickCount, aWindowGroup.iId, id);
				}
			}
		else
			{
			TThread findKey(0, 0, KNullDesC8);
			findKey.iKernelId = TUint(aWindowGroup.iThreadId);
			TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesKernelId));
			if (pos >= 0)
				{
				TFullName threadName;
				GetFullThreadName(iThreads[pos].iId, threadName);
				if (threadName.MatchF(thisNotif.iPattern))
					{
					MBtraceContextObserver& observer = thisNotif.iObserver;
					TUint id = thisNotif.iId;
					if (thisNotif.iPersistence == ENotificationOneShot)
						{
						iWindowGroupSeenNotifs.Remove(i);
						}
					observer.HandleWindowGroupSeenL(aTickCount, aWindowGroup.iId, id);
					}
				}
			}
		}
	}

void CBtraceContext::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	const TUint8* data = aFrame.iData.Ptr();

	if (aFrame.iCategory == BTrace::EThreadIdentification)
		{
		switch (aFrame.iSubCategory)
			{
			case BTrace::EThreadCreate:
				{
				TUint32 nthreadAddress = *(TUint32*)data;
				TUint32 dprocessAddress = *((TUint32*)data + 1);
				TPtrC8 name(aFrame.iData.Mid(8));

#ifdef BTRACE_CONTEXT_DEBUG
				TFullName name16;
				name16.Copy(name);
				RDebug::Print(_L("BTrace::EThreadCreate: 0x%08x \"%S\""), nthreadAddress, &name16);
#endif

				TThread thread(nthreadAddress, dprocessAddress, name);
				AppendL(thread, aFrame.iTickCount);
				break;
				}
			case BTrace::EThreadDestroy:
				{
				TUint32 nthreadAddress = *(TUint32*)data;
				TThread findKey(nthreadAddress, 0, KNullDesC8);
				TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesNThreadAddress));
				if (pos >= 0)
					{
#ifdef BTRACE_CONTEXT_DEBUG
					RDebug::Print(_L("BTrace::EThreadDestroy: %u %u 0x%08x \"%S\""), iThreads[pos].iId.Value(), iThreads[pos].iKernelId, nthreadAddress, &iThreads[pos].iName);
#endif
					GoneL(iThreads[pos], aFrame.iTickCount);
					RemoveThread(pos);
					}
				else
					{
					// This can happen if a thread is destroyed after the btrace buffer has been enabled but before the BTrace::EThreadIdentification category has been enabled.
					iReader.Log(_L("Destroyed thread 0x%08x not found\r\n"), nthreadAddress);
					}
				break;
				}
			case BTrace::EThreadName:
				{
				TUint32 nthreadAddress = *(TUint32*)data;
				TUint32 dprocessAddress = *((TUint32*)data + 1);
				TPtrC8 name(aFrame.iData.Mid(8));

#ifdef BTRACE_CONTEXT_DEBUG
				TFullName name16;
				name16.Copy(name);
				RDebug::Print(_L("BTrace::EThreadName: 0x%08x \"%S\""), nthreadAddress, &name16);
#endif				
				TThread findKey(nthreadAddress, 0, KNullDesC8);
				TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesNThreadAddress));
				if (pos >= 0)
					{
					iThreads[pos].iName.Copy(name);
					SeenL(iThreads[pos], aFrame.iTickCount);
					}
				else
					{
					TThread thread(nthreadAddress, dprocessAddress, name);
					AppendL(thread, aFrame.iTickCount);
					}
				break;
				}
			case BTrace::EProcessCreate:
				{
				TUint32 dprocessAddress = *(TUint32*)data;
#ifdef BTRACE_CONTEXT_DEBUG
				RDebug::Print(_L("BTrace::EProcessCreate: 0x%08x"), dprocessAddress);
#endif
				TProcess process(dprocessAddress);
				AppendL(process, aFrame.iTickCount);
				break;
				}
			case BTrace::EProcessName:
				{
				TUint32 dprocessAddress = *((TUint32*)data + 1);
				TPtrC8 name(aFrame.iData.Mid(8));

#ifdef BTRACE_CONTEXT_DEBUG
				TFullName name16;
				name16.Copy(name);
				RDebug::Print(_L("BTrace::EProcessName: 0x%08x \"%S\""), dprocessAddress, &name16);
#endif
				TProcess findKey(dprocessAddress);
				TInt pos = iProcesses.Find(findKey, TIdentityRelation<TProcess>(ProcessMatchesDProcessAddress));
				if (pos >= 0)
					{
					iProcesses[pos].iName.Copy(name);
					SeenAllThreadsL(iProcesses[pos], aFrame.iTickCount);
					}
				else
					{
					TProcess process(dprocessAddress, name);
					AppendL(process, aFrame.iTickCount);
					}
				break;
				}
			case BTrace::EThreadId:
				{
				TUint32 nthreadAddress = *(TUint32*)data;
				TUint32 threadId = *((TUint32*)data + 2);
				TThread findKey(nthreadAddress, 0, KNullDesC8);
				TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesNThreadAddress));
				__ASSERT_ALWAYS(pos >= 0, Panic(EBtpPanicThreadNotFoundForId));
				iThreads[pos].iKernelId = threadId;
#ifdef BTRACE_CONTEXT_DEBUG
				RDebug::Print(_L("BTrace::EThreadId: %u 0x%08x \"%S\""), threadId, nthreadAddress, &iThreads[pos].iName);
#endif
				break;
				}
			default:
				{
				// Ignore anything we don't know about.
				break;
				}
			}
		}
	else if (aFrame.iCategory == KAmTraceCategory)
		{
		if (aFrame.iSubCategory == EAmTraceSubCategoryWindowGroupName)
			{
			TUint wgId = *(TUint32*)data;
			TInt kernelThreadId = *((TInt32*)data + 1);
			TPtrC8 nameData = aFrame.iData.Mid(8);
			TPtrC name((TUint16*)nameData.Ptr(), nameData.Length() / 2);
			TWindowGroup findKey(wgId);
			TInt pos = iWindowGroups.Find(findKey, TIdentityRelation<TWindowGroup>(WindowGroupMatchesWServId));
			if (pos >= 0)
				{
				TWindowGroup& windowGroup = iWindowGroups[pos];
				windowGroup.iName = name;
				SeenL(windowGroup, aFrame.iTickCount);
				}
			else
				{
				TWindowGroup windowGroup(wgId, TThreadId(kernelThreadId), name);
				windowGroup.iId.Set(GetNextWindowGroupId());
				iWindowGroups.AppendL(windowGroup);
				SeenL(windowGroup, aFrame.iTickCount);
				}
			}
		else if ((aFrame.iSubCategory >= EAmTraceSubCategoryThreadKilled) && (aFrame.iSubCategory <= EAmTraceSubCategoryThreadPanicked))
			{
			TUint id = *(TUint32*)data;
			TInt reason = *((TInt32*)data + 1);
			TThread findKey(0, 0, KNullDesC8);
			findKey.iKernelId = id;
			TInt pos = iThreads.Find(findKey, TIdentityRelation<TThread>(ThreadMatchesKernelId));
			if (pos >= 0)
				{
				const TThread& thread = iThreads[pos];
				switch (aFrame.iSubCategory)
					{
					case EAmTraceSubCategoryThreadKilled:
						{
						ExitedL(thread, EExitKill, reason, KNullDesC, aFrame.iTickCount);
						break;
						}
					case EAmTraceSubCategoryThreadTerminated:
						{
						ExitedL(thread, EExitTerminate, reason, KNullDesC, aFrame.iTickCount);
						break;
						}
					case EAmTraceSubCategoryThreadPanicked:
						{
						TPtrC8 narrowCategory(aFrame.iData.Mid(sizeof(TInt) * 2));
						TExitCategoryName category;
						category.Copy(narrowCategory);
						ExitedL(thread, EExitPanic, reason, category, aFrame.iTickCount);
						break;
						}
					default:
						{
						// Ignore anything we don't know about.
						break;
						}
					}
				}
			}
		}
	}

void CBtraceContext::AppendL(const TThread& aThread, const TBtraceTickCount& aTickCount)
	{
	iThreads.AppendL(aThread);
	TThread& thread = iThreads[iThreads.Count() - 1];
	thread.iId.Set(GetNextThreadId());

	// Increment the reference count of the owning process.
	TProcess findKey(thread.iOwningProcess);
	TInt pos = iProcesses.Find(findKey, TIdentityRelation<TProcess>(ProcessMatchesDProcessAddress));
	if (pos >= 0)
		{
		++(iProcesses[pos].iReferenceCount);
		}

	SeenL(thread, aTickCount);
	}

void CBtraceContext::AppendL(const TProcess& aProcess, const TBtraceTickCount& aTickCount)
	{
	iProcesses.AppendL(aProcess);
	TProcess& process = iProcesses[iProcesses.Count() - 1];
	process.iId.Set(GetNextProcessId());

	// Set the reference count for this process.
	for (TInt i = (iThreads.Count() - 1); i >= 0; --i)
		{
		if (iThreads[i].iOwningProcess == process.iDProcessAddress)
			{
			++process.iReferenceCount;
			}
		}

	SeenAllThreadsL(process, aTickCount);
	}

void CBtraceContext::RemoveThread(TInt aPosition)
	{
	TProcess findKey(iThreads[aPosition].iOwningProcess);
	TInt processPos = iProcesses.Find(findKey, TIdentityRelation<TProcess>(ProcessMatchesDProcessAddress));
	if (processPos >= 0)
		{
		if (--(iProcesses[processPos].iReferenceCount) == 0)
			{
			iProcesses.Remove(processPos);
			}
		}
	iThreads.Remove(aPosition);
	}

void CBtraceContext::SeenAllThreadsL(const TProcess& aProcess, const TBtraceTickCount& aTickCount)
	{
	// Call SeenL for each thread belonging to this process because. This is called when a process
	// is created or altered because this change may allow a TThreadSeenNotif to be completed.
	const TInt numThreads = iThreads.Count();
	for (TInt i = 0; i < numThreads; ++i)
		{
		const TThread& thread = iThreads[i];
		if (thread.iOwningProcess == aProcess.iDProcessAddress)
			{
			SeenL(thread, aTickCount);
			}
		}
	}

void CBtraceContext::BtraceBufferHasBeenReset()
	{
	iThreads.Close();
	iProcesses.Close();
	}

CBtraceContext::TThread::TThread(TUint32 aNThreadAddress, TUint32 aOwningProcess, const TDesC8& aName)
	: iNThreadAddress(aNThreadAddress), iOwningProcess(aOwningProcess), iKernelId(0)
	{
	iName.Copy(aName);
	}

CBtraceContext::TProcess::TProcess(TUint32 aDProcessAddress)
	: iDProcessAddress(aDProcessAddress), iReferenceCount(0)
	{
	}

CBtraceContext::TProcess::TProcess(TUint32 aDProcessAddress, const TDesC8& aName)
	: iDProcessAddress(aDProcessAddress), iReferenceCount(0)
	{
	iName.Copy(aName);
	}

CBtraceContext::TWindowGroup::TWindowGroup(TInt aWindowGroupId)
	: iWindowGroupId(aWindowGroupId), iThreadId(0)
	{
	}

CBtraceContext::TWindowGroup::TWindowGroup(const TDesC& aName)
	: iWindowGroupId(0), iThreadId(0), iName(aName)
	{
	}

CBtraceContext::TWindowGroup::TWindowGroup(TInt aWindowGroupId, TThreadId aThreadId, const TDesC& aName)
	: iWindowGroupId(aWindowGroupId), iThreadId(aThreadId), iName(aName)
	{
	}

CBtraceContext::TThreadSeenNotif::TThreadSeenNotif(TUint aId, const TDesC& aPattern, MBtraceContextObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iId(aId), iPattern(aPattern), iObserver(aObserver), iPersistence(aPersistence)
	{
	}

CBtraceContext::TThreadGoneNotif::TThreadGoneNotif(TUint aId, const TDesC& aPattern, MBtraceContextObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iId(aId), iPattern(aPattern), iObserver(aObserver), iPersistence(aPersistence)
	{
	}

CBtraceContext::TThreadExitNotif::TThreadExitNotif(TUint aId, const TDesC& aPattern, TUint aExitTypes, TInt* aReason, const TDesC* aCategory, MBtraceContextObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iId(aId), iPattern(aPattern), iExitTypes(aExitTypes), iReason(aReason), iCategory(aCategory), iObserver(aObserver), iPersistence(aPersistence)
	{
	}

CBtraceContext::TWindowGroupSeenNotif::TWindowGroupSeenNotif(TUint aId, const TDesC& aPattern, TPatternType aPatternType, MBtraceContextObserver& aObserver, TBtraceNotificationPersistence aPersistence)
	: iId(aId), iPattern(aPattern), iPatternType(aPatternType), iObserver(aObserver), iPersistence(aPersistence)
	{
	}

TBool CBtraceContext::TThreadExitNotif::Matches(const TDesC& aThreadName, TExitType aExitType, TInt aReason, const TDesC& aCategory) const
	{
	if (aThreadName.MatchF(iPattern) != KErrNotFound)
		{
		if (MatchesType(aExitType))
			{
			if ((iReason == NULL) || (*iReason == aReason))
				{
				if ((aExitType != EExitPanic) || ((iCategory == NULL) || (*iCategory == aCategory)))
					{
					return ETrue;
					}
				}
			}
		}
	return EFalse;
	}

TBool CBtraceContext::TThreadExitNotif::MatchesType(TExitType aExitType) const
	{
	switch (aExitType)
		{
		case EExitKill:
			return (iExitTypes & EKill);
		case EExitTerminate:
			return (iExitTypes & ETerminate);
		case EExitPanic:
			return (iExitTypes & EPanic);
		default:
			ASSERT(EFalse);
			return EFalse;
		}
	}
