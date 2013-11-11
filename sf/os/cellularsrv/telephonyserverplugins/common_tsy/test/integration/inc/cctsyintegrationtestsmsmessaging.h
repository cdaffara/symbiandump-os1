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
// Test step declarations for the SmsMessaging functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTSMSMESSAGING_H__
#define __CCTSYINTEGRATIONTESTSMSMESSAGING_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tsmsmessagingtsytesthelper.h"
#include "tsmsstoretsytesthelper.h"
#include "tnetworktsytesthelper.h"
#include "tpacketservicetsytesthelper.h"
#include "tsimtsytesthelper.h"
#include "tcallcontroltsytesthelper.h" 

class CEtelSessionMgr;

_LIT(KServiceCentreSection,"SmsServiceCentre");
_LIT(KNetworkO2,"O2");
_LIT(KNetworkVodafone,"Vodafone");
_LIT(KNetworkOrange,"Orange");
_LIT(KNetworkTMobile,"TMobile");
_LIT(KNetworkNTN,"NTN");


class CCTSYIntegrationTestSmsMessagingBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestSmsMessagingBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestSmsMessagingBase();

protected:

	void GetSmsServiceCentreAddressL(TPtrC &aServiceCentreAddress);
	void SetSmsBearerL(RMobileSmsMessaging &aSmsMessaging, RMobileSmsMessaging::TMobileSmsBearer aBearer);
	void GetSmsSendAttributesL(RMobileSmsMessaging::TMobileSmsSendAttributesV1 &aSendSmsAttributes,TBool aCallToSelf=EFalse);
	RMobileSmsMessaging::TMobileSmsGsmTpdu CreateValidSmsMessageL(TDes8 &aText, 
			RMobileSmsMessaging::TMobileSmsSendAttributesV1& aSendSmsAttributes, 
			TSmsStoreTsyTestHelper::TDataCodingScheme aDataCodingScheme = TSmsStoreTsyTestHelper::ENoDataCodingScheme, 
			TBool aIsToSelf = EFalse,
			TBool aStatusReportRequest = EFalse);
	TBool ComparePDUs(RMobileSmsMessaging::TMobileSmsGsmTpdu &aRecieved, RMobileSmsMessaging::TMobileSmsGsmTpdu &aSent);

protected:
	TSmsMessagingTsyTestHelper iSmsMessagingTsyTestHelper;
	TSmsStoreTsyTestHelper iSmsStoreTsyTestHelper;
	TNetworkTsyTestHelper iNetworkTsyTestHelper;
	TPacketServiceTsyTestHelper iPacketServiceTsyTestHelper;
	TSimTsyTestHelper iSimTsyTestHelper;
	TCallControlTsyTestHelper iCallControlTestHelper;
	
	}; // class CCTSYIntegrationTestSmsMessagingBase


class CCTSYIntegrationTestSmsMessaging0001 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0001


class CCTSYIntegrationTestSmsMessaging0002 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0002


class CCTSYIntegrationTestSmsMessaging0003 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0003


class CCTSYIntegrationTestSmsMessaging0004 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0004


class CCTSYIntegrationTestSmsMessaging0005 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0005


class CCTSYIntegrationTestSmsMessaging0006 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0006


class CCTSYIntegrationTestSmsMessaging0007 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0007


class CCTSYIntegrationTestSmsMessaging0008 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0008


class CCTSYIntegrationTestSmsMessaging0009 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0009


class CCTSYIntegrationTestSmsMessaging0010 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0010


class CCTSYIntegrationTestSmsMessaging0011 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0011


class CCTSYIntegrationTestSmsMessaging0012 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0012


class CCTSYIntegrationTestSmsMessaging0013 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0013


class CCTSYIntegrationTestSmsMessaging0014 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0014


class CCTSYIntegrationTestSmsMessaging0015 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0015


class CCTSYIntegrationTestSmsMessaging0016 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0016


class CCTSYIntegrationTestSmsMessaging0017 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0017


class CCTSYIntegrationTestSmsMessaging0018 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0018


class CCTSYIntegrationTestSmsMessaging0019 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0019


class CCTSYIntegrationTestSmsMessaging0020 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0020


class CCTSYIntegrationTestSmsMessaging0021 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0021


class CCTSYIntegrationTestSmsMessaging0022 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0022


class CCTSYIntegrationTestSmsMessaging0023 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0023


class CCTSYIntegrationTestSmsMessaging0024 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0024(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0024();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0024


class CCTSYIntegrationTestSmsMessaging0025 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0025(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0025();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0025


class CCTSYIntegrationTestSmsMessaging0026 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0026(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0026();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0026


class CCTSYIntegrationTestSmsMessaging0027 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0027(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0027();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0027


class CCTSYIntegrationTestSmsMessaging0028 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0028(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0028();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0028


class CCTSYIntegrationTestSmsMessaging0029 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0029(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0029();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0029


class CCTSYIntegrationTestSmsMessaging0030 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0030(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0030();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0030


class CCTSYIntegrationTestSmsMessaging0031 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0031(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0031();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0031


class CCTSYIntegrationTestSmsMessaging0032 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0032(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0032();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0032


class CCTSYIntegrationTestSmsMessaging0033 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0033(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0033();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0033


class CCTSYIntegrationTestSmsMessaging0034 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0034(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0034();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0034


class CCTSYIntegrationTestSmsMessaging0035 : public CCTSYIntegrationTestSmsMessagingBase
	{
public:

	CCTSYIntegrationTestSmsMessaging0035(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSmsMessaging0035();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSmsMessaging0035



#endif // __CCTSYINTEGRATIONTESTSMSMESSAGING_H__
