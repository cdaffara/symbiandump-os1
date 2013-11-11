// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#if (!defined __ELEMENTSTEP_H__)
#define __ELEMENTSTEP_H__

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <testexecutestepbase.h>
#else
#include <test/testexecutestepbase.h>
#endif


#include "ElementServer.h"

class CElementStep1 : public CTestStep
	{
public:
	CElementStep1();
	~CElementStep1();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	};

class CElementStep2 : public CTestStep
	{
public:
	CElementStep2(CElementServer& aParent);
	~CElementStep2();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	CElementServer& iParent;
	};

class CElementStep3 : public CTestStep
	{
public:
	CElementStep3(CElementServer& aParent);
	~CElementStep3();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	CElementServer& iParent;
	};


_LIT(KElementStep1,"ElementStep1");
_LIT(KElementStep2,"ElementStep2");
_LIT(KElementStep3,"ElementStep3");


#endif // __ELEMENTSTEP_H__
