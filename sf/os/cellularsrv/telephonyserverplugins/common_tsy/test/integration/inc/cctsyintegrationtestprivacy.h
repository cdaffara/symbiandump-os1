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
// Test step declarations for the Privacy functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPRIVACY_H__
#define __CCTSYINTEGRATIONTESTPRIVACY_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"
#include "tcallcontroltsytesthelper.h"


class CEtelSessionMgr;


class CCTSYIntegrationTestPrivacyBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPrivacyBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPrivacyBase();

protected:

	TNetworkTsyTestHelper   iNetworkTestHelper;
	TCallControlTsyTestHelper iCallControlTestHelper;

	}; // class CCTSYIntegrationTestPrivacyBase


class CCTSYIntegrationTestPrivacy0001 : public CCTSYIntegrationTestPrivacyBase
	{
public:

	CCTSYIntegrationTestPrivacy0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPrivacy0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPrivacy0001


class CCTSYIntegrationTestPrivacy0002 : public CCTSYIntegrationTestPrivacyBase
	{
public:

	CCTSYIntegrationTestPrivacy0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPrivacy0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPrivacy0002


class CCTSYIntegrationTestPrivacy0003 : public CCTSYIntegrationTestPrivacyBase
	{
public:

	CCTSYIntegrationTestPrivacy0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPrivacy0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPrivacy0003



#endif // __CCTSYINTEGRATIONTESTPRIVACY_H__
