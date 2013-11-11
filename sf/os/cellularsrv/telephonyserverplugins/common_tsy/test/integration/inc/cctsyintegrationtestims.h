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
// Test step declarations for the IMS functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTIMS_H__
#define __CCTSYINTEGRATIONTESTIMS_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestIMSBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestIMSBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestIMSBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestIMSBase


class CCTSYIntegrationTestIMS0001 : public CCTSYIntegrationTestIMSBase
	{
public:

	CCTSYIntegrationTestIMS0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestIMS0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestIMS0001



#endif // __CCTSYINTEGRATIONTESTIMS_H__
