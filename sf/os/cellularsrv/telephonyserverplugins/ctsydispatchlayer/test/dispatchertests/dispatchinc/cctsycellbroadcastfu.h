// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// functional unit of the Common TSY Dispatcher.
//



/**
 @file The TEFUnit header file which tests the CellBroadcast
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYCellBroadcastFU_H
#define CCTSYCellBroadcastFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class RMmCustomAPI;

class CCTsyCellBroadcastFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// use-case tests
	void TestUseCase0001L();
	void TestUseCase0002L();	
	
	//other unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();	
	
private:	
	void InitSimCbTopicsL(RMmCustomAPI& aCustomApi, const TDesC& aTopic, TInt aNum);	
	}; // class CCTsyCellBroadcastFU

#endif // CCTSYCellBroadcastFU_H

