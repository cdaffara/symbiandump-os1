// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Server-side representation of a target client, which may have multiple 
// sessions associated with it.
// 


#include <e32base.h>
#include <bluetooth/logger.h>

#include "server.h"
#include "bearermanager.h"
#include "remconmessage.h"
#include "targetsession.h"

#include "targetclientprocess.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("tgclient");
#endif

TBool TargetSessionCompareUsingSupportedInterface(const TUid* aInterfaceUid, const CRemConTargetSession& aSession)
	{
	return aSession.InterfaceSupported(*aInterfaceUid);
	}

TBool TargetSessionCompareUsingSupportedMessage(const CRemConMessage* aMessage, const CRemConTargetSession& aSession)
	{
	return aSession.SupportedMessage(*aMessage);
	}

CRemConTargetClientProcess* CRemConTargetClientProcess::NewLC(TClientInfo& aClientInfo, TRemConClientId aClientId, CRemConServer& aServer, CBearerManager& aBearerManager)
	{
	LOG_STATIC_FUNC

	CRemConTargetClientProcess* result = new (ELeave) CRemConTargetClientProcess(aClientInfo, aClientId, aServer, aBearerManager);
	CleanupStack::PushL(result);
	return result;
	}

CRemConTargetClientProcess::CRemConTargetClientProcess(TClientInfo& aClientInfo, TRemConClientId aClientId, CRemConServer& aServer, CBearerManager& aBearerManager)
	: iClientInfo(aClientInfo),
	iClientId(aClientId),
	iServer(aServer),
	iBearerManager(aBearerManager),
	iPlayerInfoSet(EFalse),
	iTargetSessions(KMaxNumberTargetSessions)
	{
	LOG_FUNC
	}

CRemConTargetClientProcess::~CRemConTargetClientProcess()
	{
	LOG_FUNC

	iPlayerName.Close();
	iTargetSessions.Close();
	iServer.TargetClientClosed(*this);
	}

CRemConTargetSession* CRemConTargetClientProcess::NewSessionL(TUint aSessionId)
	{
	LOG_FUNC
	CRemConTargetSession* sess = NULL;

	// Only create a session if we havent reached our maximum.
	if (TargetSessionCount() < KMaxNumberTargetSessions)
		{
		sess = CRemConTargetSession::NewL(*this, iServer, iBearerManager, aSessionId);
		}
	else
		{
		LEAVEIFERRORL(KErrOverflow);
		}

	return sess;
	}


TInt CRemConTargetClientProcess::TargetSessionOpened(CRemConTargetSession& aSession)
	{
	LOG_FUNC
	LOG1(_L("\t&aSession = 0x%08x"), &aSession)

	// Register the session by appending it to our array and asking the server to
	// make an item for it in the record of which points in the connection history 
	// sessions are interested in.
	ASSERT(iTargetSessions.Count() < KMaxNumberTargetSessions);	// Should have been caught by now.

	// The append should never fail because the RPointerArray was constructed with a granularity of 
	// KMaxNumberTargetSessions items.
#ifdef _DEBUG	
	TInt err = iTargetSessions.Append(&aSession);
	ASSERT(err == KErrNone);
#else
	static_cast<void>(iTargetSessions.Append(&aSession));
#endif

	TInt ret = iServer.RegisterTargetSessionPointerToConnHistory(aSession);
			
	if ( ret != KErrNone )
		{
		iTargetSessions.Remove(iTargetSessions.Count() - 1);
		}
	else
		{
		// Session successfully registered.
		iServer.CancelShutdownTimer();
		}
	
	LOG1(_L("\tret = %d"), ret)
	return ret;
	}

void CRemConTargetClientProcess::TargetSessionClosed(CRemConTargetSession& aSession)
	{
	LOG_FUNC
	LOG1(_L("\t&aSession = 0x%08x"), &aSession)

	// Remove session from our list
	TInt sessionIndex = iTargetSessions.Find(&aSession);

	if (sessionIndex > KErrNotFound)
		{
		iTargetSessions.Remove(sessionIndex);

		// Inform server that session has dropped. 			
		iServer.TargetSessionClosed(*this, aSession);
		
		if (iTargetSessions.Count() > 0)
		    {
            iServer.TargetFeaturesUpdated(*this);
		    }
		
		}
	}

void CRemConTargetClientProcess::SetPlayerInformationL(const TPlayerTypeInformation& aPlayerType, const TDesC8& aPlayerName)
	{
	LOG_FUNC
	ASSERT_DEBUG(!HasPlayerInformation());

	if (!HasPlayerInformation())
		{
		iPlayerName.CreateL(aPlayerName);
		iPlayerType = aPlayerType;
		iPlayerInfoSet = ETrue;
		}
	}

void CRemConTargetClientProcess::InterfacesRegistered()
	{
	LOG_FUNC

	// If this client has not been made available yet, do so now.
	// Otherwise, notify the server that new interfaces have been registered. 
	if (!iClientAvailable)
		{
		iServer.TargetClientAvailable(*this);	
		iClientAvailable = ETrue;
		}
	else
		{
		iServer.TargetFeaturesUpdated(*this);
		}

	}

TBool CRemConTargetClientProcess::IsInterfaceTypeRegisteredByAnotherSession(CRemConTargetSession& aSession, TUid aInterfaceUid) const
	{
	LOG_FUNC

	TUint sessionCount = iTargetSessions.Count();
	for (TUint i=0; i < sessionCount; ++i)
		{
		if (aSession.Id() != iTargetSessions[i]->Id())
			{
			if (iTargetSessions[i]->InterfaceSupported(aInterfaceUid))
				{
				return ETrue;
				}
			}
		}

	return EFalse;
	}

TInt CRemConTargetClientProcess::ReceiveMessage(CRemConMessage& aMessage)
	{
	LOG_FUNC

	// Find the session supporting this message
	CRemConTargetSession* sess = FindSessionForMessage(aMessage);

	if (sess)
		{
		// Session found. Check session is able to handle this message.
		if (sess->CurrentReceiveMessage().Handle())
			{
			// Pass message to session and return the error code obtained.
			return sess->WriteMessageToClient(aMessage);
			}
		else
			{
			// Session not able to handle message at this time.
			return KErrNotReady;
			}
		}
	else
		{
		// No session supports this message
		return KErrArgument;
		}
	}

void CRemConTargetClientProcess::ConnectionsChanged()
	{
	LOG_FUNC

	// Notify each session
	TUint sessionCount = iTargetSessions.Count();
	for (TUint i = 0; i < sessionCount; ++i)
		{
		iTargetSessions[i]->ConnectionsChanged();
		}
	}

TInt CRemConTargetClientProcess::SupportedInterfaces(RArray<TUid>& aUids)
	{
	LOG_FUNC
	
	TInt err = KErrNone;
	aUids.Reset();

	// Gather the list of supported interfaces from each session.
	TUint sessionCount = iTargetSessions.Count();
	for (TUint i = 0 ; i < sessionCount && err == KErrNone; ++i)
		{
		err = iTargetSessions[i]->AppendSupportedInterfaces(aUids);
		}

	return err;
	}

TInt CRemConTargetClientProcess::SupportedBulkInterfaces(RArray<TUid>& aUids)
	{
	LOG_FUNC
	
	TInt err = KErrNone;
	aUids.Reset();

	// Gather the list of supported bulk interfaces from each session.
	TUint sessionCount = iTargetSessions.Count();
	for (TUint i = 0 ; i < sessionCount && err == KErrNone; ++i)
		{
		err = iTargetSessions[i]->AppendSupportedBulkInterfaces(aUids);
		}

	return err;
	}

TInt CRemConTargetClientProcess::SupportedOperations(TUid aInterfaceUid, RArray<TUint>& aOperations)
	{
	LOG_FUNC

	// Find the session supporting this interface.
	TInt sessIndex = FindSessionForInterface(aInterfaceUid);

	if (sessIndex >= KErrNone)
		{
		return iTargetSessions[sessIndex]->SupportedOperations(aInterfaceUid, aOperations);
		}
	else
		{
		// Interface not found, so operation not supported.
		return KErrNotSupported;
		}
	}

CRemConTargetSession* CRemConTargetClientProcess::FindSessionForMessage(const CRemConMessage& aMessage)
	{
	LOG_FUNC

	TInt result = iTargetSessions.Find(aMessage, TargetSessionCompareUsingSupportedMessage);
	if (result >= KErrNone)
		{
		return iTargetSessions[result];
		}
	else
		{
		return NULL;
		}

	}

TInt CRemConTargetClientProcess::FindSessionForInterface(TUid aInterfaceUid) const
	{
	LOG_FUNC

	return iTargetSessions.Find(aInterfaceUid, TargetSessionCompareUsingSupportedInterface);
	}

void CRemConTargetClientProcess::CompleteMessageForSession(const CRemConMessage& aMessage, CRemConTargetSession& aSession)
	{
	LOG_FUNC

	// Targets can only send responses or rejects.
	switch (aMessage.MsgType())
		{
	case ERemConResponse:
	case ERemConReject:
		aSession.CompleteSend();
		break;
	default:
		ASSERT_DEBUG(EFalse);
		break;
		}

	}

void CRemConTargetClientProcess::MrcmsoMessageSendResult(const CRemConMessage& /*aMessage*/, TInt /*aError*/)
	{
	LOG_FUNC
	
	// This method should never be called, as it is not required to support target client processes.
	ASSERT_DEBUG(EFalse);
	}

void CRemConTargetClientProcess::MrcmsoMessageSendOneOrMoreAttempt(const CRemConMessage& aMessage, TUint aNumRemotes)
	{
	LOG_FUNC

	// Notifications should not be received for reject messages, as the client did not request these be sent.
	ASSERT_DEBUG(aMessage.MsgType() != ERemConReject);
	
	// Find session and notify
	CRemConTargetSession* sess = FindSessionForMessage(aMessage);
	ASSERT_DEBUG(sess);
	
	// Session should not already be sending a message to n remotes
	ASSERT_DEBUG(sess->NumRemotesToTry() == 0);

	sess->NumRemotes() = 0;
	sess->NumRemotesToTry() = aNumRemotes;
	sess->SendError() = KErrNone;
	}

void CRemConTargetClientProcess::MrcmsoMessageSendOneOrMoreIncremental(const CRemConMessage& aMessage, TUint aNumRemotes)
	{
	LOG_FUNC

	// Notifications should not be received for reject messages, as the client did not request these be sent.
	ASSERT_DEBUG(aMessage.MsgType() != ERemConReject);
	
	// Find session and notify
	CRemConTargetSession* sess = FindSessionForMessage(aMessage);
	ASSERT_DEBUG(sess);

	if (sess->NumRemotesToTry() == 0)
		{
		MrcmsoMessageSendOneOrMoreAttempt(aMessage,aNumRemotes);
		}
	else
		{
		// No send should have yet been attempted
		ASSERT_DEBUG(sess->NumRemotes() == 0);

		sess->NumRemotesToTry() += aNumRemotes;
		}
	}
	
void CRemConTargetClientProcess::MrcmsoMessageSendOneOrMoreAttemptFailed(const CRemConMessage& aMessage, TInt aError)
	{
	LOG_FUNC

	// Notifications should not be received for reject messages, as the client did not request these be sent.
	ASSERT_DEBUG(aMessage.MsgType() != ERemConReject);
	
	// Find session and notify
	CRemConTargetSession* sess = FindSessionForMessage(aMessage);
	ASSERT_DEBUG(sess);
	
	// Session should not already be sending a message to n remotes
	ASSERT_DEBUG(sess->NumRemotesToTry() == 0);

	sess->NumRemotes() = 0;
	sess->SendError() = aError;
	CompleteMessageForSession(aMessage, *sess);
	}

void CRemConTargetClientProcess::MrcmsoMessageSendOneOrMoreResult(const CRemConMessage& aMessage, TInt aError)
	{
	LOG_FUNC

	// Notifications should not be received for reject messages, as the client did not request these be sent.
	ASSERT_DEBUG(aMessage.MsgType() != ERemConReject);
	
	// Find session and notify
	CRemConTargetSession* sess = FindSessionForMessage(aMessage);
	ASSERT_DEBUG(sess);
	
	// Ignore notification if client has been completed
	if (sess->NumRemotesToTry() > 0)
		{
		// Only set error if different from KErrNone
		if (aError == KErrNone)
			{
			++sess->NumRemotes();
			}
		else
			{
			sess->SendError() = aError;
			}

		--sess->NumRemotesToTry();
		if (sess->NumRemotesToTry() == 0)
			{
			CompleteMessageForSession(aMessage, *sess);
			}
		}
	}

void CRemConTargetClientProcess::MrcmsoMessageSendOneOrMoreAbandoned(const CRemConMessage& aMessage)
	{
	LOG_FUNC
	
	// Notifications should not be received for reject messages, as the client did not request these be sent.
	ASSERT_DEBUG(aMessage.MsgType() != ERemConReject);

	// Find session and notify
	CRemConTargetSession* sess = FindSessionForMessage(aMessage);
	ASSERT_DEBUG(sess);
	
	// Ignore notification if client has been completed
	if (sess->NumRemotesToTry() > 0)
		{
		// Do not adjust NumRemotes() as the message was not sent (but we still don't error the client!)
		--sess->NumRemotesToTry();
		if (sess->NumRemotesToTry() == 0)
			{
			CompleteMessageForSession(aMessage, *sess);
			}
		}
	}
