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
// This contains ESock CloseServer Test
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "EsockCloseServer.h"
#include <c32root.h>


const TDesC& CEsockCloseServer::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"CloseServer");
	return ret;
	}



// destructor
CEsockCloseServer::~CEsockCloseServer()
	{
	}

// 
enum TVerdict CEsockCloseServer::easyTestStepL()
	{
	iEsockSuite->iSocketServer.Close();
	return EPass;
	}

