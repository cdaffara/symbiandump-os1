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
// mectest001step.h
//

/**
@file
*/


#ifndef MECTEST001STEP_H
#define MECTEST001STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest001Step, "MecTest001");


class CMecTest001Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest001Step();

public:
    explicit CMecTest001Step()
        : CMecUnitTestStepBase(KMecTest001Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST001STEP_H


