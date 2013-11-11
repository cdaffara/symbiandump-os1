/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file WapFullySpecWSPSteps.h
*/
#if (!defined __FULLYSPECWSP_STEP_H__)
#define __FULLYSPECWSP_STEP_H__
#include <test/testexecutestepbase.h>
#include <in_sock.h>
#include <es_sock.h>
#include <wapmessage.h>
#include "WapStackSuiteStepBase.h"

_LIT(KFullySpecWSPStep_1,"FullySpecWSP_1");
_LIT(KFullySpecWSPStep_2,"FullySpecWSP_2");
_LIT(KFullySpecWSPStep_3,"FullySpecWSP_3");
_LIT(KFullySpecWSPStep_4,"FullySpecWSP_4");
_LIT(KFullySpecWSPStep_5,"FullySpecWSP_5");
_LIT(KFullySpecWSPStep_6,"FullySpecWSP_6");
_LIT(KFullySpecWSPStep_7,"FullySpecWSP_7");
_LIT(KFullySpecWSPStep_8,"FullySpecWSP_8");
_LIT(KFullySpecWSPStep_9,"FullySpecWSP_9");

class CFullySpecWSPStepBase : public CWapStackSuiteStepBase
{
public:
	CFullySpecWSPStepBase();
	~CFullySpecWSPStepBase();
	CWapFullySpecCLWSPService* LoadInterface();
	void UnloadInterface();
protected:
	CWapFullySpecCLWSPService* iImp;
};


class CFullySpecWSPStep_1 : public CFullySpecWSPStepBase
	{
public:
	CFullySpecWSPStep_1();
	~CFullySpecWSPStep_1();
	virtual TVerdict doTestStepL();
private:
	};

class CFullySpecWSPStep_2 : public CFullySpecWSPStepBase
	{
public:
	CFullySpecWSPStep_2();
	~CFullySpecWSPStep_2();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWSPStep_3: public CFullySpecWSPStepBase
	{
public:
	CFullySpecWSPStep_3();
	~CFullySpecWSPStep_3();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWSPStep_4: public CFullySpecWSPStepBase
	{
public:
	CFullySpecWSPStep_4();
	~CFullySpecWSPStep_4();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWSPStep_5: public CFullySpecWSPStepBase
	{
public:
	CFullySpecWSPStep_5();
	~CFullySpecWSPStep_5();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWSPStep_6: public CFullySpecWSPStepBase
	{
public:
	CFullySpecWSPStep_6();
	~CFullySpecWSPStep_6();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CFullySpecWSPStep_7: public CFullySpecWSPStepBase
	{
public:
	CFullySpecWSPStep_7();
	~CFullySpecWSPStep_7();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CFullySpecWSPStep_8: public CFullySpecWSPStepBase
	{
public:
	CFullySpecWSPStep_8();
	~CFullySpecWSPStep_8();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CFullySpecWSPStep_9: public CFullySpecWSPStepBase
	{
public:
	CFullySpecWSPStep_9();
	~CFullySpecWSPStep_9();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

#endif
