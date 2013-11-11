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
// mectestpanic16step.h
//

/**
@file
*/


#ifndef MECTESTPANIC16STEP_H
#define MECTESTPANIC16STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic16Step, "MecTestPanic16");


class CMecTestPanic16Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic16Step();

public:
    explicit CMecTestPanic16Step()
        : CMecUnitTestStepBase(KMecTestPanic16Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC16STEP_H


