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
 @file The TEFUnit header file which tests the DisplayText
*/

#ifndef CCSATDISPLAYTEXTFU_H
#define CCSATDISPLAYTEXTFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatDisplayTextFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyDisplayTextPCmd0001L();
	void TestNotifyDisplayTextPCmd0001bL();
	void TestNotifyDisplayTextPCmd0002L();
	void TestNotifyDisplayTextPCmd0004L();
	
private:
	
	void DisplayTextPCmdTlvBegin( 
			TTlv& aTlv, 
			TUint8 aPCmdNumber,
			TUint8 aCommandQualifier);

	void TestNotifyDisplayTextSuccessfulL(
			TUint8 aCommandQualifier,
			const TDesC8& aTextToDisplay,
			const RSat::TDisplayTextRspV1& aResp,
			const TInt aExpResult = KErrNone);

	void GenerateTerminalResponseL(
			TUint8 aCommandQualifier,
			const RSat::TDisplayTextRspV1& aResp,
			const TInt aExpResult = KErrNone);

	}; // class CCSatDisplayTextFU

#endif // CCSATDISPLAYTEXTFU_H

