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
// This is the header file for CommDb test 029.XX group of scenarios 

#include<cdbpreftable.h>

#if (!defined __STEP_029_XX_H__)
#define __STEP_029_XX_H__


////////////////////
//	Test step 029.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest029_01) : public CTestStepCommDb
{
public:
	CCommDbTest029_01();
	~CCommDbTest029_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 029.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest029_02) : public CTestStepCommDb
{
public:
	CCommDbTest029_02();
	~CCommDbTest029_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 029.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest029_03) : public CTestStepCommDb
{
public:
	CCommDbTest029_03();
	~CCommDbTest029_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 029.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest029_04) : public CTestStepCommDb
{
public:
	CCommDbTest029_04();
	~CCommDbTest029_04();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_029_XX_H__
