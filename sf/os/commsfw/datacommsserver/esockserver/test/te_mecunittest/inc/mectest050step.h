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
// mectest050step.h
//

/**
@file
*/


#ifndef MECTEST050STEP_H
#define MECTEST050STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest050Step, "MecTest050");


class CMecTest050Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest050Step();

public:
    explicit CMecTest050Step()
        : CMecUnitTestStepBase(KMecTest050Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST050STEP_H


