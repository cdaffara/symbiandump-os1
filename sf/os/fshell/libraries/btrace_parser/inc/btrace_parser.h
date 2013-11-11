// btrace_parser.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution

// Backwards Compatibility:
// The interfaces defined in this file should be considered INTERNAL to the
// fshell project as they may be subject to change. Expect to have to absorb
// downstream compatibility breaks if you use these interfaces from outside
// of the fshell project.

#ifndef __BTRACE_PARSER_H__
#define __BTRACE_PARSER_H__

#include <e32base.h>
#include <fshell/btrace_parser_defs.h>
#include <fshell/extrabtrace.h>
#include FSHELL_D32BTRACE_HEADER
#include <f32file.h>
#include <e32property.h>
#include <fshell/common.mmh>

#ifdef FSHELL_ATRACE_SUPPORT
#include <fshell/atrace/atraceconfig.h>
#endif

class TBtraceHeader;
struct TKeyEvent;
struct TPointerEvent;
class CMultipartFragment;
class CFlusher;

enum TBtraceParserPanic
	{
	EBtpPanicUndefinedCallBack = 0,
	EBtpPanicIncompatiblePubSubArgs = 1,
	EBtpPanicThreadIdNotFound = 2,
	EBtpPanicProcessNameNotFound = 3,
	EBtpPanicBtraceThreadIdOverflow = 4,
	EBtpPanicThreadNotFoundForId = 5,
	EBtpPanicFailedToInsertCpuUsageObject = 6,
	EBtpPanicBtraceProcessIdOverflow = 7,
	EBtpPanicUnused1 = 8,
	EBtpPanicUnused2 = 9,
	EBtpPanicNegativeTickInterval = 10,
	EBtpPanicUnused3 = 11,
	EBtpPanicInvalidReaderMode = 12,
	EBtpPanicUnexpectedReaderRun = 13,
	EBtpPanicUnknownCpuContext = 14,
	EBtpPanicFlusherObserverAlreadySet = 15,
	EBtpPanicReaderNotReplaying = 16,
	EBtpPanicNoReplayData = 17,
	EBtpPanicFailedToFindProcess = 18,
	EBtpPanicNegativeRefCount = 19,
	EBtpPanicNestedSynchronizeCalls = 20,
	EBtpPanicFrameObserverAlreadyExists = 21,
	EBtpPanicTriedToDisableReplayWhenBtraceNotRunning = 22,
	EBtpPanicAddObserverCalledWhileIterating = 23,
	EBtpPanicRemoveObserverCalledWhileIterating = 24,
	EBtpPanicTooManyDomainEventDataItems = 25,
	EBtpPanicBtraceWindowGroupIdOverflow = 26,
	EBtpPanicWindowGroupIdNotFound = 27,
	EBtpPanicWservWindowGroupIdNotFound = 28
	};

void Panic(TBtraceParserPanic aReason);
struct SBtraceParserTls;

enum TBtraceNotificationPersistence
	{
	ENotificationOneShot,
	ENotificationPersistent,
	};

class TBtraceUtils
	{
public:
	IMPORT_C static TUint32 MicroSecondsToNanoTicks(TTimeIntervalMicroSeconds32 aInterval);
	IMPORT_C static TUint64 MicroSecondsToFastTicks(TTimeIntervalMicroSeconds32 aInterval);
	IMPORT_C static TTimeIntervalMicroSeconds NanoTicksToMicroSeconds(TUint32 aNanoTicks);
	IMPORT_C static TTimeIntervalMicroSeconds FastTicksToMicroSeconds(const TUint64& aFastTicks);

	IMPORT_C static void DebugOverrideTimerSettings(TInt aNanoPeriod, TInt aFastCounterFreq, TBool aFastCountUp);

public:
	static TInt NanoTickPeriod();
	static TInt FastCounterFrequency();
	static TBool FastCounterCountsUp();
private:
	static SBtraceParserTls* CreateTls();
	static TInt CalculateNanoTickPeriod();
	static TInt CalculateFastCounterFrequency();
	static TInt CalculateFastCounterCountsUp();
	};

class TBtraceTickCount
	{
public:
	IMPORT_C TBtraceTickCount();
	IMPORT_C void SetToNow();
	IMPORT_C TUint32 IntervalInNanoTicks(const TBtraceTickCount& aTickCount) const;
	IMPORT_C TUint64 IntervalInFastTicks(const TBtraceTickCount& aTickCount) const;
	IMPORT_C TTimeIntervalMicroSeconds IntervalInMicroSeconds(const TBtraceTickCount& aTickCount) const;
	IMPORT_C TBool operator==(const TBtraceTickCount& aTickCount) const;
	IMPORT_C TBool operator!=(const TBtraceTickCount& aTickCount) const;
	IMPORT_C TBool operator>=(const TBtraceTickCount& aTickCount) const;
	IMPORT_C TBool operator<=(const TBtraceTickCount& aTickCount) const;
	IMPORT_C TBool operator>(const TBtraceTickCount& aTickCount) const;
	IMPORT_C TBool operator<(const TBtraceTickCount& aTickCount) const;
public:
	TUint32 iNano;
	TUint32 iFast;
	};


class TBtraceFrame
	{
public:
	TBtraceFrame(const TBtraceHeader& aHeader);
public:
	TUint8 iSize;
	TUint8 iFlags;
	TUint8 iCategory;
	TUint8 iSubCategory;
	TBtraceTickCount iTickCount;
	TUint32 iThreadContext;
	TUint32 iProgramCounter;
	TPtrC8 iData;
	};


class MBtraceLog
	{
public:
	IMPORT_C virtual TBool LogEnabled() = 0;
	IMPORT_C virtual void Log(const TDesC& aLog) = 0;
	};

class CBtraceReader;


class MBtraceObserver
	{
public:
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame) = 0;
	virtual void BtraceBufferHasBeenReset() {} // Empty default impl
	};
	
class MBtraceFrameObserver
	{
public:
	virtual void HandleFrameDispatchStart() = 0;
	virtual void HandleFrameDispatchComplete() = 0;
	};

NONSHARABLE_CLASS(CBtraceReader) : public CActive
	{
public:
	enum TMode
		{
		EAllowBtraceToOverflow,
		EFlushOnBtraceThreshold,
		ELeaveOnBtraceThreshold
		};
	enum TObserverType
		{
		ENormal,
		EIncludeSynchronizationFrames
		};
public:
	IMPORT_C static CBtraceReader* NewL(TMode aMode, TInt aBufferSize, TInt aThreshold, MBtraceLog* aLog = NULL);
	IMPORT_C static CBtraceReader* NewL(const TDesC& aReplayFileName, MBtraceLog* aLog = NULL);
	IMPORT_C ~CBtraceReader();
	IMPORT_C void Reset();
	IMPORT_C void SynchronizeL();
	IMPORT_C void Start(const TBtraceTickCount& aInitialTickCount);
	IMPORT_C void Start(const TBtraceTickCount& aInitialTickCount, TTimeIntervalMicroSeconds32 aAutoFlushPeriod);
	IMPORT_C void DisableBtrace();
	IMPORT_C void EnableCategoryL(TUint aCategory);
	IMPORT_C void DisableCategoryL(TUint aCategory);
	IMPORT_C void AddObserverL(MBtraceObserver& aObserver);
	IMPORT_C void AddObserverL(MBtraceObserver& aObserver, TObserverType aType);
	IMPORT_C void AddObserverL(TUint aCategory, MBtraceObserver& aObserver);
	IMPORT_C void AddObserverL(TUint aCategory, MBtraceObserver& aObserver, TObserverType aType);
	IMPORT_C void RemoveObserver(MBtraceObserver& aObserver);
	IMPORT_C void RemoveObserver(TUint aCategory, MBtraceObserver& aObserver);
	IMPORT_C void AddFrameObseverL(MBtraceFrameObserver& aObserver);
	IMPORT_C void RemoveFrameObserver(MBtraceFrameObserver& aObserver);
	IMPORT_C void GetReplayStartTickCountL(TBtraceTickCount& aTickCount);
	IMPORT_C void SetBufferSizeL(TInt aBufferSize);
	IMPORT_C void SetReplayFileNameL(const TDesC& aReplayFileName); // Pass empty desc to disable (not allowed to disable if constructed with the overload that took a replay filename)
	IMPORT_C const TBtraceTickCount& CurrentTickCount() const;
	IMPORT_C void RecordBtraceDataToFileL(const TDesC& aRecordFileName); // Pass empty desc to disable
	IMPORT_C void LogBtraceStatus();
	IMPORT_C void SetMultipartReassemblyL(TInt aMaxTrackedFragments); // zero to disable
	IMPORT_C void FlushL();
public:
	void Log(const TDesC& aDes) const;
	void Log(TRefByValue<const TDesC> aFmt, ...) const;
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
private:
	CBtraceReader(TMode aMode, TInt aThreshold, MBtraceLog* aLog);
	void ConstructL(TInt aBufferSize);
	void ConstructL(const TDesC& aReplayFileName);
	void QueueRead();
	void DoReadL(TBool& aFinished);
	TInt GetData(TUint8*& aData);
	void DispatchFrameL(const TBtraceFrame& aFrame);
	void NotifyObserversOfBufferReset();
	void ResetAndReprimeL();
	void NewMultipartFrameL(const TBtraceFrame& aFrame, TUint32 aExtra);
	void AdditionalMultipartFragmentL(const TBtraceFrame& aFrame, TUint32 aExtra, TBool aLast);
private:
	class TObserver
		{
	public:
		TObserver(MBtraceObserver& aObserver, TObserverType aType, TUint aCategory);
		static TObserver ArrayKey(TUint aCategory);

	private:
		TObserver(TUint aCategory); // Only for use by ArrayKey
	public:
		MBtraceObserver* iObserver;
		TObserverType iType;
		TUint iCategory;
		};
private:
	static TBool ObserversEqual(const TObserver& aA, const TObserver& aB);
	static void DecIteratingObservers(TAny* aSelf);
private:
	const TMode iMode;
	MBtraceLog* iLog;
	CFlusher* iFlusher;
	RBTrace iBtrace;
#ifdef FSHELL_ATRACE_SUPPORT
	RATraceConfig iAtraceConfig;
#endif
	TInt iThreshold;
	RArray<TObserver> iObservers; // Sorted by TObserver::iCategory
	TInt iNumBytesRead;
	TInt iNumFramesRead;
	TBtraceTickCount iCurrentTickCount;
	TBtraceTickCount iInitialTickCount;
	TPtrC8 iBtraceBuffer;
	HBufC8* iReplayData;
	RFs iFs;
	RFile iReplayFile;
	TBool iSynchronizing;
	MBtraceFrameObserver* iFrameObserver;
	RFile iRecordFile;
	TInt iIteratingObservers;
	TInt iNextSyncFrameId;
	TInt iSyncFrameIdToWaitFor;
	TInt iMaxTrackedMultipartFragments;
	RPointerArray<CMultipartFragment> iMultipartFragments;
	};

class CRefCountedObject : public CBase
	{
public:
	IMPORT_C void IncRef();
	IMPORT_C void DecRef();
	IMPORT_C TInt RefCount() const;
protected:
	CRefCountedObject();
private:
	TInt iRefCount;
	};

class TBtraceIdBase
	{
public:
	IMPORT_C void Set(TUint aValue);
	IMPORT_C TUint Value() const;
	IMPORT_C TBool operator==(const TBtraceIdBase& aId) const;
protected:
	TBtraceIdBase();
	TBtraceIdBase(TUint aId);
	TBtraceIdBase(const TBtraceIdBase& aId);
private:
	TUint iId; 
	};

class TBtraceThreadId : public TBtraceIdBase
	{
public:
	IMPORT_C TBtraceThreadId();
	IMPORT_C explicit TBtraceThreadId(TUint aId);
	IMPORT_C TBtraceThreadId(const TBtraceThreadId& aId);
	};

class TBtraceProcessId : public TBtraceIdBase
	{
public:
	IMPORT_C TBtraceProcessId();
	IMPORT_C explicit TBtraceProcessId(TUint aId);
	IMPORT_C TBtraceProcessId(const TBtraceProcessId& aId);
	};

class TBtraceWindowGroupId : public TBtraceIdBase
	{
public:
	IMPORT_C TBtraceWindowGroupId();
	IMPORT_C explicit TBtraceWindowGroupId(TUint aId);
	IMPORT_C TBtraceWindowGroupId(const TBtraceWindowGroupId& aId);
	};


class MBtraceContextObserver
	{
public:
	IMPORT_C virtual void HandleThreadSeenL(const TBtraceTickCount& aTickCount, const TBtraceThreadId& aId, TUint aUserId);
	IMPORT_C virtual void HandleThreadGoneL(const TBtraceTickCount& aTickCount, const TBtraceThreadId& aId, TUint aUserId);
	IMPORT_C virtual void HandleThreadExitL(const TBtraceTickCount& aTickCount, const TBtraceThreadId& aId, TExitType aExitType, TInt aReason, const TDesC& aCategory, TUint aUserId);
	IMPORT_C virtual void HandleWindowGroupSeenL(const TBtraceTickCount& aTickCount, const TBtraceWindowGroupId& aId, TUint aUserId);
	};

NONSHARABLE_CLASS(CBtraceContext) : public CRefCountedObject, public MBtraceObserver
	{
public:
	enum TMode
		{
		ENormal
		};
	enum TExitTypes
		{
		EKill		= 0x00000001,
		ETerminate	= 0x00000002,
		EPanic		= 0x00000004
		};
public:
	IMPORT_C static CBtraceContext* NewL(CBtraceReader& aReader, TMode aMode = ENormal);
	IMPORT_C ~CBtraceContext();
	IMPORT_C const TDesC& ThreadName(const TBtraceThreadId& aId) const;
	IMPORT_C void GetFullThreadName(const TBtraceThreadId& aId, TDes& aFullName) const;
	IMPORT_C TThreadId ThreadId(const TBtraceThreadId& aId) const;
	IMPORT_C const TDesC& ProcessName(const TBtraceProcessId& aId) const;
	IMPORT_C TInt WindowGroupId(const TBtraceWindowGroupId& aId) const;
	IMPORT_C const TDesC& WindowGroupName(const TBtraceWindowGroupId& aId) const;
	IMPORT_C const TBtraceThreadId* FindThread(TUint32 aNThreadAddress) const;
	IMPORT_C const TBtraceThreadId* FindThread(const TThreadId& aId) const;
	IMPORT_C const TBtraceWindowGroupId* FindWindowGroup(TInt aWServWgId) const;
	IMPORT_C void FindThreadsL(const TDesC& aPattern, RArray<TBtraceThreadId>& aThreads) const;
	IMPORT_C void FindWindowGroupsL(const TDesC& aPattern, RArray<TBtraceWindowGroupId>& aWindowGroups) const;
	IMPORT_C void FindWindowGroupsByThreadName(const TDesC& aPattern, RArray<TBtraceWindowGroupId>& aWindowGroups) const;
	IMPORT_C void NotifyThreadSeenL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId);
	IMPORT_C void NotifyThreadSeenL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyThreadSeen(MBtraceContextObserver& aObserver);
	IMPORT_C void NotifyThreadGoneL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId);
	IMPORT_C void NotifyThreadGoneL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyThreadGone(MBtraceContextObserver& aObserver);
	IMPORT_C void NotifyThreadExitL(const TDesC& aPattern, TUint aExitTypes, TInt* aReason, const TDesC* aCategory, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence);	// Note, the pointer parameters are optional.
	IMPORT_C void CancelNotifyThreadExit(MBtraceContextObserver& aObserver);
	IMPORT_C void NotifyWindowGroupSeenL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void NotifyWindowGroupSeenByThreadNameL(const TDesC& aPattern, MBtraceContextObserver& aObserver, TUint aId, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyWindowGroupSeen(MBtraceContextObserver& aObserver);
private:
	CBtraceContext(CBtraceReader& aReader, TMode aMode);
	void ConstructL();
	TUint GetNextThreadId();
	TUint GetNextProcessId();
	TUint GetNextWindowGroupId();
private: // From MBtraceObserver.
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
	virtual void BtraceBufferHasBeenReset();
private:
	class TThread
		{
	public:
		TThread(TUint32 aNThreadAddress, TUint32 aOwningProcess, const TDesC8& aName);
	public:
		TUint32 iNThreadAddress;
		TBtraceThreadId iId;
		TUint32 iOwningProcess;
		TUint32 iKernelId;
		TName iName;
		};
	class TProcess
		{
	public:
		TProcess(TUint32 aDProcessAddress);
		TProcess(TUint32 aDProcessAddress, const TDesC8& aName);
	public:
		TUint32 iDProcessAddress;
		TBtraceProcessId iId;
		TName iName;
		TInt iReferenceCount;
		};
	class TWindowGroup
		{
	public:
		TWindowGroup(TInt aWindowGroupId);
		TWindowGroup(const TDesC& aName);
		TWindowGroup(TInt aWindowGroupId, TThreadId aThreadId, const TDesC& aName);
	public:
		TBtraceWindowGroupId iId;
		TInt iWindowGroupId;
		TThreadId iThreadId;
		TFullName iName;
		};
	class TThreadSeenNotif
		{
	public:
		TThreadSeenNotif(TUint aId, const TDesC& aPattern, MBtraceContextObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	public:
		TUint iId;
		TName iPattern;
		MBtraceContextObserver& iObserver;
		TBtraceNotificationPersistence iPersistence;
		};
	class TThreadGoneNotif
		{
	public:
		TThreadGoneNotif(TUint aId, const TDesC& aPattern, MBtraceContextObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	public:
		TUint iId;
		TName iPattern;
		MBtraceContextObserver& iObserver;
		TBtraceNotificationPersistence iPersistence;
		};
	class TThreadExitNotif
		{
	public:
		TThreadExitNotif(TUint aId, const TDesC& aPattern, TUint aExitTypes, TInt* aReason, const TDesC* aCategory, MBtraceContextObserver& aObserver, TBtraceNotificationPersistence aPersistence);
		TBool Matches(const TDesC& aThreadName, TExitType aExitType, TInt aReason, const TDesC& aCategory) const;
	private:
		TBool MatchesType(TExitType aExitType) const;
	public:
		TUint iId;
		TName iPattern;
		TUint iExitTypes;
		TInt* iReason;
		const TDesC* iCategory;
		MBtraceContextObserver& iObserver;
		TBtraceNotificationPersistence iPersistence;
		};
	class TWindowGroupSeenNotif
		{
	public:
		enum TPatternType
			{
			EWindowGroupName,
			EThreadName
			};
	public:
		TWindowGroupSeenNotif(TUint aId, const TDesC& aPattern, TPatternType aPatternType, MBtraceContextObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	public:
		TUint iId;
		TName iPattern;
		TPatternType iPatternType;
		MBtraceContextObserver& iObserver;
		TBtraceNotificationPersistence iPersistence;
		};
private:
	void SeenL(const TThread& aThread, const TBtraceTickCount& aTickCount);
	void GoneL(const TThread& aThread, const TBtraceTickCount& aTickCount);
	void ExitedL(const TThread& aThread, TExitType aExitType, TInt aReason, const TDesC& aCategory, const TBtraceTickCount& aTickCount);
	void AppendL(const TThread& aThread, const TBtraceTickCount& aTickCount);
	void AppendL(const TProcess& aProcess, const TBtraceTickCount& aTickCount);
	void RemoveThread(TInt aPosition);
	void SeenAllThreadsL(const TProcess& aProcess, const TBtraceTickCount& aTickCount);
	void SeenL(const TWindowGroup& aWindowGroup, const TBtraceTickCount& aTickCount);
	static TBool ThreadMatchesId(const TThread& aLeft, const TThread& aRight);
	static TBool ThreadMatchesKernelId(const TThread& aLeft, const TThread& aRight);
	static TBool ThreadMatchesNThreadAddress(const TThread& aLeft, const TThread& aRight);
	static TBool ProcessMatchesId(const TProcess& aLeft, const TProcess& aRight);
	static TBool ProcessMatchesDProcessAddress(const TProcess& aLeft, const TProcess& aRight);
	static TBool WindowGroupMatchesId(const TWindowGroup& aLeft, const TWindowGroup& aRight);
	static TBool WindowGroupMatchesThreadId(const TWindowGroup& aLeft, const TWindowGroup& aRight);
	static TBool WindowGroupMatchesWServId(const TWindowGroup& aLeft, const TWindowGroup& aRight);
private:
	TMode iMode;
	CBtraceReader& iReader;
	TUint iNextBtraceThreadId;
	TUint iNextBtraceProcessId;
	TUint iNextBtraceWindowGroupId;
	RArray<TThread> iThreads;
	RArray<TProcess> iProcesses;
	RArray<TWindowGroup> iWindowGroups;
	RArray<TThreadSeenNotif> iThreadSeenNotifs;
	RArray<TThreadGoneNotif> iThreadGoneNotifs;
	RArray<TThreadExitNotif> iThreadExitNotifs;
	RArray<TWindowGroupSeenNotif> iWindowGroupSeenNotifs;
	};


class MBtraceCpuUsageObserver
	{
public:
	enum TContextSwitchType
		{
		EToThisThread,
		EFromThisThread
		};
	enum TIdleType
		{
		ENeverScheduled,
		EScheduledAtLeastOnce
		};
	class TCpuUsage
		{
	public:
		TCpuUsage(const TBtraceThreadId& aId);
		TCpuUsage(const TBtraceThreadId& aId, const TBtraceTickCount& aTickCount);
	public:
		TBtraceThreadId iId;
		TUint64 iNumFastTicks;
	private:
		friend class CBtraceCpuUsage;
		TBtraceTickCount iSwitchedInAt;
		};
public:
	virtual void HandleCpuUsageL(const TBtraceTickCount& aTickCount, const TArray<TCpuUsage>& aCpuUsage) = 0;
	virtual void HandleContextSwitchL(const TBtraceTickCount& aTickCount, const TBtraceThreadId& aId, TContextSwitchType aType) = 0;
	virtual void HandleThreadIdleL(const TBtraceTickCount& aTickCount, const TBtraceThreadId& aId, TIdleType aIdleType) = 0;
	};

NONSHARABLE_CLASS(CBtraceCpuUsage) : public CRefCountedObject, public MBtraceObserver
	{
public:
	IMPORT_C static CBtraceCpuUsage* NewL(CBtraceReader& aReader, CBtraceContext& aContext);
	IMPORT_C ~CBtraceCpuUsage();
	IMPORT_C void NotifyCpuUsageL(TUint32 aNanoTickCount, TTimeIntervalMicroSeconds32 aPeriod, MBtraceCpuUsageObserver& aObserver);
	IMPORT_C void NotifyCpuUsageL(TUint32 aNanoTickCount, TTimeIntervalMicroSeconds32 aPeriod, MBtraceCpuUsageObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void NotifyContextSwitchL(const TBtraceThreadId& aId, MBtraceCpuUsageObserver& aObserver);
	IMPORT_C void NotifyThreadIdleL(const TBtraceThreadId& aId, const TBtraceTickCount& aFromTickCount, TTimeIntervalMicroSeconds32 aPeriod, MBtraceCpuUsageObserver& aObserver);
	IMPORT_C void NotifyThreadIdleL(const TBtraceThreadId& aId, const TBtraceTickCount& aFromTickCount, TTimeIntervalMicroSeconds32 aPeriod, MBtraceCpuUsageObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyThreadIdle(MBtraceCpuUsageObserver& aObserver);
private:
	CBtraceCpuUsage(CBtraceReader& aReader, CBtraceContext& aContext);
	void ConstructL();
private: // From MBtraceObserver.
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
private:
	class RCpuUsageNotif
		{
	public:
		RCpuUsageNotif(TUint32 aStartTickCount, TUint32 aNumNanoTicks, MBtraceCpuUsageObserver& aObserver, TBtraceNotificationPersistence aPersistence);
		void Close();
	public:
		TUint32 iStartTickCount;
		TUint32 iNumNanoTicks;
		MBtraceCpuUsageObserver* iObserver;
		RArray<MBtraceCpuUsageObserver::TCpuUsage> iUsage;
		TBtraceNotificationPersistence iPersistence;
		};
	class TContextSwitchNotif
		{
	public:
		TContextSwitchNotif(const TBtraceThreadId& aId);
		TContextSwitchNotif(const TBtraceThreadId& aId, MBtraceCpuUsageObserver& aObserver);
	public:
		TBtraceThreadId iId;
		MBtraceCpuUsageObserver* iObserver;
		};
	class TThreadIdleNotif
		{
	public:
		TThreadIdleNotif(const TBtraceThreadId& aId);
		TThreadIdleNotif(const TBtraceThreadId& aId, const TBtraceTickCount& aFromTickCount, TUint aNumNanoTicks, MBtraceCpuUsageObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	public:
		TBtraceThreadId iId;
		TUint32 iNumNanoTicks;
		MBtraceCpuUsageObserver* iObserver;
		TBtraceTickCount iLastSwitchedOut;
		TBool iEverScheduled;
		TBtraceNotificationPersistence iPersistence;
		};
private:
	void HandleContextSwitchL(const TBtraceTickCount& aTickCount, const TBtraceThreadId& aNewBtraceThreadId);
	void TestCpuUsagePeriodL(const TBtraceTickCount& aTickCount);
	void TestThreadIdlenessL(const TBtraceTickCount& aTickCount);
private:
	CBtraceReader& iReader;
	CBtraceContext& iContext;
	RArray<RCpuUsageNotif> iCpuUsageNotifs;
	RArray<TContextSwitchNotif> iContextSwitchNotifs;
	RArray<TThreadIdleNotif> iThreadIdleNotifs;
	TBtraceThreadId iLastBtractThreadId;
	TInt iNumIgnoredFrames;
	TInt iNumProcessedFrames;
	TInt iNumRelevantFrames;
	};


class MBtraceKeyPressObserver
	{
public:
	virtual void HandleKeyPressL(const TBtraceTickCount& aTickCount, const TKeyEvent& aKey) = 0;
	virtual void HandlePointerPressL(const TBtraceTickCount& aTickCount, const TPointerEvent& aPointer) = 0;
	virtual void HandleUnclassifiedDataL(const TBtraceTickCount& aTickCount, const TInt aCommandClass, const TInt aOpcode, const TAny* aData) = 0;
	virtual void HandleRawKeyL(const TBtraceTickCount& aTickCount, TBool aKeyUp, TInt aScanCode) = 0;
	};

NONSHARABLE_CLASS(CBtraceKeyPress) : public CRefCountedObject, public MBtraceObserver
	{
public:
	IMPORT_C static CBtraceKeyPress* NewL(CBtraceReader& aReader);
	IMPORT_C ~CBtraceKeyPress();
	IMPORT_C void NotifyKeyEventL(MBtraceKeyPressObserver& aObserver);
	IMPORT_C void NotifyKeyEventL(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyKeyEvent(MBtraceKeyPressObserver& aObserver);
	IMPORT_C void NotifyPointerEventL(MBtraceKeyPressObserver& aObserver);
	IMPORT_C void NotifyPointerEventL(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyPointerEvent(MBtraceKeyPressObserver& aObserver);
	IMPORT_C void NotifyUnclassifiedDataL(MBtraceKeyPressObserver& aObserver);
	IMPORT_C void NotifyUnclassifiedDataL(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyUnclassifiedData(MBtraceKeyPressObserver& aObserver);
	IMPORT_C void NotifyRawKeyEventL(MBtraceKeyPressObserver& aObserver);
	IMPORT_C void NotifyRawKeyEventL(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyRawKeyEvent(MBtraceKeyPressObserver& aObserver);
private:
	CBtraceKeyPress(CBtraceReader& aReader);
	void ConstructL();
	void SeenKeyL(const TBtraceTickCount& aTickCount, const TKeyEvent& aKey);
	void SeenPointerL(const TBtraceTickCount& aTickCount, const TPointerEvent& aKey);
	void SeenUnclassifiedL(const TBtraceTickCount& aTickCount, const TInt aCommandClass, const TInt aOpcode, const TAny* aData);
	void SeenRawKeyL(const TBtraceTickCount& aTickCount, TBool aKeyUp, TInt aScanCode);
private: // From MBtraceObserver.
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
private:		
	class TKeyPressNotif
		{
	public:
		TKeyPressNotif(MBtraceKeyPressObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	public:
		MBtraceKeyPressObserver* iObserver;
		TBtraceNotificationPersistence iPersistence;
		};
	CBtraceReader& iReader;
	RArray<TKeyPressNotif> iKeyNotifs;
	RArray<TKeyPressNotif> iPointerNotifs;
	RArray<TKeyPressNotif> iGenericNotifs;
	RArray<TKeyPressNotif> iRawKeyNotifs;
	};


class MBtraceTextOnScreenObserver
	{
public:
	virtual void HandleTextSeenL(const TBtraceTickCount& aTickCount, const TDesC& aText, TInt aWindowGroupId) = 0;
	};

NONSHARABLE_CLASS(CBtraceTextOnScreen) : public CRefCountedObject, public MBtraceObserver
	{
public:
	enum TMatchMode
		{
		EWildNonSpanning,
		ESpanningNonWild
		};
public:
	IMPORT_C static CBtraceTextOnScreen* NewL(CBtraceReader& aReader);
	IMPORT_C ~CBtraceTextOnScreen();
	IMPORT_C void NotifyTextOnScreenL(const TDesC& aText, MBtraceTextOnScreenObserver& aObserver);
	IMPORT_C void NotifyTextOnScreenL(const TDesC& aText, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void NotifyTextOnScreenL(const TDesC& aText, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void NotifyTextOnScreenL(const TDesC& aText, TInt aWindowGroupId, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver);
	IMPORT_C void NotifyTextOnScreenL(const TDesC& aText, TInt aWindowGroupId, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyTextOnScreen(MBtraceTextOnScreenObserver& aObserver);
private:
	CBtraceTextOnScreen(CBtraceReader& aReader);
	void ConstructL();
	void SeenL(const TBtraceTickCount& aTickCount, const TDesC& aData, TInt aWindowGroupId);
private: // From MBtraceObserver.
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
private:
	class TTextOnScreenNotif
		{
	public:
		TTextOnScreenNotif(const TDesC& aText, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence);
		TTextOnScreenNotif(const TDesC& aText, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence);
		TTextOnScreenNotif(const TDesC& aText, TInt aWindowGroupId, TMatchMode aMatchMode, MBtraceTextOnScreenObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	public:
		TPtrC iTxtPtr;
		TInt iWindowGroupId;
		TInt iMatchedChars;
		TMatchMode iMatchMode;
		MBtraceTextOnScreenObserver* iObserver;
		TBtraceNotificationPersistence iPersistence;
		};
	CBtraceReader& iReader;
	RArray<TTextOnScreenNotif> iScreenNotifs;
	};


//
// An observer to identify the occurrence of generic events (see btrace_parser_defs.h)
//
class MBtraceGenericObserver
	{
public:
	virtual void HandleGenericEvent0L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory) = 0;
	virtual void HandleGenericEvent1L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory, TUint32 aData1) = 0;
	virtual void HandleGenericEvent2L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory, TUint32 aData1, TUint32 aData2) = 0;
	virtual void HandleGenericEvent3L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory, TUint32 aData1, TUint32 aData2, TUint32 aData3) = 0;
	};

NONSHARABLE_CLASS(CBtraceGeneric) : public CRefCountedObject, public MBtraceObserver
	{
public:
	IMPORT_C static CBtraceGeneric* NewL(CBtraceReader& aReader);
	IMPORT_C ~CBtraceGeneric();
	IMPORT_C void NotifyGenericEventL(MBtraceGenericObserver& aObserver);
private:
	CBtraceGeneric(CBtraceReader& aReader);
	void ConstructL();
	// MBtraceObserver pure virtuals
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
	void SeenGeneric0L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory);
	void SeenGeneric1L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory,	TUint32 aData1);
	void SeenGeneric2L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory,	TUint32 aData1, TUint32 aData2);
	void SeenGeneric3L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory,	TUint32 aData1, TUint32 aData2, TUint32 aData3);
private:
	CBtraceReader& iReader;
		 //TODO TBtraceNotificationPersistence iPersistence
	RPointerArray<MBtraceGenericObserver> iObservers;		// pointers not owned
	};


class MBtraceDomainEventObserver
	{
public:
	virtual void HandleDomainEventL(const TBtraceTickCount& aTickCount, MBtraceDomainEventObserver& aObserver) = 0;
	};

NONSHARABLE_CLASS(CBtraceDomainEvent) : public CRefCountedObject, public MBtraceObserver
	{
public:
	IMPORT_C static CBtraceDomainEvent* NewL(CBtraceReader& aReader);
	IMPORT_C ~CBtraceDomainEvent();
	IMPORT_C void NotifyDomainEventL(TUint aSubCategory, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void NotifyDomainEventL(TUint aSubCategory, TUint aData1, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void NotifyDomainEventL(TUint aSubCategory, TUint aData1, TUint aData2, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void NotifyDomainEventL(TUint aSubCategory, TUint aData1, TUint aData2, TUint aData3, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyDomainEvent(MBtraceDomainEventObserver& aObserver);
private:
	CBtraceDomainEvent(CBtraceReader& aReader);
	void ConstructL();
private: // From MBtraceObserver.
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
private:
	class TDomainEventNotif
		{
	public:
		TDomainEventNotif(TUint aSubCategory);
		TDomainEventNotif(TUint aSubCategory, MBtraceDomainEventObserver& aObserver, TBtraceNotificationPersistence aPersistence);
		void AddDataItem(TUint aData);
		TBool operator==(const TDomainEventNotif& aNotif) const;
	public:
		enum { KMaxNumDataItems = 3 };
	public:
		TUint iSubCategory;
		MBtraceDomainEventObserver* iObserver;
		TBtraceNotificationPersistence iPersistence;
		TInt iDataCount;
		TFixedArray<TUint, KMaxNumDataItems> iData;
		};
private:
	CBtraceReader& iReader;
	RArray<TDomainEventNotif> iNotifs;
	};


class MBtraceAppResponseObserver
	{
public:
	virtual void HandleAppResponseSeenL(const TBtraceTickCount& aTickCount) = 0;
	};

NONSHARABLE_CLASS(CBtraceAppResponse) : public CRefCountedObject, public MBtraceObserver
	{
public:
	IMPORT_C static CBtraceAppResponse* NewL(CBtraceReader& aReader);
	IMPORT_C ~CBtraceAppResponse();
	IMPORT_C void NotifyAppResponseL(MBtraceAppResponseObserver& aObserver, const TProcessId& aProcessId);
	IMPORT_C void NotifyAppResponseL(MBtraceAppResponseObserver& aObserver, const TProcessId& aProcessId, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyAppResponse(MBtraceAppResponseObserver& aObserver);
private:
	CBtraceAppResponse(CBtraceReader& aReader);
	void ConstructL();
	void SeenAppResponseL(const TBtraceTickCount& aTickCount, const TProcessId& aProcessId);
private: // From MBtraceObserver.
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
private:
	class TAppResponseNotif
		{
	public:
		TAppResponseNotif(MBtraceAppResponseObserver& aObserver, const TProcessId& aProcessId, TBtraceNotificationPersistence aPersistence);
	public:
		MBtraceAppResponseObserver& iObserver;
		TProcessId iProcessId;
		TBtraceNotificationPersistence iPersistence;
		};
private:
	CBtraceReader& iReader;
	RArray<TAppResponseNotif> iNotifs;
	};


class MBtraceScreenUpdateObserver
	{
public:
	virtual void HandleScreenUpdateL(const TBtraceTickCount& aTickCount, TInt aScreenNumber, const TRect& aRect) = 0;
	};

NONSHARABLE_CLASS(CBtraceScreenUpdate) : public CRefCountedObject, public MBtraceObserver
	{
public:
	IMPORT_C static CBtraceScreenUpdate* NewL(CBtraceReader& aReader);
	IMPORT_C ~CBtraceScreenUpdate();
	IMPORT_C void NotifyScreenUpdateL(MBtraceScreenUpdateObserver& aObserver);
	IMPORT_C void NotifyScreenUpdateL(MBtraceScreenUpdateObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void NotifyScreenUpdateL(TInt aScreenNumber, const TRect& aRect, MBtraceScreenUpdateObserver& aObserver);
	IMPORT_C void NotifyScreenUpdateL(TInt aScreenNumber, const TRect& aRect, MBtraceScreenUpdateObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyScreenUpdate(MBtraceScreenUpdateObserver& aObserver);
private:
	CBtraceScreenUpdate(CBtraceReader& aReader);
	void ConstructL();
	void HandleScreenUpdateL(const TBtraceTickCount& aTickCount, TInt aScreenNumber, const TRect& aRect);
private: // From MBtraceObserver.
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
private:
	class TScreenUpdateNotif
		{
	public:
		TScreenUpdateNotif(MBtraceScreenUpdateObserver& aObserver, TBtraceNotificationPersistence aPersistence);
		TScreenUpdateNotif(TInt aScreenNumber, const TRect& aRect, MBtraceScreenUpdateObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	public:
		TBool iNotifyAll;
		TInt iScreenNumber;
		TRect iRect;
		MBtraceScreenUpdateObserver* iObserver;
		TBtraceNotificationPersistence iPersistence;
		};
private:
	CBtraceReader& iReader;
	RArray<TScreenUpdateNotif> iNotifs;
	};

class MBtracePubSubObserver
	{
public:
	virtual void HandlePropertyChangedL(const TBtraceTickCount& aTickCount, TUint aCategory, TUint aKey, TInt aNewValue) = 0;
	virtual void HandlePropertyChangedL(const TBtraceTickCount& aTickCount, TUint aCategory, TUint aKey, const TDesC8& aNewValue) = 0;
	};

NONSHARABLE_CLASS(CBtracePubSub) : public CRefCountedObject, public MBtraceObserver
	{
public:
	IMPORT_C static CBtracePubSub* NewL(CBtraceReader& aReader);
	IMPORT_C ~CBtracePubSub();
	IMPORT_C void NotifyPropertyChangedL(MBtracePubSubObserver& aObserver);
	IMPORT_C void NotifyPropertyChangedL(TUint* aCategory, TUint* aKey, MBtracePubSubObserver& aObserver, TBtraceNotificationPersistence aPersistence);		// Note, the pointer parameters are optional.
	IMPORT_C void NotifyIntegerPropertyChangedL(TUint* aCategory, TUint* aKey, TInt* aValue, MBtracePubSubObserver& aObserver, TBtraceNotificationPersistence aPersistence);		// Note, the pointer parameters are optional.
	IMPORT_C void NotifyDataPropertyChangedL(TUint* aCategory, TUint* aKey, const TDesC8* aValue, MBtracePubSubObserver& aObserver, TBtraceNotificationPersistence aPersistence);	// Note, the pointer parameters are optional.
	IMPORT_C void CancelNotifyPropertyChanged(MBtracePubSubObserver& aObserver);
private:
	CBtracePubSub(CBtraceReader& aReader);
	void ConstructL();
private: // From MBtraceObserver.
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
private:
	class TPubSubNotif
		{
	public:
		TPubSubNotif(TUint* aCategory, TUint* aKey, TInt* aIntegerValue, const TDesC8* aDataValue, MBtracePubSubObserver& aObserver, TBtraceNotificationPersistence aPersistence);
		TBool Matches(TUint aCategory, TUint aKey, TInt aIntegerValue) const;
		TBool Matches(TUint aCategory, TUint aKey, const TDesC8& aDataValue) const;
	public:
		TUint* iCategory;
		TUint* iKey;
		TInt* iIntegerValue;
		const TDesC8* iDataValue;
		MBtracePubSubObserver& iObserver;
		TBtraceNotificationPersistence iPersistence;
		};
private:
	CBtraceReader& iReader;
	RArray<TPubSubNotif> iNotifs;
	};

class MBtraceFocusChangeObserver
	{
public:
	virtual void HandleFocusChangedL(const TBtraceTickCount& aTickCount, const TBtraceWindowGroupId& aWindowGroupId) = 0;
	};

NONSHARABLE_CLASS(CBtraceFocusChange) : public CRefCountedObject, public MBtraceObserver
	{
public:
	IMPORT_C static CBtraceFocusChange* NewL(CBtraceReader& aReader, CBtraceContext& aContext);
	IMPORT_C ~CBtraceFocusChange();
	IMPORT_C void NotifyFocusChangedL(const TDesC& aWindowGroupNamePattern, MBtraceFocusChangeObserver& aObserver);
	IMPORT_C void NotifyFocusChangedL(const TDesC& aWindowGroupNamePattern, MBtraceFocusChangeObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	IMPORT_C void CancelNotifyFocusChanged(MBtraceFocusChangeObserver& aObserver);
private:
	CBtraceFocusChange(CBtraceReader& aReader, CBtraceContext& aContext);
	void ConstructL();
private: // From MBtraceObserver.
	virtual void HandleBtraceFrameL(const TBtraceFrame& aFrame);
private:
	class TFocusChangeNotif
		{
	public:
		TFocusChangeNotif(const TDesC& aWindowGroupNamePattern, MBtraceFocusChangeObserver& aObserver, TBtraceNotificationPersistence aPersistence);
	public:
		TPtrC iWindowGroupNamePattern;
		MBtraceFocusChangeObserver& iObserver;
		TBtraceNotificationPersistence iPersistence;
		};
private:
	CBtraceReader& iReader;
	CBtraceContext& iContext;
	RArray<TFocusChangeNotif> iNotifs;
	};


#endif // __BTRACE_PARSER_H__
