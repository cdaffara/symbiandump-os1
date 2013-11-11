/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Base class for all multithreaded tests. 
* 
*
*/



/**
 @file 
 @internalComponent
*/

#ifndef PERFORMTESTTASKS_H
#define PERFORMTESTTASKS_H

#include "Te_C32PerformanceSuiteStepBase.h"
#include "C32COMM.H"
#include <e32cmn.h>

class CPerformTestTasks:public CBase
/*
Each MT test case used should be derived from CPerformTestTasks.
*/
   {
public:
   
   CPerformTestTasks(TThreadData& aTThreadData);
   virtual ~CPerformTestTasks();
   //this function can be used to load CSY
   virtual TInt InitL() = 0;
   //this function can be used to perform read/write operation through CSY   
   virtual TInt PerformTask() = 0; 
   //initialize comm ports
   virtual TInt InitializeUsbModuleL(const TDesC& aPortName0);
   virtual TInt InitializeCommModuleL(const TDesC& aCsyName,const TDesC& aPortName0);
   virtual TInt InitializeCommModuleL(const TDesC& aCsyName,const TDesC& aPortName0,const TDesC& aPortName1);
   virtual TInt ClosePorts();

   TThreadData& iTThreadData;
   
   RCommServ *iCommServ; 
   RComm *iPort1;
   RComm *iPort2; 
   
	};

#endif





