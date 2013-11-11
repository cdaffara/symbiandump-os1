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
 @file The TEFUnit header file which tests the SendDTMF
*/

#ifndef CCSATSENDDTMFFU_H
#define CCSATSENDDTMFFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatSendDTMFFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifySendDtmfPCmd0001L();
	void TestNotifySendDtmfPCmd0001bL();
	void TestNotifySendDtmfPCmd0002L();
	void TestNotifySendDtmfPCmd0004L();

private:
	
	void PrepareSendDtmfCompleteDataL(
			TUint8 aPCmdNumber,
			TUint8 aCmdQual,
			const TDesC8* aDtmf = NULL,
			const TDesC8* aAlpha = NULL,
			RSat::TIconQualifier aIconQual = RSat::EIconQualifierNotSet,
			TInt aResult = KErrNone,
			TInt aDelay = KDefaultNoDelay,
			TUint8 aDestDeviceId = KNetwork);

	void TestNotifySendDtmfPCmdL(
			TUint8 aPCmdNumber,
			TUint8 aCmdQual,
			const TDesC8* aDtmf = NULL,
			const TDesC8* aAlpha = NULL,
			RSat::TIconQualifier aIconQual = RSat::EIconQualifierNotSet,
			TInt aResult = KErrNone,
			const TDesC16* aAlphaIdCheck = NULL);
	
	void GenerateSendDtmfTerminalResponseL(
			const RSat::TSendDtmfRspV1& aResp,
			TUint8 aCmdQual,
			TInt aExpResult = KErrNone);

	}; // class CCSatSendDTMFFU

#endif // CCSATSENDDTMFFU_H

