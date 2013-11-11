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
// This is the header file for CommDb test 003.XX group of scenarios 


#if (!defined __STEP_003_XX_H__)
#define __STEP_003_XX_H__


////////////////////
//	Test step 003.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest003_01) : public CTestStepCommDb
{
public:
	CCommDbTest003_01();
	~CCommDbTest003_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TInt myOpenTableL();
	TVerdict doTestStepPreambleL();

private:


};

////////////////////
//	Test step 003.02
////////////////////
class CCommsDatabase;

NONSHARABLE_CLASS(CCommDbTest003_02) : public CTestStepCommDb
{
public:
	CCommDbTest003_02();
	~CCommDbTest003_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:
	TInt myOpenTableL();


};

////////////////////
//	Test step 003.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest003_03) : public CTestStepCommDb
{
public:
	CCommDbTest003_03();
	~CCommDbTest003_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:
	TInt myOpenTableL();


};

////////////////////
//	Test step 003.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest003_04) : public CTestStepCommDb
{
public:
	CCommDbTest003_04();
	~CCommDbTest003_04();

	virtual enum TVerdict doTestStepL( void );
	TVerdict doTestStepPreambleL();
	TInt executeStepL();

private:
	TInt myOpenTableL();


};

////////////////////
//	Test step 003.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest003_05) : public CTestStepCommDb
{
public:
	CCommDbTest003_05();
	~CCommDbTest003_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:
	TInt myOpenTableL();
	TBool iLocalObjects; //Flag set when objects of this class have created iTheDb and iTheView

};

////////////////////
//	Test step 003.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest003_06) : public CTestStepCommDb
{
public:
	CCommDbTest003_06();
	~CCommDbTest003_06();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:
	TInt myOpenTableL();
	TBool iLocalObjects; //Flag set when objects of this class have created iTheDb and iTheView

};

////////////////////
//	Test step 003.07
////////////////////

NONSHARABLE_CLASS(CCommDbTest003_07) : public CTestStepCommDb
{
public:
	CCommDbTest003_07();
	~CCommDbTest003_07();

	virtual enum TVerdict doTestStepL( void );

private:
	TBool iLocalObjects; //Flag set when objects of this class have created iTheDb and iTheView

};

////////////////////
//	Test step 003.08
////////////////////

NONSHARABLE_CLASS(CCommDbTest003_08) : public CTestStepCommDb
{
public:
	CCommDbTest003_08();
	~CCommDbTest003_08();

	virtual enum TVerdict doTestStepL( void );

private:
	TBool iLocalObjects; //Flag set when objects of this class have created iTheDb and iTheView

};


#endif //(__STEP_003_XX_H__)
