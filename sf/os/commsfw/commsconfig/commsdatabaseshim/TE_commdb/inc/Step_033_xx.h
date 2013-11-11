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
// This is the header file for CommDb test 033.XX group of scenarios 

#include<cdbpreftable.h>

#if (!defined __STEP_033_XX_H__)
#define __STEP_033_XX_H__


////////////////////
//	Test step 033.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest033_01) : public CTestStepCommDb
{
public:
	CCommDbTest033_01();
	~CCommDbTest033_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 033.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest033_02) : public CTestStepCommDb
{
public:
	CCommDbTest033_02();
	~CCommDbTest033_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 033.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest033_03) : public CTestStepCommDb
{
public:
	CCommDbTest033_03();
	~CCommDbTest033_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 033.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest033_04) : public CTestStepCommDb
{
public:
	CCommDbTest033_04();
	~CCommDbTest033_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 033.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest033_05) : public CTestStepCommDb
{
public:
	CCommDbTest033_05();
	~CCommDbTest033_05();

	virtual enum TVerdict doTestStepL( void );
};


////////////////////
//	Test step 033.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest033_06) : public CTestStepCommDb
{
public:
	CCommDbTest033_06();
	~CCommDbTest033_06();

	virtual enum TVerdict doTestStepL( void );
};

////////////////////
//	Test step 033.07
////////////////////

NONSHARABLE_CLASS(CCommDbTest033_07) : public CTestStepCommDb
{
public:
	CCommDbTest033_07();
	~CCommDbTest033_07();

	virtual enum TVerdict doTestStepL( void );
};

////////////////////
//	Test step 033.08
////////////////////

NONSHARABLE_CLASS(CCommDbTest033_08) : public CTestStepCommDb
{
public:
	CCommDbTest033_08();
	~CCommDbTest033_08();

	virtual enum TVerdict doTestStepL( void );
};

////////////////////
//	Test step 033.09
////////////////////

NONSHARABLE_CLASS(CCommDbTest033_09) : public CTestStepCommDb
{
public:
	CCommDbTest033_09();

	virtual enum TVerdict doTestStepL();
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};


#endif //__STEP_033_XX_H__
