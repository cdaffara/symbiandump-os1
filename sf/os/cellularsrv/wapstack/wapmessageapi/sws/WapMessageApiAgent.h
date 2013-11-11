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

#ifndef __MSGAPIAGENT_H__
#define __MSGAPIAGENT_H__
#include "ActiveSocket.h"
#include "WapSwsTimeOut.h"

class MProgressNotify
/** 
Interface from the ActiveSocket to MessageApiAgent
@internalComponent
@released
@since v8.0
*/
	{
	public:
	/**
	The interface that will be called when WDP pdu length and data are received by CActiveSocket.
	@internalComponent
	@param aMessage the message type received from CActiveSocket
	@param aData the data received from CActiveSocket
	*/
	virtual void Notification(TInt aMessage, const TDesC8& aData) = 0;	
	};

class CWapAsyncCallBack : public CActive
	{
public:
	CWapAsyncCallBack( const TCallBack& aCallBack, TInt aPriority );
	~CWapAsyncCallBack();

public: // API
	void CallBack();

private:
	void RunL();
	void DoCancel();

private:
	TCallBack iCallBack;
	};

class CWapMessageApiAgent : public CBase, public MTimeOutNotify, public MProgressNotify
/** 
The base class of CWspMessageApiAgent and CWdpMessageApiAgent, which handle Wsp and Wdp
for SWS message APIs
@internalComponent
@released
@since v8.0
*/
{
public:
	virtual ~CWapMessageApiAgent();
	void ConstructL();
	CActiveSocket* GetActiveSocketByBearer(Wap::TBearer aBearer);
	CActiveSocket* GetActiveSocketByStatus(TWapMessageState aState);
	TInt Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool aSecure);
	TInt Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool aSecure, TInetAddr aInetAddr);
	TInt Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool aSecure, TInt aSocketServHandle, RConnection* aConnection);
	TInt Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure, TInetAddr aInetAddr);
	TInt Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure);
	TInt Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure, TInt aSocketServHandle, RConnection* aConnection);
	TInt GetLocalPort(Wap::TPort& aPort);
	TInt GetLocalAddress(HBufC8*& aLocalHost);
	TInt GetBearer(Wap::TBearer& aBearer);
	TInt GetServerAddress(HBufC8*& aRemoteHost);
	virtual void TimerExpired();
	void CancelRequest();
	static TInt CompleteReading(TAny* aAgent);
protected:
	CWapMessageApiAgent(TWapMessageType aMessageType);
	/**
	RSocketServ instance used by all the bearers.
	*/
	RSocketServ iSocketServ;
	/**
	The array to contain all the bearers
	*/
	RPointerArray<CActiveSocket> iBearers;
	/**
	The Wdp and Wsp receiving timer.
	*/
	CTimeOutTimer* iTimeoutTimer;
	/**
	Flag the show if there is outstanding request.
	*/
	TBool iRequestActive;
	/**
	The pointer to Wap message client's request status
	*/
	TRequestStatus* iRequestStatus;
	/**
	The last error code of active socket in reading state
	*/
	TInt iLastReadingError;
	/**
	The asynchronous completion of reading
	*/
	CWapAsyncCallBack* iAsyncReadCompletion;
	/**
	The bearer that receives that last PDU
	*/
	Wap::TBearer iLastPduBearer;
	/**
	Flag to show if the API has been connected or not
	*/
	TBool iIsOpen;
	/**
	The type of the Wap message handled by this API agent.
	*/
	TWapMessageType iMessageType;
private:
	TInt DoCompleteReading();
};

class CWspMessageApiAgent : public CWapMessageApiAgent
/** 
The class to handle Wsp PDU for SWS message APIs
@internalComponent
@released
@since v8.0
*/
{
public:
	static CWspMessageApiAgent* NewL();
	CWspMessageApiAgent();
	virtual ~CWspMessageApiAgent();
	void ConstructL();
	TInt SendWspMessage(Wap::TBearer aBearer, const TDesC8& aRemoteHost, Wap::TPort aRemotePort, TUint aMethod, const TDesC& aURI, const TDesC8& aReqHeaders, const TDesC8& aReqBody, TUint8 aTransactionId);
	TInt SendWspMessage(TUint aMethod, const TDesC& aURI, const TDesC8& aReqHeaders, const TDesC8& aReqBody, TUint8 aTransactionId);
	TInt ReceiveWspMessage(TDes8& aWspHeaders, TDes8& aWspBody, TPckgBuf<TUint8>& aPushIdPckg, TWSPStatus& aWspStatus, TRequestStatus& aReqStatus, TUint32 aTimeout);
	virtual void Notification(TInt aMessage, const TDesC8& aData);
private:
	/**
	The pointer to the WSP message client's requested header buffer
	*/
	TDes8* iClientReqHeaders;
	/**
	The pointer to the WSP message client's requested body buffer
	*/
	TDes8* iClientReqBody;
	/**
	The pointer to the WSP message client's requested transaction ID.
	*/
	TUint8*	iClientTransactionId;
	/**
	The pointer to the WSP message client's requested WSP status.
	*/
	TWSPStatus* iClientWspStatus;
};

class CWdpMessageApiAgent : public CWapMessageApiAgent
/** 
The class to handle Wdp PDU for SWS message APIs
@internalComponent
@released
@since v8.0
*/
{
public:
	static CWdpMessageApiAgent* NewL();
	CWdpMessageApiAgent();
	virtual ~CWdpMessageApiAgent();
	void ConstructL();
	TInt ReceiveWdpMessage(TDes8& aRemoteHost, Wap::TPort& aRemotePort, TDes8& aBuffer, TBool& aTruncated, TRequestStatus& aReqStatus, TUint32 aTimeout);
	TInt AwaitRecvDataSize(TPckg<TUint16>& aDataSizePckg, TRequestStatus& aReqStatus);
	TInt SendWdpMessage(const TDesC8& aBuffer, const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer);
	TInt SendWdpMessage(const TDesC8& aBuffer);
	virtual void Notification(TInt aMessage, const TDesC8& aData);
private:
	/**
	The pointer to Wdp message client's requested data buffer
	*/
	TDes8*	iClientPduBuffer;
	/**
	The pointer to Wdp message client's request data size
	*/
	TPckg<TUint16>* iClientDataSize;
	/**
	The pointer to client's requested remote host buffer
	*/
	TDes8* iClientRemoteHost;
	/**
	The pointer to client's requested remote port buffer
	*/
	Wap::TPort* iClientRemotePort;
	/**
	The pointer to client's requested flag.
	*/
	TBool* iClientTruncated;
};
#endif //__MSGAPIAGENT_H__
