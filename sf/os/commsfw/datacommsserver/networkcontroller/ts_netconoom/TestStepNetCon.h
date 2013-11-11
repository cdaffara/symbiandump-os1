// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __TESTSTEPNETCON_H__)
#define __TESTSTEPNETCON_H__

#include "networking/teststep.h"

class CTestSuiteNetCon;
class CNetConTestBase;
class CTestStepNetCon : public CTestStep
	{
public:

	CTestStepNetCon(TInt aTestNumber);
	~CTestStepNetCon();

	enum TVerdict doTestStepL();

	CTestSuiteNetCon* iTestSuite;

private:
	CNetConTestBase* GetTestLC();

private:
	TInt iTestNumber;
	};

#endif // __TESTSTEPNETCON_H__

