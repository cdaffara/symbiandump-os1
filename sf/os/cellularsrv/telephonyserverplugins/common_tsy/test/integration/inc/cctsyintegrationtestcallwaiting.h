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
// Test step declarations for the CallWaiting functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTCALLWAITING_H__
#define __CCTSYINTEGRATIONTESTCALLWAITING_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

#include "tsupplementaltsytesthelper.h"

#include "listretrievalhelpers.h"
#include "tcallcontroltsytesthelper.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestCallWaitingBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestCallWaitingBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestCallWaitingBase();

protected:
	//member variables
	TCallControlTsyTestHelper iCallControlTsyTestHelper;
	TSupplementalTsyTestHelper iSupplementalTsyTestHelper;
	TNetworkTsyTestHelper iNetworkTsyTestHelper;
	}; // class CCTSYIntegrationTestCallWaitingBase


class CCTSYIntegrationTestCallWaiting0001 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0001


class CCTSYIntegrationTestCallWaiting0002 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0002


class CCTSYIntegrationTestCallWaiting0003 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0003


class CCTSYIntegrationTestCallWaiting0004 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0004


class CCTSYIntegrationTestCallWaiting0005 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0005


class CCTSYIntegrationTestCallWaiting0006 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0006


class CCTSYIntegrationTestCallWaiting0007 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0007


class CCTSYIntegrationTestCallWaiting0008 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0008


class CCTSYIntegrationTestCallWaiting0009 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0009


class CCTSYIntegrationTestCallWaiting0010 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0010


class CCTSYIntegrationTestCallWaiting0011 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0011


class CCTSYIntegrationTestCallWaiting0012 : public CCTSYIntegrationTestCallWaitingBase
	{
public:

	CCTSYIntegrationTestCallWaiting0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestCallWaiting0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestCallWaiting0012



#endif // __CCTSYINTEGRATIONTESTCALLWAITING_H__
