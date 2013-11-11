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
// Test step declarations for the SMSPStore functional unit.
// 
//

/**
 @internalTechnology
*/


#ifndef __CCTSYINTEGRATIONTESTSMSPSTORE_H__
#define __CCTSYINTEGRATIONTESTSMSPSTORE_H__


#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tsmsmessagingtsytesthelper.h"
#include "ttsytesthelperbase.h"

class CEtelSessionMgr;


class CCTSYIntegrationTestSMSPStoreBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestSMSPStoreBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestSMSPStoreBase();

protected:

	// Declare a TSY test helper here if required
	TSmsMessagingTsyTestHelper iSmsMessagingTestHelper;
	TBool CompareEntries(const RMobileSmsMessaging::TMobileSmspEntryV1 &aEntry1,
		const RMobileSmsMessaging::TMobileSmspEntryV1 &aEntry2);
//	TInt GetListEntriesL(RMobileSmsMessaging& aSmsMessaging, RArray<RMobileSmsMessaging::TMobileSmspEntryV1> &aEntryArray);
//	void RestoreSMSPListL(RMobileSmsMessaging& aSmsMessagingStore,TInt &aListCountAtStart, RArray<RMobileSmsMessaging::TMobileSmspEntryV1> &aEntryArray);
	}; // class CCTSYIntegrationTestSMSPStoreBase


class CCTSYIntegrationTestSMSPStore0001 : public CCTSYIntegrationTestSMSPStoreBase
	{
public:

	CCTSYIntegrationTestSMSPStore0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSPStore0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSPStore0001


class CCTSYIntegrationTestSMSPStore0002 : public CCTSYIntegrationTestSMSPStoreBase
	{
public:

	CCTSYIntegrationTestSMSPStore0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSPStore0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSPStore0002


class CCTSYIntegrationTestSMSPStore0003 : public CCTSYIntegrationTestSMSPStoreBase
	{
public:

	CCTSYIntegrationTestSMSPStore0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSPStore0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSPStore0003


class CCTSYIntegrationTestSMSPStore0004 : public CCTSYIntegrationTestSMSPStoreBase
	{
public:

	CCTSYIntegrationTestSMSPStore0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSPStore0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSPStore0004


class CCTSYIntegrationTestSMSPStore0005 : public CCTSYIntegrationTestSMSPStoreBase
	{
public:

	CCTSYIntegrationTestSMSPStore0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSPStore0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSPStore0005


class CCTSYIntegrationTestSMSPStore0006 : public CCTSYIntegrationTestSMSPStoreBase
	{
public:

	CCTSYIntegrationTestSMSPStore0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSPStore0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSPStore0006


class CCTSYIntegrationTestSMSPStore0007 : public CCTSYIntegrationTestSMSPStoreBase
	{
public:

	CCTSYIntegrationTestSMSPStore0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestSMSPStore0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestSMSPStore0007
class TBackupList: public TTsyTestHelperBase
	{
public:
	TBackupList(CCTSYIntegrationTestSuiteStepBase &aTestStep,
			RMobileSmsMessaging &aSmspStore);
	TBool BackupL(TInt &aListCount);
	operator TCleanupItem();

private:
	TBool iValid;
	TInt iListCount;
	RMobileSmsMessaging &iSmsMessaging;
	RArray<RMobileSmsMessaging::TMobileSmspEntryV1> iEntryArray;	
	static void DoCleanupL(TAny* aPtr);
	void CleanupL();
	};


#endif // __CCTSYINTEGRATIONTESTSMSPSTORE_H__
