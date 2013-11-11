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

#ifndef REMCONSERIALBEARER_H
#define REMCONSERIALBEARER_H

#include <e32base.h>
#include <c32comm.h>
#include <remcon/remconbearerplugin.h>
#include <remcon/remconbearerinterface.h>
#include <remcon/messagetype.h>
/*#ifndef __WINS__
#include <usbman.h>
#endif
*/
#include "sendobserver.h"
#include "receiveobserver.h"

class CSender;
class CReceiver;

/**
Concrete serial bearer.
*/
NONSHARABLE_CLASS(CRemConSerialBearer) : public CRemConBearerPlugin, 
							public MRemConBearerInterface,
							public MSendObserver, 
							public MReceiveObserver
	{
public:
	static CRemConSerialBearer* NewL(TBearerParams& aParams);
	~CRemConSerialBearer();

private: // from CRemConBearerPlugin
	TAny* GetInterface(TUid aUid);

private: // from MRemConBearerInterface
	TInt GetResponse(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr);
	TInt SendCommand(TUid aInterfaceUid, 
		TUint aCommand, 
		TUint aTransactionId,  
		RBuf8& aData, 
		const TRemConAddress& aAddr);
	TInt GetCommand(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aCommand, 
		RBuf8& aData, 
		TRemConAddress& aAddr);
	TInt SendResponse(TUid aInterfaceUid, 
		TUint aOperationId, 
		TUint aTransactionId, 
		RBuf8& aData, 
		const TRemConAddress& aAddr);
	void ConnectRequest(const TRemConAddress& aAddr);
	void DisconnectRequest(const TRemConAddress& aAddr);
	void ClientStatus(TBool aControllerPresent, TBool aTargetPresent);
	TSecurityPolicy SecurityPolicy() const;

private: // from MSendObserver
	void MsoSendComplete(TInt aError);

private: // from MReceiveObserver
	void MroReceiveComplete(TInt aError);

private:
	CRemConSerialBearer(TBearerParams& aParams);
	void ConstructL();

private: // utility
	TInt DoSend(TUid aInterfaceUid, 
		TUint aOperationId, 
		RBuf8& aData, 
		TRemConMessageType aMsgType);
	void ConnectL();
	void Receive();
	void ClosePort();
	void DoGetResponseL(TUid& aInterfaceUid, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr);
	void DoGetCommandL(TUid& aInterfaceUid,
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr);

private: // owned
/*#ifndef __WINS__
	RUsb iUsb;
#endif
*/	RCommServ iCommServ;
	RComm iComm;

	CSender* iSender;
	CReceiver* iReceiver;

	// The format of a serial bearer message is:
	// 012345678901234567890123456789
	// 0xaaaaaaaa 0xbb XXX YYYYYYYYYY
	// Where 
	//	aaaaaaaa is the interface UID
	//	bb is the operation ID
	//	XXX is Cmd or Rsp
	//	YYYYYYYYYY is any associated data
	// The message is always KRemConSerialBearerMessageLength bytes long.
	static const TUint KRemConSerialBearerMessageLength = 30;

	// The message we may be currently sending.
	TBuf8<KRemConSerialBearerMessageLength> iOutMsg;

	// The message we may be currently receiving.
	TBuf8<KRemConSerialBearerMessageLength> iInMsg;

	// Last received and corrected-decoded message.
	TUid iInterfaceUid;
	TUint iOperationId;
	TRemConMessageType iMsgType;
	TBuf8<10> iData; // This is the length of the associated data in our naive 
	// protocol.

	// The ID of the current transaction- RemCon gives us this, and expects us 
	// to remember it.
	TUint iTransactionId;
	};

#endif // REMCONSERIALBEARER_H
