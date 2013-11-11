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
// This is the header file for CommDb test 047.XX group of scenarios 


NONSHARABLE_CLASS(CCommDbTest047_01) : public CTestStepCommDb
{
public:
	CCommDbTest047_01();
	~CCommDbTest047_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommDbOverrideSettings* iOverSet;
};

NONSHARABLE_CLASS(CCommDbTest047_02) : public CTestStepCommDb
{
public:
	CCommDbTest047_02();
	~CCommDbTest047_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
};

NONSHARABLE_CLASS(CCommDbTest047_03) : public CTestStepCommDb
{
public:
	CCommDbTest047_03();
	~CCommDbTest047_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
};

NONSHARABLE_CLASS(CCommDbTest047_04) : public CTestStepCommDb
{
public:
	CCommDbTest047_04();
	~CCommDbTest047_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 047.05
////////////////////
NONSHARABLE_CLASS(CCommDbTest047_05) : public CTestStepCommDb
{
public:
	CCommDbTest047_05();
	~CCommDbTest047_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
};

////////////////////
//	Test step 047.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest047_06) : public CTestStepCommDb
{
public:
	CCommDbTest047_06();
	~CCommDbTest047_06();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 047.07
////////////////////
NONSHARABLE_CLASS(CCommDbTest047_07) : public CTestStepCommDb
{
public:
	CCommDbTest047_07();
	~CCommDbTest047_07();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
};

////////////////////
//	Test step 047.08
////////////////////
NONSHARABLE_CLASS(CCommDbTest047_08) : public CTestStepCommDb
{
public:
	CCommDbTest047_08();
	~CCommDbTest047_08();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	CCommsDbConnectionPrefTableView* iPrefView;
};

////////////////////
//	Test step 047.13
////////////////////
NONSHARABLE_CLASS(CCommDbTest047_13) : public CTestStepCommDb
{
public:
	CCommDbTest047_13();
	~CCommDbTest047_13();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
};
