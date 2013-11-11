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
// This contains ESock Test cases from section 20
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection20.h"


// Test step 20.1
const TDesC& CEsockTest20_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test20.1");
	return ret;
	}

CEsockTest20_1::~CEsockTest20_1()
	{
	}

enum TVerdict CEsockTest20_1::easyTestStepL()
	{
	TInetAddr addr, addr2;
	const TIp6Addr KInet6addr1 = {{{ 0xff,0xfe,0,0,0,0,0,0,0,0,0,0,0,0,0x23,0 }}};
	const TIp6Addr KInet6addr2 = {{{ 0xff,0xfe,0,0,0,0,0,0,0,0,0,0,0,0,0,0x23 }}};
	
	// set up 2 TCP/IP address
	addr.SetAddress(KInet6addr1);
	addr.SetPort(7);
	
	addr2.SetAddress(KInet6addr2);
	addr2.SetPort(8);
	
	// compare IP address and port number
	TESTL(addr.CmpAddr(addr2)==EFalse);
	
	// change port number - now equal
	addr2.SetPort(7);
	TESTL(addr.CmpAddr(addr2)==EFalse);
	
	// change ip address - now equal
	addr2.SetAddress(KInet6addr1);
	TESTL(addr.CmpAddr(addr2)!=EFalse);
	
	return EPass;
	}


// Test step 20.2
const TDesC& CEsockTest20_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test20.2");
	return ret;
	}

CEsockTest20_2::~CEsockTest20_2()
	{
	}

enum TVerdict CEsockTest20_2::easyTestStepL()
	{
	TInetAddr addr, addr2;
	const TIp6Addr KInet6addr1 = {{{ 0xff,0xee,0,0,0,0,0,0,0,0,0,0,0,0,0x25,0x4f }}};
	const TIp6Addr KInet6addr2 = {{{ 0xff,0xee,0,0,0,0,0,0,0,0,0,0,0,0,0x25,0x4e }}};
	
	// set up 2 TCP/IP address
	addr.SetAddress(KInet6addr1);
	addr.SetPort(5000);
	
	addr2.SetAddress(KInet6addr2);
	addr2.SetPort(5001);
	
	// compare the two addresses
	TESTL(addr.Match(addr2)==EFalse);
	
	// change the 2nd address so that the ip addresses are the same
	addr2.SetAddress(KInet6addr1);
	
	// comapre again
	TESTL(addr.Match(addr2)!=EFalse);;
	
	return EPass;
	}


// Test step 20.3
const TDesC& CEsockTest20_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test20.3");
	return ret;
	}

CEsockTest20_3::~CEsockTest20_3()
	{
	}

enum TVerdict CEsockTest20_3::easyTestStepL()
	{
	TInetAddr addr, mask;
	TInetAddr addr2, addr3, addr4, addr5, addr6, addr7;
	
	// test whether addresses are on the same subnet (Match)
	
	const TIp6Addr KInet6addr  = {{{ 0xff,0xee,0x29,0x40,0,0x1,0,0x1,0,0,0,0,0,0,0x21,0xaa }}};
	const TIp6Addr KInetmask   = {{{ 0xff,0xff,0xff,0xff,0xff,0xff,0xf8,0,0,0,0,0,0,0,0,0 }}};
	const TIp6Addr KInet6addr2 = {{{ 0xff,0xee,0x29,0x40,0,1,0,1,0,0,0,0,0,1,0x22,0x10 }}};
	const TIp6Addr KInet6addr3 = {{{ 0xff,0xee,0x29,0x40,0,1,0,1,0,0,0,0,0,0,0,0x21 }}};
	const TIp6Addr KInet6addr4 = {{{ 0xff,0xee,0x29,0x40,0,1,0,1,0,0,0,0,0,0x21,0x3a,0 }}};
	const TIp6Addr KInet6addr5 = {{{ 0xff,0xee,0x29,0x40,8,1,0,1,0,0,0,0,0,0,0,0x21 }}};
	const TIp6Addr KInet6addr6 = {{{ 0xff,0xee,0x29,0x41,0,1,0,1,0,0,0,0,0,0x21,0,0x3a }}};
	const TIp6Addr KInet6addr7 = {{{ 0xff,0xe0,0x29,0x40,0,1,0,1,0,0,0,0,0,0x21,0x3a,0 }}};
	
	// set up ip addresses and subnet mask
	addr.SetAddress(KInet6addr);
	mask.SetAddress(KInetmask);
	addr2.SetAddress(KInet6addr2);
	addr3.SetAddress(KInet6addr3);
	addr4.SetAddress(KInet6addr4);
	addr5.SetAddress(KInet6addr5);
	addr6.SetAddress(KInet6addr6);
	addr7.SetAddress(KInet6addr7);
	
	// check whether addresses are on the same subnet
	TESTL(addr.Match(addr2, mask)!=EFalse);
	TESTL(addr.Match(addr3, mask)!=EFalse);
	TESTL(addr.Match(addr4, mask)!=EFalse);
	TESTL(addr.Match(addr5, mask)==EFalse);
	TESTL(addr.Match(addr6, mask)==EFalse);
	TESTL(addr.Match(addr7, mask)==EFalse);
	
	return EPass;
	}


// Test step 20.4
const TDesC& CEsockTest20_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test20.4");
	return ret;
	}

CEsockTest20_4::~CEsockTest20_4()
	{
	}

enum TVerdict CEsockTest20_4::easyTestStepL()
	{
	TInetAddr addr;
	TInetAddr addr2, addr3, addr4, addr5;
	
	// match using prefix length
	
	const TIp6Addr KInet6addr  = {{{ 0xff,0xfe,0x12,0,0xff,0xf0,0,0,0,0,0,0,0,0,0,0 }}};
	const TIp6Addr KInet6addr2 = {{{ 0xff,0xfe,0x12,0,0xff,0xf0,0,0,0,0,0,0,0,0,0,0 }}};
	const TIp6Addr KInet6addr3 = {{{ 0xff,0xfe,0x12,0,0xff,0xf7,0,0,0,0,0,0,0,0,0,0 }}};
	const TIp6Addr KInet6addr4 = {{{ 0xff,0xfe,0x12,0,0xff,0xfb,0,0,0,0,0,0,0,0,0,0 }}};
	const TIp6Addr KInet6addr5 = {{{ 0xff,0xff,0x12,0,0xff,0xf0,0,0,0,0,0,0,0,0,0,0 }}};
	
	// set up ip addresses and subnet mask
	addr.SetAddress(KInet6addr);
	addr2.SetAddress(KInet6addr2);
	addr3.SetAddress(KInet6addr3);
	addr4.SetAddress(KInet6addr4);
	addr5.SetAddress(KInet6addr5);
	
	// call Match using a prefix length of 43
	TESTL(addr.Match(addr2, 43)!=EFalse);
	TESTL(addr.Match(addr3, 43)!=EFalse);
	TESTL(addr.Match(addr4, 43)!=EFalse);
	TESTL(addr.Match(addr5, 43)==EFalse);
	
	// call Match using a prefix length of 44
	TESTL(addr.Match(addr2, 44)!=EFalse);
	TESTL(addr.Match(addr3, 44)!=EFalse);
	TESTL(addr.Match(addr4, 44)!=EFalse);
	TESTL(addr.Match(addr5, 44)==EFalse);
	
	// call Match using a prefix length of 45
	TESTL(addr.Match(addr2, 45)!=EFalse);
	TESTL(addr.Match(addr3, 45)!=EFalse);
	TESTL(addr.Match(addr4, 45)==EFalse);
	TESTL(addr.Match(addr5, 45)==EFalse);
	
	// call Match using a prefix length of 46
	TESTL(addr.Match(addr2, 46)!=EFalse);
	TESTL(addr.Match(addr3, 46)==EFalse);
	TESTL(addr.Match(addr4, 46)==EFalse);
	TESTL(addr.Match(addr5, 46)==EFalse);
	
	return EPass;
	}

