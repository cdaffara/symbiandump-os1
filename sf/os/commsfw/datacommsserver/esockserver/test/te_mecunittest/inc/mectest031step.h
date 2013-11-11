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
// mectest031step.h
//

/**
@file
*/


#ifndef MECTEST031STEP_H
#define MECTEST031STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest031Step, "MecTest031");


class CMecTest031Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest031Step();

public:
    explicit CMecTest031Step()
        : CMecUnitTestStepBase(KMecTest031Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST031STEP_H


