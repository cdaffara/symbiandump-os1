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
 @file The TEFUnit header file which tests the LaunchBrowser
*/

#ifndef CCSATLAUNCHBROWSERFU_H
#define CCSATLAUNCHBROWSERFU_H

#include "ccsatcomponenttestbase.h"

class CCSatLaunchBrowserFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyLaunchBrowserPCmd0001L();
	void TestNotifyLaunchBrowserPCmd0002L();
	void TestNotifyLaunchBrowserPCmd0004L();

private:
	
	struct TBrowserData
		{
		TUint8 iCommandQualifier;
		RSat::TUrl iUrl;
		RSat::TBearerList iBearerList;
		TBuf8<RSat::KTextStringMaxSize> iText;
		TBuf8<RSat::KAlphaIdMaxSize> iAlphaId;
		TBool iIfAlphaId;
		TUint iBrowserId;
		TBool iIfUsedBrowserId;
		RSat::TProvisioningFileRef iFileRef;
		};	

	void PrepareTlvData ( TTlv& aTlvData );
	void PrepareExpectDataL ();
	void PrepareCompleteDataL ( TInt aResultCode = KErrNone );
	
	void ProactiveCommandCallL( 
			RSat::TLaunchBrowserV2& aLaunchBrowserData,
			TInt aCompleteErrorCode = KErrNone,
			TInt aResultCode = KErrNone,
			TBool aIfCreateTerminal = EFalse,
			RSat::TPCmdResult aGeneralResult = RSat::KSuccess,
			TUint8 aCommandQualifier = KLaunchBrowserIfNotYetLaunched,
			const TDesC& aAdditionalInfo = KNullDesC
			);
	
	void DataInitialization(
			TUint8 aCommandQualifier,
			const TDesC8& aUrl,
			const TDesC8& aBearerList,
			const TDesC8& aText,
			const TDesC8& aAlphaId,
			const TDesC8& aFileRef,
			TBool aIfUsedBrowserId = EFalse,
			TUint aBrowserId = RSat::EBrowserIdNotSet,
			TBool aIfAlphaId = EFalse
			);

	void TerminalResponseCallL(
			const RSat& aSat,
			RSat::TPCmdResult aGeneralResult = RSat::KSuccess,
			const TDesC& aAdditionalInfo = KNullDesC,
			TInt aResultCode = KErrNone,
			TUint aInfoType = RSat::KNoAdditionalInfo
			);

	void DataCompareL( const RSat::TLaunchBrowserV2& aLaunchBrowser );
	void ClearData( RSat::TLaunchBrowserV2& aLaunchBrowser );

	TBrowserData iData;

	}; // class CCSatLaunchBrowserFU

#endif // CCSATLAUNCHBROWSERFU_H

