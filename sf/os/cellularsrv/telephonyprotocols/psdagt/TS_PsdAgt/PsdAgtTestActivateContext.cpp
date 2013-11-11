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
// This contains PsdAgt TestCase Test 10.1
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

// Class definitions
#include "PsdAgtTestActivateContext.h"

// Test case 10.1
enum TVerdict CPsdAgtTest10_1::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1001);
	NormalConnectL();

	return iTestStepResult;
	}

// Test case 10.2
enum TVerdict CPsdAgtTest10_2::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1002);

	ConnectExpectErrorL(KErrGeneral, EPsdFinishedActivation);

	return iTestStepResult;
	}

// Test case 10.3
enum TVerdict CPsdAgtTest10_3::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1003);
	ConnectExpectErrorL(KErrGeneral, EPsdFinishedActivation);

	return iTestStepResult;
	}
