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

/**
 @file
 @internalTechnology
*/

#ifndef AVCTPRECEIVER_H
#define AVCTPRECEIVER_H

#include <e32base.h>
#include <es_sock.h>

#include <avctpservices.h>
#include "avctppacketstatics.h"

// forward declaration
class CAvctpBody;
class MControlChannelNotify;

/**
AVCTP active receiver
@internalComponent
*/
NONSHARABLE_CLASS(CAvctpReceiver) : public CActive
	{
public:
	static CAvctpReceiver* NewL(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, SymbianAvctp::TPid aPid);
	static CAvctpReceiver* NewLC(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, SymbianAvctp::TPid aPid);
	~CAvctpReceiver();

private:
	MAvctpEventNotify& iNotify;
	RSocket& iAvctpSocket;

	enum TRecvState
		{
		EWaitingForPktHeader,
		EWaitingForMsgInfo,
		};
	
	TRecvState iRecvState;
	
	HBufC8* iRecvBuf;
	TPtr8  iRecvPtr;
	TSockXfrLength iXfrLength;

	SymbianAvctp::TPid iPid;

	// Current packet info
	TSockAddr iAddr;
	SymbianAvctp::TTransactionLabel iTransactionLabel;
	SymbianAvctp::TMessageType iMessageType;
	TBool iIpidBitSet;
private:
	CAvctpReceiver(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, SymbianAvctp::TPid aPid);
	void ConstructL();

	inline void NotifyMessageReceived(const TBTDevAddr& aBTDevice,
								SymbianAvctp::TTransactionLabel aTransactionLabel,
								SymbianAvctp::TMessageType aType,
								TBool aIpidBitSet,
							 	const TDesC8& aMessageInformation);

	// Function that issues RecvFrom 
	void ReceiveL(TRecvState aNextState, TInt aBufLen = SymbianAvctp::ENormalHeaderLength);
	
	// From CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
    // Other
    void ChangeState(TRecvState aNextState);
    void SetPacketInfo(TDes8& aHeader);
	};
	
#endif // AVCTPRECEIVER_H
