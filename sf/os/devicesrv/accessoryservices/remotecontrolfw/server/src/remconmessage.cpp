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

/**
 @file
 @internalComponent
*/

#include "remconmessage.h"
#include "utils.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

CRemConMessage* CRemConMessage::NewL(
	const TRemConAddress& aAddr,
	TRemConMessageType aMsgType,
	TRemConMessageSubType aMsgSubType,
	TUid aInterfaceUid,
	TUint aOperationId,
	const RBuf8& aData,
	TUint aSessionId,
	TUint aTransactionId,
	TBool aIsReliableSend)
	{
	LOG_STATIC_FUNC
	return new(ELeave) CRemConMessage(
		aAddr,
		KNullClientId,
		aMsgType,
		aMsgSubType,
		aInterfaceUid,
		aOperationId,
		aData,
		aSessionId,
		aTransactionId,
		aIsReliableSend
		);
	}

CRemConMessage* CRemConMessage::NewL(
	const TRemConAddress& aAddr,
	const TRemConClientId& aClient,
	TRemConMessageType aMsgType,
	TRemConMessageSubType aMsgSubType,
	TUid aInterfaceUid,
	TUint aOperationId,
	const RBuf8& aData,
	TUint aSessionId,
	TUint aTransactionId)
	{
	LOG_STATIC_FUNC
	return CRemConMessage::NewL(
		aAddr,
		aClient,
		aMsgType,
		aMsgSubType,
		aInterfaceUid,
		aOperationId,
		aData,
		aSessionId,
		aTransactionId,
		EFalse
		);
	}

CRemConMessage* CRemConMessage::NewL(
	const TRemConAddress& aAddr,
	const TRemConClientId& aClient,
	TRemConMessageType aMsgType,
	TRemConMessageSubType aMsgSubType,
	TUid aInterfaceUid,
	TUint aOperationId,
	const RBuf8& aData,
	TUint aSessionId,
	TUint aTransactionId,
	TBool aIsReliableSend)
	{
	LOG_STATIC_FUNC
	return new(ELeave) CRemConMessage(
		aAddr,
		aClient,
		aMsgType,
		aMsgSubType,
		aInterfaceUid,
		aOperationId,
		aData,
		aSessionId,
		aTransactionId,
		aIsReliableSend
		);
	}

CRemConMessage::CRemConMessage(
	const TRemConAddress& aAddr,
	const TRemConClientId& aClient,
	TRemConMessageType aMsgType,
	TRemConMessageSubType aMsgSubType,
	TUid aInterfaceUid,
	TUint aOperationId,
	const RBuf8& aData,
	TUint aSessionId,
	TUint aTransactionId,
	TBool aIsReliableSend)
:	iAddr(aAddr),
	iClient(aClient),
	iMsgType(aMsgType),
	iMsgSubType(aMsgSubType),
	iInterfaceUid(aInterfaceUid),
	iOperationId(aOperationId),
	iSessionId(aSessionId),
	iTransactionId(aTransactionId),
	iIsReliableSend(aIsReliableSend)
	{
	LOG_FUNC
	iData.Assign(aData);
	}

CRemConMessage::~CRemConMessage()
	{
	LOG_FUNC
	iData.Close();
	}

CRemConMessage* CRemConMessage::CopyL(const CRemConMessage& aMsg)
	{
	LOG_STATIC_FUNC
	// Allocate a new heap descriptor with a copy of that in aMsg, then make a 
	// new CRemConMessage to take ownership of it.
	RBuf8 newData;
	newData.CreateL(aMsg.OperationData());
	
	CleanupClosePushL(newData);
	CRemConMessage* msg = NewL(
		aMsg.Addr(),
		aMsg.Client(),
		aMsg.MsgType(),
		aMsg.MsgSubType(),
		aMsg.InterfaceUid(),
		aMsg.OperationId(),
		newData,
		aMsg.SessionId(),
		aMsg.TransactionId(),
		aMsg.IsReliableSend()
		);
	CLEANUPSTACK_POP1(&newData);	
	return msg;
	}
