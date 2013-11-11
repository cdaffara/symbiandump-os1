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
// Test step declarations for the UserNetworkAccess functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTUSERNETWORKACCESS_H__
#define __CCTSYINTEGRATIONTESTUSERNETWORKACCESS_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestUserNetworkAccessBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestUserNetworkAccessBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestUserNetworkAccessBase();

protected:

	TNetworkTsyTestHelper iNetworkTestHelper;

	}; // class CCTSYIntegrationTestUserNetworkAccessBase


class CCTSYIntegrationTestUserNetworkAccess0001 : public CCTSYIntegrationTestUserNetworkAccessBase
	{
public:

	CCTSYIntegrationTestUserNetworkAccess0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUserNetworkAccess0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUserNetworkAccess0001



#endif // __CCTSYINTEGRATIONTESTUSERNETWORKACCESS_H__
