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

#ifndef TESTSTEPBASE_H
#define TESTSTEPBASE_H

#include <test/TestExecuteStepBase.h>
#include <es_sock.h>
#include "testserver.h"

class CBluetoothUtilsTestStepBase : public CTestStep
	{
public:
	virtual ~CBluetoothUtilsTestStepBase();
	virtual TVerdict doTestStepPreambleL();

protected:
	CBluetoothUtilsTestStepBase(CBluetoothUtilsTestServer &aMyTestServer);
	CBluetoothUtilsTestServer &iParent;
	};
//---------------------------------------------------------------------------------------------------

#endif //TESTSTEPBASE_H