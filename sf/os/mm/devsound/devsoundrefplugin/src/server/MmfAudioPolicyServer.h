// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFAUDIOPOLICYSERVER_H__
#define __MMFAUDIOPOLICYSERVER_H__

#include <e32base.h>
#include <e32std.h>
#include "MmfAudioPolicy.h"
#include "MmfPolicyClientServer.h"
#include "MmfAudioPolicyStart.h"

class CMMFAudioPolicyServer; // declared here.

NONSHARABLE_CLASS( CMMFAudioPolicyServer ): public CMmfIpcServer
/**
*@internalTechnology
*/
	{
public:
	static CMMFAudioPolicyServer* NewL(); 
	~CMMFAudioPolicyServer();
	CMmfIpcSession* NewSessionL(const TVersion &aVersion) const;
	void IncrementSessionId();
	void SendEventToClient(TInt aSessionToAlert,const TMMFAudioPolicyEvent& aEvent);
	void LaunchRequest(TInt aSessionId,const TMMFAudioPolicyEvent& aEvent);
	inline CAudioPolicy* AudioPolicy() {return iAudioPolicy;};
	inline TInt PolicySessionId() {return iPolicySessionId;};
	static TInt SendNotification(TAny* aAny);
	TBool IsTimerActive() const;
	
	IMPORT_C static TInt StartThread(TAny* aParams);
	void IncrementSessionCount();
	void DecrementSessionCount();
	TInt PolicySessionCount();

	void StopNotificationTimer();
	void StartNotificationTimer(TBool aZeroDelay=EFalse);

private:
	class CNotificationTimer : public CTimer
		{
	public:
		static CNotificationTimer* NewL(TCallBack aCallBack);
	private:
		virtual void RunL();
		CNotificationTimer(TCallBack aCallBack);
	private:
		TCallBack iCallBack;
		};
	
	CMMFAudioPolicyServer();
	void ConstructL();
    static void StartThreadL(TServerStart& aStart);

private:
	TInt iPolicySessionId;	
	CAudioPolicy* iAudioPolicy;
	TInt iPolicySessionCount;
	CNotificationTimer* iNotificationTimer;
	};

#endif
