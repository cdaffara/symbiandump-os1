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

const TRemConAddress& CRemConMessage::Addr() const
	{
	return iAddr;
	}

TRemConAddress& CRemConMessage::Addr()
	{
	return iAddr;
	}

const TRemConClientId& CRemConMessage::Client() const
	{
	return iClient;
	}

TRemConMessageType CRemConMessage::MsgType() const
	{
	return iMsgType;
	}

TUid CRemConMessage::InterfaceUid() const
	{
	return iInterfaceUid;
	}

TUint CRemConMessage::OperationId() const
	{
	return iOperationId;
	}

const RBuf8& CRemConMessage::OperationData() const
	{
	return iData;
	}

RBuf8& CRemConMessage::OperationData()
	{
	return iData;
	}

TUint CRemConMessage::SessionId() const
	{
	return iSessionId;
	}

TUint& CRemConMessage::SessionId()
	{
	return iSessionId;
	}

TUint CRemConMessage::TransactionId() const
	{
	return iTransactionId;
	}

TUint& CRemConMessage::TransactionId()
	{
	return iTransactionId;
	}

TRemConMessageSubType& CRemConMessage::MsgSubType()
	{
	return iMsgSubType;
	}

TRemConMessageSubType CRemConMessage::MsgSubType() const
	{
	return iMsgSubType;
	}

TBool CRemConMessage::IsReliableSend() const
	{
	return iIsReliableSend;
	}
