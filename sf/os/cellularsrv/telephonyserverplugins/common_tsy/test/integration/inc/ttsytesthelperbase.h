// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSYTESTHELPERS_H_
#define TSYTESTHELPERS_H_

#include <etelmm.h>
#include <etelqos.h>
#include "cctsyintegrationtestsuitebase.h"
#include "comparisonfunctions.h"

// Forward declarations
class TEtelRequestBase;

class TTsyTestHelperBase
	{
public:
	TTsyTestHelperBase(CCTSYIntegrationTestSuiteStepBase& aTestStep);

protected:

	inline CTestExecuteLogger& Logger() {return iTestStep.Logger(); };
	inline void SetAssertResult() { iTestStep.SetAssertResult(); }
	inline void SetCheckResultL() { iTestStep.SetCheckResultL(); }
	
protected:

	CCTSYIntegrationTestSuiteStepBase& iTestStep;

	}; // class TTsyTestHelperBase

#endif // TSYTESTHELPERS_H_

