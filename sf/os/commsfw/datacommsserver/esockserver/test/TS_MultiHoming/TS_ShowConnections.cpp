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
// This contains TS_MultiHoming RConnection Stop primitive.
// RConnection starts opens and starts a connection, and 
// stores this connection in C
// 
//

/**
 @file
*/
 
#include "TS_ShowConnections.h"

/* 
 * Test 1.1
 * Open a default / named connection
 */
CTS_ShowConnections::CTS_ShowConnections()
	{
	iTestStepName = KShowConnections;
	}

CTS_ShowConnections::~CTS_ShowConnections()
	{	
	}

enum TVerdict CTS_ShowConnections::doTestStepPreambleL(void)
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KShowConnections,KOomTest,iIsOOMTest)))
		iIsOOMTest=EFalse;
	return EPass;
	}


enum TVerdict CTS_ShowConnections::doTestStepL(void)

	{
	int expectedNumberOfIfs = 0;
	int err = 
		GetIntFromConfig(KShowConnections, KExpectedNumberOfIfs, expectedNumberOfIfs);

	// No config option so do nothing
	if(err == 0)
		{
		return EInconclusive;
		}
	
	int numberOfInterfaces = iOwnerSuite->DisplayConnectionDetailsL();
	// Less than zero is for display purposes only
	if(expectedNumberOfIfs < 0 || numberOfInterfaces == expectedNumberOfIfs)
		{
		return EPass;
		}
	return EFail;
	}


