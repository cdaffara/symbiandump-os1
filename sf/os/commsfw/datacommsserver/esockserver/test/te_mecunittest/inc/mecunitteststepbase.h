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
// mecunitteststepbase.h
//

/**
@file

This is the base class for all test steps in the DUN test suite
*/

#ifndef MECUNITTESTSTEPBASE_H
#define MECUNITTESTSTEPBASE_H


#include <test/testexecutestepbase.h>
#include "testextensions.h"

namespace ESock
{
	class RMetaExtensionContainer;
}

class CMecUnitTestStepBase : public CTestStep
	{
public:
	virtual ~CMecUnitTestStepBase();

public:
    explicit CMecUnitTestStepBase(const TDesC& aTestName)
        {
        SetTestStepName(aTestName);
        }

protected:
	void CreateAndAppendExtensionL(ESock::RMetaExtensionContainer& aMec, TUint32 aExtensionType, TUint32 aVersion);
	inline void CreateAndAppendExtensionL(ESock::RMetaExtensionContainer& aMec, TUint32 aExtensionType)
		{
		CreateAndAppendExtensionL(aMec, aExtensionType, KExtVersion1);
		}

	
    virtual TVerdict RunTestStepL() = 0;
    
private:
    virtual TVerdict doTestStepL();
//    virtual TVerdict doTestStepPreambleL();
//    virtual TVerdict doTestStepPostambleL();
    };


#endif
// MECUNITTESTSTEPBASE_H


