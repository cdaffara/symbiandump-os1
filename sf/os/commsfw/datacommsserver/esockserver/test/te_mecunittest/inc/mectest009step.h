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
// mectest009step.h
//

/**
@file
*/


#ifndef MECTEST009STEP_H
#define MECTEST009STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest009Step, "MecTest009");


class CMecTest009Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest009Step();

public:
    explicit CMecTest009Step()
        : CMecUnitTestStepBase(KMecTest009Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST009STEP_H


