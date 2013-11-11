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
// This contains PsdAgt TestCase Test 5.1 - 5.3
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

// RPacketContext
#include <etelpckt.h> 

// COMMDB Database Undo
#include "DbUndo.h"

// Class definitions
#include "PsdAgtTestLogging.h"


// Test case 5.1,  Wait for GetDataVolumeRequest
enum TVerdict CPsdAgtTest5_1::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(501);

	ConnectAndWaitForAsyncronousRequestL();
	
	// GetDataVolumeRequest should only have been called twice
	// ie. iStage == 3
	TESTL(iStage == 3);

	Log(_L("Check LogDataTransferred values in GENCONN.TXT"));

	return iTestStepResult;
	}


void CPsdAgtTest5_1::PsdAgtGetDataVolumeRequest(TUint32 &aSent, TUint32 &aReceived)
	{
	// Volume request should only be called twice
	TEST(iStage == 1 || iStage == 2);

	// First time data volume is requested
	if(iStage == 1)
		{
		iStage++;
		aSent = 100;     // Sent size
		aReceived = 101; // Rcv size

		// Unblock ConnectAndWaitForAsyncronousRequest()
		CActiveScheduler::Stop();
		}
	// Second time data volume is requested during the Disconnect
	else if(iStage == 2)
		{
		iStage++;
		aSent = 100;     // Sent size
		aReceived = 132; // Rcv size
		}
	}

// Test case 5.2,  Wait sufficient time for GetDataVolumeRequest
enum TVerdict CPsdAgtTest5_2::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(502);


	ConnectAndWaitForEightySecondsL();

	Log(_L("Check LogDataTransferred values in GENCONN.TXT"));

	return iTestStepResult;
	}


void CPsdAgtTest5_2::PsdAgtGetDataVolumeRequest(TUint32&, TUint32&)
	{
	// should be asking the TSY, not NIFMAN
	TEST(EFalse);
	}

// Test case 5.3,  Wait sufficient time for GetDataVolumeRequest
enum TVerdict CPsdAgtTest5_3::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(503);

	ConnectAndWaitForEightySecondsL();

	Log(_L("Check LogDataTransferred values in GENCONN.TXT"));

	return iTestStepResult;
	}


void CPsdAgtTest5_3::PsdAgtGetDataVolumeRequest(TUint32&, TUint32&)
	{
	// should be asking the TSY, not NIFMAN
	TEST(EFalse);
	}

