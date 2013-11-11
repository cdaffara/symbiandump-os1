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
 @file WapFullySpecCLPush.h
*/
#if (!defined __FULLYSPECCLPUSH_STEP_H__)
#define __FULLYSPECCLPUSH_STEP_H__
#include <test/testexecutestepbase.h>
#include <in_sock.h>
#include <es_sock.h>
#include <wapmessage.h>
#include "WapStackSuiteStepBase.h"

_LIT(KFullySpecCLPushStep_1,"FullySpecCLPush_1");
_LIT(KFullySpecCLPushStep_2,"FullySpecCLPush_2");
_LIT(KFullySpecCLPushStep_3,"FullySpecCLPush_3");
_LIT(KFullySpecCLPushStep_4,"FullySpecCLPush_4");
_LIT(KFullySpecCLPushStep_5,"FullySpecCLPush_5");
_LIT(KFullySpecCLPushStep_6,"FullySpecCLPush_6");
_LIT(KFullySpecCLPushStep_7,"FullySpecCLPush_7");
_LIT(KFullySpecCLPushStep_8,"FullySpecCLPush_8");
_LIT(KFullySpecCLPushStep_9,"FullySpecCLPush_9");

class CFullySpecCLPushBase : public CWapStackSuiteStepBase
{
public:
	CFullySpecCLPushBase();
	~CFullySpecCLPushBase();
	CWapFullySpecCLPushService* LoadInterface();
	void UnloadInterface();
protected:
	CWapFullySpecCLPushService* iImp;
};

class CFullySpecCLPushStep_1 : public CFullySpecCLPushBase
	{
public:
	CFullySpecCLPushStep_1();
	~CFullySpecCLPushStep_1();
	virtual TVerdict doTestStepL();
private:
	};

class CFullySpecCLPushStep_2 : public CFullySpecCLPushBase
	{
public:
	CFullySpecCLPushStep_2();
	~CFullySpecCLPushStep_2();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecCLPushStep_3 : public CFullySpecCLPushBase
	{
public:
	CFullySpecCLPushStep_3();
	~CFullySpecCLPushStep_3();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecCLPushStep_4 : public CFullySpecCLPushBase
	{
public:
	CFullySpecCLPushStep_4();
	~CFullySpecCLPushStep_4();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecCLPushStep_5 : public CFullySpecCLPushBase
	{
public:
	CFullySpecCLPushStep_5();
	~CFullySpecCLPushStep_5();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecCLPushStep_6 : public CFullySpecCLPushBase
	{
public:
	CFullySpecCLPushStep_6();
	~CFullySpecCLPushStep_6();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CFullySpecCLPushStep_7 : public CFullySpecCLPushBase
	{
public:
	CFullySpecCLPushStep_7();
	~CFullySpecCLPushStep_7();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CFullySpecCLPushStep_8 : public CFullySpecCLPushBase
	{
public:
	CFullySpecCLPushStep_8();
	~CFullySpecCLPushStep_8();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CFullySpecCLPushStep_9 : public CFullySpecCLPushBase
	{
public:
	CFullySpecCLPushStep_9();
	~CFullySpecCLPushStep_9();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

#endif
