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
// This is the header file for ESOCK test section 29
// This section contains stress and OOB testing of esock and tcp stack
// 
//


#if (!defined __ESOCKTEST_29_H__)
#define __ESOCKTEST_29_H__
#include "TestStepEsock.h"


// entries in ini file section for Test_28.<n>

_LIT(KSection29, "Test_29");
_LIT(KTestSocketType, "testSocketProtocolType");
_LIT(KBufSize, "bufSize");
_LIT(KTcp, "tcp");
_LIT(KUdp, "udp");
_LIT(KLoopback, "127.0.0.1");
_LIT(KLocal, "0.0.0.0");
const TInt KPort = 3441;


class CEsockTest29 : public CTestStepEsock
	{
public:
	static const TDesC& GetTestName();
	virtual enum TVerdict easyTestStepL();
	~CEsockTest29();
	TInt OpenSockets();
	TInt OpenTestTcpSocket();
	TInt OpenTestUdpSocket();
	TInt OpenDummySocket();
	void SetPort( TInetAddr &aAddr, const TInt aPort, const TDesC &aIPAddr );
	void ConnectSocketL( RSocket &aSocket, TInetAddr &aAddr );
	CEsockTest29() { iBufSize = 100; }

private:
	RSocketServ iSockServ;
	RSocket iDummySocket;
	RSocket iTestSocket;
	TPtrC iProtocol;
	TInt iBufSize;	
	};
	

class CEsockTest29_10 : public CTestStepEsock
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest29_10();
		
		virtual enum TVerdict easyTestStepL();
		
	};


class CEsockTest29_11 : public CTestStepEsock
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest29_11();
		
		virtual enum TVerdict easyTestStepL();
		
	};

#endif

