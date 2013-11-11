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
// This contains ESock Test cases from section 21
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection21.h"


// Test step 21.1
const TDesC& CEsockTest21_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test21.1");
	
	return ret;
	}

CEsockTest21_1::~CEsockTest21_1()
	{
	}

enum TVerdict CEsockTest21_1::easyTestStepL()
	{
	TInetAddr addr;
	
	// input valid ip addresses
	TESTL(addr.Input(_L("::"))==KErrNone);
	TESTL(addr.Input(_L("::11"))==KErrNone);
	TESTL(addr.Input(_L("::FFF"))==KErrNone);
	TESTL(addr.Input(_L("::FFF0"))==KErrNone);
	TESTL(addr.Input(_L("::1:FFF"))==KErrNone);
	TESTL(addr.Input(_L("::FFFF:100:FFF"))==KErrNone);
	TESTL(addr.Input(_L("::1:FFFF:100:FFF"))==KErrNone);
	TESTL(addr.Input(_L("::FFFF:1:FFFF:100:FFF"))==KErrNone);
	TESTL(addr.Input(_L("::12:FFFF:1:FFFF:100:FFF"))==KErrNone);
	TESTL(addr.Input(_L("::FFFF:12:FFFF:1:FFFF:100:FFF"))==KErrNone);
	TESTL(addr.Input(_L("1:FFFF:12:FFFF:1:FFFF:100:FFF"))==KErrNone);
	
	TESTL(addr.Input(_L("1::"))==KErrNone);
	TESTL(addr.Input(_L("FFFF:1::"))==KErrNone);
	TESTL(addr.Input(_L("1:FFFF:1::"))==KErrNone);
	TESTL(addr.Input(_L("FFFF:FFFF:FFFF:1::"))==KErrNone);
	TESTL(addr.Input(_L("1:FFFF:FFFF:FFFF:1::"))==KErrNone);
	TESTL(addr.Input(_L("FFFF:1:FFFF:FFFF:FFFF:1::"))==KErrNone);
	TESTL(addr.Input(_L("1:FFFF:1:FFFF:FFFF:FFFF:1::"))==KErrNone);
	TESTL(addr.Input(_L("0:0:0:0:0:0:0:0"))==KErrNone);
	TESTL(addr.Input(_L("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF"))==KErrNone);
	TESTL(addr.Input(_L("FFFF::FFFF"))==KErrNone);
	TESTL(addr.Input(_L("FFFF::FFFF:FFFF"))==KErrNone);
	TESTL(addr.Input(_L("FFFF:0:0:0:0:0:0:FFFF"))==KErrNone);
	TESTL(addr.Input(_L("abcd::"))==KErrNone);
	
	return EPass;
	}


// Test step 21.2
const TDesC& CEsockTest21_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test21.2");
	
	return ret;
	}

CEsockTest21_2::~CEsockTest21_2()
	{
	}

enum TVerdict CEsockTest21_2::easyTestStepL()
	{
	TInetAddr addr;
	
	// input invalid ip addresses
	
	TESTL(addr.Input(_L("::FFFF1"))!=KErrNone);
	TESTL(addr.Input(_L("::FFFG"))!=KErrNone);
	TESTL(addr.Input(_L(":::FFF1"))!=KErrNone);
	TESTL(addr.Input(_L("1:::FFF1"))!=KErrNone);
	TESTL(addr.Input(_L("::FFF1:1:21:23A1::"))!=KErrNone);
	TESTL(addr.Input(_L("0xFFF1:1:21:23A1::"))!=KErrNone);
	TESTL(addr.Input(_L("::130:179.21.200"))!=KErrNone);
	TESTL(addr.Input(_L("FFFF:FFFF:FFFF"))!=KErrNone);
	
	return EPass;
	}


// Test step 21.3
const TDesC& CEsockTest21_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test21.3");
	
	return ret;
	}

CEsockTest21_3::~CEsockTest21_3()
	{
	}

enum TVerdict CEsockTest21_3::easyTestStepL()
	{
	TInetAddr addr;
	TBuf<39> buf;
	
	// output the IP address
	
	const TIp6Addr KInet6addr1 = {{{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }}};
	const TIp6Addr KInet6addr2 = {{{ 0xff,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }}};
	const TIp6Addr KInet6addr3 = {{{ 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff }}};
	const TIp6Addr KInet6addr4 = {{{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }}};
	
	// set the IP address
	addr.SetAddress(KInet6addr1);
	
	// output as a string and check format
	addr.OutputWithScope(buf);
	TESTL(buf==_L("::1"));
	
	// change the IP address
	addr.SetAddress(KInet6addr2);
	
	// output as a string and check format
	addr.OutputWithScope(buf);
	TESTL(buf==_L("ffff::1"));
	
	// change the IP address
	addr.SetAddress(KInet6addr3);
	
	// output as a string and check format
	addr.OutputWithScope(buf);
	TESTL(buf==_L("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"));
	
	// change the IP address
	addr.SetAddress(KInet6addr4);
	
	// output as a string and check format
	addr.OutputWithScope(buf);
	TESTL(buf==_L("::"));
	
	return EPass;
	}


