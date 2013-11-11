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
// mectestpanic11step.h
//

/**
@file
*/


#ifndef MECTESTPANIC11STEP_H
#define MECTESTPANIC11STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic11Step, "MecTestPanic11");


class CMecTestPanic11Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic11Step();

public:
    explicit CMecTestPanic11Step()
        : CMecUnitTestStepBase(KMecTestPanic11Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC11STEP_H


