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
 @file WapBoundCLPushSteps.h
*/
#if (!defined __BOUNDCLPUSH_STEP_H__)
#define __BOUNDCLPUSH_STEP_H__
#include <test/testexecutestepbase.h>
#include <in_sock.h>
#include <es_sock.h>
#include <wapmessage.h>
#include "WapStackSuiteStepBase.h"

_LIT(KBoundCLPushStep_1,"BoundCLPush_1");
_LIT(KBoundCLPushStep_2,"BoundCLPush_2");
_LIT(KBoundCLPushStep_3,"BoundCLPush_3");
_LIT(KBoundCLPushStep_4,"BoundCLPush_4");
_LIT(KBoundCLPushStep_5,"BoundCLPush_5");
_LIT(KBoundCLPushStep_6,"BoundCLPush_6");
_LIT(KBoundCLPushStep_7,"BoundCLPush_7");
_LIT(KBoundCLPushStep_8,"BoundCLPush_8");
_LIT(KBoundCLPushStep_9,"BoundCLPush_9");


class CBoundCLPushStepBase : public CWapStackSuiteStepBase
{
public:
	CBoundCLPushStepBase();
	~CBoundCLPushStepBase();
	CWapBoundCLPushService* LoadInterface();
	void UnloadInterface();
protected:
	CWapBoundCLPushService* iImp;
};

class CBoundCLPushStep_1 : public CBoundCLPushStepBase
	{
public:
	CBoundCLPushStep_1();
	~CBoundCLPushStep_1();
	virtual TVerdict doTestStepL();
private:
	};

class CBoundCLPushStep_2 : public CBoundCLPushStepBase
	{
public:
	CBoundCLPushStep_2();
	~CBoundCLPushStep_2();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundCLPushStep_3 : public CBoundCLPushStepBase
	{
public:
	CBoundCLPushStep_3();
	~CBoundCLPushStep_3();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundCLPushStep_4 : public CBoundCLPushStepBase
	{
public:
	CBoundCLPushStep_4();
	~CBoundCLPushStep_4();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundCLPushStep_5 : public CBoundCLPushStepBase
	{
public:
	CBoundCLPushStep_5();
	~CBoundCLPushStep_5();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundCLPushStep_6 : public CBoundCLPushStepBase
	{
public:
	CBoundCLPushStep_6();
	~CBoundCLPushStep_6();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundCLPushStep_7 : public CBoundCLPushStepBase
	{
public:
	CBoundCLPushStep_7();
	~CBoundCLPushStep_7();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundCLPushStep_8 : public CBoundCLPushStepBase
	{
public:
	CBoundCLPushStep_8();
	~CBoundCLPushStep_8();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundCLPushStep_9 : public CBoundCLPushStepBase
	{
public:
	CBoundCLPushStep_9();
	~CBoundCLPushStep_9();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};


#endif
