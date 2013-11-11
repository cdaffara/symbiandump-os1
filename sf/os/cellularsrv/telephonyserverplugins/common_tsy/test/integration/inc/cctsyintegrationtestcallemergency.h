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
// Test step declarations for the CallEmergency functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTCALLEMERGENCY_H__
#define __CCTSYINTEGRATIONTESTCALLEMERGENCY_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tphonetsytesthelper.h"
#include "tnetworktsytesthelper.h"
#include "tcallcontroltsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestCallEmergencyBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestCallEmergencyBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestCallEmergencyBase();

protected:

    void MakeEmergencyCallL(RMobileCall& aCall, const TDesC& aNumber);
    
//attributes section
protected:
	TNetworkTsyTestHelper   iNetworkTestHelper;
	TPhoneTsyTestHelper 	iPhoneTestHelper;
	TCallControlTsyTestHelper iCallControlTestHelper;

	}; // class CCTSYIntegrationTestCallEmergencyBase


class CCTSYIntegrationTestCallEmergency0001 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

protected:

	void DoTest0001L(RMobilePhone &aPhone, RMobileCall& aCall, const TDesC& aNumber, RMobilePhone::TMobilePassword& aPIN2Password);

private:

	}; // class CCTSYIntegrationTestCallEmergency0001


class CCTSYIntegrationTestCallEmergency0002 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0002


class CCTSYIntegrationTestCallEmergency0003 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0003


class CCTSYIntegrationTestCallEmergency0004 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0004


class CCTSYIntegrationTestCallEmergency0005 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0005


class CCTSYIntegrationTestCallEmergency0006 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0006


class CCTSYIntegrationTestCallEmergency0007 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0007


class CCTSYIntegrationTestCallEmergency0008 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0008


class CCTSYIntegrationTestCallEmergency0009 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0009


class CCTSYIntegrationTestCallEmergency0010 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0010


class CCTSYIntegrationTestCallEmergency0011 : public CCTSYIntegrationTestCallEmergencyBase
	{
public:

	CCTSYIntegrationTestCallEmergency0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallEmergency0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallEmergency0011



#endif // __CCTSYINTEGRATIONTESTCALLEMERGENCY_H__
