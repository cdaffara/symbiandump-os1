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
// Base class for all multithreaded tests. 
// 
//

/**
 @file 
 @internalComponent
*/

#include "CPerformTasks.h"

CPerformTestTasks::CPerformTestTasks (TThreadData& aTThreadData) : iTThreadData (aTThreadData) 
   	{   
   	}
CPerformTestTasks::~CPerformTestTasks () 
   	{
   	delete iPort1;
  	delete iPort2;
  	delete iCommServ;   	
  	}

TInt CPerformTestTasks::InitializeCommModuleL(const TDesC& aCsyName,const TDesC& aPortName0,const TDesC& aPortName1)
	{
	//Creating a RcommSession, connecting to the C32 server	
   	
   	iCommServ = new (ELeave)RCommServ();
   	CleanupStack::PushL(iCommServ);
   	
   	iPort1 = new (ELeave)RComm();
	CleanupStack::PushL(iPort1);
	
	iPort2 = new (ELeave)RComm();
	CleanupStack::PushL(iPort2);
   	
   	TInt ret=iCommServ->Connect();
	RDebug::Printf("In CPerformTestTasks.connect RCommServ = %d",ret); 

	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult =  __LINE__ - 2;
		return 0;
		}
	//load comm module		
	ret=iCommServ->LoadCommModule(aCsyName);
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult =  __LINE__ - 2;
		return 0;
		}
		
	RDebug::Printf("In CPerformTestTasks. Load comm module = %d",ret);	
	
	//Open port for reading and writing
	ret=iPort1->Open(*iCommServ,aPortName0,ECommShared, ECommRoleDTE);
	RDebug::Printf("In CPerformTestTasks.Open the port for reading, = %d",ret); 
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult =  __LINE__ - 2;
		return 0;
		}
		
	ret=iPort2->Open(*iCommServ,aPortName1,ECommShared, ECommRoleDTE);
	RDebug::Printf("In CPerformTestTasks.Open the port for writing = %d",ret); 
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult =  __LINE__ - 2;
		return 0;
		} 	
	
	CleanupStack::Pop(iPort2);
	CleanupStack::Pop(iPort1);
	CleanupStack::Pop(iCommServ);
	
	return 0;	
	}
TInt CPerformTestTasks::InitializeCommModuleL(const TDesC& aCsyName,const TDesC& aPortName0)
	{
	//Creating a RcommSession, connecting to the C32 server	
   	
   	iCommServ = new (ELeave)RCommServ();
   	CleanupStack::PushL(iCommServ);
   	
   	iPort1 = new (ELeave)RComm();
	CleanupStack::PushL(iPort1);
   	
   	TInt ret=iCommServ->Connect();
	RDebug::Printf("In CPerformTestTasks.connect RCommServ = %d",ret); 

	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult =  __LINE__ - 2;
		return 0;
		}
	
	ret=iCommServ->LoadCommModule(aCsyName);
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
			return 0;
		}
	
	ret= iPort1->Open(*iCommServ, aPortName0, ECommShared);
		if(ret!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
			return 0;
		}
		
	TCommConfig configBuf;
	TCommConfigV01& config = configBuf();
	iPort1->Config(configBuf);
	config.iFifo=EFifoEnable;
	config.iRate=EBps115200;
	config.iHandshake=KConfigFailDSR | KConfigFailCTS;
	config.iTerminatorCount=0;
	config.iDataBits=EData8;
	config.iParity=EParityNone;
	config.iStopBits=EStop1;
	ret= iPort1->SetConfig(configBuf);
	
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
		return 0;
		}
	
	CleanupStack::Pop(iPort1);
	CleanupStack::Pop(iCommServ);
	return 0;	
	}
TInt CPerformTestTasks::InitializeUsbModuleL(const TDesC& aPortName0)
	{
	//Creating a RcommSession, connecting to the C32 server	
   	
   	iCommServ = new (ELeave)RCommServ();
   	CleanupStack::PushL(iCommServ);
   	
   	iPort1 = new (ELeave)RComm();
	CleanupStack::PushL(iPort1);
   	
   	TInt ret=iCommServ->Connect();
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult =  __LINE__ - 2;
		return 0;
		}
	RDebug::Printf("In CPerformTestTasks.connect RCommServ = %d",ret);
	
	//Open port for reading and writing	
	ret=iPort1->Open(*iCommServ,aPortName0,ECommExclusive, ECommRoleDCE);
	RDebug::Printf("In CPerformTestTasks.Open the port for reading, = %d",ret); 
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult =  __LINE__ - 2;
		return 0;
		}
		
	// Set the port's configuration to known values.
	TCommConfig configBuf;
	TCommConfigV01& config = configBuf();
	config.iRate = EBps115200;
	config.iDataBits = EData8;
	config.iStopBits = EStop1;
	config.iParity = EParityNone;
	config.iHandshake = 0;
	config.iTerminatorCount = 0;
	ret= iPort1->SetConfig(configBuf);
	
	if(ret!=KErrNone)
		{
		iTThreadData.iTestResult = __LINE__ - 2;
		return 0;
		}	
	
	CleanupStack::Pop(iPort1);
	CleanupStack::Pop(iCommServ);
	return 0;	
	}
TInt CPerformTestTasks::ClosePorts()
	{
	//close comm ports
    if(iPort1) 
   		{
   		iPort1->Close();
   		}
   	if(iPort2) 
   		{
   		iPort2->Close();
   		}
   	if(iCommServ) 
   		{
   		iCommServ->Close();
		}
	RDebug::Printf("In CPerformTestTasks.Close all the ports");
	return 0;	
	}

