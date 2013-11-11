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
// mectest004step.h
//

/**
@file
*/


#ifndef MECTEST004STEP_H
#define MECTEST004STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest004Step, "MecTest004");


class CMecTest004Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest004Step();

public:
    explicit CMecTest004Step()
        : CMecUnitTestStepBase(KMecTest004Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST004STEP_H


