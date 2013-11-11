// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains PsdAgt TestCase Test 12.1 
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "networking/log.h"
#include "networking/teststep.h"
#include "TestStepPsdAgt.h"
#include "TestSuitePsdAgt.h"

// COMMDB Database Undo
#include "DbUndo.h"

// RPacketContext
#include <etelpckt.h> 

// EPsdStartingConfiguration
#include <psdprog.h>

// EConnectionOpen
#include <connectprog.h>

// Class definitions
#include "PsdAgtTestCloseLog.h"

// Test case 12.1
enum TVerdict CPsdAgtTest12_1::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1201);

	NormalConnectL();

	return iTestStepResult;
	}

void CPsdAgtTest12_1::AgentProgress(TInt aStage, TInt aError)
	{
	if(aStage == EPsdFinishedDeactivation && aError == KErrNone)
		TEST(EFalse);  // We shouldn't get this notification
	}
