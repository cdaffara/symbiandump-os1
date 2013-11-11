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
// This is the header file for CommDb test 038.XX group of scenarios 


#if (!defined __STEP_038_XX_H__)
#define __STEP_038_XX_H__


////////////////////
//	Test step 038.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest038_01) : public CTestStepCommDb
{
public:
	CCommDbTest038_01();
	~CCommDbTest038_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 038.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest038_02) : public CTestStepCommDb
{
public:
	CCommDbTest038_02();
	~CCommDbTest038_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 038.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest038_03) : public CTestStepCommDb
{
public:
	CCommDbTest038_03();
	~CCommDbTest038_03();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_038_XX_H__
