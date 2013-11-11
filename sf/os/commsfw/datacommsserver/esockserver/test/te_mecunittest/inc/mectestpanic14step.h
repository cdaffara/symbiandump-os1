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
// mectestpanic14step.h
//

/**
@file
*/


#ifndef MECTESTPANIC14STEP_H
#define MECTESTPANIC14STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic14Step, "MecTestPanic14");


class CMecTestPanic14Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic14Step();

public:
    explicit CMecTestPanic14Step()
        : CMecUnitTestStepBase(KMecTestPanic14Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC14STEP_H


