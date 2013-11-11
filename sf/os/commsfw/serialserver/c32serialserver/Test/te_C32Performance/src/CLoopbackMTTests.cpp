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
// Perform read/write operations through Loopback Csy. 
// 
//

/**
 @file
 @internalComponent
*/

#include "CLoopbackMTTests.h"

CLoopbackMTTests::CLoopbackMTTests(TThreadData& aTThreadData):CPerformTestTasks(aTThreadData)
	{
	
	}
TInt CLoopbackMTTests::InitL()
	{
 	_LIT(KCsyName,"LOOPBACK.CSY");
    _LIT(KPortName0,"LOOPBACK::0");
    _LIT(KPortName1,"LOOPBACK::1");
     
	//Creating a RcommSession, connecting to the C32 server,open ports
	// for read,write	
	TInt ret = InitializeCommModuleL(KCsyName,KPortName0,KPortName1);

	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
		return 0;
		}
	 
  	return 0;   
	}
TInt CLoopbackMTTests::PerformTask()
/**
 * @return - TInt
 * This function is used to record the time taken to send 
 * some chunk of data through Loopback CSY. 
*/
    {
  
  	 const TInt loopSize = 200;
  	 const TInt stringLength = 16;
  	 
  	 _LIT8(KServerText,"Test Loopback : ");
  	
  	 TInt res =0;
  	 TRequestStatus stat0;
  	 
  	 TBuf8<stringLength> writeBuf;
	 TBuf8<stringLength> readBuf;
  	 
  	 TTime startTime;
   	 startTime.UniversalTime();
		 	 
	 writeBuf.Append(KServerText);
	
	 //perform write operation
	 for (TInt i=0;i<=loopSize;i++)
		{
		
		iPort1->Write(stat0,writeBuf);
		User::WaitForRequest(stat0);
		res = stat0.Int();
		if(res!=KErrNone)
			{
			iTThreadData.iTestResult =  __LINE__ - 2;
			break;
			}
		//perform read operation
		iPort2->ReadOneOrMore(stat0,readBuf);
		User::WaitForRequest(stat0);
		res = stat0.Int();
	  	if(res!=KErrNone)
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
		 
  	return 0; 
		
	}

