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
// mectest012step.h
//

/**
@file
*/


#ifndef MECTEST012STEP_H
#define MECTEST012STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest012Step, "MecTest012");


class CMecTest012Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest012Step();

public:
    explicit CMecTest012Step()
        : CMecUnitTestStepBase(KMecTest012Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST012STEP_H


