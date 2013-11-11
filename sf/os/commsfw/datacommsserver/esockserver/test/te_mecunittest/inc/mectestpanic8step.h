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
// mectestpanic8step.h
//

/**
@file
*/


#ifndef MECTESTPANIC8STEP_H
#define MECTESTPANIC8STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic8Step, "MecTestPanic8");


class CMecTestPanic8Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic8Step();

public:
    explicit CMecTestPanic8Step()
        : CMecUnitTestStepBase(KMecTestPanic8Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC8STEP_H


