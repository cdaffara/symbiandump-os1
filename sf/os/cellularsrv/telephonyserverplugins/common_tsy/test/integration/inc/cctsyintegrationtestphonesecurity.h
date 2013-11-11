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
// Test step declarations for the PhoneSecurity functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTPHONESECURITY_H__
#define __CCTSYINTEGRATIONTESTPHONESECURITY_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"

#include "tsimtsytesthelper.h"
#include "tphonetsytesthelper.h"
#include "tnetworktsytesthelper.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestPhoneSecurityBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestPhoneSecurityBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestPhoneSecurityBase();

protected:

	void GetNetworkSectionL(RMobilePhone& aMobilePhone,const TDesC*& aNetworkSection);
	void UnblockPinL(RMobilePhone& aMobilePhone,	RMobilePhone::TMobilePhoneSecurityCode aPhoneSecurityCode,
				TDesC& aPin, TDesC& aPuk);
	void ManglePassword(const TDesC& aOldPassword, TDes& aNewPassword);

//attributes section
protected:
	TSimTsyTestHelper		iSimTsyTestHelper;
	TNetworkTsyTestHelper   iNetworkTsyTestHelper;
	TPhoneTsyTestHelper 	iPhoneTsyTestHelper;

	
	}; // class CCTSYIntegrationTestPhoneSecurityBase


class CCTSYIntegrationTestPhoneSecurity0001 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0001


class CCTSYIntegrationTestPhoneSecurity0002 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0002


class CCTSYIntegrationTestPhoneSecurity0003 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0003


class CCTSYIntegrationTestPhoneSecurity0004 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0004


class CCTSYIntegrationTestPhoneSecurity0005 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0005


class CCTSYIntegrationTestPhoneSecurity0006 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0006


class CCTSYIntegrationTestPhoneSecurity0007 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0007


class CCTSYIntegrationTestPhoneSecurity0008 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0008


class CCTSYIntegrationTestPhoneSecurity0009 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0009


class CCTSYIntegrationTestPhoneSecurity0010 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0010


class CCTSYIntegrationTestPhoneSecurity0011 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0011


class CCTSYIntegrationTestPhoneSecurity0012 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0012


class CCTSYIntegrationTestPhoneSecurity0013 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0013


class CCTSYIntegrationTestPhoneSecurity0014 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0014


class CCTSYIntegrationTestPhoneSecurity0015 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0015


class CCTSYIntegrationTestPhoneSecurity0016 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0016


class CCTSYIntegrationTestPhoneSecurity0017 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0017


class CCTSYIntegrationTestPhoneSecurity0018 : public CCTSYIntegrationTestPhoneSecurityBase
	{
public:

	CCTSYIntegrationTestPhoneSecurity0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestPhoneSecurity0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestPhoneSecurity0018



#endif // __CCTSYINTEGRATIONTESTPHONESECURITY_H__
