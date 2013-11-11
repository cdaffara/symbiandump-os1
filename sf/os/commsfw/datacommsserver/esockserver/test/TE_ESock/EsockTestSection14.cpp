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
// This contains ESock Test cases from section 14
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection14.h"


// Test step 14.1
const TDesC& CEsockTest14_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test14.1");
	return ret;
	}

CEsockTest14_1::~CEsockTest14_1()
	{
	}

enum TVerdict CEsockTest14_1::easyTestStepL()
	{
	
	// compare IP address and port (CmpAddr)
	
	// set up 2 TCP/IP addresses
	TInetAddr addr1(INET_ADDR(10, 0, 0, 154), 7);
	TInetAddr addr2(INET_ADDR(10, 0, 0, 155), 8);
	
	// compare the 2 TCP/IP addresses
	TESTL(addr1.CmpAddr(addr2)==EFalse);
	
	// change addr2
	addr2.SetAddress(INET_ADDR(10, 0, 0, 154));
	
	// compare again
	TESTL(addr1.CmpAddr(addr2)==EFalse);
	
	// for addr2 change the port number
	addr2.SetPort(7);
	
	// compare once again - now addresses should match
	TESTL(addr1.CmpAddr(addr2)!=EFalse);
	
	// and the other way round
	TESTL(addr2.CmpAddr(addr1)!=EFalse);
	
	return EPass;
	}


// Test step 14.2
const TDesC& CEsockTest14_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test14.2");
	return ret;
	}

CEsockTest14_2::~CEsockTest14_2()
	{
	}

enum TVerdict CEsockTest14_2::easyTestStepL()
	{
	TInetAddr addr1, addr2;
	
	// compare IP addresses only (Match)
	
	// set the IP addresses
	addr1.SetAddress(INET_ADDR(10, 0, 0, 254));
	addr2.SetAddress(INET_ADDR(10, 0, 0, 253));
	
	// set the port number
	addr2.SetPort(5001);
	
	// compare IP addresses
	TESTL(addr1.Match(addr2)==EFalse);
	
	// change the IP address for addr2
	addr2.SetAddress(INET_ADDR(10, 0, 0, 254));
	
	// compare again - should now match
	TESTL(addr1.Match(addr2)!=EFalse);
	
	// and the other way round
	TESTL(addr2.Match(addr2)!=EFalse);
	
	return EPass;
	}


// Test step 14.3
const TDesC& CEsockTest14_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test14.3");
	return ret;
	}

CEsockTest14_3::~CEsockTest14_3()
	{
	}

enum TVerdict CEsockTest14_3::easyTestStepL()
	{
	TInetAddr addr;
	TInetAddr mask;
	TInetAddr addr1, addr2, addr3, addr4, addr5, addr6;
	
	// Test whether addresses are on the same subnet (Match)
	
	// set the IP addresses and subnet mask
	addr.SetAddress(INET_ADDR(140, 179, 220, 200));
	mask.SetAddress(INET_ADDR(255, 255, 224, 0));
	
	addr1.SetAddress(INET_ADDR(140, 179, 221, 1));
	addr2.SetAddress(INET_ADDR(140, 179, 193, 120));
	addr3.SetAddress(INET_ADDR(140, 179, 200, 174));
	addr4.SetAddress(INET_ADDR(141, 179, 200, 200));
	addr5.SetAddress(INET_ADDR(140, 180, 200, 174));
	addr6.SetAddress(INET_ADDR(140, 179, 129, 201));
	
	// match with subnet mask
	TESTL(addr.Match(addr1, mask)!=EFalse);
	TESTL(addr.Match(addr2, mask)!=EFalse);
	TESTL(addr.Match(addr3, mask)!=EFalse);
	TESTL(addr.Match(addr4, mask)==EFalse);
	TESTL(addr.Match(addr5, mask)==EFalse);
	TESTL(addr.Match(addr6, mask)==EFalse);
	
	return EPass;
	}


// Test step 14.4
const TDesC& CEsockTest14_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test14.4");
	return ret;
	}

CEsockTest14_4::~CEsockTest14_4()
	{
	}

enum TVerdict CEsockTest14_4::easyTestStepL()
	{
	TInetAddr addr;
	TInetAddr addr1, addr2, addr3, addr4;
	
	addr.SetAddress(INET_ADDR(140, 179, 255, 255));
	
	addr1.SetAddress(INET_ADDR(140, 179, 225, 200));
	addr2.SetAddress(INET_ADDR(140, 179, 242, 120));
	addr3.SetAddress(INET_ADDR(140, 179, 251, 19));
	addr4.SetAddress(INET_ADDR(140, 178, 224, 1));
	
	// match using a prefix length of 19
	TESTL(addr.Match(addr1, 19)!=EFalse);
	TESTL(addr.Match(addr2, 19)!=EFalse);
	TESTL(addr.Match(addr3, 19)!=EFalse);
	TESTL(addr.Match(addr4, 19)==EFalse);
	
	// match using a prefix length of 20
	TESTL(addr.Match(addr1, 20)==EFalse);
	TESTL(addr.Match(addr2, 20)!=EFalse);
	TESTL(addr.Match(addr3, 20)!=EFalse);
	TESTL(addr.Match(addr4, 20)==EFalse);
	
	// match using a prefix length of 21
	TESTL(addr.Match(addr1, 21)==EFalse);
	TESTL(addr.Match(addr2, 21)==EFalse);
	TESTL(addr.Match(addr3, 21)!=EFalse);
	TESTL(addr.Match(addr4, 21)==EFalse);
	
	// match using a prefix length of 22
	TESTL(addr.Match(addr1, 22)==EFalse);
	TESTL(addr.Match(addr2, 22)==EFalse);
	TESTL(addr.Match(addr3, 22)==EFalse);
	TESTL(addr.Match(addr4, 22)==EFalse);
	
	return EPass;
	}

