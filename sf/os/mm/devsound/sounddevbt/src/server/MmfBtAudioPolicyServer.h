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

#ifndef __MMFBTAUDIOPOLICYSERVER_H__
#define __MMFBTAUDIOPOLICYSERVER_H__

#include <e32base.h>
#include <e32std.h>
#include "MmfBtAudioPolicy.h"
#include "MmfBtPolicyClientServer.h"

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
	void SendEventToClient(TInt aSessionToAlert, TInt aSessionToBeLaunched, TMMFAudioPolicyEvent& aEvent);
	void LaunchRequest(TInt aSessionId, TMMFAudioPolicyEvent& aEvent);
	CAudioPolicy* AudioPolicy() {return iAudioPolicy;};
	TInt PolicySessionId() {return iPolicySessionId;};
	static TInt SendNotification(TAny* aAny);
	TBool IsTimerActive() const;
	
	IMPORT_C static TInt StartThread(TAny* aParams);
	void IncrementSessionCount();
	void DecrementSessionCount();
	TInt PolicySessionCount();

	void StopNotificationTimer();
	void StartNotificationTimer();

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

private:
	TInt iPolicySessionId;	
	TInt iSessionToBeLaunched;
	CAudioPolicy* iAudioPolicy;
	TInt iPolicySessionCount;
	CNotificationTimer* iNotificationTimer;
	};

#endif
