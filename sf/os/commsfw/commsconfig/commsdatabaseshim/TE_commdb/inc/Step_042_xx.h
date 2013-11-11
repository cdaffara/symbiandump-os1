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
// This is the header file for CommDb test 042.XX group of scenarios 

#include<cdbover.h>

#if (!defined __STEP_042_XX_H__)
#define __STEP_042_XX_H__


////////////////////
//	Test step 042.01
////////////////////

NONSHARABLE_CLASS(CCommDbTest042_01) : public CTestStepCommDb
{
public:
	CCommDbTest042_01();
	~CCommDbTest042_01();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
};

////////////////////
//	Test step 042.02
////////////////////

NONSHARABLE_CLASS(CCommDbTest042_02) : public CTestStepCommDb
{
public:
	CCommDbTest042_02();
	~CCommDbTest042_02();

	virtual enum TVerdict doTestStepL( void );
	TInt executeStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
};

////////////////////
//	Test step 042.03
////////////////////

NONSHARABLE_CLASS(CCommDbTest042_03) : public CTestStepCommDb
{
public:
	CCommDbTest042_03();
	~CCommDbTest042_03();

	virtual enum TVerdict doTestStepL( void );
};

#endif //__STEP_042_XX_H__
