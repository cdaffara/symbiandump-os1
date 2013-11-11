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
// Test step declarations for the CallForwarding functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTCALLFORWARDING_H__
#define __CCTSYINTEGRATIONTESTCALLFORWARDING_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tcallcontroltsytesthelper.h"
#include "tsupplementaltsytesthelper.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestCallForwardingBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestCallForwardingBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestCallForwardingBase();

protected:

	TBool SetCallForwardingStatusUtilL(RMobilePhone& aMobilePhone, RMobilePhone::TMobilePhoneCFChangeV1 aNewSetting, RMobilePhone::TMobilePhoneCFCondition aCFConditionSet, TInt aExpected);

	// Declare a TSY test helper here if required
	
	TSupplementalTsyTestHelper iCallForwardingSupplementalHelper;
	TCallControlTsyTestHelper iCallForwardingControlHelper;
	TNetworkTsyTestHelper iNetworkTestHelper;
	
	}; // class CCTSYIntegrationTestCallForwardingBase


class CCTSYIntegrationTestCallForwarding0001 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0001


class CCTSYIntegrationTestCallForwarding0002 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0002


class CCTSYIntegrationTestCallForwarding0003 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0003


class CCTSYIntegrationTestCallForwarding0004 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0004


class CCTSYIntegrationTestCallForwarding0005 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0005


class CCTSYIntegrationTestCallForwarding0006 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0006


class CCTSYIntegrationTestCallForwarding0007 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0007


class CCTSYIntegrationTestCallForwarding0008 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0008


class CCTSYIntegrationTestCallForwarding0009 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0009


class CCTSYIntegrationTestCallForwarding0010 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0010


class CCTSYIntegrationTestCallForwarding0011 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0011


class CCTSYIntegrationTestCallForwarding0012 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0012


class CCTSYIntegrationTestCallForwarding0013 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0013


class CCTSYIntegrationTestCallForwarding0014 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0014


class CCTSYIntegrationTestCallForwarding0015 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0015


class CCTSYIntegrationTestCallForwarding0016 : public CCTSYIntegrationTestCallForwardingBase
	{
public:

	CCTSYIntegrationTestCallForwarding0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallForwarding0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallForwarding0016



#endif // __CCTSYINTEGRATIONTESTCALLFORWARDING_H__
