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
 @file The TEFUnit header file which tests the SelectItem
*/

#ifndef CCSATSELECTITEMFU_H
#define CCSATSELECTITEMFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatSelectItemFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifySelectItemPCmd0001L();
	void TestNotifySelectItemPCmd0001bL();
	void TestNotifySelectItemPCmd0001cL();
	void TestNotifySelectItemPCmd0001dL();
	void TestNotifySelectItemPCmd0001eL();
	void TestNotifySelectItemPCmd0001fL();
	void TestNotifySelectItemPCmd0001gL();
	void TestNotifySelectItemPCmd0001hL();
	void TestNotifySelectItemPCmd0001iL();
	void TestNotifySelectItemPCmd0001jL();
	void TestNotifySelectItemPCmd0001kL();
	void TestNotifySelectItemPCmd0001lL();
	void TestNotifySelectItemPCmd0002L();
	void TestNotifySelectItemPCmd0004L();
	
private:
	
	void PrepareSelItemCompleteDataL(
			TUint8 aCmdNum,
			TUint8 aCmdQual,
			RSat::TAlphaIdStatus aAlphaIdStatus = RSat::EAlphaIdNotSet,
			const TDesC8& aAlpha = KNullDesC8,
			TUint8 aNumItems = 0,
			const TUint8* aItemIds = NULL,
			const TPtrC8* aItemNames = NULL,
			const RArray<TUint32>* aActionArr = NULL,
			RSat::TIconQualifier aIconQual = RSat::EIconQualifierNotSet,
			TUint8 aIcon = 0,
			RSat::TIconQualifier aIconListQual = RSat::EIconQualifierNotSet,
			const RArray<TUint32>* aIconList = NULL,
			TUint8 aDefaultItem = 0,
			TInt aResult = KErrNone,
			TInt aDelay = 1);

	void TestNotifySelectItemPCmdL(
			TUint8 aCmdNum,
			TUint8 aCmdQual,
			RSat::TSatVersion aParamVersion = RSat::KSatV2,
			RSat::TAlphaIdStatus aAlphaIdStatus = RSat::EAlphaIdNotSet,
			const TDesC8& aAlpha = KNullDesC8,
			TUint8 aNumItems = 0,
			const TUint8* aItemIds = NULL,
			const TPtrC8* aItemNames = NULL,
			const RArray<TUint32>* aActionArr = NULL,
			RSat::TIconQualifier aIconQual = RSat::EIconQualifierNotSet,
			TUint8 aIcon = 0,
			RSat::TIconQualifier aIconListQual = RSat::EIconQualifierNotSet,
			const RArray<TUint32>* aIconList = NULL,
			TUint8 aDefaultItem = 0,
			TInt aResult = KErrNone,
			const TDesC16* aAlphaIdCheck = NULL,
			const TPtrC16* aItemNamesCheck = NULL);

	void GenerateSelItemTerminalResponseL(
			TUint8 aSelectItemPCmdQualifier,
			const RSat::TSelectItemRspV1& aResp,
			const TInt aExpResult = KErrNone);
	
	}; // class CCSatSelectItemFU

#endif // CCSATSELECTITEMFU_H

