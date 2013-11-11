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
// This is the header file for PsdAgt test 13.1 - 13.2
// 
//


#if (!defined __PSDAGTTEST_CLOSURE_H__)
#define __PSDAGTTEST_CLOSURE_H__


class CPsdAgtTest13_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest13_1() : CTestStepPsdAgt(_L("Test13.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};


class CPsdAgtTest13_2 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest13_2() : CTestStepPsdAgt(_L("Test13.2")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

#endif //(__PSDAGTTEST_CLOSURE_H__)
