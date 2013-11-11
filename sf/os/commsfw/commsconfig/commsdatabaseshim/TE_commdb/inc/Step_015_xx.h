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
// This is the header file for CommDb test 015.XX group of scenarios 


#if (!defined __STEP_015_XX_H__)
#define __STEP_015_XX_H__


////////////////////
//	Test step 015.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_01) : public CTestStepCommDb
{
public:
	CCommDbTest015_01();
	~CCommDbTest015_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
private:

};


////////////////////
//	Test step 015.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_02) : public CTestStepCommDb
{
public:
	CCommDbTest015_02();
	~CCommDbTest015_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();


private:

};

////////////////////
//	Test step 015.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_03) : public CTestStepCommDb
{
public:
	CCommDbTest015_03();
	~CCommDbTest015_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();


private:

};

////////////////////
//	Test step 015.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_04) : public CTestStepCommDb
{
public:
	CCommDbTest015_04();
	~CCommDbTest015_04();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

private:

};


////////////////////
//	Test step 015.05
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_05) : public CTestStepCommDb
{
public:
	CCommDbTest015_05();
	~CCommDbTest015_05();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

private:

};


////////////////////
//	Test step 015.06
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_06) : public CTestStepCommDb
{
public:
	CCommDbTest015_06();
	~CCommDbTest015_06();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

private:

};


////////////////////
//	Test step 015.07
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_07) : public CTestStepCommDb
{
public:
	CCommDbTest015_07();
	~CCommDbTest015_07();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 015.08
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_08) : public CTestStepCommDb
{
public:
	CCommDbTest015_08();
	~CCommDbTest015_08();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	
private:
	//These members are run as a separate thread
	static	TInt step015_08Thread ( TAny * ptr );
	static	TInt tryDatabaseWriteL(void);
};

////////////////////
//	Test step 015.09
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_09) : public CTestStepCommDb
{
public:
	CCommDbTest015_09();
	~CCommDbTest015_09();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();

private:

};

////////////////////
//	Test step 015.10
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_10) : public CTestStepCommDb
{
public:
	CCommDbTest015_10();
	~CCommDbTest015_10();

	virtual enum TVerdict doTestStepL( void );


private:

};

////////////////////
//	Test step 015.11
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_11) : public CTestStepCommDb
{
public:
	CCommDbTest015_11();
	~CCommDbTest015_11();

	virtual enum TVerdict doTestStepL( void );


private:

};

////////////////////
//	Test step 015.12
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_12) : public CTestStepCommDb
{
public:
	CCommDbTest015_12();
	~CCommDbTest015_12();

	virtual enum TVerdict doTestStepL( void );


private:

};

////////////////////
//	Test step 015.13
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_13) : public CTestStepCommDb
{
public:
	CCommDbTest015_13();
	~CCommDbTest015_13();

	virtual enum TVerdict doTestStepL( void );


private:

};

////////////////////
//	Test step 015.14
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_14) : public CTestStepCommDb
{
public:
	CCommDbTest015_14();
	~CCommDbTest015_14();

	virtual enum TVerdict doTestStepL( void );


private:

};

////////////////////
//	Test step 015.15
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_15) : public CTestStepCommDb
{
public:
	CCommDbTest015_15();
	~CCommDbTest015_15();

	virtual enum TVerdict doTestStepL( void );


private:

};

////////////////////
//	Test step 015.16
////////////////////

NONSHARABLE_CLASS(CCommDbTest015_16) : public CTestStepCommDb
{
public:
	CCommDbTest015_16();
	~CCommDbTest015_16();

	virtual enum TVerdict doTestStepL( void );


private:

};


#endif //(__STEP_015_XX_H__)
