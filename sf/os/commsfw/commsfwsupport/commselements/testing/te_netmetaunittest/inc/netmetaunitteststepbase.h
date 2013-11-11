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
// netmetaunitteststepbase.h
//

/**
@file
*/

#ifndef NETMETAUNITTESTSTEPBASE_H
#define NETMETAUNITTESTSTEPBASE_H


#include <test/testexecutestepbase.h>


class CNetMetaUnitTestStepBase : public CTestStep
	{
public:
	virtual ~CNetMetaUnitTestStepBase();

public:
    explicit CNetMetaUnitTestStepBase(const TDesC& aTestName)
        {
        SetTestStepName(aTestName);
        }

protected:
    virtual TVerdict RunTestStepL() = 0;
    
private:
    virtual TVerdict doTestStepL();
//    virtual TVerdict doTestStepPreambleL();
//    virtual TVerdict doTestStepPostambleL();
    };


#endif
// NETMETAUNITTESTSTEPBASE_H

