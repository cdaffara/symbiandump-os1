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
// Test step declarations for the ONStore functional unit.
// 
//

/**
 @internalTechnology
*/

#ifndef __CCTSYINTEGRATIONTESTONSTORE_H__
#define __CCTSYINTEGRATIONTESTONSTORE_H__

#include <test/testexecutestepbase.h>
#include "cctsyintegrationtestsuitebase.h"
#include "tsimtsytesthelper.h"
#include "tphonetsytesthelper.h"
#include "listretrievalhelpers.h"

class CEtelSessionMgr;

class CCTSYIntegrationTestONStoreBase : public CCTSYIntegrationTestSuiteStepBase
	{
public:

	CCTSYIntegrationTestONStoreBase(CEtelSessionMgr& aEtelSessionMgr);
	virtual ~CCTSYIntegrationTestONStoreBase();

protected:


	TBool CompareEntries(const RMobileONStore::TMobileONEntryV1 &aEntry1,
			const RMobileONStore::TMobileONEntryV1 &aEntry2);
	TBool GetUsedEntriesL(RMobileONStore& aOnStore,RMobilePhone& aPhone,TInt& aUsedEntries);
	TBool GetUsedandTotalEntriesL(RMobileONStore& aOnStore,RMobilePhone& aPhone,
			TInt& aUsedEntries,TInt& aTotalEntries);
	TInt GetNumberOfUsedAndTotalEntriesInStoreL(	
			RMobileONStore& aOnStore,
			RMobilePhone& aPhone,
			TInt& aUsedEntries, 
			TInt& aUsedEntries2,
			TInt& aTotalEntries, 
			TInt& aTotalEntries2);
    TBool OpenADNStoreL();
    
	// Declare a TSY test helper here if required
	TSimTsyTestHelper iSimTestHelper;
	TPhoneTsyTestHelper iPhoneTsyTestHelper;
	}; // class CCTSYIntegrationTestONStoreBase


class CCTSYIntegrationTestONStore0001 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0001(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0001();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0001


class CCTSYIntegrationTestONStore0002 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0002(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0002();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0002


class CCTSYIntegrationTestONStore0003 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0003(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0003();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0003


class CCTSYIntegrationTestONStore0004 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0004(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0004();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0004


class CCTSYIntegrationTestONStore0005 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0005(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0005();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0005


class CCTSYIntegrationTestONStore0006 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0006(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0006();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0006


class CCTSYIntegrationTestONStore0007 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0007(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0007();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:
	void WriteEntryAndCheckThatUsedEntriesDidNotChangedL(
			RMobileONStore& aStore, TInt aIndex, const TDesC& aNumber);
	}; // class CCTSYIntegrationTestONStore0007


class CCTSYIntegrationTestONStore0008 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0008(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0008();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0008


class CCTSYIntegrationTestONStore0009 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0009(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0009();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0009


class CCTSYIntegrationTestONStore0010 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0010(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0010();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0010


class CCTSYIntegrationTestONStore0011 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0011(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0011();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0011


class CCTSYIntegrationTestONStore0012 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0012(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0012();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0012


class CCTSYIntegrationTestONStore0013 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0013(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0013();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0013


class CCTSYIntegrationTestONStore0014 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0014(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0014();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0014


class CCTSYIntegrationTestONStore0015 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0015(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0015();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0015


class CCTSYIntegrationTestONStore0016 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0016(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0016();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0016


class CCTSYIntegrationTestONStore0017 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0017(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0017();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0017


class CCTSYIntegrationTestONStore0018 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0018(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0018();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0018


class CCTSYIntegrationTestONStore0019 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0019(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0019();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0019


class CCTSYIntegrationTestONStore0020 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0020(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0020();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0020


class CCTSYIntegrationTestONStore0021 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0021(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0021();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0021


class CCTSYIntegrationTestONStore0022 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0022(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0022();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0022


class CCTSYIntegrationTestONStore0023 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0023(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0023();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0023


class CCTSYIntegrationTestONStore0024 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0024(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0024();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0024


class CCTSYIntegrationTestONStore0025 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0025(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0025();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0025


class CCTSYIntegrationTestONStore0026 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0026(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0026();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0026


class CCTSYIntegrationTestONStore0027 : public CCTSYIntegrationTestONStoreBase
	{
public:

	CCTSYIntegrationTestONStore0027(CEtelSessionMgr& aEtelSessionMgr);
	~CCTSYIntegrationTestONStore0027();
	virtual TVerdict doTestStepL();
	static TPtrC GetTestStepName();

private:

	}; // class CCTSYIntegrationTestONStore0027

class TBackupAllEntries
	{
public:
	TBackupAllEntries(CCTSYIntegrationTestONStoreBase &aBase,
                      RMobileONStore &aOnStore);
	TBool BackupL();
	operator TCleanupItem();

private:
	TBool iValid;
	RMobileONStore &iOnStore;
	CCTSYIntegrationTestONStoreBase &iBase;
    CRetrieveMobilePhoneONListExec *iHelper;
	CMobilePhoneONList *iList;

	static void DoCleanupL(TAny* aPtr);
	void CleanupL();
	};

#endif // __CCTSYINTEGRATIONTESTONSTORE_H__

