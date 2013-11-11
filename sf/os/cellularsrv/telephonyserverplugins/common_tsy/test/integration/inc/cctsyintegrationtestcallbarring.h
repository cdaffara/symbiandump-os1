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
// Test step declarations for the CallBarring functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTCALLBARRING_H__
#define __CCTSYINTEGRATIONTESTCALLBARRING_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "listretrievalhelpers.h"
#include "tsupplementaltsytesthelper.h" 
#include "tcallcontroltsytesthelper.h" 
#include "tnetworktsytesthelper.h" 

class CEtelSessionMgr;


class CCTSYIntegrationTestCallBarringBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestCallBarringBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestCallBarringBase();

protected:
	void CheckCBPasswordL( RMobilePhone &aPhone, RMobilePhone::TMobilePassword& aPassword );
	
	TInt DeactivateCallBarringL( RMobilePhone &aPhone, const RMobilePhone::TMobilePassword& aPassword );

	TInt SetCBStatusL( RMobilePhone& aPhone, 
						RMobilePhone::TMobilePhoneCBChangeV1& aInfo,
						RMobilePhone::TMobilePhoneCBCondition aCondition );
						
	TBool SearchCBStatusListL( CMobilePhoneCBList* aList, 
								RMobilePhone::TMobilePhoneCBCondition aCondition, 
								RMobilePhone::TMobileService aService,
								RMobilePhone::TMobilePhoneCBStatus aStatus );
	
	
	
protected:

	TSupplementalTsyTestHelper iSupplementalTsyTestHelper;
	TCallControlTsyTestHelper iCallControlTsyTestHelper;
	TNetworkTsyTestHelper	iNetworkTsyTestHelper;
	
	

	}; // class CCTSYIntegrationTestCallBarringBase


class CCTSYIntegrationTestCallBarring0001 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0001


class CCTSYIntegrationTestCallBarring0002 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0002


class CCTSYIntegrationTestCallBarring0003 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0003


class CCTSYIntegrationTestCallBarring0004 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0004


class CCTSYIntegrationTestCallBarring0005 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0005


class CCTSYIntegrationTestCallBarring0006 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0006


class CCTSYIntegrationTestCallBarring0007 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0007


class CCTSYIntegrationTestCallBarring0008 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0008


class CCTSYIntegrationTestCallBarring0009 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	void TestActivateAndDeactivateCallBarringL( RMobilePhone &aPhone, const RMobilePhone::TMobilePassword& aPassword,
												RMobilePhone::TMobilePhoneCBCondition aCondition
								             );

	}; // class CCTSYIntegrationTestCallBarring0009


class CCTSYIntegrationTestCallBarring0010 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0010


class CCTSYIntegrationTestCallBarring0011 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0011


class CCTSYIntegrationTestCallBarring0012 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0012


class CCTSYIntegrationTestCallBarring0013 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0013


class CCTSYIntegrationTestCallBarring0014 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0014


class CCTSYIntegrationTestCallBarring0015 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0015


class CCTSYIntegrationTestCallBarring0016 : public CCTSYIntegrationTestCallBarringBase
	{
public:

	CCTSYIntegrationTestCallBarring0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallBarring0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallBarring0016



#endif // __CCTSYINTEGRATIONTESTCALLBARRING_H__
