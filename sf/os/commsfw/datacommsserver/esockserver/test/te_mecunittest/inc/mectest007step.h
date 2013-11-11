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
// mectest007step.h
//

/**
@file
*/


#ifndef MECTEST007STEP_H
#define MECTEST007STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest007Step, "MecTest007");


class CMecTest007Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest007Step();

public:
    explicit CMecTest007Step()
        : CMecUnitTestStepBase(KMecTest007Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST007STEP_H


