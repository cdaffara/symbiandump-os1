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

/**
 @file
 @test
*/

#ifndef EGLTEST_GENERAL_H
#define EGLTEST_GENERAL_H

#include "eglteststep.h"

// General high level tests for EGL that do not belong to any particular category
_LIT(KDumpStrings, "DumpStrings");
NONSHARABLE_CLASS(CEglTest_DumpStrings) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
private:
	void DumpString(const TDesC& aField, const TDesC8& aValue);
	};

_LIT(KQueryString_Extensions, "QueryString_Extensions");
NONSHARABLE_CLASS(CEglTest_QueryString_Extensions) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

_LIT(KGeneral_Negative_ProcAddress, "General_Negative_ProcAddress");
NONSHARABLE_CLASS(CEglTest_General_Negative_ProcAddress) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();
    };

_LIT(KEglTerminate_Negative, "EglTerminate_Negative");
NONSHARABLE_CLASS(CEglTest_EglTerminate_Negative) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();
    };

#endif // EGLTEST_GENERAL_H
