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
// This contains ESock Test cases from section 23
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection23.h"



// Test step 23.1
const TDesC& CEsockTest23_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.1");
	return ret;
	}

CEsockTest23_1::~CEsockTest23_1()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest23_1::easyTestStepL()
	{
	
	// IsClassA
	
	// get list of IP addresses
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		// check if the address is type class A. They all should fail as they make no sense for ip6. 
		TESTL(appendixInArray[i].IsClassA()==EFalse);
		}
	
	appendixInArray.Reset();
	return EPass;
	}



// Test step 23.2
const TDesC& CEsockTest23_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.2");
	return ret;
	}

CEsockTest23_2::~CEsockTest23_2()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest23_2::easyTestStepL()
	{
	
	// IsClassB
	
	// get list of IP addresses
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		// check if the address is type class B
		TESTL(appendixInArray[i].IsClassB()==EFalse);
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 23.3
const TDesC& CEsockTest23_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.3");
	return ret;
	}

CEsockTest23_3::~CEsockTest23_3()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest23_3::easyTestStepL()
	{
	
	// IsClassC
	
	// get list of IP addresses
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		// check if the address is type class C
		TESTL(appendixInArray[i].IsClassC()==EFalse);
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 23.4
const TDesC& CEsockTest23_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.4");
	return ret;
	}

CEsockTest23_4::~CEsockTest23_4()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest23_4::easyTestStepL()
	{
	
	// IsMulticast
	
	// get list of IP addresses
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		if (i == ipAddress4 || i == ipAddress5 )
			{
			// check if the address is type multicast
			TESTL(appendixInArray[i].IsMulticast()!=EFalse);
			}
		else //address is not multicast
			{
			TESTL(appendixInArray[i].IsMulticast()==EFalse);
			}
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 23.5
const TDesC& CEsockTest23_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.5");
	return ret;
	}

CEsockTest23_5::~CEsockTest23_5()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest23_5::easyTestStepL()
	{
	//	TBool bRet;
	
	// IsWildAddr
	
	// get list of IP addresses
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		// check if the address is the wild address
		
		// only first address is the wild address
		if (i == ipAddress1)
			{
			TESTL(appendixInArray[i].IsWildAddr()!=EFalse);
			}
		else
			{
			TESTL(appendixInArray[i].IsWildAddr()==EFalse);
			}
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 23.6
const TDesC& CEsockTest23_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.6");
	return ret;
	}

CEsockTest23_6::~CEsockTest23_6()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest23_6::easyTestStepL()
	{
	
	// IsBroadcast
	
	// get list of IP addresses
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		// check if the address is of type broadcast
		TESTL(appendixInArray[i].IsBroadcast()==EFalse);
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 23.7
const TDesC& CEsockTest23_7::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.7");
	return ret;
	}

CEsockTest23_7::~CEsockTest23_7()
	{
	}

enum TVerdict CEsockTest23_7::easyTestStepL()
	{
	TInetAddr addr;
	
	// IsWildPort
	
	// set the IP address
	const TIp6Addr KInet6addr = {{{ 0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10 }}};
	addr.SetAddress(KInet6addr);
	
	// set the port number
	addr.SetPort(0);
	
	// check if the port is the wild port
	TESTL(addr.IsWildPort()!=EFalse);
	
	// change the port number
	addr.SetPort(21);
	
	// check if the port is the wild port
	TESTL(addr.IsWildPort()==EFalse);
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 23.8
const TDesC& CEsockTest23_8::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.8");
	return ret;
	}

CEsockTest23_8::~CEsockTest23_8()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest23_8::easyTestStepL()
	{
	
	// IsLoopback
	
	// get list of IP addresses
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		// check if the address is of type loopback
		if ((i == ipAddress2 ))
			{
			TESTL(appendixInArray[i].IsLoopback()!=EFalse);
			}
		else
			{
			TESTL(appendixInArray[i].IsLoopback()==EFalse);
			}
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 23.9
const TDesC& CEsockTest23_9::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.9");
	return ret;
	}

CEsockTest23_9::~CEsockTest23_9()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest23_9::easyTestStepL()
	{
	
	// IsUnspecified
	
	// get list of IP addresses
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		if (i == ipAddress1 )
			{
			// check if the address is of type inspecified
			TESTL(appendixInArray[i].IsUnspecified()!=EFalse);
			}
		else
			{
			TESTL(appendixInArray[i].IsUnspecified()==EFalse);
			}
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 23.10
const TDesC& CEsockTest23_10::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test23.10");
	return ret;
	}

CEsockTest23_10::~CEsockTest23_10()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest23_10::easyTestStepL()
	{
	
	// IsUnicast
	
	// get list of IP addresses
	if (!GetAppendixC1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i=0; i < appendixInArray.Count(); i++)
		{
		// check if the address is of type unicast
		if (i == ipAddress2 || i== ipAddress3 || i >= ipAddress7 && i <= ipAddress9 )
			{
			TESTL(appendixInArray[i].IsUnicast()!=EFalse);
			}
		else
			{
			TESTL(appendixInArray[i].IsUnicast()==EFalse);
			}
		}
	
	appendixInArray.Reset();
	return EPass;
	}

