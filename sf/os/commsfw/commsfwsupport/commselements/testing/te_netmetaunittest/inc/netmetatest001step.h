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
// netmetatest001step.h
//

/**
@file
*/


#ifndef NETMETATEST001STEP_H
#define NETMETATEST001STEP_H

#include "netmetaunitteststepbase.h"


_LIT(KNetMetaTest001Step, "NetMetaTest001");


class CNetMetaTest001Step : public CNetMetaUnitTestStepBase
	{
public:
	virtual ~CNetMetaTest001Step();

public:
    explicit CNetMetaTest001Step()
        : CNetMetaUnitTestStepBase(KNetMetaTest001Step)
        {
        }

    virtual TVerdict RunTestStepL();
    };
  
#endif
// NETMETATEST001STEP_H

