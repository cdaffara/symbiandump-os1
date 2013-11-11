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
// TestStepMmfPerformance.cpp
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <TestFramework.h>

#include "TestStepMmfPfm.h"
#include "TestSuiteMmfPfm.h"


/**
OpenL is the preamble function for test steps.
Add a scheduler
@lib "TSU_MMF_PFM_01.lib"
*/
TVerdict RTestStepMmfPerformance::OpenL()
	{
	iScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install( iScheduler );
	
	return EPass;
	}

/**
The post amble step cleans up the allocated memory in the preamble
Remove the scheduler
@lib "TSU_MMF_PFM_01.lib"
*/

void RTestStepMmfPerformance::Close()
	{
	delete iScheduler; iScheduler = NULL;
	}

