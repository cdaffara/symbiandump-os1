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
// mectestpanic6step.h
//

/**
@file
*/


#ifndef MECTESTPANIC6STEP_H
#define MECTESTPANIC6STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic6Step, "MecTestPanic6");


class CMecTestPanic6Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic6Step();

public:
    explicit CMecTestPanic6Step()
        : CMecUnitTestStepBase(KMecTestPanic6Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC6STEP_H


