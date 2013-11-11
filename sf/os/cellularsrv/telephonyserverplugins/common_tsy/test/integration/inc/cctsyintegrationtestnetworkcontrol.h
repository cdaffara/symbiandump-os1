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
// Test step declarations for the NetworkControl functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTNETWORKCONTROL_H__
#define __CCTSYINTEGRATIONTESTNETWORKCONTROL_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tphonetsytesthelper.h"
#include "tnetworktsytesthelper.h"
#include "tcallcontroltsytesthelper.h" 
#include "tpacketservicetsytesthelper.h"


class CEtelSessionMgr;


class CCTSYIntegrationTestNetworkControlBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestNetworkControlBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestNetworkControlBase();

protected:		

	TBool IsPreferredNetworkInListL( CMobilePhoneStoredNetworkList& aList, 
								RMobilePhone::TMobilePreferredNetworkEntryV3& aTarget);
	void SetAttachModeToWhenPossibleAndWaitForAttachL(RPacketService& aPacketService);
	
protected:

	TNetworkTsyTestHelper iNetworkTestHelper;
	TCallControlTsyTestHelper iCallControlTestHelper;
	TPacketServiceTsyTestHelper iPacketServiceTestHelper;
		
	}; // class CCTSYIntegrationTestNetworkControlBase


class CCTSYIntegrationTestNetworkControl0001 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0001


class CCTSYIntegrationTestNetworkControl0002 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0002


class CCTSYIntegrationTestNetworkControl0003 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0003


class CCTSYIntegrationTestNetworkControl0004 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0004


class CCTSYIntegrationTestNetworkControl0005 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0005


class CCTSYIntegrationTestNetworkControl0006 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0006


class CCTSYIntegrationTestNetworkControl0007 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0007


class CCTSYIntegrationTestNetworkControl0008 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0008


class CCTSYIntegrationTestNetworkControl0009 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0009


class CCTSYIntegrationTestNetworkControl0010 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0010


class CCTSYIntegrationTestNetworkControl0011 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0011


class CCTSYIntegrationTestNetworkControl0012 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0012


class CCTSYIntegrationTestNetworkControl0013 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0013


class CCTSYIntegrationTestNetworkControl0014 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0014


class CCTSYIntegrationTestNetworkControl0015 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0015


class CCTSYIntegrationTestNetworkControl0016 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0016


class CCTSYIntegrationTestNetworkControl0017 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0017


class CCTSYIntegrationTestNetworkControl0018 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0018


class CCTSYIntegrationTestNetworkControl0019 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0019


class CCTSYIntegrationTestNetworkControl0020 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0020


class CCTSYIntegrationTestNetworkControl0021 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0021


class CCTSYIntegrationTestNetworkControl0022 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0022


class CCTSYIntegrationTestNetworkControl0023 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0023


class CCTSYIntegrationTestNetworkControl0024 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0024(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0024();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0024


class CCTSYIntegrationTestNetworkControl0025 : public CCTSYIntegrationTestNetworkControlBase
	{
public:

	CCTSYIntegrationTestNetworkControl0025(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestNetworkControl0025();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestNetworkControl0025



#endif // __CCTSYINTEGRATIONTESTNETWORKCONTROL_H__
