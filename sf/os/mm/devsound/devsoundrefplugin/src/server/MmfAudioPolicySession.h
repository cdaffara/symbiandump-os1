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

#ifndef __MMFAUDIOPOLICYSESSION_H__
#define __MMFAUDIOPOLICYSESSION_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "MmfAudioPolicyRequest.h"  
#include "MmfPolicyClientServer.h"
#include "MmfDevSoundEventHandler.h"
#include <mmf/common/mmfstandardcustomcommands.h>

class CMMFAudioPolicyEventReceiver : public CBase
/**
*@internalTechnology
*/
	{
public:
	static CMMFAudioPolicyEventReceiver* NewL(const RMmfIpcMessage& aMessage);
	~CMMFAudioPolicyEventReceiver();
	void SendEvent(const TMMFAudioPolicyEvent& aEvent);
	void SendEvent(const TMMFEvent& aEvent);
private:
	CMMFAudioPolicyEventReceiver(const RMmfIpcMessage& aMessage);
private:
	RMmfIpcMessage iMessage;
	TBool iNeedToCompleteMessage;
	};

class CAudioPolicy;
class CMMFAudioPolicyServer;




class CMMFAudioPolicySession : public CMmfIpcSession
/**
*@internalTechnology
*/
	{
public:
	static CMMFAudioPolicySession* NewL();
	~CMMFAudioPolicySession();
	void CreateL(const CMmfIpcServer& aServer);
	void ServiceL(const RMmfIpcMessage& aMessage);
	CMMFAudioPolicyRequest* AudioPolicyRequest(void) {return iAudioPolicyRequest;};
	TInt PolicySessionId(void) {return iPolicySessionId;};
	void SendEventToClient(const TMMFAudioPolicyEvent& aEvent);
	CMMFAudioPolicyServer* Server();
	void ClearEventQ();

private:
	class TMMFAudioPolicyEventHolder
		{
	public:
		TMMFAudioPolicyEventHolder(const TMMFAudioPolicyEvent& aEvent, TBool aNeedsDeletion) : 	iEvent(aEvent),
																								iShallBeDeleted(aNeedsDeletion) {};
		TMMFAudioPolicyEvent iEvent;
		const TBool iShallBeDeleted;
		TSglQueLink iLink;
		};

private:
	CMMFAudioPolicySession();
	void ConstructL();
	void MakeRequestL(const RMmfIpcMessage& aMessage);
	TBool UpdateStateL(const RMmfIpcMessage& aMessage);
	TBool ReceiveEventsL(const RMmfIpcMessage& aMessage);
	TBool CancelReceiveEvents();
	TBool GetPlayFormatsSupportedL(const RMmfIpcMessage& aMessage);
	TBool GetRecordFormatsSupportedL(const RMmfIpcMessage& aMessage);
	TBool GetPlayFormatL(const RMmfIpcMessage& aMessage);
	TBool GetRecordFormatL(const RMmfIpcMessage& aMessage);
	TBool SetDevSoundInfoL(const RMmfIpcMessage& aMessage);
	TBool LaunchRequest();
	TBool RequestResourceNotificationL(const RMmfIpcMessage& aMessage);
	TBool CancelRequestResourceNotificationL(const RMmfIpcMessage& aMessage);
	TBool StopNotificationL(const RMmfIpcMessage& aMessage);
	TBool GetResourceNotificationEventL(const RMmfIpcMessage& aMessage) const;
	CMMFAudioPolicyServer* iServer;
	CMMFAudioPolicyRequest* iAudioPolicyRequest;
	CAudioPolicy* iAudioPolicy;
	CMMFAudioPolicyEventReceiver* iEventReceiver;
	TSglQue<TMMFAudioPolicyEventHolder> iEventsQue;
	TInt iPolicySessionId;
	TInt iDevSoundId;
	TMMFAudioPolicyPrioritySettings	iAudioPolicyPrioritySettings;
	TMMFAudioPolicyEventHolder 	iNoMemoryEventHolder;
	TBool iNoMemoryEventUsed;
	TInt iEventQSize;
	};


#endif
