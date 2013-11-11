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
// This is the header file for CommDb test 017.XX group of scenarios 


#if (!defined __STEP_017_XX_H__)
#define __STEP_017_XX_H__


////////////////////
//	Test step 017.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest017_01) : public CTestStepCommDb
{
public:
	CCommDbTest017_01();
	~CCommDbTest017_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	TInt GlobalSettingTestL( const TDesC& aName );
	TInt GlobalSettingClearL( const TDesC& aName );

private:

};


////////////////////
//	Test step 017.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest017_02) : public CTestStepCommDb
{
public:
	CCommDbTest017_02();
	~CCommDbTest017_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};



////////////////////
//	Test step 017.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest017_03) : public CTestStepCommDb
{
public:
	CCommDbTest017_03();
	~CCommDbTest017_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};



////////////////////
//	Test step 017.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest017_04) : public CTestStepCommDb
{
public:
	CCommDbTest017_04();
	~CCommDbTest017_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};


////////////////////
//	Test step 017.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest017_05) : public CTestStepCommDb
{
public:
	CCommDbTest017_05();
	~CCommDbTest017_05();

	virtual enum TVerdict doTestStepL( void );

private:

};

////////////////////
//	Test step 017.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest017_06) : public CTestStepCommDb
{
public:
	CCommDbTest017_06();
	~CCommDbTest017_06();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 017.07
////////////////////

NONSHARABLE_CLASS(CCommDbTest017_07) : public CTestStepCommDb
{
public:
	CCommDbTest017_07();
	~CCommDbTest017_07();

	virtual enum TVerdict doTestStepL( void );

private:

};


#endif //(__STEP_017_XX_H__)
