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
// Test step declarations for the DataCallControl functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTDATACALLCONTROL_H__
#define __CCTSYINTEGRATIONTESTDATACALLCONTROL_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tcallcontroltsytesthelper.h" 
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;
//class TCallControlTsyTestHelper;


class CCTSYIntegrationTestDataCallControlBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestDataCallControlBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestDataCallControlBase();

protected:

	TCallControlTsyTestHelper iCallControlTestHelper;
	TNetworkTsyTestHelper iNetworkTestHelper;
	}; // class CCTSYIntegrationTestDataCallControlBase


class CCTSYIntegrationTestDataCallControl0001 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDataCallControl0001


class CCTSYIntegrationTestDataCallControl0002 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDataCallControl0002


class CCTSYIntegrationTestDataCallControl0003 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDataCallControl0003


class CCTSYIntegrationTestDataCallControl0004 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDataCallControl0004


class CCTSYIntegrationTestDataCallControl0005 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDataCallControl0005


class CCTSYIntegrationTestDataCallControl0006 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

protected:

	void DoTest0006L(RMobileCall& aCall1, RMobileCall::TMobileCallDataSpeed aSpeed);

private:

	}; // class CCTSYIntegrationTestDataCallControl0006


class CCTSYIntegrationTestDataCallControl0007 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

protected:

	void DoTest0007L(RMobileLine& aDataLine);

private:

	}; // class CCTSYIntegrationTestDataCallControl0007


class CCTSYIntegrationTestDataCallControl0008 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDataCallControl0008


class CCTSYIntegrationTestDataCallControl0009 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDataCallControl0009


class CCTSYIntegrationTestDataCallControl0010 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDataCallControl0010


class CCTSYIntegrationTestDataCallControl0011 : public CCTSYIntegrationTestDataCallControlBase
	{
public:

	CCTSYIntegrationTestDataCallControl0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestDataCallControl0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestDataCallControl0011



#endif // __CCTSYINTEGRATIONTESTDATACALLCONTROL_H__
