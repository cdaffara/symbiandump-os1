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
// Test step declarations for the SupplementaryServiceCaps functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTSUPPLEMENTARYSERVICECAPS_H__
#define __CCTSYINTEGRATIONTESTSUPPLEMENTARYSERVICECAPS_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestSupplementaryServiceCapsBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestSupplementaryServiceCapsBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestSupplementaryServiceCapsBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestSupplementaryServiceCapsBase


class CCTSYIntegrationTestSupplementaryServiceCaps0001 : public CCTSYIntegrationTestSupplementaryServiceCapsBase
	{
public:

	CCTSYIntegrationTestSupplementaryServiceCaps0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSupplementaryServiceCaps0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSupplementaryServiceCaps0001



#endif // __CCTSYINTEGRATIONTESTSUPPLEMENTARYSERVICECAPS_H__
