// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed to test DevVideo.
// 
//

/**
 @file tsu_mmf_devvideociu.cpp
*/
#include "tsu_mmf_devvideociu.h"


/**
 * Test Preamble routines.
 *
 * Creates our own Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Preamble complete.
 */
TVerdict CTestDevVideoCiuStep::DoTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	
	return EPass;
	}

/**
 * Test Postamble routines.
 *
 * Destroys our Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Postamble complete.
 */
TVerdict CTestDevVideoCiuStep::DoTestStepPostambleL()
	{
	delete iActiveScheduler;
	iActiveScheduler = NULL;

	return EPass;
	}

/**
 * CTestDevVideoCiuStep Implementation
 */
CTestDevVideoCiuStep::CTestDevVideoCiuStep()
	:iActiveScheduler( NULL )
	{}

CTestDevVideoCiuStep::~CTestDevVideoCiuStep()
	{
	}


