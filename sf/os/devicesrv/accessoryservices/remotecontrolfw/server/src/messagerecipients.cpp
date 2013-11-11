// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include "messagerecipients.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

CMessageRecipients* CMessageRecipients::NewL()
	{
	LOG_STATIC_FUNC
	return new(ELeave) CMessageRecipients();
	}

CMessageRecipients::CMessageRecipients()
:	iClientInfo(_FOFF(TClientInfo, iLink)),
	iIter(iClientInfo),
	iConstIter(iClientInfo)
	{
	LOG_FUNC
	}

CMessageRecipients::~CMessageRecipients()
	{
	LOG_FUNC
	iIter.SetToFirst();
	TClientInfo* client;
	while ((client = iIter++) != NULL)
		{
		iClientInfo.Remove(*client);
		delete client;
		}
	}

TUint& CMessageRecipients::TransactionId()
	{
	return iTransactionId;
	}

TSglQue<TClientInfo>& CMessageRecipients::Clients()
	{
	return iClientInfo;
	}

TClientInfoConstIter& CMessageRecipients::ConstIter()
	{
	return iConstIter;
	}

void CMessageRecipients::RemoveAndDestroyClient(const TClientInfo& aClientInfo)
	{
	iIter.SetToFirst();
	TClientInfo* client;
	while ((client = iIter++) != NULL)
		{
		if (client->ProcessId() == aClientInfo.ProcessId() && client->SecureId() == aClientInfo.SecureId())
			{
			iClientInfo.Remove(*client);
			delete client;
			}
		}
	}

CMessageRecipientsList* CMessageRecipientsList::NewL()
	{
	return new(ELeave) CMessageRecipientsList();
	}

CMessageRecipientsList::CMessageRecipientsList()
	:iMessages(_FOFF(CMessageRecipients, iLink)),
	iIter(iMessages)
	{
	}

CMessageRecipientsList::~CMessageRecipientsList()
	{
	iIter.SetToFirst();
	CMessageRecipients* message;
	while ((message = iIter++) != NULL)
		{
		iMessages.Remove(*message);
		delete message;
		}
	}

TSglQue<CMessageRecipients>& CMessageRecipientsList::Messages()
	{
	return iMessages;
	}

TSglQueIter<CMessageRecipients>& CMessageRecipientsList::Iter()
	{
	return iIter;
	}

void CMessageRecipientsList::RemoveAndDestroyMessage(const TUint aTransactionId)
	{
	iIter.SetToFirst();
	CMessageRecipients* message;
	while ((message = iIter++) != NULL)
		{
		if (message->TransactionId() == aTransactionId)
			{
			iMessages.Remove(*message);
			delete message;
			}
		}
	}

CMessageRecipients* CMessageRecipientsList::Message(TUint aTransactionId)
	{
	iIter.SetToFirst();
	CMessageRecipients* message;
	while ((message = iIter++) != NULL)
		{
		if (message->TransactionId() == aTransactionId)
			{
			break;
			}
		}
	return message;
	}
