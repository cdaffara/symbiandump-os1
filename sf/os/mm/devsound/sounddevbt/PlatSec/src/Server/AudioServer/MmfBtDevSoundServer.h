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

#ifndef __MMFDEVSOUNDSERVER_H__
#define __MMFDEVSOUNDSERVER_H__

#include <e32base.h>
#include <e32def.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "MmfBtAudioServer.h"


class CMMFDevSoundServer; // declared here.

NONSHARABLE_CLASS( CMMFDevSoundServer ): public CMmfIpcServer
/**
*@internalTechnology
*/
	{
public:
	IMPORT_C static CMMFDevSoundServer* NewL(CMMFAudioServer* aAudioServer, TProcessId& aClientPID);
	~CMMFDevSoundServer();
	CMmfIpcSession* NewSessionL(const TVersion &aVersion) const;
	void IncrementSessionId();
	void DecrementSessionId();
	void SendEventToClient(TInt aSessionToAlert, TInt aSessionToBeLaunched);
	void LaunchRequest(TInt aSessionId);
	TInt DevSoundSessionId() {return iDevSoundSessionId;};
	TBool CheckClientCapabilities();
	void SetClientCapabilitiesL(TThreadId aTid);
	
	IMPORT_C static TInt StartThread(TAny* aParams);

private:

	CMMFDevSoundServer(CMMFAudioServer* aAudioServer, TProcessId& aClientPID);
	void ConstructL();

	class CDelayDevSoundShutDown : public CActive
        {
    public:
        // Construct/destruct
        static CDelayDevSoundShutDown* NewL();
        ~CDelayDevSoundShutDown(); 
        // Request
        void SetDelay(TTimeIntervalMicroSeconds32 aDelay);   
    private:
        // Construct/destruct
        CDelayDevSoundShutDown();
        void ConstructL();
    
        // From CActive
        void RunL();
        void DoCancel();   
    private:
        RTimer iTimer;      // Has
        };


private:
	TInt iDevSoundSessionId;	
	TInt iSessionToBeLaunched;
	TProcessId iClientPID;
	TBool iClientHasCaps;
	CDelayDevSoundShutDown* iDelayDevSoundShutDown;
	CMMFAudioServer* iAudioServer;
	};


#endif
