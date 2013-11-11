// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MmfBtAudioServerSession.h"
#include "MmfBtAudioClientServer.h"
#include "MmfBtAudioServer.h"
#include <s32mem.h>
#include <e32math.h>


CMMFAudioServerSession* CMMFAudioServerSession::NewL(TName& aDevSoundServerName)
	{
	CMMFAudioServerSession* self = new(ELeave) CMMFAudioServerSession(aDevSoundServerName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CMMFAudioServerSession::ConstructL()
	{
	}

CMMFAudioServerSession::CMMFAudioServerSession(TName& aDevSoundServerName)// : iEventsQue(_FOFF(TMMFAudioPolicyEventHolder,iLink))
	{
	iDevSoundServerName = aDevSoundServerName;
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
	}

void CMMFAudioServerSession::ServiceL(const RMmfIpcMessage& aMessage)
	{
	TBool complete = EFalse;
	switch(aMessage.Function())
		{
	case EMMFDevSoundServerNameLength:
		complete = DoGetDevSoundServerNameLengthL(aMessage);
		break;
	case EMMFDevSoundServerName:
		complete = DoGetDevSoundServerNameL(aMessage);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		aMessage.Complete(KErrNone);

	}

TInt CMMFAudioServerSession::DoGetDevSoundServerNameLengthL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TInt> descriptorSizePckg(iDevSoundServerName.Length());	
	aMessage.WriteL(2,descriptorSizePckg);
	return ETrue;
	}

TInt CMMFAudioServerSession::DoGetDevSoundServerNameL(const RMmfIpcMessage& aMessage)
	{
	delete iDevSoundServerNameFlat;
	iDevSoundServerNameFlat = NULL;

	iDevSoundServerNameFlat = CBufFlat::NewL(32);
	RBufWriteStream stream;
	stream.Open(*iDevSoundServerNameFlat);
	CleanupClosePushL(stream);
	stream << iDevSoundServerName;
	CleanupStack::PopAndDestroy();//s

	aMessage.WriteL(2,iDevSoundServerNameFlat->Ptr(0));
	return ETrue;
	}


void CMMFAudioServerSession::SendEventToClient(/*TMMFAudioPolicyEvent& aEvent*/)
	{
	}



