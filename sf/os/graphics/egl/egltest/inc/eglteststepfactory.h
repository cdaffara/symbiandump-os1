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

#ifndef EGLTESTSTEPFACTORY_H
#define EGLTESTSTEPFACTORY_H

#include <test/egltestcommonstep.h>

NONSHARABLE_CLASS(EglTestStepFactory)
	{
public:
	IMPORT_C static CEglTestStep* GetEglTestStep(const TDesC& aStepName);
	};
	
#endif // EGLTESTSTEPFACTORY_H
