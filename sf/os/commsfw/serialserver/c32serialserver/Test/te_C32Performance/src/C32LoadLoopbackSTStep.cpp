// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CTestStep derived implementation
// 
//

/**
 @file
 @internalComponent 
*/
#include "C32LoadLoopbackSTStep.h"
#include "Te_C32PerformanceSuiteDefs.h"


CC32LoadLoopbackSTStep::CC32LoadLoopbackSTStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KC32LoadLoopbackSTStep);
	}
	
CC32LoadLoopbackSTStep::~CC32LoadLoopbackSTStep()
/**
 * Destructor
 */
	{
	}

TVerdict CC32LoadLoopbackSTStep::doTestStepL()
/**
 * @return - TVerdict code
*/
	{
   
     const TInt loopSize     = 100;
  	 const TInt stringLength = 16;
  	 
  	 TInt res =0;
  	 TVerdict testResult = EPass; 
  	 TRequestStatus stat0;
  	 
  	 TBuf8<stringLength> writeBuf;
	 TBuf8<stringLength> readBuf;
    
    _LIT(KCsyName,"LOOPBACK.CSY");
    _LIT(KPortName0,"LOOPBACK::0");
    _LIT(KPortName1,"LOOPBACK::1");
    _LIT8(KServerText,"Test Loopback : ");
     
	//Creating a RcommSession, connecting to the C32 server,open ports
	// for read,write	
	TInt ret = InitializeCommModule(KCsyName,KPortName0,KPortName1);

	if(ret!=KErrNone)
		{
		return EFail;
		}
  	 
  	 TTime startTime;
   	 startTime.UniversalTime();
		 	 
	 writeBuf.Append(KServerText);
	
	 //perform write operation
	 for (TInt i=0;i<=loopSize;i++)
		{
		
		iPortList[0].Write(stat0,writeBuf);
		User::WaitForRequest(stat0);
		res = stat0.Int();
		if(res!=KErrNone)
			{
			testResult = EFail;
			break;
			}
		//perform read operation
		iPortList[1].ReadOneOrMore(stat0,readBuf);
		User::WaitForRequest(stat0);
		res = stat0.Int();
	  	if(res!=KErrNone)
			{
			testResult = EFail;
			break;
			}
	
		}

  	   	TTime stopTime;
		stopTime.UniversalTime();
		
		if (testResult == EFail)
			{
			INFO_PRINTF1(_L("Failed to do read/write operation"));
			return EFail;
			}
		else
			{
			INFO_PRINTF1(_L(" "));
			INFO_PRINTF2(_L(" Time taken to send data through Loopback CSY is = %ld microseconds "),stopTime.MicroSecondsFrom(startTime).Int64());
			INFO_PRINTF1(_L(" "));
			}
		
		//close ports,RCommServ.
		ClosePorts();
	 
  	
	return TestStepResult();
	}


