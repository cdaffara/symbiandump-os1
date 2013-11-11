// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for ESOCK basic socket test section 1
// 
//


#if !defined(SYMBIAN_NETWORKING_ESOCKTEST_ESOCKSSATEST1_H_)
#define SYMBIAN_NETWORKING_ESOCKTEST_ESOCKSSATEST1_H_

#include "TestStepESockSSA.h"

class CESockSSATest1 : public CTestStepESockSSA
	{
public:
	static const TDesC &GetTestName();
	virtual enum TVerdict doTestStepL();
	};

class CESockSSATest2 : public CTestStepESockSSA
	{
public:
	static const TDesC &GetTestName();
	virtual enum TVerdict doTestStepL();
	};

class CESockSSATest3 : public CTestStepESockSSA
	{
public:
	static const TDesC &GetTestName();
	virtual enum TVerdict doTestStepL();
	};

class CESockSSATest4 : public CTestStepESockSSA
	{
public:
	static const TDesC &GetTestName();
	virtual enum TVerdict doTestStepL();
	};

class CESockSSATest5 : public CTestStepESockSSA
	{
public:
	static const TDesC &GetTestName();
	virtual enum TVerdict doTestStepL();
	};

#endif //SYMBIAN_NETWORKING_ESOCKTEST_ESOCKSSATEST1_H_


