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
#include "MmfAudioServer.h"
#include "MmfDevSoundServerStart.h"

/** 
Panic category and codes that MmfDevSoundServer raises on the client
@internalTechnology
*/
_LIT(KMMFDevSoundClientPanicCategory, "DevSoundClient");
enum TMMFDevSoundClientPanicCodes
	{
	EMMFDevSoundPlayDataWithoutInitialize,
	EMMFDevSoundRecordDataWithoutInitialize
	};
	
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
	static void RenamePrioritizeServer();
	static void StartThreadL(TDevSoundServerStart& aStart);
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

GLDEF_C void PanicClient(const RMmfIpcMessage& aMessage, TMMFDevSoundClientPanicCodes aPanicCode);

/**
@SYMPatchable
@publishedPartner
 
Patchable constant for licencee to set devsound server thread priority
Licencees should ensure that the devsound server thread priority is equal to the system server priority for better responsiveness.
If SYMBIAN_CURB_SYSTEMSERVER_PRIORITIES is defined, setting devsound server thread priority to EPriorityAbsoluteHigh will ensure
devsound server's thread priority is set to system server's priority.
If the macro is unavailable it is recommended to use EPriorityAbsoluteRealTime1. Usage of EPriorityAbsoluteRealTime1 requires ProtServ capability.
The constant can be changed at ROM build time using patchdata OBY keyword.
 */
IMPORT_C extern const TInt KDevsoundServerPriority;

#endif
