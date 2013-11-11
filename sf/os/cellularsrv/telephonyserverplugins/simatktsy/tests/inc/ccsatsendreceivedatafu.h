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
 @file The TEFUnit header file which tests the SendReceiveData
*/

#ifndef CCSATSENDRECEIVEDATAFU_H
#define CCSATSENDRECEIVEDATAFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>
#include <ctsy/ltsy/sat_defs.h>

#include "ccsatcomponenttestbase.h"

const TUint8 KPCmdNumberOne = 1;

class CCSatSendReceiveDataFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyReceiveDataPCmd0001L();
	void TestNotifyReceiveDataPCmd0001bL();
	void TestNotifyReceiveDataPCmd0002L();
	void TestNotifyReceiveDataPCmd0004L();
	void TestNotifySendDataPCmd0001L();
	void TestNotifySendDataPCmd0001bL();
	void TestNotifySendDataPCmd0002L();
	void TestNotifySendDataPCmd0004L();
	
private:

	void PrepareSendDataCompleteDataL(
			const TDesC8* aChannelData,
			TInt aDelay = KDefaultNoDelay);
	
	void PrepareSendDataCompleteDataL(
			const TDesC8* aChannelData,
			TUint8 aPCmdNumber,
			TUint8 aCmdQual,
			TUint8 aDestDeviceId,
			const TDesC8* aAlphaId,
			RSat::TIconQualifier aIconQual,
			TInt aResult,
			TInt aDelay = KDefaultNoDelay);

	void TestNotifySendDataPCmdL(
			const TDesC8* aChannelData,
			TUint8 aCmdQual = KBipSendDataImmediately);

	void TestNotifySendDataPCmdErrorL(
			const TDesC8* aChannelData,
			TInt aError);

	void TestNotifySendDataPCmdL(
			const TDesC8* aChannelData,
			RSat::TDeviceId aDestDeviceId);

	void TestNotifySendDataPCmdL(
			const TDesC8* aChannelData,
			const TDesC8* aAlphaId,
			const TDesC16* aAlphaIdCheck = NULL,
			RSat::TIconQualifier aIconQual = RSat::EIconQualifierNotSet);
	
	void TestNotifySendDataPCmdL(
			const TDesC8* aChannelData,
			TUint8 aPCmdNumber,
			TUint8 aCmdQual,
			RSat::TDeviceId aDestDeviceId,
			const TDesC8* aAlphaId,
			RSat::TIconQualifier aIconQual,
			TInt aResult,
			const TDesC16* aAlphaIdCheck);
	
	void GenerateSendDataTerminalResponseL(const RSat::TSendDataRspV2& aResp);

	void GenerateSendDataTerminalResponseL(
			const RSat::TSendDataRspV2& aResp,
			TUint8 aCmdQual,
			TInt aExpResult = KErrNone);

	void PrepareReceiveDataCompleteDataL(
			const TUint8* aChannelDataLength,
			TInt aDelay = KDefaultNoDelay);
	
	void PrepareReceiveDataCompleteDataL(
			TUint8 aPCmdNumber,
			const TUint8* aChannelDataLength,
			const TDesC8* aAlphaId,
			RSat::TIconQualifier aIconQual,
			TInt aResult,
			TInt aDelay,
			TUint8 aDestDeviceId);

	void TestNotifyReceiveDataPCmdL(
			const TUint8* aChannelDataLength,
			TInt aResult = KErrNone);
	
	void TestNotifyReceiveDataPCmdL(
			const TUint8* aChannelDataLength,
			RSat::TDeviceId aDestDeviceId);
	
	void TestNotifyReceiveDataPCmdL(
			const TUint8* aChannelDataLength,
			const TDesC8* aAlphaId,
			const TDesC16* aAlphaIdCheck = NULL,
			RSat::TIconQualifier aIconQual = RSat::EIconQualifierNotSet);
	
	void TestNotifyReceiveDataPCmdL(
			TUint8 aPCmdNumber,
			const TUint8* aChannelDataLength,
			const TDesC8* aAlphaId,
			RSat::TIconQualifier aIconQual,
			TInt aResult,
			const TDesC16* aAlphaIdCheck,
			RSat::TDeviceId aDestDeviceId);
	
	void GenerateReceiveDataTerminalResponseL(
			const RSat::TReceiveDataRspV2& aResp,
			TInt aExpResult = KErrNone);

	}; // class CCSatSendReceiveDataFU

#endif // CCSATSENDRECEIVEDATAFU_H

