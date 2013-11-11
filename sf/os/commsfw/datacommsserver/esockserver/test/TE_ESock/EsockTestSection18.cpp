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
// This contains ESock Test cases from section 18
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection18.h"


// Test step 18.1
const TDesC& CEsockTest18_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test18.1");
	return ret;
	}

CEsockTest18_1::~CEsockTest18_1()
	{
	}

enum TVerdict CEsockTest18_1::easyTestStepL()
	{
	const TIp6Addr KInet6AddrOne = {{{0xff,0xff,0x2f,0xfe,0,0,0,0,0,0,0,0,0,0,0,0}}};
	TBuf<39> buf;
	
	// set IP address
	TInetAddr addr(KInet6AddrOne, 5001);
	
	// check address has bben set correctly
	addr.OutputWithScope(buf);
	TESTL(buf==_L("ffff:2ffe::"));
	
	// check port number set correctly
	TESTL(addr.Port()==5001);
	
	return EPass;
	}

