// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES
#include <f32file.h>
#include <bacline.h>
#include <e32std.h>

#include "epos_cpospsycrtesthandler.h"

// FUNCTION PROTOTYPES
void StartPSYCRTesterL();

// Panic Function
GLDEF_C void Panic(TInt aErrorCode)
	{
	_LIT(KPSYTester, "PSY Tester CR Tester");
	User::Panic(KPSYTester, aErrorCode);
	}
	
	
// Main function called by E32
GLDEF_C TInt E32Main()
    {
	    __UHEAP_MARK;
	    
		CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
		
		TRAPD(error, StartPSYCRTesterL());
		
		__ASSERT_ALWAYS(!error, Panic(error));
		
		delete cleanup; // destroy clean-up stack

		__UHEAP_MARKEND;
	
       return 0;
    }
    
    
// PSYCRTester is an application that tests the
// capabilities specified in the PSY CR file.
// This is used by PSY Tester to ensure that the
// PSY CR is protected with "ReadUserData" and 
// "WriteDeviceData" only

void StartPSYCRTesterL() 
	{

	// Read the Process Arguments
	TInt psyIDValue;

    // PSY Tester sends the UID of the PSY as an argument
	CCommandLineArguments *args = CCommandLineArguments::NewL();
	CleanupStack::PushL(args);
	if(args->Count() <= 1)
	{
			User::Leave(KErrArgument);
	}
	TPtrC ptr = args->Arg(1);
	
	TLex lex(ptr.Ptr());
	TInt err = lex.Val(psyIDValue);

	if(KErrNone == err)
	{
		
		CPosPSYCRTestHandler* psyCRTestHandler = CPosPSYCRTestHandler::NewL();
		CleanupStack::PushL(psyCRTestHandler);

		TUid psyID;
		psyID.iUid = psyIDValue;
	
	    // Start the test	
		psyCRTestHandler->StartPSYCRTesterL(psyID);
		
		CleanupStack::PopAndDestroy(psyCRTestHandler);
	}
	
	CleanupStack::PopAndDestroy(args);
		
	RProcess::Rendezvous(KErrNone);

	}
	



