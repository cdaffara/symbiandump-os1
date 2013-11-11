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
// mectestpanic3step.h
//

/**
@file
*/


#ifndef MECTESTPANIC3STEP_H
#define MECTESTPANIC3STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic3Step, "MecTestPanic3");


class CMecTestPanic3Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic3Step();

public:
    explicit CMecTestPanic3Step()
        : CMecUnitTestStepBase(KMecTestPanic3Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC3STEP_H


