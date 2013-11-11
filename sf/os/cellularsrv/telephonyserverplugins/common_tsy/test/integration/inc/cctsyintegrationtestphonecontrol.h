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
// Test step declarations for the PhoneControl functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPHONECONTROL_H__
#define __CCTSYINTEGRATIONTESTPHONECONTROL_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestPhoneControlBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPhoneControlBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPhoneControlBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestPhoneControlBase


class CCTSYIntegrationTestPhoneControl0001 : public CCTSYIntegrationTestPhoneControlBase
	{
public:

	CCTSYIntegrationTestPhoneControl0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneControl0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneControl0001


class CCTSYIntegrationTestPhoneControl0002 : public CCTSYIntegrationTestPhoneControlBase
	{
public:

	CCTSYIntegrationTestPhoneControl0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneControl0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneControl0002



#endif // __CCTSYINTEGRATIONTESTPHONECONTROL_H__
