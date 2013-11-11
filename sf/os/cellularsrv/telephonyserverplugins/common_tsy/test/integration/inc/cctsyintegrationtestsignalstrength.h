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
// Test step declarations for the SignalStrength functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTSIGNALSTRENGTH_H__
#define __CCTSYINTEGRATIONTESTSIGNALSTRENGTH_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestSignalStrengthBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestSignalStrengthBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestSignalStrengthBase();

protected:
	
	TNetworkTsyTestHelper iNetworkTestHelper;

	}; // class CCTSYIntegrationTestSignalStrengthBase


class CCTSYIntegrationTestSignalStrength0001 : public CCTSYIntegrationTestSignalStrengthBase
	{
public:

	CCTSYIntegrationTestSignalStrength0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSignalStrength0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSignalStrength0001


class CCTSYIntegrationTestSignalStrength0002 : public CCTSYIntegrationTestSignalStrengthBase
	{
public:

	CCTSYIntegrationTestSignalStrength0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSignalStrength0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSignalStrength0002



#endif // __CCTSYINTEGRATIONTESTSIGNALSTRENGTH_H__
