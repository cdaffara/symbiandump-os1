// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMFDRMPLUGINSERVER_H
#define MMFDRMPLUGINSERVER_H

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
class CStartAndMonitorControllerThread;
class RMMFControllerServerProxy;

class CMMFDRMPluginServer : public CMmfIpcServer
/**
*@internalTechnology
*/
	{
public:
	static CMMFDRMPluginServer* NewL();
	~CMMFDRMPluginServer();
	CMmfIpcSession* NewSessionL(const TVersion &aVersion) const;

	void IncrementSessionId();
	void DecrementSessionId();
	
	void IncrementControllerCount();
	void DecrementControllerCount();

	TInt StartControllerServer(const RThread& aClientThread, TUint aMaxHeapSize, TBool aUseSharedHeap,
							RMMFControllerServerProxy& aDevSoundSessionHandle, TThreadId& aControllerTID, TUint aStackSize) const;
							
	void PanicControllerThread(TThreadId aTid, const TDesC& aCategory,TInt aReason);
	void KillControllerThread(TThreadId aTid, TInt aReason);
	TInt SetThreadPriority(TThreadId aTid, TThreadPriority aPriority);
	void SetTimeout(TInt aTimeout) { iServerTimeout = aTimeout; };
	
private:

	class CDelayServerShutDown : public CActive
        {
    public:
        // Construct/destruct
        static CDelayServerShutDown* NewL();
        ~CDelayServerShutDown(); 
        // Request
        void SetDelay(TTimeIntervalMicroSeconds32 aDelay);   
        
    private:
        // Construct/destruct
        CDelayServerShutDown();
        void ConstructL();
    
        // From CActive
        void RunL();
        void DoCancel();   
    private:
        RTimer iShutDownTimer;      // Has
        };

private:
	CMMFDRMPluginServer();
	void ConstructL();

private:
	TInt iSessionCount;
	TInt iControllerCount;
	TInt iServerTimeout;
	CDelayServerShutDown* iDelayServerShutDown;
	mutable RPointerArray<CStartAndMonitorControllerThread> iControllerServList;
	};

class CStartAndMonitorControllerThread: public CActive
	{
public:
	static CStartAndMonitorControllerThread* NewL(CMMFDRMPluginServer* aPluginServer);
	~CStartAndMonitorControllerThread();
	TInt StartControllerServer(const RThread& aClientThread, TUint aMaxHeapSize, TBool aUseSharedHeap,
							RMMFControllerServerProxy& aControllerSessionHandle, TThreadId& aControllerTID, TUint aStackSize);
	RThread& Thread() { return iServerThread; };
private:
	// Construct
	CStartAndMonitorControllerThread(CMMFDRMPluginServer* aPluginServer);
	void ConstructL();
   
	// From CActive
	void RunL();
	void DoCancel();
private:
	CMMFDRMPluginServer* iDrmPluginServer;
	RThread iServerThread;
    };
        
class RMMFControllerServerProxy : public RMmfSessionBase
	{
public:
	TInt Open(RServer2& aControllerServerHandle);
	};

#endif
