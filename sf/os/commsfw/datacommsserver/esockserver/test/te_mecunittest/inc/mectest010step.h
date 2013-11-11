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
// mectest010step.h
//

/**
@file
*/


#ifndef MECTEST010STEP_H
#define MECTEST010STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest010Step, "MecTest010");


class CMecTest010Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest010Step();

public:
    explicit CMecTest010Step()
        : CMecUnitTestStepBase(KMecTest010Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST010STEP_H


