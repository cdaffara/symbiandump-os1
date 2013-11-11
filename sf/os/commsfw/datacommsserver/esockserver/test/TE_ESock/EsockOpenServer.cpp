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
// This contains ESock TestCase OpenServer
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "esockopenserver.h"
#include <c32root.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestEsckOpn, "ESockTestEsckOpn");
#endif

const TDesC& CEsockOpenServer::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"OpenServer");
	
	return ret;
	}

// destructor
CEsockOpenServer::~CEsockOpenServer()
	{
	}

// 
enum TVerdict CEsockOpenServer::easyTestStepL()
	{
	__ASSERT_DEBUG(iEsockSuite, User::Panic(KSpecAssert_ESockTestEsckOpn, 1));
	const TInt ret = iEsockSuite->iSocketServer.Connect();
	if (ret==KErrNone)
		{
		iEsockSuite->iSocketServer.ShareAuto();
		Logger().WriteFormat(_L("Connecting to socket server success"));
		return EPass;
		}
	else
		{
		Logger().WriteFormat(_L("Connecting to socket server failed with %d"), ret);
		return EFail;
		}
	}


