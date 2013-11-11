// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TE_SMSPRTSTRESS_H
#define TE_SMSPRTSTRESS_H

#include "TE_smsprtstressbase.h"

class CTestRxAndRead : public CSmsPrtStressTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestMultipleSimultaneousRequests : public CSmsPrtStressTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestSmsPrtBackup : public CSmsPrtStressTestStep
	{
public:
	virtual TVerdict doTestStepL();

private:
	// TODO: replace with one from CSmsBaseTestStep
	TInt SendSmsL(RSocket& aSocket);
	};

class CTestSmsPrtBackupWhenPhoneIsOff : public CSmsPrtStressTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

#endif // TE_SMSPRTSTRESS_H
