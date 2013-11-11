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

#if !defined(__STEP051_01_H__)
#define __STEP051_01_H__


////////////////////
//Normal Table protection
////////////////////

NONSHARABLE_CLASS(CCommDbTest051_01) : public CTestStepCommDb
{
public:
	CCommDbTest051_01();
	virtual ~CCommDbTest051_01();
	TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL( void );
};


////////////////////
//Normal Table Unprotection
////////////////////
NONSHARABLE_CLASS(CCommDbTest051_02) : public CTestStepCommDb
{
public:
	CCommDbTest051_02();
	virtual ~CCommDbTest051_02();
	TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL( void );

};

#endif 
