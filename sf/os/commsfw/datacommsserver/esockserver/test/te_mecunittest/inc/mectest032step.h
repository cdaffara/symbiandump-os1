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
// mectest032step.h
//

/**
@file
*/


#ifndef MECTEST032STEP_H
#define MECTEST032STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest032Step, "MecTest032");


class CMecTest032Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest032Step();

public:
    explicit CMecTest032Step()
        : CMecUnitTestStepBase(KMecTest032Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST032STEP_H


