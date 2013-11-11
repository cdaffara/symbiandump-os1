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
// mectest033step.h
//

/**
@file
*/


#ifndef MECTEST033STEP_H
#define MECTEST033STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest033Step, "MecTest033");


class CMecTest033Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest033Step();

public:
    explicit CMecTest033Step()
        : CMecUnitTestStepBase(KMecTest033Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST033STEP_H


