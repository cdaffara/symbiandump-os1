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
// Test step declarations for the UssdMessaging functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTUSSDMESSAGING_H__
#define __CCTSYINTEGRATIONTESTUSSDMESSAGING_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tsmsstoretsytesthelper.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestUssdMessagingBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestUssdMessagingBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestUssdMessagingBase();

protected:

	// Simulate network sends a release complete
	TInt SimulateNetworkSendsReleaseCompleteL();
	// Simulate network sends a USSD notify message
	TInt SimulateNetworkSendsNotifyMessageL(RMobileUssdMessaging::TMobileUssdMessageType aType, TInt aLength=0);
	// SendMessage, Simulate complete, CheckRelease
	TInt SendSimulateCompleteCheckCompleteL(RMobileUssdMessaging &aUssdMessaging, TDes8& aMsg,RMobileUssdMessaging::TMobileUssdMessageType aMessageType);
	// SendMessage, Simulate notify, CheckRelease
	TInt SendSimulateNotifyCheckCompleteL(RMobileUssdMessaging &aUssdMessaging, TDes8& aMsg,RMobileUssdMessaging::TMobileUssdMessageType aMessageType,RMobileUssdMessaging::TMobileUssdMessageType aSimulateType, TInt aLength=0);
	// Simulate Notify, check receive
	TInt SimulateNotifyCheckReceiveL(RMobileUssdMessaging &aUssdMessaging,RMobileUssdMessaging::TMobileUssdMessageType aSimulateType, TInt aLength=0);

	TSmsStoreTsyTestHelper iSmsStoreTsyTestHelper;
	TNetworkTsyTestHelper iNetworkTsyTestHelper;

	}; // class CCTSYIntegrationTestUssdMessagingBase


class CCTSYIntegrationTestUssdMessaging0001 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0001


class CCTSYIntegrationTestUssdMessaging0002 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0002


class CCTSYIntegrationTestUssdMessaging0003 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0003


class CCTSYIntegrationTestUssdMessaging0004 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0004


class CCTSYIntegrationTestUssdMessaging0005 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0005


class CCTSYIntegrationTestUssdMessaging0006 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0006


class CCTSYIntegrationTestUssdMessaging0007 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0007


class CCTSYIntegrationTestUssdMessaging0008 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0008


class CCTSYIntegrationTestUssdMessaging0009 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0009


class CCTSYIntegrationTestUssdMessaging0010 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0010


class CCTSYIntegrationTestUssdMessaging0011 : public CCTSYIntegrationTestUssdMessagingBase
	{
public:

	CCTSYIntegrationTestUssdMessaging0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestUssdMessaging0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestUssdMessaging0011



#endif // __CCTSYINTEGRATIONTESTUSSDMESSAGING_H__
