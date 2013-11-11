// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ECAMDIRECTVIEWFINDERTEST_H
#define ECAMDIRECTVIEWFINDERTEST_H

#include <testframework.h>
#include "directviewfinder.h"


class RECamDirectViewFinderTest : public RTestStep
	{
public:
	static RECamDirectViewFinderTest* NewL(const TDesC& aTestStep);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	
	// new
	TVerdict DoTestStep_44L();
	TVerdict DoTestStep_Alloc_44L();
	TVerdict DoTestStep_45L();
	
private:
	RECamDirectViewFinderTest(const TDesC& aTestStep);
	};

#endif // ECAMDIRECTVIEWFINDERTEST_H
