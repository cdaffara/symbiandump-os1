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
// Test step declarations for the SessionManagement functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTSESSIONMANAGEMENT_H__
#define __CCTSYINTEGRATIONTESTSESSIONMANAGEMENT_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestSessionManagementBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestSessionManagementBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestSessionManagementBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestSessionManagementBase


class CCTSYIntegrationTestSessionManagement0001 : public CCTSYIntegrationTestSessionManagementBase
	{
public:

	CCTSYIntegrationTestSessionManagement0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSessionManagement0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSessionManagement0001



#endif // __CCTSYINTEGRATIONTESTSESSIONMANAGEMENT_H__
