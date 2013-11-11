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
 @file WapFullySpecWDPSteps.h
*/
#if (!defined __FULLYSPECWDP_STEP_H__)
#define __FULLYSPECWDP_STEP_H__
#include <test/testexecutestepbase.h>
#include <in_sock.h>
#include <es_sock.h>
#include <wapmessage.h>

#include "WapStackSuiteStepBase.h"

_LIT(KFullySpecWDPStep_1,"FullySpecWDP_1");
_LIT(KFullySpecWDPStep_2,"FullySpecWDP_2");
_LIT(KFullySpecWDPStep_3,"FullySpecWDP_3");
_LIT(KFullySpecWDPStep_4,"FullySpecWDP_4");
_LIT(KFullySpecWDPStep_5,"FullySpecWDP_5");
_LIT(KFullySpecWDPStep_6,"FullySpecWDP_6");
_LIT(KFullySpecWDPStep_7,"FullySpecWDP_7");
_LIT(KFullySpecWDPStep_8,"FullySpecWDP_8");
_LIT(KFullySpecWDPStep_9,"FullySpecWDP_9");

class CFullySpecWDPStepBase : public CWapStackSuiteStepBase
{
public:
	~CFullySpecWDPStepBase();
	void UnloadInterface();
protected:
	CFullySpecWDPStepBase();
	CWapFullySpecDatagramService* LoadInterface();
	CWapFullySpecDatagramService* iImp;
};

class CFullySpecWDPStep_1 : public CFullySpecWDPStepBase
	{
public:
	CFullySpecWDPStep_1();
	~CFullySpecWDPStep_1();
	virtual TVerdict doTestStepL();
private:
	};

class CFullySpecWDPStep_2 : public CFullySpecWDPStepBase
	{
public:
	CFullySpecWDPStep_2();
	~CFullySpecWDPStep_2();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWDPStep_3 : public CFullySpecWDPStepBase
	{
public:
	CFullySpecWDPStep_3();
	~CFullySpecWDPStep_3();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWDPStep_4 : public CFullySpecWDPStepBase
	{
public:
	CFullySpecWDPStep_4();
	~CFullySpecWDPStep_4();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWDPStep_5: public CFullySpecWDPStepBase
	{
public:
	CFullySpecWDPStep_5();
	~CFullySpecWDPStep_5();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWDPStep_6: public CFullySpecWDPStepBase
	{
public:
	CFullySpecWDPStep_6();
	~CFullySpecWDPStep_6();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWDPStep_7: public CFullySpecWDPStepBase
	{
public:
	CFullySpecWDPStep_7();
	~CFullySpecWDPStep_7();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWDPStep_8: public CFullySpecWDPStepBase
	{
public:
	CFullySpecWDPStep_8();
	~CFullySpecWDPStep_8();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

class CFullySpecWDPStep_9: public CFullySpecWDPStepBase
	{
public:
	CFullySpecWDPStep_9();
	~CFullySpecWDPStep_9();
	virtual TVerdict doTestStepL();
	virtual TInt CallStateMachine();
private:
	};

#endif
