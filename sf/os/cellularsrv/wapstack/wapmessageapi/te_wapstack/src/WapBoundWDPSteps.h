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
 @file WapBoundWDPSteps.h
*/
#if (!defined __BOUNDWDP_STEP_H__)
#define __BOUNDWDP_STEP_H__
#include <test/testexecutestepbase.h>
#include <in_sock.h>
#include <es_sock.h>
#include <wapmessage.h>
#include "WapStackSuiteStepBase.h"

_LIT(KBoundWDPStep_1,"BoundWDP_1");
_LIT(KBoundWDPStep_2,"BoundWDP_2");
_LIT(KBoundWDPStep_3,"BoundWDP_3");
_LIT(KBoundWDPStep_4,"BoundWDP_4");
_LIT(KBoundWDPStep_5,"BoundWDP_5");
_LIT(KBoundWDPStep_6,"BoundWDP_6");
_LIT(KBoundWDPStep_7,"BoundWDP_7");
_LIT(KBoundWDPStep_8,"BoundWDP_8");
_LIT(KBoundWDPStep_9,"BoundWDP_9");
_LIT(KBoundWDPStep_OutOfMemory,"BoundWDP_OutOfMemory");

class CBoundWDPStepBase : public CWapStackSuiteStepBase
{
public:
	CBoundWDPStepBase();
	~CBoundWDPStepBase();
	CWapBoundDatagramService* LoadInterface();
	void UnloadInterface();
protected:
	CWapBoundDatagramService* iImp;
};


class CBoundWDPStep_1 : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_1();
	~CBoundWDPStep_1();
	virtual TVerdict doTestStepL();
private:
	};

class CBoundWDPStep_2 : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_2();
	~CBoundWDPStep_2();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
	};

class CBoundWDPStep_3 : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_3();
	~CBoundWDPStep_3();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
public:
	};

class CBoundWDPStep_4 : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_4();
	~CBoundWDPStep_4();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
protected:
private:
	};

class CBoundWDPStep_5 : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_5();
	~CBoundWDPStep_5();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CBoundWDPStep_6 : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_6();
	~CBoundWDPStep_6();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CBoundWDPStep_7 : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_7();
	~CBoundWDPStep_7();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CBoundWDPStep_8 : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_8();
	~CBoundWDPStep_8();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CBoundWDPStep_9 : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_9();
	~CBoundWDPStep_9();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};
class CBoundWDPStep_OutOfMemory : public CBoundWDPStepBase
	{
public:
	CBoundWDPStep_OutOfMemory();
	~CBoundWDPStep_OutOfMemory();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};	


#endif
