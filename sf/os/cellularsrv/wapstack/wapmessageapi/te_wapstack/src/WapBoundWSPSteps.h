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
 @file WapBoundWSPSteps.h
*/
#if (!defined __BOUNDWSP_STEP_H__)
#define __BOUNDWSP_STEP_H__
#include <test/testexecutestepbase.h>
#include <in_sock.h>
#include <es_sock.h>
#include <wapmessage.h>
#include "WapStackSuiteStepBase.h"

// steps names
_LIT(KBoundWSPStep_1,"BoundWSP_1");
_LIT(KBoundWSPStep_2,"BoundWSP_2");
_LIT(KBoundWSPStep_3,"BoundWSP_3");
_LIT(KBoundWSPStep_4,"BoundWSP_4");
_LIT(KBoundWSPStep_5,"BoundWSP_5");
_LIT(KBoundWSPStep_6,"BoundWSP_6");
_LIT(KBoundWSPStep_7,"BoundWSP_7");
_LIT(KBoundWSPStep_8,"BoundWSP_8");
_LIT(KBoundWSPStep_9,"BoundWSP_9");

class CBoundWSPStepBase : public CWapStackSuiteStepBase
{
public:
	CBoundWSPStepBase();
	~CBoundWSPStepBase();
	CWapBoundCLWSPService* LoadInterface();
	void UnloadInterface();
protected:
	CWapBoundCLWSPService* iImp;
};

class CBoundWSPStep_1 : public CBoundWSPStepBase
	{
public:
	CBoundWSPStep_1();
	~CBoundWSPStep_1();
	virtual TVerdict doTestStepL();
private:
	};

class CBoundWSPStep_2 : public CBoundWSPStepBase
	{
public:
	CBoundWSPStep_2();
	~CBoundWSPStep_2();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundWSPStep_3 : public CBoundWSPStepBase
	{
public:
	CBoundWSPStep_3();
	~CBoundWSPStep_3();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundWSPStep_4 : public CBoundWSPStepBase
	{
public:
	CBoundWSPStep_4();
	~CBoundWSPStep_4();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundWSPStep_5 : public CBoundWSPStepBase
	{
public:
	CBoundWSPStep_5();
	~CBoundWSPStep_5();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundWSPStep_6 : public CBoundWSPStepBase
	{
public:
	CBoundWSPStep_6();
	~CBoundWSPStep_6();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CBoundWSPStep_7 : public CBoundWSPStepBase
	{
public:
	CBoundWSPStep_7();
	~CBoundWSPStep_7();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CBoundWSPStep_8 : public CBoundWSPStepBase
	{
public:
	CBoundWSPStep_8();
	~CBoundWSPStep_8();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CBoundWSPStep_9 : public CBoundWSPStepBase
	{
public:
	CBoundWSPStep_9();
	~CBoundWSPStep_9();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

#endif
