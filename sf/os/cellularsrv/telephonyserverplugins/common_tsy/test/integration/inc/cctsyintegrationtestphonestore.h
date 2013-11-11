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
// Test step declarations for the PhoneStore functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPHONESTORE_H__
#define __CCTSYINTEGRATIONTESTPHONESTORE_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestPhoneStoreBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPhoneStoreBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPhoneStoreBase();

protected:

	// Declare a TSY test helper here if required

	}; // class CCTSYIntegrationTestPhoneStoreBase


class CCTSYIntegrationTestPhoneStore0001 : public CCTSYIntegrationTestPhoneStoreBase
	{
public:

	CCTSYIntegrationTestPhoneStore0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneStore0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneStore0001


class CCTSYIntegrationTestPhoneStore0002 : public CCTSYIntegrationTestPhoneStoreBase
	{
public:

	CCTSYIntegrationTestPhoneStore0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneStore0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneStore0002


class CCTSYIntegrationTestPhoneStore0003 : public CCTSYIntegrationTestPhoneStoreBase
	{
public:

	CCTSYIntegrationTestPhoneStore0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneStore0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneStore0003



#endif // __CCTSYINTEGRATIONTESTPHONESTORE_H__
