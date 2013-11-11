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
// This contains ESock Test cases from section 16
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection16.h"


// Test step 16.1
const TDesC& CEsockTest16_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test16.1");
	return ret;
	}

CEsockTest16_1::~CEsockTest16_1()
	{
	appendixInArray.Close();
	appendixOutArray.Close();
	}

enum TVerdict CEsockTest16_1::easyTestStepL()
	{
	//
	// NetMask()	   //
	//
	
	TInetAddr addr;
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	// expected result - appendixOutArray
	if (!GetAppendixB2())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		// Get IP subnet mask
		addr.NetMask(appendixInArray[i]);
		
		// Match against expected result
		TESTL(addr.Match(appendixOutArray[i]));
		}
	
	appendixInArray.Reset();
	appendixOutArray.Reset();
	return EPass;
	}


// Test step 16.2
const TDesC& CEsockTest16_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test16.2");
	return ret;
	}

CEsockTest16_2::~CEsockTest16_2()
	{
	appendixInArray.Close();
	appendixOutArray.Close();
	}

enum TVerdict CEsockTest16_2::easyTestStepL()
	{
	//
	// Net()       //
	//
	
	TInetAddr addr;
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	// expected result - appendixOutArray
	if (!GetAppendixB3())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		// Get IP net
		addr.Net(appendixInArray[i]);
		
		// Match against expected result
		TESTL(addr.Match(appendixOutArray[i]));
		}

	appendixInArray.Reset();
	appendixOutArray.Reset();
	return EPass;
	}


// Test step 16.3
const TDesC& CEsockTest16_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test16.3");
	return ret;
	}

CEsockTest16_3::~CEsockTest16_3()
	{
	appendixInArray.Close();
	appendixOutArray.Close();
	}

enum TVerdict CEsockTest16_3::easyTestStepL()
	{
	//
	// SubNet()       //
	//
	TInetAddr addr, mask;
	
	mask.SetAddress(IPADDR(255,255,255,0));
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	// expected result - appendixOutArray
	if (!GetAppendixB4())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		// Get IP Subnet
		addr.SubNet(appendixInArray[i], mask);
		
		// Match against expected result
		TESTL(addr.Match(appendixOutArray[i]));
		}

	appendixInArray.Reset();
	appendixOutArray.Reset();
	return EPass;
	}


// Test step 16.4
const TDesC& CEsockTest16_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test16.4");
	return ret;
	}

CEsockTest16_4::~CEsockTest16_4()
	{
	appendixInArray.Close();
	appendixOutArray.Close();
	}

enum TVerdict CEsockTest16_4::easyTestStepL()
	{
	//
	// NetBroadcast()       //
	//
	
	TInetAddr addr;
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	// expected result - appendixOutArray
	if (!GetAppendixB5())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		// Get IP netbroadcast
		addr.NetBroadcast(appendixInArray[i]);
		
		// Match against expected result
		TESTL(addr.Match(appendixOutArray[i]));
		}

	appendixInArray.Reset();
	appendixOutArray.Reset();
	return EPass;
	}


// Test step 16.5
const TDesC& CEsockTest16_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test16.5");
	return ret;
	}

CEsockTest16_5::~CEsockTest16_5()
	{
	appendixInArray.Close();
	appendixOutArray.Close();
	}

enum TVerdict CEsockTest16_5::easyTestStepL()
	{
	//
	// SubNetBroadcast()       //
	//
	
	TInetAddr addr, mask;
	
	mask.SetAddress(IPADDR(255,255,255,0));
	
	// Get Valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		return EFail;
	
	// expected result - appendixOutArray
	if (!GetAppendixB6())
		return EFail;
	
	for (TInt i = 0;i < appendixInArray.Count();i++)
		{
		// Get IP subnetbroadcast
		addr.SubNetBroadcast(appendixInArray[i], mask);
		
		// Match against expected result
		TESTL(addr.Match(appendixOutArray[i]));
		}

	appendixInArray.Reset();
	appendixOutArray.Reset();
	return EPass;
	}

