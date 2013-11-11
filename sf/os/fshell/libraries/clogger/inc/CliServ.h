// CliServ.h
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

#ifndef CLISERV_H
#define CLISERV_H

#include <fshell/common.mmh>

_LIT(KMyServerName,"CloggerServer");
_LIT(KMyServerImg,"CloggerServer");		// DLL/EXE name
const TUid KServerUid3 = { FSHELL_UID_CLOGGERSERVER };
const TUid KCloggerUid = { FSHELL_UID_CLOGGERSERVER };

enum TPubSubKeys
	{
	ESequenceNumber,
	
	EDebugRouterPtr = 1000, // Used only be debug router, included here for completeness
	};

#if defined(__WINS__) && !defined(EKA2)
const TInt KMyServerStackSize=0x2000;			//  8KB
const TInt KMyServerInitHeapSize=0x1000;		//  4KB
const TInt KMyServerMaxHeapSize=0x1000000;		// 16MB
#endif

enum TOpPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	ENotRegisteredForLogs,
	};


#ifdef HIDE_TSERVERSTART_FROM_DAMN_EIKSERVER

class TServerStart;

#else

class TServerStart
	{
public:
	TServerStart(TRequestStatus& aStatus);
	TPtrC AsCommand() const;

	TServerStart();
	TInt GetCommand();
	void SignalL();
private:
	TThreadId iId;
	TRequestStatus* iStatus;
	};

#endif

#define DISOWN(x) { delete x; x = NULL; }

enum TCallbackCode
	{
	ETagEnabledChanged,
	};

/* Types:
 */
class TServerCallback
	{
public:
	TServerCallback() : iContextLength(0), iData() {}
	TServerCallback(TCallbackCode aCode) : iCode(aCode), iContextLength(0), iData() {}

	TCallbackCode iCode;
	TInt iContextLength;
	TBuf8<40> iData;
	};

/* Format of stuff in iData above (and any context) is a one character code (below) which identifies the type, 
 * followed by the data for that type. 16-bit descriptors are automatically 2-byte aligned. (If you didn't know 
 * this was a requirement to construct a TPtrC16, well you've learned something)
 * Types are:
 * i - TInt
 * u - TUint
 * P - TPoint
 * S - TSize
 * G - TRgb
 * R - TRect
 * D - TDesC16 (an 'i' followed by an int is written first for the length of the descriptor)
 * 8 - TDesC8  (an 'i' followed by an int is written first for the length of the descriptor)
 */

enum TOpCode
	{
	EDummy, // Does nothing, just completes with no error
	ERegisterCallbackNotifier = 1, // [TPkg<TServerCallback>]
	EGetCallbackContext, // [TDes8]
	EWriteCallbackResultAndReregister, // [TPkg<TServerCallback>, TDesX, len]
	ECancelCallbackNotifier, // []
	//EMultiball, // [a number of IPC args packaged in a TDesC8, num, TDes8 result, result len] async. result must be big enough to handle all the results from the IPC.

	ESetTag, // [tag, secondLevelTag, TPckg<enabledMask>] returns sequence number that the enabledMask is valid for, or error
	ESetTag8, // [tag, secondLevelTag, TPckg<enabledMask>]
	ELog8, // [text, tickcount, logMask]
	ESetEnabled, // [tag, logMask]
	EIsEnabled, // [tag, TPkg<logMask>]
	//ETailLogToRDebug, // [numChars]
	//ETailLogToBuf, // [buf]
	ESetGlobalOptions, // [flags]
	EGetGlobalOptions, // []
	ESetRamBufferSize, // [size, num]
	EGetRamBufferSize, // [TPkg<size, num>]
	ESetRotateBehaviour, // [numLogs, flags]
	ERotate, // [filename(optional)]
	EPersistSettings, // []
	EResetSettings, // []
	EGetTagStates1, // [TPkg<TServerCallback: enabledTagsBuf, tags...>]
	EGetTagStates2, // [&context]
	ESetTagStates, // [RBufStream: numTags, enabledTagsBuf, tags...]
	EGetRotateBehaviour, // [TPkg<behaviour, numLogs>]
	EHexDump, // [header, data, tickcount, logMask]
	EUpdateEnabledMask, // [TPkg<logMask>]
	ERegisterPerformanceLoggingChunk, // [chunkHandle]
	ECreatePerformanceLoggingChunk, // [size]

	EStartSessionWriterServer, //[]
	ERegisterForLogMessages, // []
	EGetNextLog, // [TPckg<TPtrC8>(aResultBuffer), sharedChunkBase]
	ECancelGetNextLog, // []

	EDebugAlignLogFile = 2000, // [lessThisAmount]
	EDebugShutdownServer, // 2001, []
	EHexDump16, // 2002, [header16, data, tickcount, logMask]
	EDebugForceBreakpointInServiceL, // 2003
	};

_LIT(KSessionLogServer, "CloggerSessionLogServer");

#endif
