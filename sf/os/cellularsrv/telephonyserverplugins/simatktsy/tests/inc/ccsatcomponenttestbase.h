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
 @file base class for SAT component tests
*/

#ifndef CCSATCOMPONENTTESTBASE_H
#define CCSATCOMPONENTTESTBASE_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>
#include <etelpckt.h>
#include <etelsat.h>
#include <ctsy/mmtsy_names.h>

#include <test/rmockltsy.h>
#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/ltsy/sat_defs.h>
 
#include <cstktsy/ttlv.h>

// Delay constants for MockLtsy completion delay
const TInt KDefaultNoDelay = 1;
const TInt KDefaultDelay = 10;


class CCSatComponentTestBase : public CTestFixture
	{

protected:
	enum TExtendedErrorClient
		{
		EUseBasicError,
		EUseExtendedError
		};
	void OpenEtelServerL(TExtendedErrorClient aSetExtendedErrorClient = EUseBasicError);
	void OpenPhoneL();

	void DialL(RMobileCall& aCall, TInt aCallId, RMobilePhone::TMobileService aService);
	void OpenPacketServiceL(RPacketService& aRPacketService, RPacketService::TStatus aStartAttachStatus = RPacketService::EStatusUnattached, RPacketService::TAttachMode aStartAttachMode = RPacketService::EAttachWhenNeeded);

    TInt InitContextL(RPacketContext& aPacketContext, TInfoName& aContextName, TInfoName aHostCidName = _L(""));
	TInt SetContextActiveL(TInfoName& aContextName);
	TInt SetContextStatusL(TInfoName& aContextName, RPacketContext::TContextStatus aStatus);
	TInt CreateQoSAndSetProfileParamLC(RPacketQoS& aPacketQoS, 
									   RPacketContext& aPacketContext,
									   TInfoName& aContextName,
									   TPacketDataConfigBase& aRequested,
									   TInfoName aPrimaryContextName = _L(""));
									 
	TInt SetPacketServiceStatusL(RPacketService::TStatus& aStatus, TBool aIsResumed);
	
	TInt AddContextPacketFilterL(RPacketContext& aContext, TInfoName& aContextName, 
								 RPacketContext::TPacketFilterV2& aFilter, TBool aIsFailInCSAT = 0);

	TInt OpenNewCall(RLine& aLine,
						RCall& aCall,
						const TDesC& aLineName);
	
	TInt OpenNewCall(RLine& aLine, RCall& aCall, const TDesC& aLineName, TName& aNewName);
	
	TInt CreateIncomingCall(RLine& aLine, 
	                        const TInt aCallId, 
	                        const TDesC& aLineName,
	                        TName& aIncomingCallName,
	                        RMobilePhone::TMobileService aMobileService);

	void CloseIncomingCall(RCall& aCall, const TInt aCallId,
	                                               RMobilePhone::TMobileService aMobileService);
	
	void CloseLineL(RLine& aLine, TInt aCallId = 0, RMobilePhone::TMobileService aMobileService = RMobilePhone::EServiceUnspecified);

	void AssertMockLtsyStatusL();

	// to push a CleanupItem ont the cleanupstack to close the phone
	static void Cleanup(TAny* self);
	virtual void DoCleanup();
	
	void PrepInternalGettersMockLtsy();
	
	void DriverPrepInternalGettersMockLtsyL();
	
	void WaitForMockLTSYTerminated();
	
	CActiveScheduler* InstallSchedulerLC();
	// Fake type used for bad parameter testing.
	// These will have the correct extension ID but the wrong descriptor
	// size compared to the T-class which the API expects.
	class TTestMultimodeType
		{
	public:
		TTestMultimodeType(TInt aExtensionId) { iExtensionId = aExtensionId; }
		TInt ExtensionId() const { return iExtensionId; }

	private:
		TInt iExtensionId;
		};

	typedef TPckg<TTestMultimodeType> TTestMultimodeTypePckg;
		
	void CallGetMobileCallInfoL(TInt aCallId, 
	                           RMobilePhone::TMobileService aService, 
    	                       const TDesC &aTelNumber);
    	                       
	void OpenSatL(TUint8 aEnabled = 0x00);

protected:
	void PrepareMockWithExpDataForPCmdNotifyL( TUint8 aPCmdNumber );
	void PrepareMockWithCompleteDataForPCmdNotifyL( TPtrC8 aCompleteTlv, TInt aError = RSat::KSuccess, TInt aDelay = KDefaultNoDelay);

	void GenerateTermRspTlv(
			RBuf8& aBuf, 
			TUint8 aPCmdNumber,
			TUint8 aTypeOfCommand, 
			TUint8 aCommandQualifier, 
			RSat::TPCmdResult aGeneralResult = RSat::KSuccess, 
			const TDesC& aGeneralResultAddInfo = KNullDesC, 
			const TDesC8& aAdditionalInfoTlv = KNullDesC8);
	void PrepareTerminalResponseMockDataL(
			TUint8 aPCmdNumber,
			TUint8 aTypeOfCommand,
			TUint8 aCommandQualifier,
			const TDesC& aGeneralResultAddInfo = KNullDesC,
			RSat::TPCmdResult aGeneralResult = RSat::KSuccess,
			const TDesC8& aAdditionalInfoTlv = KNullDesC8,
			const TInt aResultForExpect = KErrNone);
	void TerminalResponseL(
			RSat::TPCmd aRSatTypeOfCommand,
			const TDesC8& aRspPckg,
			const TInt aExpResult = KErrNone);
	void GenerateTerminalResponseL(
			TUint8 aPCmdNumber,
			TUint8 aTypeOfCommand,
			TUint8 aCommandQualifier,
			RSat::TPCmd aRSatTypeOfCommand,
			const TDesC8& aRspPckg,
			const TDesC& aGeneralResultAddInfo = KNullDesC,
			RSat::TPCmdResult aGeneralResult = RSat::KSuccess,
			const TDesC8& aAdditionalInfoTlv = KNullDesC8,
			const TInt aExpResult = KErrNone);
	
	void EndPSimSessionL();

	TBool CommandPerformedSuccessfully( TUint8 aGeneralResult );

	TInt UnicodeToPacked7L(const TDesC16& aInput, TDes8& aOutput);

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	
	RMockLtsy  iMockLTSY;	
	RSat iSat;
	RMobileSmsMessaging iSms;
	}; // class CCSatComponentTestBase


void ProactiveCmdTlvBegin( 
		TTlv& aTlv, 
		TUint8 aPCmdNumber,
		TUint8 aTypeOfCommand,
		TUint8 aCommandQualifier,
		TUint8 aDestDeviceId);

TInt Compare16bitDesWith8bitDesL(const TDesC8& aStringToCompareWith, const TDesC16& aStringToCompare);

// ------------------------------------------------------------------------ //

//
// Defintions of various Network Information type data for use in test-steps
//

// Data values for RMobilePhone::TMobilePhoneNetworkInfoV5
const RMobilePhone::TMobilePhoneNetworkCountryCode 	KCountryCode= _L("011");
const RMobilePhone::TMobilePhoneNetworkIdentity    	KCdmaSID    = _L("666");
const RMobilePhone::TMobilePhoneNetworkIdentity    	KAnalogSID  = _L("777");
const RMobilePhone::TMobilePhoneNetworkIdentity    	KNetworkId  = _L("88888");
const RMobilePhone::TMobilePhoneNetworkDisplayTag  	KDisplayTag = _L("MyNet");
const RMobilePhone::TMobilePhoneNetworkShortName   	KShortName  = _L("MyNetwork");
const RMobilePhone::TMobilePhoneNetworkLongName    	KLongName	= _L("MyNetworkLongName");
const RMobilePhone::TMobilePhoneNetworkAccess 		KAccess 	= RMobilePhone::ENetworkAccessGsm;
const RMobilePhone::TMobilePhoneNetworkMode 		KMode 		= RMobilePhone::ENetworkModeCdma95;
const RMobilePhone::TMobilePhoneNetworkStatus 		KStatus  	= RMobilePhone::ENetworkStatusAvailable;
const RMobilePhone::TMobilePhoneNetworkBandInfo 	KBandInfo 	= RMobilePhone::E1900BandF;
const TBool KEgprsAvailable = ETrue;
const TBool KHsdpaAvailable = EFalse;

// Data values for RMobilePhone::TMobilePhoneLocationAreaV1
const TBool KAreaKnown = ETrue;
const TUint KLocationAreaCode = 9999;
const TUint KCellId = 111;

// Data values for RMobilePhone::TMobilePhoneNetworkMode
const RMobilePhone::TMobilePhoneNetworkMode KNetworkMode = RMobilePhone::ENetworkModeWcdma;

// Data values for RMobilePhone::TMobilePhoneNITZ
const TInt32 KNitzFieldsUsed = 	RMobilePhone::KCapsTimeAvailable |
								RMobilePhone::KCapsTimezoneAvailable |
								RMobilePhone::KCapsDSTAvailable |
								RMobilePhone::KCapsShortNameAvailable |
								RMobilePhone::KCapsLongNameAvailable;

const TInt KTimeZone = 10;
const TInt KDst = 5;

// Data values for Signal Strength
const TInt32 KSignalStrength = 99;
const TInt8 KBar = 5;

// Data values for RMobilePhone::TMobilePhoneBatteryInfoV1
const RMobilePhone::TMobilePhoneBatteryStatus KBatteryStatus = RMobilePhone::EPoweredByBattery;
const TUint KBatteryChargeLevel = 77;

const TUint8 KSmsMoControlEnabled 	= 0x01;
const TUint8 KSmsPpDownloadEnabled 	= 0x02;
const TUint8 KUssdEnabled		 	= 0x04;

// Flags for Text Formatting Mode coding in Text Attribute Tlv tag data.
// See ETSI TS 123 040 in subclause 9.2.3.24.10.1.1
const TUint8 KTlvTextAttributeAlignmentLeft      = 0x00;
const TUint8 KTlvTextAttributeAlignmentCentre    = 0x01;
const TUint8 KTlvTextAttributeAlignmentRight     = 0x02;
const TUint8 KTlvTextAttributeAlignmentDefault   = 0x03;
const TUint8 KTlvTextAttributeFontSizeNormal     = 0x00;
const TUint8 KTlvTextAttributeFontSizeLarge      = 0x04;
const TUint8 KTlvTextAttributeFontSizeSmall      = 0x08;
const TUint8 KTlvTextAttributeStyleNormal        = 0x00;
const TUint8 KTlvTextAttributeStyleBold          = 0x10;
const TUint8 KTlvTextAttributeStyleItalic        = 0x20;
const TUint8 KTlvTextAttributeStyleUnderline     = 0x40;
const TUint8 KTlvTextAttributeStyleStrikethrough = 0x80;

// Flags for Text Foreground Colour coding in Text Attribute Tlv tag data.
// See ETSI TS 123 040 in subclause 9.2.3.24.10.1.1
const TUint8 KTlvTextAttributeForegroundColourBlack         = 0x00;
const TUint8 KTlvTextAttributeForegroundColourDarkGrey      = 0x01;
const TUint8 KTlvTextAttributeForegroundColourDarkRed       = 0x02;
const TUint8 KTlvTextAttributeForegroundColourDarkYellow    = 0x03;
const TUint8 KTlvTextAttributeForegroundColourDarkGreen     = 0x04;
const TUint8 KTlvTextAttributeForegroundColourDarkCyan      = 0x05;
const TUint8 KTlvTextAttributeForegroundColourDarkBlue      = 0x06;
const TUint8 KTlvTextAttributeForegroundColourDarkMagenta   = 0x07;
const TUint8 KTlvTextAttributeForegroundColourGrey          = 0x08;
const TUint8 KTlvTextAttributeForegroundColourWhite         = 0x09;
const TUint8 KTlvTextAttributeForegroundColourBrightRed     = 0x0A;
const TUint8 KTlvTextAttributeForegroundColourBrightYellow  = 0x0B;
const TUint8 KTlvTextAttributeForegroundColourBrightGreen   = 0x0C;
const TUint8 KTlvTextAttributeForegroundColourBrightCyan    = 0x0D;
const TUint8 KTlvTextAttributeForegroundColourBrightBlue    = 0x0E;
const TUint8 KTlvTextAttributeForegroundColourBrightMagenta = 0x0F;

// Flags for Text Background Colour coding in Text Attribute Tlv tag data.
// See ETSI TS 123 040 in subclause 9.2.3.24.10.1.1
const TUint8 KTlvTextAttributeBackgroundColourBlack         = 0x00;
const TUint8 KTlvTextAttributeBackgroundColourDarkGrey      = 0x10;
const TUint8 KTlvTextAttributeBackgroundColourDarkRed       = 0x20;
const TUint8 KTlvTextAttributeBackgroundColourDarkYellow    = 0x30;
const TUint8 KTlvTextAttributeBackgroundColourDarkGreen     = 0x40;
const TUint8 KTlvTextAttributeBackgroundColourDarkCyan      = 0x50;
const TUint8 KTlvTextAttributeBackgroundColourDarkBlue      = 0x60;
const TUint8 KTlvTextAttributeBackgroundColourDarkMagenta   = 0x70;
const TUint8 KTlvTextAttributeBackgroundColourGrey          = 0x80;
const TUint8 KTlvTextAttributeBackgroundColourWhite         = 0x90;
const TUint8 KTlvTextAttributeBackgroundColourBrightRed     = 0xA0;
const TUint8 KTlvTextAttributeBackgroundColourBrightYellow  = 0xB0;
const TUint8 KTlvTextAttributeBackgroundColourBrightGreen   = 0xC0;
const TUint8 KTlvTextAttributeBackgroundColourBrightCyan    = 0xD0;
const TUint8 KTlvTextAttributeBackgroundColourBrightBlue    = 0xE0;
const TUint8 KTlvTextAttributeBackgroundColourBrightMagenta = 0xF0;

// Proactive command destination device Ids
const TUint8 KDeviceIdNotSet = 0x00;

#endif // CCSATCOMPONENTTESTBASE_H 

