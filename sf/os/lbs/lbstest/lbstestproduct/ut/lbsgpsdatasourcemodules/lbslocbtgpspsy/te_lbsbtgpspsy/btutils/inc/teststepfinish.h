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

#ifndef TESTSTEPFINISH_H
#define TESTSTEPFINISH_H

#include "teststepbase.h"
#include <es_sock.h>
#include "testserver.h"

//---------------------------------------------------------------------------------------------------
_LIT( KTestStepFinish,  "TestStepFinish");
//---------------------------------------------------------------------------------------------------

/*
Step cleans resources
*/
class CTestStepFinish : public CBluetoothUtilsTestStepBase
	{
public:
	static CTestStepFinish* New(CBluetoothUtilsTestServer &aTestServer);
	TVerdict doTestStepL();
	~CTestStepFinish();

private:
	CTestStepFinish(CBluetoothUtilsTestServer &aTestServer);
	};
//---------------------------------------------------------------------------------------------------
#endif //TESTSTEPFINISH_H