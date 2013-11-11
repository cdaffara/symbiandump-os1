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
// Test step declarations for the PhonePower functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPHONEPOWER_H__
#define __CCTSYINTEGRATIONTESTPHONEPOWER_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tphonetsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestPhonePowerBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPhonePowerBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPhonePowerBase();

protected:

	TPhoneTsyTestHelper	iPhoneTsyTestHelper;
	}; // class CCTSYIntegrationTestPhonePowerBase


class CCTSYIntegrationTestPhonePower0001 : public CCTSYIntegrationTestPhonePowerBase
	{
public:

	CCTSYIntegrationTestPhonePower0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhonePower0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhonePower0001


class CCTSYIntegrationTestPhonePower0002 : public CCTSYIntegrationTestPhonePowerBase
	{
public:

	CCTSYIntegrationTestPhonePower0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhonePower0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhonePower0002



#endif // __CCTSYINTEGRATIONTESTPHONEPOWER_H__
