/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Etel3rdParty.dll interface, using CTelephony class defined here.
* Etel 3rd Party API Header file
* Describes the Etel 3rd Party API - classes, methods and types.
*
*/


/**
 @file
*/


#ifndef ETEL3RDPARTY_H__
#define ETEL3RDPARTY_H__

#include <e32base.h>

const TInt	KTelephonyMajorVersionNumber=7; ///< Major version-number
const TInt	KTelephonyMinorVersionNumber=0; ///< Minor version-number
const TInt	KTelephonyBuildVersionNumber=0; ///< Build version-number

// Panic numbers returned to applications. 0 upwards...
const TInt KTelephonyPanicIllegalReuse = 0;	///< Client code has attempted to re-post an asynchronous request before the original request has completed.

class CTelephonyFunctions;		// forward declaration

/**
This class provides a simple interface to the phone's telephony
system. It provides two services:

1. You can find out information about the phone. This class
provides support for retrieving Phone Settings, Line Information, Call
Functionality, Network Information and (basic) Supplementary Service
Settings.

2. You can dial, answer and control voice calls.  You cannot make fax or data calls.

Architecturally, CTelephony provides an interface to the Telephony
Multimode API.  This interface exposes only a subset of the complete
Multimode functionality.

Note that a CActiveScheduler is required to be installed for all asynchronous CTelephony calls. 

@publishedAll
@released
 */
class CTelephony : public CBase
	{
public:

//////////////////////////////////////////////////////////
//General Functionality
//////////////////////////////////////////////////////////

	/**
	Unique parameter class identifier.
	*/
	enum
		{
		/**
		Unique reference identifier for Etel 3rd Party v1.0 parameter classes.
		*/
		KEtelISVV1 = 1,
		/**
		Unique reference identifier for Etel 3rd Party v2.0 parameter classes.
		*/
		KEtelISVV2 = 2
		};

	class TEtelISVType
	/**
	Specifies which version of an API parameter a client has used.
 	For most ETel ISV v1.0/2.0 parameters this will equal KETelISV1.
	*/
		{
	public:
		IMPORT_C TInt VersionId() const;
	protected:
		TEtelISVType();
	protected:
		TInt iVersionId;
		};

	/**
	The maximum number of digits allowed in a multimode telephone number.
	*/
	enum { KMaxTelNumberSize = 100 };

	/**
	Address types.
	*/
	enum TTelNumberTON
		{
		/**
		User or the network has no knowledge of the type of number.
	    */
		EUnknownNumber,
		/**
		International number.
	    */
		EInternationalNumber,
		/**
		National number.
	    */
		ENationalNumber,
		/**
		Administration/service number specific to the serving network, e.g. used to
		access an operator.
	    */
		ENetworkSpecificNumber,
		/**
		Subscriber number.
	    */
		ESubscriberNumber,
		/**
		Alphanumeric number.
	    */
		EAlphanumericNumber,
		/**
		Abbreviated number.
	    */
		EAbbreviatedNumber
		};

	/**
	Number Plan Indicator.
	*/
	enum TTelNumberNPI
		{
		/**
		User or the network has no knowledge of the numbering plan.
	    */
		EUnknownNumberingPlan		= 0,
		/**
		ISDN/telephony numbering plan.
	    */
		EIsdnNumberPlan				= 1,
		/**
		Data numbering plan.
	    */
		EDataNumberPlan				= 3,
		/**
		Telex numbering plan.
	    */
		ETelexNumberPlan			= 4,
		/**
		Service centre specific plan used to indicate a numbering plan specific to external
		Short Message entities attached to the SMSC.
	    */
		EServiceCentreSpecificPlan1	= 5,
		/**
		Service centre specific plan used to indicate a numbering plan specific to external
		Short Message entities attached to the SMSC.
	    */
		EServiceCentreSpecificPlan2	= 6,
		/**
		National numbering plan.
	    */
		ENationalNumberPlan			= 8,
		/**
		Private numbering plan.
	    */
		EPrivateNumberPlan			= 9,
		/**
		ERMES numbering plan.
	    */
		EERMESNumberPlan			= 10
		};

	/**
	A typedef to hold the telephone number.
	*/
	typedef TBuf<KMaxTelNumberSize> TTelNumber;

	class TTelAddress
	/**
	Defines API abstraction of a mobile telephone number.
	*/
		{
	public:
		IMPORT_C TTelAddress();
	public:
		/**
		Type of number.
		*/
		TTelNumberTON 				iTypeOfNumber;
		/**
		Number plan.
		*/
		TTelNumberNPI 				iNumberPlan;
		/**
		Telephone number.
		*/
		TTelNumber					iTelNumber;
		};

//////////////////////////////////////////////////////////
// Phone Functionality
//////////////////////////////////////////////////////////

	/**
	Max size of Manufacturer Id.
	*/
	enum { KPhoneManufacturerIdSize = 50 };
	/**
	Max size of Phone Model Id.
	*/
	enum { KPhoneModelIdSize 		= 50 };
	/**
	Max size ofSerial Number.
	*/
	enum { KPhoneSerialNumberSize 	= 50 };


	class TPhoneIdV1 : public TEtelISVType
	/**
	Defines the mobile phone identity.
	*/
		{
	public:
		IMPORT_C TPhoneIdV1();
	public:
		/**
		Phone manufacturer identification, in character string format.
		*/
		TBuf<KPhoneManufacturerIdSize> 	iManufacturer;
		/**
		Phone model identification, in character string format.
		*/
		TBuf<KPhoneModelIdSize> 		iModel;
		/**
		Phone serial number (IMEI or ESN), in character string format.
		*/
		TBuf<KPhoneSerialNumberSize> 	iSerialNumber;
		};

	/**
	A typedef'd packaged CTelephony::TPhoneIdV1 for passing through a generic API method
	*/
	typedef TPckg<TPhoneIdV1> TPhoneIdV1Pckg;

	/**
	Maximum size of IMSI identifier.
	*/
	enum { KIMSISize = 15 };

	class TSubscriberIdV1 : public TEtelISVType
	/**
	Defines the Subscriber (IMSI) Id
	*/
		{
	public:
		IMPORT_C TSubscriberIdV1();
	public:
		/**
		IMSI identifier.
		*/
		TBuf<KIMSISize> iSubscriberId;
		};

	/**
	A typedef'd packaged CTelephony::TSubscriberIdV1 for passing through a generic API method.
	*/
	typedef TPckg<TSubscriberIdV1> TSubscriberIdV1Pckg;

	/**
	The flight mode status.
	*/
	enum TFlightModeStatus
		{
		/**
		Flight mode is off.
		*/
		EFlightModeOff	= 0,
		/**
		Flight mode is on.
		*/
		EFlightModeOn	= 1,
		};

	class TFlightModeV1 : public TEtelISVType
	/**
	Defines the flight mode status.
	*/
		{
	public:
		IMPORT_C TFlightModeV1();
	public:
		/**
		The current status of the mobile radio interface and bluetooth.
		*/
		TFlightModeStatus iFlightModeStatus;
		};

	/**
	A typedef'd packaged CTelephony::TFlightModeV1 for passing through a generic API method.
	*/
	typedef TPckg<TFlightModeV1> TFlightModeV1Pckg;

	/**
	The mobile phone indicators.
	*/
	enum TPhoneIndicators
		{
		/**
		If bit-flag is set to '1' indicates that the battery charger
		is connected to the phone. If bit-flag is set to '0' indicates
		that the battery charger is disconnected

		For capability: If bit-flag is set to '0' indicates that the
		battery charger indication reporting is unavailable.
		*/
		KIndChargerConnected	= 0x00000001,
		/**
		If bit-flag is set to '1' indicates that network service is
		available. If bit-flag is set to '0' indicates that network
		service is unavailable

		For capability: If bit-flag is set to '0' indicates that the
		network availability indication reporting is unavailable.
		*/
		KIndNetworkAvailable	= 0x00000002,
		/**
		If bit-flag is set to '1' indicates that a call is in
		progress. If set to '0' indicates that a call is not in
		progress

		For capability: If bit-flag is set to '0' indicates
		that the call is in progress indication reporting is
		unavailable.
		*/
		KIndCallInProgress		= 0x00000004
		};

	class TIndicatorV1 : public TEtelISVType
	/**
	Contains indicator parameters:
	@see TPhoneIndicators
	*/
		{
	public:
		IMPORT_C TIndicatorV1();
	public:
		/**
		The value of the indicators.  It is the sum
		of CTelephony::TPhoneIndicators constants.
		*/
		TUint32 iIndicator;
		/**
		The supported (available) indicator capability that the telephony service module offers.
		It is the sum of CTelephony::TPhoneIndicators constants.
		*/
		TUint32 iCapabilities;
		};

	/**
	A typedef'd packaged CTelephony::TIndicatorV1 for passing through a generic API method.
	*/
	typedef TPckg<TIndicatorV1> TIndicatorV1Pckg;

	/**
	The mobile phone battery status.
	*/
	enum TBatteryStatus
		{
		/**
		The phone software can not determine the phone's current power status.
		*/
		EPowerStatusUnknown,
		/**
		The phone is powered by the battery.
		*/
		EPoweredByBattery,
		/**
		A battery is connected, but the phone is externally powered.
		*/
		EBatteryConnectedButExternallyPowered,
		/**
		No battery is connected.
		*/
		ENoBatteryConnected,
		/**
		Power fault.
		*/
		EPowerFault
		};

	class TBatteryInfoV1 : public TEtelISVType
	/**
	Defines contents of the battery status of the phone.
	*/
		{
	public:
		IMPORT_C TBatteryInfoV1();
	public:
		/**
		The power and battery status.
		*/
		TBatteryStatus iStatus;
		/**
		The percentage battery charge level.
		*/
		TUint iChargeLevel;
		};

	/**
	A typedef'd packaged CTelephony::TBatteryInfoV1 for passing through a generic API method.
	*/
	typedef TPckg<TBatteryInfoV1> TBatteryInfoV1Pckg;

	class TSignalStrengthV1 : public TEtelISVType
	/**
	Defines the phone's current signal strength.
	*/
		{
	public:
		IMPORT_C TSignalStrengthV1();
	public:
		/**
		The value of signal strength.
		*/
		TInt32 iSignalStrength;
		/**
		The absolute number of signal "bars" that the phone should display.
		*/
		TInt8 iBar;
		};

	/**
	A typedef'd packaged CTelephony::TSignalStrengthV1 for passing through a generic API method.
	*/
	typedef TPckg<TSignalStrengthV1> TSignalStrengthV1Pckg;

	/**
	Pin 1/Pin 2 security
	*/
	enum TIccLock
		{
		/**
		Lock PIN1 on the ICC.
		*/
		ELockPin1,
		/**
		Lock PIN2 on the ICC.
		*/
		ELockPin2
		};

	/**
	Status of an ICC lock.
	*/
	enum TIccLockStatus
		{
		/**
		The status of the lock is unknown.
		*/
		EStatusLockUnknown,
		/**
		Lock is closed.

		User can not access functionality governed by this lock until
		user correctly enters associated security code.
		*/
		EStatusLocked,
		/**
		Lock is open.

		User can access functionality governed by this lock
		*/
		EStatusUnlocked,
		/**
		Lock is blocked.

		User should enter the unblocking code to be able to switch back to the unlocked mode.
		Blocking is enabled after a number of unsuccessful attempts to verify PIN1/2.
		*/
		EStatusBlocked
		};

	/**
	Setting of the ICC lock.
	*/
	enum TIccLockSetting
		{
		/**
		The setting of the lock is unknown.
		*/
		ELockSetUnknown,
		/**
		Lock is enabled, and is available for use. Its status may be
		CTelephony::EStatusLocked, CTelephony::EStatusUnlocked,
		or CTelephony::EStatusBlocked.

		The associated security code will be required to unlock the lock
		the next time the lock's status is CTelephony::EStatusLocked.
		*/
		ELockSetEnabled,
		/**
		Lock is disabled.  It is not available for use, so its status
		is always CTelephony::EStatusUnlocked.
		*/
		ELockSetDisabled
		};

	class TIccLockInfoV1 : public TEtelISVType
	/**
	Defines the Icc Lock parameters.
	*/
		{
	public:
		IMPORT_C TIccLockInfoV1();
	public:
		/**
		The current status of the lock.
		*/
		TIccLockStatus iStatus;
		/**
		The current availability of the lock. When not available (i.e. the
		lock is not in use) then its status will always be CTelephony::EStatusUnlocked.
		*/
		TIccLockSetting iSetting;
		};

	/**
	A typedef'd packaged CTelephony::TIccLockInfoV1 for passing through a generic API method.
	*/
	typedef TPckg<TIccLockInfoV1> TIccLockInfoV1Pckg;

//////////////////////////////////////////////////////////
// Line Functionality
//////////////////////////////////////////////////////////

	/**
	Line types
	*/
	enum TPhoneLine
		{
		/**
		Voice line.
		*/
		EVoiceLine,
		/**
		Data line.
		*/
		EDataLine,
		/**
		Fax line.
		*/
		EFaxLine,
		};

	/**
	Describes the possible call or line states.
	*/
	enum TCallStatus
		{
		/**
		Indicates that the status is unknown.
		*/
		EStatusUnknown,
		/**
		Idle line status (no active calls).
		*/
		EStatusIdle,
		/**
		Call dialling status .
		*/
		EStatusDialling,
		/**
		Call ringing status.
		*/
		EStatusRinging,
		/**
		Call answering status.
		*/
		EStatusAnswering,
		/**
		Call connecting status.
		*/
		EStatusConnecting,
		/**
		Call connected status.
		*/
		EStatusConnected,
		/**
		Call is undergoing temporary channel loss and it may or may not be reconnected.
		*/
		EStatusReconnectPending,
		/**
		Call disconnecting status.
		*/
		EStatusDisconnecting,
		/**
		Call on hold.
		*/
		EStatusHold,
		/**
		Call is transferring.
		*/
		EStatusTransferring,
		/**
		Call in transfer is alerting the remote party.
		*/
		EStatusTransferAlerting
		};

	class TCallStatusV1 : public TEtelISVType
	/**
	Defines the current call or line status.
	*/
		{
	public:
		IMPORT_C TCallStatusV1();
	public:
		/**
		The current call or line status.
		*/
		TCallStatus iStatus;
		};

	/**
	A typedef'd packaged CTelephony::TCallStatusV1 for passing through a generic API method.
	*/
	typedef TPckg<TCallStatusV1> TCallStatusV1Pckg;

	/**
	Select a call defined by it current stats.
	*/
	enum TCallSelect
		{
		/**
		Currently active call.
		*/
		EActiveCall,
		/**
		Currently held (applicable only for voice) call.
		*/
		EHeldCall,
		/**
		Call during the setup/disconnecting process - (dialling/connecting/answering/hanging up).
		*/
		EInProgressCall
		};

	class TCallSelectionV1 : public TEtelISVType
	/**
	Defines parameters to select a call, determined by its call state for a specific line
	*/
		{
	public:
		IMPORT_C TCallSelectionV1();
	public:
		/**
		The current phone line selection.
		*/
		TPhoneLine iLine;
		/**
		The current call selection.
		*/
		TCallSelect iSelect;
		};

	/**
	A typedef'd packaged CTelephony::TCallSelectionV1 for passing through a generic API method.
	*/
	typedef TPckg<TCallSelectionV1> TCallSelectionV1Pckg;

	/**
	Etel 3rd Party owned call identifiers
	*/
	enum TCallId
		{
		/**
		Call owned by this ISV application.
		*/
		EISVCall1,
		/**
		Call owned by this ISV application.
		*/
		EISVCall2,
		/**
		Max number of calls supported by Etel 3rd Party.
		*/
		EISVMaxNumOfCalls
		};

	/**
	Remote party identity status.
	*/
	enum TCallRemoteIdentityStatus
		{
		/**
		The remote party's identity can not be determined.
		*/
		ERemoteIdentityUnknown,
		/**
		The remote party's identity is available.
		*/
		ERemoteIdentityAvailable,
		/**
		The remote party has suppressed the transmission of its identity.
		*/
		ERemoteIdentitySuppressed
		};

	/**
	The direction of the call.
	*/
	enum TCallDirection
		{
		/**
		The direction of the call is unknown.
		*/
		EDirectionUnknown,
		/**
		The call was originated by this phone, i.e. it is an outgoing call.
		*/
		EMobileOriginated,
		/**
		The call was terminated by this phone, i.e. it is an incoming call.
		*/
		EMobileTerminated
		};

	/**
	Enumerated network security types.
	*/
	enum TPhoneNetworkSecurity
		{
		/**
		The encryption level is NONE.
		*/
		ECipheringOff,
		/**
		The encryption level is GSM.(standard encryption algorithms for 2nd Generation Mobile networks).
		*/
		ECipheringGSM,
		/**
		The encryption level is WDCMA.(standard encryption algorithms for 3rd Generation Mobile networks).
		*/
		ECipheringWCDMA
		};

	class TCallInfoV1 : public TEtelISVType
	/**
	Defines general information about a call.
	*/
		{
	public:
		IMPORT_C TCallInfoV1();
	public:
		/**
		The current status of the call.
		*/
		TCallStatus iStatus;
		/**
		The time & date the call started.
		*/
		TDateTime iStartTime;
		/**
		The current duration of the call.
		*/
		TTimeIntervalSeconds iDuration;
		/**
		The original number (including DTMF) dialled for an outgoing call.
		*/
		TTelAddress iDialledParty;
		/**
		The reason for termination of a finished call.
		Will equal KErrNone if the call ended normally and KErrNotFound if the call has not ended.
		*/
		TInt iExitCode;
		/**
		This attribute indicates whether Ciphering Status of a Call is enabled or disabled.
		*/
		TPhoneNetworkSecurity iSecurity;
		/**
		The call id of an ISV application owned call. For calls not owned by this ISV application (-1) is returned.
		*/
		TInt iCallId;
		};

	/**
	A typedef'd packaged CTelephony::TCallInfoV1 for passing through a generic API method.
	*/
	typedef TPckg<TCallInfoV1> TCallInfoV1Pckg;

	/**
	Maximum size of the calling party name.
	*/
	enum { KCallingNameSize = 80 };

	class TRemotePartyInfoV1 : public TEtelISVType
		{
		/**
		Defines information about the remote party of a call.
		*/
	public:
		IMPORT_C TRemotePartyInfoV1();
	public:
		/**
		Indicates whether the remote party information in the rest of this structure is valid or not.
		*/
		TCallRemoteIdentityStatus iRemoteIdStatus;
		/**
		Calling party name available through the CNAP supplementary service (if provisioned).
		*/
		TBuf<KCallingNameSize> iCallingName;
		/**
		The phone number of the remote party if available.
		*/
		TTelAddress iRemoteNumber;
		/**
		The direction of the call and hence the role of the remote party.
		i.e. if the call is mobile originated then the remote party is the called party.
		Whereas if the call is mobile terminated then the remote party is the calling party.
		*/
		TCallDirection iDirection;
		};

	/**
	A typedef'd packaged CTelephony::TRemotePartyInfoV1 for passing through a generic API method.
	*/
	typedef TPckg<TRemotePartyInfoV1> TRemotePartyInfoV1Pckg;

//////////////////////////////////////////////////////////
// Call Functionality
//////////////////////////////////////////////////////////

	/**
	Caller Id restriction settings.
	*/
	enum TCallerIdentityRestrict
		{
		/**
		The default setting should be used if the user has not explicitly requested their identity to be restricted/allowed.
		*/
		EIdRestrictDefault,
		/**
		The user has explicitly requested their identity to be sent for this call.
		*/
		ESendMyId,
		/**
		The user has explicitly requested their identity not to be sent for this call.
		*/
		EDontSendMyId
		};

	class TCallParamsV1 : public TEtelISVType
		{
		/**
		Defines the parameters used for set-up of a call.
		*/
	public:
		IMPORT_C TCallParamsV1();
	public:
		/**
		Call Id restriction setting to be used for this call.
		*/
		TCallerIdentityRestrict iIdRestrict;
		};

	/**
	A typedef'd packaged CTelephony::TCallParamsV1 for passing through a generic API method.
	*/
	typedef TPckg<TCallParamsV1> TCallParamsV1Pckg;

	/**
	Mobile call control capabilities.
	*/
	enum TMobileCallControlCaps
		{
		/**
		Indicates that this call can be put on hold.
		This implies that the call is currently active and that there is no other held call.
		*/
		KCapsHold 	= 0x00000200,
		/**
		Indicates that this call can be resumed.
		This implies that the call is currently on hold and that there is no other active call.
		*/
		KCapsResume = 0x00000400,
		/**
		Indicates that this call's state can be swapped to the opposite state.
		This implies that this call is either active or held.
		There may be another call in the opposite state and if this is the case then both calls will be simultaneously swapped to their opposite state.
		*/
		KCapsSwap 	= 0x00000800
		};

	class TCallCapsV1 : public TEtelISVType
		{
		/**
		Defines the dynamic capabilities of a call.
		@see TMobileCallControlCaps
		*/
	public:
		IMPORT_C TCallCapsV1();
	public:
		/**
		Call Id restriction setting to be used for this call.
		It is the sum of CTelephony::TMobileCallControlCaps constants.
		*/
		TUint32 iControlCaps;
		};

	/**
	A typedef'd packaged CTelephony::TCallParamsV1 for passing through a generic API method.
	*/
	typedef TPckg<TCallCapsV1> TCallCapsV1Pckg;


//////////////////////////////////////////////////////////
// Network Functionality
//////////////////////////////////////////////////////////

	/**
	The registration status of the phone.
	*/
	enum TRegistrationStatus
		{
		/**
		Registration status is unknown.
		*/
		ERegistrationUnknown,
		/**
		Not registered. The ME can not detect any other networks and is not currently searching a new operator to register to.
		*/
		ENotRegisteredNoService,
		/**
		Not registered. The ME can detect other networks on which it is possible to make emergency calls only.
		*/
		ENotRegisteredEmergencyOnly,
		/**
		Not registered, but the ME is currently searching a new operator to register to.
		*/
		ENotRegisteredSearching,
		/**
		Registered, network busy.
		*/
		ERegisteredBusy,
		/**
		Registered on home network.
		*/
		ERegisteredOnHomeNetwork,
		/**
		Registration denied.
		*/
		ERegistrationDenied,
		/**
		Registered, roaming.
		*/
		ERegisteredRoaming
		};

	class TNetworkRegistrationV1 : public TEtelISVType
		/**
		Defines the current network registration status
		*/
		{
	public:
		IMPORT_C TNetworkRegistrationV1();
	public:
		/**
        The current network registration status.
		*/
		TRegistrationStatus iRegStatus;
		};

	/**
	A typedef'd packaged CTelephony::TNetworkRegistrationV1 for passing through a generic API method.
	*/
	typedef TPckg<TNetworkRegistrationV1> TNetworkRegistrationV1Pckg;

	/***********************************************/
	// CTelephony::TNetworkInfoV1

	/**
	Maximum size of the network country code.
	*/
	enum { KNetworkCountryCodeSize 	= 4  };
	/**
	Maximum size of the network identity..
	*/
	enum { KNetworkIdentitySize 	= 8  };
	/**
	Maximum size of the network display tag..
	*/
	enum { KNetworkDisplayTagSize 	= 32 };
	/**
	Maximum size of the short network name.
	*/
	enum { KNetworkShortNameSize 	= 8  };
	/**
	Maximum size of the long network name.
	*/
	enum { KNetworkLongNameSize 	= 16 };
	/**
	Maximum size of the short network name version 2.
	*/
	enum { KNetworkShortNameSizeV2 	= 10  };
	/**
	Maximum size of the long network name version 2.
	*/
	enum { KNetworkLongNameSizeV2 	= 20 };

	/**
	Defines Current Network Modes.
	*/
	enum TNetworkMode
		{
		/**
		Network mode is unknown.
		*/
		ENetworkModeUnknown,
		/**
		Mobile device is not registered.
		*/
		ENetworkModeUnregistered,
		/**
		GSM/GPRS or DCS1800 network.
		*/
		ENetworkModeGsm,
		/**
		AMPS network.
		*/
		ENetworkModeAmps,
		/**
		CDMA (IS-95) network.
		*/
		ENetworkModeCdma95,
		/**
		CDMA (cdma2000) network.
		*/
		ENetworkModeCdma2000,
		/**
		WCDMA (UTRA Frequency Division Duplex (FDD)) network.
		*/
		ENetworkModeWcdma,
		/**
		TD-CDMA (UTRA Time Division Duplex (TDD)) network.
		 */
		ENetworkModeTdcdma
		};

	/**
	The access technology that the network is based on.
	*/
	enum TNetworkAccess
		{
		/**
		This is used when there is no network activity and therefore no RAT active.
		*/
		ENetworkAccessUnknown,
		/**
		The access technology is GSM.
		*/
		ENetworkAccessGsm,
		/**
		The access technology is GSM COMPACT.
		However GSM COMPACT systems which use GSM frequency bands but with the CBPCCH broadcast channel are considered as a separate access technology from GSM.
		*/
		ENetworkAccessGsmCompact,
		/**
		The access technology is UTRAN (UMTS Network).
		*/
		ENetworkAccessUtran
		};

	/**
	Phone network status.
	*/
	enum TNetworkStatus
		{
		/**
		Status is unknown.
		*/
		ENetworkStatusUnknown,
		/**
		A network that the mobile device is allowed to register to.
		*/
		ENetworkStatusAvailable,
		/**
		Currently registered network.
		*/
		ENetworkStatusCurrent,
		/**
		A network that the ME is not allowed to register to.
		*/
		ENetworkStatusForbidden
		};

	/**
	Mobile phone network band information.
	*/
	enum TNetworkBandInfo
		{
		/**
		The current band and band class is unknown.
		*/
		EBandUnknown,
		/**
		The network operates at 800MHz on Band A.
		*/
		E800BandA,
		/**
		The network operates at 800MHz on Band B.
		*/
		E800BandB,
		/**
		The network operates at 800MHz on Band C.
		*/
		E800BandC,
		/**
		The network operates at 1900MHz on Band A.
		*/
		E1900BandA,
		/**
		The network operates at 1900MHz on Band B.
		*/
		E1900BandB,
		/**
		The network operates at 1900MHz on Band C.
		*/
		E1900BandC,
		/**
		The network operates at 1900MHz on Band D.
		*/
		E1900BandD,
		/**
		The network operates at 1900MHz on Band E.
		*/
		E1900BandE,
		/**
		The network operates at 1900MHz on Band F.
		*/
		E1900BandF
		};

	/**
	Defines information related to a mobile phone network.

	@see CTelephony::TNetworkInfoV2
	*/
	class TNetworkInfoV1 : public TEtelISVType
		{
	public:
		IMPORT_C TNetworkInfoV1();
	public:
		/**
		Mode of the network.
		*/
		TNetworkMode iMode;
		/**
		Status of the network
		*/
		TNetworkStatus iStatus;
		/**
		The MCC of the network.
		*/
		TBuf<KNetworkCountryCodeSize> 	iCountryCode;
		/**
		The network identity (NID in CDMA and MNC in GSM).
		*/
		TBuf<KNetworkIdentitySize> 		iNetworkId;
		/**
		The alpha-tag displayed when this is the serving network.
		*/
		TBuf<KNetworkDisplayTagSize> 	iDisplayTag;
		/**
		On GSM/WCDMA networks, the short name (up to 8 characters) of the network operator.
		*/
		TBuf<KNetworkShortNameSize> 	iShortName;
		/**
		On CDMA networks, the band and band class of the CDMA network operator.
		*/
		TNetworkBandInfo iBandInfo;
		/**
		On CDMA networks, the system identity (SID) of the CDMA or AMPS network
		*/
		TBuf<KNetworkIdentitySize> 		iCdmaSID;
		/**
		On GSM/WCDMA networks, the long name (up to 16 characters) of the network operator.
		*/
		TBuf<KNetworkLongNameSize> 		iLongName;
		/**
		On GSM/WCDMA networks, the type of network access.
		*/
		TNetworkAccess iAccess;
		/**
		On GSM/WCDMA networks, boolean indicating whether the location area is known.
		*/
		TBool iAreaKnown;
		/**
		On GSM/WCDMA networks, the location area code.  Only valid when iAreaKnown is true.		
		*/
		TUint iLocationAreaCode;
		/**
		On GSM/WCDMA networks, the cell identity code.  Only valid when iAreaKnown is true.		
		*/
		TUint iCellId;
		};

	/**
	A typedef'd packaged CTelephony::TNetworkInfoV1 for passing through a generic API method.
	*/
	typedef TPckg<TNetworkInfoV1> TNetworkInfoV1Pckg;

	class TNetworkNameV1 : public TEtelISVType
	/**
	Defines the name network name.

    Note: This information is only available on GSM/WCDMA networks.
	*/
		{
	public:
		IMPORT_C TNetworkNameV1();
	public:
		/**
		The displayed name (up to 16 characters) of the network provider.
		*/
		TBuf<KNetworkLongNameSize> 		iNetworkName;
		};

	/**
	A typedef'd packaged CTelephony::TNetworkNameV1 for passing through a generic API method.
	*/
	typedef TPckg<TNetworkNameV1> TNetworkNameV1Pckg;

	class TOperatorNameV1 : public TEtelISVType
	/**
	Defines the current operator.

    Note: This information is only available on GSM/WCDMA networks.
	*/
		{
	public:
		IMPORT_C TOperatorNameV1();
	public:
		/**
		 The displayed name (up to 16 characters) of the network operator.
		*/
		TBuf<KNetworkLongNameSize> 		iOperatorName;
		};

	/**
	A typedef'd packaged CTelephony::TOperatorNameV1 for passing through a generic API method.
	*/
	typedef TPckg<TOperatorNameV1> TOperatorNameV1Pckg;


	/**
	Defines information related to a mobile phone network.
	TNetworkInfoV2 differs from Version 1 (TNetworkInfoV1) in that the
	length of iShortName and iLongName are 10 and 20 respectively
	(rather than 8 and 16).

	TNetworkInfoV2 should be used in preference to TNetworkInfoV1.  The
	short and long network names provided in this V2 class are of the
	length supplied by the platform.  The V1 class's names will be
	truncated if the platform provides names longer than its limits.
	*/
	class TNetworkInfoV2 : public TEtelISVType
		{
	public:
		IMPORT_C TNetworkInfoV2();
	public:
		/**
		Mode of the network.
		*/
		TNetworkMode iMode;
		/**
		Status of the network.
		*/
		TNetworkStatus iStatus;
		/**
		The MCC of the network.
		*/
		TBuf<KNetworkCountryCodeSize>    iCountryCode;
		/**
		The network identity (NID in CDMA and MNC in GSM).
		*/
		TBuf<KNetworkIdentitySize>       iNetworkId;
		/**
		The alpha-tag displayed when this is the serving network.
		*/
		TBuf<KNetworkDisplayTagSize>     iDisplayTag;
		/**
		On GSM/WCDMA networks, the short name (up to 10 characters) of the network operator.
		*/
		TBuf<KNetworkShortNameSizeV2>    iShortName;
		/**
		On CDMA networks, the band and band class of the CDMA network operator.
		*/
		TNetworkBandInfo                 iBandInfo;
		/**
		On CDMA networks, the system identity (SID) of the CDMA or AMPS network.
		*/
		TBuf<KNetworkIdentitySize>       iCdmaSID;
		/**
		On GSM/WCDMA networks, the long name (up to 20 characters) of the network operator.
		*/
		TBuf<KNetworkLongNameSizeV2>     iLongName;
		/**
		On GSM/WCDMA networks, the type of network access.
		*/
		TNetworkAccess                   iAccess;
		/**
		On GSM/WCDMA networks, boolean indicating whether the location area is known.
		*/
		TBool                            iAreaKnown;
		/**
		On GSM/WCDMA networks, the location area code.  Only valid when iAreaKnown is true.		
		*/
		TUint                            iLocationAreaCode;
		/**
		On GSM/WCDMA networks, the cell identity code.  Only valid when iAreaKnown is true.		
		*/
		TUint                            iCellId;
		};

	/**
	A typedef'd packaged CTelephony::TNetworkInfoV2 for passing
	through a generic API method.
	*/
	typedef TPckg<TNetworkInfoV2> TNetworkInfoV2Pckg;

	/**
	Defines the name network name.
	TNetworkNameV2 differs from Version 1 (TNetworkNameV1) in that the
	length of iNetworkName is 20 (rather than 16).

	TNetworkNameV2 should be used in preference to TNetworkNameV1.  The
	network name provided in this V2 class is of the length supplied
	by the platform.  The V1 class' network name will be truncated if
	the platform provides a name longer than V1 name's limit.

   Note: This information is only available on GSM/WCDMA networks.
	*/
	class TNetworkNameV2 : public TEtelISVType
		{
	public:
		IMPORT_C TNetworkNameV2();
	public:
		/**
		The displayed name (up to 20 characters) of the network provider.
		*/
		TBuf<KNetworkLongNameSizeV2> iNetworkName;
		};

	/**
	A typedef'd packaged CTelephony::TNetworkNameV2 for passing through
	a generic API method.  TNetworkNameV2 differs from Version 1 in
	that the length of iNetworkName is 20 rather than 16.
	*/
	typedef TPckg<TNetworkNameV2> TNetworkNameV2Pckg;

//////////////////////////////////////////////////////////
// (Basic) Supplementary Services Functionality
//////////////////////////////////////////////////////////

	/**
	Supplementary Service status.
	*/
	enum TSupplServiceStatus
		{
		/**
		The supplementary service is currently active and operative.
		*/
		EStatusActive,
		/**
		The supplementary service is currently deactivated or quiescent.
		*/
		ENotActive,
		/**
		In GSM/WCDMA mode, the supplementary service is not provisioned.
		In CDMA mode, this value has no meaning.
		*/
		ENotProvisioned,
		/**
		In GSM/WCDMA mode, the supplementary service is not available in serving network.
		In CDMA mode, the supplementary service is not available in the phone.
		*/
		ENotAvailable,
		/**
		The phone can not determine supplementary service status.
		*/
		EUnknown
		};

	/**
	Call Forwarding conditions.
	*/
	enum TCallForwardingCondition
		{
		/**
		All calls to this subscriber are forwarded.
		*/
		ECallForwardingUnconditional,
		/**
		Calls are forwarded when this subscriber is busy.
		*/
		ECallForwardingBusy,
		/**
		Calls are forwarded when this subscriber does not reply within a timeout period.
		*/
		ECallForwardingNoReply,
		/**
		Calls are forwarded when this subscriber is unreachable.
		*/
		ECallForwardingNotReachable
		};

	class TCallForwardingSupplServicesV1 : public TEtelISVType
	/**
	Defines information about the call forwarding service.

    Note: This information is only available on GSM/WCDMA networks.
	*/
		{
	public:
		IMPORT_C TCallForwardingSupplServicesV1();
	public:
		/**
		The status of the call forwarding supplementary service.
		It applies to the condition in iCallForwardingCondition and
		to the line specified to CTelephony::GetCallForwardingStatus().
		*/
		TSupplServiceStatus iCallForwarding;
		/**
		The provisioned call forwarding condition to which iCallForwarding applies.
		*/
		TCallForwardingCondition iCallForwardingCondition;
		};

	/**
	A typedef'd packaged CTelephony::TCallForwardingSupplServicesV1 for passing through a generic API method.
	*/
	typedef TPckg<TCallForwardingSupplServicesV1> TCallForwardingSupplServicesV1Pckg;


	/**
	Call Barring conditions.
	*/
	enum TCallBarringCondition
		{
		/**
		All incoming calls to this subscriber are barred (BAIC).
		*/
		EBarAllIncoming,
		/**
		All incoming calls to this subscriber are barred when roaming outside the home PLMN country (BAIC-roam).
		*/
		EBarIncomingRoaming,
		/**
		All outgoing calls by this subscriber are barred (BAOC).
		*/
		EBarAllOutgoing,
		/**
		All outgoing international calls by this subscriber are barred (BOIC).
		*/
		EBarOutgoingInternational,
		/**
		All outgoing international calls except those directed to the home PLMN country by this subscriber are barred (BOIC-ExHC).
		*/
		EBarOutgoingInternationalExHC
		};

	class TCallBarringSupplServicesV1 : public TEtelISVType
	/**
	Defines information about the call barring service.
	*/
		{
	public:
		IMPORT_C TCallBarringSupplServicesV1();
	public:
		/**
		The status of the call barring supplementary service.
		It applies to the condition in iCallBarringCondition and
		to the service group specified to CTelephony::GetCallBarringStatus().
		*/
		TSupplServiceStatus iCallBarring;
		/**
	    The provisioned call barring condition to which iCallBarring applies.
		*/
		TCallBarringCondition iCallBarringCondition;
		};

	/**
	A typedef'd packaged CTelephony::TCallBarringSupplServicesV1 for passing through a generic API method.
	*/
	typedef TPckg<TCallBarringSupplServicesV1> TCallBarringSupplServicesV1Pckg;

	class TCallWaitingSupplServicesV1 : public TEtelISVType
	/**
	Defines information about the call waiting service.
	*/
		{
	public:
		IMPORT_C TCallWaitingSupplServicesV1();
	public:
		/**
		The status of the call waiting supplementary service
		for the service group specified to CTelephony::GetCallWaitingStatus().
		*/
		TSupplServiceStatus iCallWaiting;
		};

	/**
	A typedef'd packaged CTelephony::TCallWaitingSupplServicesV1 for passing through a generic API method.
	*/
	typedef TPckg<TCallWaitingSupplServicesV1> TCallWaitingSupplServicesV1Pckg;

	/**
	Phone ID services.
	*/
	enum TIdentityService
		{
		/**
		The identity service is unspecified.
		*/
		EIdServiceUnspecified,
		/**
		The caller's ID is presented to the called party.
		*/
		EIdServiceCallerPresentation,
		/**
		The caller's ID is restricted to the called party.
		*/
		EIdServiceCallerRestriction
		};

	/**
	Service status of the Phone ID services.
	*/
	enum TIdentityServiceStatus
		{
		/**
		The interrogated identity service is provisioned and permanently active.
		*/
		EIdServiceActivePermanent,
		/**
		The interrogated identity service is provisioned and active.
		By default, the number is restricted unless overridden by the user.
		*/
		EIdServiceActiveDefaultRestricted,
		/**
		The interrogated identity service is provisioned and active.
		By default, the number is displayed unless specifically restricted by the user.
		*/
		EIdServiceActiveDefaultAllowed,
		/**
		The interrogated identity service is not provisioned.
		*/
		EIdServiceNotProvisioned,
		/**
		The status of the interrogated Identity service is unknown.
		*/
		EIdServiceUnknown
		};

	class TIdentityServiceV1 : public TEtelISVType
	/**
	Defines the call identity service status.

    Note: This information is only available on GSM/WCDMA networks.
	*/
		{
	public:
		IMPORT_C TIdentityServiceV1();
	public:
		/**
		The status of the call identity service.
		*/
		TIdentityServiceStatus iIdentityStatus;
		};

	/**
	A typedef'd packaged CTelephony::TIdentityServiceV1 for passing through a generic API method.
	*/
	typedef TPckg<TIdentityServiceV1> TIdentityServiceV1Pckg;

	/**
	Service group identifier.
	*/
	enum TServiceGroup
		{
		/**
		Voice service group
		*/
		EVoiceService,
		/**
		Fax service group
		*/
		EFaxService,
		/**
		Data service group
		*/
		EDataService
		};

	/**
	Cancel apending request
	*/
	enum TCancellationRequest
		{
		/**
		Cancel a pending GetPhoneId request.
		*/
		EGetPhoneIdCancel,                             // 0
		/**
		Cancel a pending GetSubscriberId request.
		*/
		EGetSubscriberIdCancel,                        // 1
		/**
		Cancel a pending GetFlightMode request.
		*/
		EGetFlightModeCancel,                          // 2
		/**
		Cancel a pending FlightModeChange request.
		*/
		EFlightModeChangeCancel,                       // 3
		/**
		Cancel a pending GetIndicator request.
		*/
		EGetIndicatorCancel,                           // 4
		/**
		Cancel a pending IndicatorChange request.
		*/
		EIndicatorChangeCancel,                        // 5
		/**
		Cancel a pending GetBatteryInfo request.
		*/
		EGetBatteryInfoCancel,                         // 6
		/**
		Cancel a pending BatteryInfoChange request.
		*/
		EBatteryInfoChangeCancel,                      // 7
		/**
		Cancel a pending GetSignalStrength request.
		*/
		EGetSignalStrengthCancel,                      // 8
		/**
		Cancel a pending SignalStrengthChange request.
		*/
		ESignalStrengthChangeCancel,                   // 9
		/**
		Cancel a pending GetLockInfo request.
		*/
		EGetLockInfoCancel,                            // 10
		/**
		Cancel a pending Pin 1 LockInfoChange request.
		*/
		EPin1LockInfoChangeCancel,                     // 11
		/**
		Cancel a pending Pin 2 LockInfoChange request.
		*/
		EPin2LockInfoChangeCancel,                     // 12
		/**
		Cancel a pending VoiceLineStatusChange request.
		*/
		EVoiceLineStatusChangeCancel,                  // 13
		/**
		Cancel a pending Call1StatusChange request.
		*/
		EOwnedCall1StatusChangeCancel,                 // 14
		/**
		Cancel a pending Call2StatusChange request.
		*/
		EOwnedCall2StatusChangeCancel,                 // 15
		/**
		Cancel a pending OwnedCall1RemotePartyInfoChange request.
		*/
		EOwnedCall1RemotePartyInfoChangeCancel,        // 16
		/**
		Cancel a pending OwnedCall2RemotePartyInfoChange request.
		*/
		EOwnedCall2RemotePartyInfoChangeCancel,        // 17
		/**
		Cancel a pending SendDTMFTones request.
		*/
		ESendDTMFTonesCancel,                          // 18
		/**
		Cancel a pending DialNewCall request.
		*/
		EDialNewCallCancel,                            // 19
		/**
		Cancel a pending Hold request.
		*/
		EHoldCancel,                                   // 20
		/**
		Cancel a pending Resume request.
		*/
		EResumeCancel,                                 // 21
		/**
		Cancel a pending Swap request.
		*/
		ESwapCancel,                                   // 22
		/**
		Cancel a pending Hangup request.
		*/
		EHangupCancel,                                 // 23
		/**
		Cancel a pending AnswerIncomingCall request.
		*/
		EAnswerIncomingCallCancel,                     // 24
		/**
		Cancel a pending GetNetworkRegistrationStatus request.
		*/
		EGetNetworkRegistrationStatusCancel,           // 25
		/**
		Cancel a pending NetworkRegistrationStatusChange request.
		*/
		ENetworkRegistrationStatusChangeCancel,        // 26
		/**
		Cancel a pending GetCurrentNetworkInfo request.
		*/
		EGetCurrentNetworkInfoCancel,                  // 27
		/**
		Cancel a pending CurrentNetworkInfoChange request.
		*/
		ECurrentNetworkInfoChangeCancel,               // 28
		/**
		Cancel a pending GetCurrentNetworkName request.
		*/
		EGetCurrentNetworkNameCancel,                  // 29
		/**
		Cancel a pending GetOperatorName request.
		*/
		EGetOperatorNameCancel,                        // 30
		/**
		Cancel a pending GetCallForwardingStatus request.
		*/
		EGetCallForwardingStatusCancel,                // 31
		/**
		Cancel a pending GetCallBarringStatus request.
		*/
		EGetCallBarringStatusCancel,                   // 32
		/**
		Cancel a pending GetCallWaitingStatus request.
		*/
		EGetCallWaitingStatusCancel,                   // 33
		/**
		Cancel a pending GetIdentityServiceStatus request.
		*/
	 	EGetIdentityServiceStatusCancel,               // 34
		/**
		Cancel a pending FaxLineStatusChange request.
		*/
	 	EFaxLineStatusChangeCancel,                    // 35
		/**
		Cancel a pending DataLineStatusChange request.
		*/
	 	EDataLineStatusChangeCancel                    // 36
		};

//////////////////////////////////////////////////////////
// Notification Functionality
//////////////////////////////////////////////////////////

	/**
	Notification events
	*/
	enum TNotificationEvent
		{
		/**
		Register interest for receiving a notification for flight mode changes.

        New flight mode returned in a CTelephony::TFlightModeV1Pckg, a packaged CTelephony::TFlightModeV1.

		Pass CancelAsync() CTelephony::EFlightModeChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		EFlightModeChange,                   // 0
		/**
		Register interest for receiving a notification for phone indicator changes.

		New indicators returned in a CTelephony::TIndicatorV1Pckg, a packaged CTelephony::TIndicatorV1.

        Pass CancelAsync() CTelephony::EIndicatorChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		EIndicatorChange,                    // 1
		/**
		Register interest for receiving a notification for Battery information changes.

		New Battery information returned in a CTelephony::TBatteryInfoV1Pckg, a packaged CTelephony::TBatteryInfoV1.

        Pass CancelAsync() CTelephony::EBatteryInfoChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		EBatteryInfoChange,                  // 2
		/**
		Register interest for receiving a notification for Signal Strength changes.

		New Signal Strength returned in a CTelephony::TSignalStrengthV1Pckg, a packaged CTelephony::TSignalStrengthV1.

        Pass CancelAsync() CTelephony::ESignalStrengthChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		ESignalStrengthChange,               // 3
		/**
		Register interest for receiving a notification for Icc Pin1 Lock Information changes.

		New Icc Pin1 Lock Information returned in a CTelephony::TIccLockInfoV1Pckg, a packaged CTelephony::TIccLockInfoV1.

        Pass CancelAsync() CTelephony::EPin1LockInfoChangeCancel to cancel.
		
		@capability ReadDeviceData
		@ref CTelephony::NotifyChange
		*/
		EPin1LockInfoChange,                 // 4
		/**
		Register interest for receiving a notification for Icc Pin2 Lock Information changes.

		New Icc Pin2 Lock Information returned in a CTelephony::TIccLockInfoV1Pckg, a packaged CTelephony::TIccLockInfoV1.

        Pass CancelAsync() CTelephony::EPin2LockInfoChangeCancel to cancel.
		
		@capability ReadDeviceData
		@ref CTelephony::NotifyChange
		*/
		EPin2LockInfoChange,                 // 5
		/**
		Register interest for receiving a notification for Voice Line Status changes.

		New Voice Line Status returned in a CTelephony::TCallStatusV1Pckg, a packaged CTelephony::TCallStatusV1.

        Pass CancelAsync() CTelephony::EVoiceLineStatusChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		EVoiceLineStatusChange,              // 6
		/**
		Register interest for receiving a notification for owned Call 1 Status changes.

		New Call 1 Status returned in a CTelephony::TCallStatusV1Pckg, a packaged CTelephony::TCallStatusV1.

        Pass CancelAsync() CTelephony::EOwnedCall1StatusChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		EOwnedCall1StatusChange,             // 7
		/**
		Register interest for receiving a notification for owned Call 2 Status changes.

		New Call 2 Status returned in a CTelephony::TCallStatusV1Pckg, a packaged CTelephony::TCallStatusV1.

        Pass CancelAsync() CTelephony::EOwnedCall2StatusChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		EOwnedCall2StatusChange,             // 8
		/**
		Register interest for receiving a notification for Call 1 Remote Party Info changes.

		New Call 1 Remote Party Info returned in a CTelephony::TRemotePartyInfoV1Pckg, a packaged CTelephony::TRemotePartyInfoV1.

        Pass CancelAsync() CTelephony::EOwnedCall1RemotePartyInfoChangeCancel to cancel.
		
		@capability ReadUserData
		@ref CTelephony::NotifyChange
		*/
		EOwnedCall1RemotePartyInfoChange,    // 9
		/**
		Register interest for receiving a notification for Call 2 Remote Party Info changes.

		New Call 2 Remote Party Info returned in a CTelephony::TRemotePartyInfoV1Pckg, a packaged CTelephony::TRemotePartyInfoV1.

        Pass CancelAsync() CTelephony::EOwnedCall2RemotePartyInfoChangeCancel to cancel.
		
		@capability ReadUserData
		@ref CTelephony::NotifyChange
		*/
		EOwnedCall2RemotePartyInfoChange,    // 10
		/**
		Register interest for receiving a notification for Network registration status changes.

		New Network registration status returned in a CTelephony::TNetworkRegistrationV1Pckg, a packaged CTelephony::TNetworkRegistrationV1.

        Pass CancelAsync() CTelephony::ENetworkRegistrationStatusChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		ENetworkRegistrationStatusChange,    // 11
		/**
		Register interest for receiving a notification for Network information changes.

		New Network information returned in a CTelephony::TNetworkInfoV1Pckg, a packaged CTelephony::TNetworkInfoV1.

        Pass CancelAsync() CTelephony::ECurrentNetworkInfoChangeCancel to cancel.
		
		@capability ReadDeviceData
		@ref CTelephony::NotifyChange
		*/
		ECurrentNetworkInfoChange,           // 12
		/**
		Register interest for receiving a notification for Fax Line Status changes.

		New Fax Line Status returned in a CTelephony::TCallStatusV1Pckg, a packaged CTelephony::TCallStatusV1.

        Pass CancelAsync() CTelephony::EFaxLineStatusChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		EFaxLineStatusChange,                // 13
		/**
		Register interest for receiving a notification for Data Line Status changes.

		New Data Line Status returned in a CTelephony::TCallStatusV1Pckg, a packaged CTelephony::TCallStatusV1.

        Pass CancelAsync() CTelephony::EDataLineStatusChangeCancel to cancel.
		
		@capability None
		@ref CTelephony::NotifyChange
		*/
		EDataLineStatusChange                // 14
		};

public:
	// Constructors
	IMPORT_C static CTelephony* NewLC();
	IMPORT_C static CTelephony* NewL();

	// Destructor - virtual and class not intended
	// for derivation, so not exported

	~CTelephony();

	// General Functionality
	inline TVersion Version() const;

	// Phone Functionality
	IMPORT_C void GetPhoneId(TRequestStatus& aReqStatus, TDes8& aId) const;
	IMPORT_C void GetSubscriberId(TRequestStatus& aReqStatus, TDes8& aId) const;
	IMPORT_C void GetFlightMode(TRequestStatus& aReqStatus, TDes8& aMode) const;
	IMPORT_C void GetIndicator(TRequestStatus& aReqStatus, TDes8& aIndicator) const;
	IMPORT_C void GetBatteryInfo(TRequestStatus& aReqStatus, TDes8& aBatteryInfo) const;
	IMPORT_C void GetSignalStrength(TRequestStatus& aReqStatus, TDes8& aSignalStrength) const;
	IMPORT_C void GetLockInfo(TRequestStatus& aReqStatus, const TIccLock& aLock, TDes8& aLockInfo) const;
	IMPORT_C void SendDTMFTones(TRequestStatus& aReqStatus, const TDesC& aTones) const;
	// Line Functionality
	IMPORT_C TInt GetLineStatus(const TPhoneLine& aLine, TDes8& aStatus) const;
	IMPORT_C TInt GetCallInfo(TDes8& aCallSelect, TDes8& aCallInfo, TDes8& aRemoteInfo) const;
	// Call Functionality
	IMPORT_C void DialNewCall(TRequestStatus& aStatus, TDes8& aCallParams,
		const TTelNumber& aTelNumber, TCallId& aCallId, const TPhoneLine aLine=EVoiceLine) const;
	IMPORT_C TInt GetCallDynamicCaps(const TCallId& aCallId, TDes8& aCallCaps) const;
	IMPORT_C TInt GetCallStatus(const TCallId& aCallId, TDes8& aCallStatus) const;
	IMPORT_C void Hold(TRequestStatus& aReqStatus, const TCallId& aCallId) const;
	IMPORT_C void Resume(TRequestStatus& aReqStatus, const TCallId& aCallId) const;
	IMPORT_C void Swap(TRequestStatus& aReqStatus, const TCallId& aCallId1,
		const TCallId& aCallId2) const;
	IMPORT_C void Hangup(TRequestStatus& aReqStatus, const TCallId& aCallId) const;
	IMPORT_C void AnswerIncomingCall(TRequestStatus& aReqStatus, TCallId& aCallId,
		const TPhoneLine aLine=EVoiceLine) const;
	// Network Functionality
	IMPORT_C void GetNetworkRegistrationStatus(TRequestStatus& aReqStatus, TDes8& aStatus) const;
	IMPORT_C void GetCurrentNetworkInfo(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const;
	IMPORT_C void GetCurrentNetworkName(TRequestStatus& aReqStatus, TDes8& aNetworkName) const;
	IMPORT_C void GetOperatorName(TRequestStatus& aReqStatus, TDes8& aOperator) const;
 	// (Basic) Supplementary Services Functionality
	IMPORT_C void GetCallForwardingStatus(TRequestStatus& aRequestStatus, const	TCallForwardingCondition aCondition, TDes8& aSSInfo, const TServiceGroup aServiceGroup=EVoiceService) const;
	IMPORT_C void GetCallBarringStatus(TRequestStatus& aRequestStatus, const TCallBarringCondition aCondition, TDes8& aSSInfo, const TServiceGroup aServiceGroup=EVoiceService) const;
	IMPORT_C void GetCallWaitingStatus(TRequestStatus& aRequestStatus, TDes8& aSSInfo, const TServiceGroup aServiceGroup=EVoiceService) const;
	IMPORT_C void GetIdentityServiceStatus(TRequestStatus& aReqStatus, const TIdentityService& aService, TDes8& aStatus) const;
  	// Cancel Request Functionality
	IMPORT_C TInt CancelAsync(TCancellationRequest aCancel) const;
 	// Notification Functionality
	IMPORT_C void NotifyChange(TRequestStatus& aReqStatus, const TNotificationEvent& aEvent, TDes8& aDes) const;

	IMPORT_C TInt FindDeviceStatus(TInt& aCallStatus);
	IMPORT_C TInt EstablishDataCall(TInt& aDataChannel, const TDesC& aDialNumber);
	IMPORT_C void EstablishDataCall(TRequestStatus& aRequestStatus, TInt& aDataChannel, const TDesC& aDialNumber);
	IMPORT_C TInt TerminateDataCall();
	IMPORT_C TInt ReceiveDataCall(TInt& aDataChannel);
	IMPORT_C void ReceiveDataCall(TRequestStatus& aRequestStatus, TInt& aDataChannel);
	IMPORT_C void CancelEstablishDataCall();
	IMPORT_C void CancelReceiveDataCall();

private:
	CTelephony();	// C++ constructor - not exported. Called from NewLC()
	void ConstructL();	// second-phase constructor
	CTelephonyFunctions*	iTelephonyFunctions;	///< Object to do the real work
	};

#include <telephony.inl>
#endif	// ETEL3RDPARTY_H__
