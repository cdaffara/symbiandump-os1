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
// This is the header file for PsdAgt test 2.1 - 2.4
// 
//


#if (!defined __PSDAGTTEST_CONTEXT_CONFIG_CHANGE_H__)
#define __PSDAGTTEST_CONTEXT_CONFIG_CHANGE_H__


class CPsdAgtTest2_1 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest2_1() : CTestStepPsdAgt(_L("Test2.1")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	
	// QoS change response functions
	virtual void DialogHasAppeared();
	virtual void PsdAgtDisconnectRequest();
	};

class CPsdAgtTest2_2 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest2_2() : CTestStepPsdAgt(_L("Test2.2")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	
	// QoS change response functions
	virtual void DialogHasAppeared();
	};

class CPsdAgtTest2_3 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest2_3() : CTestStepPsdAgt(_L("Test2.3")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

class CPsdAgtTest2_4 : public CTestStepPsdAgt
	{
public:
	CPsdAgtTest2_4() : CTestStepPsdAgt(_L("Test2.4")) {};

	virtual enum TVerdict doPsdAgtTestStepL( void );
	};

#endif //(__PsdAgtTEST_CONTEXT_CONFIG_CHANGE_H__)
