// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "TSU_MmTsthStep03.h"
#include "TSU_MmTsthSuite03.h"

// preamble
TVerdict RTSUMmTsthStep03::OpenL()
	{
	// create a CFileName with NewL, for use of tests - if this fails,
	// the tests are inconclusive (cannot be run)
	CFileName* theFileName = NULL;
	TRAPD(err, theFileName = CFileName::NewL());
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CFileName::NewL left with code %d"), err);
		return iTestStepResult = EInconclusive;
		}
	iFileName = theFileName;
	return iTestStepResult = EPass;
	}

// postamble
void RTSUMmTsthStep03::Close()
	{
	delete iFileName;
	}

