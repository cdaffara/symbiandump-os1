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
 @file The TEFUnit header file which tests the PlayTone
*/

#ifndef CCSATPLAYTONEFU_H
#define CCSATPLAYTONEFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatPlayToneFU : public CCSatComponentTestBase
	{
public:

	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyPlayTonePCmd0001L();
	void TestNotifyPlayTonePCmd0002L();
	void TestNotifyPlayTonePCmd0004L();
	
	void TestNotifyPlayTonePCmd0001bL();
	
private:
	
	struct TData
		{
		struct TTone
			{
			TTone();
			void Clear();

			TBool iIfUse;
			RSat::TTone iTone;
			}; 
		struct TDuration
			{
			TDuration();
			void Clear();
			
			TBool iIfUse;
			TUint8 iTimeUnit;
			TUint8 iNumOfUnits;
			};

		TBuf8<RSat::KAlphaIdMaxSize> iAlphaId;
		TTone iTone;
		TDuration iDuration;
		};
	
	void PrepareTlvData ( TTlv& aTlvData, TBool aAlphaIdLengthCheck );

	void PrepareExpectDataL ();
	
	void PrepareCompleteDataL ( TInt aResultCode = KErrNone, TBool aAlphaIdLengthCheck = EFalse );

	void ProactiveCommandCallL( RSat::TPlayToneV2& aPlayToneData,
			                    TInt aCompleteErrorCode = KErrNone, 
			                    TInt aResultCode = KErrNone,
			                    TBool aAlphaIdLengthCheck = EFalse);

	void DataInitialization(
			const TData::TTone& aTone,
			const TData::TDuration& aDuration,
			const TDesC8& aAlphaId = KNullDesC8);

	void TerminalResponseCallL(
			const RSat& aSat,
			RSat::TPCmdResult aGeneralResult = RSat::KSuccess);
	
	void DataCompareL( const RSat::TPlayToneV2& aPlayToneData );

	TData iData;

	}; // class CCSatPlayToneFU

#endif // CCSATPLAYTONEFU_H

