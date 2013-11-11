// Clogger.h
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

#ifndef CLOGGER_H
#define CLOGGER_H

#if defined(_DEBUG) && !defined(USE_CLOGGER)
#define USE_CLOGGER
#endif

#ifdef __cplusplus

#include <e32std.h>

class CDesC16Array;

class RClogger: public RSessionBase
	{
public:
	/*
	 * Logging commands
	 */
	
	IMPORT_C void Log(TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C void Log(TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C void Log(const char* aFmt, ...);
	IMPORT_C void LogList(TRefByValue<const TDesC> aFmt, VA_LIST aList);
	IMPORT_C void LogList(TRefByValue<const TDesC8> aFmt, VA_LIST aList);
	IMPORT_C void LogList(const char* aFmt, VA_LIST aList);
	IMPORT_C void HexDump(const TDesC8& aHeader, const TDesC8& aData);
	
	IMPORT_C void Log(TUint32 aLogMask, TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C void Log(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C void Log(TUint32 aLogMask, const char* aFmt, ...);
	IMPORT_C void LogList(TUint32 aLogMask, TRefByValue<const TDesC> aFmt, VA_LIST aList);
	IMPORT_C void LogList(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, VA_LIST aList);
	IMPORT_C void LogList(TUint32 aLogMask, const char* aFmt, VA_LIST aList);
	IMPORT_C void HexDump(TUint32 aLogMask, const TDesC8& aHeader, const TDesC8& aData);

	enum TLogBehaviour {
		/* This flag is no longer needed, and has no effect
		 */
		EMonitorTagState = 1,
			
		/* If set, a heap buffer will be allocated (and cached between calls) 
		 * so that arbitrary sized strings can be logged. If it is not set, a 
		 * fixed 256-character buffer on the stack will be used and anything
		 * longer will be silently truncated. It means memory doesn't have to 
		 * be allocated in the client thread (which is good for performance, 
		 * and also means RClogger instances can be shared between threads).
		 *
		 * Note: an RClogger instance CANNOT be shared by multiple threads 
		 * if this flag is set.
		 *
		 * This flag is set by default.
		*/
		EUseHeapBuffer = 2,

		ETrimTrailingNewlines = 4,
	};
		
	IMPORT_C void SetLogBehaviour(TUint aLogBehaviour);
	
	/*
	 * These APIs use TLS to cache an RClogger session. The first time they 
	 * are called they will create a session, store it in TLS, and call
	 * Connect() on it. They then call the non-static function of the same
	 * name on that session. These functions will not allocate any memory
	 * in the client heap unless SetStaticLogBehaviour is called with
	 * EUseHeapBuffer. In other words the EUseHeapBuffer
	 * flag is NOT set by default when calling the static functions.
	 *
	 * You can also call RClogger::StaticConnect(tag) to assign a tag to the
	 * static session
	 */
	
	IMPORT_C static void Slog(TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C static void Slog(TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C static void Slog(const char* aFmt, ...);
	IMPORT_C static void SlogList(TRefByValue<const TDesC> aFmt, VA_LIST aList);
	IMPORT_C static void SlogList(TRefByValue<const TDesC8> aFmt, VA_LIST aList);
	IMPORT_C static void SlogList(const char* aFmt, VA_LIST aList);
	IMPORT_C static void StaticHexDump(const TDesC8& aHeader, const TDesC8& aData);

	IMPORT_C static void Slog(TUint32 aLogMask, TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C static void Slog(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C static void Slog(TUint32 aLogMask, const char* aFmt, ...);
	IMPORT_C static void SlogList(TUint32 aLogMask, TRefByValue<const TDesC> aFmt, VA_LIST aList);
	IMPORT_C static void SlogList(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, VA_LIST aList);
	IMPORT_C static void SlogList(TUint32 aLogMask, const char* aFmt, VA_LIST aList);
	IMPORT_C static void StaticHexDump(TUint32 aLogMask, const TDesC8& aHeader, const TDesC8& aData);
	
	IMPORT_C static void SetStaticLogBehaviour(TUint aLogBehaviour);
	IMPORT_C static TInt StaticConnect(); // This is implicitly called if you call Slog when not connected
	IMPORT_C static TInt StaticConnect(const TDesC& aTag);
	IMPORT_C static void StaticClose(); // Cleans up TLS, deallocates memory, closes the TLS session
		
	/*
	 * Configuring logging on the fly - APIs for a gui config tool to use
	 */

	static const TUint32 EAllEnabled = 0xFFFFFFFFu;
	
	IMPORT_C TInt SetEnabled(const TDesC& aTag, TUint32 aEnabledMask);
	IMPORT_C TUint32 IsEnabled(const TDesC& aTag); // Non-existant tags will return all true, ie 0xFFFFFFFF, because if someone did create it later, it would default to being enabled). Failure to connect to the server will return false (because we can't log!)
	
	IMPORT_C void GetTagStatesL(CDesC16Array*& aTagNames, RBuf8& aEnabled); // aTagNames and aEnabled will be zeroed/deleted before being used. aEnabled is a bitfield of the enabled states (32 bits per tag in aTagNames)
	IMPORT_C void SetTagStatesL(const CDesC16Array* aTagNames, const TDesC8& aEnabled); // Any tag names not yet known to clogger will (in the current implementation) be ignored. aEnabled[0..4] is the enabled mask for aTagNames[0], aEnabled[4..8] is for aTagNames[1] etc
	
	/*
	 * Global logging options (ones that affect all users of Clogger)
	 */
	 
	enum TGlobalOptions {
		// All these options default to being unset
		EBufferLog = 1, // If set, use a RAM buffer for increased performance. If unset, writes are guaranteed to be flushed to disk before Log() returns. Default is not to buffer.
		ERDebugPrint = 2, // Deprecated because the name is no longer clear given we have ERedirectRDebugPrintToClogger as well. Use EMirrorToRDebugPrint instead.
		EMirrorToRDebugPrint = 2, // Mirror all logging to RDebug::Print. Note enabling this may impact performance as RDebug can be quite slow. If EBufferLog is enabled as well the RDebug printing will be done from the buffer in a separate thread, which should mean performance is not affected unless the clogger serve runs out of memory to buffer the output
		EMirrorToBluetooth = 4, // Outputs to a bluetooth serial port. The logger is dormant until something connects to the port
		EMirrorToMessageQueue = 32, // This outputs the log data to an RMsgQueue. Caution is advised, you should not turn on this option unless you know what you're doing!
		ERedirectRDebugPrintToClogger = 8, // Mirrors Kern::Printf and RDebug::Print to the clogger log file. May be combined with EMirrorToRDebugPrint, in which case all RDebug::Print calls *except* ones made by clogger itself are redirected
		EDisableFileWriter = 16, // Set this option to disable writing to the log file. Useful for debugging out-of-disk problems assuming you're using EMirrorToRDebugPrint 
	};
 		
	IMPORT_C void SetGlobalOptions(TUint aGlobalOptions);
	IMPORT_C TUint GetGlobalOptions();
	
	IMPORT_C TInt GetRamBufferSize(TInt* aNumberOfBuffers);
	IMPORT_C void SetRamBufferSize(TInt aSizeInBytes, TInt aNum); // If buffering is enabled the server will use this many buffers of this size
	
	/*
	 * Configuring rotation on the fly
	 */
	 
	IMPORT_C TInt Rotate();
	IMPORT_C TInt Rotate(TDes& aFileName); // On success aFileName will be set to the name of the rotated log

	enum TRotateBehaviour {
		EDoNothingSpecial = 0,
		ECopyRotatedToExternalMedia = 1,
		EAutoRotateAtStartup = 4,
		ECompressRotatedLogs = 8, // Currently uses GZip compression on the rotated log files
	};
		
	IMPORT_C void SetRotateBehaviour(TInt aNumberOfOldLogsToKeep, TUint aRotateBehaviour); // First argument of zero means don't keep any. If you don't want to ever delete the old logs, use A Big Number
	IMPORT_C TUint GetRotateBehaviour(TInt* aNumberOfOldLogsToKeep);
	
	/*
	 * General
	 */
	IMPORT_C RClogger(); 
	IMPORT_C TInt Connect(); // If using this overload your thread full name becomes your tag
	IMPORT_C TInt Connect(const TDesC& aTag); // All logging will be prepended with this tag. Multiple clients may use the same tag.
	IMPORT_C void Close();
	
	IMPORT_C void PersistSettings(); // Store enabled and rotate settings for next boot
	IMPORT_C void ResetSettings(); // Restore all settings to default, including stored ones (ie PersistSettings is implicitly called)

	IMPORT_C TInt Reserved(TInt aCode, TAny*& aArg, TAny* aArg2); // Reserved, do not call or you will get panicked
	IMPORT_C static TInt StaticReserved(TInt aCode, TAny*& aArg, TAny* aArg2); // Reserved, do not call or you will get panicked

private:
	static TBool GetTls(RClogger*& aClogger, TBool aAutoConnect, const TDesC* aTag = NULL, TInt* aError = 0);
	inline TBool Enabled(TUint32);
	void UpdateEnabled(TInt aSequence);
	IMPORT_C RClogger(TAny* aTls); // Do not call directly

protected:
	TUint iFlags;
	TUint32 iEnabled;
	RBuf8 iBuf;
	TUint32 iPimpl[2]; // an RCloggerBody
	TUint32 iSpare;
	TInt iSequence;

	friend class RCloggerBody;
	};

_LIT(KCloggerDefaultTag, "DefaultForNewTags"); // This tag controls the default value for the enabled mask of things that start logging for the first time

#ifdef USE_CLOGGER
#define SLOG(args...) RClogger::Slog(args)
#else
#define SLOG(args...)
#endif

#ifdef USE_CLOGGER
// Debatable whether you'd want/need these
#define CLOGGER_STATICCONNECT(aTag) RClogger::StaticConnect(aTag)
#define CLOGGER_STATICCONNECTL(aTag) User::LeaveIfError(RClogger::StaticConnect(aTag))
#define CLOGGER_STATICCLOSE() RClogger::StaticClose()
#else
#define CLOGGER_STATICCONNECT(aTag)
#define CLOGGER_STATICCONNECTL(aTag)
#define CLOGGER_STATICCLOSE()
#endif

class RCloggerLogConsumer : public RSessionBase
	{
public:
	/*
	 * API for clients that want the logging data - not for general usage!
	 */
	IMPORT_C RCloggerLogConsumer();
	IMPORT_C TInt Connect();
	IMPORT_C void GetNextLog(TRequestStatus& aStatus, TPtrC8& aResultBuffer);
	IMPORT_C void CancelGetNextLog();
	IMPORT_C void Close();

private:
	RChunk iSharedChunk;
	TPtr8 iResultBufferPtr;
	};

extern "C" IMPORT_C void Clogger_Slog(const char* aFmt, ...);

#else // __cplusplus

IMPORT_C void Clogger_Slog(const char* aFmt, ...);

#endif // __cplusplus


#endif // CLOGGER_H
