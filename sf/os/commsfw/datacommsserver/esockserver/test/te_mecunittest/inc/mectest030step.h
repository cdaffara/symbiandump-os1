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
// mectest030step.h
//

/**
@file
*/


#ifndef MECTEST030STEP_H
#define MECTEST030STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest030Step, "MecTest030");


class CMecTest030Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest030Step();

public:
    explicit CMecTest030Step()
        : CMecUnitTestStepBase(KMecTest030Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST030STEP_H


