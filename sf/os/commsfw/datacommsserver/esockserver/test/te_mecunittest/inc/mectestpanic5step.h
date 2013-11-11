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
// mectestpanic5step.h
//

/**
@file
*/


#ifndef MECTESTPANIC5STEP_H
#define MECTESTPANIC5STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic5Step, "MecTestPanic5");


class CMecTestPanic5Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic5Step();

public:
    explicit CMecTestPanic5Step()
        : CMecUnitTestStepBase(KMecTestPanic5Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC5STEP_H


