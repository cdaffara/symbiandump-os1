// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ACTIVESOCKET_H__
#define __ACTIVESOCKET_H__

#include <e32std.h>
#include <wapmessage.h>
#include "WapSwsTimeOut.h"


class CActiveSocketSMS;
class CActiveSocketUDP;
class MProgressNotify;

enum TWapMessageState
	{
	/**
	The state that is no outstanding request.
	*/
	EIdle,
	/**
	The state that the messaging API client is requesting data length
	*/
	ERequestingLength,
	/**
	The state the the messaging API client is requesting data
	*/
	ERequestingData,
	/**
	The state that the data size is ready to be read
	*/
	EPendingLength,
	/**
	The state that the messaging API client got the data length
	*/
	EGotLength,
	/**
	The state that the received data is ready to be read
	*/
	EPendingData,
	/**
	The state the data is being reading, but not finished
	*/
	EReading,
	/**
	The state that there are more data to be received from ESock (PRT 1.5)
	*/
	EContinuous
	};
enum TWapMessageType
	{
	/**
	Wsp message
	*/
	EWapWsp,
	/**
	Wdp message
	*/
	EWapWdp
	};
enum TWapActiveSocketState
	{
	/**
	No outstanding request state
	*/
	ESocketIdle,
	/**
	Waiting for the PDU length state
	*/
	ESocketWaitingForLength,
	/**
	Waiting for the PDU data state
	*/
	ESocketWaitingForData
	};
enum TWapNotificationEvent
	{
	/**
	The event that PDU length is ready.
	*/
	EPduLengthReceived,
	/**
	The event thst PDU us ready
	*/
	EPduReceived
	};

class TWapNotificationInfo
	{
public:
	inline TWapNotificationInfo(Wap::TBearer aBearer, TInt aError);
	Wap::TBearer iBearer;
	TInt iError;
	};

inline TWapNotificationInfo::TWapNotificationInfo(Wap::TBearer aBearer, TInt aError):
iBearer(aBearer), iError(aError)
	{
	}

typedef TPckgBuf<TWapNotificationInfo> TWapNotificationInfoBuf;

const TUint KMaxUdpBearerDataBufferLength=1024;

class CWapMessageRecord: public CBase
/** 
The Base class to represent the received WSP and WDP PDU
@internalComponent
@released
@since v8.0
*/
	{
public:
	static CWapMessageRecord* NewL(TWapMessageType aType);
	virtual ~CWapMessageRecord();
	TWapMessageState GetDataState();
	void SetDataState(TWapMessageState aStatus);
	void CreatePduBufferL(TBool aFixLengthFlag);
	TPckgBuf<TUint32>* GetPduSizeRef();
	void SetPduSize(TUint32 aLength);
	TUint32 GetPduSize();
	HBufC8*& GetPduPtr();
	/**
	To read the received Wdp pdu
	@internalComponent
	@released
	@since v8.0
	@param aBuffer(out) the buffer to contain the received wdp pdu
	@param aTruncated(out) the flag to represent if the data is truncated or not
	@returns KErrNone on successful completion, or one of the system error codes on failure.
	*/
	virtual TInt GetPduData(TDes8& aBuffer, TBool& aTruncated)=0;
	/**
	To unpack the received wdp pdu to wsp message.
	@internalComponent
	@released
	@since v8.0
	*/
	virtual void UnpackPduToWspDataL()=0;
	/**
	To read the wsp message from the buffer
	@internalComponent
	@released
	@since v8.0
	@param aWspHeader(out) the buffer to contain the wsp header
	@param aWspBody(out) the buffer to contain the wsp body
	@param iTransactionId(out) the received transaction ID
	@param aWspStatus(out) the received wsp status
	@returns KErrNone on successful completion, or one of the system error codes on failure.
	*/
	virtual TInt GetWspData(TDes8& aWspHeader, TDes8& aWspBody, TUint8& aTransactionId, TWSPStatus& aWspStatus)=0;
	virtual void CleanUpData();
public:
	/**The received WDP PDU Data buffer
	*/
	HBufC8* iPdu;
	/**The received WDP PDU length buffer
	*/
	TPckgBuf<TUint32> iDataLength;

protected:
	CWapMessageRecord();
	/**The state of the received the WSP or WDP data
	*/
	TWapMessageState iState;
	};

class CWspMessageRecord: public CWapMessageRecord
/** 
The class to represent the received WSP PDU
@internalComponent
@released
@since v8.0
*/
	{
public: 
	CWspMessageRecord();
	~CWspMessageRecord();
	void UnpackPduToWspDataL();
	TInt GetWspData(TDes8& aWspHeader, TDes8& aWspBody, TUint8& aTransactionId, TWSPStatus& aWspStatus);
	TInt GetPduData(TDes8& aBuffer, TBool& aTruncated);
	void CleanUpData();
private:
	/**
	The buffer for received Wsp header.
	*/
	HBufC8* iWspHeader;
	/**
	The buffer for received Wsp body.
	*/
	HBufC8* iWspBody;
	/**
	The received the transaction Id.
	*/
	TUint8 iTransactionId;
	/**
	The received WSP layer status
	*/
	TWSPStatus iWspStatus;
	/**
	The offset of the header for next reading
	*/
	TInt iHeaderOffset;
	/**
	The offset of the body for next reading
	*/
	TInt iBodyOffset;
	};

class CWdpMessageRecord: public CWapMessageRecord
/** 
The class to represent the received WDP PDU
@internalComponent
@released
@since v8.0
*/
	{
public:
	CWdpMessageRecord();
	~CWdpMessageRecord();
	void UnpackPduToWspDataL();
	TInt GetWspData(TDes8& aWspHeader, TDes8& aWspBody, TUint8& aTransactionId, TWSPStatus& aWspStatus);
	TInt GetPduData(TDes8& aBuffer, TBool& aTruncated);
	void CleanUpData();
private:
	/**
	The offset of the pdu for next reading
	*/
	TUint32 iPduOffset;
	};

class CActiveSocket: public CActive
/** 
The base class for the different bearer classes.
@internalComponent
@released
@since v8.0
*/
	{
public:
	static void NewL(RSocketServ& aSocketServ, RPointerArray<CActiveSocket>& aActiveSockets, Wap::TBearer aBearer, TWapMessageType aType, MProgressNotify* aNotify, Wap::TPort aLocalPort, RConnection* aConnection=NULL);
	static void NewL(RSocketServ& aSocketServ, RPointerArray<CActiveSocket>& aActiveSockets, Wap::TBearer aBearer, TWapMessageType aType, MProgressNotify* aNotify, const TSockAddr& aRemoteAddr, RConnection* aConnection=NULL);
	virtual ~CActiveSocket();
	virtual void ConstructL(TWapMessageType aType);
	/**
	Read the received Wdp pdu length.
	@internalComponent
	@released
	@since v8.0
	@returns KErrNone on successful completion, or one of the system error codes on failure.
	*/
	virtual TInt AwaitRecvDataSize() = 0;
	/**
	Read the received Wdp pdu.
	@internalComponent
	@released
	@since v8.0
	@returns KErrNone on successful completion, or one of the system error codes on failure.
	*/
	virtual TInt Receive() = 0;
	TSockAddr& GetLocalAddress();
	TInt GetLocalPort(Wap::TPort& aLocalPort);
	TInt GetRemoteAddress(HBufC8*& aAddr);
	TSockAddr& GetRemoteAddress();

	TWapMessageState GetDataState();
	void SetDataState(TWapMessageState aState);
	TUint32 GetPduSize();
	TInt GetPduData(TDes8& aBuffer, TBool& aTruncated);
	TInt GetWspData(TDes8& aWspHeader, TDes8& aWspBody, TUint8& aTransactionId, TWSPStatus& aStatus);	
	void UnpackPduToWspDataL();
	Wap::TBearer GetBearerType();
	RSocket& Socket();
	void CleanUpData();

public:
	/**The RSocket instance to send and receive WDP PDU.
	*/
	RSocket iSocket;
	/**The remote address for the last received message
	*/
	TSockAddr iRemoteAddr;
	/**The local address for the last received message
	*/
	TSockAddr iLocalAddr;

protected:
	CActiveSocket(RSocketServ& aSocketServ, Wap::TBearer aBearerType, MProgressNotify* aNotify, Wap::TPort aLocalPort);
	CActiveSocket(RSocketServ& aSocketServ, Wap::TBearer aBearerType, MProgressNotify* aNotify, const TSockAddr& aRemoteAddr, Wap::TPort aLocalPort);

protected:
	/**The data buffer to receive WDP or WSP message
	*/
	CWapMessageRecord* iMessageRecord;
	/**The RSocketServ reference from WAP message API
	*/
	RSocketServ& iSocketServ;
	/**The Bearer type
	*/
	Wap::TBearer iBearerType;
	/**The port number for the last received message
	*/
	Wap::TPort iLocalPort;
	/**The state of the bearer
	*/
	TWapActiveSocketState iSocketState;
	/**
	The instance that to be notified when data is received
	*/
	MProgressNotify* iNotify;
	/**
	Buffer for the first read from ESock
	*/
	TPtr8 iBuf;
	/**
	Reported remaining length for PRT 1.5 
	*/
	TSockXfrLength iRxlength;
	/**
	Buffer for continious read from ESock
	*/
	TPtr8 iBufCon;
	};

class CActiveSocketSMS : public CActiveSocket
/** 
The class represent the SMS bearer. It will open a RSocket over WapSMS protocol, send and 
receive WDP PDU.
@internalComponent
@released
@since v8.0
*/
{
public:
	CActiveSocketSMS(RSocketServ& aSocketServ, MProgressNotify* aNotify, Wap::TBearer aBearer, Wap::TPort aLocalPort);
	CActiveSocketSMS(RSocketServ& aSocketServ, MProgressNotify* aNotify, Wap::TBearer aBearer, const TSockAddr& aRemoteAddr);
	virtual ~CActiveSocketSMS();
	TInt AwaitRecvDataSize();
	TInt Receive();
	void ConstructL(TWapMessageType aType);
private:
	void RunL();
	void DoCancel();
};

class CActiveSocketUDP : public CActiveSocket
/** 
The class represent the UDP bearer. It will open a RSocket over UDP protocol, send and 
receive WDP PDU. It is not supported in released v8.0.
@internalComponent
@released
@since v8.0
*/
{
public:
	CActiveSocketUDP(RSocketServ& aSocketServ, MProgressNotify* aNotify, Wap::TBearer aBearer, Wap::TPort aLocalPort, RConnection* aConnection);
	CActiveSocketUDP(RSocketServ& aSocketServ, MProgressNotify* aNotify, Wap::TBearer aBearer, const TSockAddr& aRemoteAddr, RConnection* aConnection);
	virtual ~CActiveSocketUDP();
	TInt AwaitRecvDataSize();
	TInt Receive();
	void ConstructL(TWapMessageType aType);
private:
	void RunL();
	void DoCancel();
	/**
	The RConnection shared with the WAP message API client.
	*/
	RConnection* iConnection;
};

#endif // __ACTIVESOCKET_H__

