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
 
#if !defined(__STEP_054_XX_H__)
#define __STEP_054_XX_H__



NONSHARABLE_CLASS(CCommDbTest054_01 ) : public CTestStepCommDb
{
public:
	CCommDbTest054_01();
	virtual ~CCommDbTest054_01();
	TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL( void );

};


////////////////////
//Normal Table Unprotection
////////////////////
NONSHARABLE_CLASS(CCommDbTest054_02) : public CTestStepCommDb
{
public:
	CCommDbTest054_02();
	virtual ~CCommDbTest054_02();
	TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL( void );

};

#endif
