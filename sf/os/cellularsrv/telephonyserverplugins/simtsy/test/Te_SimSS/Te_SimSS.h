
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file
*/

#ifndef TE_SimSS_H__
#define TE_SimSS_H__

#include "Te_SimSSTestStepBase.h"

#define CHECKPOINT(code,expected,msg){\
	TEST((code)==(expected));\
	if (TestStepResult() != EPass ) INFO_PRINTF2(_L(" : %s "),(msg));\
	}

const TUint KAddressType=129;


//------------------------------------------------------------------ 


//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallBarring()
 * 
*/
class CTestCallBarring : public CSimSSTestStepBase 
	{
public:
	CTestCallBarring() ;
	~CTestCallBarring(){} ;
	virtual TVerdict doTestStepL( void );	
friend class CTestGetCBList;
	};
	
	
	
class CTestGetCBList : public CActive
		{
	public:
		static CTestGetCBList* NewLC(RMobilePhone& aPhone, CTestCallBarring* aTest);
		~CTestGetCBList();
		void Start(RMobilePhone::TMobilePhoneCBCondition aCondition);
		CMobilePhoneCBList* RetrieveListL();
		TInt SetExpectedResultsL(RMobilePhone::TMobilePhoneCBInfoEntryV1 aExpectedEntry);
		void PrintCondition(RMobilePhone::TMobilePhoneCBCondition aCondition);
	protected:
		void PrintEntry(RMobilePhone::TMobilePhoneCBInfoEntryV1 aCbEntry, TInt aEntryNumber);
		TInt CheckResultsL();
		CTestGetCBList(RMobilePhone& aPhone, CTestCallBarring* aTest);
		void ConstructL();
		void RunL();
		void DoCancel();
	protected:
		CRetrieveMobilePhoneCBList* iRetrieveCB;
		RMobilePhone& iPhone;
		CMobilePhoneCBList* iExpectedcbList;
		TBool iChecked;
	public:
		CTestCallBarring* iTest;	
		};
		
/**
 * This class substitutes function testCallForwarding
 * 
*/
class CTestCallForwarding : public CSimSSTestStepBase 
	{
public:
	CTestCallForwarding() ;
	~CTestCallForwarding(){} ;
	virtual TVerdict doTestStepL( void );	
friend class CTestGetCFList;
	};
	
class CTestGetCFList : public CActive
		{
	public:
		static CTestGetCFList* NewLC(RMobilePhone& aPhone, CTestCallForwarding* aTest);
		~CTestGetCFList();
		void Start(RMobilePhone::TMobilePhoneCFCondition aCondition);
		void Start(RMobilePhone::TMobilePhoneCFCondition aCondition,
		           RMobilePhone::TMobileService aServiceGroup);												
		CMobilePhoneCFList* RetrieveListL();
		TInt SetExpectedResultsL(RMobilePhone::TMobilePhoneCFInfoEntryV1 aExpectedEntry);
		void PrintCondition(RMobilePhone::TMobilePhoneCFCondition aCondition);
	protected:
		void PrintEntry(RMobilePhone::TMobilePhoneCFInfoEntryV1 aCfEntry, TInt aEntryNumber);
		TInt CheckResultsL();
		CTestGetCFList(RMobilePhone& aPhone, CTestCallForwarding* aTest);
		void ConstructL();
		void RunL();
		void DoCancel();
	protected:
		CRetrieveMobilePhoneCFList* iRetrieveCF;
		RMobilePhone& iPhone;
		CMobilePhoneCFList* iExpectedCfList;
		TBool iChecked;
	public:
		CTestCallForwarding* iTest;	
		};
		
//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallWaiting()
 * 
*/
class CTestCallWaiting : public CSimSSTestStepBase 
	{
public:
	CTestCallWaiting() ;
	~CTestCallWaiting(){} ;
	virtual TVerdict doTestStepL( void );	
friend class CTestGetCWList;
	};
	
	
	
class CTestGetCWList : public CActive
		{
	public:
		static CTestGetCWList* NewLC(RMobilePhone& aPhone, CTestCallWaiting* aTest);
		~CTestGetCWList();
		void Start();
		CMobilePhoneCWList* RetrieveListL();
		TInt SetExpectedResultsL(RMobilePhone::TMobilePhoneCWInfoEntryV1 aExpectedEntry);
		//void PrintCondition(RMobilePhone::TMobilePhoneCBCondition aCondition);
	protected:
		void PrintEntry(RMobilePhone::TMobilePhoneCWInfoEntryV1 aCwEntry, TInt aEntryNumber);
		TInt CheckResultsL();
		CTestGetCWList(RMobilePhone& aPhone, CTestCallWaiting* aTest);
		void ConstructL();
		void RunL();
		void DoCancel();
	protected:
		CRetrieveMobilePhoneCWList* iRetrieveCW;
		RMobilePhone& iPhone;
		CMobilePhoneCWList* iExpectedcwList;
		TBool iChecked;
	public:
		CTestCallWaiting* iTest;	
		};

/**
 * This class substitutes function testCallForwarding
 * 
*/
class CTestIdentityService : public CSimSSTestStepBase 
	{
public:
	CTestIdentityService() ;
	~CTestIdentityService(){} ;
	virtual TVerdict doTestStepL( void );	
	void PrintIdServiceStatus(RMobilePhone::TMobilePhoneIdServiceStatus aIdServiceStat, RMobilePhone::TMobilePhoneIdService aIdService);

	};
	
		
//------------------------------------------------------------------ 



#endif// TE_SimSS_H__

