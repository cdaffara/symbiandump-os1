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
// Test step declarations for the BroadcastMessaging functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTBROADCASTMESSAGING_H__
#define __CCTSYINTEGRATIONTESTBROADCASTMESSAGING_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tbroadcastmessagingtsytesthelper.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestBroadcastMessagingBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestBroadcastMessagingBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestBroadcastMessagingBase();

protected:
	
	TBool AssertValidBroadcastMessage(const RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 &aBraodcastAttributes);
	TBroadcastMessagingTsyTestHelper iBroadcastMessagingTestHelper;
	TNetworkTsyTestHelper iNetworkTestHelper;
	}; // class CCTSYIntegrationTestBroadcastMessagingBase


class CCTSYIntegrationTestBroadcastMessaging0001 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0001


class CCTSYIntegrationTestBroadcastMessaging0002 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0002


class CCTSYIntegrationTestBroadcastMessaging0003 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0003


class CCTSYIntegrationTestBroadcastMessaging0004 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0004


class CCTSYIntegrationTestBroadcastMessaging0005 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0005


class CCTSYIntegrationTestBroadcastMessaging0006 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0006


class CCTSYIntegrationTestBroadcastMessaging0007 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0007


class CCTSYIntegrationTestBroadcastMessaging0008 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0008


class CCTSYIntegrationTestBroadcastMessaging0009 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0009


class CCTSYIntegrationTestBroadcastMessaging0010 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0010


class CCTSYIntegrationTestBroadcastMessaging0011 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0011


class CCTSYIntegrationTestBroadcastMessaging0012 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0012


class CCTSYIntegrationTestBroadcastMessaging0013 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0013


class CCTSYIntegrationTestBroadcastMessaging0014 : public CCTSYIntegrationTestBroadcastMessagingBase
	{
public:

	CCTSYIntegrationTestBroadcastMessaging0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestBroadcastMessaging0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestBroadcastMessaging0014

	
#endif // __CCTSYINTEGRATIONTESTBROADCASTMESSAGING_H__
