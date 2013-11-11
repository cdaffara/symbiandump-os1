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

#include "MmfAudioServerSession.h"
#include "MmfAudioClientServer.h"
#include "MmfAudioServer.h"
#include <s32mem.h>
#include <e32math.h>


/*
 This call takes ownership of aDevSoundSessionHandle. We need to ensure that it is closed
 in case of failure, but want to avoid double close situations. We emporarily push the argument
 onto cleanup stack and pass it into the CMMFAudioServerSession once we know that the assignment
 can't fail. The latter object itself takes ownership.
 */
CMMFAudioServerSession* CMMFAudioServerSession::NewL(RHandleBase& aDevSoundSessionHandle)
	{
	CleanupClosePushL(aDevSoundSessionHandle);
	CMMFAudioServerSession* self = new(ELeave) CMMFAudioServerSession();
	CleanupStack::Pop(&aDevSoundSessionHandle);
	self->SetDevSoundSessionHandle(aDevSoundSessionHandle);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CMMFAudioServerSession::ConstructL()
	{
	}

CMMFAudioServerSession::CMMFAudioServerSession()// : iEventsQue(_FOFF(TMMFAudioPolicyEventHolder,iLink))
	{
	}

void CMMFAudioServerSession::CreateL(const CMmfIpcServer& aServer)
	{
	CMmfIpcSession::CreateL(aServer);
	CMMFAudioServer* server = static_cast<CMMFAudioServer*>(CONST_CAST(CMmfIpcServer*, &aServer));
	server->IncrementSessionId();
	iAudioServerSessionId = server->AudioServerSessionId();
	}

CMMFAudioServerSession::~CMMFAudioServerSession()
	{
	CMMFAudioServer* server = 
		const_cast<CMMFAudioServer*>(static_cast<const CMMFAudioServer*>(Server()));
	if (server)
		{
		server->DecrementSessionId();		
		}
	iDevSoundSessionHandle.Close();
	}

void CMMFAudioServerSession::ServiceL(const RMmfIpcMessage& aMessage)
	{
	switch(aMessage.Function())
		{
	case EMMFDevSoundSessionHandle:
		aMessage.Complete(iDevSoundSessionHandle);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	}

void CMMFAudioServerSession::SendEventToClient(/*TMMFAudioPolicyEvent& aEvent*/)
	{
	}

void CMMFAudioServerSession::SetDevSoundSessionHandle(RHandleBase& aDevSoundSessionHandle)
	{
	iDevSoundSessionHandle = aDevSoundSessionHandle;
	}
