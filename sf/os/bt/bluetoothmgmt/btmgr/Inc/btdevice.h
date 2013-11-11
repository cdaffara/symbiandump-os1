// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTDEVICE_H
#define BTDEVICE_H

#include <e32std.h>
#include <e32base.h>
#include <s32mem.h>

#include <bttypes.h>

/** The maximum length of a bluetooth device name, as stored in CBTDevice.
@publishedAll
@released
**/
static const TInt KMaxBluetoothNameLen = 248;

/** The maximum length of the user-assinged friendly name for a bluetooth device, as stored in CBTDevice.
@publishedAll
@released
**/
static const TInt KMaxFriendlyNameLen = 100;

/** Length of a bluetooth device name. This is not to be used outside of the sub-system. Retained for binary compatibility.
@internalComponent
@deprecated
**/
static const TInt KMaxBCBluetoothNameLen = 256;

//forward declarations
class TBTDeviceClass;
class CBTDevice;
class TBTServiceSecurityPerDevice;

/** A typedef'ed array of CBTDevice objects.
@publishedAll
@released
**/
typedef CArrayPtrFlat<CBTDevice> CBTDeviceArray;

/** A typedef'ed array of TBTServiceSecurityPerDevice objects.
@publishedAll
@released
**/
typedef RArray<TBTServiceSecurityPerDevice> RBTSecurityArray;

/** Wide descriptor of correct length to hold a bluetooth device name.
@publishedAll
@released
**/
typedef TBuf<KMaxBCBluetoothNameLen> TBTDeviceName;

/** Narrow descriptor of correct length to hold a bluetooth device name.

@publishedAll
@released
**/
typedef TBuf8<KMaxBluetoothNameLen> TBTDeviceName8;

/** Utility class to convert between a wide TBTDeviceName and a narrow TBTDeviceName8.
@publishedAll
@released
**/
class BTDeviceNameConverter
	{
public:
	IMPORT_C static TBTDeviceName ToUnicodeL(const TBTDeviceName8& aName);
	IMPORT_C static TBTDeviceName8 ToUTF8L(const TBTDeviceName& aName);
	};

/** Cleanup utility for CBTDeviceArrays.

Creates a TCleanupItem that calls ResetAndDestroy and then deletes the array.

@publishedAll
@released
*/
class BTDeviceArrayCleanupStack

	{
public:
	IMPORT_C static void PushL(CBTDeviceArray* aBTDeviceArray);
private:
	static void ResetAndDestroy(TAny* aBTDeviceArray);
	};

/** Enum used to describe the Bluetooth major service classes.  

See the bluetooth specification for more details.

@publishedAll
@released
**/
enum TBTMajorServiceClass
	{												//23        13	<- bit number
	/** Limited discoverable mode **/
	EMajorServiceLimitedDiscoverableMode = 0x01,	//00000000001
	/** Positioning **/
	EMajorServicePositioning = 0x08,				//00000001000
	/** Networking **/
	EMajorServiceNetworking = 0x10,					//00000010000
	/** Rendering **/
	EMajorServiceRendering = 0x20,					//00000100000
	/** Capturing **/
	EMajorServiceCapturing = 0x40,					//00001000000
	/** Object transfer **/
	EMajorServiceObjectTransfer = 0x80,				//00010000000
	/** Audio **/
	EMajorServiceAudio = 0x100,						//00100000000
	/** Telephony **/
	EMajorServiceTelephony = 0x200,					//01000000000
	/** Information **/
	EMajorServiceInformation = 0x400,				//10000000000
	/**
	 * Old name kept for SC.
	 * @deprecated
	 * @see EMajorServiceAudio
	 */
	EMajorServiceAudioService = 0x100				//00100000000
	};

/** Enum used to describe the Bluetooth major device classes.  

See the bluetooth specification for more details.

@publishedAll
@released
**/
enum TBTMajorDeviceClass
	{												//12  8	<- bit number
	/** Miscellaneous device **/
	EMajorDeviceMisc = 0x0,							//00000
	/** Computer device **/
	EMajorDeviceComputer = 0x1,						//00001
	/** Phone device **/
	EMajorDevicePhone = 0x2,						//00010
	/** LAN access point device **/
	EMajorDeviceLanAccessPoint = 0x3,				//00011
	/** Audio-video device **/
	EMajorDeviceAV = 0x4,							//00100
	/** Peripheral device **/
	EMajorDevicePeripheral = 0x5,					//00101
	/** Imaging device **/
	EMajorDeviceImaging = 0x6,						//00110
	/** Wearable device **/
	EMajorDeviceWearable = 0x7,						//00111
	/** Toy device **/
	EMajorDeviceToy = 0x8,							//01000
	/** Unclassified device **/
	EMajorDeviceUnclassified = 0x1F,				//11111
	/**
	 * Old name kept for SC.
	 * @deprecated
	 * @see EMajorDeviceAV
	 */
	EMajorDeviceAudioDevice = 0x4,					//00100
	/**
	 * Old name kept for SC.
	 * @deprecated
	 * @see EMajorDeviceUnclassified
	 */
	EMajorDeviceDeviceUnclassified = 0x1F			//11111
	};

/** Enum used to describe the Bluetooth minor device classes relating to the "Computer" major device class.

See the bluetooth specification for more details.

@publishedAll
@released
**/
enum TBTMinorDeviceClassComputer
	{										//7    2	<- bit number
	/** Unclassified computer device **/
	EMinorDeviceComputerUnclassified = 0x0,	//000000
	/** Desktop computer device **/
	EMinorDeviceComputerDesktop = 0x1,		//000001
	/** Server computer device **/
	EMinorDeviceComputerServer = 0x2,		//000010
	/** Laptop computer device **/
	EMinorDeviceComputerLaptop = 0x3,		//000011
	/** Handheld computer device **/
	EMinorDeviceComputerHandheld = 0x4,		//000100
	/** Palm computer device **/
	EMinorDeviceComputerPalmSize = 0x5,		//000101
	/** Wearable computer device **/
	EMinorDeviceComputerWearable = 0x6		//000110
	};

/** Enum used to describe the Bluetooth minor device classes relating to the "phone" major device class.

See the bluetooth specification for more details.

@publishedAll
@released
**/
enum TBTMinorDeviceClassPhone
	{										//7    2	<- bit number
	/** Unclassified phone device **/
	EMinorDevicePhoneUnclassified = 0x0,	//000000
	/** Cellular phone device **/
	EMinorDevicePhoneCellular = 0x1,		//000001
	/** Cordless phone device **/
	EMinorDevicePhoneCordless = 0x2,		//000010
	/** Smart phone device **/
	EMinorDevicePhoneSmartPhone = 0x3,		//000011
	/** Wired modem phone device **/
	EMinorDevicePhoneWiredModem = 0x4,		//000100
	/** ISDN phone device **/
	EMinorDevicePhoneCommonISDNAccess = 0x5	//000101
	};

/** Enum used to describe the Bluetooth minor device classes relating to the "LAN" major device class.

See the bluetooth specification for more details.
@publishedAll
@released
**/
enum TBTMinorDeviceClassLAN
	{										//7    2	<- bit number
	/** LAN is currently 0% utilitized **/
	EMinorDeviceLANFullyAvailable = 0x0,	//000000
	/** LAN is currently 1-17% utilitized **/
	EMinorDeviceLANServiceOne = 0x8,		//001000
	/** LAN is currently 17-33% utilitized **/
	EMinorDeviceLANServiceTwo = 0x10,		//010000
	/** LAN is currently 33-50% utilitized **/
	EMinorDeviceLANServiceThree = 0x18,		//011000
	/** LAN is currently 50-67% utilitized **/
	EMinorDeviceLANServiceFour = 0x20,		//100000
	/** LAN is currently 67-83% utilitized **/
	EMinorDeviceLANServiceFive = 0x28,		//101000
	/** LAN is currently 83-99% utilitized **/
	EMinorDeviceLANServiceSix = 0x30,		//110000
	/** LAN is currently 100% utilitized **/
	EMinorDeviceLANNoService = 0x38			//111000
	};

/** Enum used to describe the Bluetooth minor device classes relating to the "AV" major device class.

See the bluetooth specification for more details.

@publishedAll
@released
**/
enum TBTMinorDeviceClassAV
	{												//7    2
	/** Unclassified AV Device **/
	EMinorDeviceAVUnclassified = 0x0,				//000000
	/** Headset AV Device **/
	EMinorDeviceAVHeadset = 0x1,					//000001
	/** Handsfree AV Device **/
	EMinorDeviceAVHandsfree = 0x2,					//000010
	/** Microphone AV Device **/
	EMinorDeviceAVMicrophone = 0x4,					//000100
	/** Loudspeaker AV Device **/
	EMinorDeviceAVLoudspeaker = 0x5,				//000101
	/** Headphone AV Device **/
	EMinorDeviceAVHeadphones = 0x6,					//000110
	/** Portable audio AV Device **/
	EMinorDeviceAVPortableAudio = 0x7,				//000111
	/** Car audio AV Device **/
	EMinorDeviceAVCarAudio = 0x8,					//001000
	/** Set top box AV Device **/
	EMinorDeviceAVSetTopBox = 0x9,					//001001
	/** Hifi audio AV Device **/
	EMinorDeviceAVHiFiAudio = 0xA,					//001010
	/** VCR AV Device **/
	EMinorDeviceAVVCR = 0xB,						//001011
	/** Video camera AV Device **/
	EMinorDeviceAVVideoCamera = 0xC,				//001100
	/** Camcorder AV Device **/
	EMinorDeviceAVCamcorder = 0xD,					//001101
	/** Video monitor AV Device **/
	EMinorDeviceAVVideoMonitor = 0xE,				//001110
	/** Video display and loudspeaker AV Device **/
	EMinorDeviceAVVideoDisplayAndLoudspeaker = 0xF,	//001111
	/** Video conferencing AV Device **/
	EMinorDeviceAVVideoConferencing = 0x10,			//010000
	/** Gaming toy AV Device **/
	EMinorDeviceAVGamingToy = 0x12					//010010
	};	
	
/** Enum used to describe the Bluetooth minor device classes relating to the "Peripheral" major device class.

See the bluetooth specification for more details.
@publishedAll
@released
**/
enum TBTMinorDeviceClassPeripheral
	{													//7    2
	/** Keyboard peripheral device.
	
	Note that the keyboard bit can be independently
	or'd with all other bits.
	**/
	EMinorDevicePeripheralKeyboard = 0x10,				//010000
	/** Pointer peripheral device.
	
	Note that the pointer bit can be independently
	or'd with all other bits.
	**/
	EMinorDevicePeripheralPointer = 0x20,				//100000
	//
	// All other bits are mutually exclusive
	//
	/** Uncategorized peripheral device **/
	EMinorDevicePeripheralUncategorized = 0x00,			//000000
	/** Joystick peripheral device **/
	EMinorDevicePeripheralJoystick = 0x01,				//000001
	/** Gamepad peripheral device **/
	EMinorDevicePeripheralGamepad = 0x02,				//000010
	/** Remote control peripheral device **/
	EMinorDevicePeripheralRemoteControl = 0x03,			//000011
	/** Sensing peripheral device **/
	EMinorDevicePeripheralSensingDevice = 0x04,			//000100
	/** Digitizer tablet peripheral device **/
	EMinorDevicePeripheralDigitizerTablet = 0x05,		//000101
	/** Card reader peripheral device **/
	EMinorDevicePeripheralCardReader = 0x06				//000110
	};

/** Enum used to describe the Bluetooth minor device classes relating to the "Imaging" major device class.

See the bluetooth specification for more details.
@publishedAll
@released

**/
enum TBTMinorDeviceClassImaging
	{											//7    2
	/** Display imaging device **/
	EMinorDeviceImagingDisplay = 0x04,			//000100
	/** Camera imaging device **/
	EMinorDeviceImagingCamera = 0x08,			//001000
	/** Scanner imaging device **/
	EMinorDeviceImagingScanner = 0x10,			//010000
	/** Printer imaging device **/
	EMinorDeviceImagingPrinter = 0x20,			//100000
	};

/** Enum used to describe the Bluetooth minor device classes relating to the "Wearable" major device class.

See the bluetooth specification for more details.
@publishedAll
@released

**/
enum TBTMinorDeviceClassWearable
	{											//7    2
	/** Wrist watch wearable device **/
	EMinorDeviceWearableWatch = 0x01,			//000001
	/** Pager wearable device **/
	EMinorDeviceWearablePager = 0x02,			//000010
	/** Jacket wearable device **/
	EMinorDeviceWearableJacket = 0x03,			//000011
	/** Helmet wearable device **/
	EMinorDeviceWearableHelmet = 0x04,			//000100
	/** Glasses wearable device **/
	EMinorDeviceWearableGlasses = 0x05,			//000101	
	};

/** Enum used to describe the Bluetooth minor device classes relating to the "Toy" major device class.

See the bluetooth specification for more details.
@publishedAll
@released

**/
enum TBTMinorDeviceClassToy
	{											//7    2
	/** Robot toy device **/
	EMinorDeviceToyRobot = 0x01,				//000001
	/** Vehicle toy device **/
	EMinorDeviceToyVehicle = 0x02,				//000010
	/** Doll toy device **/
	EMinorDeviceToyDoll = 0x03,					//000011
	/** Controller toy device **/
	EMinorDeviceToyController = 0x04,			//000100
	/** Game toy device **/
	EMinorDeviceToyGame = 0x05,					//000101	
	};
	
/** Encapsulation of device class definitions.

Contains a single TInt as member data and methods to extract the major, minor and service
class information from that TInt.
@publishedAll
@released
**/
NONSHARABLE_CLASS(TBTDeviceClass)
	{
public:
	IMPORT_C TBTDeviceClass();
	IMPORT_C TBTDeviceClass(TUint32 aDeviceClass);
	IMPORT_C TBTDeviceClass(TUint16 aMajorServiceClass, TUint8 aMajorDeviceClass,
							TUint8 aMinorDeviceClass);

	IMPORT_C TUint16 MajorServiceClass() const;
	IMPORT_C TUint8 MajorDeviceClass() const;
	IMPORT_C TUint8 MinorDeviceClass() const;
	IMPORT_C TUint32 DeviceClass() const;
	
	IMPORT_C TBool operator==(const TBTDeviceClass& aDeviceClass) const;
	IMPORT_C TBTDeviceClass& operator=(const TBTDeviceClass& aDeviceClass);
	
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
private:
	static const TUint32 KStreamVersion = 0x00000001;
private:
	TUint32	iDeviceClass;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32	iPadding1; 
	TUint32	iPadding2; 
	};

/** Class to store the security override parameters for a particular remote device.

@publishedAll
@released
*/
NONSHARABLE_CLASS(TBTDeviceSecurity)
	{
public:
	/** Enumeration to provide select MITM protection required. */
	enum TMitmRequired
		{
		EMitmUnspecified,	/*!< No specific MITM protection requirements */
		EMitmRequired		/*!< Require the link is MITM protected */
		};
	
public:
	IMPORT_C TBTDeviceSecurity();
	IMPORT_C TBTDeviceSecurity(TUint8 aSecurity);
	IMPORT_C TBTDeviceSecurity(TUint8 aSecurity, TUint aPasskeyMinLength);
	IMPORT_C TBTDeviceSecurity(TBool aNoAuthenticate, TBool aNoAuthorise, TBool aEncrypt, TBool aBanned);
	IMPORT_C TBTDeviceSecurity(TMitmRequired aMitmRequirements, TBool aNoAuthorise, TBool aEncrypt, TBool aBanned);
	
	IMPORT_C void SetNoAuthenticate(TBool aDecision);
	IMPORT_C void SetMitmRequirements(TMitmRequired aDecision);
	IMPORT_C void SetNoAuthorise(TBool aDecision);
	IMPORT_C void SetEncrypt(TBool aDecision);
	IMPORT_C void SetBanned(TBool aDecision);
	
	IMPORT_C TBool NoAuthenticate() const;
	IMPORT_C TMitmRequired MitmRequirements() const;
	IMPORT_C TBool NoAuthorise() const;
	IMPORT_C TBool Encrypt() const;
	IMPORT_C TBool Banned() const;
	IMPORT_C TUint PasskeyMinLength() const;
	
	
	IMPORT_C TUint8 SecurityValue() const;
	IMPORT_C void SetSecurityValue(TUint8 aDeviceSecurity);
	IMPORT_C TInt SetPasskeyMinLength(TUint aPasskeyMinLength);	
	
	
	IMPORT_C TBool operator==(const TBTDeviceSecurity& aDeviceSecurity) const;
	IMPORT_C TBool operator!=(const TBTDeviceSecurity& aDeviceSecurity) const;
	
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);

public:
	/** Enumeration to assist in parsing of security settings. */
	enum TBTDeviceSecuritySettings
		{
		ENoAuthenticate			= 0x01, /*!< Don't authenticate the link @deprecated */
		ENoAuthorise			= 0x02, /*!< Don't authorise the connection */
		EEncrypt				= 0x04, /*!< Encrypt the link */
		EBanned					= 0x08, /*!< Don't connect to the device */
		EMitmProtectionRequired	= 0x10, /*!< Require the link is MITM protected */
		};
	
private:
	static const TUint32 KStreamVersion = 0x00000001;

private:
	TUint8	iSecurity;
	TUint	iPasskeyMinLength;	
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32	iPadding1; 
	TUint32	iPadding2; 
	};

/** Class to tie a service uid with a device security setting.

This allows us to have service-specific security settings for a device.

@publishedAll
@released
*/
NONSHARABLE_CLASS(TBTServiceSecurityPerDevice)
	{
public:
	IMPORT_C TBTServiceSecurityPerDevice();
	IMPORT_C TBTServiceSecurityPerDevice(const TBTDevAddr& aDevice, const TBTDeviceSecurity& aSecuritySettings);
	IMPORT_C TBTServiceSecurityPerDevice(const TBTServiceSecurityPerDevice&);
	
	IMPORT_C void SetDeviceSecurity(const TBTDeviceSecurity& aSecuritySettings);
	IMPORT_C void SetAddress(const TBTDevAddr& aAddress);

	IMPORT_C const TBTDeviceSecurity& DeviceSecurity() const;
	IMPORT_C const TBTDevAddr& DeviceAddress() const;
	
	IMPORT_C void operator=(const TBTServiceSecurityPerDevice& aServiceSecurityPerDevice);
	IMPORT_C TBool operator==(const TBTServiceSecurityPerDevice& aServiceSecurityPerDevice) const;
	IMPORT_C TBool operator!=(const TBTServiceSecurityPerDevice& aServiceSecurityPerDevice) const;
	
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
private:
	static const TUint32 KStreamVersion = 0x00000001;
private:
	TBTDevAddr			iDeviceAddress;		/*!< the address of the device for the overriding security **/
	TBTDeviceSecurity	iDeviceSecurity;	/*!< the overriding security */
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32		iPadding1; 
	TUint32		iPadding2; 	
	};

/** Stores parameters for a nameless device.

A class which stores useful parameters concerning a remote device
But *not* including names
@see CBTDevice
@publishedAll
@released
*/
NONSHARABLE_CLASS(TBTNamelessDevice)
	{
friend class CBTDevice;

private:
	/** For Symbian use only
	@internalComponent	
	*/
	NONSHARABLE_CLASS(TBTBasebandParameters)
		{
	public:
		void ExternalizeL(RWriteStream& aStream) const;
		void InternalizeL(RReadStream& aStream);

		TUint8 iPageScanRepetitionMode;			/*!< Page Scan repetition mode of remote device ( valid if > 0 ) */
		TUint8 iPageScanPeriodMode;				/*!< Page scan period mode of remote device */
		TUint8 iPageScanMode;					/*!< Page scan mode of remote device ( valid if > 0 ) */
		TUint16 iClockOffset;					/*!< clock off set of remote device ( valid if > 0 ) */
		
	private:
		static const TUint32 KStreamVersion = 0x00000001;
		};
public:
	
	IMPORT_C TBTNamelessDevice();
	IMPORT_C const TBTDevAddr& Address() const;
	IMPORT_C const TBTDeviceClass& DeviceClass() const;
	IMPORT_C const TBTLinkKey& LinkKey() const;
	IMPORT_C TBTLinkKeyType LinkKeyType() const;
	IMPORT_C TUint8 PageScanRepMode() const;
	IMPORT_C TUint8 PageScanMode() const;
	IMPORT_C TUint8 PageScanPeriodMode() const;
	IMPORT_C TUint16 ClockOffset() const;
	IMPORT_C const TTime& Seen() const;
	IMPORT_C const TTime& Used() const;
	IMPORT_C const TBTDeviceSecurity& GlobalSecurity() const;
	IMPORT_C void SetAddress(const TBTDevAddr& aBDAddr);
	IMPORT_C void SetDeviceClass(TBTDeviceClass aDeviceClass);
	IMPORT_C void SetLinkKey(const TBTLinkKey& aLinkKey);
	IMPORT_C void SetLinkKey(const TBTLinkKey& aLinkKey, TBTLinkKeyType aLinkKeyType);
	IMPORT_C void SetPageScanRepMode(TUint8 aPageScanRepMode);
	IMPORT_C void SetPageScanPeriodMode(TUint8 aPageScanPeriodMode);
	IMPORT_C void SetPageScanMode(TUint8 aPageScanMode);
	IMPORT_C void SetClockOffset(TUint16 aClockOffSet);
	IMPORT_C void SetGlobalSecurity(const TBTDeviceSecurity& aSetting);
	IMPORT_C void SetUsed(const TTime& aDateTime);
	IMPORT_C void SetSeen(const TTime& aDateTime);
	IMPORT_C TBool IsValidAddress() const;
	IMPORT_C TBool IsValidDeviceClass() const;
	IMPORT_C TBool IsValidLinkKey() const;
	IMPORT_C TBool IsValidGlobalSecurity() const;
	IMPORT_C TBool IsValidPageScanRepMode() const;
	IMPORT_C TBool IsValidPageScanMode() const;
	IMPORT_C TBool IsValidPageScanPeriodMode() const;
	IMPORT_C TBool IsValidClockOffset() const;
	IMPORT_C TBool IsValidUsed() const;
	IMPORT_C TBool IsValidSeen() const;
	IMPORT_C void DeleteLinkKey();
	IMPORT_C void Update(const TBTNamelessDevice& aDevice);
	IMPORT_C TBTNamelessDevice& operator=(const TBTNamelessDevice& aDevice);
	IMPORT_C TBool operator==(const TBTNamelessDevice& aDevice) const;
	IMPORT_C TBool operator!=(const TBTNamelessDevice& aDevice) const;
	IMPORT_C TUint CompareTo(const TBTNamelessDevice& aDevice) const;
	IMPORT_C TBool IsPaired() const;
	IMPORT_C void SetPaired(TBool aPaired);
	IMPORT_C void SetPaired(TBTLinkKeyType aLinkKeyType);
	IMPORT_C TBool IsValidPaired() const;
	IMPORT_C const TBTPinCode& PassKey() const;
	IMPORT_C void  SetPassKey(const TBTPinCode& aPassKey);
	IMPORT_C TBool IsValidPassKey() const;
	IMPORT_C TUint PassKeyLength() const;
	IMPORT_C TBool IsValidUiCookie() const;
	IMPORT_C void SetUiCookie(TUint32 aUiCookie);
	IMPORT_C TUint32 UiCookie() const;
	
	/**	Bitfield of TBTNamelessDevice attributes
	This enum is to only be used by phone manufacturers, not by application developers.
	@publishedPartner
	@released
	*/	
	enum TBTDeviceSet
		{
		EAddress				=	0x000001, /*!< Device address */
		EDeviceClass			=	0x000002, /*!< Device class */
		ELinkKey				=	0x000004, /*!< Link key */
		EGlobalSecurity			=	0x000008, /*!< Global security settings */
		EPageScanRepMode		=	0x000010, /*!< Page scan repition mode */
		EPageScanMode			=	0x000020, /*!< Page scan mode */
		EPageScanPeriodMode		=	0x000040, /*!< Page scan period */
		EClockOffset			=	0x000080, /*!< Clock offset */
		EUsed					=	0x000100, /*!< Last used */
		ESeen					=	0x000200, /*!< Last seen */
		EIsPaired				=	0x000400, /*!< Is paired */
		EPassKey				=	0x000800, /*!< PassKey */
		EUiCookie				=	0x001000, /*!< User interface specific cookie */

		EAllNamelessProperties	= 0x00ffffff, /*!< All nameless properties set */
		// Two highest nibbles used by CBTDevice
		};

private:
	TUint32					iSetMask;		/*!< Bit mask indicating the data members that have been set */
	TBTDevAddr				iBDAddr; 		/*!< The BT address of the remote device */
	TBTDeviceClass			iDeviceClass;	/*!< The class(es) of the remote device */
	TBTLinkKey				iLinkKey;		/*!< The link key */
	TBTDeviceSecurity		iGlobalSecurity;/*!< The overall security criteria for all services which this device may wish to use */
	TBTBasebandParameters	iBasebandParams;/*!< The baseband parameters */
	TTime					iUsed;			/*!< when last connect from/to */
	TTime					iSeen;			/*!< when last seen in inquiry */
	TBool					iPaired;		/*!< Whether this device is paired */
	TBTPinCode				iPassKey;		/*!< PassKey */
	TBTLinkKeyType			iLinkKeyType;	/*!< Link Key Type */
	TUint32					iUiCookie;		/*!< The cookie attached to this device by the UI */
	
	// This data padding has been added to help prevent future binary compatibility breaks.
	// iPadding1 has been used for iLinkKeyType
	// iPadding2 has been used for iUiCookie 
	TUint32		iPadding3; 
	TUint32		iPadding4; 
	};


/** Encapsulates all information that must be known about a device.

Stores useful information concerning a remote device, including names

@publishedAll
@released
*/
NONSHARABLE_CLASS(CBTDevice) : public CBase
	{
friend class TBTNamelessDevice;
public:
	
	IMPORT_C static CBTDevice* NewL(const TBTDevAddr& aBDAddr);
	IMPORT_C static CBTDevice* NewLC(const TBTDevAddr& aBDAddr);
	IMPORT_C static CBTDevice* NewL(const TBTNamelessDevice& aNamelessDevice);
	IMPORT_C static CBTDevice* NewLC(const TBTNamelessDevice& aNamelessDevice);
	IMPORT_C static CBTDevice* NewL();
	IMPORT_C static CBTDevice* NewLC();
	IMPORT_C ~CBTDevice();
	IMPORT_C CBTDevice* CopyL() const;
	IMPORT_C void UpdateL(const CBTDevice& aDevice);
	IMPORT_C void AssignL(CBTDevice& aDevice);
	IMPORT_C const TDesC8& DeviceName() const;
	IMPORT_C const TDesC& FriendlyName() const;
	IMPORT_C void SetDeviceNameL(const TDesC8& aName);
	IMPORT_C void SetFriendlyNameL(const TDesC& aName);
	IMPORT_C TBool IsValidDeviceName() const;
	IMPORT_C TBool IsValidFriendlyName() const;
	IMPORT_C TBool operator==(const CBTDevice& aDevice) const;
	IMPORT_C TBool operator!=(const CBTDevice& aDevice) const;
	IMPORT_C TUint CompareTo(const CBTDevice& aDevice) const;
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C const TBTDevAddr& BDAddr() const;
	IMPORT_C const TBTLinkKey& LinkKey() const;
	IMPORT_C TBTLinkKeyType LinkKeyType() const;
	IMPORT_C TBTDeviceClass DeviceClass() const;
	IMPORT_C const TBTDeviceSecurity GlobalSecurity() const;
	IMPORT_C void SetDeviceAddress(const TBTDevAddr& aBDAddr);
	IMPORT_C void SetDeviceClass(TBTDeviceClass aDeviceClass);
	IMPORT_C void SetLinkKey(const TBTLinkKey& aLinkKey);
	IMPORT_C void SetLinkKey(const TBTLinkKey& aLinkKey, TBTLinkKeyType aLinkKeyType);
	IMPORT_C void SetGlobalSecurity(const TBTDeviceSecurity& aSetting);
	IMPORT_C void DeleteLinkKey();
	IMPORT_C TBool IsValidBDAddr() const;
	IMPORT_C TBool IsValidDeviceClass() const;
	IMPORT_C TBool IsValidLinkKey() const;
	IMPORT_C TBool IsValidGlobalSecurity() const;
	IMPORT_C const TTime& Seen() const;	
	IMPORT_C const TTime& Used() const;
	IMPORT_C void SetUsed(const TTime& aDateTime);
	IMPORT_C void SetSeen(const TTime& aDateTime);
	IMPORT_C TBool IsValidUsed() const;
	IMPORT_C TBool IsValidSeen() const;
	IMPORT_C TBTNamelessDevice& AsNamelessDevice();	
	IMPORT_C const TBTNamelessDevice& AsNamelessDevice() const;
	IMPORT_C TBool IsPaired() const;
	IMPORT_C void SetPaired(TBool aPaired);
	IMPORT_C void SetPaired(TBTLinkKeyType aLinkKeyType);
	IMPORT_C TBool IsValidPaired() const;
	IMPORT_C const TBTPinCode& PassKey() const;		
	IMPORT_C void  SetPassKey(const TBTPinCode& aPassKey);		
	IMPORT_C TBool IsValidPassKey() const;
	IMPORT_C TUint PassKeyLength() const;
	IMPORT_C TBool IsValidUiCookie() const;
	IMPORT_C void SetUiCookie(TUint32 aUiCookie);
	IMPORT_C TUint32 UiCookie() const;
	
private:
	CBTDevice();
	void ConstructL();
	void ConstructL(const TBTDevAddr& aAddr);
	void ConstructL(const TBTNamelessDevice& aNamelessDevice);
	void CommonConstructL();
		
public:
	/** Describes the similarities between two devices. */
	enum TBTDeviceNameSelector // not very encapsulated - we extend the enum of TBTNamelessDevice
		{
		/** Device name. */
		EDeviceName		= 0x40000000,
		/** Friendly name. */
		EFriendlyName	= 0x80000000,
		/** 
		All name properties
		*/
		EAllNameProperties = 0xff000000,
		/** Last enum. This is NOT a name property */
		ELast 			= KMaxTUint,
		// All but two highest nibbles used by TBTNamelessDevice
		};
private:
	static const TUint32 KStreamVersion = 0x00000001;
private:
	HBufC8*	iDeviceName;
	HBufC*	iFriendlyName;
	TBTNamelessDevice iDevice;
	};


/**Number of bytes in an AFH Host Channel Classification. 

Size and Length of the TBTAFHHostChannelClassification Descriptor

@see TBTAFHHostChannelClassification
@publishedAll
@released
*/
const TUint8 KHCIAFHHostChannelClassificationSize=10;


/**Required number of unknown channels in an AFH Host Channel Classification.

That is least number of channels in the 79 channel set that must be left
NOT set to bad. In AFH host channel classification a channel may labelled
'bad' (implies do not use) or 'unknown' (implies that it is up to the 
controller to decide if should be used). An attempt to create a 
TBTAFHHostChannelClassification with more than KAFHMinUnknownChannels 
set to 'bad' will result in a KErrUnderflow error being returned.

@see TBTAFHHostChannelClassification
@publishedAll
@released
*/
const TUint8 KAFHMinUnknownChannels=20;


/**Number of channels in the Bluetooth channel set that is used with AFH

The Bluetooth channel set is the set of frequencies used in the Bluetooth
hopping sequence. AFH allows a connection to opt not to use some of the 
frequencies in this set.

@see TBTAFHHostChannelClassification
@publishedAll
@released
*/
const TUint8 KAFHNumChannels=79;


/**Descriptor class that should be used to help set up an 
AFH host channel classification for the controller.

The bits in each byte of this descriptor are used
to tell the controller which channels the host
anticipates will become busy and so advisable for
Bluetooth to avoid. Such channels are referred to 
as 'bad', and are represented by bits set to 0.  
Channels that are not 'bad' are referred to as 'unknown' implying
simply that the host has no knowledge that they are, or
are about to become 'bad'. These are represented by bits set to 1.
Note: The Bluetooth specification demands that at least 20 channels 
must be set to 'unknown'. Any attempt by the user of this class
to set too many 'bad' channels will result in the return of a 
KErrNotSupported error, and the attempt having no effect.


@publishedAll
@released
*/
NONSHARABLE_CLASS(TBTAFHHostChannelClassification) : public TBuf8<KHCIAFHHostChannelClassificationSize>
	{
public:
	IMPORT_C TBTAFHHostChannelClassification();
	IMPORT_C void Reset();
	IMPORT_C TInt Validate() const;
	IMPORT_C TInt SetChannelBad(const TUint8 aChannel);
	IMPORT_C TInt SetChannelRangeBad(const TUint8 aChannelRangeLowest, const TUint8 aChannelRangeHighest);
	
private:
	TInt DoSetChannelBad(const TUint8 aChannel);
	TInt BitByteIndices(TUint8& aByteIndex, TUint8& aBitIndex, const TUint8 aChannel) const;
	TUint8 NumUnknownsWithinRange(const TUint8 aChannelRangeLowest, const TUint8 aChannelRangeHighest) const;
	
	// This data padding has been added to help prevent future binary compatibility breaks
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used	
	TUint32     iPadding1; 
	TUint32     iPadding2;	
    };


/** Class used to exchange the local settings with the Registry server.

	Not a complete set of details of the local device

	@publishedAll
	@released

*/
NONSHARABLE_CLASS(TBTLocalDevice)
	{
public:
	IMPORT_C TBTLocalDevice();	
	IMPORT_C const TBTDevAddr& Address() const;
	IMPORT_C TUint32 DeviceClass() const;	
	IMPORT_C const TDesC8& DeviceName() const;	
	IMPORT_C THCIScanEnable ScanEnable() const;	
	IMPORT_C TBool LimitedDiscoverable() const;	
	IMPORT_C TUint8 PowerSetting() const;
	IMPORT_C TBool AFHChannelAssessmentMode() const;
	IMPORT_C TBool AcceptPairedOnlyMode() const;
	IMPORT_C void SetAddress(const TBTDevAddr& aAddr);	
	IMPORT_C void SetDeviceClass(TUint32 aCod);	
	IMPORT_C void SetDeviceName(const TDesC8& aName);	
	IMPORT_C void SetScanEnable(THCIScanEnable aEnable);	
	IMPORT_C void SetLimitedDiscoverable(TBool aOn);	
	IMPORT_C void SetPowerSetting(TUint8 aPowerSetting);	
	IMPORT_C void SetAFHChannelAssessmentMode(TBool aOn);	
	IMPORT_C void SetAcceptPairedOnlyMode(TBool aOn);	
	IMPORT_C TBool IsValidAddress() const;	
	IMPORT_C TBool IsValidDeviceClass() const;	
	IMPORT_C TBool IsValidDeviceName() const;	
	IMPORT_C TBool IsValidScanEnable() const;	
	IMPORT_C TBool IsValidLimitedDiscoverable() const;	
	IMPORT_C TBool IsValidPowerSetting() const;
	IMPORT_C TBool IsValidAFHChannelAssessmentMode() const;
	IMPORT_C TBool IsValidAcceptPairedOnlyMode() const;

protected:
	enum
		{
		EAddress					= 0x0001,
		ECoD						= 0x0002,
		EDeviceName					= 0x0004,
		EPowerSetting				= 0x0008,
		EScanEnable					= 0x0010,
		ELimitedDiscoverable 		= 0x0020,
		EAFHChannelAssessmentMode	= 0x0040,
		EAcceptPairedOnlyMode 		= 0x0080,
		};	//< Used for indicating if a setting has been set (i.e. not a default/random value) @see iBitMask
		
private:		
	enum
		{
		EAFHChannelAssessmentModeValue	= 0x0001,
		EAcceptPairedOnlyModeValue		= 0x0002,
		};	//< Used for settings requiring little space: allows new settings ot be introduced without increasing the size of this class @see iSimpleSettings
			
		

	TUint			iSetMask;		 //< Bit mask indicating the data members that have been set
	TBTDevAddr		iAddress;
	TUint32			iCod;
	TBTDeviceName8	iLocalName;
	TUint8			iPowerSetting;
	THCIScanEnable	iScanEnable;
	TBool			iLimitedDiscoverable;
	TUint32			iSimpleSettings; //< Only first two bits currently used - for AFH Channel Assessment Mode, and Accept Paired Only Mode
	};


#endif
