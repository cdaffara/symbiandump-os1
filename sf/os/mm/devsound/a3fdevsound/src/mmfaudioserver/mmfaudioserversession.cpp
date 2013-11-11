// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <s32mem.h>
#include <e32math.h>
#include "mmfaudioserversession.h"
#include "mmfaudioclientserver.h"
#include "mmfaudioserver.h"

// -----------------------------------------------------------------------------
// CMMFAudioServerSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMMFAudioServerSession* CMMFAudioServerSession::NewL(RHandleBase& aDevSoundSessionHandle)
	{
	CMMFAudioServerSession* self = new(ELeave) CMMFAudioServerSession(aDevSoundSessionHandle);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// void CMMFAudioServerSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMMFAudioServerSession::ConstructL()
	{
	}

// -----------------------------------------------------------------------------
// CMMFAudioServerSession::CMMFAudioServerSession
// -----------------------------------------------------------------------------
//
CMMFAudioServerSession::CMMFAudioServerSession(
		RHandleBase& aDevSoundSessionHandle):iDevSoundSessionHandle(aDevSoundSessionHandle)
	{
	}


// -----------------------------------------------------------------------------
// CMMFAudioServerSession::CreateL
// Creates a new server-side session
// -----------------------------------------------------------------------------
//
void CMMFAudioServerSession::CreateL(const CMmfIpcServer& aServer)
	{
	CMmfIpcSession::CreateL(aServer);
	CMMFAudioServer* server = static_cast<CMMFAudioServer*>(const_cast<CMmfIpcServer*>(&aServer));
	server->IncrementSessionId();
	iAudioServerSessionId = server->AudioServerSessionId();
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
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


// -----------------------------------------------------------------------------
// CMMFAudioServerSession::ServiceL
// Handles the client requests
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
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

//  End of File
