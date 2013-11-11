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
// This contains ESock Test cases from section 24
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection24.h"


// Test step 24.1
const TDesC& CEsockTest24_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test24.1");
	return ret;
	}

CEsockTest24_1::~CEsockTest24_1()
	{
	}

enum TVerdict CEsockTest24_1::easyTestStepL()
	{
	TInetAddr addr;
	
	// flow label
	
	// set the IP address
	const TIp6Addr KInet6addr1 = {{{ 0xff,0xfe,0x10,0x2c,0xff,0xaa,0,0,0,0,0,0,0,0,0,1 }}};
	addr.SetAddress(KInet6addr1);
	
	
	TInt def_flow = addr.FlowLabel();
	TESTL(def_flow==0);
	
	// set the flow label
	addr.SetFlowLabel(27);
	
	// get the flow label
	TInt flow = addr.FlowLabel();
	TESTL(flow==27);
	
	// set the flow label again
	addr.SetFlowLabel(0xffff);
	
	// get the flow label again
	flow = addr.FlowLabel();
	TESTL(flow==0xffff);
	
	addr.SetFlowLabel(0);
	
	return EPass;
	}


// Test step 24.2
const TDesC& CEsockTest24_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test24.2");
	return ret;
	}

CEsockTest24_2::~CEsockTest24_2()
	{
	}

enum TVerdict CEsockTest24_2::easyTestStepL()
	{
	TInetAddr addr;
	
	// scope
	
	// set the IP address
	const TIp6Addr KInet6addr1 = {{{ 0xff,0xfe,0x10,0x2c,0xff,0xaa,0,0,0,0,0,0,0,0,0,1 }}};
	addr.SetAddress(KInet6addr1);
	
	TInt default_scope = addr.Scope();
	TESTL(default_scope==0);
	
	
	// set the scope
	addr.SetScope(1);
	
	// get the scope
	TInt scope = addr.Scope();
	TESTL(scope==1);
	
	// change the scope
	addr.SetScope(15);
	
	// get the scope
	scope = addr.Scope();
	TESTL(scope==15);
	
	addr.SetScope(0);
	
	
	return EPass;
	}

