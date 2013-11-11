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

/**
 @file
 @test
 */
#ifndef EGLTEST_REFERENCE_H
#define EGLTEST_REFERENCE_H

#include "egltestrefstep.h"

_LIT(KQueryString, "QueryString");
NONSHARABLE_CLASS(CEglTest_QueryString) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();    
    };

#endif /* EGLTEST_REFERENCE_H */
