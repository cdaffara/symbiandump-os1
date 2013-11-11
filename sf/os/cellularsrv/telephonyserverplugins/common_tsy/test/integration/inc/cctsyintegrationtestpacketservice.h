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
// Test step declarations for the PacketService functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPACKETSERVICE_H__
#define __CCTSYINTEGRATIONTESTPACKETSERVICE_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tpacketservicetsytesthelper.h"
#include "tnetworktsytesthelper.h"
#include "cctsyinidata.h"

class CEtelSessionMgr;

class CCTSYIntegrationTestPacketServiceBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPacketServiceBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPacketServiceBase();

protected:
	
	void EnsureAttachModeL(RPacketService &aPacketService, RPacketService::TAttachMode &aModeAttach);
	
	TPacketServiceTsyTestHelper iPacketServiceTestHelper;
	TNetworkTsyTestHelper iNetworkTestHelper;
	}; // class CCTSYIntegrationTestPacketServiceBase


class CCTSYIntegrationTestPacketService0001 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0001


class CCTSYIntegrationTestPacketService0002 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0002


class CCTSYIntegrationTestPacketService0003 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0003();
	virtual TVerdict doTestStepL();
	void GetGprsSettingsFromInfo(RMobilePhone::TMobilePhoneNetworkInfoV1 info, TPtrC& aAccessPoint, TPtrC& aUser, TPtrC& aPassword);
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0003


class CCTSYIntegrationTestPacketService0004 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0004


class CCTSYIntegrationTestPacketService0005 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0005


class CCTSYIntegrationTestPacketService0006 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0006


class CCTSYIntegrationTestPacketService0007 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0007


class CCTSYIntegrationTestPacketService0008 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0008


class CCTSYIntegrationTestPacketService0009 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0009


class CCTSYIntegrationTestPacketService0010 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0010


class CCTSYIntegrationTestPacketService0011 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0011


class CCTSYIntegrationTestPacketService0012 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0012


class CCTSYIntegrationTestPacketService0013 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0013


class CCTSYIntegrationTestPacketService0014 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0014


class CCTSYIntegrationTestPacketService0015 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0015


class CCTSYIntegrationTestPacketService0016 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0016


class CCTSYIntegrationTestPacketService0017 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0017


class CCTSYIntegrationTestPacketService0018 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0018


class CCTSYIntegrationTestPacketService0019 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0019


class CCTSYIntegrationTestPacketService0020 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0020


class CCTSYIntegrationTestPacketService0021 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0021


class CCTSYIntegrationTestPacketService0022 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0022


class CCTSYIntegrationTestPacketService0023 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0023


class CCTSYIntegrationTestPacketService0024 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0024(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0024();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0024


class CCTSYIntegrationTestPacketService0025 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0025(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0025();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0025


class CCTSYIntegrationTestPacketService0026 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0026(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0026();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0026


class CCTSYIntegrationTestPacketService0027 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0027(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0027();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0027


class CCTSYIntegrationTestPacketService0028 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0028(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0028();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0028


class CCTSYIntegrationTestPacketService0029 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0029(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0029();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0029


class CCTSYIntegrationTestPacketService0030 : public CCTSYIntegrationTestPacketServiceBase
	{
public:

	CCTSYIntegrationTestPacketService0030(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPacketService0030();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPacketService0030



#endif // __CCTSYINTEGRATIONTESTPACKETSERVICE_H__
