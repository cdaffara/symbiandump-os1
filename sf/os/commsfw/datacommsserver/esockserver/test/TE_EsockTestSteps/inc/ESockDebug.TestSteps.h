/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ESockDebug.TestSteps.h
*/

#if !defined (ESOCKDEBUG_TESTSTEPS_H)
#define ESOCKDEBUG_TESTSTEPS_H

// Includes
#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include <elements/sd_mintercept.h>

using namespace Den;

// Ini file section and field names
_LIT(KPatternName, "PatternName");
_LIT(KExpression, "Expression");

// Test steps
//===========

class CMsgInterceptDebug : public CTe_EsockStepBase
	{
public:
	CMsgInterceptDebug(CCEsockTestBase*& aEsockTest);
	~CMsgInterceptDebug();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt SendDebugMessage(RSocketServ& aSocketSvr) const;

	TPtrC iSocketSvrName;
	};

_LIT(KMsgInterceptDebug,"ESockDebug_MessageIntercept_Debug");


// Append new pattern to the intercept register
//---------------------------------------------
class CAppendMessageInterceptPattern : public CTe_EsockStepBase
	{
public:
	CAppendMessageInterceptPattern(CCEsockTestBase*& aEsockTest);
	~CAppendMessageInterceptPattern();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	void AppendPatternToRegisterL(RSocketServ& aSocketSvr) const;
	void ReadActionL(TAction& aAction);
	void ReadEventExpressionL(TInt aIndex, TEventExpression& aExpression);

	CPattern* iPattern;
	TPtrC iSocketSvrName;
	};

_LIT(KAppendMessageInterceptPattern,"ESockDebug_MsgIntercept_AppendPattern");


// EnableRegister
//---------------
class CEnableMessageInterceptRegister : public CTe_EsockStepBase
	{
public:
	CEnableMessageInterceptRegister(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TPtrC iSocketSvrName;
	};

_LIT(KEnableMessageInterceptRegister,"ESockDebug_MsgIntercept_Enable");



// DisableRegister
//----------------
class CDisableMessageInterceptRegister : public CTe_EsockStepBase
	{
public:
	CDisableMessageInterceptRegister(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt DisableESockDebugRegister(RSocketServ& socketSvrForDebug) const;
	TPtrC iSocketSvrName;
	};

_LIT(KDisableMessageInterceptRegister,"ESockDebug_MsgIntercept_Disable");


// ResetRegister
//--------------
class CResetMessageInterceptRegister : public CTe_EsockStepBase
	{
public:
	CResetMessageInterceptRegister(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt ResetESockDebugRegister(RSocketServ& socketSvrForDebug) const;
	TPtrC iSocketSvrName;
	};

_LIT(KResetMessageInterceptRegister,"ESockDebug_MsgIntercept_Reset");


// QueryComplete
//--------------
class CQueryMsgInterceptComplete : public CTe_EsockStepBase
	{
public:
	CQueryMsgInterceptComplete(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TInt MsgInterceptQueryComplete(RSocketServ& socketSvrForDebug) const;
	TPtrC iSocketSvrName;
	};

_LIT(KQueryMsgInterceptComplete,"ESockDebug_MsgIntercept_QueryComplete");




#endif

