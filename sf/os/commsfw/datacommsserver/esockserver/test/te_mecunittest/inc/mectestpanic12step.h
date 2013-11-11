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
// mectestpanic12step.h
//

/**
@file
*/


#ifndef MECTESTPANIC12STEP_H
#define MECTESTPANIC12STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic12Step, "MecTestPanic12");


class CMecTestPanic12Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic12Step();

public:
    explicit CMecTestPanic12Step()
        : CMecUnitTestStepBase(KMecTestPanic12Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC12STEP_H


