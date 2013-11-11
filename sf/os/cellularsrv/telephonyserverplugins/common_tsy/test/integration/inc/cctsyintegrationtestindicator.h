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
// Test step declarations for the Indicator functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTINDICATOR_H__
#define __CCTSYINTEGRATIONTESTINDICATOR_H__


#include <test/testexecutestepbase.h>
#include "tcallcontroltsytesthelper.h"
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;

class CCTSYIntegrationTestIndicatorBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestIndicatorBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestIndicatorBase();

protected:
	TNetworkTsyTestHelper iNetworkTestHelper;
	TCallControlTsyTestHelper iCallForwardingControlHelper;
	}; // class CCTSYIntegrationTestIndicatorBase


class CCTSYIntegrationTestIndicator0001 : public CCTSYIntegrationTestIndicatorBase
	{
public:

	CCTSYIntegrationTestIndicator0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestIndicator0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestIndicator0001


class CCTSYIntegrationTestIndicator0002 : public CCTSYIntegrationTestIndicatorBase
	{
public:

	CCTSYIntegrationTestIndicator0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestIndicator0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestIndicator0002


class CCTSYIntegrationTestIndicator0003 : public CCTSYIntegrationTestIndicatorBase
	{
public:

	CCTSYIntegrationTestIndicator0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestIndicator0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestIndicator0003



#endif // __CCTSYINTEGRATIONTESTINDICATOR_H__
