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
// mectestpanic4step.h
//

/**
@file
*/


#ifndef MECTESTPANIC4STEP_H
#define MECTESTPANIC4STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic4Step, "MecTestPanic4");


class CMecTestPanic4Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic4Step();

public:
    explicit CMecTestPanic4Step()
        : CMecUnitTestStepBase(KMecTestPanic4Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC4STEP_H


