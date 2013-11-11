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
 @file The TEFUnit header file which tests the GetInkey
*/

#ifndef CCSATGETINKEYFU_H
#define CCSATGETINKEYFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatGetInkeyFU : public CCSatComponentTestBase
	{
public:
	
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyGetInkeyPCmd0001L();
	void TestNotifyGetInkeyPCmd0002L();
	void TestNotifyGetInkeyPCmd0004L();
	
	void TestNotifyGetInkeyPCmd0001bL();

private:

	struct TData
		{
		struct TIcon
			{
			TIcon(); 
			void Clear();

			TBool iIfUse;
			TUint8 iQualifier;
			TUint8 iIdentifier;
			};
		
		TIcon iIcon;
		TUint8 iCommandQualifier;
		TUint8 iCodingScheme;
		TBuf8<RSat::KGetInkeyTextMaxSize> iText;
		};	

	void PrepareTlvData ( TTlv& aTlvData );

	void PrepareExpectDataL ();
	
	void PrepareCompleteDataL ( TInt aResultCode = KErrNone );

	void ProactiveCommandCallL( RSat::TGetInkeyV2& aGetInkeyData,
			                    TInt aCompleteErrorCode = KErrNone, 
			                    TInt aResultCode = KErrNone );

	void DataInitialization(
			const TData::TIcon& aIcon,
			const TDesC8& aText = KNullDesC8,
			TUint8 aCommandQualifier = 0,
			TUint8 aCodingScheme = K8BitDCS);

	void TerminalResponseCallL(
			const RSat& aSat,
			RSat::TPCmdResult aGeneralResult = RSat::KSuccess,
			TUint aInfoType = RSat::KNoAdditionalInfo,
			const TDesC8& aText = KNullDesC8,
			TUint8 aCodingScheme = K8BitDCS,
			TInt aCodeError = KErrNone,
			RSat::TGetInkeyRspFormat aRspFormat = RSat::EDigitOnly);
	
	void DataCompareL( const RSat::TGetInkeyV2& aGetInkeyData );

	TData iData;

	}; // class CCSatGetInkeyFU

#endif // CCSATGETINKEYFU_H

