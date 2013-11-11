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
* Perform read/write operations through Dummy Csy.
* 
*
*/



/**
 @file 
 @internalComponent
*/

#ifndef DUMMYMTTESTS_H
#define DUMMYMTTESTS_H

#include "CPerformTasks.h"
#include "C32COMM.H"
#include <e32cmn.h>

class CDummyMTTests:public CPerformTestTasks 
	{
     
public:
   
   CDummyMTTests(TThreadData& aTThreadData);
   virtual TInt InitL();
   virtual TInt PerformTask();
   
       	
   };
#endif

