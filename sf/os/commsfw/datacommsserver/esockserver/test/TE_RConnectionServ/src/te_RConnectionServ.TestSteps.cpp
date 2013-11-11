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
// Contains the implementation of test cases covering the RConnectionServ API
// 
//

/**
 @file
*/

#include <ss_std.h>
#include "te_RConnectionServ.TestSteps.h"

const TInt KIpProtoTierId = 0x10281DF0;

#if 0 // TODO_CDG not needed. just for borrowing code from
enum TVerdict CTestStep_NET_RCONNECTION_SERV_04_030::doTestStepL(void)
{
	// Negative test on tier ID during connection
	// Check that invalid values of tier ID during RConnectionServ::Connect are handled as expected
	TInt err;
	RConnectionServ connServ;
	
	// Try to connect at all tiers
	err = connServ.Connect(0x0);
	TESTEL(err == KErrNotSupported, KErrGeneral);
	
	// Try to connect invalid tier
	err = connServ.Connect(0x0);
	TESTEL(err == KErrNotSupported, KErrGeneral);
	
	// Connect valid tier
	err = connServ.Connect(KNetworkTierId);
	TESTEL(err == KErrNotSupported, KErrGeneral);
	connServ.Close();

	// Try to connect invalid tier again
	err = connServ.Connect(0x0);
	TESTEL(err == KErrNotSupported, KErrGeneral);
	
	// Connect valid tier again
	err = connServ.Connect(0x0);
	TESTEL(err == KErrNotSupported, KErrGeneral);
	
	return TestStepResult();
}

enum TVerdict CTestStep_NET_RCONNECTION_SERV_04_040::doTestStepL(void)
{
	// Confirm that when connecting to the ipproto tier that this is not supported
	// This represents a special case in RConnectionServ support in that the ipproto tier is off limits
	TInt err;
	RConnectionServ connServ;
	
	// Try to connect
	err = connServ.Connect(KIpProtoTierId);
	TESTEL(err == KErrNotSupported, KErrGeneral);
	
	return TestStepResult();
}
#endif

enum TVerdict CTestStep_NET_RCONNECTION_SERV_04_000::doTestStepL(void)
{
	// Confirm that when connecting to the ipproto tier that this is not supported
	// This represents a special case in RConnectionServ support in that the ipproto tier is off limits
	TInt err;
	RConnectionServ connServ;
	
	// Connect to the connection serv instance
	err = connServ.Connect(KIpProtoTierId);
	TESTEL(err == KErrNotSupported, KErrGeneral);
	
	return TestStepResult();
}




