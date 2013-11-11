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
// Test step declarations for the AlternateLineService functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTALTERNATELINESERVICE_H__
#define __CCTSYINTEGRATIONTESTALTERNATELINESERVICE_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"
#include "tcallcontroltsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestAlternateLineServiceBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestAlternateLineServiceBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestAlternateLineServiceBase();

protected:

	TNetworkTsyTestHelper iNetworkTestHelper;
	TCallControlTsyTestHelper iCallControlTestHelper;

	}; // class CCTSYIntegrationTestAlternateLineServiceBase


class CCTSYIntegrationTestAlternateLineService0001 : public CCTSYIntegrationTestAlternateLineServiceBase
	{
public:

	CCTSYIntegrationTestAlternateLineService0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAlternateLineService0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAlternateLineService0001


class CCTSYIntegrationTestAlternateLineService0002 : public CCTSYIntegrationTestAlternateLineServiceBase
	{
public:

	CCTSYIntegrationTestAlternateLineService0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAlternateLineService0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAlternateLineService0002


class CCTSYIntegrationTestAlternateLineService0003 : public CCTSYIntegrationTestAlternateLineServiceBase
	{
public:

	CCTSYIntegrationTestAlternateLineService0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAlternateLineService0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAlternateLineService0003



#endif // __CCTSYINTEGRATIONTESTALTERNATELINESERVICE_H__
