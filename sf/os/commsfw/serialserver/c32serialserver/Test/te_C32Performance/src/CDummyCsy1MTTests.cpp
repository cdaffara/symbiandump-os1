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
// Perform read/write operations through DummyCsy1.
// 
//

/**
 @file
 @internalComponent
*/
#include "CDummyCsy1MTTests.h"

CDummyCsy1MTTests::CDummyCsy1MTTests(TThreadData& aTThreadData):CPerformTestTasks(aTThreadData)
	{
	
	}
TInt CDummyCsy1MTTests::InitL()
	{
  	_LIT(KCsyName,"DummyCsy1");
	_LIT(KPortName0,"DummyCsy1::6");
	_LIT(KPortName1,"DummyCsy1::0");
	
   	//Creating a RcommSession, connecting to the C32 server,open ports
	//for read,write		

	TInt ret = InitializeCommModuleL(KCsyName,KPortName0,KPortName1);

	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult =  __LINE__ - 2;
		return 0;
		} 
	
	return 0;       

	}
TInt  CDummyCsy1MTTests::PerformTask()
/**
 * @return - TInt
 * This function is used to record the time taken to send 
 * some chunk of data through DummyCsy1. 
*/
	{
 	 const TInt loopSize     = 50;
 	 const TInt stringLength = 82;
 	
	_LIT8(KServerText,"This string has to be exactly like this othervice write returns KErrCommsLineFail.");

  	TRequestStatus stat0;
	TRequestStatus stat1;
	
	TInt ret =0;
	TBuf8<stringLength> outbuf;
	TBuf8<stringLength> inbuf;
	
	TTime startTime;
	startTime.UniversalTime();
	
	//perform read/write operations
	for (TInt i=0;i<=loopSize;i++)
		{
		outbuf.Copy(KServerText);
	
		iPort1->Write(stat0,5000000,outbuf);
		User::WaitForRequest(stat0);
		ret = stat0.Int();
		if(ret != KErrNone)
		   {
		   iTThreadData.iTestResult =  __LINE__ - 2;
		   break;
		   }
		
		iPort2->Read(stat1,inbuf);
		User::WaitForRequest(stat1);
		ret = stat1.Int();
		
		if(ret != KErrNone)
		   {
		   iTThreadData.iTestResult =  __LINE__ - 2;
		   break;
		   }
		 }

		TTime stopTime; 
		stopTime.UniversalTime();
		
		iTThreadData.iThreadTime = stopTime.MicroSecondsFrom(startTime).Int64();
		
		//close ports,RCommServ.	
  	    ClosePorts();
  	    
  	    //DummyCsy needs sometime to shutdown
  	    User::After(2000000);
  	   
 	return 0;    
 	}

