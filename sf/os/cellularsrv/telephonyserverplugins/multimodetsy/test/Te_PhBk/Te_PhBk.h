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
 @internalComponent 
*/

#ifndef TE_PhBk_H__
#define TE_PhBk_H__

#include "Te_PhBkTestStepBase.h"

//------------------------------------------------------------------ 

//Test Checking Macros

#define CHECKPOINT(code,expected,msg){\
	TEST((code)==(expected));\
	if (code != expected ) INFO_PRINTF1(msg);\
	}

#define CHECKPOINT_EXPR(boolexpr,msg) {\
	TEST((boolexpr));\
	if (EFalse == (boolexpr)) INFO_PRINTF1(msg);\
	}

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestPhoneInfoL() from T_PhBk
 * 
*/
class CTestPhoneInfo : public CPhBkTestStep
	{
public:
	CTestPhoneInfo() ;
	virtual TVerdict doTestStepL( void );
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestGetCapsAndInfoL() from T_PhBk
 * 
*/
class CTestGetCapsAndInfo : public CPhBkTestStep
	{
public:
	CTestGetCapsAndInfo() ;
	virtual TVerdict doTestStepL( void );
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestReadAndWriteL() from T_PhBk
 * 
*/
class CTestReadAndWrite : public CPhBkTestStep
	{
public:
	CTestReadAndWrite() ;
	virtual TVerdict doTestStepL( void );
private:
	void WriteToMEPhBkL() ;
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestDeleteL() from T_PhBk
 * 
*/
class CTestDelete : public CPhBkTestStep
	{
public:
	CTestDelete() ;
	virtual TVerdict doTestStepL( void );
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestNotificationsL() from T_PhBk
 * 
*/
class CTestNotifications : public CPhBkTestStep
	{
public:
	CTestNotifications() ;
	virtual TVerdict doTestStepL( void );
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestInvalidOpsL() from T_PhBk
 * 
*/
class CTestInvalidOps : public CPhBkTestStep
	{
public:
	CTestInvalidOps() ;
	virtual TVerdict doTestStepL( void );
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestAdvancedPhBkL() from T_PhBk
 * 
*/
class CTestAdvancedPhBk : public CPhBkTestStep
	{
public:
	CTestAdvancedPhBk() ;
	virtual TVerdict doTestStepL( void );
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function RetrievalOfWholePhoneBookL() from T_PhBk
 * 
*/
class CRetrievalOfWholePhoneBook : public CPhBkTestStep
	{
public:
	CRetrievalOfWholePhoneBook() ;
	virtual TVerdict doTestStepL( void );
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function RetrievalOfBatchOfPhoneBookEntriesL() from T_PhBk
 * 
*/
class CRetrievalOfBatchOfPhoneBookEntries : public CPhBkTestStep
	{
public:
	CRetrievalOfBatchOfPhoneBookEntries() ;
	virtual TVerdict doTestStepL( void );
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function RetrievalOfHalfOfPhoneBookEntriesL() from T_PhBk
 * 
*/
class CRetrievalOfHalfOfPhoneBookEntries : public CPhBkTestStep
	{
public:
	CRetrievalOfHalfOfPhoneBookEntries() ;
	virtual TVerdict doTestStepL( void );
	} ;

#endif // TE_PhBk_H__

