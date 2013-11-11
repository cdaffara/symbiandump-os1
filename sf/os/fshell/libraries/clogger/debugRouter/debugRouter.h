// debugRouter.h
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __DebugRouter_H__
#define __DebugRouter_H__

#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif
#include <u32std.h>

_LIT(KDebugRouterName,"Clogger-DebugRouter");
class RChunk;

struct SCloggerCrashDumpArea
	{
	TInt iChunkHandle;
	TUint iOffset;
	TUint iSize;
	TBuf8<32> iName;
	};

struct SCreateChunkParams
	{
	TInt iHandleOfOtherThread;
	TInt iMaxSize;
	TInt iCommittedSize;
	TInt iChunkHandle; // Return value
	TInt iOtherThreadChunkHandle; // Return value, If iHandleOfOtherThread != 0
	};

struct SCloggerTraceInfo
	{
	TUint8 iTraceType; // 'P', 'K', 'U'
	TUint8 iReserved;
	TUint16 iLength; // Kern trace is always <=256 chars but unfortunately platsec diagnostics aren't!
	TUint32 iTickCount;
	TUint iThreadId;
	};

struct SDebugChunkHeader
	{
	TUint iStartOffset;
	TUint iEndOffset;
	TUint iOverflows; // Number of log statements that were dropped because the buffer was full (measured since last completion, not cumulative)
	};
	
class RCloggerDebugRouter : public RBusLogicalChannel
	{
public:
	enum TControl
		{
		EControlEnableRouting,
		EControlOpenChunk,
		ERequestGetData,
		EControlRegisterBuffers,
		EControlCreateAndOpenAChunk,
		EControlAdjustChunk,
		ENumRequests,  // Add new commands above this line
		EAllRequests= (1<<ENumRequests)-1
        };
	enum TEnableOption
		{
		EDisable,
		EEnableRouting,
		EEnableRoutingAndConsume,
		};
public:
	static TInt LoadDriver();
	static void CloseDriver();
	TInt Open();
	void Close();

public: // For retreiving RDebug::Print data
	TInt OpenChunk(RChunk& aChunk);
	void ReceiveData(TRequestStatus& aStatus);
	void CancelReceive();
	TInt EnableDebugRouting(TEnableOption aEnable);

public: // For registering buffers with the crash dumber (NOTE: requires support in the baseport to have any effect)
	// This supercedes any previous call to RegisterCrashDumpAreas. Therefore to unregister everything, call RegisterCrashDumpAreas(KNullDesC8)
	TInt RegisterCrashDumpAreas(const TDesC8& aCrashDumpAreas); // packed array of SCloggerCrashDumpAreas
	
	TInt CreateChunk(SCreateChunkParams& aParams); // returns chunk handle or error
	TInt AdjustChunk(RChunk& aChunk, TInt aNewSize);
	};

#ifndef __KERNEL_MODE__
// Inline implementations of user side interface

inline TInt RCloggerDebugRouter::LoadDriver()
	{	return User::LoadLogicalDevice(KDebugRouterName);	}
inline void RCloggerDebugRouter::CloseDriver()
	{	User::FreeLogicalDevice(KDebugRouterName);	}
inline void RCloggerDebugRouter::Close()
	{	RHandleBase::Close(); }
inline TInt RCloggerDebugRouter::Open()
	{	return DoCreate(KDebugRouterName,TVersion(1,0,0),KNullUnit,NULL,NULL); 	}
	
inline TInt RCloggerDebugRouter::EnableDebugRouting(TEnableOption aEnable)
	{	return DoControl(EControlEnableRouting, (TAny*)aEnable);	}

inline TInt RCloggerDebugRouter::OpenChunk(RChunk& aChunk)
	{	
	TInt res = DoControl(EControlOpenChunk);
	if (res >= 0) 
		{
		aChunk.SetHandle(res);
		return KErrNone;
		}
	return res;
	}

inline void RCloggerDebugRouter::ReceiveData(TRequestStatus& aStatus)
	{
	DoRequest(ERequestGetData, aStatus);
	}

inline void RCloggerDebugRouter::CancelReceive()
	{
	DoCancel(0xFFFFFFFF); // We only have 1 async request at the moment
	}

inline TInt RCloggerDebugRouter::RegisterCrashDumpAreas(const TDesC8& aCrashDumpAreas)
	{
	return DoControl(EControlRegisterBuffers, (TAny*)&aCrashDumpAreas);
	}

inline TInt RCloggerDebugRouter::CreateChunk(SCreateChunkParams& aParams)
	{
	return DoControl(EControlCreateAndOpenAChunk, (TAny*)&aParams);
	}

inline TInt RCloggerDebugRouter::AdjustChunk(RChunk& aChunk, TInt aNewSize)
	{
	return DoControl(EControlAdjustChunk, (TAny*)aChunk.Handle(), (TAny*)aNewSize);
	}

#endif //__KERNEL_MODE__

#endif //__DebugRouter_H__
