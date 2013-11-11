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
// This is the header file for CommDb test 043.XX group of scenarios 

#include<cdbover.h>

#if (!defined __STEP_043_XX_H__)
#define __STEP_043_XX_H__


////////////////////
//	Test step 043.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest043_01) : public CTestStepCommDb
{
public:
	CCommDbTest043_01();
	~CCommDbTest043_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 043.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest043_02) : public CTestStepCommDb
{
public:
	CCommDbTest043_02();
	~CCommDbTest043_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 043.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest043_03) : public CTestStepCommDb
{
public:
	CCommDbTest043_03();
	~CCommDbTest043_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 043.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest043_04) : public CTestStepCommDb
{
public:
	CCommDbTest043_04();
	~CCommDbTest043_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 043.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest043_05) : public CTestStepCommDb
{
public:
	CCommDbTest043_05();
	~CCommDbTest043_05();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_043_XX_H__
