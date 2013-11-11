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
// This is the header file for CommDb test 037.XX group of scenarios 


#if (!defined __STEP_037_XX_H__)
#define __STEP_037_XX_H__


////////////////////
//	Test step 037.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest037_01) : public CTestStepCommDb
{
public:
	CCommDbTest037_01();
	~CCommDbTest037_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 037.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest037_02) : public CTestStepCommDb
{
public:
	CCommDbTest037_02();
	~CCommDbTest037_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 037.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest037_03) : public CTestStepCommDb
{
public:
	CCommDbTest037_03();
	~CCommDbTest037_03();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_037_XX_H__
