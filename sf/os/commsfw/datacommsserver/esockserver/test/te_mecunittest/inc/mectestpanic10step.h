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
// mectestpanic10step.h
//

/**
@file
*/


#ifndef MECTESTPANIC10STEP_H
#define MECTESTPANIC10STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic10Step, "MecTestPanic10");


class CMecTestPanic10Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic10Step();

public:
    explicit CMecTestPanic10Step()
        : CMecUnitTestStepBase(KMecTestPanic10Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC10STEP_H


