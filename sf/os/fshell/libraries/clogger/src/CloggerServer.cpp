// CloggerServer.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "CloggerServer.h"
#include "Clogger.h"
#include "cliserv.h"
#include "common.h"
#include <HAL.h>
#include <centralrepository.h>
#include <s32mem.h>
#include "SessionWriter.h"
#include <fshell/common.mmh>

#define ClientPanic(aMsg) PanicClient(aMsg, __LINE__)
static const TUint32 KAllEnabled = 0xFFFFFFFFu;

enum TFlags {
	EAllowCurrentBufferToBeRead,
	EInMiddleOfLog, // so don't call Log again, you'll stamp all over the buffers! This is a debug check to guard against my carelessness, ie calling something from within Log() that tries to call Log() again. Log() is not reenterant!
	ELogBufferOverflow, // Can't log when the overflow occurs, so remember it until the log is in an ok state to record the fact
	EWrittenToLog, // This indicates we have got far enough through construction to have written stuff to the buffers. This is an explicit flag to guard against future changes to the startup ordering
	ECenrepFailed, // Can't log when cenrep is accessed, so remember for when we can log it
	ENeedToCopyCompressedLog, // Indicates we need to call CopyLogToExternalMediaL when the background compress of the log completes
	EForceBreakpoint, // Forces a __DEBUGGER call at the start of DoServiceL(). Useful when debugging tclog
	EUsingTempBuf, // Another check to make sure I don't do something stupid with my buffers
	EMaxFlags
};

__ASSERT_COMPILE(EMaxFlags <= 32);

#define ACQUIRE_FLAG_LOCK_OR_FAIL(aFlag, failureOperation) \
	ASSERT(!iFlags.IsSet(aFlag)); /* Debug behaviour */ \
	if (iFlags.IsSet(aFlag)) { failureOperation; } /* Non debug behaviour - fail gracefully */ \
	iFlags.Set(aFlag);
#define ACQUIRE_FLAG_LOCK(aFlag) ACQUIRE_FLAG_LOCK_OR_FAIL(aFlag, return)
#define RELEASE_FLAG_LOCK(aFlag) iFlags.Clear(aFlag)

enum TInternalLoggingMasks {
	ELogTagChanges = 1,
	ELogNewTags = 2,
	ELogDisks = 4,
	ELogNewRdebuggers = 8,
	};

enum TRDebugPrintMasks {
	ELogRDebugPrint = 1,
	ELogKernPrintf = 2,
	ELogPlatSecDiagnostics = 4,
	};

_LIT8(KCloggerTag, "Clogger");
_LIT8(KOldRDebugTag, "RDebug::Print");
_LIT8(KRDebugTag, "RDebug");
_LIT8(KKernPrintfTag, "Kern::Printf"); // Not used any more - keep the definition because we explicitly ignore it when loading tags from cenrep
_LIT8(KOldFallbackTag, "**Everything else**");
_LIT8(KFallbackTag, "DefaultForNewTags");
_LIT(KLogDir, "\\logs\\"); // Don't include the C: because that would upset moap. Rely on RFs to default to the system drive
_LIT(KLogFile, "clogger.txt");

// Don't reorder these without also changing order they are constructed in CCloggerServer::ConstructL
enum TWriters {
	EFile = 0,
	ERDebug,
	EMessageQueue,
	ESessionWriter,
	// Add new writers here!
	EMaxWriters
	};

enum TCenRep {
	EGlobalOptions = 0,
	EBufferSize,
	ENumBuffers,
	ENumRotates,
	ERotateBehaviour,
	EEnabledTags, // This is obsolete because we now use the partial key functionality of cenrep to store enabled tags. And that was necessary because cenrep limits binary values to 2048 bytes
	ERepositoryVersion,
	};

// Cenrep constants
const TUint32 KEverythingMask = 0xFF000000;
const TUint32 KEverythingPartialKey = 0x80000000;
const TUint32 KEnabledMask = 0xFF000001;
const TUint32 KTagEnabledPartialKey = 0x80000000;
enum TCenrepVersion
	{
	ECenrepOriginalFormat = 0,
	ECenrepThreadNamesArePrettified = 1,
	};

const TInt KMinWriteSize = 512; // 512 bytes is a good minimum size for efficient writes to disk
const TInt KSyncBufferIdx = -99; // Magic number given by the server to writers to mean 'the sync buffer'
const TInt KCompressFileNameBufferIdx = -100; // Magic number given by the server to the compress sync writer, its meaning is internal to CLogCompressor and CCloggerServer

/*
Clogger AOs:

EPriorityHigh + 1:
CBtIncomingPrompt Starter - This shouldn't get starved because of writers

EPriorityHigh:
CLogWriter - emptying buffers
CSyncWriterWrapper - emptying buffers
CLogsDirWatcher - will never be running at same time as CLogWriter
CSessionWriterServer - emptying buffers

EPriorityStandard + 1:
CDebugRouterClient - reads input from RDdebug::Print redirector

EPriorityStandard:
CCloggerServer
CServerCallbackDispatcher - unused

EPriorityLow:
iIdleWriteTimer - only go to next buffer when nothing better to do
*/


class CLogsDirWatcher : public CActive
	{
public:
	CLogsDirWatcher(RFs& aFs, CCloggerServer& aServer)
		: CActive(EPriorityHigh), iFs(aFs), iServer(aServer)
		, iDirToWatch(KLogDir().Left(KLogDir().Length()-1)) // Strip trailing backslash
		{

		CActiveScheduler::Add(this);	
		//__DEBUGGER();
		iFs.NotifyChange(ENotifyAll, iStatus, iDirToWatch);
		SetActive();
		}

	void DoCancel()
		{
		iFs.NotifyChangeCancel();
		}

	~CLogsDirWatcher()
		{
		Cancel();
		}

	void RunL()
		{
		//__DEBUGGER();
		if (iStatus != KErrNone) return; // Give up
		TEntry entry;
		if (iFs.Entry(KLogDir, entry) == KErrNone)
			{
			iServer.LogsDirHasBeenCreated();
			}
		else
			{
			// Keep listening
			iFs.NotifyChange(ENotifyAll, iStatus, iDirToWatch);
			SetActive();
			}	
		}

private:
	RFs& iFs;
	CCloggerServer& iServer;
	TPtrC iDirToWatch;
	};

CSession2* CCloggerServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	return new(ELeave) CCloggerSession();
	}

CCloggerServer::CCloggerServer()
: CSensibleServer()
	{
	}

TInt CCloggerServer::TransientServerShutdownTime() const
	{
	return 0; // Never shutdown
	}

CCloggerServer::~CCloggerServer()
	{
	//__DEBUGGER();
	if (iFlags.IsSet(EWrittenToLog)) FlushBuffers();
	CloseBuffers();
	iBufs.Close();
	iChunkForBufs.Close();

	for (TInt i = 0; i < iWriters.Count(); i++)
		{
		iWriters[i]->CloseWriter();
		}
	iWriters.Close();
	delete iSessionWriterServer;

	if (iCompressor)
		{
		iCompressor->Cancel(); // This will wait for the compress to complete. Safest thing to do
		delete iCompressor;
		}

	delete iLogsDirWatcher;
	iLogFile.Close();
	iFs.Close();
	//iFileBeingRotated.Close();

	TPtrHashMapIter<TDesC8, TTagData> tagIter(iTags);
	TTagData* tag;
	while ((tag = const_cast<TTagData*>(tagIter.NextValue())) != NULL)
		{
		tagIter.RemoveCurrent();
		delete tag->iTagName;
		delete tag;
		}
	iTags.Close();
	iTempBuf.Close();
	iSessionTempBuf.Close();
	iRDebugTags.Close();

	delete iIdleWriteTimer;
	ASSERT(!iFlushBufferWait.IsStarted());
	delete iKernDebugRouter; // When should this be deleted?
	}

void CCloggerServer::ConstructL()
	{
	CSensibleServer::ConstructL();
	GetSettingsL();

	iWriters.ReserveL(EMaxWriters);
	// Note: important to get the order of writers in the array to be the same as is defined in enum TWriters
	CLogWriter* filelogger = new(ELeave) CLogWriter(*this, iLogFile);
	filelogger->SetEnabled(ETrue);
	iWriters.Append(filelogger);
	
	CRDebugWriter* rdebuglogger = new(ELeave) CRDebugWriter(*this);
	CleanupStack::PushL(rdebuglogger);
	CSyncWriterWrapper* rdebugWrapper = CSyncWriterWrapper::NewL(*this, *rdebuglogger, iWriters.Count());
	CleanupStack::Pop(rdebuglogger);
	rdebugWrapper->SetEnabled(EFalse);
	iWriters.Append(rdebugWrapper);

	CMessageQueueWriter* msgq = CMessageQueueWriter::NewL();
	CleanupStack::PushL(msgq);
	CSyncWriterWrapper* msgqWrapper = CSyncWriterWrapper::NewL(*this, *msgq, iWriters.Count());
	CleanupStack::Pop(msgq);
	rdebugWrapper->SetEnabled(EFalse);
	iWriters.Append(msgqWrapper);

	CSessionWriter* sessionWriter = new(ELeave) CSessionWriter(*this);
	sessionWriter->SetEnabled(EFalse);
	iWriters.Append(sessionWriter);

	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.SetSessionPath(KLogDir));

	TInt err = OpenLogFile();
	if (err == KErrPathNotFound)
		{
		err = KErrNone;
		iLogsDirWatcher = new(ELeave) CLogsDirWatcher(iFs, *this);
		}
	User::LeaveIfError(err);
	/*TInt err = iLogFile.Open(iFs, KLogFile, EFileWrite|EFileShareAny|EFileStream);
	if (err == KErrNotFound) err = iLogFile.Create(iFs, KLogFile, EFileWrite|EFileShareAny|EFileStream);
	if (err == KErrPathNotFound)
		{
		iWriters[EFile]->SetEnabled(EFalse);
		err = KErrNone;
		iLogsDirWatcher = new(ELeave) CLogsDirWatcher(iFs, *this);
		}
	else
		{
		User::LeaveIfError(err);
		TInt seekTo = -1;
		User::LeaveIfError(iLogFile.Seek(ESeekEnd, seekTo));
		}
	*/
	iIdleWriteTimer = CPeriodic::NewL(CActive::EPriorityLow);
	
	// Important that we start the kern debug router early, before the first call to UpdateBufferSizeL because we now rely on the debug router to allocate our chunk for us
	TRAPD(debugRouterErr, iKernDebugRouter = CDebugRouterClient::NewL(*this)); // We check 'debugRouterErr' further down
	const TInt KMaxChunkSize = 512*1024; // We will say 512 KB for max chunk size
	err = CreateKernChunkForClient(NULL, KMaxChunkSize, 0, iChunkForBufs);
	if (err == KErrNotSupported)
		{
		// Then debug router not found - so just create a chunk ourselves
		_LIT(KName, "CloggerBufferChunk");
		err = iChunkForBufs.CreateGlobal(KName, 0, KMaxChunkSize);
		}
	User::LeaveIfError(err);

	UpdateBufferSizeL(iBufferSize, iNumBuffers);
	iTempBuf.CreateL(512);
	iSessionTempBuf.CreateL(512);

	// The mechanism to compress log files in the background is a special case of a CSyncWriterWrapper
	CLogCompressor* compressor = CLogCompressor::NewLC(*this);
	iCompressor = CSyncWriterWrapper::NewL(*this, *compressor, -1);
	CleanupStack::Pop(compressor);
	iCompressor->SetEnabled(ETrue); // Otherwise CSyncWriterWrapper will ignore our requests to compress things!

	iTags.ReserveL(32);
	// Construct some tags that always exist
	iCloggerTag = iTags.Find(KCloggerTag);
	if (!iCloggerTag)
		{
		HBufC8* tag = KCloggerTag().AllocLC();
		iCloggerTag = reinterpret_cast<TTagData*>(NewSessionForTagL(tag));
		iCloggerTag->SetShouldDisplay();
		iCloggerTag->iEnabled = ELogNewRdebuggers; // Defaults to off, except for logging new rdebuggers - because you generally want to be able to see the thread name
		CleanupStack::Pop(tag);
		}
	iRDebugTag = iTags.Find(KRDebugTag);
	if (!iRDebugTag)
		{
		HBufC8* tag = KRDebugTag().AllocLC();
		iRDebugTag = reinterpret_cast<TTagData*>(NewSessionForTagL(tag));
		// Should not be shown unless used, so don't SetShouldDisplay
		CleanupStack::Pop(tag);
		}
	iFallbackTag = iTags.Find(KFallbackTag);
	if (!iFallbackTag)
		{
		HBufC8* tag = KFallbackTag().AllocLC();
		iFallbackTag = reinterpret_cast<TTagData*>(NewSessionForTagL(tag));
		iFallbackTag->SetShouldDisplay();
		CleanupStack::Pop(tag);
		}

	TUint32 tickCount = User::NTickCount();
	iTimeAtStartup.UniversalTime();
	TInt tickPeriod;
	//User::LeaveIfError(HAL::Get(HAL::EFastCounterFrequency, iTickFreq));
	User::LeaveIfError(HAL::Get(HAL::ENanoTickPeriod, tickPeriod));
	iTickFreq = 1000000 / tickPeriod; // We work in frequencies because they are the round numbers when using the fast counter, and at some point we might want to again

	iStartupTickInMicroseconds = ((TInt64)tickCount * 1000000) / (TInt64)iTickFreq; // Just making damn sure we're using 64bit math

	// Rotate uses the tick stuff so must come after it
	if (iRotateBehaviour & RClogger::EAutoRotateAtStartup)
		{
		Rotate();
		}

	UpdatedGlobalOptionsL(iOptions & RClogger::EBufferLog); // Passing in the current value of EBufferLog means we don't trigger another UpdateBufferSizeL, which is good because we've explicitly called that earlier on in this function

	iFlags.Set(EWrittenToLog);

	_LIT8(KCrLf, "\r\n");
	_LIT8(KStarted, "Server started (log format v4)");
	// Format v1 used incorrect time stamps, v2 fixed that, v3 indicates that the buffer info is published via P&S
	// v4 scraps P&S and uses the kern debug router to map the buffers into the crashlog

	LogLine(KCrLf);
	Log(NULL, KStarted, User::NTickCount());
	if (iFlags.IsSet(ECenrepFailed))
		{
		_LIT8(KCenrep, "Couldn't access the cenrep repository.");
		LogError(KCenrep);
		}
	if (debugRouterErr)
		{
		_LIT8(KDebugRouterFailed, "Failed to load the debug router LDD, err=%i.");
		LogError(KDebugRouterFailed, debugRouterErr);
		}

	_LIT_SECURITY_POLICY_PASS(KRead);
	_LIT_SECURITY_POLICY_S0(KWrite, KCloggerUid.iUid);

	RProperty::Define(KCloggerUid, ESequenceNumber, RProperty::EInt, KRead, KWrite);
	RProperty::Set(KCloggerUid, ESequenceNumber, iEnabledStatesSequenceNumber);
	}

void CCloggerServer::GetSettingsL()
	{
	TRAPD(err, DoGetSettingsL());
	// Don't allow messed-up cenrep config to prevent us from launching

	if (err)
		{
		iBufferSize = 4096;
		iNumBuffers = 2;
		iNumRotates = 4;
		iFlags.Set(ECenrepFailed);
		}
	}

void CCloggerServer::DoGetSettingsL()
	{
	CRepository* cenrep = CRepository::NewLC(KCloggerUid);
	User::LeaveIfError(cenrep->Get(EGlobalOptions, (TInt&)iOptions));
	User::LeaveIfError(cenrep->Get(EBufferSize, iBufferSize));
	User::LeaveIfError(cenrep->Get(ENumBuffers, iNumBuffers));
	User::LeaveIfError(cenrep->Get(ENumRotates, iNumRotates));
	User::LeaveIfError(cenrep->Get(ERotateBehaviour, (TInt&)iRotateBehaviour));
	TInt version = 0;
	TBool needToPrettifyThreadNames = EFalse;
	TInt err = cenrep->Get(ERepositoryVersion, version);
	if (err || version < ECenrepThreadNamesArePrettified)
		{
		needToPrettifyThreadNames = ETrue;
		}

	RArray<TUint32> foundKeys;
	CleanupClosePushL(foundKeys);
	err = cenrep->FindL(KTagEnabledPartialKey, KEnabledMask, foundKeys);
	if (err == KErrNone)
		{
		TInt n = foundKeys.Count();
		TBuf8<1> dummyBuf;
		for (TInt i = 0; i < n; i++)
			{
			TUint32 enabled;
			User::LeaveIfError(cenrep->Get(foundKeys[i], (TInt&)enabled));
			TInt len;
			HBufC8* tagName = NULL;
			TInt err = cenrep->Get(foundKeys[i]+1, dummyBuf, len);
			if (err == KErrNone)
				{
				tagName = dummyBuf.AllocLC();
				}
			else if (err == KErrOverflow)
				{
				tagName = HBufC8::NewLC(len);
				}
			else
				{
				User::LeaveIfError(err);
				}
			TPtr8 ptr = tagName->Des();
			User::LeaveIfError(cenrep->Get(foundKeys[i]+1, ptr, len));
			if (needToPrettifyThreadNames)
				{
				ThreadPrettyName(ptr);
				}

			if (*tagName == KKernPrintfTag)
				{
				// We don't use this any more
				CleanupStack::PopAndDestroy(tagName);
				continue;
				}
			else if (*tagName == KOldFallbackTag)
				{
				// In case the old definition is kicking round in a cenrep file
				*tagName = KFallbackTag;
				}
			else if (*tagName == KOldRDebugTag)
				{
				// We've renamed this too
				*tagName = KRDebugTag;
				}

			TTagData* tagData = iTags.Find(*tagName);
			if (tagData)
				{
				// tagName is not needed
				CleanupStack::PopAndDestroy(tagName);
				}
			else
				{
				tagData = new(ELeave)TTagData(tagName);
				if (*tagName == KRDebugTag)
					{
					// We don't want this tag to show up unless we've actually used it, since it's only used for last-ditch OOM logging (this is to minimise confusion about which tags are used for what)
					}
				else
					{
					tagData->SetShouldDisplay(); // If the tag comes from cenrep then we should show it
					}
				CleanupStack::PushL(tagData);
				iTags.InsertL(tagName, tagData);
				CleanupStack::Pop(tagData);
				CleanupStack::Pop(tagName);
				}
			tagData->iEnabled = enabled;
			}
		}
	CleanupStack::PopAndDestroy(&foundKeys);

	/*
	TInt descSize = 0;
	TBuf8<1> dummyBuf; // Don't bother trying to succeed without knowing the buffer size
	TInt err = cenrep->Get(EEnabledTags, dummyBuf, descSize);
	if (err == KErrOverflow)
		{
		// If err is not found, we don't care (repository key hasn't been defined) so do nothing
		RBuf8 buf;
		CleanupClosePushL(buf);
		buf.CreateL(descSize);
		err = cenrep->Get(EEnabledTags, buf, descSize);
		User::LeaveIfError(err); // Shouldn't happen

		CBufFlat* cbuf = CBufFlat::NewL(buf.Length());
		CleanupStack::PushL(cbuf);
		cbuf->InsertL(0, buf);
		RBufReadStream stream(*cbuf);
		CleanupClosePushL(stream);

		TInt count = stream.ReadInt32L();
		while (count--)
			{
			TUint32 enabled = stream.ReadUint32L();
			HBufC8* tagName = HBufC8::NewL(stream, 2000);
			TTagData* tagData = iTags.Find(*tagName);
			if (tagData)
				{
				delete tagName; // Not needed
				}
			else
				{
				tagData = new(ELeave)TTagData();
				tagData->iTagName = tagName;
				tagData->iRefCount = 0;
				CleanupStack::PushL(tagData);
				iTags.InsertL(tagName, tagData); // TODO hmm if this leaves, tagName will be leaked
				CleanupStack::Pop(tagData);
				}
			tagData->iEnabled = enabled;
			}
		CleanupStack::PopAndDestroy(3, &buf); // stream, cbuf, buf
		}
	*/
	CleanupStack::PopAndDestroy(cenrep);
	}

void CCloggerServer::PersistSettingsL()
	{
	CRepository* cenrep = CRepository::NewLC(KCloggerUid);
	User::LeaveIfError(cenrep->Set(EGlobalOptions, (TInt&)iOptions));
	User::LeaveIfError(cenrep->Set(EBufferSize, iBufferSize));
	User::LeaveIfError(cenrep->Set(ENumBuffers, iNumBuffers));
	User::LeaveIfError(cenrep->Set(ENumRotates, iNumRotates));
	User::LeaveIfError(cenrep->Set(ERotateBehaviour, (TInt&)iRotateBehaviour));
	User::LeaveIfError(cenrep->Set(ERepositoryVersion, ECenrepThreadNamesArePrettified));

	User::LeaveIfError(cenrep->StartTransaction(CRepository::EReadWriteTransaction));
	cenrep->CleanupCancelTransactionPushL();
	TUint32 dontCare;
	TInt err = cenrep->Delete(KEverythingPartialKey, KEverythingMask, dontCare);
	if (err && err != KErrNotFound) User::Leave(err);
	TUint32 key = KTagEnabledPartialKey;
	TPtrHashMapIter<TDesC8, TTagData> tagIter(iTags);
	const TTagData* tag;
	while ((tag = tagIter.NextValue()) != NULL)
		{
		User::LeaveIfError(cenrep->Set(key, (TInt&)tag->iEnabled));
		key++;
		User::LeaveIfError(cenrep->Set(key, *tag->iTagName));
		key++;
		}
	CleanupStack::Pop(); // Transaction
	User::LeaveIfError(cenrep->CommitTransaction(dontCare));
	CleanupStack::PopAndDestroy(cenrep);
	}

void CCloggerServer::ResetSettingsL()
	{
	CRepository* cenrep = CRepository::NewLC(KCloggerUid);
	cenrep->Reset();
	CleanupStack::PopAndDestroy(cenrep);
	GetSettingsL();
	}


void CCloggerServer::UpdateBufferSizeL(TInt aSize, TInt aNum)
	{
	if (aSize < KMinWriteSize) aSize = KMinWriteSize; // Anything smaller is just pointless
	if (aNum < 2) aNum = 2; // Likewise

	FlushBuffers(); // Ensure everything is empty before we start
	CloseBuffers();

	iBufferSize = aSize;
	iNumBuffers = aNum;
	if (!(iOptions & RClogger::EBufferLog))
		{
		return;
		}
	TUint oldOptions = iOptions;
	iOptions &= ~RClogger::EBufferLog; // Clear this, and set it back to oldOptions at the end, so that if we leave our flags accurately describe the state of our buffers
	
	User::LeaveIfError(AdjustBufferChunk(iBufferSize * iNumBuffers));
	TUint8* ptr = iChunkForBufs.Base();
	
	while (aNum--)
		{
		TBufEntry* entry = new(ELeave) TBufEntry();
		CleanupStack::PushL(entry);
		entry->iBuf.Set(ptr, 0, iBufferSize);
		entry->iArrayIdx = iBufs.Count();
		iBufs.AppendL(entry);
		CleanupStack::Pop(entry);
		ptr += iBufferSize;
		if (iBufs.Count() > 1) iBufs[iBufs.Count()-2]->iNext = entry;
		}
	
	iOptions = oldOptions;
	iBufs[iBufs.Count()-1]->iNext = iBufs[0];
	iCurrent = iBufs[0];
	UpdatedBuffers();
	
	if (iLogFile.SubSessionHandle())
		{
		ReCalculateFileAlignment();
		// Need to update iSubtractSizeOfNextBuffer in case there's been some unbuffered logging since the file opened
		// (Or indeed if this is the first time UpdateBuffers has been called, immediately after the file was opened)
		}
	}

void CCloggerServer::ReCalculateFileAlignment()
	{
	TInt seekTo = 0;
	/*TInt err =*/ iLogFile.Seek(ESeekCurrent, seekTo);
	// Assuming iLogFile is valid, ESeekCurrent can never return an error (the documentation is not clear but the implementation is)
	iSubtractSizeOfNextBuffer = KMinWriteSize - (seekTo % KMinWriteSize);
	}

TInt CCloggerServer::OpenLogFile()
	{
	TInt err = iLogFile.Open(iFs, KLogFile, EFileWrite|EFileShareAny|EFileStream);
	if (err == KErrNotFound) err = iLogFile.Create(iFs, KLogFile, EFileWrite|EFileShareAny|EFileStream);
	if (err == KErrNone)
		{
		TInt seekTo = -1;
		/*err =*/ iLogFile.Seek(ESeekEnd, seekTo);
		iWriters[EFile]->SetEnabled(ETrue);
		ReCalculateFileAlignment();
		}
	else
		{
		iWriters[EFile]->SetEnabled(EFalse);
		}
	
	return err;
	}

// Session

inline CCloggerServer& CCloggerSession::Server()
	{
	return *static_cast<CCloggerServer*>(const_cast<CServerBase*>(CSessionBase::Server()));
	}

CCloggerSession::CCloggerSession()
: CSensibleSession()
	{
	}


CCloggerSession::~CCloggerSession()
	{
	if (iPerformanceLoggingChunk.Handle())
		{
		TUint8* base = iPerformanceLoggingChunk.Base();
		TInt size = *(TInt*)base;
		TPtrC8 ptr(base+4, size);
		//__DEBUGGER();
		Server().LogHighPerformanceBuffer(ptr);
		iPerformanceLoggingChunk.Close();
		}
	iGetTagStatesContext.Close();
	Server().DropSession(this);
	}

TBool CCloggerSession::DoServiceL(const RMessage& aMessage)
	{
	//CleanupPanicPushL(); //DEBUG

	if (Server().ForceBreakpointRequested())
		{
		__DEBUGGER();
		}

	TBool handled = ETrue;
	TInt res = KErrNone;

	TInt func = aMessage.Function();
	if (!iContext && func != ESetTag && func != ESetTag8)
		{
		ClientPanic(aMessage); // SetTag should have been called
		return ETrue;
		}

	switch (func)
		{
		case ESetTag:
		case ESetTag8:
			{
			TInt tagLen = aMessage.GetDesLengthL(0);
			TBool twoLevel = aMessage.Ptr1() != NULL;
			if (tagLen && twoLevel)
				{
				// 2-level tag name, as used by flogger/CDU
				tagLen = tagLen + 1 + aMessage.GetDesLengthL(1); // plus 1 for the slash in "tag/subtag"
				}
			if (tagLen > 256)
				{
				// It makes it easier to guarantee correctness if we limit tag lengths. Since thread fullnames are also 256, this is a good number
				User::Leave(KErrTooBig);
				}

			HBufC8* newTag = NULL;
			if (tagLen)
				{
				newTag = HBufC8::NewMaxLC(tagLen);
				if (func == ESetTag)
					{
					HBufC* tag = HBufC::NewMaxLC(tagLen);
					TPtr ptr = tag->Des();
					aMessage.ReadL(0, ptr);
					if (twoLevel)
						{
						ptr.Append('/');
						TInt len = ptr.Length();
						TPtr16 endBit((TUint16*)ptr.Ptr()+len, 0, ptr.MaxLength() - len);
						aMessage.ReadL(1, endBit);
						ptr.SetLength(len + endBit.Length());
						}
					newTag->Des().Copy(ptr);
					CleanupStack::PopAndDestroy(tag);
					}
				else
					{
					TPtr8 ptr = newTag->Des();
					aMessage.ReadL(0, ptr);
					if (twoLevel)
						{
						// Use a different separator for the 8-bit variant (as that distinguishes commsdebug from flogger)
						ptr.Append(':');
						TInt len = ptr.Length();
						TPtr8 endBit((TUint8*)ptr.Ptr()+len, 0, ptr.MaxLength() - len);
						aMessage.ReadL(1, endBit);
						ptr.SetLength(len + endBit.Length());
						}
					}
				if (*newTag == KCloggerTag || *newTag == KFallbackTag)
					{
					// Not allowed for any session to pretend to be us
					CleanupStack::PopAndDestroy(newTag);
					newTag = NULL;
					tagLen = 0;
					// Drop through to using thread name in this case
					}
				}

			if (!tagLen)
				{
				// Default to using thread name for the tag
				RThread client;
				aMessage.ClientL(client);
				CleanupClosePushL(client);
				TFullName threadName = client.FullName();
				CleanupStack::PopAndDestroy(&client);
				newTag = HBufC8::NewLC(threadName.Length());
				TPtr8 tagPtr = newTag->Des();
				tagPtr.Copy(threadName);
				CCloggerServer::ThreadPrettyName(tagPtr);
				}

			if (iContext)
				{
				Server().ReplaceTagL(iContext, newTag);
				CleanupStack::Pop(newTag);
				}
			else
				{
				iContext = Server().NewSessionForTagL(newTag);
				CleanupStack::Pop(newTag);
				}

			TInt sequenceNumber;
			TUint32 enabled = Server().TagEnabled(iContext, &sequenceNumber);
			TPckg<TUint32> enablePkg(enabled);
			aMessage.WriteL(2, enablePkg);
			res = sequenceNumber;
			break;
			}
		case ELog8:
			{
			TUint32 enabledMask = aMessage.Int2();
			TUint32 tagEnabled = Server().TagEnabled(iContext);
			if (!(enabledMask & tagEnabled))
				{
				Server().RegisterDisabledLog(iContext); // SetShouldDisplay usually gets called when you call log, but if it's disabled, then we don't get that far...
				// Don't bother reading the log data
				break;
				}

			TInt stringLen = aMessage.GetDesLengthL(0);
			RBuf8& buf = Server().SessionTempBuf();
			if (stringLen > buf.MaxLength())
				{
				buf.ReAllocL(stringLen);
				}
			aMessage.ReadL(0, buf);
			TInt tickCount = aMessage.Int1();
			Server().Log(iContext, buf, tickCount);
			break;
			}
		case EUpdateEnabledMask:
			{
			TUint32 e = Server().TagEnabled(iContext);
			TPckg<TUint32> enabledBuf(e);
			aMessage.WriteL(0, enabledBuf);
			break;
			}
		case EHexDump:
		case EHexDump16:
			{
			TUint32 enabledMask = aMessage.Int3();
			if (!(enabledMask & Server().TagEnabled(iContext)))
				{
				// Don't bother reading the log data
				break;
				}
			
			RBuf8& header = Server().SessionTempBuf();
			TInt dataLen = aMessage.GetDesLengthL(1);

			if (func == EHexDump)
				{
				aMessage.ReadL(0, header);
				}
			else
				{
				TPtr16 widePtr((TUint16*)header.Ptr(), header.MaxLength() / 2);
				aMessage.ReadL(0, widePtr);
				header.SetLength(widePtr.Size());
				header.Collapse();
				}

			// Should we use a preallocated buffer here? For ease of coding, don't
			RBuf8 data;
			CleanupClosePushL(data);
			data.CreateL(dataLen);
			aMessage.ReadL(1, data);
			
			TInt tickCount = aMessage.Int2();
			Server().HexDumpL(iContext, header, data, tickCount);
			CleanupStack::PopAndDestroy(&data);

			break;
			}
		case EGetTagStates1:
			{
			iGetTagStatesContext.Close();
			TServerCallback cb;
			HBufC8* context;
			TCallbackWriter writer(cb, &context);
			Server().WriteCallbackForGetTagStatesL(writer);

			if (context) iGetTagStatesContext.Assign(context);

			TPckg<TServerCallback> pkg(cb);
			aMessage.WriteL(0, pkg);
			break;
			}
		case EGetTagStates2:
			{
			aMessage.WriteL(0, iGetTagStatesContext);
			iGetTagStatesContext.Close();
			break;
			}
		case ECreatePerformanceLoggingChunk:
			{
			if (iPerformanceLoggingChunk.Handle())
				{
				return KErrAlreadyExists;
				}
			RThread client;
			aMessage.ClientL(client);
			TInt size = aMessage.Int0();
			res = Server().CreateKernChunkForClient(&client, size, size, iPerformanceLoggingChunk);
			client.Close();
			break;
			}
		case ERegisterPerformanceLoggingChunk:
			{
			TInt err = iPerformanceLoggingChunk.Open(aMessage, 0, EFalse);
			User::LeaveIfError(err);
			break;
			}
		default:
			handled = EFalse;
			break;
		}
	//CleanupStack::Pop(); //DEBUG
	if (handled)
		{
		aMessage.Complete(res);
		}
	else
		{
		// Then just pass the message on to the server
		return Server().DoServiceL(aMessage);

		//DEBUG to catch unbalanced PushLs
		//TBool r = 0;
		//TRAPD(err, r = Server().DoServiceL(aMessage));
		//User::LeaveIfError(err);
		//return r;
		//END DEBUG
		}
	return handled;
	}


TBool CCloggerServer::DoServiceL(const RMessage& aMessage)
	{
	TBool handled = ETrue;
	TInt res = KErrNone;

	switch (aMessage.Function())
		{
		case EDebugAlignLogFile:
		case EDebugShutdownServer:
		case EDebugForceBreakpointInServiceL:
			{
			_LIT_SECURITY_POLICY_S0(KTClogPolicy, FSHELL_UID_TCLOG);
			User::LeaveIfError(KTClogPolicy().CheckPolicy(aMessage, "Debug API called  from not within tclog.exe"));
			}
			break;
		default:
			break;
		}

	switch (aMessage.Function())
		{
		case ESetGlobalOptions:
			{
			TUint oldOptions = iOptions;
			iOptions = aMessage.Int0();

			UpdatedGlobalOptionsL(oldOptions);
			break;
			}
		case EGetGlobalOptions:
			{
			res = iOptions;
			break;
			}
		case ESetEnabled:
			{

			ACQUIRE_FLAG_LOCK_OR_FAIL(EUsingTempBuf, break);
			iTempBuf.Zero();
			TPtr16 ptr((TUint16*)iTempBuf.Ptr(), iTempBuf.MaxLength()/2);
			TInt err = aMessage.Read(0, ptr);
			if (err)
				{
				RELEASE_FLAG_LOCK(EUsingTempBuf);
				User::Leave(err);
				}
			TPtrC8 tagname8 = ptr.Collapse();
			TTagData* tag = iTags.Find(tagname8);
			if (tag == NULL)
				{
				// Implicitly create the tag
				HBufC8* tagBuf = tagname8.AllocLC();
				tag = reinterpret_cast<TTagData*>(NewSessionForTagL(tagBuf));
				tag->SetShouldDisplay(); // The user wants to see it if they've just created it
				CleanupStack::Pop(tagBuf);
				}
			RELEASE_FLAG_LOCK(EUsingTempBuf);

			tag->iEnabled = aMessage.Int1();
			_LIT8(KTagChanged, "Client %S changed log mask of tag [%S] to 0x%08x");
			TPtrC8 safeTagName = tag->iTagName->Left(128); // LogNote doesn't check lengths
			RThread client;
			aMessage.ClientL(client);
			TFullName clientName = client.FullName();
			TPtrC8 safeClientName = clientName.Collapse().Left(128);
			client.Close();
			LogNote(ELogTagChanges, KTagChanged, &safeClientName, &safeTagName, tag->iEnabled);

			// Send update message to all sessions that have this tag and have enabled
			// EDontLogIfTagIsDisabled
			NotifySessionsOfChangedTag(tag);

			iEnabledStatesSequenceNumber++;
			RProperty::Set(KCloggerUid, ESequenceNumber, iEnabledStatesSequenceNumber);
			break;
			}
		case EIsEnabled:
			{
			ACQUIRE_FLAG_LOCK_OR_FAIL(EUsingTempBuf, break);
			iTempBuf.Zero();
			TPckgBuf<TUint32> logMask;
			TPtr16 ptr((TUint16*)iTempBuf.Ptr(), iTempBuf.MaxLength()/2);
			TInt err = aMessage.Read(0, ptr);
			if (err)
				{
				RELEASE_FLAG_LOCK(EUsingTempBuf);
				User::Leave(err);
				}
			TTagData* tag = iTags.Find(ptr.Collapse());
			RELEASE_FLAG_LOCK(EUsingTempBuf);
			if (tag)
				{
				logMask = tag->iEnabled;
				}
			else
				{
				logMask = iFallbackTag->iEnabled; // If/when it does start logging, this is the enabled mask it will get
				}
			aMessage.WriteL(1, logMask);
			break;
			}
		case EGetRamBufferSize:
			{
			TBuf8<8> buf;
			buf.SetLength(8);
			TInt* ptr = (TInt*)buf.Ptr();
			ptr[0] = iBufferSize;
			ptr[1] = iNumBuffers;
			aMessage.WriteL(0, buf);
			break;
			}
		case ESetRamBufferSize:
			{
			UpdateBufferSizeL(aMessage.Int0(), aMessage.Int1());
			break;
			}
		case ERotate:
			{
			Rotate(&aMessage);
			return ETrue;
			}
		case EGetRotateBehaviour:
			{
			TBuf8<8> buf;
			buf.SetLength(8);
			TInt* ptr = (TInt*)buf.Ptr();
			ptr[0] = iRotateBehaviour;
			ptr[1] = iNumRotates;
			aMessage.WriteL(0, buf);
			break;
			}
		case ESetRotateBehaviour:
			{
			iNumRotates = aMessage.Int0();
			iRotateBehaviour = aMessage.Int1();
			break;
			}
		case EPersistSettings:
			{
			PersistSettingsL();			
			break;
			}
		case EResetSettings:
			{
			ResetSettingsL();			
			break;
			}
		case EDebugAlignLogFile:
			{
			FlushBuffers();
			iLogFile.Write(TPtrC8((const TUint8*)MINTAGSTART));
			TInt pos = 0;
			User::LeaveIfError(iLogFile.Seek(ESeekCurrent, pos));
			pos = 4096 - (pos % 4096) - aMessage.Int0();
			if (pos < 2) pos += 4096; // use 2 rather than zero so that we always have room in buf for "\r\n"
			RBuf8 buf;
			buf.CreateMaxL(pos);
			buf.Fill('.');
			buf[pos-2] = '\r';
			buf[pos-1] = '\n';
			iLogFile.Write(buf);
			iLogFile.Flush();
			buf.Close();
			break;
			}
		case EDebugShutdownServer:
			{
			CloseBuffers(); // The tests assume we don't flush buffers when we receive a shutdown. Because it tests more things we do a tidy shutdown that would normally involve flushing the buffers.
			CActiveScheduler::Stop();
			return ETrue; // To skip the message complete
			}
		case ESetTagStates:
			{
			RBuf8 buf;
			CleanupClosePushL(buf);
			buf.CreateL(aMessage.GetDesLengthL(0));
			aMessage.ReadL(0, buf);

			// Dear Points of View, Why Oh Why does RBufReadStream's constructor take a CBufBase and not a TDesC8?
			// Answer: Because no-one's really thought about this API for 15 years.
			//
			// There is one copy for IPC, one to construct the CBufFlat, and one to read an HBufC from the stream
			// Something like TCallbackWriter extended slightly could do it in one copy (the IPC)

			CBufFlat* cbuf = CBufFlat::NewL(buf.Length());
			CleanupStack::PushL(cbuf);
			cbuf->InsertL(0, buf);
			RBufReadStream stream(*cbuf);
			CleanupClosePushL(stream);
			TInt count = stream.ReadInt32L();
			RBuf8 enabled;
			CleanupClosePushL(enabled);
			enabled.Assign(HBufC8::NewL(stream, KMaxTInt));

			iEnabledStatesSequenceNumber++;
			RProperty::Set(KCloggerUid, ESequenceNumber, iEnabledStatesSequenceNumber);

			_LIT8(KTagChanged, "Client %S updating tags");
			RThread client;
			aMessage.ClientL(client);
			TFullName clientName = client.FullName();
			TPtrC8 safeClientName = clientName.Collapse().Left(128);
			client.Close();
			LogNote(ELogTagChanges, KTagChanged, &safeClientName);

			for (TInt i = 0; i < count; i++)
				{
				RBuf tagName;
				CleanupClosePushL(tagName);
				tagName.Assign(HBufC::NewL(stream, KMaxTInt));
				TUint32 enabledMask = *((TUint32*)&enabled[i * 4]);
				TTagData* tag = iTags.Find(tagName.Collapse());
				if (tag)
					{
					tag->iEnabled = enabledMask;
					NotifySessionsOfChangedTag(tag);
					}
				else
					{
					// If the client asked us to configure a non-existant tag, we'll just ignore it. The other option
					// would be to set it up on the assumption that tag will be used later. But we've specified in the API
					// what we'll do.
					}
				CleanupStack::PopAndDestroy(&tagName);
				}
			CleanupStack::PopAndDestroy(4, &buf); // enabled, stream, cbuf, buf
			break;
			}
		case EDebugForceBreakpointInServiceL:
			{
			iFlags.Set(EForceBreakpoint);
			break;
			}
		case EStartSessionWriterServer:
			{
			if (iSessionWriterServer)
				{
				res = KErrAlreadyExists;
				}
			else
				{
				iSessionWriterServer = new(ELeave) CSessionWriterServer(*this);
				iSessionWriterServer->ConstructL();
				}
			break;
			}
		default:
			handled = EFalse;
			break;
		}
	if (handled)
		{
		aMessage.Complete(res);
		}
	return handled;
	}

TAny* CCloggerServer::NewSessionForTag(HBufC8* aTag)
	{
	// TODO reverse these so that we don't need a TRAP
	TAny* res = NULL;
	TRAP_IGNORE(res = NewSessionForTagL(aTag));
	return res;
	}

TAny* CCloggerServer::NewSessionForTagL(HBufC8* aTag)
	{
	TTagData* tagData = iTags.Find(*aTag);
	if (!tagData)
		{
		tagData = new(ELeave) TTagData(aTag);
		tagData->iRefCount = 1;
		if (iFallbackTag)
			{
			// If the fallback tag has been initialised (Which it always will be except when we are setting up default tags during startup
			tagData->iEnabled = iFallbackTag->iEnabled;
			}
		else
			{
			tagData->iEnabled = KAllEnabled;
			}
		// Don't call SetShouldDisplay
		CleanupStack::PushL(tagData);
		iTags.InsertL(aTag, tagData);
		CleanupStack::Pop(tagData);

		TPtrC8 safeTag = aTag->Left(200);
		_LIT8(KNewTag, "New tag created: [%S]");
		LogNote(ELogNewTags, KNewTag, &safeTag);
		}
	else
		{
		delete aTag;
		tagData->iRefCount++;
		}
	return tagData;
	}

void CCloggerServer::ReplaceTagL(TAny*& aCurrentContext, HBufC8* aNewTag)
	{
	//TTagData* tag = iTags.Find(aCurrentTag);
	TTagData* tag = reinterpret_cast<TTagData*>(aCurrentContext);
	if (!tag || tag->iRefCount != 1 || tag->ShouldDisplay())
		{
		// Can't imagine how tag could be non-null. 
		// Don't allow tag name to updated if more than one session has been opened for it, or if anything's been written to the log

		// The commsdebug API actually allows this, so we have to cater for it by instead doing a NewSessionForTag
		aCurrentContext = NewSessionForTagL(aNewTag);
		}
	else if (iTags.Find(*aNewTag) != NULL)
		{
		// We already have a session for the new tag name, so again NewSessionForTagL is the right thing to do
		aCurrentContext = NewSessionForTagL(aNewTag);
		}
	else
		{
		// This is a bit more complex than you'd expect because tag->iTagName must not be deleted until tag has been removed from the hashtable (since it is used as the key)

		iTags.InsertL(aNewTag, tag); // On error, don't change anything, particularly what tag->iTagName points to
		// If that succeeded, update the tag's iTagName, remove the old tag name from the hashtable and finally delete it
		HBufC8* oldTagName = tag->iTagName;
		tag->iTagName = aNewTag; // Takes ownership
		iTags.Remove(oldTagName);

		_LIT8(KNewTag, "Tag %S renamed to %S");
		TPtrC8 safeOld = oldTagName->Left(128);
		TPtrC8 safeNew = aNewTag->Left(128);
		LogNote(ELogNewTags, KNewTag, &safeOld, &safeNew);

		delete oldTagName;
		}
	}

void CCloggerServer::DropSession(CCloggerSession* /*aSession*/)
	{
	//TODO decrement ref count and remove tag if reaches zero?
	}

void CCloggerServer::LogHighPerformanceBuffer(const TDesC8& aBuf)
	{
	// To avoid having to copy the data again (it could be 2MB) we use the sync write mechanism
	TUint oldOptions = iOptions;
	if (iOptions & RClogger::EBufferLog)
		{
		iOptions &= ~RClogger::EBufferLog;
		FlushBuffers();
		}
	_LIT8(KHighPerf, "Dumping high-performance logging chunk (one tick=%00.3fus)");
	LogError(KHighPerf, 1000000.0/iTickFreq);
	LogLine(aBuf);
	// Should really call UpdatedGlobalOptions, or at least UpdateBufferSizeL. Meh.
	iOptions = oldOptions;
	ReCalculateFileAlignment();
	}

void CCloggerServer::LogLine(const TDesC8& aLine)
	{
	if (iOptions & RClogger::EBufferLog)
		{
		ResetIdleWriteTimer();
		TPtrC8 lineFrag(aLine);
		while (lineFrag.Length())
			{
			TDes8& buf = iCurrent->iBuf;
			TInt bufMaxLength = buf.MaxLength();
			if (iSubtractSizeOfNextBuffer > buf.MaxLength())
				{
				// Then we have a buffer < 512B. Can't happen because that's the min buffer size
				}
			bufMaxLength -= iSubtractSizeOfNextBuffer;

			TInt spaceFree = bufMaxLength - buf.Length();
			ASSERT(spaceFree >= 0); // If it's negative then something's gone very wrong, we have less space in the buffer than the size of iSubtractSizeOfNextBuffer, implying the buffer was allowed to get too full
			buf.Append(lineFrag.Left(spaceFree));
			if (buf.Length() == bufMaxLength)
				{
				TRAPD(err, GotoNextBufL()); //TODO make this not need a trap?
				iSubtractSizeOfNextBuffer = 0; // If iSubtractSizeOfNextBuffer was non-zero, then by rotating buffers right now, we become all square
				if (err == KErrNoMemory)
					{
					// We needed to allocate a new buffer but couldn't, so block until we have space
					FlushBuffers(); // strictly we only need to wait for one buffer to be free, not all of them
					_LIT8(KNoMem, MINTAGSTART "Ran out of memory trying to expand buffers\r\n"); // we shouldn't be using iTempBuf here, so we can't format the timestamp!
					LogLine(KNoMem);
					}
				else if (err)
					{
					ASSERT(EFalse); // Nothing else should be causing an error in that function
					}
				}
			lineFrag.Set(lineFrag.Mid(Min(spaceFree, lineFrag.Length())));
			// If the line was longer than the amount of freespace in the current buffer, we'll loop here to write the rest
			}
		}
	else
		{
		iSyncWriteBuffer = &aLine;
		TUint anythingEnabled = GetEnabledWriters(); // If no writers are enabled, no need to do anything (and moreover we'd hang if we tried it)
		if (anythingEnabled)
			{
			TellAllWritersToWriteBuf(KSyncBufferIdx);
			BlockRequestsFrom(this, iKernDebugRouter);
			iFlushBufferWait.Start();
			// The logic to do the stop is in CompletedWritingBuf
			ASSERT(iSyncWriteBufferBusy == 0);
			StopBlocking();
			}
		iSyncWriteBuffer = NULL;
		}
	}

void CCloggerServer::Log(TAny* aContext, const TDesC8& aLine, TUint32 aTickCount)
	{
	ACQUIRE_FLAG_LOCK(EInMiddleOfLog);
	ACQUIRE_FLAG_LOCK(EUsingTempBuf);

	TTagData* tag = reinterpret_cast<TTagData*>(aContext);
	if (tag)
		{
		tag->SetShouldDisplay();
		}
	TBuf8<32> rdebugId;
	const TDesC8* t = &KCloggerTag;
	if (tag)
		{
		if (tag->iThreadIdForRDebugger)
			{
			// For RDebug::Prints we use [tid] for the tag name
			rdebugId.Num(tag->iThreadIdForRDebugger);
			t = &rdebugId;
			}
		else
			{
			t = tag->iTagName;
			}
		}
	const TDesC8& aTag = *t;

	TDateTime dt = TickCountToTime(aTickCount).DateTime();
	_LIT8(KFormat, "%i-%02i-%02i %02i:%02i:%02i.%03i: [%S] %S\r\n");
	// Have to add 1 to Month and Day, as these are zero-based
	#define SF_ARGLIST dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond()/1000, &aTag, &aLine

	#define SF_FORMAT KFormat
	#define SF_BUF iTempBuf
	#define SF_ACTION(buf) LogLine(buf)
	#include "SensibleFormat.h"

	RELEASE_FLAG_LOCK(EInMiddleOfLog);
	RELEASE_FLAG_LOCK(EUsingTempBuf);

	if (iFlags.IsSet(ELogBufferOverflow))
		{
		iFlags.Clear(ELogBufferOverflow);
		_LIT8(KCreated, "Had to create a new buffer because a writer wasn't finished (iBufferBusy=0x%x)");

		// Is OK to use the temp buf now that the rest of the Log() function has finished
		// Since we know our string is < 512 chars (and that's the minimum for iTempBuf) we can dispense with the usual paranoid formatting constructs above
		LogError(KCreated, iCurrent->iNext->iBufferBusy);
		}
	}

void CCloggerServer::CloseBuffers()
	{
	UpdatedBuffers(ETrue);
	for (TInt i = iBufs.Count()-1; i >= 0; i--)
		{
		TBufEntry* buf = iBufs[i];
		//buf->iBuf.Close();
		iBufs.Remove(i);
		delete buf;
		}
	AdjustBufferChunk(0);
	iCurrent = NULL;
	}

void CCloggerServer::FlushBuffers()
	{
	iIdleWriteTimer->Cancel();
	// In the if statement below, check not only that we are buffering (iCurrent) but also that we have some writers to wait for (anythingEnabled)
	TUint anythingEnabled = GetEnabledWriters(); // If no writers are enabled, no need to do anything (and moreover we'd hang if we tried it)
	if (iCurrent && anythingEnabled)
		{
		iFlags.Set(EAllowCurrentBufferToBeRead);
		// All enabled writers should already be writing out any filled buffers we have
		// In case some have already finished writing all buffers, explicitly ask them to write out the current buffer as well
		TellAllWritersToWriteBuf(iCurrent->iArrayIdx);
		BlockRequestsFrom(this, iKernDebugRouter);
		iFlushBufferWait.Start();
		// The logic to do the stop is in CompletedWritingBuf
		StopBlocking();
		iFlags.Clear(EAllowCurrentBufferToBeRead);

		ASSERT(iCurrent->iBufferBusy == 0); // Otherwise something is still outstanding...
		}
	}

void CCloggerServer::GotoNextBufL()
	{
	iSubtractSizeOfNextBuffer = KMinWriteSize - (iCurrent->iBuf.Size() % KMinWriteSize);
	if (iSubtractSizeOfNextBuffer == KMinWriteSize) iSubtractSizeOfNextBuffer = 0;

	// Tell writers to get cracking if they aren't already
	TellAllWritersToWriteBuf(iCurrent->iArrayIdx);

	// Now update current buf
	TBufEntry* next = iCurrent->iNext;
	if (next->iBufferBusy)
		{
		// Buffer is still busy - so need to allocate a new one
		TBufEntry* entry = new(ELeave) TBufEntry();
		CleanupStack::PushL(entry);
		TInt size = iChunkForBufs.Size();
		User::LeaveIfError(AdjustBufferChunk(size + iBufferSize));
		entry->iBuf.Set(iChunkForBufs.Base() + size, 0, iBufferSize);
		entry->iArrayIdx = iBufs.Count();
		iBufs.AppendL(entry);
		CleanupStack::Pop(entry);
		// Insert buffer into the circular buffer
		entry->iNext = next;
		iCurrent->iNext = entry;
		iCurrent = entry;
		iFlags.Set(ELogBufferOverflow);
		}
	else
		{
		next->iBuf.Zero(); // If there are no writers enabled, next might not actually be empty
		iCurrent = next;
		}
	}

const TDesC8& CCloggerServer::GetBuf(TInt aIdx)
	{
	if (aIdx == KSyncBufferIdx) return *iSyncWriteBuffer;
	if (aIdx == KCompressFileNameBufferIdx) return KNullDesC8; // The compressor sync writer doesn't use this parameter

	TBufEntry* entry = iBufs[aIdx];
	ASSERT(entry->iBufferBusy); // If the buffer isn't marked as busy, noone should be accessing it
	return entry->iBuf;
	}

void CCloggerServer::CompletedWritingBuf(MWriter* aWriter, TInt aIdx)
	{
	if (aIdx == KSyncBufferIdx)
		{
		// Magic number for sync writes
		TInt writerId = iWriters.Find(aWriter);
		ASSERT(writerId != KErrNotFound);
		iSyncWriteBufferBusy &= ~(1 << writerId);
		if (iSyncWriteBufferBusy == 0)
			{
			iFlushBufferWait.AsyncStop();
			}
		return;
		}
	else if (aIdx == KCompressFileNameBufferIdx)
		{
		// Magic number for background compress completed
		TInt err = KErrNone;
		if (iFlags.IsSet(ENeedToCopyCompressedLog) && iFileBeingRotated.Length())
			{
			iFlags.Clear(ENeedToCopyCompressedLog);
			TRAP(err, CopyLogToExternalMediaL(iFileBeingRotated));
			}
		if (!iRotationMessage.IsNull())
			{
			// Tell the client the rotate has finished
			if (!err) 
				{
				err = iRotationMessage.Write(0, iFileBeingRotated);
				if (err == KErrBadDescriptor) err = KErrNone; // Probably means they called Rotate() with no arguments and so didn't care about the file name
				}
			iRotationMessage.Complete(err);
			}
		iFileBeingRotated.SetLength(0);
		return;
		}

	TBufEntry* entry = iBufs[aIdx];

	TInt writerId = iWriters.Find(aWriter);
	ASSERT(writerId != KErrNotFound);

	// Don't leave this in, it will cause an infinite loop if rdebug redirection is enabled!
	//RDebug::Printf("Writer %i finished writing buffer %i", writerId, aIdx);

	// This writer is done with this buffer
	entry->iBufferBusy &= ~(1 << writerId);

	TInt bufSize = entry->iBuf.Size();
	if (entry->iBufferBusy == 0)
		{
		// This buffer is now available again
		entry->iBuf.Zero();
		}

	// Decide whether the next buffer in the chain also needs writing by this writer

	TBufEntry* next = entry->iNext;
	if ((next->iBuf.Size() == next->iBuf.MaxSize() && entry != iCurrent) // The check for entry != iCurrent is to ensure we don't read past the end of the chain (ie it's technically possible for current to be full and next to be full but part of the tail of the chain)
		|| (next == iCurrent && iFlushBufferWait.IsStarted()))
		{
		// Next entry needs writing too
		TellWriterToWriteBuf(writerId, next->iArrayIdx);
		}
	else
		{
		// No more buffers need writing
		if (entry == iCurrent && entry->iBufferBusy == 0 && iFlushBufferWait.IsStarted())
			{
			// All writers have now completed so stop the scheduler to return to FlushBuffers()
			iFlushBufferWait.AsyncStop();

			// Additionally we need to update iSubtractSizeOfNextBuffer since we don't do a GotoNextBuf when doing FlushBuffers
			iSubtractSizeOfNextBuffer = KMinWriteSize - (bufSize % KMinWriteSize);
			}
		}
	}

TUint32 CCloggerServer::TagEnabled(const TAny* aContext, TInt* aSequenceNumber)
	{
	TUint32 res = KAllEnabled;
	const TTagData* data = reinterpret_cast<const TTagData*>(aContext);
	if (data)
		{
		res = data->iEnabled;
		}
	if (aSequenceNumber)
		{
		*aSequenceNumber = iEnabledStatesSequenceNumber;
		}
	return res;
	}

void CCloggerServer::WriteCallbackForGetTagStatesL(TCallbackWriter& aWriter)
	{
	RBuf8 enabled;
	enabled.CreateL(iTags.Count() * 4);
	CleanupClosePushL(enabled);

	TPtrHashMapIter<TDesC8, TTagData> tagIter(iTags);
	const TTagData* tag;
	while ((tag = tagIter.NextValue()) != NULL)
		{
		if (tag->ShouldDisplay())
			{
			// Don't include tags that have never logged anything - they are probably only here for getting the tag states
			const TPckgC<TUint32> tagEnabled(tag->iEnabled);
			enabled.Append(tagEnabled);
			}
		}
	aWriter.AddL(enabled);
	CleanupStack::PopAndDestroy(&enabled);

	tagIter.Reset();
	while ((tag = tagIter.NextValue()) != NULL)
		{
		if (tag->ShouldDisplay())
			{
			const TDesC8* tagName = tag->iTagName;
			// Don't include tags that have never logged anything - they are probably only here for getting the tag states
			HBufC* tag16 = HBufC::NewLC(tagName->Length());
			tag16->Des().Copy(*tagName);
			aWriter.AddL(*tag16);
			CleanupStack::PopAndDestroy(tag16);
			}
		}
	}

void CCloggerServer::NotifySessionsOfChangedTag(CCloggerServer::TTagData* tag)
	{
	iSessionIter.SetToFirst();
	CCloggerSession* sess;
	TServerCallback callback(ETagEnabledChanged);
	TCallbackWriter writer(callback, NULL);
	writer.AddL((TUint)tag->iEnabled);

	while ((sess = (CCloggerSession*)iSessionIter++) != NULL)
		{
		if (sess->iContext == tag)
			{
			sess->DispatchCallback(callback);
			}
		}
	}
void CCloggerServer::ResetIdleWriteTimer()
	{
	ASSERT(iOptions & RClogger::EBufferLog); // The timer shouldn't get set when buffering is disabled
	const TInt KIdleTime = 500000; // 0.5 seconds
	iIdleWriteTimer->Cancel();
	TCallBack cb(StaticIdleTimerExpired, this);
	iIdleWriteTimer->Start(KIdleTime, KIdleTime, cb);
	}

TInt CCloggerServer::StaticIdleTimerExpired(TAny* aThis)
	{
	static_cast<CCloggerServer*>(aThis)->IdleTimerExpired();
	return 0;
	}

void CCloggerServer::IdleTimerExpired()
	{
	ASSERT(iOptions & RClogger::EBufferLog); // The timer shouldn't be firing when buffering is disabled
	iIdleWriteTimer->Cancel(); // Don't be periodic, this'll just waste CPU

	// Go to the next buf, that will take care of everything
	TRAPD(err, GotoNextBufL());
	if (err == KErrNoMemory)
		{
		// We needed to allocate a new buffer but couldn't, so block until we have space
		FlushBuffers(); //TODO we only need to wait for one buffer to be free, not all of them
		_LIT8(KNoMem, "Ran out of memory trying to expand buffers during idle write");
		Log(NULL, KNoMem, User::NTickCount());
		}
	else if (err)
		{
		ASSERT(EFalse); // Nothing else should be causing an error in that function
		}
	if (iFlags.IsSet(ELogBufferOverflow))
		{
		// Important to check this flag whenever GotoNextBufL is called! Otherwise you risk it not getting picked up
		// until the next call to Log(), by which time buffering might have been disabled meaning iCurrent is no
		// longer valid
		iFlags.Clear(ELogBufferOverflow);
		_LIT8(KCreated, "Had to create a new buffer during idle write because a writer wasn't finished (iBufferBusy=0x%x)");

		LogError(KCreated, iCurrent->iNext->iBufferBusy);
		}

	}

TUint CCloggerServer::GetEnabledWriters() const
	{
	TUint res = 0;
	for (TInt i = 0; i < iWriters.Count(); i++)
		{
		if (iWriters[i]->IsEnabled()) res |= 1 << i;
		}
	return res;
	}

void CCloggerServer::Rotate(const RMessage* aMsg)
	{
	// This function uses a number of different error variables (rotateErr, continuationErr, listingErr) because not all errors are fatal or should be handled in the same way

	if (iCompressor->IsActive() && (iRotateBehaviour & RClogger::ECompressRotatedLogs))
		{
		if (aMsg) aMsg->Complete(KErrNotReady);
		return;
		}

	ASSERT(iRotationMessage.IsNull());

	_LIT(KFormat, "%c:\\logs\\clogger_%i%02i%02i_%02i-%02i-%02i.%03i.txt");
	TDateTime dt = TickCountToTime(User::NTickCount()).DateTime();
	TDes& newName = iFileBeingRotated;
	iFs.SessionPath(newName);
	TUint driveLetter = newName[0];
	newName.Format(KFormat, driveLetter, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond()/1000);
	
	FlushBuffers(); // overkill again but easier to implement
	iLogFile.Close();
	// NOTE: Be sure not to call LogNote or LogError between this point and the call to OpenLogFile below!
	RFile logFile;
	TInt rotateErr = logFile.Open(iFs, KLogFile, EFileShareExclusive|EFileWrite); // Need to reopen the file for exclusive access so I can rename it
	if (rotateErr == KErrNone)
		{
		rotateErr = logFile.Rename(newName);
		}
	logFile.Close();

	if (rotateErr == KErrNotFound)
		{
		rotateErr = KErrNone;
		// No worries
		}
	else if (rotateErr)
		{
		// We couldn't rotate the log file. Just log the fact later on
		}

	TInt continuationErr = OpenLogFile();
	if (continuationErr)
		{
		// We're in trouble now. Log to any remaining writers
		_LIT8(KRotateErr, "Error trying to reopen log file (%i)");
		LogError(KRotateErr, continuationErr);
		}

	if (rotateErr)
		{
		// Then we failed to rotate (although we may have succeeded in reopening the file for logging)
		_LIT8(KRotateErr, "Failed to rotate log file (%i)");
		LogError(KRotateErr, rotateErr);
		}

	// Delete all log files more than iNumRotates
	_LIT(KLogFileFormat, "clogger_*");
	CDir* list = NULL;
	TInt listingErr = iFs.GetDir(KLogFileFormat, KEntryAttNormal, ESortByName, list);
	if (!listingErr && list->Count() > iNumRotates)
		{
		for (TInt i = 0; i < list->Count() - iNumRotates; i++)
			{
			// Don't delete the log we've just created if we need to rotate it or if iNumRotates is anything other than zero
			// (On devices where the time is consistantly wrong it is possible to get in this situation)
			TBool justCreated = (*list)[i].iName.CompareC(newName) == 0;
			if (justCreated && ((iRotateBehaviour & RClogger::ECopyRotatedToExternalMedia) || iNumRotates > 0))
				{
				continue;
				}
			iFs.Delete((*list)[i].iName); // Ignore any errors because there's nothing we need to do
			}
		}
	delete list;

	TBool compress = EFalse;
#ifdef FSHELL_EZLIB_SUPPORT
	compress = (rotateErr == KErrNone && (iRotateBehaviour & RClogger::ECompressRotatedLogs));
#endif
	if (compress)
		{
		if (iRotateBehaviour & RClogger::ECopyRotatedToExternalMedia)
			{
			iFlags.Set(ENeedToCopyCompressedLog);
			}
		iCompressor->WriteBuf(KCompressFileNameBufferIdx);
		}
	
	if (rotateErr == KErrNone && !compress && (iRotateBehaviour & RClogger::ECopyRotatedToExternalMedia))
		{
		TRAP(rotateErr, CopyLogToExternalMediaL(newName));
		}
	TInt aResult = rotateErr ? rotateErr : continuationErr;

	TBool completeNow = !compress;
	if (aMsg)
		{
		if (completeNow)
			{
			if (aResult == KErrNone)
				{
				aResult = aMsg->Write(0, iFileBeingRotated);
				if (aResult == KErrBadDescriptor) aResult = KErrNone; // Probably means they called Rotate() with no arguments and so didn't care about the file name
				}
			aMsg->Complete(aResult);
			iFileBeingRotated.SetLength(0); // Finished with this buffer for now
			}
		else
			{
			iRotationMessage = *aMsg;
			}
		}
	}

void CCloggerServer::CopyLogToExternalMediaL(TDes& aFile)
	{
	TInt err = KErrNone;
	TInt removableDrive = KErrNotFound;
	TInt hdAttributeDrive = KErrNotFound;
	// Figure out the first external media device
	for (TInt drive = EDriveA; drive <= EDriveZ; drive++)
		{
		TVolumeInfo info;
		err = iFs.Volume(info, drive);
		if (err == KErrNone)
			{
			LogNote(ELogDisks, _L8("Found drive %c iDrivaAtt=0x%x iType=0x%x"), 'A' + drive, info.iDrive.iDriveAtt, info.iDrive.iType);
			}
		if (err == KErrNone && (info.iDrive.iDriveAtt & KDriveAttRemovable))
			{
			removableDrive = drive;
			}
		else if (err == KErrNone && info.iDrive.iType == EMediaHardDisk)
			{
			// Added the check for hard disk without removable so we can use _epoc_drive_d in the emulator
			hdAttributeDrive = drive;
			break;
			}
		}
	TInt theDrive = hdAttributeDrive;
	if (removableDrive != KErrNotFound) theDrive = removableDrive; // Prefer something with the removable attribute, if we have both

	if (theDrive != KErrNotFound)
		{
		CFileMan* fm = CFileMan::NewL(iFs);
		CleanupStack::PushL(fm);
		// aName is of the format C:\logs\clogger_xyz
		// extName needs to be D:\logs\clogger_xyz
		HBufC* nm = aFile.AllocLC();
		RBuf extName;
		extName.Assign(nm);
		TChar ffs;
		RFs::DriveToChar(theDrive, ffs);
		extName[0] = ffs;
		// Actually copy the file
		User::LeaveIfError(fm->Copy(aFile, extName, CFileMan::ERecurse));
		aFile = extName; // If all went well update the file name to reflect where we copied to
		CleanupStack::PopAndDestroy(2, fm); // extName/nm, fms
		}
	else
		{
		LogNote(ELogDisks, _L8("Couldn't find any suitable volumes to copy log file on to"));
		}
	}

void CCloggerServer::LogError(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	DoLogErrorOrNote(RClogger::EAllEnabled, aFmt, args);
	VA_END(args);
	}

void CCloggerServer::LogNote(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	DoLogErrorOrNote(aLogMask, aFmt, args);
	VA_END(args);
	}

void CCloggerServer::DoLogErrorOrNote(TUint32 aLogMask, const TDesC8 &aFmt, VA_LIST args)
	{
	if (!iFlags.IsSet(EWrittenToLog))
		{
		return; // Not ready to log yet - this can happen in the case that logging is attempted during ConstructL
		}

	if (!(aLogMask & iCloggerTag->iEnabled) && aLogMask != RClogger::EAllEnabled)
		{
		// Make sure we always log errors, which are signified by aLogMask being EAllEnabled
		return;
		}

	// Whole string must be less than iTempBuf.MaxLength (ie 512) and will cause a panic if it isn't. So use with caution!
	// ie don't pass in user-generated strings that might be massive.

	_LIT8(KFmt, "%i-%02i-%02i %02i:%02i:%02i.%03i: [Clogger] ");
	_LIT8(KEnd, "\r\n");

	ACQUIRE_FLAG_LOCK(EUsingTempBuf);

	iTempBuf.Zero();
	//iTempBuf.Append(KFmt);

	TDateTime dt = TickCountToTime(User::NTickCount()).DateTime();
	iTempBuf.Format(KFmt, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond()/1000);
	iTempBuf.AppendFormatList(aFmt, args);
	iTempBuf.Append(KEnd);
	LogLine(iTempBuf);

	RELEASE_FLAG_LOCK(EUsingTempBuf);
	}

void CCloggerServer::LogsDirHasBeenCreated()
	{
	//__DEBUGGER();
	OpenLogFile(); // Ignore error
	}

TDes& CCloggerServer::GetFilenameToRotate()
	{
	return iFileBeingRotated;
	}

void CCloggerServer::HexDumpL(TAny* aContext, const TDesC8& aHeader, const TDesC8& aData, TUint32 aTickCount)
	{
	const TInt KLineSize = 16;
	const TInt KExtra = 7 + 1; // The number of chars of padding that go into the output line.
	// Header0000 : 00 01 02 03  ....
	// So line len is headerLen + 4 + 3 + (3 * lineSize) + 1 + lineSize
	_LIT8(KFormat, "%04X : ");
	RBuf8 lineBuf;
	CleanupClosePushL(lineBuf);
	lineBuf.CreateL(aHeader.Length() + 4 * KLineSize + KExtra);

	TPtrC8 lineFrag(aData);
	TInt offset = 0;
	while (lineFrag.Length())
		{
		lineBuf.Zero();
		TInt lineSize = Min(KLineSize, lineFrag.Length());
		// Start with header (or empty space if continuation)
		if (offset == 0)
			{
			lineBuf.Append(aHeader);
			}
		else
			{
			lineBuf.AppendFill(' ', aHeader.Length());
			}

		// Add offset
		lineBuf.AppendFormat(KFormat, offset);
		offset += lineSize;

		// Add hex representation
		for (TInt i = 0; i < lineSize; i++)
			{
			TBuf8<2> charBuf;
			charBuf.NumFixedWidthUC(lineFrag[i], EHex, 2);
			lineBuf.Append(charBuf);
			lineBuf.Append(' ');
			}
		// Add filler if lineSize < KLineSize
		lineBuf.AppendFill(' ', (KLineSize - lineSize) * 3);

		// Add ascii representation
		lineBuf.Append(' ');
		for (TInt i = 0; i < lineSize; i++)
			{
			TChar c = lineFrag[i];
			lineBuf.Append(c.IsPrint() ? c : TChar('.'));
			}
		lineFrag.Set(lineFrag.Mid(lineSize));
		Log(aContext, lineBuf, aTickCount);
		}
	CleanupStack::PopAndDestroy(&lineBuf);
	}

void CCloggerServer::SetKernelLoggingL(TBool aEnable)
	{
	if (!iKernDebugRouter)
		{
		// Either means we're being called very early during server ConstructL, or the debug router LDD is missing
		return;
		}
	if (aEnable)
		{
		iKernDebugRouter->OpenChunkL(); // This will do nothing if already open
		}
	
	if (aEnable)
		{
		TBool consume = ETrue; //iOptions & RClogger::EMirrorToRDebugPrint;
		// ^ We always consume, it makes things easier.
		iKernDebugRouter->StartRouting(consume);
		}
	else
		{
		iKernDebugRouter->StopRouting();
		}
	}

const TInt KThreadTagNamBufLen = 256;

void CCloggerServer::LogKernMessage(TUint8 aWhere, TUint32 aTickCount, TUint aThreadId, const TDesC8& aMsg)
	{
	TUint enableMask = aWhere == 'K' ? ELogKernPrintf : aWhere == 'P' ? ELogPlatSecDiagnostics : ELogRDebugPrint;
	TBool kern = aWhere != 'U';
	TTagData* tag = iRDebugTag; // This is used only as a last-ditch fallback if we've run out memory

	if (aThreadId != 0)
		{
		// Look for a tag in the iRDebugTags
		TTagData** tagDataPtr = iRDebugTags.Find(aThreadId);
		if (tagDataPtr) tag = *tagDataPtr;
		else
			{
			// New tag (or rather, look in iTags keyed by thread name)
			RThread thread;
			TInt err = KErrNone;
			HBufC8* tagBuf = HBufC8::New(KThreadTagNamBufLen*2);
			if (!tagBuf) err = KErrNoMemory;
			
			if (err)
				{
				// Fall back to just RDebug::Print or Kern::Printf as appropriate
				_LIT8(KUnknown, "Unknown thread %u has started rdebugging");
				_LIT8(KUnknownKern, "Unknown kernel thread %u has started rdebugging");
				LogNote(ELogNewRdebuggers, kern ? KUnknownKern() : KUnknown() , aThreadId);
				iRDebugTags.Insert(aThreadId, tag); // So that we don't get the above message every time this thread debugs
				}
			else
				{
				err = thread.Open(aThreadId);
				TPtr wPtr((TUint16*)tagBuf->Ptr(), KThreadTagNamBufLen);
				if (err)
					{
					// Couldn't open thread - use Thread-xxx as tag
					_LIT(KThreadFormat, "Thread-%u");
					wPtr.Format(KThreadFormat, aThreadId);
					}
				else
					{
					//thread.FullName(wPtr); // This didn't exist on 9.1...
					TFullName name = thread.FullName();
					wPtr = name;
					}
				thread.Close();
				TInt len = wPtr.Collapse().Length();
				TPtr8 narrowPtr = tagBuf->Des();
				narrowPtr.SetLength(len);
				ThreadPrettyName(narrowPtr);
				TPtrC8 safeName = tagBuf->Left(150);
				_LIT8(KNewTag, "Thread [%u] (%S) has started rdebugging");
				LogNote(ELogNewRdebuggers, KNewTag, aThreadId, &safeName);
				TAny* theTag = NewSessionForTag(tagBuf);
				if (theTag) 
					{
					tag = reinterpret_cast<TTagData*>(theTag);
					tag->iThreadIdForRDebugger = aThreadId;
					iRDebugTags.Insert(aThreadId, tag);
					}
				else
					{
					delete tagBuf;
					}
				}
			}
			// If an error occurred then we'll just log using the RDebug::Print tag
		}

	if (tag->iEnabled & enableMask)
		{
		Log(tag, aMsg, aTickCount);
		}
	else
		{
		RegisterDisabledLog(tag);
		}
	}

void CCloggerServer::UpdatedGlobalOptionsL(TUint aOldOptions)
	{
	if ((aOldOptions & RClogger::EBufferLog) != (iOptions & RClogger::EBufferLog))
		{
		UpdateBufferSizeL(iBufferSize, iNumBuffers);
		}

	iWriters[ERDebug]->SetEnabled(iOptions & RClogger::EMirrorToRDebugPrint);
	iWriters[EMessageQueue]->SetEnabled(iOptions & RClogger::EMirrorToMessageQueue);
	iWriters[EFile]->SetEnabled(!(iOptions & RClogger::EDisableFileWriter));

	TBool redirectRDebugBeingChanged = (aOldOptions & RClogger::ERedirectRDebugPrintToClogger) != (iOptions & RClogger::ERedirectRDebugPrintToClogger);
	TBool mirrorRDebugChanged = (aOldOptions & RClogger::EMirrorToRDebugPrint) != (iOptions & RClogger::EMirrorToRDebugPrint);

	if (redirectRDebugBeingChanged || mirrorRDebugChanged)
		{
		SetKernelLoggingL(iOptions & RClogger::ERedirectRDebugPrintToClogger);
		}
	}

void CCloggerServer::TellAllWritersToWriteBuf(TInt aBuf)
	{
	for (TInt i = 0; i < iWriters.Count(); i++)
		{
		TellWriterToWriteBuf(i, aBuf);
		}
	}

void CCloggerServer::TellWriterToWriteBuf(TInt aWriterId, TInt aBuf)
	{
	MWriter* writer = iWriters[aWriterId];
	if (writer->IsEnabled() && !writer->IsBusyWriting())
		{
		if (aBuf == KSyncBufferIdx)
			{
			iSyncWriteBufferBusy |= (1<<aWriterId);
			}
		else
			{
			TBufEntry* entry = iBufs[aBuf];
			entry->iBufferBusy |= (1<<aWriterId);
			}
		writer->WriteBuf(aBuf);
		}
	}

void CCloggerServer::RegisterDisabledLog(TAny* aContext)
	{
	reinterpret_cast<TTagData*>(aContext)->SetShouldDisplay();
	}

void CCloggerServer::UpdatedBuffers(TBool aAboutToCloseBuffers)
	{
	if (!iKernDebugRouter)
		{
		// No point doing anything
		return;
		}
	
	// If there are more than 8 buffers then you're on your own!
	const TInt KMaxBuffersInParams = 8; // Note - this should probably be defined to be the same as KMaxCrashDumpAreas
	
	TUint32 base = (TUint32)iChunkForBufs.Base();
	
	SCloggerCrashDumpArea bufAreas[KMaxBuffersInParams];
	TInt numBuffers = 0;
	if (!aAboutToCloseBuffers)
		{
		for (TInt i = 0; numBuffers < KMaxBuffersInParams && i < iBufs.Count(); i++)
			{
			TBufEntry* buf = iBufs[i];
			bufAreas[numBuffers].iChunkHandle = iChunkForBufs.Handle();
			bufAreas[numBuffers].iOffset = (TUint32)buf->iBuf.Ptr() - base;
			bufAreas[numBuffers].iSize = buf->iBuf.MaxSize();
			_LIT8(KBufName, "Clogger Buffer %i");
			bufAreas[numBuffers].iName.Format(KBufName, i);
			numBuffers++;
			}
		}
	// Now check sessions for performance logging chunks
	iSessionIter.SetToFirst();
	CCloggerSession* sess;
	while ((sess = (CCloggerSession*)iSessionIter++) != NULL)
		{
		TInt chunkHandle = sess->iPerformanceLoggingChunk.Handle();
		if (chunkHandle && numBuffers < KMaxBuffersInParams)
			{
			bufAreas[numBuffers].iChunkHandle = chunkHandle;
			bufAreas[numBuffers].iOffset = 0; // Performance logging uses the whole chunk
			bufAreas[numBuffers].iSize = sess->iPerformanceLoggingChunk.Size();
			_LIT8(KBufName, "Clogger MemLog ");
			bufAreas[numBuffers].iName = KBufName;
			TPtrC8 tagName = reinterpret_cast<TTagData*>(sess->iContext)->iTagName->Left(bufAreas[numBuffers].iName.MaxLength() - bufAreas[numBuffers].iName.Length());
			bufAreas[numBuffers].iName.Append(tagName);
			numBuffers++;
			}
		}
	
	TPtrC8 ptr((const TUint8*)&bufAreas[0], numBuffers * sizeof(SCloggerCrashDumpArea));
	iKernDebugRouter->RegisterCrashDumpAreas(ptr);
	}

TInt CCloggerServer::CreateKernChunkForClient(RThread* aClient, TInt aMaxSize, TInt aCommittedSize, RChunk& aOurChunk)
	{
	if (!iKernDebugRouter)
		{
		return KErrNotSupported;
		}

	return iKernDebugRouter->CreateKernChunkForClient(aClient, aMaxSize, aCommittedSize, aOurChunk);
	}

TInt CCloggerServer::AdjustBufferChunk(TInt aNewSize)
	{
	// This is needed because you can't adjust a chunk created on kernel-side from user side. Grr.
	TInt err = KErrNotSupported;
	if (iKernDebugRouter)
		{
		err = iKernDebugRouter->AdjustChunk(iChunkForBufs, aNewSize);
		} 
	else if (iChunkForBufs.Handle()) // Might not have been created yet
		{
		// If the chunk wasn't created via the device driver, it's fine to adjust it directly
		err = iChunkForBufs.Adjust(aNewSize);
		}
	return err;
	}

CSessionWriter* CCloggerServer::RegisterSessionWithSessionWriter(CSessionWriterSession* aSession)
	{
	CSessionWriter* sessionWriter = static_cast<CSessionWriter*>(iWriters[ESessionWriter]);
	if (!aSession)
		{
		// Session closing
		sessionWriter->SetEnabled(EFalse);
		sessionWriter->iSession = NULL;
		return NULL;
		}
	else if (sessionWriter->iSession)
		{
		// Already got one
		return NULL;
		}
	else
		{
		sessionWriter->iSession = aSession;
		return sessionWriter;
		}
	}

RChunk& CCloggerServer::GetBufChunk()
	{
	return iChunkForBufs;
	}

void CCloggerServer::ThreadPrettyName(TDes8& aName)
	{
	// Algorithm is:
	// Given a aName xxx(.exe)?\[uid\]nonce::(!)?yyy
	// if yyy == xxx (case insensitive compare) then just display yyy (with exclamation mark if present)
	// else display xxx::yyy

	// Find the :: that splits thread and process name
	_LIT8(KDoubleColon, "::");
	TInt colon = aName.Find(KDoubleColon);
	if (colon == KErrNotFound)
		{
		// It's not a thread name
		return;
		}

	// Remove .exe if present immediately before [uid]
	TInt brak = aName.Left(colon).LocateReverse('[');
	_LIT8(KExe, ".exe");
	if (brak >= 5 && aName.Mid(brak-4, 4).CompareF(KExe) == 0)
		{
		aName.Delete(brak-4, 4);
		brak -= 4;
		colon -= 4;
		}

	TPtrC8 procaName = brak >=0 ? aName.Left(brak) : aName.Left(colon);
	TPtrC8 threadaName = aName.Mid(colon+2);
	TPtrC8 realThreadaName = threadaName.Length() && threadaName[0] == '!' ? threadaName.Mid(1) : threadaName;
	if (procaName.CompareF(realThreadaName) == 0)
		{
		aName.Delete(0, colon+2);
		}
	else if (brak >= 0)
		{
		aName.Delete(brak, colon-brak);
		}
	}

TBool CCloggerServer::ForceBreakpointRequested() const
	{
	return iFlags.IsSet(EForceBreakpoint);
	}

inline TTime CCloggerServer::TickCountToTime(TUint32 aTickCount) const
	{
	return TTime(iTimeAtStartup.Int64() + (((TInt64)aTickCount*1000000) / (TInt64)iTickFreq) - iStartupTickInMicroseconds);
	}
