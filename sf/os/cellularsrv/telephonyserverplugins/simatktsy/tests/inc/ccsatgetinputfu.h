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
 @file The TEFUnit header file which tests the GetInput
*/

#ifndef CCSATGETINPUTFU_H
#define CCSATGETINPUTFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"


class CCSatGetInputFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	//
	void TestNotifyGetInputPCmd0001L();
	void TestNotifyGetInputPCmd0001bL();
	void TestNotifyGetInputPCmd0002L();
	void TestNotifyGetInputPCmd0004L();

private:
	
	void TestGetInputSuccessfulWithoutTerminalResponseL(
			TUint8 aPCmdNumber,
			TUint8 aGetInputQualifier,
			const TDesC8& aGetInputText,
			TUint8 aMinRspLength,
			TUint8 aMaxRspLength);

	void TestGetInputWithUnsuccessfulTerminalResponseL(
			TUint8 aPCmdNumber,
			RSat::TPCmdResult aError,
			const TDesC& aMeProblemAddInfo = KNullDesC,
			const TInt aExpResult = KErrNone);

	void GenerateTerminalResponseL(
			TUint8 aCommandQualifier,
			const RSat::TGetInputRspV1& aResp,
			TUint8 aCodingScheme = K8BitDCS,
			const TInt aExpResult = KErrNone);

	}; // // class CCSatGetInputFU

#endif // CCSATGETINPUTFU_H

