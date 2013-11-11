
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
// TS_SimPhBk.h
// Header file: Basic  tests.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TS_SimPhBk_H__
#define TS_SimPhBk_H__

#include "Te_SimPhBkTestStepBase.h"


//------------------------------------------------------------------ 

/**
 * This class substitutes function testPhoneInfoL() from T_SimPhBk
 * 
*/
class CTestPhoneInfo : public CSimPhBkTestStepBase
	{
public:
	CTestPhoneInfo() ;
	~CTestPhoneInfo(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function testGetCapsAndInfoL() from T_SimPhBk
 * 
*/
class CTestGetCapsAndInfo : public CSimPhBkTestStepBase
	{
public:
	CTestGetCapsAndInfo() ;
	~CTestGetCapsAndInfo(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function testReadAndWriteL() from T_SimPhBk
 * 
*/
class CTestReadAndWrite : public CSimPhBkTestStepBase
	{
public:
	CTestReadAndWrite() ;
	~CTestReadAndWrite(){} ;
	virtual TVerdict doTestStepL( void );
private:	
	void WriteToMEPhBkL() ;
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function testDeleteL() from T_SimPhBk
 * 
*/
class CTestDelete : public CSimPhBkTestStepBase
	{
public:
	CTestDelete() ;
	~CTestDelete(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function testNotificationsL() from T_SimPhBk
 * 
*/
class CTestNotifications : public CSimPhBkTestStepBase
	{
public:
	CTestNotifications() ;
	~CTestNotifications(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function testInvalidOpsL() from T_SimPhBk
 * 
*/
class CTestInvalidOps : public CSimPhBkTestStepBase
	{
public:
	CTestInvalidOps() ;
	~CTestInvalidOps(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function testAdvancedPhBkL() from T_SimPhBk
 * 
*/
class CTestAdvancedPhBk : public CSimPhBkTestStepBase
	{
public:
	CTestAdvancedPhBk() ;
	~CTestAdvancedPhBk(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function RetrievalOfWholePhoneBookL() from T_SimPhBk
 * 
*/
class CRetrievalOfWholePhoneBook : public CSimPhBkTestStepBase
	{
public:
	CRetrievalOfWholePhoneBook() ;
	~CRetrievalOfWholePhoneBook(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function RetrievalOfBatchOfPhoneBookEntriesL() from T_SimPhBk
 * 
*/
class CRetrievalOfBatchOfPhoneBookEntries : public CSimPhBkTestStepBase
	{
public:
	CRetrievalOfBatchOfPhoneBookEntries() ;
	~CRetrievalOfBatchOfPhoneBookEntries(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function RetrievalOfHalfOfPhoneBookEntriesL() from T_SimPhBk
 * 
*/
class CRetrievalOfHalfOfPhoneBookEntries : public CSimPhBkTestStepBase
	{
public:
	CRetrievalOfHalfOfPhoneBookEntries() ;
	~CRetrievalOfHalfOfPhoneBookEntries(){} ;
	virtual TVerdict doTestStepL( void );
	
	} ;


//------------------------------------------------------------------ 

/**
 * This class substitutes function TestAllPhBooksL() from T_SimPhBk
 * 
*/
class CTestAllPhBooks : public CSimPhBkTestStepBase
	{
public:
	CTestAllPhBooks() ;
	~CTestAllPhBooks(){} ;
	virtual TVerdict doTestStepL( void );
private:
	TPtrC MakeCheckPointName(TInt aMajorNum, TInt aMinorNum ) ;
	void Print_TMobilePhoneBookInfoV1(RMobilePhoneBookStore::TMobilePhoneBookInfoV1& aArg) ;
	void Print_TMobilePhoneBookLocation(RMobilePhoneBookStore::TMobilePhoneBookLocation& aArg);
	void Print_TMobilePhoneBookCaps(TUint32 aArg) ;
	TInt GetPhoneBookInfoL(const TDesC& aPhoneBookName, TInt aMinorNum) ;
	TInt ClearPhoneBookL(const TDesC& aPhoneBookName, TInt aMinorNum) ;
	void TestReadAndWritePhBkL(const TDesC& aPhoneBookName, TInt aMinorNum );
private:	
	struct TPhBkNameAndID 
		{
		TName iPhBkName ;
		TInt iPhBkID ;
		} ;

	TPhBkNameAndID iPhBkNames[11] ;
	} ;



#endif// TS_SimPhBk_H__

