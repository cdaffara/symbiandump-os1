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
// Test step declarations for the NetworkSecurity functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTNETWORKSECURITY_H__
#define __CCTSYINTEGRATIONTESTNETWORKSECURITY_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestNetworkSecurityBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestNetworkSecurityBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestNetworkSecurityBase();

protected:

	TNetworkTsyTestHelper iNetworkTestHelper;
	}; // class CCTSYIntegrationTestNetworkSecurityBase


class CCTSYIntegrationTestNetworkSecurity0001 : public CCTSYIntegrationTestNetworkSecurityBase
	{
public:

	CCTSYIntegrationTestNetworkSecurity0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkSecurity0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkSecurity0001


class CCTSYIntegrationTestNetworkSecurity0002 : public CCTSYIntegrationTestNetworkSecurityBase
	{
public:

	CCTSYIntegrationTestNetworkSecurity0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkSecurity0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkSecurity0002



#endif // __CCTSYINTEGRATIONTESTNETWORKSECURITY_H__
