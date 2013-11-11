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
// Base class for CTSY component tests
//

/**
 @file
 @internalAll 
*/

#ifndef CCTSYCOMPONENTTESTBASE_H
#define CCTSYCOMPONENTTESTBASE_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>
#include <etelpckt.h>

#include <test/rmockltsy.h>
#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/rmmcustomapi.h>

class CCtsyComponentTestBase : public CTestFixture
	{

protected:
	enum TExtendedErrorClient
		{
		EUseBasicError,
		EUseExtendedError
		};

	void OpenEtelServerL(TExtendedErrorClient aSetExtendedErrorClient = EUseBasicError);

	void OpenPhoneNegativeL(TInt aMode = 1);

	void OpenPhoneL();
	void OpenPhoneL(RMobilePhone::TMobilePhoneServiceTableV1& aSimServiceTable);
	
	void OpenCustomApiLC(RMmCustomAPI& aCustomAPI);
	
	void OpenPacketServiceLC(RPacketService& aPacketService, RPacketService::TStatus aPacketServiceStatus);
	TInt SetContextActiveL(TInfoName& aContextName);
	//

	// Driver calls
	void DriverExpectDialCallRequestL(TInt aCallId, RMobilePhone::TMobileService aMode, TInt aDialError, TBool aIsIsvCall = EFalse, TBool aIsSimOriginated = EFalse, TBool aPerformFdnCheck = ETrue);
	void DriverCompleteGetMobileCallInfoL(TInt aCallId, RMobilePhone::TMobileService aService);
	void DriverCompleteSuccessfulDialNotificationsL(TInt aCallId);
	void DriverCompleteCallStatusChangeNotificationsL(RArray<RMobileCall::TMobileCallStatus>& aStatuses, TInt aCallId, TInt aNotifyError, TInt aExtraDelay = 0);
	void DriverDialCallL(TInt aCallId, RMobilePhone::TMobileService aMode, TBool aIsIsvCall = EFalse, TBool aIsSimOriginated = EFalse, TBool aPerformFdnCheck = ETrue); //aIsSimOriginated is set to ETrue when the call originates from the SIM i.e. a SET UP CALL proactive command
	void DriverHangUpCallL(TInt aCallId, TInt aHangUpCause, TBool aCallHasDuration = ETrue);
	void DriverCompleteSuccessfulHangUpNotificationsL(TInt aCallId, TBool aWithInBandInfo, TInt aNotifyError, TBool aCallHasDuration = ETrue, TInt aExtraDelay = 0); //aCallDuration should be set to true if we expect an update to the phone's life timer... i.e. if the call has been connected then an update will come through
	void DriverHoldCallL(TInt aCallId, TInt aHoldError);
	void DriverDialAndHoldFirstCallDialSecondCallL(RMobilePhone::TMobileService aMobileService, TInt aCallId1, TInt aCallId2);

	void DriverPrepInternalGettersMockLtsyL();

	// Client calls
	TInt OpenLineAndCallL(RMobileLine& aMobileLine, RMobileCall& aMobileCall, RMobilePhone::TMobileService aMode);
	void ClientDialCallL(RMobileLine& aMobileLine, RMobileCall& aMobileCall, RMobilePhone::TMobileService aMode, TBool aDialEmergencyCall=EFalse, TBool aPerformFdnCheck=ETrue);
	void ClientWaitForCallStatusNotifications(RMobileCall& aMobileCall, TRequestStatus& aRequestStatus, RMobileCall::TMobileCallStatus& aCallStatus, RArray<RMobileCall::TMobileCallStatus>& aExpectedStatuses, TInt aExpectedError);
	void ClientDialAndHoldFirstCallDialSecondCallL(RMobilePhone::TMobileService aMobileService, RMobileCall& aMobileCall1, RMobileLine& aMobileLine1, RMobileCall& aMobileCall2, RMobileLine& aMobileLine2);
	void ClientHangUpCallL(RMobileCall& aMobileCall);

	//

	void AssertMockLtsyStatusL();
	
	void WaitForMockLTSYTerminated();

	// To push a CleanupItem ont the cleanupstack to close the phone
	static void Cleanup(TAny* self);
	virtual void DoCleanup();


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
    	                       const TDesC& aTelNumber);
	
	void PrintMockLtsyLog();
protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	RMockLtsy  iMockLTSY;
	TBuf<255> iLogBuf;

	}; // class CCtsyComponentTestBase


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


#endif // CCTSYCOMPONENTTESTBASE_H

