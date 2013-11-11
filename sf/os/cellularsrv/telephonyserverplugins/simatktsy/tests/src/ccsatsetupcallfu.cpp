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
//



/**
 @file The TEFUnit test suite for SetUpCall in the SAT.
*/

#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <satcs.h>
#include <ctsy/ltsy/sat_defs.h>

#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <test/mockltsyindicatorids.h>
#include <test/tmockltsydata.h>

#include "ccsatsetupcallfu.h"

const TInt KDelay = 10;

CTestSuite* CCSatSetUpCallFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatSetUpCallFU, TestNotifySetUpCallPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpCallFU, TestNotifySetUpCallPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpCallFU, TestNotifySetUpCallPCmd0004L);

	ADD_TEST_STEP_ISO_CPP(CCSatSetUpCallFU, TestNotifySetUpCallPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpCallFU, TestNotifySetUpCallPCmd0001cL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpCallFU, TestNotifySetUpCallPCmd0001dL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpCallFU, TestNotifySetUpCallPCmd0001eL);

	END_SUITE;
	}

void CCSatSetUpCallFU::PrepareTlvData ( TTlv& aTlvData,
		                                const TData& aData,
		                                const TDesC8& aAlphaIdentifier,
		                                const TDesC8& aSecondAlphaIdentifier )
	{
	aTlvData.Begin(KBerTlvProactiveSimCommandTag);
	
	aTlvData.AddTag(KTlvCommandDetailsTag);         // Command details
	aTlvData.AddByte(aData.iCommandNumber);               // Command number     
	aTlvData.AddByte(aData.iCommandType);                   // Command type     
	aTlvData.AddByte(aData.iCommandQualifier);            // Command qualifier  
	
	aTlvData.AddTag(KTlvDeviceIdentityTag);         // Device identities          
	aTlvData.AddByte(aData.iSourceDevice);                // Source device     
	aTlvData.AddByte(aData.iDestinationDevice);           // Destination device     

	if (aAlphaIdentifier.Length() > 0)
		{
		aTlvData.AddTag(KTlvAlphaIdentifierTag);        // Alpha identifier
		aTlvData.AddData(aAlphaIdentifier);
		}
	
	aTlvData.AddTag(KTlvAddressTag);                // Address
	aTlvData.AddByte(aData.iAddress);                // International, ISDN / telephone numbering plan
	
	TInt callNumberSize = aData.iCallNumber.Length();
	if (callNumberSize > 0)
		{
		for (TInt i = 0; i < callNumberSize; i++)
			{
			aTlvData.AddByte(aData.iCallNumber[i]);           
			}
		}

	TInt infoTransferCapSize = aData.iInfoTransferCap.Length();
	if (infoTransferCapSize > 0)
		{
		// Capability configuration parameters
		aTlvData.AddTag(KTlvCapabilityConfigurationParametersTag);
		
		for (TInt i = 0; i < infoTransferCapSize; i++)
			{
			aTlvData.AddByte(aData.iInfoTransferCap[i]);
			}
		}
	
	TInt subaddresSize = aData.iSubAddress.Length();
	if (subaddresSize > 0)
		{
		aTlvData.AddTag(KTlvSubaddressTag);
		
		for (TInt i = 0; i < subaddresSize; i++)
			{
			aTlvData.AddByte(aData.iSubAddress[i]);
			}
		}
	
	if ( aData.iDuration.iIfUse )
		{
		aTlvData.AddTag(KTlvDurationTag);
		aTlvData.AddByte(aData.iDuration.iUnit);
		aTlvData.AddByte(aData.iDuration.iInterval);
		}

	if ( aData.iIcon1.iIfUse )
		{
		aTlvData.AddTag(KTlvIconIdentifierTag);
		aTlvData.AddByte(aData.iIcon1.iQualifier);
		aTlvData.AddByte(aData.iIcon1.iIdentifier);
		}

	// This blocks are always called last.
	if (aSecondAlphaIdentifier.Length() > 0)
		{
		aTlvData.AddTag(KTlvAlphaIdentifierTag);
		aTlvData.AddData(aSecondAlphaIdentifier);
		}

	if ( aData.iIcon2.iIfUse )
		{
		aTlvData.AddTag(KTlvIconIdentifierTag);
		aTlvData.AddByte(aData.iIcon2.iQualifier);
		aTlvData.AddByte(aData.iIcon2.iIdentifier);
		}

	if ( aData.iTextAttribute1.iIfUse )
		{
		aTlvData.AddTag(KBerTlvProactiveSimCommandTag);
		aTlvData.AddByte(aData.iTextAttribute1.iFormattingPosition);
		aTlvData.AddByte(aData.iTextAttribute1.iFormattingLength);
		aTlvData.AddByte(aData.iTextAttribute1.iFormattingMode);
		aTlvData.AddByte(aData.iTextAttribute1.iColour);
		}

	if ( aData.iTextAttribute2.iIfUse )
		{
		aTlvData.AddTag(KBerTlvProactiveSimCommandTag);
		aTlvData.AddByte(aData.iTextAttribute2.iFormattingPosition);
		aTlvData.AddByte(aData.iTextAttribute2.iFormattingLength);
		aTlvData.AddByte(aData.iTextAttribute2.iFormattingMode);
		aTlvData.AddByte(aData.iTextAttribute2.iColour);
		}

	}

void CCSatSetUpCallFU::PrepareExpectDataL ( TUint8 aCommandType )
	{
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TMockLtsyData1<TUint8> expLtsyData(aCommandType);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data);

	CleanupStack::PopAndDestroy(1); // data
	}

void CCSatSetUpCallFU::PrepareCompleteDataL ( const TData& aData,
		                                     const TDesC8& aAlphaIdentifier,
		                                     TInt aResultCode,
		                                     const TDesC8& aSecondAlphaIdentifier )
	{
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TTlv tlv;
	PrepareTlvData( tlv, aData, aAlphaIdentifier, aSecondAlphaIdentifier );
	
	TPtrC8 tlvDsc = tlv.End();
	TDesC8* tlvDscPtr = &tlvDsc;
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aResultCode, data, KDelay);

	CleanupStack::PopAndDestroy(1); // data
	}

void CCSatSetUpCallFU::ProactiveCommandCallL( RSat::TSetUpCallV6& aSetupCallData, 
		                                     const TData& aData,
		                                     const TDesC8& aAlphaIdentifier,
		                                     TInt aResultCode,
		                                     const TDesC8& aSecondAlphaIdentifier )
	{
	TRequestStatus requestStatus;
	
	PrepareExpectDataL ( aData.iCommandType );
	
	RSat::TSetUpCallV6Pckg setpPck(aSetupCallData);
	iSat.NotifySetUpCallPCmd(requestStatus, setpPck);
		
	PrepareCompleteDataL ( aData, aAlphaIdentifier, aResultCode, aSecondAlphaIdentifier );
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aResultCode, requestStatus.Int());
	}

void CCSatSetUpCallFU::DataCompareL(const RSat::TSetUpCallV6& aSetupCallData,
		                           const TUint8 aCommandQualifier,
		                           const TDesC& aAlphaId,
		                           const RSat::TTypeOfNumber aTypeOfNumber,
		                           const RSat::TNumberingPlan aNumberPlan)
	{
	RSat::TSetUpCallType type;
	
    switch ( aCommandQualifier )
        {
        case KOnlyIfNotBusy:
            {
            // Set up call, if not busy on another call
            type = RSat::EOnlyIfNotBusy;
            break;
            }
        case KOnlyIfNotBusyWithRedial:
            {
            // Set up call, if not busy on another call, with redial
            type = RSat::EOnlyIfNotBusyWithRedial;
            break;
            }
        case KHoldOtherCalls:
            {
            // Set up call, put other calls (if any) on hold
            type = RSat::EHoldOtherCalls;
            break;
            }
        case KHoldOtherCallsWithRedial:
            {
            // Set up call, put other calls (if any) on hold, with 
            // redial
            type = RSat::EHoldOtherCallsWithRedial;
            break;
            }
        case KDisconnectOtherCalls:
            {
            // Set up call, disconnect other calls (if any)
            type = RSat::EDisconnectOtherCalls;
            break;
            }
        case KDisconnectOtherCallsWithRedial:
            {
            // Set up call, disconn. other calls (if any), with redial
            type = RSat::EDisconnectOtherCallsWithRedial;
            break;
            }
        default:
            {
            // Call type not set
            type = RSat::ESetUpCallTypeNotSet;
            break;
            }
        }
    
	ASSERT_EQUALS(type, aSetupCallData.iType);
	ASSERT_TRUE(0 == aSetupCallData.iAlphaIdConfirmationPhase.iAlphaId.Compare(aAlphaId));
	ASSERT_EQUALS(aTypeOfNumber, aSetupCallData.iAddress.iTypeOfNumber);
	ASSERT_EQUALS(aNumberPlan, aSetupCallData.iAddress.iNumberPlan);
	}

//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-SUC-SNSUCPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpCallPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpCallPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpCallFU::TestNotifySetUpCallPCmd0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	TRequestStatus requestStatus;
	
	const TUint8 KAddress = 0x91; //10010001
	RSat::TSetUpCallV6 setupCallData;
	TData data;
	data.iCommandNumber = 1;
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	data.iSourceDevice = KSim;
	data.iDestinationDevice = KNetwork;
	data.iAddress = KAddress;
	data.iCommandType = KSetUpCall;

	_LIT8(KAlphaIdentifier1, "Not busy");
	_LIT16(KAlphaIdentifier1_16, "Not busy");
	
	data.iCallNumber.Append(0x10);
	data.iCallNumber.Append(0x32);
	data.iCallNumber.Append(0x04);
	data.iCallNumber.Append(0x21);
	data.iCallNumber.Append(0x43);
	data.iCallNumber.Append(0x65);
	data.iCallNumber.Append(0x1C);
	data.iCallNumber.Append(0x2C);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifySetUpCallPCmd 
 	//-------------------------------------------------------------------------

	// test C - implemented in 0001b-0001d functions

 	//-------------------------------------------------------------------------
	// TEST E: Coverage test(s) for RSat::NotifySetUpCallPCmd
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(requestStatus);
	
	PrepareCompleteDataL ( data, KAlphaIdentifier1 );

	RSat::TAdditionalInfo additionalInfo;
	additionalInfo.Append(KNoCause);

	PrepareTerminalResponseMockDataL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, additionalInfo, RSat::KMeUnableToProcessCmd, KNullDesC8 );

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifySetUpCall::CompleteNotifyL coverage 
 	//-------------------------------------------------------------------------
	
	PrepareExpectDataL ( data.iCommandType );
	
	RSat::TSetUpCallV6Pckg setpPck(setupCallData);
	iSat.NotifySetUpCallPCmd(requestStatus, setpPck);
	
	RBuf8 bufData;
	CleanupClosePushL(bufData);	
	
	TTlv tlv;
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	
	tlv.AddTag(KTlvCommandDetailsTag);         // Command details
	tlv.AddByte(data.iCommandNumber);               // Command number     
	tlv.AddByte(data.iCommandType);                   // Command type     
	tlv.AddByte(data.iCommandQualifier);            // Command qualifier  
	
	tlv.AddTag(KTlvDeviceIdentityTag);         // Device identities          
	tlv.AddByte(data.iSourceDevice);                // Source device     
	tlv.AddByte(data.iDestinationDevice);           // Destination device     

	tlv.AddTag(KTlvAlphaIdentifierTag);
	tlv.AddTag(KTlvAlphaIdentifierTag);
	tlv.AddTag(KTlvAddressTag);
	tlv.AddTag(KTlvSubaddressTag);

	TPtrC8 tlvDsc = tlv.End();
	TDesC8* tlvDscPtr = &tlvDsc;
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(bufData);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrNone, bufData, KDelay);

	PrepareTerminalResponseMockDataL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, KNullDesC16, RSat::KCmdDataNotUnderstood, KNullDesC8 );

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifySetUpCall::CompleteNotifyL coverage 
	// in case of  "command qualifier"
 	//-------------------------------------------------------------------------

	const TUint8 KDefualtCommand = 0x7F;
	const TUint8 qualifierArray[] = { KHoldOtherCallsWithRedial, KDisconnectOtherCallsWithRedial, 
			                          KDefualtCommand };
	TUint8 sizeArray = sizeof(qualifierArray)/sizeof(qualifierArray[0]);
	RSat::TSetUpCallRspV2 resp;
	RSat::TSetUpCallRspV2Pckg respPck(resp);

	for ( TUint8 i = 0; i < sizeArray; i++ )
		{
		data.iCommandQualifier = qualifierArray[i];
		ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier1 );

		DataCompareL(setupCallData, 
				     data.iCommandQualifier, 
				     KAlphaIdentifier1_16,
				     RSat::EInternationalNumber,
				     RSat::EIsdnNumberPlan);

		resp.iInfoType = RSat::KNoAdditionalInfo;
		resp.iGeneralResult = RSat::KSuccess;
		CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
				data.iCommandQualifier, RSat::ESetUpCall, respPck, resp.iAdditionalInfo, resp.iGeneralResult );
		AssertMockLtsyStatusL();
		}

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier1, KErrGeneral);

	CleanupStack::PopAndDestroy(2, this); // this, bufData

	}


/**
@SYMTestCaseID BA-CSAT-SUC-SNSUCPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySetUpCallPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySetUpCallPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpCallFU::TestNotifySetUpCallPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TUint8 commandType = KSetUpCall;
	
	TRequestStatus requestStatus;

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifySetUpCallPCmd
 	//-------------------------------------------------------------------------

	PrepareExpectDataL ( commandType );

	RSat::TSetUpCallV6 setupCallData;
	RSat::TSetUpCallV6Pckg setupCallDataPck(setupCallData);
	iSat.NotifySetUpCallPCmd(requestStatus, setupCallDataPck);

	iSat.CancelAsyncRequest(ESatNotifySetUpCallPCmd);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this); // this	

	}


/**
@SYMTestCaseID BA-CSAT-SUC-SNSUCPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySetUpCallPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySetUpCallPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpCallFU::TestNotifySetUpCallPCmd0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RSat sat2;
	ret = sat2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(sat2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	const TUint8 KAddress = 0x91; //10010001

	TData data;
	data.iCommandNumber = 1;
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	data.iSourceDevice = KSim;
	data.iDestinationDevice = KNetwork;
	data.iAddress = KAddress;
	data.iCommandType = KSetUpCall;

	_LIT8(KAlphaIdentifier1, "Not busy");
	_LIT16(KAlphaIdentifier2, "Not busy");
	
	data.iCallNumber.Append(0x10);
	data.iCallNumber.Append(0x32);
	data.iCallNumber.Append(0x04);
	data.iCallNumber.Append(0x21);
	data.iCallNumber.Append(0x43);
	data.iCallNumber.Append(0x65);
	data.iCallNumber.Append(0x1C);
	data.iCallNumber.Append(0x2C);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifySetUpCallPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	PrepareExpectDataL ( data.iCommandType );
	
	RSat::TSetUpCallV6 setupCallData1;
	RSat::TSetUpCallV6Pckg setpPck1(setupCallData1);
	iSat.NotifySetUpCallPCmd(requestStatus1, setpPck1);

	RSat::TSetUpCallV6 setupCallData2;
	RSat::TSetUpCallV6Pckg setpPck2(setupCallData2);
	sat2.NotifySetUpCallPCmd(requestStatus2, setpPck2);

	PrepareCompleteDataL ( data, KAlphaIdentifier1 );

	User::WaitForRequest(requestStatus1);
	User::WaitForRequest(requestStatus2);

	ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	DataCompareL(setupCallData1, 
			     data.iCommandQualifier, 
			     KAlphaIdentifier2,
			     RSat::EInternationalNumber,
			     RSat::EIsdnNumberPlan);

	DataCompareL(setupCallData2, 
			     data.iCommandQualifier, 
			     KAlphaIdentifier2,
			     RSat::EInternationalNumber,
			     RSat::EIsdnNumberPlan);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}

/**
@SYMTestCaseID BA-CSAT-SUC-SNSUCPC-0001b
@SYMPREQ 1780
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for set up call ipc ( Expected Sequence 2 See 3GPP TS 31.124 ) 
@SYMTestPriority High
@SYMTestActions Invokes RSat.NotifySetUpCallPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
 */

void CCSatSetUpCallFU::TestNotifySetUpCallPCmd0001bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	RSat::TSetUpCallRspV2 resp;
	RSat::TSetUpCallRspV2Pckg respPck(resp);
	RSat::TSetUpCallV6 setupCallData;

	const TUint8 KAddress = 0x91; //10010001

	TData data;
	data.iCommandNumber = 1;
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	data.iSourceDevice = KSim;
	data.iDestinationDevice = KNetwork;
	data.iAddress = KAddress;
	data.iCommandType = KSetUpCall;

	//
	// Expected Sequence 2.1 (SET UP CALL, two alpha identifiers)
	//

	data.iCallNumber.Append(0x10);
	data.iCallNumber.Append(0x32);
	data.iCallNumber.Append(0x04);
	data.iCallNumber.Append(0x21);
	data.iCallNumber.Append(0x43);
	data.iCallNumber.Append(0x65);
	data.iCallNumber.Append(0x1C);
	data.iCallNumber.Append(0x2C);

	_LIT8(KAlphaIdentifier8_1, "CONFIRMATION");
	_LIT16(KAlphaIdentifier16, "CONFIRMATION");

	_LIT8(KAlphaIdentifier8_2, "CALL");

	data.iDuration.iIfUse = EFalse;
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_1, KErrNone, KAlphaIdentifier8_2 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.iGeneralResult = RSat::KSuccess;
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this); // this
	}

/**
@SYMTestCaseID BA-CSAT-SUC-SNSUCPC-0001c
@SYMPREQ 1780
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for set up call ipc ( Expected Sequence 3 See 3GPP TS 31.124 ) 
@SYMTestPriority High
@SYMTestActions Invokes RSat.NotifySetUpCallPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
 */

void CCSatSetUpCallFU::TestNotifySetUpCallPCmd0001cL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	RSat::TSetUpCallRspV2 resp;
	RSat::TSetUpCallRspV2Pckg respPck(resp);
	RSat::TSetUpCallV6 setupCallData;

	const TUint8 KAddress = 0x91; //10010001

	TData data;
	data.iCommandNumber = 1;
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	data.iSourceDevice = KSim;
	data.iDestinationDevice = KNetwork;
	data.iAddress = KAddress;
	data.iCommandType = KSetUpCall;
	
	//
	// Expected Sequence 3.1A (SET UP CALL, display of basic icon during confirmation phase, 
	// not selfexplanatory, successful)
	//

	data.iCallNumber.Append(0x10);
	data.iCallNumber.Append(0x32);
	data.iCallNumber.Append(0x04);
	data.iCallNumber.Append(0x21);
	data.iCallNumber.Append(0x43);
	data.iCallNumber.Append(0x65);
	data.iCallNumber.Append(0x1C);
	data.iCallNumber.Append(0x2C);

	const TUint8 KIconNotSelfExplanatory = 0x01;

	_LIT8(KAlphaIdentifier8_9, "Set up call Icon 3.1.1");
	_LIT16(KAlphaIdentifier16_9, "Set up call Icon 3.1.1");
	data.iIcon1.iIfUse = ETrue;
	data.iIcon1.iIdentifier = 0x01;
	data.iIcon1.iQualifier = KIconNotSelfExplanatory;

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_9 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_9, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.iGeneralResult = RSat::KSuccess;

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 3.2A (SET UP CALL, display of basic icon during confirmation phase, 
	// selfexplanatory, successful)
	//

	const TUint8 KIconSelfExplanatory = 0x00;

	_LIT8(KAlphaIdentifier8_10, "Set up call Icon 3.2.1");
	_LIT16(KAlphaIdentifier16_10, "Set up call Icon 3.2.1");
	data.iIcon1.iQualifier = KIconSelfExplanatory;
	data.iIcon1.iIdentifier = 0x02;

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_10 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_10, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iGeneralResult = RSat::KSuccess;
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 3.3A (SET UP CALL, display of colour icon during confirmation phase, 
	// not self-explanatory, successful)
	//

	_LIT8(KAlphaIdentifier8_11, "Set up call Icon 3.3.1");
	_LIT16(KAlphaIdentifier16_11, "Set up call Icon 3.3.1");
	data.iIcon1.iQualifier = KIconNotSelfExplanatory;
	data.iIcon1.iIdentifier = 0x02;

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_11 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_11, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iGeneralResult = RSat::KSuccess;
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 3.4A (display of self explanatory basic icon during set up call,
	// successful)
	//

	_LIT8(KAlphaIdentifier8_12_1, "Set up call Icon 3.4.1");
	_LIT8(KAlphaIdentifier8_12_2, "Set up call Icon 3.4.2");
	_LIT16(KAlphaIdentifier16_12, "Set up call Icon 3.4.1");
	data.iIcon1.iQualifier = KIconSelfExplanatory;
	data.iIcon1.iIdentifier = 0x01;
	data.iIcon2.iIfUse = ETrue;
	data.iIcon2.iIdentifier = 0x01;
	data.iIcon2.iQualifier = KIconSelfExplanatory;

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_12_1, 
			               KErrNone, KAlphaIdentifier8_12_2 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_12, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iGeneralResult = RSat::KSuccess;
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 3.1B (SET UP CALL, display of basic icon during confirmation phase, 
	// not selfexplanatory, requested icon could not be displayed)
	//

	// Parameters call for Expected Sequence 3.1A
	data.iIcon1.iQualifier = KIconNotSelfExplanatory;
	data.iIcon1.iIdentifier = 0x01;

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_9 );

	
	/*
	 * The Expected Sequence 3.1B does not correspond to code. 
	 * CSatNotifySetUpCall::TerminalResponseL function returns KErrCorrupt code error
	 * in case of "resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed".
	 * But 3GPP TS 31.124 ( Section 27.22.4.13.3.4.2 ) document says that in case of 
	 * "resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed" 
	 * CSatNotifySetUpCall::TerminalResponseL function should return KErrNone.
	 */

	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;

	CCSatComponentTestBase::PrepareTerminalResponseMockDataL(
			data.iCommandNumber,
			data.iCommandType,
			data.iCommandQualifier,
			KNullDesC16,
			resp.iGeneralResult,
			KNullDesC8);

	TRequestStatus reqStatus;
	iSat.TerminalRsp(reqStatus, RSat::ESetUpCall, respPck);	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 3.2B (SET UP CALL, display of basic icon during confirmation phase, 
	// selfexplanatory, requested icon could not be displayed)
	//

	data.iIcon1.iIdentifier = 0x02;

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_10 );

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 3.3B (SET UP CALL, display of colour icon during confirmation phase, 
	// not self-explanatory, requested icon could not be displayed)
	//

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_11 );

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 3.4B (SET UP CALL, display of self explanatory basic icon during 
	// set up call, requested icon could not be displayed)
	//

	data.iIcon1.iQualifier = KIconSelfExplanatory;
	data.iIcon1.iIdentifier = 0x01;

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_12_1, 
			               KErrNone, KAlphaIdentifier8_12_2 );

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this); // this
	}

/**
@SYMTestCaseID BA-CSAT-SUC-SNSUCPC-0001d
@SYMPREQ 1780
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for set up call ipc ( Expected Sequence 4 See 3GPP TS 31.124 ) 
@SYMTestPriority High
@SYMTestActions Invokes RSat.NotifySetUpCallPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
 */

void CCSatSetUpCallFU::TestNotifySetUpCallPCmd0001dL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	//
	// Expected Sequence 4.1 (SET UP CALL, Text Attribute ?Left Alignment)
	//

	TestTextAttributeL(CCSatSetUpCallFU::TData::TTextAttribute::ELeft);

	//
	// Expected Sequence 4.2 (SET UP CALL, Text Attribute ?Left Alignment)
	//

	TestTextAttributeL(CCSatSetUpCallFU::TData::TTextAttribute::ECenter);

	//
	// Expected Sequence 4.3 (SET UP CALL, Text Attribute ?Right Alignment)
	//

	TestTextAttributeL(CCSatSetUpCallFU::TData::TTextAttribute::ERight);

	//
	// Expected Sequence 4.4 (SET UP CALL, Text Attribute ?Large Font Size)
	//

	TestTextAttributeL(CCSatSetUpCallFU::TData::TTextAttribute::ELarge);

	//
	// Expected Sequence 4.5 (SET UP CALL, Text Attribute ?Small Font Size)
	//

	TestTextAttributeL(CCSatSetUpCallFU::TData::TTextAttribute::ESmall);

	//
	// Expected Sequence 4.6 (SET UP CALL, Text Attribute ?Bold On)
	//

	TestTextAttributeL(CCSatSetUpCallFU::TData::TTextAttribute::EBond);

	//
	// Expected Sequence 4.7 (SET UP CALL, Text Attribute ?Italic On)
	//

	TestTextAttributeL(CCSatSetUpCallFU::TData::TTextAttribute::EItalic);

	//
	// Expected Sequence 4.8 (SET UP CALL, Text Attribute ?Underline On)
	//

	TestTextAttributeL(CCSatSetUpCallFU::TData::TTextAttribute::EUnderline);

	//
	// Expected Sequence 4.9 (SET UP CALL, Text Attribute ?Strikethrough On)
	//

	TestTextAttributeL(CCSatSetUpCallFU::TData::TTextAttribute::EStrikethrough);

	//
	// Expected Sequence 4.10 (SET UP CALL, Text Attribute ?Foreground and Background Colour)
	//

	TestTextAttributeL( CCSatSetUpCallFU::TData::TTextAttribute::ELeft,
			            CCSatSetUpCallFU::TData::TTextAttribute::EMode1, 
			            CCSatSetUpCallFU::TData::TTextAttribute::EMode2 );

	CleanupStack::PopAndDestroy(1, this); // this
	}

void CCSatSetUpCallFU::TestTextAttributeL( const TUint8 aMode,
		                                   const TUint8 aColour1,
		                                   const TUint8 aColour2 )
	{

	RSat::TSetUpCallRspV2 resp;
	RSat::TSetUpCallRspV2Pckg respPck(resp);
	RSat::TSetUpCallV6 setupCallData;

	const TUint8 KAddress = 0x91; //10010001

	TData data;
	data.iCommandNumber = 1;
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	data.iSourceDevice = KSim;
	data.iDestinationDevice = KNetwork;
	data.iAddress = KAddress;
	data.iCommandType = KSetUpCall;
	
	data.iCallNumber.Append(0x10);
	data.iCallNumber.Append(0x32);
	data.iCallNumber.Append(0x04);
	data.iCallNumber.Append(0x21);
	data.iCallNumber.Append(0x43);
	data.iCallNumber.Append(0x65);
	data.iCallNumber.Append(0x1C);
	data.iCallNumber.Append(0x2C);

	_LIT8(KAlphaIdentifier8_1, "CONFIRMATION 1");
	_LIT8(KAlphaIdentifier8_2, "CALL 1");
	_LIT16(KAlphaIdentifier16, "CONFIRMATION 1");

	data.iTextAttribute1.iIfUse = ETrue;
	data.iTextAttribute1.iFormattingPosition = 0;
	data.iTextAttribute1.iFormattingLength = 0x14;
	data.iTextAttribute1.iFormattingMode = aMode;
	
	// Dark Green Foreground, Bright Yellow Background
	data.iTextAttribute1.iColour = aColour1;

	data.iTextAttribute2 = data.iTextAttribute1;
	data.iTextAttribute2.iFormattingLength = 0x06;
	data.iTextAttribute2.iColour = aColour2;

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_1,
			               KErrNone, KAlphaIdentifier8_2 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.iGeneralResult = RSat::KSuccess;

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	}

/**
@SYMTestCaseID BA-CSAT-SUC-SNSUCPC-0001e
@SYMPREQ 1780
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for set up call ipc ( Expected Sequence 1 See 3GPP TS 31.124 ) 
@SYMTestPriority High
@SYMTestActions Invokes RSat.NotifySetUpCallPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
 */

void CCSatSetUpCallFU::TestNotifySetUpCallPCmd0001eL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	RSat::TSetUpCallRspV2 resp;
	RSat::TSetUpCallRspV2Pckg respPck(resp);
	RSat::TSetUpCallV6 setupCallData;

	const TUint8 KAddress = 0x91; //10010001

	TData data;
	data.iCommandNumber = 1;
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	data.iSourceDevice = KSim;
	data.iDestinationDevice = KNetwork;
	data.iAddress = KAddress;
	data.iCommandType = KSetUpCall;
	
	//
	// See 3GPP TS 31.124 V7.4.0 (2008-06) in subclause 27.22.4.13, 
	// Expected Sequence 1.1 (SET UP CALL, call confirmed by the user and connected)
	//
	
	_LIT8(KAlphaIdentifier8_1, "Not busy");
	_LIT16(KAlphaIdentifier16_1, "Not busy");
	
	data.iCallNumber.Append(0x10);
	data.iCallNumber.Append(0x32);
	data.iCallNumber.Append(0x04);
	data.iCallNumber.Append(0x21);
	data.iCallNumber.Append(0x43);
	data.iCallNumber.Append(0x65);
	data.iCallNumber.Append(0x1C);
	data.iCallNumber.Append(0x2C);
	
	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_1 );
	
	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_1, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.SetPCmdNumber(data.iCommandNumber);
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 1.4 (SET UP CALL, putting all other calls on hold, ME busy)
	//
		
	data.iCommandQualifier = KSetUpCallPuttingOtherCallsOnHold;
	_LIT8(KAlphaIdentifier8_2, "On hold");
	_LIT16(KAlphaIdentifier16_2, "On hold");
	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_2 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_2, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 1.5 (SET UP CALL, disconnecting all other calls, ME busy)
	//

	data.iCommandQualifier = KSetUpCallDisconnectingOtherCalls;
	_LIT8(KAlphaIdentifier8_3, "Disconnect");
	_LIT16(KAlphaIdentifier16_3, "Disconnect");
	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_3 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_3, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 1.6 (SET UP CALL, only if not currently busy on another call, ME busy)
	//

	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_1 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_1, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Append(RSat::KMeBusyOnCall);
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, resp.iAdditionalInfo, resp.iGeneralResult );
	AssertMockLtsyStatusL();
	
	//
	// Expected Sequence 1.7A (SET UP CALL, putting all other calls on hold, call hold is not allowed)
	//

	data.iCommandQualifier = KSetUpCallPuttingOtherCallsOnHold;
	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_2 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_2, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);
	resp.iGeneralResult = RSat::KNetworkUnableToProcessCmd;
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, resp.iAdditionalInfo, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 1.7B
	//

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_2 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_2, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	const TUint KFacilityRejected = 0x9D;
	
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(KFacilityRejected);
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, resp.iAdditionalInfo, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 1.8 (SET UP CALL, Capability configuration)
	//
	
	RSat::TCcp infoTransferCap;
	data.iInfoTransferCap.Append(0x01);
	data.iInfoTransferCap.Append(0xA0);
	_LIT8(KAlphaIdentifier8_4, "Capability config");
	_LIT16(KAlphaIdentifier16_4, "Capability config");
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_4 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_4, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.iGeneralResult = RSat::KSuccess;
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 1.9 (SET UP CALL, max dialling number string, no alpha identifier)
	//
	data.iInfoTransferCap.Zero();
	
	data.iCallNumber.Zero();
	data.iCallNumber.Append(0x10);
	data.iCallNumber.Append(0x32);
	data.iCallNumber.Append(0x54);
	data.iCallNumber.Append(0x76);
	data.iCallNumber.Append(0x98);
	data.iCallNumber.Append(0x10);
	data.iCallNumber.Append(0x32);
	data.iCallNumber.Append(0x54);
	data.iCallNumber.Append(0x76);
	data.iCallNumber.Append(0x98);
	data.iCallNumber.Append(0x10);

	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusyWithRedial;
	ProactiveCommandCallL( setupCallData, data, KNullDesC8 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KNullDesC16, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();
	
	//
	// Expected Sequence 1.10 (SET UP CALL, 256 octets length, long first alpha identifier)
	//

	data.iCallNumber.Zero();
	data.iCallNumber.Append(0x10);

	_LIT8(KAlphaIdentifier8_5, "Three types are defined: - set up a call, but only if not currently busy on another\
call; - set up a call, putting all other calls (if any) on hold; - set up a call,\
disconnecting all other calls (if any) first. For each of these types,");

	_LIT16(KAlphaIdentifier16_5, "Three types are defined: - set up a call, but only if not currently busy on another\
call; - set up a call, putting all other calls (if any) on hold; - set up a call,\
disconnecting all other calls (if any) first. For each of these types,");

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_5 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_5, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 1.11A (SET UP CALL, Called party subaddress, command performed successfully)
	//

	data.iCallNumber.Zero();
	data.iCallNumber.Append(0x10);
	data.iCallNumber.Append(0x32);
	data.iCallNumber.Append(0x04);
	data.iCallNumber.Append(0x21);
	data.iCallNumber.Append(0x43);
	data.iCallNumber.Append(0x65);
	data.iCallNumber.Append(0x1C);
	data.iCallNumber.Append(0x2C);
	
	data.iSubAddress.Append(0x80);
	data.iSubAddress.Append(0x50);
	data.iSubAddress.Append(0x95);
	data.iSubAddress.Append(0x95);
	data.iSubAddress.Append(0x95);
	data.iSubAddress.Append(0x95);
	data.iSubAddress.Append(0x95);
	
	_LIT8(KAlphaIdentifier8_6, "Called party");
	_LIT16(KAlphaIdentifier16_6, "Called party");
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusy;
	
	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_6 );

	DataCompareL(setupCallData, 
			     data.iCommandQualifier,
			     KAlphaIdentifier16_6, 
			     RSat::EInternationalNumber, 
			     RSat::EIsdnNumberPlan);

	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();
	
	//
	// Expected Sequence 1.11B
	//

	ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_6 );

	resp.iGeneralResult = RSat::KCmdBeyondMeCapabilities;
	CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
			data.iCommandQualifier, RSat::ESetUpCall, respPck, KNullDesC16, resp.iGeneralResult );
	AssertMockLtsyStatusL();

	//
	// Expected Sequence 1.12 (SET UP CALL, maximum duration for the redial mechanism)
	//

	const TUint8 KDefualtTimeUnit = 0x7F;
	const TUint8 timeUnitArray[] = { KMinutes, KSeconds, KTenthsOfSeconds, KDefualtTimeUnit };

	data.iSubAddress.Zero();
	_LIT8(KAlphaIdentifier8_7, "Duration");
	_LIT16(KAlphaIdentifier16_7, "Duration");
	data.iDuration.iIfUse = ETrue;
	data.iDuration.iInterval = 0x0A;
	data.iCommandQualifier = KSetUpCallIfNotCurrentlyBusyWithRedial;

	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(KFacilityRejected);
	resp.iGeneralResult = RSat::KNetworkUnableToProcessCmd;

	TUint8 sizeArray = sizeof(timeUnitArray)/sizeof(timeUnitArray[0]);
	
	/*
	 * to increase CSatNotifySetUpCall::CompleteNotifyL coverage 
	 * in case of  "duration"
	 */
	for (TUint8 i = 0; i < sizeArray; i++)
		{
		data.iDuration.iUnit = timeUnitArray[i];
		
		ProactiveCommandCallL( setupCallData, data, KAlphaIdentifier8_7 );

		DataCompareL(setupCallData, 
				     data.iCommandQualifier,
				     KAlphaIdentifier16_7, 
				     RSat::EInternationalNumber, 
				     RSat::EIsdnNumberPlan);

		CCSatComponentTestBase::GenerateTerminalResponseL( data.iCommandNumber, data.iCommandType, 
				data.iCommandQualifier, RSat::ESetUpCall, respPck, resp.iAdditionalInfo, resp.iGeneralResult );
		AssertMockLtsyStatusL();
		}

	CleanupStack::PopAndDestroy(1, this); // this
	}


