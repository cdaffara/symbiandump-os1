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
// This contains ESock Test cases from section 17
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection17.h"


// Test step 17.1
const TDesC& CEsockTest17_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.1");
	return ret;
	}

CEsockTest17_1::~CEsockTest17_1()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest17_1::easyTestStepL()
	{
	//
	// ClassA()  	   //
	//
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		// Get IP Class A
		if ((i == 0)||(i==5)||(i==6))	//These are all class A addresses
			{
			TESTL(appendixInArray[i].IsClassA()!=EFalse);
			}
		else
			{
			TESTL(appendixInArray[i].IsClassA()==EFalse);
			}
		} 

	appendixInArray.Reset();
	return EPass;
	}


// Test step 17.2
const TDesC& CEsockTest17_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.2");
	return ret;
	}

CEsockTest17_2::~CEsockTest17_2()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest17_2::easyTestStepL()
	{
	//
	// ClassB()  	   //
	//
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		// Get IP Class B
		if (i == 1)
			{
			TESTL(appendixInArray[i].IsClassB()!=EFalse);
			}
		else
			{
			TESTL(appendixInArray[i].IsClassB()==EFalse);
			}
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 17.3
const TDesC& CEsockTest17_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.3");
	return ret;
	}

CEsockTest17_3::~CEsockTest17_3()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest17_3::easyTestStepL()
	{
	//
	// ClassC()  	   //
	//
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		if (i == 2)
			{
			TESTL(appendixInArray[i].IsClassC()!=EFalse);
			}
		else
			{
			TESTL(appendixInArray[i].IsClassC()==EFalse);
			}
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 17.4
const TDesC& CEsockTest17_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.4");
	return ret;
	}

CEsockTest17_4::~CEsockTest17_4()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest17_4::easyTestStepL()
	{
	//
	// IsMulticast() - ClassD //
	//
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		if (i == 3)
			{
			TESTL(appendixInArray[i].IsMulticast()!=EFalse);
			}
		else
			{
			TESTL(appendixInArray[i].IsMulticast()==EFalse);
			}
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 17.5
const TDesC& CEsockTest17_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.5");
	return ret;
	}

CEsockTest17_5::~CEsockTest17_5()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest17_5::easyTestStepL()
	{
	//
	// IsWildAddr()           //
	//
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		if (i == 6)
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


// Test step 17.6
const TDesC& CEsockTest17_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.6");
	return ret;
	}

CEsockTest17_6::~CEsockTest17_6()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest17_6::easyTestStepL()
	{
	//
	// IsBroadcast()          //
	//
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		if (i == 7)
			{
			TESTL(appendixInArray[i].IsBroadcast()!=EFalse);
			}
		else
			{
			TESTL(appendixInArray[i].IsBroadcast()==EFalse);
			}
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 17.7
const TDesC& CEsockTest17_7::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.7");
	return ret;
	}

CEsockTest17_7::~CEsockTest17_7()
	{
	}

enum TVerdict CEsockTest17_7::easyTestStepL()
	{
	//
	//     IsWildPort() //
	//
	
	TInetAddr addr;
	
	addr.SetAddress(IPADDR(126,0,0,1));
	addr.SetPort(0);		// IP Address to 126,0,0,1 and Port to Zero
	TESTL(addr.IsWildPort()!=EFalse);				// Port = 0 -	True
	
	addr.SetPort(21);								// Sets Port to 21 FTP
	TESTL(addr.IsWildPort()==EFalse);				// Port = 21 -	False
	
	addr.SetAddress(IPADDR(191,220,145,22));		// IP Address to 191.220.145.22
	TESTL(addr.IsWildPort()==EFalse);				// Port = 21 -	False
	
	addr.SetPort(0);								// Sets Port to 0
	TESTL(addr.IsWildPort()!=EFalse);				// Port = 0 -	True
	
	return EPass;
	}


// Test step 17.8
const TDesC& CEsockTest17_8::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.8");
	return ret;
	}

CEsockTest17_8::~CEsockTest17_8()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest17_8::easyTestStepL()
	{
	//
	// IsLoopback()           //
	//
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		if (i == 5)
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


// Test step 17.9
const TDesC& CEsockTest17_9::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.9");
	return ret;
	}

CEsockTest17_9::~CEsockTest17_9()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest17_9::easyTestStepL()
	{
	//
	// IsUnspecified()        //
	//
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		if (i == 6)
			{
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


// Test step 17.10
const TDesC& CEsockTest17_10::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test17.10");
	return ret;
	}

CEsockTest17_10::~CEsockTest17_10()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest17_10::easyTestStepL()
	{
	//
	// IsUnicast()     //
	//	If NOT Multicast() (0xff) then is IsUnicast()
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	// Class A - Pass
	TESTL(appendixInArray[0].IsUnicast()!=EFalse);
	
	// Class B - Pass
	TESTL(appendixInArray[1].IsUnicast()!=EFalse);
	
	// Class C - Pass
	TESTL(appendixInArray[2].IsUnicast()!=EFalse);
	
	// Class D - Fail
	TESTL(appendixInArray[3].IsUnicast()==EFalse);
	
	// Loopback - Pass
	TESTL(appendixInArray[5].IsUnicast()!=EFalse);
	
	// Unspecified - Fail
	TESTL(appendixInArray[6].IsUnicast()==EFalse);
	
	appendixInArray.Reset();
	return EPass;
	}

