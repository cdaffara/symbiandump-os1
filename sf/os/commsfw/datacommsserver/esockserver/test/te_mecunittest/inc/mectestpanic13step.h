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
// mectestpanic13step.h
//

/**
@file
*/


#ifndef MECTESTPANIC13STEP_H
#define MECTESTPANIC13STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic13Step, "MecTestPanic13");


class CMecTestPanic13Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic13Step();

public:
    explicit CMecTestPanic13Step()
        : CMecUnitTestStepBase(KMecTestPanic13Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC13STEP_H


