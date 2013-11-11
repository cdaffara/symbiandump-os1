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
// This is the header file for PsdAgt test 10.1 - 10.3
// 
//


#if (!defined __PSDAGTTEST_ACTIVATE_CONTEXT_H__)
#define __PSDAGTTEST_ACTIVATE_CONTEXT_H__


class CPsdAgtTest10_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest10_1() : CTestStepPsdAgt(_L("Test10.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest10_2 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest10_2() : CTestStepPsdAgt(_L("Test10.2")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest10_3 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest10_3() : CTestStepPsdAgt(_L("Test10.3")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

#endif //(__PSDAGTTEST_ACTIVATE_CONTEXT_H__)
