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
// mectest055step.h
//

/**
@file
*/


#ifndef MECTEST055STEP_H
#define MECTEST055STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest055Step, "MecTest055");


class CMecTest055Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest055Step();

public:
    explicit CMecTest055Step()
        : CMecUnitTestStepBase(KMecTest055Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST055STEP_H


