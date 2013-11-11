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

#include "egltestrefstep.h"

#include "egltest_reference.h"

/*
Entry point from egltest to create test step
*/
EXPORT_C CEglTestStep* EglTestStepFactory::GetEglTestStep(const TDesC& aStepName)
// static
	{
	CEglTestStep* testStep = NULL;

	// General Reference EGL tests
    if (aStepName == KQueryString)							testStep = new CEglTest_QueryString;

	return testStep;
	}
