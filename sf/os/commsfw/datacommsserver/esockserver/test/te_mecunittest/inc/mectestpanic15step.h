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
// mectestpanic15step.h
//

/**
@file
*/


#ifndef MECTESTPANIC15STEP_H
#define MECTESTPANIC15STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTestPanic15Step, "MecTestPanic15");


class CMecTestPanic15Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTestPanic15Step();

public:
    explicit CMecTestPanic15Step()
        : CMecUnitTestStepBase(KMecTestPanic15Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTESTPANIC15STEP_H


