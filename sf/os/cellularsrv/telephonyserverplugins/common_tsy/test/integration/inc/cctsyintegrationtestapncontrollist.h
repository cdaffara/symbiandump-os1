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
// Test step declarations for the APNControlList functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTAPNCONTROLLIST_H__
#define __CCTSYINTEGRATIONTESTAPNCONTROLLIST_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tphonetsytesthelper.h"
#include "tnetworktsytesthelper.h"
#include "tsimtsytesthelper.h"
#include "tpacketservicetsytesthelper.h"
#include "ttsytesthelperbase.h"

class CEtelSessionMgr;

class CCTSYIntegrationTestAPNControlListBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestAPNControlListBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestAPNControlListBase();

protected:
	
	void EnsureApnListIsEmptyL(RMobilePhone &aMobilePhone);
	TBool CheckEntryIsInListL(RMobilePhone &aMobilePhone,RMobilePhone::TAPNEntryV3 &aExpectedAPnEntry, TUint32 &aIndex);
	
	// Declare a TSY test helper here if required
	TPhoneTsyTestHelper iPhoneTestHelper;
	TNetworkTsyTestHelper iNetworkTestHelper;
	TSimTsyTestHelper iSimTestHelper;
	TPacketServiceTsyTestHelper iPacketServiceHelper;
	
	}; // class CCTSYIntegrationTestAPNControlListBase


class CCTSYIntegrationTestAPNControlList0001 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0001


class CCTSYIntegrationTestAPNControlList0002 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0002


class CCTSYIntegrationTestAPNControlList0003 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0003


class CCTSYIntegrationTestAPNControlList0004 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0004


class CCTSYIntegrationTestAPNControlList0005 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0005


class CCTSYIntegrationTestAPNControlList0006 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0006


class CCTSYIntegrationTestAPNControlList0007 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0007


class CCTSYIntegrationTestAPNControlList0008 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0008


class CCTSYIntegrationTestAPNControlList0009 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0009


class CCTSYIntegrationTestAPNControlList0010 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0010


class CCTSYIntegrationTestAPNControlList0011 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0011


class CCTSYIntegrationTestAPNControlList0012 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0012


class CCTSYIntegrationTestAPNControlList0013 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0013


class CCTSYIntegrationTestAPNControlList0014 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0014


class CCTSYIntegrationTestAPNControlList0015 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0015


class CCTSYIntegrationTestAPNControlList0016 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0016


class CCTSYIntegrationTestAPNControlList0017 : public CCTSYIntegrationTestAPNControlListBase
	{
public:

	CCTSYIntegrationTestAPNControlList0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestAPNControlList0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestAPNControlList0017
class TBackupAPNList: public TTsyTestHelperBase
	{
public:
	TBackupAPNList(CCTSYIntegrationTestSuiteStepBase &aTestStep,
			RMobilePhone &aMobilePhone);
	TBool BackupL(TInt &aListCount);
	operator TCleanupItem();

private:
	TBool iValid;
	TInt iListCount;
	RMobilePhone &iMobilePhone;
	RArray<RMobilePhone::TAPNEntryV3> iEntryArray;
	static void DoCleanupL(TAny* aPtr);
	void CleanupL();
	};


#endif // __CCTSYINTEGRATIONTESTAPNCONTROLLIST_H__
