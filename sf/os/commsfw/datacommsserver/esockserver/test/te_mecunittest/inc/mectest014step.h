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
// mectest014step.h
//

/**
@file
*/


#ifndef MECTEST014STEP_H
#define MECTEST014STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest014Step, "MecTest014");


class CMecTest014Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest014Step();

public:
    explicit CMecTest014Step()
        : CMecUnitTestStepBase(KMecTest014Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST014STEP_H


