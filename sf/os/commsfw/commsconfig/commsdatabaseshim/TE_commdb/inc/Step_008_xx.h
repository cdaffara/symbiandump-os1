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
// This is the header file for CommDb test 008.XX group of scenarios 


#if (!defined __STEP_008_XX_H__)
#define __STEP_008_XX_H__


////////////////////
//	Test step 008.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest008_01) : public CTestStepCommDb
{
public:
	CCommDbTest008_01();
	~CCommDbTest008_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 008.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest008_02) : public CTestStepCommDb
{
public:
	CCommDbTest008_02();
	~CCommDbTest008_02();

	virtual enum TVerdict doTestStepL( void );
	
private:

};

////////////////////
//	Test step 008.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest008_03) : public CTestStepCommDb
{
public:
	CCommDbTest008_03();
	~CCommDbTest008_03();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();

private:

};

////////////////////
//	Test step 008.04
////////////////////

NONSHARABLE_CLASS(CCommDbTest008_04) : public CTestStepCommDb
{
public:
	CCommDbTest008_04();
	~CCommDbTest008_04();

	virtual enum TVerdict doTestStepL( void );
	
private:

};

#endif //(__STEP_008_XX_H__)
