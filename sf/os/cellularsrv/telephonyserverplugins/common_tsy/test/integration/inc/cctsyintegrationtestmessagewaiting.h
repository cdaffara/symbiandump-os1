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
// Test step declarations for the MessageWaiting functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTMESSAGEWAITING_H__
#define __CCTSYINTEGRATIONTESTMESSAGEWAITING_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestMessageWaitingBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestMessageWaitingBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestMessageWaitingBase();

protected:

	TNetworkTsyTestHelper iNetworkTestHelper;

	}; // class CCTSYIntegrationTestMessageWaitingBase


class CCTSYIntegrationTestMessageWaiting0001 : public CCTSYIntegrationTestMessageWaitingBase
	{
public:

	CCTSYIntegrationTestMessageWaiting0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMessageWaiting0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMessageWaiting0001


class CCTSYIntegrationTestMessageWaiting0002 : public CCTSYIntegrationTestMessageWaitingBase
	{
public:

	CCTSYIntegrationTestMessageWaiting0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMessageWaiting0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMessageWaiting0002



#endif // __CCTSYINTEGRATIONTESTMESSAGEWAITING_H__
