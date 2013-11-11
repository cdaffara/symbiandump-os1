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
// Test step declarations for the FixedDialling functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTFIXEDDIALLING_H__
#define __CCTSYINTEGRATIONTESTFIXEDDIALLING_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tphonetsytesthelper.h"
#include "tcallcontroltsytesthelper.h"
#include "tnetworktsytesthelper.h"
#include "tsimtsytesthelper.h" 

class CEtelSessionMgr;

class CCTSYIntegrationTestFixedDiallingBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestFixedDiallingBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestFixedDiallingBase();
	
protected:

	void GetPIN2PasswordL( RMobilePhone &aPhone, RMobilePhone::TMobilePassword& aPassword );
	

protected:

	TPhoneTsyTestHelper	iPhoneTsyTestHelper;
	TCallControlTsyTestHelper iCallControlTsyTestHelper;
	TNetworkTsyTestHelper iNetworkTsyTestHelper;
	TSimTsyTestHelper iSimTsyTestHelper;

	}; // class CCTSYIntegrationTestFixedDiallingBase


class CCTSYIntegrationTestFixedDialling0001 : public CCTSYIntegrationTestFixedDiallingBase
	{
public:

	CCTSYIntegrationTestFixedDialling0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestFixedDialling0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();
	
private:

	}; // class CCTSYIntegrationTestFixedDialling0001


class CCTSYIntegrationTestFixedDialling0002 : public CCTSYIntegrationTestFixedDiallingBase
	{
public:

	CCTSYIntegrationTestFixedDialling0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestFixedDialling0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestFixedDialling0002


class CCTSYIntegrationTestFixedDialling0003 : public CCTSYIntegrationTestFixedDiallingBase
	{
public:

	CCTSYIntegrationTestFixedDialling0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestFixedDialling0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestFixedDialling0003


class CCTSYIntegrationTestFixedDialling0004 : public CCTSYIntegrationTestFixedDiallingBase
	{
public:

	CCTSYIntegrationTestFixedDialling0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestFixedDialling0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestFixedDialling0004


#endif // __CCTSYINTEGRATIONTESTFIXEDDIALLING_H__
