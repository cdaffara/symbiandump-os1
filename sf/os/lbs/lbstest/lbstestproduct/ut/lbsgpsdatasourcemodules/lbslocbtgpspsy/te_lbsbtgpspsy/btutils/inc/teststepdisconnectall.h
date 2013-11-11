// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef TESTSTEPDISCONNECTALL_H
#define TESTSTEPDISCONNECTALL_H

#include "teststepbase.h"
#include <es_sock.h>
#include "testserver.h"

//---------------------------------------------------------------------------------------------------
_LIT( KTestStepDisconnectAll,  "TestStepDisconnectAll");
//---------------------------------------------------------------------------------------------------

/*
Step cleans resources
*/
class CTestStepDisconnectAll : public CBluetoothUtilsTestStepBase
	{
public:
	static CTestStepDisconnectAll* New(CBluetoothUtilsTestServer &aTestServer);
	TVerdict doTestStepL();
	~CTestStepDisconnectAll();

private:
	CTestStepDisconnectAll(CBluetoothUtilsTestServer &aTestServer);
	};
//---------------------------------------------------------------------------------------------------
#endif //TESTSTEPDISCONNECTALL_H