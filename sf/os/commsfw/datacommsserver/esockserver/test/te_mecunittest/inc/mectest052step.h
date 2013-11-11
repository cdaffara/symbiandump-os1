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
// mectest052step.h
//

/**
@file
*/


#ifndef MECTEST052STEP_H
#define MECTEST052STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest052Step, "MecTest052");


class CMecTest052Step : public CMecUnitTestStepBase
    {
public:
    virtual ~CMecTest052Step();

public:
    explicit CMecTest052Step()
        : CMecUnitTestStepBase(KMecTest052Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST052STEP_H


