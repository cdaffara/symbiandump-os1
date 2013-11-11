/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
// This is the header file for CommDb test 028.XX group of scenarios 

#include<cdbpreftable.h>

#if (!defined __STEP_028_XX_H__)
#define __STEP_028_XX_H__


////////////////////
//	Test step 028.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest028_01) : public CTestStepCommDb
{
public:
	CCommDbTest028_01();
	~CCommDbTest028_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 028.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest028_02) : public CTestStepCommDb
{
public:
	CCommDbTest028_02();
	~CCommDbTest028_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 028.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest028_03) : public CTestStepCommDb
{
public:
	CCommDbTest028_03();
	~CCommDbTest028_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 028.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest028_04) : public CTestStepCommDb
{
public:
	CCommDbTest028_04();
	~CCommDbTest028_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 028.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest028_05) : public CTestStepCommDb
{
public:
	CCommDbTest028_05();
	~CCommDbTest028_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 028.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest028_06) : public CTestStepCommDb
{
public:
	CCommDbTest028_06();
	~CCommDbTest028_06();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 028.07
////////////////////

NONSHARABLE_CLASS(CCommDbTest028_07) : public CTestStepCommDb
{
public:
	CCommDbTest028_07();
	~CCommDbTest028_07();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_028_XX_H__
