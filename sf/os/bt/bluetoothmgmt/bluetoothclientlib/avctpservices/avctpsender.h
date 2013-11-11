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

#ifndef AVCTPSENDER_H
#define AVCTPSENDER_H

#include <e32base.h>
#include <es_sock.h>
#include <avctpservices.h>

// forward declaration
class CAvctpBody;

/**
AVCTP active sender
@internalComponent
*/
NONSHARABLE_CLASS(CAvctpSender) : public CActive
	{
public:
	
	static CAvctpSender* NewL(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, SymbianAvctp::TPid aPid);
	~CAvctpSender();

	TInt SendMessage(const TBTDevAddr& aBTDevice, 
					 SymbianAvctp::TTransactionLabel aTransactionLabel,
					 SymbianAvctp::TMessageType aType,
					 const TDesC8& aMessageInformation);
	
private:
	CAvctpSender(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, SymbianAvctp::TPid aPid);

	inline void NotifyMessageSent(const TBTDevAddr& aBTDevice, 
						   	SymbianAvctp::TTransactionLabel aTransactionLabel,   
							TInt aSendResult); 

	// From CActive
	void DoCancel();
	void RunL();
	
	// Other
	void ResetSendInfo();
	
private:
	MAvctpEventNotify& iNotify;	
	RSocket& iAvctpSocket;
	
	TAvctpSockAddr iAddr;
	SymbianAvctp::TTransactionLabel iCurrentTrans;
	};
	
#endif // AVCTPSENDER_H
