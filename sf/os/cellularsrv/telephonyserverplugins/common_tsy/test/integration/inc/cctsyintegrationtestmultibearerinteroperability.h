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
// Test step declarations for the MultiBearerInteroperability functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTMULTIBEARERINTEROPERABILITY_H__
#define __CCTSYINTEGRATIONTESTMULTIBEARERINTEROPERABILITY_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tcallcontroltsytesthelper.h" 
#include "tnetworktsytesthelper.h" 
#include "tpacketservicetsytesthelper.h"
#include "tsmsstoretsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestMultiBearerInteroperabilityBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestMultiBearerInteroperabilityBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestMultiBearerInteroperabilityBase();

protected:

    void SetAttachModeToWhenPossibleAndWaitForAttachL(RPacketService& aPacketService);
	TNetworkTsyTestHelper	iNetworkTestHelper;
	TPacketServiceTsyTestHelper iPacketServiceTestHelper;
	TCallControlTsyTestHelper iCallControlTestHelper;
	TSmsStoreTsyTestHelper iSmsStoreTsyTestHelper;

	}; // class CCTSYIntegrationTestMultiBearerInteroperabilityBase


class CCTSYIntegrationTestMultiBearerInteroperability0001 : public CCTSYIntegrationTestMultiBearerInteroperabilityBase
	{
public:

	CCTSYIntegrationTestMultiBearerInteroperability0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultiBearerInteroperability0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultiBearerInteroperability0001


class CCTSYIntegrationTestMultiBearerInteroperability0002 : public CCTSYIntegrationTestMultiBearerInteroperabilityBase
	{
public:

	CCTSYIntegrationTestMultiBearerInteroperability0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultiBearerInteroperability0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultiBearerInteroperability0002


class CCTSYIntegrationTestMultiBearerInteroperability0003 : public CCTSYIntegrationTestMultiBearerInteroperabilityBase
	{
public:

	CCTSYIntegrationTestMultiBearerInteroperability0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultiBearerInteroperability0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultiBearerInteroperability0003


class CCTSYIntegrationTestMultiBearerInteroperability0004 : public CCTSYIntegrationTestMultiBearerInteroperabilityBase
	{
public:

	CCTSYIntegrationTestMultiBearerInteroperability0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultiBearerInteroperability0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultiBearerInteroperability0004


class CCTSYIntegrationTestMultiBearerInteroperability0005 : public CCTSYIntegrationTestMultiBearerInteroperabilityBase
	{
public:

	CCTSYIntegrationTestMultiBearerInteroperability0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestMultiBearerInteroperability0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestMultiBearerInteroperability0005



#endif // __CCTSYINTEGRATIONTESTMULTIBEARERINTEROPERABILITY_H__
