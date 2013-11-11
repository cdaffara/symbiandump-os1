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
//

/**
 @file
 @test
 @internalComponent
*/

#include "t_wservintegstepfps.h"

/**
 CT_WServIntegStepFps constructor.
 @return N/A
 @pre None
 @post CT_WServIntegStepFps 1st stage constructor
*/
CT_WServIntegStepFps::CT_WServIntegStepFps()
	{
	SetTestStepName(KT_WServIntegStepFps);
	}

/**
Starts test step
@internalComponent
@return TVerdict pass / fail
@pre N/A
@post N/A
*/
enum TVerdict CT_WServIntegStepFps::doTestStepL()
	{
	__UHEAP_MARK;
	
	PrintTestStepName();

	TInt err=CreateTestProcessL(KWServFpsApp, EPriorityForeground);
	TEST(err==KErrNone);

	// Let the test step run for 10 secs before clean up
	User::After(10000000);
	CleanUp();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

