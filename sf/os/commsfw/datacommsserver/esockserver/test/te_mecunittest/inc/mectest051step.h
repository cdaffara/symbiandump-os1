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
// mectest051step.h
//

/**
@file
*/


#ifndef MECTEST051STEP_H
#define MECTEST051STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest051Step, "MecTest051");


class CMecTest051Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest051Step();

public:
    explicit CMecTest051Step()
        : CMecUnitTestStepBase(KMecTest051Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST051STEP_H


