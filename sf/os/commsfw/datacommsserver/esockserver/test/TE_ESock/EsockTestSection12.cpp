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
// This contains ESock Test cases from section 12
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection12.h"


// Test step 12.1
const TDesC& CEsockTest12_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test12.1");
	return ret;
	}

CEsockTest12_1::~CEsockTest12_1()
	{
	}

enum TVerdict CEsockTest12_1::easyTestStepL()
	{
	// simple construction
	TInetAddr addr;
	
	// check port has been assigned to zero (0)
	TESTL(addr.Port() == 0);
	
	// check address is unspecified
	// ouput address into a buffer - check length is 0
	TBuf<39> buf;
	addr.OutputWithScope(buf);
	TESTL(buf.Length()==0);
	
	return EPass;
	}


// Test step 12.2
const TDesC& CEsockTest12_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test12.2");
	return ret;
	}

CEsockTest12_2::~CEsockTest12_2()
	{
	}

enum TVerdict CEsockTest12_2::easyTestStepL()
	{
	TInt port1, port2, port3, port4;
	
	// construction specifying a port number
	
	// get port numbers from script
	TESTL(GetIntFromConfig(_L("Test_12.2"), _L("port1"), port1));
	TESTL(GetIntFromConfig(_L("Test_12.2"), _L("port2"), port2));
	TESTL(GetIntFromConfig(_L("Test_12.2"), _L("port3"), port3));
	TESTL(GetIntFromConfig(_L("Test_12.2"), _L("port4"), port4));
	
	// construct TCP/IP address with port number
	TInetAddr addr1(port1);
	
	// check port number assigned correctly
	TESTL(addr1.Port() == (TUint)port1);
	
	// check address is unspecified
	// ouput address into a buffer - check length is 0
	TBuf<39> buf;
	addr1.OutputWithScope(buf);
	TESTL(buf.Length()==0);
	
	// repeat for port2
	TInetAddr addr2(port2);
	TESTL(addr2.Port() == (TUint)port2);
	addr2.OutputWithScope(buf);
	TESTL(buf.Length()==0);
	
	// repeat for port3
	TInetAddr addr3(port3);
	TESTL(addr3.Port() == (TUint)port3);
	addr3.OutputWithScope(buf);
	TESTL(buf.Length()==0);
	
	// repeat for port4
	TInetAddr addr4(port4);
	TESTL(addr4.Port() == (TUint)port4);
	addr4.OutputWithScope(buf);
	TESTL(buf.Length()==0);
	
	return EPass;
	}


// Test step 12.3
const TDesC& CEsockTest12_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test12.3");
	return ret;
	}

CEsockTest12_3::~CEsockTest12_3()
	{
	}

enum TVerdict CEsockTest12_3::easyTestStepL()
	{
	TInetAddr addr1, addr2, addr3, addr4;
	TInt port1, port2, port3, port4;
	
	// construction with IP address and port number
	
	// get IP addresses and port numbers from script
	TESTL(GetIpAddressFromConfig(_L("Test_12.3"), _L("ipAddress1"), addr1));
	TESTL(GetIpAddressFromConfig(_L("Test_12.3"), _L("ipAddress2"), addr2));
	TESTL(GetIpAddressFromConfig(_L("Test_12.3"), _L("ipAddress3"), addr3));
	TESTL(GetIpAddressFromConfig(_L("Test_12.3"), _L("ipAddress4"), addr4));
	TESTL(GetIntFromConfig(_L("Test_12.3"), _L("port1"), port1));
	TESTL(GetIntFromConfig(_L("Test_12.3"), _L("port2"), port2));
	TESTL(GetIntFromConfig(_L("Test_12.3"), _L("port3"), port3));
	TESTL(GetIntFromConfig(_L("Test_12.3"), _L("port4"), port4));
	
	// convert IP address to a TUint32
	TUint32 longAddr=addr1.Address();
	
	// construct TCP/IP address from TUint32 and port
	TInetAddr addrA(longAddr, port1);
	
	// check port number assigned correctly
	TESTL(addrA.Port() == (TUint)port1);
	
	// check ip address has been assigned correctly
	// check against ip address from script
	TESTL(addrA.Match(addr1));
	
	// repeat for 2nd ip address
	longAddr=addr2.Address();
	TInetAddr addrB(longAddr, port2);
	TESTL(addrB.Port() == (TUint)port2);
	TESTL(addrB.Match(addr2));
	
	// repeat for 3rd ip address
	longAddr=addr3.Address();
	TInetAddr addrC(longAddr, port3);
	TESTL(addrC.Port() == (TUint)port3);
	TESTL(addrC.Match(addr3));
	
	// repeat for 4th ip address
	longAddr=addr4.Address();
	TInetAddr addrD(longAddr, port4);
	TESTL(addrD.Port() == (TUint)port4);
	TESTL(addrD.Match(addr4));
	
	return EPass;
	}


// Test step 12.4
const TDesC& CEsockTest12_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test12.4");
	return ret;
	}

CEsockTest12_4::~CEsockTest12_4()
	{
	}

enum TVerdict CEsockTest12_4::easyTestStepL()
	{
	TInetAddr addr1, addr2, addr3, addr4;
	TInt port1, port2, port3, port4;
	
	// get IP addresses and port numbers from script
	TESTL(GetIpAddressFromConfig(_L("Test_12.4"), _L("ipAddress1"), addr1));
	TESTL(GetIpAddressFromConfig(_L("Test_12.4"), _L("ipAddress2"), addr2));
	TESTL(GetIpAddressFromConfig(_L("Test_12.4"), _L("ipAddress3"), addr3));
	TESTL(GetIpAddressFromConfig(_L("Test_12.4"), _L("ipAddress4"), addr4));
	TESTL(GetIntFromConfig(_L("Test_12.4"), _L("port1"), port1));
	TESTL(GetIntFromConfig(_L("Test_12.4"), _L("port2"), port2));
	TESTL(GetIntFromConfig(_L("Test_12.4"), _L("port3"), port3));
	TESTL(GetIntFromConfig(_L("Test_12.4"), _L("port4"), port4));
	
	// set port number
	addr1.SetPort(port1);
	
	// construct TCP/IP address from another TInetAddr
	TInetAddr addrA((TSockAddr)addr1);
	
	// check port number
	TESTL(addrA.Port() == (TUint)port1);
	
	// check ip address has been assigned correctly
	// check against ip address from script
	TESTL(addrA.Match(addr1));
	
	// repeat for 2nd ip address
	addr2.SetPort(port2);
	TInetAddr addrB((TSockAddr)addr2);
	TESTL(addrB.Port() == (TUint)port2);
	TESTL(addrB.Match(addr2));
	
	// repeat for 3rd ip address
	addr3.SetPort(port3);
	TInetAddr addrC((TSockAddr)addr3);
	TESTL(addrC.Port() == (TUint)port3);
	TESTL(addrC.Match(addr3));
	
	// repeat for 4th ip address
	addr4.SetPort(port4);
	TInetAddr addrD((TSockAddr)addr4);
	TESTL(addrD.Port() == (TUint)port4);
	TESTL(addrD.Match(addr4));
	
	return EPass;
	}

