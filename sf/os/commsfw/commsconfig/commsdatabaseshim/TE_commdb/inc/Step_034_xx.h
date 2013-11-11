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
// This is the header file for CommDb test 034.XX group of scenarios 

#include<cdbpreftable.h>

#if (!defined __STEP_034_XX_H__)
#define __STEP_034_XX_H__


////////////////////
//	Test step 034.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest034_01) : public CTestStepCommDb
{
public:
	CCommDbTest034_01();
	~CCommDbTest034_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 034.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest034_02) : public CTestStepCommDb
{
public:
	CCommDbTest034_02();
	~CCommDbTest034_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 034.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest034_03) : public CTestStepCommDb
{
public:
	CCommDbTest034_03();
	~CCommDbTest034_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 034.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest034_04) : public CTestStepCommDb
{
public:
	CCommDbTest034_04();
	~CCommDbTest034_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 034.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest034_05) : public CTestStepCommDb
{
public:
	CCommDbTest034_05();
	~CCommDbTest034_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 034.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest034_06) : public CTestStepCommDb
{
public:
	CCommDbTest034_06();
	~CCommDbTest034_06();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_034_XX_H__
