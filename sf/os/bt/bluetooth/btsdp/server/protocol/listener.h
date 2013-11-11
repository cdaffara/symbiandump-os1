// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LISTENER_H
#define LISTENER_H


#include <e32std.h>
#include <es_sock.h>
#include <btmanclient.h>

#include "reqhandler.h"

//Timeouts in microseconds 
const TUint KInitialAcceptDelay = 0x400;//was 1
const TUint KMaxAcceptDelay = 0x1000000;//was0x7fffffff;

class CSdpConnection;
class CSdpListener;

NONSHARABLE_CLASS(TSdpPdu)
	{
public:
	TSdpPdu();
	TUint8	iPduId;
	TUint16	iTransId;
	TPtr8	iParams;
	};

NONSHARABLE_CLASS(CSdpClientTimer) : public CTimer
	{
	public:
		static CSdpClientTimer* NewL(CSdpConnection& aConnection);
		void Start();
		void ReStart();
	private:
		CSdpClientTimer(CSdpConnection& aConnection);
		void ConstructL();
		void RunL();
	private:
		CSdpConnection& iConnection;
	};
	
NONSHARABLE_CLASS(CSdpConnectionReader) : public CActive
	{
friend class CSdpConnection;
public:
	static CSdpConnectionReader* NewL(CSdpConnection& aConnection);
	~CSdpConnectionReader();

private:
	CSdpConnectionReader(CSdpConnection& aConnection);
	void ConstructL();
	
	void RunL();
	void DoCancel();

private:
	CSdpConnection& iConnection;
	};

NONSHARABLE_CLASS(CSdpConnection) : public CActive
	{
friend class CSdpListener;
public:
	enum TConnState
		{
		EReading,
		EHandling,
		EWriting,
		EShutting
		};

	static CSdpConnection* NewL(RSocket& aSocket, CSdpDatabase& aDatabase);
	~CSdpConnection();
	void ClientTimeout();
	void ReadComplete(TInt aStatus);
	void CancelRead();

private:
	CSdpConnection(RSocket& aSocket, CSdpDatabase& aDatabase);
	void ConstructL();

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	void QueRead();
	void WriteResponse();
	void ParseNextPacketL();
	void HandleReadL();
	void ShutdownConnection();

private:
	RSocket iSocket;
	TConnState iConnState;
	CSdpDatabase& iDatabase;
	HBufC8 *iReadFragmentHBuf;
	TPtr8 iReadFragmentBuf;
	TSockXfrLength iRemainingDataLength;
	HBufC8 *iReadTotalHBuf;
	TPtr8 iReadTotalBuf;
	HBufC8 *iWriteHBuf;
	TPtr8 iWriteBuf;
	TSdpPdu	iRequest;
	TSdpPdu	iResponse;

	CSdpClientTimer* iSDPClientTimer;	// for disconnecting inactive clients

	// For the listener Q of Connections
	TDblQueLink       iLink;
	
	CSdpConnectionReader* iReader;
	};


NONSHARABLE_CLASS(CSdpListener) : public CActive
	{
public:
	static CSdpListener* NewL(RSocketServ& aSockServ, TInt aQueSize, CSdpDatabase& aDatabase);
	static CSdpListener* NewLC(RSocketServ& aSockServ, TInt aQueSize, CSdpDatabase& aDatabase);
	~CSdpListener();

	void TryRestartL();

private:
	CSdpListener(RSocketServ& aSockServ, CSdpDatabase& aDatabase);
	void ConstructL(TInt aQueSize);
	void OpenListeningSocketL();
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	void QueAcceptL();

private:

	RSocketServ iSockServ;
	RSocket iListener;
	RSocket iAccepter;
	CSdpDatabase& iDatabase;

	TBool	iIsAcceptDelayed;
	TUint	iAcceptDelay;
	RTimer	iDelayAcceptTimer;

	// List of connections
	TDblQue<CSdpConnection>      iConns;

	TInt iQueSize;
	};

inline TSdpPdu::TSdpPdu() 
	: iParams(0,0)
	{
	}



#endif
