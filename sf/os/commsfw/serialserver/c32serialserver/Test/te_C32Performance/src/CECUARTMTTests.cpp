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
// Perform read/write operations through ECUART Csy.
// 
//

/**
 @file 
 @internalComponent
*/

#include "CECUARTMTTests.h"

CECUARTMTTests::CECUARTMTTests(TThreadData& aTThreadData):CPerformTestTasks(aTThreadData)
	{
	
	}
TInt CECUARTMTTests::InitL()
	{
 
	_LIT(KCsyName,"ECUART");
	_LIT(KPortName0, "COMM::0");
	
	TInt ret = InitializeCommModuleL(KCsyName,KPortName0);
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
		return 0;
		}

  	return 0;   
	}
TInt CECUARTMTTests::PerformTask()
/**
 * @return - TInt
 * This function is used to record the time taken to send 
 * 1 MB of data through ECUART CSY. 
*/
    {
  	/*
      send 1 MB (1048576 bytes) of data through ECACM Csy.
      16384 * 64 = 1048576 bytes
    */
  	const TInt loopSize 	= 16384;
  	const TInt stringLength = 64 ;
  	 
  	_LIT8(KServerText,"Test Loopback : 1 mb of data  Test Loopback : 1 mb of data  Test");

  	TInt res =0;
	TRequestStatus status1;
		
	TBuf8<stringLength> writeBuf;
	writeBuf.Append(KServerText);

	TTime startTime;
   	startTime.UniversalTime();
   	 
	for (TInt i=0;i<=loopSize;i++)
		{
		iPort1->Write(status1,writeBuf);
		User::WaitForRequest(status1);
		
		res = status1.Int();
		if(res!=KErrNone)
			{
			iTThreadData.iTestResult = __LINE__ - 2;
			break;
			}
		}
	 
	 TTime stopTime;
 	 stopTime.UniversalTime();
		
	 iTThreadData.iThreadTime = stopTime.MicroSecondsFrom(startTime).Int64();
	
  	return 0; 
		
	}

