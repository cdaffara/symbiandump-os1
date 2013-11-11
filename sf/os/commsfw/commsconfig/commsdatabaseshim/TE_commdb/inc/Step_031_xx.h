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
// This is the header file for CommDb test 031.XX group of scenarios 

#include<cdbpreftable.h>

#if (!defined __STEP_031_XX_H__)
#define __STEP_031_XX_H__


////////////////////
//	Test step 031.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest031_01) : public CTestStepCommDb
{
public:
	CCommDbTest031_01();
	~CCommDbTest031_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 031.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest031_02) : public CTestStepCommDb
{
public:
	CCommDbTest031_02();
	~CCommDbTest031_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 031.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest031_03) : public CTestStepCommDb
{
public:
	CCommDbTest031_03();
	~CCommDbTest031_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 031.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest031_04) : public CTestStepCommDb
{
public:
	CCommDbTest031_04();
	~CCommDbTest031_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 031.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest031_05) : public CTestStepCommDb
{
public:
	CCommDbTest031_05();
	~CCommDbTest031_05();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_031_XX_H__
