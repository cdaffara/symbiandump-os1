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

#if !defined (__TESTSUITENETCON_H_)
#define __TESTSUITENETCON_H_

#include "networking/testsuite.h"

class CTestStepNetCon;
class CTestSuiteNetCon : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestSuiteNetCon();
	void AddTestStepL(CTestStepNetCon* aTestStep );
	TPtrC GetVersion();
	};

#endif // __TESTSUITENETCON_H_

