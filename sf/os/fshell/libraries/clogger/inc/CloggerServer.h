// CloggerServer.h
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

#ifndef CLOGGERSERVER_H
#define CLOGGERSERVER_H

#include "SensibleServer.h"
#include <babitflags.h>
#include <e32hashtab.h>
#include <e32msgqueue.h>
#include "debugrouter.h"

class CCloggerServer;
class MWriter;
class CLogsDirWatcher;
class CSyncWriterWrapper;
class CDebugRouterClient;
class CSessionWriter;
class CSessionWriterSession;
class CSessionWriterServer;

class CCloggerSession : public CSensibleSession
	{
public:
	CCloggerSession();

private:
	TBool DoServiceL(const RMessage& aMessage);

	inline CCloggerServer& Server();
	~CCloggerSession();

public:
	TAny* iContext;
	RBuf8 iGetTagStatesContext;
	RChunk iPerformanceLoggingChunk; // Only used by the high performance logging library clogger-buffered.dll
	};

class CCloggerServer : public CSensibleServer
	{
public:
	CCloggerServer();
	~CCloggerServer();
	void ConstructL();

	TBool DoServiceL(const RMessage& aMessage);

	// Functions to be called by the session
	inline RBuf8& SessionTempBuf() { iSessionTempBuf.Zero(); return iSessionTempBuf; }
	void Log(TAny* aContext, const TDesC8& aLine, TUint32 aTickCount); // aContext can be NULL to indicate KCloggerTag
	TUint32 TagEnabled(const TAny* aContext, TInt* aCurrentSequenceNumber = NULL);
	void WriteCallbackForGetTagStatesL(TCallbackWriter& aWriter);
	TAny* NewSessionForTagL(HBufC8* aTag); // Takes ownership at end. Returns an opaque identifier for this tag (which may have been shared with other sessions)
	TAny* NewSessionForTag(HBufC8* aTag);
	void ReplaceTagL(TAny*& aCurrentContext, HBufC8* aNewTag); // If the tag for aCurrentContext hasn't been used yet, deletes it from the tag list. In either case, it then calls NewSessionForTagL. Takes ownership of both arguments if nothing leaves
	void DropSession(CCloggerSession* aSession);
	void HexDumpL(TAny* aContext, const TDesC8& aHeader, const TDesC8& aData, TUint32 aTickCount);
	void RegisterDisabledLog(TAny* aContext);
	void UpdatedBuffers(TBool aAboutToCloseBuffers=EFalse); // Registers the log buffers with the debug router, so they can be recorded in a crash log (if supported by the baseport)
	TInt CreateKernChunkForClient(RThread* aClient, TInt aMaxSize, TInt aCommittedSize, RChunk& aOurChunk);
	CSessionWriter* RegisterSessionWithSessionWriter(CSessionWriterSession* aSession);
	RChunk& GetBufChunk();
	static void ThreadPrettyName(TDes8& aName);
	TBool ForceBreakpointRequested() const;

	void LogsDirHasBeenCreated();
	void ResetIdleWriteTimer();

	// Functions to be called by writers (eg CLogFile, RDebug::Print thingy)
	const TDesC8& GetBuf(TInt aIdx); // Increments ref count. May be called from other threads
	void CompletedWritingBuf(MWriter* aWriter, TInt aBuf); // Tells the server that aWriter has finished writing aBuf. If all writers have finished with this buffer then the buffer is marked ready to use. Must be called from main thread.
	TDes& GetFilenameToRotate(); // Should only be called by iCompressor, but may be called from its SyncWriter thread. iCompressor should update the name to indicate the new file it has created (so that ECopyRotatedToExternalMedia will work)

	void LogKernMessage(TUint8 aWhere, TUint32 aTickCount, TUint aThreadId, const TDesC8& aMsg); //  Should only be called by CDebugRouterClient
	void LogHighPerformanceBuffer(const TDesC8& aBuf);
	void LogError(TRefByValue<const TDesC8> aFmt, ...);

private:
	TInt TransientServerShutdownTime() const;
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	void LogLine(const TDesC8& aLine);
	void CloseBuffers(); // Deletes all buffers and removes them from iBufs
	void FlushBuffers();
	void GotoNextBufL(); // Sets iCurrent to the next empty buffer, or allocates one if there isn't a free one. Also tells writers to get cracking if they aren't already busy
	void GetSettingsL();
	void DoGetSettingsL();
	void PersistSettingsL();
	void ResetSettingsL();
	void UpdateBufferSizeL(TInt aSize, TInt aNum);
	static TInt StaticIdleTimerExpired(TAny* aThis);
	void IdleTimerExpired();
	TUint GetEnabledWriters() const;
	void Rotate(const RMessage* aMessage = NULL);
	void LogNote(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, ...);
	void DoLogErrorOrNote(TUint32 aLogMask, const TDesC8 &aFmt, VA_LIST args);
	void CopyLogToExternalMediaL(TDes& aFile);
	class TTagData; // I see now why the coding standards say declare inner classes etc at the start of the class declaration...
	void NotifySessionsOfChangedTag(TTagData* aTag);
	void SetKernelLoggingL(TBool aEnable);
	void UpdatedGlobalOptionsL(TUint aOldOptions);
	void TellAllWritersToWriteBuf(TInt aBuf);
	void TellWriterToWriteBuf(TInt aWriterId, TInt aBuf);
	void ReCalculateFileAlignment();
	TInt OpenLogFile();
	TInt AdjustBufferChunk(TInt aNewSize);
	inline TTime TickCountToTime(TUint32 aTickCount) const;

private:
	// Buffers
	class TBufEntry {
	public:
		TBufEntry() : iBuf(NULL, 0), iArrayIdx(-1), iNext(NULL), iBufferBusy(0) {}

		//RBuf8 iBuf;
		TPtr8 iBuf;
		TInt iArrayIdx;
		TBufEntry* iNext;
		TUint iBufferBusy; // This is used to track when all the different consumers of the buffer (RDebug, Bluetooth, File etc) are finished with it
		};
	RPointerArray<TBufEntry> iBufs;
	TBufEntry* iCurrent;
	TInt iSubtractSizeOfNextBuffer;
	RBuf8 iSessionTempBuf; // Used by sessions for reading descriptor from client
	RBuf8 iTempBuf; // Used for formatting line with timestamp and tag
	RChunk iChunkForBufs; // Use a separate chunk so we can map into crashlogs

	// Miscellaneous stuff used when doing flushes and sync writes
	CActiveSchedulerWait iFlushBufferWait;
	const TDesC8* iSyncWriteBuffer; // This is only used when doing synchronous writing. If not for the OOM handling behaviour of LogLine, it would always be equal to &iTempBuf
	TUint iSyncWriteBufferBusy;
	CPeriodic* iIdleWriteTimer;

	// Log file
	RFs iFs; // A couple of things assume the session path is always in the log directory on the c drive
	RFile iLogFile;
	CLogsDirWatcher* iLogsDirWatcher;
	RPointerArray<MWriter> iWriters;
	TFileName iFileBeingRotated; // This is used to temporarily store the name of a file while it's being rotated and possibly compressed
	CSyncWriterWrapper* iCompressor;
	RMessagePtr2 iRotationMessage; // To be completed when the log file compression finishes

	// Settings
	TUint iOptions; // TGlobalOptions
	TBitFlags32 iFlags;
	TInt iBufferSize;
	TInt iNumBuffers;
	TInt iNumRotates;
	TUint iRotateBehaviour;
	TInt64 iStartupTickInMicroseconds;
	TTime iTimeAtStartup;
	TInt iTickFreq;
	// Tags
	class TTagData
		{
	public:
		TTagData(HBufC8* aName) : iTagName(aName), iRefCount(0), iEnabled(0), iThreadIdForRDebugger(0), iFlags() {}
		void SetShouldDisplay() { iFlags.Set(EShouldDisplay); }
		TBool ShouldDisplay() const { return iFlags.IsSet(EShouldDisplay); }

		HBufC8* iTagName;
		TInt iRefCount;
		TUint32 iEnabled; // bitmask
		TUint iThreadIdForRDebugger;
	private:
		enum TFlags {
			EShouldDisplay, // Indicates this tag should be exposed to the client via GetTagStatesL
			};
		TBitFlags32 iFlags;
		};

	RPtrHashMap<TDesC8, TTagData> iTags; // Keys are the tag names, values are TTagDatas
	RHashMap<TUint, TTagData*> iRDebugTags;
	CDebugRouterClient* iKernDebugRouter;
	TTagData* iRDebugTag; // Is always set. Only used in OOM conditions when we can't create a threadid&name tag
	TTagData* iFallbackTag; // Always set. This tag represents the default enabled state for any new tags that appear while we're running
	TTagData* iCloggerTag; // Always set. Used to figure out the internal logging mask (previously used iInternalLoggingMask)

	CSessionWriterServer* iSessionWriterServer; // Support for the session writer
	TInt iEnabledStatesSequenceNumber;
	};

#define MINTAGSTART "---------- --:--:--.---: [Clogger] " // Writers can use this too, if they want to

class MSyncWriter
	{
public:
	/*
	 * Synchronously writes aBuf. Can leave to indicate failure (although the failure may well be ignored)
	 */
	virtual void WriteBufL(const TDesC8& aBuf) =0;
	
	/*
	 * delete writer and all resources.
	 */
	virtual void CloseWriter() =0;
	};

class MWriter
	{
public:
	/*
	 * Start writing aBuf. The server guarantees not to call this if the plugin is not enabled (as defined by a call
	 * to IsEnabled()) or if the plugin is already busy writing something else (as defined by IsBusyWriting()).
	 * If the writer is enabled and not currently writing something else, but is unable to perform the write,
	 * then it should call CompletedWritingBuf before returning, to indicate that the server shouldn't wait for it.
	 *
	 * All calls to WriteBuf must be matched by a call to CompletedWritingBuf.
	 */
	virtual void WriteBuf(TInt aBuf) =0;

	virtual void CloseWriter() =0; // delete writer and all resources. Will never be called while a write is in progress.

	/*
	 * Enables complex writers to take action when they are disabled.
	 * Writers must honour this setting by returning immediately when a write function is called when they are disabled.
	 * This may be called when a write is in progess - if so the write of the current buffer should complete as normal, it
	 * should just update its internal flag so that next time the server calls WriteBuf it returns immediately
	 *
	 * Generally though SetEnabled will simply update an iEnabled member variable
	 *
	 * Writers will get a call to SetEnabled after they are constructed, before WriteBuf is called for the first time.
	 */
	virtual void SetEnabled(TBool aEnabled) =0;
	virtual TBool IsEnabled() =0;

	virtual TBool IsBusyWriting() =0;
	};

class CLogWriter : public CActive, public MWriter
	{
public:
	CLogWriter(CCloggerServer& aServer, RFile& aFile)
		: CActive(EPriorityHigh), // It's more important to keep writing than to handle clientserver calls
		iServer(aServer), iFile(aFile), iBuf(-1), iEnabled(EFalse)
		{
		CActiveScheduler::Add(this);
		}

	void WriteBuf(TInt aBuf);
	void DoCancel();
	void RunL();
	void SetEnabled(TBool aEnabled);
	TBool IsEnabled() { return iEnabled; }
	void CloseWriter();
	~CLogWriter();
	TBool IsBusyWriting();
		
private:
	CCloggerServer& iServer;
	RFile& iFile;
	TInt iBuf;
	TBool iEnabled;
	};

class CRDebugWriter : public CBase, public MSyncWriter
	{
public:
	CRDebugWriter(CCloggerServer& aServer);
	void WriteBufL(const TDesC8& aBuf);
	void CloseWriter();

private:
	CCloggerServer& iServer;
	};

class CMessageQueueWriter : public CBase, public MSyncWriter
	{
public:
	static CMessageQueueWriter* NewL();	
	void WriteBufL(const TDesC8& aBuf);
	void CloseWriter();

private:
	CMessageQueueWriter();
	~CMessageQueueWriter();

private:
	RMsgQueue<TBuf8<128> > iQ;
	};

class CSessionWriter : public CBase, public MWriter
	{
public:
	CSessionWriter(CCloggerServer& aServer);
	~CSessionWriter();

	void WriteBuf(TInt aBuf);
	void DoCancel();
	void RunL();
	void SetEnabled(TBool aEnabled);
	TBool IsEnabled() { return iEnabled; }
	void CloseWriter();
	TBool IsBusyWriting();

	void Completed();

public:
	CSessionWriterSession* iSession; // Support only one session

private:
	CCloggerServer& iServer;
	TInt iBuf;
	TBool iEnabled;
	};

class CSyncWriterWrapper : public CActive, public MWriter
	{
public:
	static CSyncWriterWrapper* NewL(CCloggerServer& aServer, MSyncWriter& aWriter, TInt aWriterId); // Takes ownership of aWriter at end
	void WriteBuf(TInt aBuf);
	void DoCancel();
	void RunL();
	void SetEnabled(TBool aEnabled);
	TBool IsEnabled() { return iEnabled; }
	void CloseWriter();
	~CSyncWriterWrapper();
	TBool IsBusyWriting();

private:
	CSyncWriterWrapper(CCloggerServer& aServer, MSyncWriter& aWriter);
	void ConstructL(TInt aWriterId);
	static TInt ThreadFunction(TAny* aSelf);
private:
	CCloggerServer& iServer;
	MSyncWriter& iWriter;
	TInt iBuf;
	TBool iEnabled;
	RThread iMainThread;
	RThread iWorkerThread;
	TRequestStatus iThreadStatus;
	TBool iOwnWriter; // Bit of a cludge to ensure ownership of aWriter is correct
	};

class CLogCompressor : public CBase, public MSyncWriter
	{
public:
	static CLogCompressor* NewLC(CCloggerServer& aServer);
	~CLogCompressor();
	void WriteBufL(const TDesC8& aBuf);
	void CloseWriter();

private:
	CLogCompressor(CCloggerServer& aServer);
	void DoGzipL(RFile& aInput, const TDesC& aOutput);

private:
	CCloggerServer& iServer;
	RFs iFs;
	};

class CDebugRouterClient : public CActive
	{
public:
	static CDebugRouterClient* NewL(CCloggerServer& aServer);
	~CDebugRouterClient();
	void RunL();
	void DoCancel();
	void StartRouting(TBool aConsumeLogs);
	void StopRouting();
	void OpenChunkL();
	TInt RegisterCrashDumpAreas(const TDesC8& aCrashDumpAreas);
	TInt CreateKernChunkForClient(RThread* aClient, TInt aMaxSize, TInt aCommittedSize, RChunk& aOurChunk);
	TInt AdjustChunk(RChunk& aChunk, TInt aNewSize);

private:
	CDebugRouterClient(CCloggerServer& aServer);
	void ConstructL();

private:
	CCloggerServer& iServer;
	RCloggerDebugRouter iDebugRouter;
	RChunk iSharedChunk;
	RBuf8 iTempBuf;
	};

#endif
