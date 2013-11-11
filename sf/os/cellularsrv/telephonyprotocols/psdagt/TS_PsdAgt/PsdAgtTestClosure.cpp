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
// This contains PsdAgt TestCase Test 13.1 - 13.2
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
#include "PsdAgtTestClosure.h"

// Test case 13.1
enum TVerdict CPsdAgtTest13_1::doPsdAgtTestStepL( void )
	{
	// This test is really only here for code coverage purposes
	// It's difficult to actually test for any particular result
	// aside from a successful connection / disconnection
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1301);
	NormalConnectL();
	
	return iTestStepResult;
	}

// Test case 13.2
enum TVerdict CPsdAgtTest13_2::doPsdAgtTestStepL( void )
	{
	// This test is really only here for code coverage purposes
	// It's difficult to actually test for any particular result
	// aside from a successful connection / disconnection
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1302);

	NormalConnectL();

	return iTestStepResult;
	}
