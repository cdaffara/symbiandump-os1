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
// mectestpanic2step.h
//

/**
@file
*/


#ifndef MECTESTPANIC2STEP_H
#define MECTESTPANIC2STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic2Step, "MecTestPanic2");


class CMecTestPanic2Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic2Step();

public:
    explicit CMecTestPanic2Step()
        : CMecUnitTestStepBase(KMecTestPanic2Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC2STEP_H


