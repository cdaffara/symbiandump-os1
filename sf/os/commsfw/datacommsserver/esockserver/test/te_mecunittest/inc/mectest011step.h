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
// mectest011step.h
//

/**
@file
*/


#ifndef MECTEST011STEP_H
#define MECTEST011STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest011Step, "MecTest011");


class CMecTest011Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest011Step();

public:
    explicit CMecTest011Step()
        : CMecUnitTestStepBase(KMecTest011Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST011STEP_H


