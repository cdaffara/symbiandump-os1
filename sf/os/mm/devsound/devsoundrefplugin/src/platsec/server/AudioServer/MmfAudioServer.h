// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFAUDIOSERVER_H__
#define __MMFAUDIOSERVER_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "mmfAudioPolicyProxy.h"

class CStartAndMonitorDevSoundThread;
class RMMFDevSoundServerProxy;

class CMMFAudioServer : public CMmfIpcServer
/**
*@internalTechnology
*/
	{
public:
	static CMMFAudioServer* NewL();
	~CMMFAudioServer();
	CMmfIpcSession* NewSessionL(const TVersion &aVersion) const;
	void IncrementSessionId();
	void DecrementSessionId();

	void IncrementDevSoundCount();
	void DecrementDevSoundCount();

	void SendEventToClient(TInt aSessionToAlert, TInt aSessionToBeLaunched);
	void LaunchRequest(TInt aSessionId);
	//CAudioPolicy* AudioPolicy() {return iAudioPolicy;};
	TInt AudioServerSessionId() {return iAudioServerSessionId;};
	TInt StartDevSoundServer(RMMFDevSoundServerProxy& aDevSoundSessionHandle) const;
	RServer2& PolicyServerHandle();
private:

	class CDelayAudioServerShutDown : public CActive
        {
    public:
        // Construct/destruct
        static CDelayAudioServerShutDown* NewL();
        ~CDelayAudioServerShutDown(); 
        // Request
        void SetDelay(TTimeIntervalMicroSeconds32 aDelay);   
    private:
        // Construct/destruct
        CDelayAudioServerShutDown();
        void ConstructL();
    
        // From CActive
        void RunL();
        void DoCancel();   
    private:
        RTimer iShutDownTimer;      // Has
        };

private:
	CMMFAudioServer();
	void ConstructL();

private:
	TInt iAudioServerSessionId;
	TInt iDevSoundCount;
	TInt iSessionToBeLaunched;
	CDelayAudioServerShutDown* iDelayAudioServerShutDown;
	mutable RPointerArray<CStartAndMonitorDevSoundThread> iDevSoundServList;
	RServer2 iPolicyServerHandle;
	RMMFAudioPolicyProxy* iAudioPolicyProxy;
	};

class CStartAndMonitorDevSoundThread: public CActive
	{
public:
	static CStartAndMonitorDevSoundThread* NewL(CMMFAudioServer* aAudioServer);
	~CStartAndMonitorDevSoundThread();
	TInt StartDevSoundServer(RMessage2& aMessage, RMMFDevSoundServerProxy& aDevSoundSessionHandle);
private:
	// Construct
	CStartAndMonitorDevSoundThread(CMMFAudioServer* aAudioServer);
	void ConstructL();
   
	// From CActive
	void RunL();
	void DoCancel();
private:
	CMMFAudioServer* iAudioServer;
	RThread iServer;
        };
        
class RMMFDevSoundServerProxy : public RMmfSessionBase
	{
public:
	TInt Open(RServer2& aDevSoundServerHandle);
	};
	
#include "MmfAudioServer.inl"
#endif
