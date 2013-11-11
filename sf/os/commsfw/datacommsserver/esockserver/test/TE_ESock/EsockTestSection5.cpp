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
// This contains ESock Test cases from section 5
// EPOC includes
// 
//

#include <e32base.h>
#include <in_sock.h>
#include <e32math.h>

//anonymous namespace
namespace
    {
    //once the aRefBoolFlag set to false it never gets set again...
    void setResultFlagBool(const TBool aFlag, TBool& aRefBoolFlag)
        {
        if (aRefBoolFlag)
            {
            aRefBoolFlag = aFlag;
            }
        }
    
    //once the aRefBoolFlag set to false it never gets set again...
     void setResultFlagInt(const TInt aFlag, TBool& aRefBoolFlag)
        {
        if (aRefBoolFlag)
            {
            if (aFlag != KErrNone)
                {
                    aRefBoolFlag = EFalse;
                }
            }
        }
        
        const TUint KMaxTestStepCounter = 5;
    }
// Test system includes
#include "EsockTestSection5.h"

// Test step SendData
// this test is used by the integration test scripts
// and is based on esock test 5.3
// This test step sends TCP and UDP (if enabled in ini file) data
const TDesC& CEsockSendData::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"SendData");
	
	return ret;
	}

CEsockSendData::~CEsockSendData()
	{
	}

enum TVerdict CEsockSendData::easyTestStepL()
	{
	TInetAddr addrRemote, addr;
	TRequestStatus stat(KErrNone);
	TInt connectTimer, Port;
	TBool UdpEnable;
	
	TInt testStepCounter = 0;
	TBool testStepResult;
	TBool socketConnected = EFalse;
	TBool udpBindOK = EFalse;
	
	do
	    {
	    testStepResult = ETrue;
	    
    	// get ip address to connect to (on echo port)
    	TESTL(GetIpAddressFromConfig(_L("Test_Common"), _L("ipAddress"), addrRemote));
    	
    	// get the port number
    	TESTL(GetIntFromConfig(_L("SendData"), _L("Port"), Port));
    	addrRemote.SetPort(Port);
    	
    	// format for display 
    	TBuf<39> AddrBuf;
    	addrRemote.OutputWithScope(AddrBuf);
    	
    	// check if UDP test enabled
    	TBool res = GetBoolFromConfig(_L("SendData"), _L("UdpEnable"), UdpEnable );
    	
    	if (( res == EFalse ) || ( UdpEnable == EFalse ))
    		{
    		Logger().WriteFormat( _L("UDP test disabled"));
    		UdpEnable = EFalse;
    		}
    	
    	// get a value for the connection guard timer 
    	TESTL(GetIntFromConfig(_L("SendData"), _L("connectTimer"), connectTimer));
    	
    	// create timer to use as a guard timer
    	RTimer GuardTimer;
    	GuardTimer.CreateLocal();			// for this thread
    	TRequestStatus TimerStatus;			// status to monitor timer
    	TInt RepeatCount = 0;				// repeat counter 
    	TInt WaitTimeout = 5000000;

        if (!socketConnected)
            {
        	// try 5 times (sometimes it fails because of test network)
        	for(RepeatCount=0; RepeatCount <=5;  RepeatCount++)
        		{
        		// connect
        		(iEsockSuite->GetSocketHandle(1)).Connect(addrRemote, stat);
        		
        		// start timer
        		GuardTimer.After(TimerStatus,connectTimer * 1000000); 
        		
        		// wait for either timer or connect
        		User::WaitForRequest(stat, TimerStatus);
        		GuardTimer.Cancel();
        		User::WaitForAnyRequest();
        		
        		// check connect result
        		if (stat.Int() == KRequestPending)
        			{
        			(iEsockSuite->GetSocketHandle(1)).CancelConnect();
        			Logger().WriteFormat( _L("Socket connect to %S port %d has not completed within %d seconds (connectTimer from ini) "), 
        				&AddrBuf,
        				Port,
        				connectTimer ); 
        			User::After(WaitTimeout);
        			continue; // next attempt
        			}
        		else if(stat.Int() != KErrNone)
        			{
        			Logger().WriteFormat( _L("Socket connect failed with error %d"), stat.Int());
        			if(RepeatCount <= 5)
        				{
        				Logger().WriteFormat( _L("Try to reconnect. RepeatCount %d"), RepeatCount);
        				User::After(WaitTimeout);
        				continue; // next attempt
        				}
        		    else
        		        {
        		        setResultFlagInt(stat.Int(), testStepResult);
        		        }
        			}
        		else
        		    {
        		    socketConnected = ETrue;
        		    }
        		
        		break; // exit "for"
        		}
        	
        	TESTEL((stat.Int()==KErrNone),stat.Int());
            }
    	
    	// get number of packets and size of packets 
    	TInt PacketSize = 0;
    	TESTL(GetIntFromConfig(_L("SendData"), _L("PacketSize"), PacketSize));
    	TInt NoPackets = 0;
    	TESTL(GetIntFromConfig(_L("SendData"), _L("NoPackets"), NoPackets));
    	Logger().WriteFormat( _L("Sending to %S port %d TCP data %d packets of %d bytes = %d"), 
    		&AddrBuf, Port, NoPackets, PacketSize, NoPackets * PacketSize);
    	
    	// set up data buffers
    	HBufC8 * writebuf = HBufC8::NewMaxLC( PacketSize );
    	HBufC8 * readbuf  = HBufC8::NewMaxLC( PacketSize );
    	TPtr8 ptrWritebuf = writebuf->Des();
    	TPtr8 ptrReadbuf = readbuf->Des();
    	
    	// small buffer for data pattern to be sent
    	TBuf8<50> Data;
    	
    	// send data (NoPackets packets of PacketSize bytes)
    	TInt recvCount = 0;
    	for (TInt i = 0; i < NoPackets; i++)
    		{
    		// initialise data
    		Data.Format(_L8("TCP-packet:%d helloworld"),i);
    		ptrWritebuf.Repeat( Data );
    		
    		// write data 
    		GuardTimer.After(TimerStatus,connectTimer * 1000000); 
    		(iEsockSuite->GetSocketHandle(1)).Write(ptrWritebuf, stat);
    		User::WaitForRequest(stat, TimerStatus);
    		GuardTimer.Cancel();
    		User::WaitForAnyRequest();
    		
    		if (stat == KRequestPending)
    			{
    			(iEsockSuite->GetSocketHandle(1)).CancelWrite();
    			Logger().WriteFormat(_L("Timeout on Tcp Send packet %d after %d seconds"), i, connectTimer);
    			setResultFlagInt(stat.Int(), testStepResult);
    			}
    		else if (stat != KErrNone)
    			{
    			Logger().WriteFormat(_L("Tcp Send packet %d failed with error %d"), i, stat.Int());
    			setResultFlagInt(stat.Int(), testStepResult);
    			}
    		TESTEL( stat == KErrNone, stat.Int() );
    		
    		// read data (from echo server)
    		GuardTimer.After(TimerStatus,connectTimer * 1000000); 
    		(iEsockSuite->GetSocketHandle(1)).Read(ptrReadbuf, stat);
    		User::WaitForRequest(stat, TimerStatus);
    		GuardTimer.Cancel();
    		User::WaitForAnyRequest();
    		
    		TESTE( stat == KErrNone, stat.Int() );
    		if (stat == KRequestPending)
    			{
    			(iEsockSuite->GetSocketHandle(1)).CancelRead();
    			Logger().WriteFormat(_L("Timeout on Tcp Recv packet %d after %d seconds"), i, connectTimer);
    			setResultFlagInt(stat.Int(), testStepResult);
    			}
    		else if (stat != KErrNone)
    			{
    			Logger().WriteFormat(_L("Tcp Recv packet %d failed with error %d"), i, stat.Int());
    			setResultFlagInt(stat.Int(), testStepResult);
    			}
    		else
    			{
    			// compare the data
    			TEST( ptrWritebuf.Compare( ptrReadbuf ) == 0);
    			recvCount+=ptrReadbuf.Length();
    			}
    		}
    	
    	// check the total received.
    	TEST(recvCount==(NoPackets*PacketSize));
    	
    	//** UDP STARTS HERE
    	// test UDP if required
    	if ( UdpEnable )
    		{
    		TESTL(GetIntFromConfig(_L("SendData"), _L("UdpPacketSize"), PacketSize));
    		
    		// get the port number
    		// Can't send back on Echo port as it would enter an infinite loop
    		if (!GetIntFromConfig(_L("SendData"), _L("UdpPort"), Port))
    			// use default
    			Port = 9200;
    		
    		Logger().WriteFormat( _L("Sending to %S Port %d UDP data %d packets of %d bytes = %d"), 
    			&AddrBuf, Port, NoPackets, PacketSize, NoPackets * PacketSize);
    		
    		
    		// and make some new buffers the correct size
    		HBufC8 * UdpWritebuf = HBufC8::NewMaxLC( PacketSize );
    		HBufC8 * UdpReadbuf  = HBufC8::NewMaxLC( PacketSize );
    		TPtr8 ptrUdpWritebuf = UdpWritebuf->Des();
    		TPtr8 ptrUdpReadbuf = UdpReadbuf->Des();
    		
    		// open a udp socket
    		RSocket UdpSock;
    		TInt nRet = UdpSock.Open(iEsockSuite->iSocketServer, KAfInet, KSockDatagram, KProtocolInetUdp);
    		if (nRet != KErrNone)
    			{
    			Logger().WriteFormat(_L("Failed to open socket: return value = <%d>"), nRet);
    			setResultFlagBool(EFail, testStepResult);
    			//return EFail;  
    			}
    		
    		if (testStepResult)
    		    {
            		addr.SetPort(Port);
            		
            		// Bind
            		TInt result = UdpSock.SetOpt(KSoUdpSynchronousSend, KSolInetUdp, 1 );
            		
            		UdpSock.SetOpt(KSoReuseAddr, KSolInetIp, KMaxTestStepCounter);
            		
            		TESTEL( KErrNone ==result,result);
            		
            		if (!udpBindOK)
            		    {
                		result = UdpSock.Bind(addr);
                		TESTEL( KErrNone ==result,result);
                		udpBindOK = ETrue;
            		    }
            		
            		
            		// write & read back the data
            		recvCount = 0;
            		for (TInt j = 0; j < NoPackets; j++)
            			{
            			// initialise data
            			Data.Format(_L8("UDP-packet:%d helloworld"),j);
            			ptrUdpWritebuf.Repeat( Data );
            			
            			// write data 
            			GuardTimer.After(TimerStatus,connectTimer * 1000000); 
            			UdpSock.SendTo(ptrUdpWritebuf, addrRemote, 0, stat);
            			User::WaitForRequest(stat, TimerStatus);
            			GuardTimer.Cancel();
            			User::WaitForAnyRequest();
            			
            			if (stat == KRequestPending)
            				{
            				UdpSock.CancelSend();
            				Logger().WriteFormat(_L("Timeout on Udp Send packet %d after %d seconds"), j, connectTimer);
            				setResultFlagInt(stat.Int(), testStepResult);
                            
            				}
            			else if (stat != KErrNone)
            				{
            				Logger().WriteFormat(_L("Udp Send packet %d failed with error %d"), j, stat.Int());
            				setResultFlagInt(stat.Int(), testStepResult);
            				}
            			
            			// read data (from echo server)
            			GuardTimer.After(TimerStatus,connectTimer * 1000000); 
            			UdpSock.RecvFrom(ptrUdpReadbuf, addr, 0, stat);
            			User::WaitForRequest(stat, TimerStatus);
            			GuardTimer.Cancel();
            			User::WaitForAnyRequest();
            			
            			if (stat == KRequestPending)
            				{
            				UdpSock.CancelRecv();
            				Logger().WriteFormat(_L("Timeout on Udp Recv packet %d after %d seconds"), j, connectTimer);
            				setResultFlagInt(stat.Int(), testStepResult);
            				}
            			else if (stat.Int() != KErrNone) 
            				{
            				Logger().WriteFormat(_L("Udp Recv packet %d failed with error %d"), j, stat.Int());
            				setResultFlagInt(stat.Int(), testStepResult);
            				}
            			//TEST( stat == KErrNone );  ** We could have packet loss with UDP in real life
            			else
            				{
            				// compare the data
            				TEST( ptrUdpWritebuf.Compare( ptrUdpReadbuf ) == 0);
            				recvCount += ptrUdpReadbuf.Length();
            				}
            			}
            		
            		Logger().WriteFormat(_L("Udp data sent %d bytes, data received %d bytes"),(NoPackets*PacketSize),recvCount);
            		
            		// check the total received (95 per cent is allowable for us)
            		//TESTE( recvCount >= (NoPackets*PacketSize), (NoPackets*PacketSize) );
            				
            		// close the UDP socket
                    UdpSock.Close();
    		    }
            
    		// get rid of the old buffers 
    		CleanupStack::PopAndDestroy();	//	UdpWritebuf;
    		CleanupStack::PopAndDestroy();	//	UdpReadbuf;
    		}
    		
    	CleanupStack::PopAndDestroy();	//	writebuf;
    	CleanupStack::PopAndDestroy();	//	readbuf;
    	
    	
    	if (!testStepResult)
    	    {
    	    Logger().WriteFormat(_L("TestStep failed %d time(s) from %d. Trying further..."), testStepCounter, KMaxTestStepCounter);
    	    
    	    ++testStepCounter;
    	    }
    	
	    }
	    while ( (testStepCounter < KMaxTestStepCounter) &&
	          (!testStepResult) ); //big while...
         

    if ( testStepResult )
        {
	    return EPass;
        }
    else
        {
        return EFail;
        }
	}
	
	
// Test step SendTcpData
// this test is used by the integration test scripts
// and is based on esock test 5.3
// This test step sends TCP data only
const TDesC& CEsockSendTcpData::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"SendTcpData");
	
	return ret;
	}

CEsockSendTcpData::~CEsockSendTcpData()
	{
	}

enum TVerdict CEsockSendTcpData::easyTestStepL()
	{
	TInetAddr addrRemote;
	TRequestStatus stat;
	TInt connectTimer, Port;
	
	// get ip address to connect to (on echo port)
	TESTL(GetIpAddressFromConfig(_L("Test_Common"), _L("ipAddress"), addrRemote));
	
	// get the port number
	TESTL(GetIntFromConfig(_L("SendData"), _L("Port"), Port));
	addrRemote.SetPort(Port);
	
	// format for display 
	TBuf<39> AddrBuf;
	addrRemote.OutputWithScope(AddrBuf);
	
	// get a value for the connection guard timer 
	TESTL(GetIntFromConfig(_L("SendData"), _L("connectTimer"), connectTimer));
	
	// create timer to use as a guard timer
	RTimer GuardTimer;
	GuardTimer.CreateLocal();			// for this thread
	TRequestStatus TimerStatus;			// status to monitor timer
	
    // connect
    (iEsockSuite->GetSocketHandle(1)).Connect(addrRemote, stat);
	
	// start timer
	GuardTimer.After(TimerStatus,connectTimer * 1000000); 
	
	// wait for either timer or connect
	User::WaitForRequest(stat, TimerStatus);
	GuardTimer.Cancel();
	User::WaitForAnyRequest();
	
	// check connect result
	if (stat!=KErrNone)
		{
		// either connect failed or timer expired
		// because it may be the timer cancel connect
		(iEsockSuite->GetSocketHandle(1)).CancelConnect();
		
		Logger().WriteFormat( _L("Socket connect has not completed within %d seconds (connectTimer from ini) "), connectTimer ); 
		
		// report this failure and leave
		TESTL((stat==KErrNone));
		}
	
	// get number of packets and size of packets 
	TInt PacketSize = 0;
	TESTL(GetIntFromConfig(_L("SendData"), _L("PacketSize"), PacketSize));
	TInt NoPackets = 0;
	TESTL(GetIntFromConfig(_L("SendData"), _L("NoPackets"), NoPackets));
	Logger().WriteFormat( _L("Sending to %S TCP data %d packets of %d bytes = %d"), 
		&AddrBuf, NoPackets, PacketSize, NoPackets * PacketSize);
	
	// set up data buffers
	HBufC8 * writebuf = HBufC8::NewMaxLC( PacketSize );
	HBufC8 * readbuf  = HBufC8::NewMaxLC( PacketSize );
	TPtr8 ptrWritebuf = writebuf->Des();
	TPtr8 ptrReadbuf = readbuf->Des();
	
	// initialise data
	TBuf8<50> Data;
	
	// send data (NoPackets packets of PacketSize bytes)
	TInt recvCount = 0;
	for (TInt i = 0; i < NoPackets; i++)
		{
		// initialise data
		Data.Format(_L8("TCP-packet:%d helloworld"),i);
		ptrWritebuf.Repeat( Data );
		
		// write data 
		GuardTimer.After(TimerStatus,connectTimer * 1000000); 
		(iEsockSuite->GetSocketHandle(1)).Write(ptrWritebuf, stat);
		User::WaitForRequest(stat, TimerStatus);
		GuardTimer.Cancel();
		User::WaitForAnyRequest();
		
		if (stat != KErrNone) Logger().WriteFormat(_L("Timeout on Tcp Send after %d seconds"), connectTimer);
		TESTL( stat == KErrNone );
		
		// read data (from echo server)
		GuardTimer.After(TimerStatus,connectTimer * 1000000); 
		(iEsockSuite->GetSocketHandle(1)).Read(ptrReadbuf, stat);
		User::WaitForRequest(stat, TimerStatus);
		GuardTimer.Cancel();
		User::WaitForAnyRequest();
		
		if (stat != KErrNone) Logger().WriteFormat(_L("Timeout on Tcp Recv after %d seconds"), connectTimer);
		TESTL( stat == KErrNone );
		
		// compare the data
		TESTL( ptrWritebuf.Compare( ptrReadbuf ) == 0);
		
		recvCount+=ptrReadbuf.Length();
		}
	
	// check the total received.
	TESTL(recvCount==(NoPackets*PacketSize));
	
	CleanupStack::PopAndDestroy();	//	writebuf;
	CleanupStack::PopAndDestroy();	//	readbuf;
	
	return EPass;
	}
	
// Open Udp Socket
const TDesC& CEsockOpenUdpSocket::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"OpenUdpSocket");
	
	return ret;
	}

CEsockOpenUdpSocket::~CEsockOpenUdpSocket()
	{
	}

enum TVerdict CEsockOpenUdpSocket::easyTestStepL()
	{
	TInt nRet;
	TInt nSockets; // number of sockets to open
	TInetAddr addr;
	TBuf<32> buf;
	TInt i;
	
	RSocket sock;
	
	// get number of sockets from script
	TESTL(GetIntFromConfig(SectionName(_L("Test_1.2")), _L("numSockets"), nSockets));
	
	Logger().WriteFormat(_L("Open <%d> UDP socket(s)"), nSockets);
	for (i = 0; i < nSockets; i++)
		{
		
		// open a UDP socket
		nRet = sock.Open(iEsockSuite->iSocketServer, KAfInet, KSockDatagram, KProtocolInetUdp);
		
		if (nRet != KErrNone)
			{
			
			Logger().WriteFormat(_L("Failed to open socket: return value = <%d>"), nRet);
			return EFail;  // do not try to open more sockets
			}
		
		// store socket handle
		iEsockSuite->AddSocketToListL(sock);
		}
	
	for (i = 0; i < nSockets; i++)
		{
		// Get local address of the socket (Empty)
		(iEsockSuite->GetSocketHandle(i + 1)).LocalName(addr);
		
		// Check length of buffer, should be set to (Zero)
		addr.OutputWithScope(buf);
		TESTL(buf.Length() == 0);
		}
	
	return EPass;
	}

// send Udp Data
const TDesC& CEsockSendUdpData::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"SendUdpData");
	
	return ret;
	}

CEsockSendUdpData::~CEsockSendUdpData()
	{
	}

enum TVerdict CEsockSendUdpData::easyTestStepL()
	{
	TInt testStepCounter = 0;
	TBool bindIsOK = EFalse;
	TBool testStepResult;
	
	TInetAddr addrRemote, addr;
	TRequestStatus stat;
	TInt connectTimer, Port;
	TInt result;
	
	do
	    {
	    testStepResult = ETrue;
	    
    	// get ip address to connect to (on echo port)
    	TESTL(GetIpAddressFromConfig(_L("Test_Common"), _L("ipAddress"), addrRemote));
    	
    	// get the port number
    	TESTL(GetIntFromConfig(_L("SendData"), _L("Port"), Port));
    	addrRemote.SetPort(Port);
    	
    	// get the port number
    	if(!GetIntFromConfig(_L("SendData"), _L("UdpPort"), Port))
    		Port = 9200;  // WAP UDP by default
    	
    	// Can't send back on Echo port as it would enter an infinite loop
    	addr.SetPort(Port);
    	
    	// format for display 
    	TBuf<39> AddrBuf;
    	addrRemote.OutputWithScope(AddrBuf);
    	
    	// get a value for the connection guard timer 
    	TESTL(GetIntFromConfig(_L("SendData"), _L("connectTimer"), connectTimer));
    	
    	// create timer to use as a guard timer
    	RTimer GuardTimer;
    	GuardTimer.CreateLocal();			// for this thread
    	TRequestStatus TimerStatus;			// status to monitor timer
    	
    	// get number of packets and size of packets 
    	TInt PacketSize = 0;
    	TESTL(GetIntFromConfig(_L("SendData"), _L("UdpPacketSize"), PacketSize));
    	TInt NoPackets = 0;
    	TESTL(GetIntFromConfig(_L("SendData"), _L("NoPackets"), NoPackets));
    	Logger().WriteFormat( _L("Send Udp Data to %S port %d in %d packets of %d bytes = %d"), 
    		&AddrBuf, Port, NoPackets, PacketSize, NoPackets * PacketSize);
    	
    	// and make some new buffers the correct size
    	HBufC8 * UdpWritebuf = HBufC8::NewMaxLC( PacketSize );
    	HBufC8 * UdpReadbuf  = HBufC8::NewMaxLC( PacketSize );
    	TPtr8 ptrUdpWritebuf = UdpWritebuf->Des();
    	TPtr8 ptrUdpReadbuf = UdpReadbuf->Des();
    	
    	result = (iEsockSuite->GetSocketHandle(1)).SetOpt(KSoUdpSynchronousSend, KSolInetUdp, 1 );
    	
    	// Bind
    	if (!bindIsOK)
    	    {
    	    result = (iEsockSuite->GetSocketHandle(1)).Bind(addr);
    	    TESTEL( KErrNone ==result,result);
    	    bindIsOK = ETrue;
    	    }
    	
    	TBuf8<50> Data;
    	
    	// write & read back the data
    	TInt recvCount = 0;
    	for (TInt i = 0; i < NoPackets; i++)
    		{
    		// initialise data
    		Data.Format(_L8("UDP-packet:%d helloworld"),i);
    		ptrUdpWritebuf.Repeat( Data );
    		
    		// write data 
    		GuardTimer.After(TimerStatus,connectTimer * 1000000); 
    		(iEsockSuite->GetSocketHandle(1)).SendTo(ptrUdpWritebuf, addrRemote, 0, stat);
    		User::WaitForRequest(stat, TimerStatus);
    		GuardTimer.Cancel();
    		User::WaitForAnyRequest();
    		
    		if (stat.Int() != KErrNone)
    		    {
    			Logger().WriteFormat(_L("Error on Udp Send after %d seconds, reason"), connectTimer, stat.Int());
    			setResultFlagInt(stat.Int(), testStepResult);
    		    }
    		
    		//TESTE( stat.Int() == KErrNone, stat.Int() );
    		
    		// read data (from echo server)
    		GuardTimer.After(TimerStatus,connectTimer * 1000000); 
    		(iEsockSuite->GetSocketHandle(1)).RecvFrom(ptrUdpReadbuf, addr, 0, stat);
    		User::WaitForRequest(stat, TimerStatus);
    		GuardTimer.Cancel();
    		User::WaitForAnyRequest();
    		
    		if (stat.Int() != KErrNone) 
    			{
    			Logger().WriteFormat(_L("Error on Udp Recv (packet %d) after %d seconds, reason %d"), i, connectTimer, stat.Int());
    			(iEsockSuite->GetSocketHandle(1)).CancelRecv();
    			setResultFlagInt(stat.Int(), testStepResult);
    			//TESTE( (stat.Int() == KErrNone), stat.Int() );  ** this is UDP, packet loss possible
    			}
    		else
    			{
    			// compare the data
    			TEST( ptrUdpWritebuf.Compare( ptrUdpReadbuf ) == 0 );
    			recvCount += ptrUdpReadbuf.Length();
    			}
    		
    		}
    	// Log what the % of the data was received!
    	Logger().WriteFormat(_L("Number of UDP packets sent %d, Number received %d"),NoPackets,recvCount);
    	
    	
    	// get rid of the old buffers 
    	CleanupStack::PopAndDestroy();	//	UdpWritebuf;
    	CleanupStack::PopAndDestroy();	//	UdpReadbuf;
    	
    	
	    if (!testStepResult)
    	    {
    	    Logger().WriteFormat(_L("TestStep failed %d time(s) from %d. Trying further..."), testStepCounter, KMaxTestStepCounter);
    	    
    	    ++testStepCounter;
    	    }
    	    
	    }
	    while ( (testStepCounter < KMaxTestStepCounter) &&
	          (!testStepResult) ); //big while...
     

    if ( testStepResult )
        {
	    return EPass;
        }
    else
        {
        return EFail;
        }
	}
	
	
// Test step 5.1
const TDesC& CEsockTest5_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test5.1");
	
	return ret;
	}

CEsockTest5_1::~CEsockTest5_1()
	{
	}

enum TVerdict CEsockTest5_1::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_5.1"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest5_1::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// set up data buffers
	TBuf8<100> wbuf, rbuf1;
	TBuf8<20> rbuf2;
	wbuf.SetLength(20);
	StripeDes(wbuf, 0, wbuf.Length(), '@', 'Z');
	
	// for each pair of connected sockets
	TRequestStatus stat;
	TInt i;
	for (i = 0; i < iNumSockets; i++)
		{
		// send data from client to server (20 bytes)
		iEsockSuite->GetSocketHandle(iSockIndex2 + (i*2)).Write(wbuf, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrNone, stat.Int());
		
		// read data (server)
		iEsockSuite->GetSocketHandle(iSockIndex3 + (i*2)).Read(rbuf2, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrNone, stat.Int());
		TESTL(rbuf2==wbuf);
		}
	
	// send data from server to client (100 bytes)
	wbuf.SetMax();
	StripeDes(wbuf, 0, wbuf.Length(), '@', 'Z');
	
	// for each pair of connected sockets
	for (i = 0; i < iNumSockets; i++)
		{
		// send data from server to client
		(iEsockSuite->GetSocketHandle(iSockIndex2 + (i*2))).Write(wbuf, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrNone, stat.Int());
		
		// read data (client)
		(iEsockSuite->GetSocketHandle(iSockIndex3 + (i*2))).Read(rbuf1, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrNone, stat.Int());
		
		// check received data
		TESTL(rbuf1==wbuf);
		}
	
	return EPass;
	}


// Test step 5.2
const TDesC& CEsockTest5_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test5.2");
	
	return ret;
	}

CEsockTest5_2::~CEsockTest5_2()
	{
	}

enum TVerdict CEsockTest5_2::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_5.2"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest5_2::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// set up data buffers
	TBuf8<1024> wbuf;
	
	StripeDes(wbuf, 0, wbuf.MaxLength(), '@', 'Z');
	wbuf.SetLength(20);
	
	// send data from client to server (20 bytes)
	TRequestStatus stat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Send(wbuf, 0, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// read data 
	TBuf8<20> rbuf2;
	iEsockSuite->GetSocketHandle(iSockIndex3).Recv(rbuf2, 0, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	TESTL(wbuf==rbuf2);
	
	// send data from server to client (20 packets of 1024 bytes)
	wbuf.SetLength(1024);
	TInt recvCount = 0;
	TBuf8<1024> rbuf1;
	for (TInt i = 0; i < 20; i++)
		{
		iEsockSuite->GetSocketHandle(iSockIndex3).Send(wbuf, 0, stat);
		User::WaitForRequest(stat);
		TESTL(stat==KErrNone);
		
		iEsockSuite->GetSocketHandle(iSockIndex2).Recv(rbuf1, 0, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrNone, stat.Int());
		
		recvCount+=rbuf1.Length();
		}
	
	// check number of bytes received
	TESTL(recvCount==(20*1024));
	
	// send data from client to server (20 packets of 512 bytes)
	wbuf.SetLength(512);
	recvCount = 0;
	TBuf8<512> rbuf3;
	for (TInt ii = 0; ii < 20; ii++)
		{
		iEsockSuite->GetSocketHandle(iSockIndex2).Send(wbuf, 0, stat);
		User::WaitForRequest(stat);
		TESTL(stat==KErrNone);
		
		iEsockSuite->GetSocketHandle(iSockIndex3).Recv(rbuf3, 0, stat);
		User::WaitForRequest(stat);
		TESTEL(stat==KErrNone, stat.Int());
		
		recvCount+=rbuf3.Length();
		}
	
	// check number of bytes received
	TESTL(recvCount==(20*512));
	
	return EPass;
	}


// Test step 5.3
const TDesC& CEsockTest5_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test5.3");
	
	return ret;
	}

CEsockTest5_3::~CEsockTest5_3()
	{
	}

enum TVerdict CEsockTest5_3::easyTestStepPreambleL()
	{
	CloseSockets();
	if (KErrNone != OpenMinSockets(_L("Test_5.3"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	// get ip address to connect to (on echo port)
	TInetAddr addrRemote;
	if (!GetIpAddressFromConfig(_L("Test_Common"), _L("ipAddress"), addrRemote))
		{
		return EFail;
		}
	addrRemote.SetPort(7);
	
	// get log level
	if (!GetIntFromConfig(_L("Test_5.3"), _L("logLevel"), iLogLevel))
		{
		iLogLevel = 0;
		}

	// connect
	if (iLogLevel > 0)
		{
		Logger().Write(_L("Connect to remote host..."));
		}
	TRequestStatus stat;
	(iEsockSuite->GetSocketHandle(1)).Connect(addrRemote, stat);
	User::WaitForRequest(stat);
	if (KErrNone != stat.Int())
		{
		Logger().WriteFormat(_L("Connect returned %d"), stat.Int());
		return EFail;
		}
	if (iLogLevel > 0)
		{
		Logger().Write(_L("completed"));
		}

	return EPass;
	}

enum TVerdict CEsockTest5_3::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// set up data buffers
	TBuf8<512> wbuf;
	StripeDes(wbuf, 0, wbuf.MaxLength(), '@', 'Z');
	
	// send data (20 bytes)
	wbuf.SetLength(20);
	TRequestStatus stat;
	if (iLogLevel > 0)
		{
		Logger().Write(_L("Sending..."));
		}
	RSocket& sock = iEsockSuite->GetSocketHandle(1);
	sock.Write(wbuf, stat);
	User::WaitForRequest(stat);
	TInt ret = stat.Int();
	TESTEL(KErrNone == ret, ret);
	
	// read data (from echo server)
	if (iLogLevel > 0)
		{
		Logger().Write(_L("Receiving..."));
		}
	TBuf8<20> rbuf2;
	sock.Read(rbuf2, stat);
	User::WaitForRequest(stat);
	ret = stat.Int();
	TESTEL(KErrNone == ret, ret);
	
	TESTL(rbuf2==wbuf);
	
	// send data (20 packets of 512 bytes)
	wbuf.SetMax();
	TInt sendCount = 0;

	if (iLogLevel > 0)
		{
		Logger().Write(_L("Send loop..."));
		}
	for (TInt i = 0; i < 20; i++)
		{
		sock.Write(wbuf, stat);
		User::WaitForRequest(stat);
		ret = stat.Int();
		TESTEL(KErrNone == ret, ret);
		sendCount += wbuf.Length();
		}

	TBuf8<128> rbuf1;
	TInt recvCount = 0;
	ret = KErrNone;
	while (recvCount < sendCount)
		{
		sock.Read(rbuf1, stat);
		User::WaitForRequest(stat);
		ret = stat.Int();
		
		if (KErrNone != ret)
			{
			break;
			}
		
		recvCount += rbuf1.Length();
		if (iLogLevel > 0)
			{
			Logger().WriteFormat(_L("Received %d octets"), rbuf1.Length());
			}
		}
	TESTEL(sendCount == recvCount, recvCount);
	
	if (iLogLevel > 0)
		{
		Logger().Write(_L("End Test 5.3"));
		}
	
	return EPass;
	}


// Test step 5.4
const TDesC& CEsockTest5_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test5.4");
	
	return ret;
	}

CEsockTest5_4::~CEsockTest5_4()
	{
	}

enum TVerdict CEsockTest5_4::easyTestStepPreambleL()
	{
	CloseSockets();
	if (KErrNone != OpenMinSockets(_L("Test_5.4"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	// get ip address to connect to (on echo port)
	TInetAddr addrRemote;
	if (!GetIpAddressFromConfig(_L("Test_Common"), _L("ipAddress"), addrRemote))
		{
		return EFail;
		}
	addrRemote.SetPort(7);
	
	// get log level
	if (!GetIntFromConfig(_L("Test_5.4"), _L("logLevel"), iLogLevel))
		{
		iLogLevel = 0;
		}

	// connect
	if (iLogLevel > 0)
		{
		Logger().Write(_L("Connect to remote host..."));
		}
	TRequestStatus stat;
	(iEsockSuite->GetSocketHandle(1)).Connect(addrRemote, stat);
	User::WaitForRequest(stat);
	if (KErrNone != stat.Int())
		{
		Logger().WriteFormat(_L("Connect returned %d"), stat.Int());
		return EFail;
		}
	if (iLogLevel > 0)
		{
		Logger().Write(_L("completed"));
		}
	return EPass;
	}

enum TVerdict CEsockTest5_4::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// set up data buffers
	TBuf8<512> wbuf;
	TBuf8<128> rbuf;
	StripeDes(wbuf, 0, wbuf.MaxLength(), '@', 'Z');
	
	// send data (30 bytes)
	wbuf.SetLength(30);
	TRequestStatus stat;
	if (iLogLevel > 0)
		{
		Logger().Write(_L("Sending..."));
		}
	RSocket& sock = iEsockSuite->GetSocketHandle(1);
	sock.Send(wbuf, 0, stat);
	User::WaitForRequest(stat);
	TInt ret = stat.Int();
	TESTEL(KErrNone == ret, ret);
	
	// read data (from echo server)
	if (iLogLevel > 0)
		{
		Logger().Write(_L("Receiving..."));
		}
	TSockXfrLength xfrLen;
	sock.RecvOneOrMore(rbuf, 0, stat, xfrLen);
	User::WaitForRequest(stat);
	ret = stat.Int();
	TESTEL(KErrNone == ret, ret);
	TInt len = xfrLen();
	TESTEL(30 == len, len);
	
	TESTL(rbuf==wbuf);
	
	// send data (20 packets of 512 bytes)
	wbuf.SetMax();
	TInt sendCount = 0;

	if (iLogLevel > 0)
		{
		Logger().Write(_L("Send loop..."));
		}
	for (TInt i = 0; i < 20; i++)
		{
		sock.Send(wbuf, 0, stat);
		User::WaitForRequest(stat);
		ret = stat.Int();
		TESTEL(KErrNone == ret, ret);
		sendCount += wbuf.Length();
		}

	TInt recvCount = 0;
	while (recvCount < sendCount)
		{
		sock.RecvOneOrMore(rbuf, 0, stat, xfrLen);
		User::WaitForRequest(stat);
		ret = stat.Int();

		if (KErrNone != ret)
			{
			break;
			}

		recvCount += xfrLen();
		if (iLogLevel > 0)
			{
			Logger().WriteFormat(_L("Received %d octets"), xfrLen());
			}
		}
	TESTEL(sendCount == recvCount, recvCount);
	
	if (iLogLevel > 0)
		{
		Logger().Write(_L("End Test 5.4"));
		}
	return EPass;
	}


// Test step 5.5
const TDesC& CEsockTest5_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test5.5");
	
	return ret;
	}

CEsockTest5_5::~CEsockTest5_5()
	{
	}

enum TVerdict CEsockTest5_5::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_5.5"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest5_5::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	const TInt KBufSize = 15000;
	// Constructs an empty 8 bit modifiable buffer descriptor. It contains no data.
	typedef TBuf8<KBufSize> TBuffer;
	
	TBuffer* wtemp=new (ELeave) TBuffer;
	CleanupStack::PushL(wtemp);
	TBuffer& wbuf=*wtemp;
	
	StripeDes(wbuf, 0, wbuf.MaxLength(), '@', 'Z');
	wbuf.SetMax();

	TRequestStatus wstat;
	(iEsockSuite->GetSocketHandle(iSockIndex2)).Write(wbuf, wstat);
	(iEsockSuite->GetSocketHandle(iSockIndex2)).CancelWrite();
	User::WaitForRequest(wstat);
	TESTEL(wstat==KErrNone || wstat==KErrCancel, wstat.Int());
	
	Logger().WriteFormat(_L("wstat =  %d"),wstat.Int());
	
	TBuf8<1024> wbuf2, rbuf;
	
	StripeDes(wbuf2, 0, wbuf2.MaxLength(), '@', 'Z');
	wbuf2.SetMax();
	
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(wbuf2, wstat);
	User::WaitForRequest(wstat);
	TESTL(wstat==KErrNone);

	TRequestStatus rstat;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, rstat);
	User::WaitForRequest(rstat);	
	TESTEL(rstat==KErrNone, rstat.Int());
	
	TESTL(rbuf==wbuf2);
	
	CleanupStack::PopAndDestroy();
	
	// shutdown the client socket - do not wait for completion
	return EPass;
	}


// Test step 5.6
const TDesC& CEsockTest5_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test5.6");
	
	return ret;
	}

CEsockTest5_6::~CEsockTest5_6()
	{
	}

enum TVerdict CEsockTest5_6::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_5.6"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest5_6::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	const TInt KBufSize = 15000;
	// Constructs an empty 8 bit modifiable buffer descriptor. It contains no data.
	typedef TBuf8<KBufSize> TBuffer;
	
	TBuffer* wtemp=new (ELeave) TBuffer;
	CleanupStack::PushL(wtemp);
	TBuffer& wbuf=*wtemp;
	
	StripeDes(wbuf, 0, wbuf.MaxLength(), '@', 'Z');
	wbuf.SetMax();
	
	TRequestStatus wstat;
	(iEsockSuite->GetSocketHandle(iSockIndex2)).Send(wbuf, 0, wstat);	
	(iEsockSuite->GetSocketHandle(iSockIndex2)).CancelSend();
	User::WaitForRequest(wstat);
	TESTEL(wstat==KErrNone || wstat==KErrCancel, wstat.Int());
	
	Logger().WriteFormat(_L("wstat =  %d"),wstat.Int());
	
	TBuf8<1024> wbuf2, rbuf;
	
	StripeDes(wbuf2, 0, wbuf2.MaxLength(), '@', 'Z');
	wbuf2.SetMax();
	
	(iEsockSuite->GetSocketHandle(iSockIndex2)).Send(wbuf2, 0, wstat);
	User::WaitForRequest(wstat);
	TESTL(wstat==KErrNone);

	TRequestStatus rstat;
	(iEsockSuite->GetSocketHandle(iSockIndex3)).Recv(rbuf, 0, rstat);
	User::WaitForRequest(rstat);
	TESTEL(rstat==KErrNone, rstat.Int());
	
	TESTL(rbuf==wbuf2);
	
	CleanupStack::PopAndDestroy();
	
	// shutdown the client socket - do not wait for completion
	return EPass;
	}


// Test step 5.7
const TDesC& CEsockTest5_7::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test5.7");
	
	return ret;
	}

CEsockTest5_7::~CEsockTest5_7()
	{
	}

enum TVerdict CEsockTest5_7::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_5.7"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest5_7::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	TBuf8<24> wbuf, rbuf;
	StripeDes(wbuf, 0, wbuf.MaxLength(), '1', '9');
	// send data (20 bytes)
	wbuf.SetLength(20);
	
	TRequestStatus wstat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(wbuf, wstat);
	User::WaitForRequest(wstat);
	TESTL(wstat==KErrNone);
	
	// read the data - deliberately trying to read more than was sent so as to block indefinitely
	TRequestStatus rstat;
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf, rstat);
	User::After(1000000);	// wait a second so that initial 20 bytes can be recv'd
	// cancel read before it completes
	iEsockSuite->GetSocketHandle(iSockIndex3).CancelRead();
	User::WaitForRequest(rstat);
	TESTL(rstat==KErrCancel);
	
	TBuf8<1024> wbuf2, rbuf2;
	StripeDes(wbuf2, 0, wbuf2.MaxLength(), '@', 'Z');
	wbuf2.SetMax();
	
	iEsockSuite->GetSocketHandle(iSockIndex2).Write(wbuf2, wstat);
	User::WaitForRequest(wstat);
	TESTL(wstat==KErrNone);
	iEsockSuite->GetSocketHandle(iSockIndex3).Read(rbuf2, rstat);
	User::WaitForRequest(rstat);
	TESTL(rstat==KErrNone);
	
	TESTL(wbuf2==rbuf2);
	
	// shutdown the client socket - do not wait for completion
	return EPass;
	}


// Test step 5.8
const TDesC& CEsockTest5_8::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test5.8");
	
	return ret;
	}

CEsockTest5_8::~CEsockTest5_8()
	{
	}

enum TVerdict CEsockTest5_8::easyTestStepPreambleL()
	{
	TInt connections;
	TRAPD(ret, connections = OpenConnectionsL(_L("Test_5.8"), iSockIndex2, iSockIndex3));
	if (KErrNone != ret)
		{
		return EFail;
		}
	if (0 >= connections)
		{
		return EFail;
		}

	return EPass;
	}

enum TVerdict CEsockTest5_8::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());

	TBuf8<24> wbuf, rbuf;
	StripeDes(wbuf, 0, wbuf.MaxLength(), '1', '9');
	// send data (20 bytes)
	wbuf.SetLength(20);
	
	TRequestStatus wstat;
	iEsockSuite->GetSocketHandle(iSockIndex2).Send(wbuf, 0, wstat);
	User::WaitForRequest(wstat);
	TESTL(wstat==KErrNone);
	// read the data - deliberately trying to read more than was sent so as to block indefinitely
	
	TRequestStatus rstat;
	iEsockSuite->GetSocketHandle(iSockIndex3).Recv(rbuf, 0, rstat);
	User::After(1000000);	// wait a second so that initial 20 bytes can be recv'd
	// cancel read before it completes
	iEsockSuite->GetSocketHandle(iSockIndex3).CancelRecv();
	User::WaitForRequest(rstat);
	TESTL(rstat==KErrCancel);
	
	TBuf8<1024> wbuf2, rbuf2;
	StripeDes(wbuf2, 0, wbuf2.MaxLength(), '@', 'Z');
	wbuf2.SetMax();
	
	iEsockSuite->GetSocketHandle(iSockIndex2).Send(wbuf2, 0, wstat);
	User::WaitForRequest(wstat);
	TESTL(wstat==KErrNone);
	iEsockSuite->GetSocketHandle(iSockIndex3).Recv(rbuf2, 0, rstat);
	User::WaitForRequest(rstat);
	TESTL(rstat==KErrNone);
	
	TESTL(wbuf2==rbuf2);
	
	// shutdown the client socket - do not wait for completion
	return EPass;
	}

// Test step 5.9
// Open a TCP socket along with a RawIP socket on TCP protocol. Write some data to the 
// TCP socke and perform a read on the RawIP socket, this should not fail.
const TDesC& CEsockTest5_9::GetTestName()
	{		
	// store the name of this test case
	_LIT(ret,"Test5.9");
	
	return ret;
	}

CEsockTest5_9::~CEsockTest5_9()
	{
	}

enum TVerdict CEsockTest5_9::easyTestStepPreambleL()
	{
	CloseSockets();
	if (KErrNone != OpenMinSockets(_L("Test_5.9"),KAfInet, KSockStream, KProtocolInetTcp))
		{
		return EFail;
		}

	TInetAddr addrRemote;
	// get ip address to connect to (on echo port)
	if (!GetIpAddressFromConfig(_L("Test_Common"), _L("ipAddress"), addrRemote))
		{
		return EFail;
		}
	addrRemote.SetPort(7);	
		
	// connect
	TRequestStatus stat;
	(iEsockSuite->GetSocketHandle(1)).Connect(addrRemote, stat);
	User::WaitForRequest(stat);
	TInt ret = stat.Int();
	if (KErrNone != ret)
		{
		Logger().WriteFormat(_L("Connect returned %d"), ret);
		return EFail;
		}

	return EPass;
	}
		
TVerdict CEsockTest5_9::easyTestStepL()
	{
	TESTL(EPass == TestStepResult());
	
	// prepare for test
	// set up data buffers
	TBuf8<512> wbuf;
	StripeDes(wbuf, 0, wbuf.MaxLength(), '@', 'Z');
	
	// setup data (30 bytes) for send
	wbuf.SetLength(30);
	
	// open RawIP socket
	RSocket rawsock;
	CleanupClosePushL(rawsock);
	_LIT(KRawIp, "rawip");
	TInt ret = rawsock.Open(iEsockSuite->iSocketServer, KRawIp);
	TESTEL(KErrNone == ret, ret);
		
	// bind RawIP socket to TCP protocol
	ret = rawsock.SetLocalPort(KProtocolInetTcp);
	TESTEL(KErrNone == ret, ret);
	
	// prepare to receive data
	TBuf8<30> recvBuf;
	recvBuf.Zero();
	TRequestStatus receiveStat;
	rawsock.Read(recvBuf, receiveStat);
	
	// now there is an outstanding receive send to the echo server
	TRequestStatus stat;
	(iEsockSuite->GetSocketHandle(1)).Write(wbuf, stat);
	User::WaitForRequest(stat);
	ret = stat.Int();
	TESTEL(KErrNone == ret, ret);
	
	// check that we have received the echo'ed data
	User::WaitForRequest(receiveStat);
	ret = receiveStat.Int();
	TESTEL(KErrNone == ret, ret);
	TESTL(recvBuf.Length() != 0);
	
	CleanupStack::PopAndDestroy(&rawsock);
	return EPass;
	}

