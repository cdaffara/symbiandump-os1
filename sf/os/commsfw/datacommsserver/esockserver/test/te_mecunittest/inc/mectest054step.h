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
// mectest054step.h
//

/**
@file
*/


#ifndef MECTEST054STEP_H
#define MECTEST054STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest054Step, "MecTest054");


class CMecTest054Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest054Step();

public:
    explicit CMecTest054Step()
        : CMecUnitTestStepBase(KMecTest054Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST054STEP_H


