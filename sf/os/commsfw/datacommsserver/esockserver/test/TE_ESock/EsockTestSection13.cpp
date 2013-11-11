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
// This contains ESock Test cases from section 13
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection13.h"


// Test step 13.1
const TDesC& CEsockTest13_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test13.1");
	return ret;
	}

CEsockTest13_1::~CEsockTest13_1()
	{
	}

enum TVerdict CEsockTest13_1::easyTestStepL()
	{
	TInetAddr addr, addr1, addr2;
	
	// setting the IP address
	
	// get IP addresses from script
	TESTL(GetIpAddressFromConfig(_L("Test_13.1"), _L("ipAddress1"), addr1));
	TESTL(GetIpAddressFromConfig(_L("Test_13.1"), _L("ipAddress2"), addr2));
	
	// convert IP address to TUint32
	TUint32 longAddr=addr1.Address();
	
	// set the IP address
	addr.SetAddress(longAddr);
	
	// check port number is 0
	TESTL(addr.Port() == 0);
	
	// check IP address has been assigned correctly
	TESTL(addr.Match(addr1));
	
	// change the IP address and repeat tests
	longAddr=addr2.Address();
	addr.SetAddress(longAddr);
	TESTL(addr.Port() == 0);
	TESTL(addr.Match(addr2));
	
	return EPass;
	}


// Test step 13.2
const TDesC& CEsockTest13_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test13.2");
	return ret;
	}

CEsockTest13_2::~CEsockTest13_2()
	{
	}

enum TVerdict CEsockTest13_2::easyTestStepL()
	{
	TInetAddr addr1, addr2;
	TInetAddr addr;
	
	// getting the IP address
	
	// get IP addresses from script
	TESTL(GetIpAddressFromConfig(_L("Test_13.2"), _L("ipAddress1"), addr1));
	TESTL(GetIpAddressFromConfig(_L("Test_13.2"), _L("ipAddress2"), addr2));
	
	// convert IP address to TUint32
	TUint32 longAddr=addr1.Address();
	
	// set the IP address
	addr.SetAddress(longAddr);
	
	// get the IP address and check against the input
	TUint32 tmpAddr=addr.Address();
	TESTL(longAddr==tmpAddr);
	
	// change the IP address and repeat tests
	longAddr=addr2.Address();
	addr.SetAddress(longAddr);
	tmpAddr=addr.Address();
	TESTL(longAddr==tmpAddr);
	
	return EPass;
	}

