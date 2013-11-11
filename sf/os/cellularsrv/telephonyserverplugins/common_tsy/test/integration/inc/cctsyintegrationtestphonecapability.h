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
// Test step declarations for the PhoneCapability functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPHONECAPABILITY_H__
#define __CCTSYINTEGRATIONTESTPHONECAPABILITY_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestPhoneCapabilityBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPhoneCapabilityBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPhoneCapabilityBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestPhoneCapabilityBase


class CCTSYIntegrationTestPhoneCapability0001 : public CCTSYIntegrationTestPhoneCapabilityBase
	{
public:

	CCTSYIntegrationTestPhoneCapability0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneCapability0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneCapability0001


class CCTSYIntegrationTestPhoneCapability0002 : public CCTSYIntegrationTestPhoneCapabilityBase
	{
public:

	CCTSYIntegrationTestPhoneCapability0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneCapability0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneCapability0002



#endif // __CCTSYINTEGRATIONTESTPHONECAPABILITY_H__
