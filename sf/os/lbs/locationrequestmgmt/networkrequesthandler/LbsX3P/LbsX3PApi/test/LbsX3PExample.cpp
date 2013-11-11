// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cons.h>

#include <lbs.h>
#include <lbs/lbsx3p.h>
#include <e32test.h>

// Test
_LIT(KTestName, "X3P Test");
LOCAL_D RTest test(KTestName);

void mainL()
	{
	// An example of how to use the X3P API to send the current position to a third party.
	// Two requests are made simultaneously and then the test harness waits for both to complete. 
	// For the sake of simplicity, the example calls User::WaitForRequest rather than using
	// active objects.
	  
	test.Printf(_L("Transmit current position to both mother and sister\n"));
	RLbsTransmitPositionServer server;
	CleanupClosePushL(server);
	User::LeaveIfError(server.Connect());

	RLbsTransmitPosition transmitPositionToMother;
	
	
	CleanupClosePushL(transmitPositionToMother);
	User::LeaveIfError(transmitPositionToMother.Open(server));

	TLbsTransmitPositionOptions transmitPositionOptions(5000000);
	transmitPositionToMother.SetTransmitOptions(transmitPositionOptions);
	
	TRequestStatus transmitPositionToMotherStatus;
	_LIT(KMothersPhoneNumber,"01223987654");

	
	//create a TPositionInfo object to hold the position transmitted 
	TPositionInfo posInfo;
	
	// Send with priority of 6
	transmitPositionToMother.TransmitPosition(KMothersPhoneNumber, 6, transmitPositionToMotherStatus, posInfo );

	RLbsTransmitPosition transmitPositionToSis;
	CleanupClosePushL(transmitPositionToSis);
	User::LeaveIfError(transmitPositionToSis.Open(server));

	TRequestStatus transmitPositionToSisterStatus;
	_LIT(KSistersEmailAddress,"sister@relative.com");
	// This time send with priority of 7 - one higher than previous
	transmitPositionToSis.TransmitPosition(KSistersEmailAddress, 7, transmitPositionToSisterStatus, posInfo);

	// Wait for both to complete
	User::WaitForRequest(transmitPositionToMotherStatus);
	User::WaitForRequest(transmitPositionToSisterStatus);

    if ((transmitPositionToMotherStatus == KErrNone) && (transmitPositionToSisterStatus == KErrNone)) 
    	{
   		test.Printf(_L("Two positions successfully transmitted\n"));
    	}
    	else
    	{
	   	test.Printf(_L("Positions NOT successfully transmitted\n"));
	   	}
	   	
	CleanupStack::PopAndDestroy(3, &server);

	}


void doMainL()
	{
	TRAPD(error, mainL())
	if (error)
		test.Printf(_L("Error setting up the tests: %d\n"), error);	
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(KTestName);
		
	CActiveScheduler* rootScheduler = new CActiveScheduler;
	CActiveScheduler::Install(rootScheduler);
	CTrapCleanup* theCleanup=CTrapCleanup::New();

	TRAPD(ret,doMainL());	
	test.Printf(_L("\nTest Completion code: %d\n"), ret);	
	test(ret==KErrNone);
	
	delete theCleanup;	
	delete rootScheduler;
	
	test.End();
	test.Close();
		
	__UHEAP_MARKEND;
	return 0;
	}



//
// End of file
