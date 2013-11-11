// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 4
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection4.h"


// Test step 4.1
const TDesC& CEsockTest4_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test4.1");
	
	return ret;
	}

CEsockTest4_1::~CEsockTest4_1()
	{
	}

enum TVerdict CEsockTest4_1::easyTestStepPreambleL()
	{
	if (iEsockSuite->GetSocketListCount() > 1)
		{
		CloseSockets();
		}
	
	if (iEsockSuite->GetSocketListCount() < 1)
		{
		const TPtrC &sectName = SectionName(_L("Test_4.1"));
		
		// get size of listening queue
		if (!GetIntFromConfig(sectName, _L("qSize"), iSize))
			{
			return EFail;
			}
		
		// get local ip address
		TInetAddr addrLocal;
		if (!GetIpAddressFromConfig(sectName, _L("ipAddressLocal"), addrLocal))
			{
			return EFail;
			}
		
		// get remote ip address
		if (!GetIpAddressFromConfig(sectName, _L("ipAddressRemote"), iAddrRemote))
			{
			return EFail;
			}

		// get port number
		if (!GetIntFromConfig(sectName, _L("port"), iPort))
			{
			return EFail;
			}
		
		// set port number
		iAddrRemote.SetPort(iPort);
		
		// open socket and listen for connect requests
		TInt nRet = OpenListeningSocketL(addrLocal, iSockIndex, iPort, iSize);
		if (KErrNone != nRet)
			{
			if (KErrServerTerminated == nRet)
				{
				User::Leave(EFail);
				}
			return EFail;
			}
		}
	
	return EPass;
	}

enum TVerdict CEsockTest4_1::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());

	// open <nSize> active sockets
	TInt sockIndexIgnore;  // socket index - returned value we can ignore
	for (TInt n = 0; n < iSize; n++)
		{
		TESTL(KErrNone == OpenActiveSocketL(iAddrRemote, sockIndexIgnore, iPort));
		}
	
	// open another active socket
	RSocket sock1;
	CleanupClosePushL(sock1);
	TInt nRet = sock1.Open(iEsockSuite->iSocketServer, KAfInet,KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// add socket to list
	sockIndexIgnore=iEsockSuite->AddSocketToListL(sock1);
	CleanupStack::Pop();
	
	// make connect request
	TRequestStatus stat;
	sock1.Connect(iAddrRemote, stat);
	User::WaitForRequest(stat);
	nRet = stat.Int();
	TESTEL(KErrTimedOut == nRet, nRet);
	
	// accept one of the outstanding requests
	TESTL(KErrNone == AcceptConnectionL(sockIndexIgnore, iSockIndex));
	
	// make another connect request
	TESTL(KErrNone == OpenActiveSocketL(iAddrRemote, sockIndexIgnore, iPort));
	
	// accept one of the outstanding requests
	TESTL(KErrNone == AcceptConnectionL(sockIndexIgnore, iSockIndex));
	
	return EPass;
	}


// Test step 4.2
const TDesC& CEsockTest4_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test4.2");
	
	return ret;
	}

CEsockTest4_2::~CEsockTest4_2()
	{
	}

enum TVerdict CEsockTest4_2::easyTestStepPreambleL()
	{
	if (iEsockSuite->GetSocketListCount() < 1)
		{
		const TPtrC &sectName = SectionName(_L("Test_4.2"));
		// get local ip address
		TInetAddr addrLocal, addrRemote;
		if (!GetIpAddressFromConfig(sectName, _L("ipAddressLocal"), addrLocal))
			{
			return EFail;
			}
		
		// get ip address to connect to (usually loopback)
		if (!GetIpAddressFromConfig(sectName, _L("ipAddressRemote"), addrRemote))
			{
			return EFail;
			}
		
		// get port number
		TInt port;
		if (!GetIntFromConfig(sectName, _L("port"), port))
			{
			return EFail;
			}
		
		// open socket and listen for connect requests
		TInt nRet = OpenListeningSocketL(addrLocal, iSockIndex, port);
		if (KErrNone != nRet)
			{
			if (KErrServerTerminated == nRet)
				{
				User::Leave(EFail);
				}
			return EFail;
			}
		}

	return EPass;
	}

enum TVerdict CEsockTest4_2::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// open active socket and make connect request
	// by not doing this allow time for the CancelAccept() after the Accept() below
	// TESTL(OpenActiveSocketL(addrRemote, sockIndexIgnore, port));

	// open a blank socket
	RSocket sock;
	CleanupClosePushL(sock);
	TInt nRet = sock.Open(iEsockSuite->iSocketServer);
	TESTEL(KErrNone == nRet, nRet);
	
	// accept a connection on the listening socket
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex).Accept(sock, stat);
	
	// cancel the accept before completion
	iEsockSuite->GetSocketHandle(iSockIndex).CancelAccept();
	
	User::WaitForRequest(stat);
	nRet = stat.Int();
	TESTEL(KErrCancel == nRet, nRet);

	CleanupStack::PopAndDestroy();
	return EPass;
	}

