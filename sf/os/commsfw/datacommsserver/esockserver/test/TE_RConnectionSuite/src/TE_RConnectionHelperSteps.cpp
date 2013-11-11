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
// Contains the implementation of the helper steps (initialisation and waiting
// for esock to die)
// 
//

/**
 @file
*/

#include "TE_RConnectionHelperSteps.h"
//

#include <e32base.h>
#include <e32std.h>
#include <c32comm.h>
#include <e32hal.h>
#include <comms-infras/nifif.h>
#include <comms-infras/nifagt.h>
#include <c32root.h>
#include <comms-infras/esockloader.h>
//

TE_RConnectionForceCCoverWrite::TE_RConnectionForceCCoverWrite(TPtrC /*aName*/) : TE_RConnectionStep()
{
	// TODO CPC iTestStepName=aName;
}

TE_RConnectionForceCCoverWrite::~TE_RConnectionForceCCoverWrite()
{
}

enum TVerdict TE_RConnectionForceCCoverWrite::doTestStepL(void)
/*
 * It appears that CCover only writes to its data file every now and again, so force it here 
 * to write its data just before we shutdown and lose anything that has changed
 */
{
	#ifdef CCOVER
		cov_write();
	#endif

	return TestStepResult();
} // TE_RConnectionForceCCoverWrite

