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
// mectestpanic9step.h
//

/**
@file
*/


#ifndef MECTESTPANIC9STEP_H
#define MECTESTPANIC9STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic9Step, "MecTestPanic9");


class CMecTestPanic9Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic9Step();

public:
    explicit CMecTestPanic9Step()
        : CMecUnitTestStepBase(KMecTestPanic9Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC9STEP_H


