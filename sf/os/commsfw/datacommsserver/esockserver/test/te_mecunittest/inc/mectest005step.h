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
// mectest005step.h
//

/**
@file
*/


#ifndef MECTEST005STEP_H
#define MECTEST005STEP_H

#include "mecunitteststepbase.h"


_LIT(KMecTest005Step, "MecTest005");


class CMecTest005Step : public CMecUnitTestStepBase
	{
public:
	virtual ~CMecTest005Step();

public:
    explicit CMecTest005Step()
        : CMecUnitTestStepBase(KMecTest005Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// MECTEST005STEP_H


