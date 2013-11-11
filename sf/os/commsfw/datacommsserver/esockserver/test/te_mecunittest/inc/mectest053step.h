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
// mectest053step.h
//

/**
@file
*/


#ifndef MECTEST053STEP_H
#define MECTEST053STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest053Step, "MecTest053");


class CMecTest053Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest053Step();

public:
    explicit CMecTest053Step()
        : CMecUnitTestStepBase(KMecTest053Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST053STEP_H


