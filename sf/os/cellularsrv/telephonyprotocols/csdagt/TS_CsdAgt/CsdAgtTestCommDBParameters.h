// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This is the header file for CsdAgt test 1.1 - 1.8
// 
//


#if (!defined __CSDAGTTEST_COMMDB_PARAMETERS_H__)
#define __CSDAGTTEST_COMMDB_PARAMETERS_H__


class CCsdAgtTest1_1 : public CTestStepCsdAgt
	{
public:
	CCsdAgtTest1_1() : CTestStepCsdAgt(_L("Test1.1")) {};

	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest1_2 : public CTestStepCsdAgt
	{
public:
	CCsdAgtTest1_2() : CTestStepCsdAgt(_L("Test1.2")) {};

	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest1_3 : public CTestStepCsdAgt
	{
public:
	CCsdAgtTest1_3() : CTestStepCsdAgt(_L("Test1.3")) {};

	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest1_5 : public CTestStepCsdAgt
	{
public:
	CCsdAgtTest1_5() : CTestStepCsdAgt(_L("Test1.5")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest1_6 : public CTestStepCsdAgt
	{
public:
	CCsdAgtTest1_6() : CTestStepCsdAgt(_L("Test1.6")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest1_7 : public CTestStepCsdAgt
	{
public:
	CCsdAgtTest1_7() : CTestStepCsdAgt(_L("Test1.7")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};

class CCsdAgtTest1_8 : public CTestStepCsdAgt
	{
public:
	CCsdAgtTest1_8() : CTestStepCsdAgt(_L("Test1.8")) {};
	
	virtual enum TVerdict doCsdAgtTestStepL( void );
	};


#endif //(__CSDAGTTEST_COMMDB_PARAMETERS_H__)
