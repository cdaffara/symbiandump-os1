// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TFBSMULTITHREAD_H
#define TFBSMULTITHREAD_H

/**
* Enums for passing between the test thread and the second thead used in testing,
* used to define which test should be run on the second thread.
*/
enum TFbsMultiThreadTestCase
	{
	EFbsSecondThreadInvalidHandleInIpcCall,
	};

/**
 * Class for passing test info to other threads.
 */	
struct TFbsTestThreadInfo 
	{
	TFbsMultiThreadTestCase iTestCase;
	};

#endif // TFBSMULTITHREAD_H
