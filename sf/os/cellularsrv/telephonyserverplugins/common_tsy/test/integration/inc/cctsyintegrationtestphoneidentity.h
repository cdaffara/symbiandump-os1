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
// Test step declarations for the PhoneIdentity functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPHONEIDENTITY_H__
#define __CCTSYINTEGRATIONTESTPHONEIDENTITY_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tcallcontroltsytesthelper.h"
#include "tsupplementaltsytesthelper.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestPhoneIdentityBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPhoneIdentityBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPhoneIdentityBase();

protected:

	// Declare a TSY test helper here if required
	
	TCallControlTsyTestHelper iCallControlHelper;
	
	TSupplementalTsyTestHelper iSupplementalHelper;
	
	TNetworkTsyTestHelper iNetworkTestHelper;
	
	}; // class CCTSYIntegrationTestPhoneIdentityBase


class CCTSYIntegrationTestPhoneIdentity0001 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0001


class CCTSYIntegrationTestPhoneIdentity0002 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0002


class CCTSYIntegrationTestPhoneIdentity0003 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0003


class CCTSYIntegrationTestPhoneIdentity0004 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0004


class CCTSYIntegrationTestPhoneIdentity0005 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0005


class CCTSYIntegrationTestPhoneIdentity0006 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0006


class CCTSYIntegrationTestPhoneIdentity0007 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0007


class CCTSYIntegrationTestPhoneIdentity0008 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0008


class CCTSYIntegrationTestPhoneIdentity0009 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0009


class CCTSYIntegrationTestPhoneIdentity0010 : public CCTSYIntegrationTestPhoneIdentityBase
	{
public:

	CCTSYIntegrationTestPhoneIdentity0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneIdentity0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneIdentity0010



#endif // __CCTSYINTEGRATIONTESTPHONEIDENTITY_H__
