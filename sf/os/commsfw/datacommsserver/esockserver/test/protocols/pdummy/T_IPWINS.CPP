// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// #include <es_inet.h>
#include <e32test.h>


#define TCP_PORT_CHARGEN (TUint16)19
#define TCP_PORT_ECHO	 (TUint16)7
#define TCP_PORT_DISCARD (TUint16)9
#define TCP_PORT_FINGER  (TUint16)79
#define TCP_PORT_TEST	 (TUint16)20000
#define TCP_CONNECT_BACK_PORT (TUint16)20003

#define IPADDR(a,b,c,d) (TUint32)(((a)<<24)|((b)<<16)|((c)<<8)|(d))

#define BUFFER_SIZE 8192

const TUint KDefaultHeapSize=0x1000;
const TUint KNumSockets=10;

TInetAddr HostAddr;

// #define TEST_ADDR IPADDR(194,129,1,226)
#define TEST_ADDR IPADDR(194,129,1,6)
#define NULL_ADDR IPADDR(0,0,0,0)
#define TEST_PORT TCP_PORT_TEST


typedef TBuf8<0x4000> TBuf_4000;

RTest test(_L("eSock Emulation test"));


struct TSocketThreadArg
	{
	RSemaphore* iSem;
	TUint iNumSockets;
	};

TInt socketThread(TAny * anArg)
//
//
//
	{

	RTest test(_L("eSock client test sub thread - creating sockets"));
	RSocketServ ss;
	TInt ret=ss.Connect();
    test(ret==KErrNone);

	TInt numSockets=((TSocketThreadArg *)anArg)->iNumSockets;

	RSocket* sock=new RSocket[numSockets];
	RHostResolver* hostR=new RHostResolver[numSockets];
	RServiceResolver* servR=new RServiceResolver[numSockets];

	for (TInt i=0;i<numSockets;i++)
		{
		ret=sock[i].Open(ss,KAFInet,KSockStream,KProtocolInetTCP);
		test(ret==KErrNone);
		}

	((TSocketThreadArg *)anArg)->iSem->Signal();
	User::WaitForAnyRequest();

	return KErrNone;
	}


void StripeMem(TDes8 &aBuf,TUint aStartChar,TUint anEndChar)
//
// Mark a buffer with repeating byte pattern
//
	{

//	__ASSERT_ALWAYS(aStartChar<=anEndChar,Panic(EBadArg));

	if (aStartChar==anEndChar)
		{
		aBuf.Fill(aStartChar);
		return;
		}

	TUint character=aStartChar;
	for (TInt i=0;i<aBuf.Length();i++)
		{
		aBuf[i]=(TText8)character;
		if(++character>anEndChar)
			character=aStartChar;
		}
	}

TInt test0()
//
// Name resolver tests
//
	{
	test.Start(_L("eSock Emulation test - Name resolution"));

	// Connect to the actual socket server
	RSocketServ ss;
	TInt ret=ss.Connect();
	test(ret==KErrNone);

	test.Next(_L("Create host resolver"));
	RHostResolver hr;
	ret=hr.Open(ss,KAFInet,KSockStream);
	test(ret==KErrNone);

	test.Next(_L("Get host name"));
	TBuf<20> name;
	ret=hr.GetHostName(name);
	test(ret==KErrNone);
	test.Printf(_L("Hostname: %S\r\n"),&name);

	TNameRecord t;
	TNameEntry addrEnt(t);
	ret=hr.GetByName(name,addrEnt);
	test(ret==KErrNone);

	HostAddr=addrEnt().iAddr;
	TUint32 ad=ByteOrder::Swap32(HostAddr.Address());
	test.Printf(_L("Address: %d.%d.%d.%d"),(ad&0x000000ff),(ad&0x0000ff00)>>8,(ad&0x00ff0000)>>16,(ad&0xff000000)>>24);
	test.Printf(_L(" (0x%X.0x%X.0x%X.0x%X) \r\n"),(ad&0x000000ff),(ad&0x0000ff00)>>8,(ad&0x00ff0000)>>16,(ad&0xff000000)>>24);
	hr.Close();
	ss.Close();
	test.End(); // }

   	return 0;
    
    }



TInt test1()
//
// Test connection and disconnection
//
	{
	test.Start(_L("eSock Emulation test - connect/disconnect"));

	// Connect to the actual socket server
	RSocketServ ss;
	TInt ret = ss.Connect();
	test(ret==KErrNone);

	test.Next(_L("Create socket")); // {

	RSocket sock;
	ret = sock.Open(ss, KAFInet, KSockStream, KProtocolInetTCP);
	test(ret==KErrNone);

	test.Next(_L("Connecting"));
	HostAddr.SetPort(TEST_PORT);
	TRequestStatus status;
	sock.Connect(HostAddr, status);
	User::WaitForRequest(status);
	test(status==KErrNone);

	test.Next(_L("Closing"));
	sock.Close();

	ss.Close();

	test.End(); // }

   	return 0;
    
    }


TInt test2()
//
// Test read and write
//
	{
	test.Start(_L("eSock Emulation test - read/write"));

	// Connect to the actual socket server
	RSocketServ ss;
	TInt ret = ss.Connect();
	test(ret==KErrNone);

	test.Next(_L("Create socket")); // {

	RSocket sock;
	ret = sock.Open(ss, KAFInet, KSockStream, KProtocolInetTCP);
	test(ret==KErrNone);

	test.Next(_L("Connecting"));
	HostAddr.SetPort(TEST_PORT);
	TRequestStatus status;
	sock.Connect(HostAddr, status);
	User::WaitForRequest(status);
	test(status==KErrNone);

	TRequestStatus readStatus;
	TRequestStatus writeStatus;

	test.Next(_L("Writing"));
	TBuf_4000* temp=new TBuf_4000;
	TBuf_4000& outBuf=*temp;
	outBuf.SetLength(100);
	StripeMem(outBuf, 'A', 'Z');
	sock.Write(outBuf,writeStatus);
	
	TRequestStatus stat;
	sock.SetOpt(KSOBlockingIO,KSOLSocket);
	User::WaitForRequest(writeStatus);
	test(writeStatus==KErrNone);

	test.Next(_L("Reading"));
	TBuf_4000* temp2=new TBuf_4000;
	TBuf_4000& inBuf=*temp2;
	inBuf.SetLength(100);
	sock.Read(inBuf,readStatus);
	User::WaitForRequest(readStatus);
	test(status==KErrNone);
	test(readStatus==KErrNone);

	test.Next(_L("Checking Reply"));
	test(outBuf.Compare(inBuf)==0);

	test.Next(_L("Closing"));
	sock.Close();

	delete temp;
	delete temp2;
	test.End(); // }

   	return 0;
    
    }

void test3()
//
// Test that everything is funky when we close sockets by killing a thread
//
	{

	test.Start(_L("Checking thread termintaion clean up code"));
	RSemaphore s;
	s.CreateLocal(0);
	RThread t;

	TSocketThreadArg a;
	a.iSem=&s;
	a.iNumSockets=KNumSockets;

	test.Next(_L("Creating sub thread"));
	t.Create(_L("socketThread"),socketThread,KDefaultStackSize,KDefaultHeapSize,KDefaultHeapSize,&a);
	t.Resume();
	s.Wait();
	test.Next(_L("Killing Sub thread"));
	t.Kill(KErrNone);
	t.Close();
	test.End();
	}
		

TInt test4()
//
// Stuff about a megabyte down its neck.
//
	{
	test.Start(_L("Read/write multiple"));

	User::After(400000);
   
	// Connect to the actual socket server
	RSocketServ ss;
	TInt ret = ss.Connect();
	test(ret==KErrNone);

	test.Next(_L("Create socket")); // {

	RSocket sock;
	ret = sock.Open(ss, KAFInet, KSockStream, KProtocolInetTCP);
	test(ret==KErrNone);

	test.Next(_L("Connecting"));
	TInetAddr addr(HostAddr);
	TRequestStatus status;
	sock.Connect(addr, status);
	User::WaitForRequest(status);
	test(status==KErrNone);

	TRequestStatus stat;
	sock.SetOpt(KSOBlockingIO,KSOLSocket);

	TRequestStatus writeStatus;
	TBuf_4000* temp=new TBuf_4000;
	TBuf_4000& outBuf=*temp;
	outBuf.SetLength(BUFFER_SIZE);
	StripeMem(outBuf, 'A', 'Z');

	TRequestStatus readStatus;
	TBuf_4000* temp2=new TBuf_4000;
	TBuf_4000& inBuf=*temp2;
	inBuf.SetLength(BUFFER_SIZE);

	test.Next(_L("Reading and Writing"));
	for (int i=0; i<128; i++)
		{

		sock.Write(outBuf,writeStatus);
		User::WaitForRequest(writeStatus);

		sock.Read(inBuf,readStatus);
		User::WaitForRequest(readStatus);

		test(writeStatus==KErrNone);
		test(readStatus==KErrNone);

		test(outBuf.Compare(inBuf)==0);
		test.Printf(_L("Written %d K\r"),(i*BUFFER_SIZE)/1024);
		}

	test.Printf(_L("\n"));

	test.Next(_L("Closing"));
	sock.Close();

	test.End(); // }
	delete temp;
	delete temp2;

   	return 0;
    
    }

void test5()
	{
	test.Start(_L("Dual socket Read/write multiple"));

	RSocketServ ss;
	TInt ret = ss.Connect();
	test(ret==KErrNone);

	test.Next(_L("Create sockets"));

	RSocket sock1;
	ret = sock1.Open(ss, KAFInet, KSockStream, KProtocolInetTCP);
	test(ret==KErrNone);

	RSocket listener;
	ret = listener.Open(ss, KAFInet, KSockStream, KProtocolInetTCP);
	test(ret==KErrNone);
	TInetAddr addr(HostAddr);
	addr.SetPort(TCP_CONNECT_BACK_PORT+1);

	TRequestStatus status;
	ret=listener.Bind(addr);
	test(ret==KErrNone);

	test.Next(_L("Start listening"));

	ret=listener.Listen(5);
	test(ret==KErrNone);

	RSocket sock2;
	ret = sock2.Open(ss);
	test(ret==KErrNone);

	test.Next(_L("Accept"));

	TRequestStatus acceptStat;
	TInetAddr from;
	sock2.Accept(listener,from,acceptStat);
	
	test.Next(_L("Start connect"));

	addr.SetPort(TCP_CONNECT_BACK_PORT);
	TRequestStatus connectStat;
	sock1.Connect(addr,connectStat);

	User::WaitForRequest(connectStat);
	test(connectStat==KErrNone);
	User::WaitForRequest(acceptStat);
	test(acceptStat==KErrNone);

	TBuf_4000* temp=new TBuf_4000;
	TBuf_4000& outBuf=*temp;
	outBuf.SetLength(BUFFER_SIZE);

	TRequestStatus readStat;
	TRequestStatus writeStat;
	TBuf_4000* temp2=new TBuf_4000;
	TBuf_4000& inBuf=*temp2;
	inBuf.SetLength(BUFFER_SIZE);
		test.Printf(_L("\n"));

	test.Next(_L("Reading and Writing"));
	for (int i=0; i<128; i++)
		{

		sock1.Write(outBuf,writeStat);
		User::WaitForRequest(writeStat);
		test.Printf(_L("\rWritten %d K"),(i*BUFFER_SIZE)/1024);

		sock2.Read(inBuf,readStat);
		User::WaitForRequest(readStat);

		test(writeStat==KErrNone);
		test(readStat==KErrNone);

		test(outBuf.Compare(inBuf)==0);
		StripeMem(outBuf, 'A'+i%13, 'Z');

		test.Printf(_L(" read %d K"),(i*BUFFER_SIZE)/1024);
		}

	test.Printf(_L("\n"));

	sock1.Close();

	sock2.Close();

	listener.Close();

	delete temp;
	delete temp2;
	test.End();
	}

TInt E32Main()
//
//
//
	{

	test.Title();

	test.Start(_L("ESock tests"));

	test0();
	test1();
	test2();
	test3();
	test4();
	test5();

	test.End();
	return 0;
	}


