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
// functional unit of the Common TSY.
// 
//

/**
 @file The TEFUnit header file which tests the MBMS Context
*/

#ifndef CCTSYMBMSCONTEXTFU_H
#define CCTSYMBMSCONTEXTFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>
#include <pcktlist.h>

#include <pcktcs.h>
#include "pcktretrieve.h"

#include "cctsycomponenttestbase.h"

const TUint KMmConfigTMGI1 = 1;
const TInt  KMmConfigTmgiMMC1 = 1;
const TInt  KMmConfigTmgiMNC1 = 1;
const TUint KMmConfigTMGI2 = 2;
const TInt  KMmConfigTmgiMMC2 = 2;
const TInt  KMmConfigTmgiMNC2 = 2;


class CCTsyMbmsContextFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

    void TestOpenNewContext0001L();
    void TestInitialiseContext0001L();    
    void TestInitialiseContext0002L();
    void TestInitialiseContext0003L();
    void TestInitialiseContext0004L();
    void TestInitialiseContext0005L();
    void TestSetConfig0001L();
    void TestSetConfig0002L();
    void TestSetConfig0003L();
    void TestSetConfig0004L();
    void TestSetConfig0005L();
    void TestActivate0001L();
    void TestActivate0002L();
    void TestActivate0003L();
    void TestActivate0004L();
    void TestActivate0005L();
    void TestDeactivate0001L();
    void TestDeactivate0002L();
    void TestDeactivate0004L();
    void TestDeactivate0005L();
    void TestDelete0001L();
    void TestDelete0004L();
    void TestDelete0005L();
    void TestClose0001L();
    void TestGetConfig0001L();
    void TestGetConfig0002L();
    void TestGetConfig0003L();
    void TestGetStatus0001L();
    void TestGetLastErrorCause0001L();
    void TestNotifyConfigChanged0001L();
    void TestNotifyConfigChanged0002L();
    void TestNotifyConfigChanged0003L();
    void TestNotifyConfigChanged0004L();
    void TestNotifyStatusChange0001L();
    void TestNotifyStatusChange0002L();
    void TestNotifyStatusChange0003L();


private:
	static void CleanupArrayPtrFlat(TAny* self);	

	}; // class CCTsyMbmsContextFU
        

#endif // CCTSYMBMSCONTEXTFU_H


