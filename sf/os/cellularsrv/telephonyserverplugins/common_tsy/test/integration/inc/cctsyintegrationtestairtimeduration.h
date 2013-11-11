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
// Test step declarations for the AirTimeDuration functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTAIRTIMEDURATION_H__
#define __CCTSYINTEGRATIONTESTAIRTIMEDURATION_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestAirTimeDurationBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestAirTimeDurationBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestAirTimeDurationBase();
	

protected:

	// Declare a TSY test helper here if required
	TNetworkTsyTestHelper iNetworkTestHelper;
	
	}; // class CCTSYIntegrationTestAirTimeDurationBase


class CCTSYIntegrationTestAirTimeDuration0001 : public CCTSYIntegrationTestAirTimeDurationBase
	{
public:

	CCTSYIntegrationTestAirTimeDuration0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAirTimeDuration0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAirTimeDuration0001


class CCTSYIntegrationTestAirTimeDuration0002 : public CCTSYIntegrationTestAirTimeDurationBase
	{
public:

	CCTSYIntegrationTestAirTimeDuration0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAirTimeDuration0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAirTimeDuration0002


class CCTSYIntegrationTestAirTimeDuration0003 : public CCTSYIntegrationTestAirTimeDurationBase
	{
public:

	CCTSYIntegrationTestAirTimeDuration0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAirTimeDuration0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAirTimeDuration0003


class CCTSYIntegrationTestAirTimeDuration0004 : public CCTSYIntegrationTestAirTimeDurationBase
	{
public:

	CCTSYIntegrationTestAirTimeDuration0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAirTimeDuration0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAirTimeDuration0004



#endif // __CCTSYINTEGRATIONTESTAIRTIMEDURATION_H__
