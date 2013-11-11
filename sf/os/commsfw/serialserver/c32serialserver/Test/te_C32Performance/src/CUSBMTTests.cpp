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
// Perform read/write operations through ACM Csy.
// 
//

/**
 @file 
 @internalComponent
*/

#include "CUSBMTTests.h"

CUSBMTTests::CUSBMTTests(TThreadData& aTThreadData):CPerformTestTasks(aTThreadData)
	{
	
	}
TInt CUSBMTTests::InitL()
	{

	_LIT(KPortName0,"ACM::0");
	TRequestStatus status;
	
 	TInt ret = iUSBManager.Connect();
 	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
		return 0;
		}
		
	//start the USB Manager
	iUSBManager.Start(status);
	User::WaitForRequest(status);
	ret = status.Int();
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
		return 0;
		}
	
	//creating a RcommSession, connecting to the C32 server,open ports
	ret = InitializeUsbModuleL(KPortName0);
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
		return 0;
		}
			
	TBuf8<16> buffer;
	TRequestStatus readstatus;

	TInt res =0;
		
	// Read any data available into the buffer.
	// Handshaking - In wait state until data is sent by PC
	while(1)
		{
		iPort1->ReadOneOrMore(readstatus, buffer);
		User::WaitForRequest(readstatus);
			
		res = readstatus.Int();
		if(res==KErrNone)
			{
			break;
			}	
		}

  	return 0;   
	}
TInt CUSBMTTests::PerformTask()
/**
 * @return - TInt
 * This function is used to record the time taken to send 
 * 1 MB of data through ECACM CSY. 
*/
    {
    /*
      send 1 MB (1048576 bytes) of data through ECACM Csy.
      16384 * 64 = 1048576 bytes
    */
  	 const TInt loopSize     = 16384;
  	 const TInt stringLength = 64 ;
  	 
  	 //send 1mb of data through ACM Csy.
  	 _LIT8(KServerText,"Test Loopback : 1 mb of data  Test Loopback : 1 mb of data  Test");
  	
  	 TInt res =0;
  	 TRequestStatus status;
  	 
  	 TBuf8<stringLength> writeBuf;
  	 writeBuf.Append(KServerText);
	  	 
  	 TTime startTime;
   	 startTime.UniversalTime();
		 
	 for (TInt i=0;i<=loopSize;i++)
		 {
		 // write data to port
		 iPort1->Write(status, writeBuf);
		 User::WaitForRequest(status);
			
		 res = status.Int();
		 if(res!=KErrNone)
			{
			iTThreadData.iTestResult = __LINE__ - 2;
			break;
			}
		 }	 	 
	
  	 TTime stopTime;
 	 stopTime.UniversalTime();
		
	 iTThreadData.iThreadTime = stopTime.MicroSecondsFrom(startTime).Int64();
	
	 //close ports,RCommServ.
	 ClosePorts();
	  
	 iUSBManager.Stop(status);
	 User::WaitForRequest(status);
	 res = status.Int();
	 if(res!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
		return 0;
		}
  		 
  	return 0; 
	}

