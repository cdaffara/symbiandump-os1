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
#include "TSU_MmTsthStep11.h"
#include "TSU_MmTsthSuite11.h"

// preamble
TVerdict RTSUMmTsthStep11::OpenL()
	{
	// initialise iStepStub for tests. if this fails, tests are
	// inconclusive (will not run)
	RTestStepVirtualStub* theStepStub = NULL;
	TRAPD(err, theStepStub = RTestStepVirtualStub::NewL());
	if(err != KErrNone)
		{
		ERR_PRINTF1(_L("RTestStepVirtualStub::NewL() left"));
		return iTestStepResult = EInconclusive;
		}

	iStepStub = theStepStub;
	// set its suite to the suite of this step - so that it can do logging
	iStepStub->SetSuite(iSuite);

	return iTestStepResult = EPass;
	}

// postamble
void RTSUMmTsthStep11::Close()
	{
	delete iStepStub;
	}
