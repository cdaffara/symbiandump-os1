// t_wsp.cpp
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <e32svr.h>
#include <e32math.h>
#include <f32file.h>
#include <es_sock.h>
#include <in_sock.h>


//
// Constants.
//

//#define _HOME
#ifdef _HOME
const TInt KWebPort = 80;
const TUint32 KSymbianWebServerIpAddress = INET_ADDR(192, 168, 0, 240);
const TInt KLocalHostPort = 9999;
const TUint32 KLocalHostIpAddress = INET_ADDR(127, 0, 0, 1);
_LIT(KSymbianWebServerName, "main");
_LIT(KAltSymbianWebServerName, "sunshare.office");
_LIT8(KSmallWebPage,"GET http://sunshare.office/index.html \r\n\r\n");
_LIT(KLogFile, "c:\\t_wsp.txt");
#else
const TInt KWebPort = 80;
const TUint32 KSymbianWebServerIpAddress = INET_ADDR(81, 89, 143, 203);
const TInt KLocalHostPort = 9999;
const TUint32 KLocalHostIpAddress = INET_ADDR(127, 0, 0, 1);
_LIT(KSymbianWebServerName, "www.symbian.com");
_LIT(KAltSymbianWebServerName, "www.symbian.com");
_LIT8(KSmallWebPage,"GET http://www.symbian.com/index.html \r\n\r\n");
_LIT(KLogFile, "c:\\t_wsp.txt");
#endif

const TInt KTestTransferDataSize = 8192;

//
// Globals.
//

RFs FileServerSession;
RFile LogFile;
RSocketServ SocketServerSession;
RTest TestConsole(_L("t_wsp"));


//
// Functions.
//

void RandomFill(TDes8& aBuf)
	{
	while (aBuf.Length() < (aBuf.MaxLength() - TInt(sizeof(TUint32))))
		{
		TUint32 rand = Math::Random();
		TPckgC<TUint32> randPckg(rand);
		aBuf.Append(randPckg);
		}
	while (aBuf.Length() < (aBuf.MaxLength()))
		{
		aBuf.Append(_L("p")); // Pack out last few bytes.
		}
	}

void ReceiveHtmlL(RSocket& aSock)
	{
	HBufC8* webPage = HBufC8::NewMaxLC(4096);

	TPtr8 des(webPage->Des());
	des = KSmallWebPage;

	TRequestStatus stat;
	aSock.Write(*webPage, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());

	TSockXfrLength HTTPHeaderLen;
	TInt rcount = 0;

	TBuf<128> infoPrintBuf;
	FOREVER
		{
		des.Zero();
		aSock.RecvOneOrMore(des,0,stat,HTTPHeaderLen);
		User::WaitForRequest(stat);
		if (stat != KErrNone)
			break;
		
		LogFile.Write(des);
		LogFile.Flush();
		rcount += webPage->Length();
		infoPrintBuf.Format(_L("\rRecved:%10d Bytes"), rcount);
		TestConsole.Printf(infoPrintBuf);
		}
	if (stat != KErrEof)
		{
		User::Leave(stat.Int());
		}

	CleanupStack::PopAndDestroy(webPage);
	}

void TcpOpenAndConnectC(RSocket& aSocket)
	{
	RHostResolver resolver;
	TestConsole(resolver.Open(SocketServerSession, KAfInet, KProtocolInetTcp) == KErrNone);
	CleanupClosePushL(resolver);
	TNameEntry nameEntry;
	TestConsole(resolver.GetByName(KSymbianWebServerName, nameEntry) == KErrNone);
	TNameRecord nameRecord(nameEntry());
	TInetAddr addr(TInetAddr::Cast(nameRecord.iAddr).Address(), KWebPort);
	CleanupStack::PopAndDestroy(); // resolver.
	TestConsole(aSocket.Open(SocketServerSession, KAfInet, KSockStream, KProtocolInetTcp) == KErrNone);
	CleanupClosePushL(aSocket);
	TRequestStatus stat;
	aSocket.Connect(addr, stat);
	User::WaitForRequest(stat);
	TestConsole(stat.Int() == KErrNone);
	}

void GetByAddress()
	{
	RHostResolver resolver;
	TestConsole(resolver.Open(SocketServerSession, KAfInet, KProtocolInetTcp) == KErrNone);
	CleanupClosePushL(resolver);
	TNameEntry nameEntry;
	TInetAddr testAddr(KSymbianWebServerIpAddress, KWebPort);
	testAddr.SetFamily(KAfInet);
	TestConsole(resolver.GetByAddress(testAddr, nameEntry) == KErrNone);
	TNameRecord nameRecord(nameEntry());
	TestConsole(nameRecord.iName.CompareF(KAltSymbianWebServerName) == KErrNone);
	CleanupStack::PopAndDestroy(); // resolver.
	}

void TcpActiveOpen()
	{
	RSocket socket;
	TcpOpenAndConnectC(socket);
	TRAPD(err, ReceiveHtmlL(socket));
	TestConsole(err == KErrNone);
	CleanupStack::PopAndDestroy(); // socket.
	}

void TcpGetLocalName()
	{
	RSocket socket;
	TcpOpenAndConnectC(socket);
	TSockAddr localName;
	socket.LocalName(localName);
	CleanupStack::PopAndDestroy(); // socket.
	}

void TcpGetRemoteName()
	{
	RSocket socket;
	TcpOpenAndConnectC(socket);
	TSockAddr remoteName;
	socket.RemoteName(remoteName);
	CleanupStack::PopAndDestroy(); // socket.
	}

void TcpListenAccept()
	{
	// Create a listening server socket and a socket to accept connections.
	RSocket listeningSocket;
	TestConsole(listeningSocket.Open(SocketServerSession, KAfInet, KSockStream, KProtocolInetTcp) == KErrNone);
	TInetAddr addr(KLocalHostIpAddress, KLocalHostPort);
	TestConsole(listeningSocket.Bind(addr) == KErrNone);
	TestConsole(listeningSocket.Listen(1) == KErrNone);
	RSocket serverConnectionSocket;
	TestConsole(serverConnectionSocket.Open(SocketServerSession) == KErrNone);
	TRequestStatus acceptStatus;
	listeningSocket.Accept(serverConnectionSocket, acceptStatus);

	// Create a client socket.
	RSocket clientSocket;
	TestConsole(clientSocket.Open(SocketServerSession, KAfInet, KSockStream, KProtocolInetTcp) == KErrNone);
	TRequestStatus connectStatus;
	clientSocket.Connect(addr,connectStatus);
	User::WaitForRequest(connectStatus);
	User::WaitForRequest(acceptStatus);
	TestConsole(connectStatus.Int() == KErrNone);
	TestConsole(acceptStatus.Int() == KErrNone);

	// Write some data from the client to the server.
	HBufC8* out = HBufC8::NewLC(KTestTransferDataSize);
	TPtr8 outPtr(out->Des());
	RandomFill(outPtr);
	TRequestStatus writeStatus;
	TRequestStatus readStatus;
	HBufC8* in = HBufC8::NewLC(KTestTransferDataSize);
	TPtr8 inPtr(in->Des());
	clientSocket.Write(*out, writeStatus);
	serverConnectionSocket.Read(inPtr, readStatus);
	User::WaitForRequest(writeStatus);
	User::WaitForRequest(readStatus);
	TestConsole(writeStatus.Int() == KErrNone);
	TestConsole(readStatus.Int() == KErrNone);
	TestConsole(in->Compare(*out) == KErrNone);
	clientSocket.Close();
	serverConnectionSocket.Close();

	// Re-connect the client socket.
	TestConsole(clientSocket.Open(SocketServerSession, KAfInet, KSockStream, KProtocolInetTcp) == KErrNone);
	TestConsole(serverConnectionSocket.Open(SocketServerSession) == KErrNone);
	listeningSocket.Accept(serverConnectionSocket, acceptStatus);
	clientSocket.Connect(addr,connectStatus);
	User::WaitForRequest(connectStatus);
	User::WaitForRequest(acceptStatus);
	TestConsole(connectStatus.Int() == KErrNone);
	TestConsole(acceptStatus.Int() == KErrNone);
	listeningSocket.Close();

	// Re-sent the data.
	inPtr.Zero();
	clientSocket.Read(inPtr, readStatus);
	serverConnectionSocket.Write(*out, writeStatus);
	User::WaitForRequest(writeStatus);
	User::WaitForRequest(readStatus);
	TestConsole(writeStatus.Int() == KErrNone);
	TestConsole(readStatus.Int() == KErrNone);
	TestConsole(in->Compare(*out) == KErrNone);

	// Cleanup.
	clientSocket.Close();
	listeningSocket.Close();
	CleanupStack::PopAndDestroy(2, out);
	}

void UdpSendReceive()
	{
	// Create a socket to receive data.
	RSocket receiveSocket;
	TestConsole(receiveSocket.Open(SocketServerSession, KAfInet, KSockDatagram, KProtocolInetUdp) == KErrNone);
	TInetAddr receiveAddress(KLocalHostIpAddress, KLocalHostPort);
	TestConsole(receiveSocket.Bind(receiveAddress) == KErrNone);
	TBuf8<32> receiveBuf;
	receiveBuf.SetMax();
	receiveBuf.Fill('a');
	TRequestStatus receiveStatus;
	TInetAddr sendAddress;
	receiveSocket.RecvFrom(receiveBuf, sendAddress, 0, receiveStatus);

	// Create a socket to send data.
	RSocket sendSocket;
	TestConsole(sendSocket.Open(SocketServerSession, KAfInet, KSockDatagram, KProtocolInetUdp) == KErrNone);
	TBuf8<32> sendBuf;
	sendBuf.SetMax();
	sendBuf.Fill('b');
	TRequestStatus sendStatus;
	sendSocket.SendTo(sendBuf, receiveAddress, 0, sendStatus);

	// Wait and check.
	User::WaitForRequest(receiveStatus);
	User::WaitForRequest(sendStatus);
	TestConsole(receiveStatus.Int() == KErrNone);
	TestConsole(sendStatus.Int() == KErrNone);
	TestConsole(receiveBuf.Compare(sendBuf) == KErrNone);

	// Cleanup.
	receiveSocket.Close();
	sendSocket.Close();
	}

void GetSetOption()
	{
	RSocket socket;
	TcpOpenAndConnectC(socket);
	TInt val;
	TestConsole(socket.GetOpt(KSoTcpNoDelay, KSolInetTcp, val) == KErrNone);
	val = !val;
	TestConsole(socket.SetOpt(KSoTcpNoDelay, KSolInetTcp, val) == KErrNone);
	TInt newVal;
	TestConsole(socket.GetOpt(KSoTcpNoDelay, KSolInetTcp, newVal) == KErrNone);
	TestConsole(val == newVal);
	TRAPD(err, ReceiveHtmlL(socket));
	TestConsole(err == KErrNone);
	CleanupStack::PopAndDestroy(); // socket.
	}

TInt InitialiseLC()
	{
	User::LeaveIfError(FileServerSession.Connect());
	CleanupClosePushL(FileServerSession);
	User::LeaveIfError(LogFile.Replace(FileServerSession, KLogFile,EFileShareAny | EFileWrite | EFileStreamText));
	CleanupClosePushL(LogFile);
	User::LeaveIfError(SocketServerSession.Connect());
	CleanupClosePushL(SocketServerSession);
	return 3;
	}

GLDEF_C void RunTestsL()
	{
	TInt numItemsOnCleanupStack = InitialiseLC();
//	GetByAddress();
	TcpActiveOpen();
	TcpGetLocalName();
	TcpGetRemoteName();
	TcpListenAccept();
	UdpSendReceive();
	GetSetOption();
	CleanupStack::PopAndDestroy(numItemsOnCleanupStack);
	TestConsole.Close();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler* scheduler = new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup = CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err, RunTestsL());
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
	}
