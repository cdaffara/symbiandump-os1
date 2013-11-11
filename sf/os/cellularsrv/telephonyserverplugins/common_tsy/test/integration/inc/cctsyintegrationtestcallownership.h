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
// Test step declarations for the CallOwnership functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTCALLOWNERSHIP_H__
#define __CCTSYINTEGRATIONTESTCALLOWNERSHIP_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestCallOwnershipBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestCallOwnershipBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestCallOwnershipBase();

protected:

	TNetworkTsyTestHelper iNetworkTestHelper;
	
	}; // class CCTSYIntegrationTestCallOwnershipBase


class CCTSYIntegrationTestCallOwnership0001 : public CCTSYIntegrationTestCallOwnershipBase
	{
public:

	CCTSYIntegrationTestCallOwnership0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallOwnership0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallOwnership0001


class CCTSYIntegrationTestCallOwnership0002 : public CCTSYIntegrationTestCallOwnershipBase
	{
public:

	CCTSYIntegrationTestCallOwnership0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallOwnership0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallOwnership0002


class CCTSYIntegrationTestCallOwnership0003 : public CCTSYIntegrationTestCallOwnershipBase
	{
public:

	CCTSYIntegrationTestCallOwnership0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallOwnership0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallOwnership0003


class CCTSYIntegrationTestCallOwnership0004 : public CCTSYIntegrationTestCallOwnershipBase
	{
public:

	CCTSYIntegrationTestCallOwnership0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallOwnership0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallOwnership0004


class CCTSYIntegrationTestCallOwnership0005 : public CCTSYIntegrationTestCallOwnershipBase
	{
public:

	CCTSYIntegrationTestCallOwnership0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallOwnership0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallOwnership0005



#endif // __CCTSYINTEGRATIONTESTCALLOWNERSHIP_H__
