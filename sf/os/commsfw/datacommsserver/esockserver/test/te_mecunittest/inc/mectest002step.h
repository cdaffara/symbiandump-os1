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
// mectest002step.h
//

/**
@file
*/


#ifndef MECTEST002STEP_H
#define MECTEST002STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest002Step, "MecTest002");


class CMecTest002Step : public CMecUnitTestStepBase
    {
public:
    virtual ~CMecTest002Step();

public:
    explicit CMecTest002Step()
        : CMecUnitTestStepBase(KMecTest002Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST002STEP_H


