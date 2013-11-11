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
// functional unit of the SAT.
//



/**
 @file The TEFUnit header file which tests the LanguageNotification
*/

#ifndef CCSATLANGUAGENOTIFICATIONFU_H
#define CCSATLANGUAGENOTIFICATIONFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatLanguageNotificationFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyLanguageNotificationPCmd0001L();
	void TestNotifyLanguageNotificationPCmd0002L();
	void TestNotifyLanguageNotificationPCmd0004L();

	static void PrepareTlv(TTlv& aTlv, TUint8 aCmdQualifier, const TDesC8& aLang = KNullDesC8);

private:	

	void PrepareDispatchL();
	void CompleteDispatchL(const TDesC8& aTlvDsc, TInt aRes = KErrNone);
	
	}; // class CCSatLanguageNotificationFU

#endif // CCSATLANGUAGENOTIFICATIONFU_H

