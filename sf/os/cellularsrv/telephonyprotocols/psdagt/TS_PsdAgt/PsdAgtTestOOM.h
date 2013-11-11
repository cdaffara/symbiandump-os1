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
// PsdAgtTestClosure.h
// This is the header file for PsdAgt test 15.1 - 15.4
// 
//


#if (!defined __PSDAGTTEST_OOM_H__)
#define __PSDAGTTEST_OOM_H__


class CPsdAgtTest15_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest15_1() : CTestStepPsdAgt(_L("Test15.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};


class CPsdAgtTest15_2 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest15_2() : CTestStepPsdAgt(_L("Test15.2")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest15_3 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest15_3() : CTestStepPsdAgt(_L("Test15.3")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest15_4 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest15_4() : CTestStepPsdAgt(_L("Test15.4")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};


#endif //(__PSDAGTTEST_OOM_H__)
