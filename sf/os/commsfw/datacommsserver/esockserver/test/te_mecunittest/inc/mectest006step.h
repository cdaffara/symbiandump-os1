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
// mectest006step.h
//

/**
@file
*/


#ifndef MECTEST006STEP_H
#define MECTEST006STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest006Step, "MecTest006");


class CMecTest006Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest006Step();

public:
    explicit CMecTest006Step()
        : CMecUnitTestStepBase(KMecTest006Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST006STEP_H


