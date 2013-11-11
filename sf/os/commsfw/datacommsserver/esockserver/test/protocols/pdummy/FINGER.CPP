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

#include <es_inet.h>
#include <e32test.h>

#define TCP_PORT_FINGER  (TUint16)79

#define IPADDR(a,b,c,d) (TUint32)(((a)<<24)|((b)<<16)|((c)<<8)|(d))

#define BUFFER_SIZE 8192


// #define TEST_ADDR IPADDR(194,129,1,226)
#define TEST_ADDR IPADDR(194,129,1,98)
#define NULL_ADDR IPADDR(0,0,0,0)
#define TEST_PORT TCP_PORT_TEST


typedef TBuf8<0x4000> TBuf_4000;


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


TInt RecvLine(RTest &aTest, TDes8 &aBuf, RSocket &aSock)
	{
	TInt offset=0;
	TText ch=0;

	do
		{
		TPtr8 ptr(NULL, 0);
		TSockXfrLength len;
		TRequestStatus stat;

		ptr.Set((TUint8 *)aBuf.Ptr()+offset, aBuf.Length()-offset, aBuf.Length()-offset);
		aSock.RecvOneOrMore(ptr,0,stat,len);
		User::WaitForRequest(stat);
		aTest(stat==KErrNone);
		TInt length=len();
		TInt n=0;
		while (length--)
			{
			ch = *(ptr.Ptr()+n);
			if (ch=='\r' || ch=='\n')
				break;
			++offset;
			++n;
			}
		}
	while (ch!='\r' && ch!='\n' );

	aBuf.SetLength(offset);
	return offset;
	}


TInt Finger()
//
//
//
	{
	RTest test(_L("eSock Emulation test - Simple Finger Server"));

	test.Title();

	User::AfterInMicroSeconds(400000);
   
	// Connect to the actual socket server
	TRequestStatus stat;
	RSocketServ ss;
	TInt ret = ss.Connect();
	test(ret==KErrNone);

	test.Start(_L("Create Server Socket")); // {

	RSocket server;
	ret = server.Open(ss, KAFInet, KSockStream, KProtocolInetTCP);
	test(ret==KErrNone);

	test.Next(_L("Starting server"));
	TInetAddr svraddr(NULL_ADDR, TCP_PORT_FINGER);

	server.Bind(svraddr, stat);
	User::WaitForRequest(stat);
	test(stat==KErrNone);

	// Set client to non-blocking
	server.SetOpt(KSOBlockingIO,NULL,KSOLSocket);

	server.Listen(5, stat);
	User::WaitForRequest(stat);
	test(stat==KErrNone);


	FOREVER
		{
		const TInt KBufLen=256;
		RSocket client;
		TSockAddr cliaddr;

		test.Next(_L("Opening null socket to accept with"));

		ret=client.Open(ss);
		test(ret==KErrNone);

		test.Next(_L("Awaiting connection"));
		// Wait for connection request
		server.SetOpt(KSOBlockingIO,NULL,KSOLSocket);
		client.Accept(server, cliaddr, stat);		
		User::WaitForRequest(stat);
		test(stat==KErrNone);

		test.Next(_L("Get request string"));
		// Set client to non-blocking
		server.SetOpt(KSONonBlockingIO,NULL,KSOLSocket);

		// Read request string from remote client
		TBuf<KBufLen> reqbuf;
  		reqbuf.SetLength(KBufLen);

		test(RecvLine(test, reqbuf, client)>0);
		test.Printf(_L("Request: %s\n"), reqbuf.PtrZ());

		test.Next(_L("Send answer text"));
		TBuf<100> tmpbuf;
  		tmpbuf.SetLength(0);
		tmpbuf.Format(_L("No information available on user \"%s\".\r\n"), reqbuf.PtrZ());
		client.Write(tmpbuf,stat);
		User::WaitForRequest(stat);
		test(stat==KErrNone);
		tmpbuf.Format(_L("\r\n"), reqbuf.PtrZ());
		client.Write(tmpbuf,stat);
		User::WaitForRequest(stat);
		test(stat==KErrNone);

		test.Next(_L("Close"));
		test(client.Close()==KErrNone);
		// break;
		}

	test.Next(_L("Closing"));
	test(server.Close()==KErrNone);

	test.End(); // }

   	return 0;
    
    }


	
TInt E32Main()
//
//
//
	{

	Finger();
	return 0;
	}


