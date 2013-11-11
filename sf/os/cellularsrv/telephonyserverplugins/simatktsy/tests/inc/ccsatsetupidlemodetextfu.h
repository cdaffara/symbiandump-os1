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
 @file The TEFUnit header file which tests the SetUpIdleModeText
*/

#ifndef CCSATSETUPIDLEMODETEXTFU_H
#define CCSATSETUPIDLEMODETEXTFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatSetUpIdleModeTextFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifySetUpIdleModeTextPCmd0001L();
	void TestNotifySetUpIdleModeTextPCmd0001bL();
	void TestNotifySetUpIdleModeTextPCmd0002L();
	void TestNotifySetUpIdleModeTextPCmd0004L();
	
private:
	
	void PrepareSetUpIdleModeTextCompleteDataL(
			TUint8 aPCmdNumber,
			RSat::TSetUpIdleModeTextType aType = RSat::ESetUpIdleModeTextTypeNotSet,
			const TDesC8& aText = KNullDesC8,
			TUint8 aCodingScheme = K8BitDCS,
			RSat::TIconQualifier aIconQual = RSat::EIconQualifierNotSet,
			TUint8 aIcon = 0,
			TInt aResult = KErrNone,
			TInt aDelay = 1);

	void TestNotifySetUpIdleModeTextPCmdL(
			TUint8 aPCmdNumber,
			RSat::TSetUpIdleModeTextType aType,
			const TDesC8& aText = KNullDesC8,
			TUint8 aCodingScheme = K8BitDCS,
			RSat::TIconQualifier aIconQual = RSat::EIconQualifierNotSet,
			TUint8 aIcon = 0,
			TInt aResult = KErrNone,
			const TDesC16* aTextCheck = NULL);
	
	void GenerateSetUpIdleModeTextTerminalResponseL(const RSat::TSetUpIdleModeTextRspV1& aResp, TInt aExpResult = KErrNone);

	}; // class CCSatSetUpIdleModeTextFU

#endif // CCSATSETUPIDLEMODETEXTFU_H

