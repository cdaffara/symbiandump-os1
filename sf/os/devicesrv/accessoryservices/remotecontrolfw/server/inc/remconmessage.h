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

#ifndef REMCONMESSAGE_H
#define REMCONMESSAGE_H

#include <e32base.h>
#include <remconaddress.h>
#include <remcon/messagetype.h>
#include <remcon/clientid.h>

/**
Encapsulates a message internal to RemCon server.
*/
NONSHARABLE_CLASS(CRemConMessage) : public CBase
	{
public:
	/**
	Factory function.
	@param aAddr The remote address associated with the message (source or 
	destination).
	@param aMsgType The message type.
	@param aMsgSubType The message subtype. This should be ERemConDefault,
	except in the case of notify commands or notify responses
	@param aInterfaceUid The UID of the outer-layer interface to which the 
	message belongs.
	@param aOperationId The operation ID of the message.
	@param aData The data associated with the operation ID. On success, 
	ownership of aData is taken.
	@param aSessionId The unique ID of the CRemConSession associated with the 
	message (source or destination).
	@param aTransactionId The transaction of which this message is a part.
	@param aIsReliableSend Stores whether the message was sent using Send or SendUnreliable
	@return Ownership of a new CRemConMessage.
	*/
	static CRemConMessage* NewL(
		const TRemConAddress& aAddr,
		TRemConMessageType aMsgType,
		TRemConMessageSubType aMsgSubType,
		TUid aInterfaceUid,
		TUint aOperationId,
		const RBuf8& aData,
		TUint aSessionId,
		TUint aTransactionId,
		TBool aIsReliableSend);
	
	/**
	Factory function
	Overload for messages created outside of CRemConSession, where the value of
	aIsReliableSend is not relevant.
	*/
	static CRemConMessage* NewL(
		const TRemConAddress& aAddr,
		const TRemConClientId& aClient,
		TRemConMessageType aMsgType,
		TRemConMessageSubType aMsgSubType,
		TUid aInterfaceUid,
		TUint aOperationId,
		const RBuf8& aData,
		TUint aSessionId,
		TUint aTransactionId);
	
	/**
	Factory function.
	@param aMsg The data in this message is copied to a new CRemConMessage. 
	The new message does not take ownership of aMsg's OperationData, it makes 
	its own copy.
	@return Ownership of a new CRemConMessage.
	*/
	static CRemConMessage* CopyL(const CRemConMessage& aMsg);

	~CRemConMessage();

public:
	// const accessors
	inline const TRemConAddress& Addr() const;
	inline const TRemConClientId& Client() const;
	inline TRemConMessageType MsgType() const;
	inline TUid InterfaceUid() const;
	inline TUint OperationId() const;
	inline const RBuf8& OperationData() const;
	inline TUint SessionId() const;
	inline TUint TransactionId() const;
	inline TRemConMessageSubType MsgSubType() const;
	inline TBool IsReliableSend() const;
	
	// non-const accessors- the only things which may be changed about a 
	// message after it's been created
	inline TRemConAddress& Addr();
	inline RBuf8& OperationData();
	inline TUint& SessionId();
	inline TUint& TransactionId();
	inline TRemConMessageSubType& MsgSubType();
	
public:
	/** Link between elements of this type in a TSglQue. */
	TSglQueLink iLink;

private:
	CRemConMessage(
		const TRemConAddress& aAddr,
		const TRemConClientId& aClient,
		TRemConMessageType aMsgType,
		TRemConMessageSubType aMsgSubType,
		TUid aInterfaceUid,
		TUint aOperationId,
		const RBuf8& aData,
		TUint aSessionId,
		TUint aTransactionId,
		TBool aIsReliableSend);
	
	static CRemConMessage* NewL(
		const TRemConAddress& aAddr,
		const TRemConClientId& aClient,
		TRemConMessageType aMsgType,
		TRemConMessageSubType aMsgSubType,
		TUid aInterfaceUid,
		TUint aOperationId,
		const RBuf8& aData,
		TUint aSessionId,
		TUint aTransactionId,
		TBool aIsReliableSend);

private: // owned
	/** For incoming messages, the address it came from. For outgoing 
	messages, the address it's going to. */
	TRemConAddress iAddr;
	
	/** For incoming messages this is the client which the bearer requested
	the message be addressed to.  If the bearer did not address the command
	it is set to KNullClientId.
	*/
	TRemConClientId iClient;

	/** The message type (command or response). */
	TRemConMessageType iMsgType;

	/** The message subtype - could be
	Default - used for normal commands or responses
	Notify command awaiting interim/changed
	Notify interim/changed response
	*/
	TRemConMessageSubType iMsgSubType;

	/** The interface UID. */
	TUid iInterfaceUid;

	/** The operation ID. */
	TUint iOperationId;

	/** Arbitrary data associated with the operation. */
	RBuf8 iData;

	/** For outgoing commands, the ID of the sending session. */
	TUint iSessionId;

	/** The identifier of the transaction this message is part of. */
	TUint iTransactionId;
	
	/** Whether the message was send using Send() or SendUnreliable(), if applicable */
	TBool iIsReliableSend;
	
	};

#include "remconmessage.inl"

#endif // REMCONMESSAGE_H
