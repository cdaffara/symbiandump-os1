/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file TestLoginDialog.h
*/
#if (!defined __TESTLOGINDIALOG_H__)
#define __TESTLOGINDIALOG_H__

//Symbian OS Header
#include <agentdialog.h>
//Test Framework Header
#include <test/testexecutestepbase.h>
//Test Harness Header
#include "TE_DialogServer.h"



class CTestLoginDialog : public CTestStep
	{
public:
	CTestLoginDialog();
	~CTestLoginDialog();
	virtual TVerdict doTestStepL();
	void checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError);
	void checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError);

private:
	};


_LIT(KTestLoginDialog,"TestLoginDialog");


#endif // __TESTLOGINDIALOG_H__

