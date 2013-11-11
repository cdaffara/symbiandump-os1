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
// Test step declarations for the SMSStore functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTSMSSTORE_H__
#define __CCTSYINTEGRATIONTESTSMSSTORE_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tsmsstoretsytesthelper.h"
#include "tsimtsytesthelper.h"

class CEtelSessionMgr;

_LIT(KNumber, "12345");
_LIT(KNumberInternational, "+12345"); // International version of KNumber
_LIT(KNumber2, "54321");
_LIT(KNumber3, "678912345");

class CCTSYIntegrationTestSMSStoreBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestSMSStoreBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestSMSStoreBase();

protected:
	void CreateGsmSmsEntry(	RMobileSmsStore::TMobileGsmSmsEntryV1& aEntry, 
							TInt aIndex,
							const TDesC8& aMsgData,
							RMobileSmsStore::TMobileSmsStoreStatus aStoreStatus,
							RMobilePhone::TMobileTON aServiceCentreTON = RMobilePhone::ENationalNumber,
							RMobilePhone::TMobileNPI aServiceCentreNumberingPlan = RMobilePhone::EIsdnNumberPlan,
							const TDesC& aServiceCentreNum = KNumber());
	
	void WaitForPhoneBookCachingToFinish();
	
	TSmsStoreTsyTestHelper iSmsStoreTestHelper;

	TSimTsyTestHelper iSimTsyTestHelper;
	
	}; // class CCTSYIntegrationTestSMSStoreBase


class CCTSYIntegrationTestSMSStore0001 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0001


class CCTSYIntegrationTestSMSStore0002 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0002


class CCTSYIntegrationTestSMSStore0003 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0003


class CCTSYIntegrationTestSMSStore0004 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0004


class CCTSYIntegrationTestSMSStore0005 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0005


class CCTSYIntegrationTestSMSStore0006 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0006


class CCTSYIntegrationTestSMSStore0007 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0007


class CCTSYIntegrationTestSMSStore0008 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0008


class CCTSYIntegrationTestSMSStore0009 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0009


class CCTSYIntegrationTestSMSStore0010 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0010


class CCTSYIntegrationTestSMSStore0011 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0011


class CCTSYIntegrationTestSMSStore0012 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0012


class CCTSYIntegrationTestSMSStore0013 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0013


class CCTSYIntegrationTestSMSStore0014 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0014


class CCTSYIntegrationTestSMSStore0015 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0015


class CCTSYIntegrationTestSMSStore0016 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0016


class CCTSYIntegrationTestSMSStore0017 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0017


class CCTSYIntegrationTestSMSStore0018 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0018


class CCTSYIntegrationTestSMSStore0019 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0019


class CCTSYIntegrationTestSMSStore0020 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0020


class CCTSYIntegrationTestSMSStore0021 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0021


class CCTSYIntegrationTestSMSStore0022 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0022


class CCTSYIntegrationTestSMSStore0023 : public CCTSYIntegrationTestSMSStoreBase
	{
public:

	CCTSYIntegrationTestSMSStore0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSStore0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSStore0023



#endif // __CCTSYINTEGRATIONTESTSMSSTORE_H__
