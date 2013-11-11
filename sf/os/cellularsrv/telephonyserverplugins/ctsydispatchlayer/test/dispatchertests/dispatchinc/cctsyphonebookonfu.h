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
 @file The TEFUnit header file which tests the PhonebookOn
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYPHONEBOOKONFU_H
#define CCTSYPHONEBOOKONFU_H

#include <test/tefunit.h>
#include <etelmm.h>
#include <etelmmcs.h>
#include "cctsyphonebookfu.h"

_LIT(KText, "Number%d");											
_LIT(KNumber, "0000000%d");
_LIT(KText1, "Number1");											
_LIT(KNumber1, "11111111");

class CCTsyPhonebookOnFU : public CCTsyPhonebookFU
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// use-case tests
	void TestUseCase0001L();
	void TestUseCase0002L();
	void TestUseCase0003L();
	
	//other unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();
	void TestUnit0005L();
	void TestUnit0006L();
	void TestUnit0007L();
	
private:
	
	void GenerateONList3L(CMobilePhoneONList* aONList);
	void StoreAllL(RMobileONStore& aOnStore);
	void ReadAllL(RMobileONStore& aOnStore);

	}; // class CCTsyPhonebookOnFU

#endif // CCTSYPHONEBOOKONFU_H

