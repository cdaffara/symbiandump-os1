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
// This is the header file for CommDb test 039.XX group of scenarios 


#if (!defined __STEP_039_XX_H__)
#define __STEP_039_XX_H__


////////////////////
//	Test step 039.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest039_01) : public CTestStepCommDb
{
public:
	CCommDbTest039_01();
	~CCommDbTest039_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

};

////////////////////
//	Test step 039.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest039_02) : public CTestStepCommDb
{
public:
	CCommDbTest039_02();
	~CCommDbTest039_02();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_038_XX_H__
