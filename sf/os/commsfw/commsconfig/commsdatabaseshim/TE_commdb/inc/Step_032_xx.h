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
// This is the header file for CommDb test 032.XX group of scenarios 

#include<cdbpreftable.h>

#if (!defined __STEP_032_XX_H__)
#define __STEP_032_XX_H__


////////////////////
//	Test step 032.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest032_01) : public CTestStepCommDb
{
public:
	CCommDbTest032_01();
	~CCommDbTest032_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 032.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest032_02) : public CTestStepCommDb
{
public:
	CCommDbTest032_02();
	~CCommDbTest032_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 032.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest032_03) : public CTestStepCommDb
{
public:
	CCommDbTest032_03();
	~CCommDbTest032_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 032.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest032_04) : public CTestStepCommDb
{
public:
	CCommDbTest032_04();
	~CCommDbTest032_04();

	virtual enum TVerdict doTestStepL( void );
};


////////////////////
//	Test step 032.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest032_05) : public CTestStepCommDb
{
public:
	CCommDbTest032_05();
	~CCommDbTest032_05();

	virtual enum TVerdict doTestStepL( void );
};


#endif //__STEP_032_XX_H__
