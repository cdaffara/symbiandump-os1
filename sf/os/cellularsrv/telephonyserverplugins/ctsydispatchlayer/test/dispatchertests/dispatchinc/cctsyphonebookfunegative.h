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
 @file The TEFUnit header file which tests the PhonebookEn
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYPHONEBOOKFUNEGATIVE_H
#define CCTSYPHONEBOOKFUNEGATIVE_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>

#include <ctsy/rmmcustomapi.h>

#include "cctsycomponenttestbase.h"

#include <ctsy/ltsy/cphonebookentry.h>

class CPhoneBookEntry;

class CCTsyPhonebookFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	//'negative' unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();
	void TestUnit0005L();
	void TestUnit0006L();
	void TestUnit0007L();
	void TestUnit0008L();
	void TestUnit0009L();
	void TestUnit0010L();
	void TestUnit0011L();
	void TestUnit0012L();
	void TestUnit0013L();
	void TestUnit0014L();
	void TestUnit0015L();
	
private:
	void OpenSmsStoreL(RMobileSmsMessaging& aSmsMessaging, RMobileSmsStore& aSmsStore, const TDesC& aSmsStoreName);
	void CreatePhonebookStoreDataL();

private:
	DispatcherPhonebook::TPhonebookStoreInfoV1 iPhonebookStoreData;

	}; // class CCTsyPhonebookFUNegative

#endif // CCTSYPHONEBOOKFUNEGATIVE_H

