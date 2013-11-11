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
// This contains ESock Test cases from section 15
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection15.h"


// Test step 15.1
const TDesC& CEsockTest15_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test15.1");
	return ret;
	}

CEsockTest15_1::~CEsockTest15_1()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest15_1::easyTestStepL()
	{
	TInetAddr addr;
	TBuf<39> buf;
	
	// input valid IP addresses
	
	// get valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		{
		return EFail;
		}

	// for each IP address
	for (TInt i = 0; i < appendixInArray.Count(); i++)
		{
		// convert to a string
		appendixInArray[i].OutputWithScope(buf);
		
		// check Input is successful
		TESTL(addr.Input(buf)==KErrNone);
		TESTL(appendixInArray[i].CmpAddr(addr) );
		}
	
	appendixInArray.Reset();
	return EPass;
	}


// Test step 15.2
const TDesC& CEsockTest15_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test15.2");
	return ret;
	}

CEsockTest15_2::~CEsockTest15_2()
	{
	}

enum TVerdict CEsockTest15_2::easyTestStepL()
	{
	TInetAddr addr;
	
	// input invalid IP addresses
	TESTL(addr.Input(_L("256.1.2.3"))!=KErrNone);
	TESTL(addr.Input(_L("56.1.999.3"))!=KErrNone);
	TESTL(addr.Input(_L("0x1000000000"))!=KErrNone);
	TESTL(addr.Input(_L("10.0x1000000"))!=KErrNone);
	TESTL(addr.Input(_L("10.0.0x10000"))!=KErrNone);
	TESTL(addr.Input(_L("10.0.0.0x100"))!=KErrNone);
	TESTL(addr.Input(_L("56.1.XYZ.3"))!=KErrNone);
	TESTL(addr.Input(_L("..1.2.3"))!=KErrNone);
	TESTL(addr.Input(_L("1.2.3.4."))!=KErrNone);
	TESTL(addr.Input(_L("www.symbian.com"))!=KErrNone);
	
	return EPass;
	}


// Test step 15.3
const TDesC& CEsockTest15_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test15.3");
	return ret;
	}

CEsockTest15_3::~CEsockTest15_3()
	{
	appendixInArray.Close();
	}

enum TVerdict CEsockTest15_3::easyTestStepL()
	{
	TBuf<39> buf;
	TInetAddr addr;
	
	// Output
	
	// get valid IPv4 Addresses - appendixInArray
	if (!GetAppendixB1())
		{
		return EFail;
		}
	
	// for each IP address
	for (TInt i = 0; i < appendixInArray.Count(); i++)
		{
		// call Output for each IP address
		appendixInArray[i].OutputWithScope(buf);
		
		// check buffer is populated
		TESTL( buf.Length()>0);
		
		addr.Input( buf );
		TESTL( appendixInArray[i].CmpAddr(addr) );
		}
	
	appendixInArray.Reset();
	return EPass;
	}

