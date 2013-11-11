// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Generic functions associated with all Bluetooth socket addresses
// 
//

#include <bluetooth/logger.h>
#include <bt_sock.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BTLIB);
#endif

#ifdef _DEBUG
PANICCATEGORY("BtLib");
#endif

enum TBTSockPanics
	{
	EUnfinishedBusiness = 0,
	EBadArgument = 1,
	EBadRequest = 2,
	EBTExportedMethodMovedDLL =3,
	};
void Panic(TBTSockPanics aCode)
	{
	_LIT(KPanicName, "BTSock");
	User::Panic(KPanicName, aCode);
	}

EXPORT_C TBTSockAddr::TBTSockAddr()
/**
Constructor
@publishedAll
@released
*/
	{
	SetFamily(KBTAddrFamily);
	SetUserLen(sizeof(SBTAddrSecurity));
	AddressPtr().FillZ();
	
	// set default security
	TBTServiceSecurity defaultSec;
	defaultSec.SetAuthorisation(EFalse);
	defaultSec.SetAuthentication(EMitmNotRequired);
	defaultSec.SetDenied(EFalse);
	defaultSec.SetEncryption(EFalse);
	defaultSec.SetPasskeyMinLength(0);
	
	BTAddrSecStruct().iSecurity = defaultSec;
	}

EXPORT_C TBTSockAddr::TBTSockAddr(const TSockAddr &aAddr)
/**
Constructor taking socket address baseclass reference. Rather like a copy constructor

Copies the port number and the entirety of the user data, which conatins	
the Bluetooth Address and Security Settings	
	
The family type is set to KBTAddrFamily
@param aAddr a previously filled TSockAddr 
@publishedAll
@released
*/
	{
	SetFamily(KBTAddrFamily);
	SetPort(aAddr.Port());
	SetUserLen(sizeof(SBTAddrSecurity));
	// Copy the user data	
	// The strange cast is to get at UserPtr() while ensuring	
	// that the compiler does not try and create a temporary	
	Mem::Copy(UserPtr(), (*static_cast<const TBTSockAddr*>(&aAddr)).UserPtr(), GetUserLen());
	}

EXPORT_C TBTDevAddr TBTSockAddr::BTAddr() const
/**
Getter
@return The device address specified by this Socket address
@publishedAll
@released
*/
	{
	return BTAddrSecStruct().iAddress; 
	}

EXPORT_C void TBTSockAddr::SetBTAddr(const TBTDevAddr& aRemote)
/**
Sets BT address from a TBTDevAddr.
@param aRemote Bluetooth device address of remote device
@publishedAll
@released
*/
	{
	BTAddrSecStruct().iAddress = aRemote;
	}

EXPORT_C void TBTSockAddr::SetSecurity(const TBTServiceSecurity& aSecurity)
/**
Setter
Care needs to be taken when setting outgoing security as if a device in the system
is using pre-Bluetooth v2.1 hardware then connection attempts may fail.
@param aSecurity reference to const TBTServiceSecurity holding security settings
@publishedAll
@released
*/
	{
	BTAddrSecStruct().iSecurity = aSecurity;
	}


EXPORT_C TAny* TBTSockAddr::EndBTSockAddrPtr() const
	{
	return UserPtr()+sizeof(SBTAddrSecurity);
	}

TBTSockAddr::SBTAddrSecurity& TBTSockAddr::BTAddrSecStruct() const
	{
	return *reinterpret_cast<SBTAddrSecurity*>(UserPtr());
	}

TPtr8 TBTSockAddr::AddressPtr() const
	{
	return BTAddrSecStruct().iAddress.Des();
	}

EXPORT_C TBTServiceSecurity TBTSockAddr::BTSecurity() const
/**
Getter
@return Security settings	
*/
	{
	return BTAddrSecStruct().iSecurity;
	}

EXPORT_C TBTSockAddr& TBTSockAddr::Cast(const TSockAddr &aAddr)
/**
Utility function to downcast a TSockAddr to a TBTSockAddr
@return TBTSockAddr reference
*/
	{
	return *((TBTSockAddr*)&aAddr);
	}
	
/****** Inquiry Socket Addresses *****/

/** Constructor for TInquirySockAddr object
*/
EXPORT_C TInquirySockAddr::TInquirySockAddr()
	{
	SetFamily(KBTAddrFamily);
	SetUserLen(sizeof(SInquiryAddr));
	AddressPtr().FillZ();
	SetVersion(TVersion(1, 1, 0));
	}

/** Constructor taking socket address base class reference. Rather like a copy constructor
   
Copies the port number and the entirety of the user data, which contains	
the Bluetooth Address
	
The family type is set to KBTAddrFamily
@param aAddr a previously filled TSockAddr 
*/   
EXPORT_C TInquirySockAddr::TInquirySockAddr(const TSockAddr &aAddr)
	{
	SetFamily(KBTAddrFamily);
	SetUserLen(sizeof(SInquiryAddr));
	AddressPtr().Copy((*static_cast<const TInquirySockAddr*>(&aAddr)).AddressPtr());
	SetPort(aAddr.Port());
	InquiryAddrStruct() = TInquirySockAddr::Cast(aAddr).InquiryAddrStruct();
	SetVersion(TVersion(1, 1, 0));
	}

/** Getter.
@return The device address specified by this Socket address.
*/
EXPORT_C TBTDevAddr TInquirySockAddr::BTAddr() const
	{
	return InquiryAddrStruct().iAddress; 
	}
/** Setter.
@param aRemote is bluetooth device address
*/
EXPORT_C void TInquirySockAddr::SetBTAddr(const TBTDevAddr& aRemote)
	{
	InquiryAddrStruct().iAddress = aRemote;
	}

/** Utility function to downcast a TSockAddr to a TInquirySockAddr
@param aAddr Represents an end point address
@return  Socket address used for inquiries
*/
EXPORT_C TInquirySockAddr& TInquirySockAddr::Cast(const TSockAddr &aAddr)
	{
	return *((TInquirySockAddr *)&aAddr);
	}

/**  Getter for major service class of the device
 @return The major service class of device
*/
EXPORT_C TUint16 TInquirySockAddr::MajorServiceClass() const
	{
	return InquiryAddrStruct().iMajorServiceClass;
	}

/**  Sets major service class of the device
@param aClass is major service class to be assigned 
 */
EXPORT_C void TInquirySockAddr::SetMajorServiceClass(TUint16 aClass)
	{
	InquiryAddrStruct().iMajorServiceClass=aClass;
	}

/**  Getter for major device class
 @return The major class of device
*/
EXPORT_C TUint8 TInquirySockAddr::MajorClassOfDevice() const
	{
	return InquiryAddrStruct().iMajorDeviceClass;
	}

/**  Sets major device class
@param aClass is major class of device to be assigned 
*/
EXPORT_C void TInquirySockAddr::SetMajorClassOfDevice(TUint8 aClass)
	{
	InquiryAddrStruct().iMajorDeviceClass=aClass;
	}

/**  Getter for minor device class
 @return The minor class of device
*/
EXPORT_C TUint8 TInquirySockAddr::MinorClassOfDevice() const
	{
	return InquiryAddrStruct().iMinorDeviceClass;
	}

/**  Sets minor device class
@param aClass is minor device class to be assigned
 */
EXPORT_C void TInquirySockAddr::SetMinorClassOfDevice(TUint8 aClass)
	{
	InquiryAddrStruct().iMinorDeviceClass=aClass;
	}

/** Getter for Inquiry Access Code
 @return Inquiry Access Code 
*/
EXPORT_C TUint TInquirySockAddr::IAC() const
	{
	return InquiryAddrStruct().iIAC;
	}

/** Sets the Inquiry Access Code
@param aIAC is Inquiry Access Code to be assigned
*/
EXPORT_C void TInquirySockAddr::SetIAC(const TUint aIAC)
	{
	InquiryAddrStruct().iIAC=aIAC;
	}

/**  Utility function to get the host resolve action option
 @return action which can be KHostResInquiry and/or KHostResName 
*/
EXPORT_C TUint8 TInquirySockAddr::Action() const
	{
	return InquiryAddrStruct().iActionFlags;
	}

/**  Utility function to set the host resolve action option
@param aFlags can be KHostResInquiry and/or KHostResName 
 */
EXPORT_C void TInquirySockAddr::SetAction(TUint8 aFlags)
	{
	InquiryAddrStruct().iActionFlags=aFlags;
	}

/**  Utility function to get SInquiryAddr
@return SInquiryAddr class contained within the TInquirySockAddr
*/
EXPORT_C TInt8 TInquirySockAddr::Rssi() const
	{
	return InquiryAddrStruct().iRssi;
	}

EXPORT_C void TInquirySockAddr::SetRssi(const TInt8 aRssi)
	{
	InquiryAddrStruct().iRssi=aRssi;
	}

EXPORT_C TUint8 TInquirySockAddr::ResultFlags() const
	{
	return InquiryAddrStruct().iResultFlags;
	}

EXPORT_C void TInquirySockAddr::SetResultFlags(TUint8 aResultFlags)
	{
	InquiryAddrStruct().iResultFlags=aResultFlags;
	}


TInquirySockAddr::SInquiryAddr& TInquirySockAddr::InquiryAddrStruct() const
	{
	return *reinterpret_cast<SInquiryAddr*>(UserPtr());	// this class' data at end of superclass
	}

/**  Getter
@return bluetooth device address 
*/
TPtr8 TInquirySockAddr::AddressPtr() const
	{
	return InquiryAddrStruct().iAddress.Des();
	}

/** Getter
@return version information
*/
EXPORT_C TVersion TInquirySockAddr::Version() const
	{
	TInt major = InquiryAddrStruct().iFormatTypeField >> 6;
	TInt minor = InquiryAddrStruct().iFormatTypeField >> 4;
	minor &= 0x03; // 0011
	return TVersion(major, minor, 0); // build number unused and returned as zero
	}

/**
Sets a version number. Only the major and minor numbers of the TVersion parameter are used. Both these must be <=3.
@param aVersion TVersion specifying version number to be assigned 
@internalComponent 
*/
void TInquirySockAddr::SetVersion(TVersion aVersion)
	{
	// N.B. TVersion's iBuild number is not used
	TInt major = aVersion.iMajor;
	TInt minor = aVersion.iMinor;
	ASSERT_DEBUG(major<=3 && minor<=3); // since we only want to use 2 bits for each major/minor number in iFormatTypeField
	major = major << 6;
	minor = minor << 4;
	TUint8 version = major | minor; // MMmmbb00, where M = major, m = minor, b = build (not used)
	InquiryAddrStruct().iFormatTypeField &= 0x0F; // 0000 1111 - set first 4 bits to 0 (erase)
	InquiryAddrStruct().iFormatTypeField |= version; // set version information from aVersion
	}

/** Getter
@return Format Type Field number
*/
EXPORT_C TUint8 TInquirySockAddr::FormatTypeField() const
	{
	// only the 2 least significant bits of iFormatTypeField are used (the other bits are used for version information)
	return (InquiryAddrStruct().iFormatTypeField & 0x03);
	}

/** Sets the Format Type Field number. This is defined as a 2 bit value and so must be <=3.
@param aType is Format Type Field number to be assigned
@internalComponent 
*/
void TInquirySockAddr::SetFormatTypeField(TUint8 aType)
	{
	InquiryAddrStruct().iFormatTypeField &= 0xFC; // 1111 1100 - set last 2 bits to 0 (erase)
	InquiryAddrStruct().iFormatTypeField |= (aType & 0x03); // 0000 0011 - set last 2 bits of iFormatTypeField to those in aFlags
	}


/************ Service Security ************/

EXPORT_C TBTServiceSecurity::TBTServiceSecurity(const TBTServiceSecurity& aService)
/**
Copy constructor
@publishedAll
@released
*/
	{
	iUid = aService.Uid();
	iSecurityRequirements = aService.iSecurityRequirements;
	}

EXPORT_C TBTServiceSecurity::TBTServiceSecurity()
/**
Default constructor.
Sets all member data to 0.
@publishedAll
@released
*/
	{
	iUid.iUid = 0;
	}

EXPORT_C void TBTServiceSecurity::SetUid(TUid aUid)
/**
Set Uid of the service - used opaquely by Bluetooth system.  

When a security procedure is initiated the Uid may be given to the notifier framework.
The notifier can then use the Uid to produce a displayable string
detailing the service to which the security procudure pertains.
@param aUid 
The uid of the service.  This is not the Bluetooth UUID, 
but is a Symbian-allocated Uid.
@publishedAll
@released
*/
	{
	iUid = aUid;
	}


EXPORT_C void TBTServiceSecurity::SetAuthentication(TBool aPreference)
/**
Sets whether or not any connections to this service need to be authenticated.

Authentication involves the comparison of a stored link key.  If no link key
has been generated and stored (e.g. the remote device has never been authenticated before)
then the user will be asked to enter a pin code (a process known as pairing).
If authentication fails, the connection will not be permitted.

For Bluetooth v2.1 and onward this has slightly changed in that pin code entry is not neccessary 
to generate a link key.  Because of this, this API has been deprecated, users of this API are
encouraged to migrate to the alterative SetAuthentication method which specifies the level of
Man-in-the-Middle protection required for the service.

@see TBTServiceSecurity::SetAuthentication(TBluetoothMitmProtection aPreference)

@param aPreference if aPreference = EFalse then the Bluetooth stack will not perform pairing on Bluetooth v2.0
and earlier connections.
@publishedAll
@deprecated The TBTServiceSecurity::SetAuthentication(TBluetoothMitmProtection)
			function should be used instead.
*/
	{
	iSecurityRequirements.SetAuthentication(aPreference);
	}

EXPORT_C void TBTServiceSecurity::SetAuthorisation(TBool aPreference)
/**
Sets whether or not any connections to this service need to be authorised.

A dialog will be presented to the user alerting them of the connection.  This will
occur for every connection to this service unless the user has explicitly expressed
their trust for the remote device during a previous connection.
Note: it should not be assumed that the UI will support the setting up of trust for remote devices.
@param aPreference.  
if aPreference = ETrue then the Bluetooth stack will perform authorisation
@publishedAll
@released
*/
	{
	iSecurityRequirements.SetAuthorisation(aPreference);
	}

EXPORT_C void TBTServiceSecurity::SetEncryption(TBool aPreference)
/**
Sets whether or not any connections to this service need to be encrypted.

Authentication must precede encryption, therefore it is recommended that SetAuthentication(ETrue)
is also called.
If encryption fails, the connection will not be permitted.
@param aPreference.  
if aPreference = ETrue then the Bluetooth stack will perform encryption
@publishedAll
@released
*/
	{
	iSecurityRequirements.SetEncryption(aPreference);
	}

EXPORT_C void TBTServiceSecurity::SetDenied(TBool aPreference)
/**
Sets whether or not connections to this service are being denied to all but specified devices.

@param aPreference.  if aPreference = ETrue then the Bluetooth stack will deny the connection
@see per device overrides - service can be reached by only selected devices using this method
@publishedAll
@released
*/
	{
	iSecurityRequirements.SetDenied(aPreference);
	}

EXPORT_C TInt TBTServiceSecurity::SetPasskeyMinLength(TUint aPasskeyMinLength)
/**
Sets the minimum of passkey length for connection.

@param aPasskeyMinLength.  if aPreference = 0, no minimal length requirement.
According to BT spec aPasskeyMinLength <= 16
@publishedAll
@released
*/
	{
	return iSecurityRequirements.SetPasskeyMinLength(aPasskeyMinLength);
	}

/**
Sets the level of authentication needed for any connections to this service.

Authentication involves the comparison of a stored link key.  If no link key
has been generated and stored or one has been stored but is of an insufficient
level of authentication then pairing will be performed.

For Bluetooth v2.0 and earlier pairing involves the user entering the same pin code
for both devices involved in the connection to this service.

For Bluetooth v2.1 and onward pairing can be performed in one of four ways.  Some of which
involve the user of the Symbian OS device, some of which do not.  The pairing performed
is dependant on a number of factors including the input and output capabilities of the devices
and the level of authentication (Man-in-the-Middle protection) required.

If authentication fails, or succeeds but with an insufficient level of Man-in-the-Middle
protection, the connection will not be permitted.

@param aPreference The level of Man-in-the-Middle protection required for authentication.
@publishedAll
@released
*/
EXPORT_C void TBTServiceSecurity::SetAuthentication(TBluetoothMitmProtection aPreference)
	{
	iSecurityRequirements.SetAuthentication(aPreference);
	}

/**
Getter - return the level of Man-in-the-middle protection required this service.

@return The level of Man-in-the-Middle protection required for authentication.
@publishedAll
@released
*/
EXPORT_C TBluetoothMitmProtection TBTServiceSecurity::MitmProtection() const
	{
	return iSecurityRequirements.MitmProtection();
	}

EXPORT_C TBool TBTServiceSecurity::AuthorisationRequired() const
/**
Getter - return whether authorisation is required for this service

This function is only appropriate for when the Bluetooth v2.0 and earlier
authentication specification function is used.  Once migrated to the new MITM level
setting this function will not return a useful result, the MitmProtection function
should be used instead.

@see TBTServiceSecurity::MitmProtection

@return EAuthorise if authorisation is required, zero otherwise
@publishedAll
@released
*/
	{
	return iSecurityRequirements.AuthorisationRequired();
	}

EXPORT_C TBool TBTServiceSecurity::EncryptionRequired() const
/**
Getter - return whether encryption is required for this service.
@return EEncrypt if encryption is required, zero otherwise
@publishedAll
@released
*/
	{
	return iSecurityRequirements.EncryptionRequired();
	}

EXPORT_C TBool TBTServiceSecurity::AuthenticationRequired() const
/**
Getter - return whether authentication is required for this service.
@return EAuthenticate if authentication is required, zero otherwise
@publishedAll
@deprecated The TBTServiceSecurity::MitmProtection function should 
			be used instead.
*/
	{
	return iSecurityRequirements.AuthenticationRequired();
	}

EXPORT_C TBool TBTServiceSecurity::Denied() const
/**
Getter - return whether denied for this service.
@return EDenied if denied, zero otherwise
@publishedAll
@released
*/
	{
	return iSecurityRequirements.Denied();
	}

EXPORT_C TUint TBTServiceSecurity::PasskeyMinLength() const
/**
Getter - return minimal passkey length requirement
@return 0 - no passkey length requirement. 1..16  
@publishedAll
@released
*/
	{
	return iSecurityRequirements.PasskeyMinLength();
	}

EXPORT_C TUid TBTServiceSecurity::Uid() const
/**
Getter - return Uid for this service.
@return Uid
@publishedAll
@released
*/
	{
	return iUid;
	}

//

EXPORT_C TBTAccessRequirements::TBTAccessRequirements()
/** Constructor
*/
	{
	iRequirements = 0;
	iPasskeyMinLength = 0;
	}

EXPORT_C void TBTAccessRequirements::SetAuthentication(TBool aPreference)
/** Sets the authentication requirement of this class.
@param aPreference If true authentification is added to the requirements.
@deprecated The TBTAccessRequirements::SetAuthentication(TBluetoothMitmProtection)
			function should be used instead.
*/
	{
	if (aPreference)
		iRequirements |= EAuthenticate;
	else
		iRequirements &= ~EAuthenticate;
	}

EXPORT_C void TBTAccessRequirements::SetAuthorisation(TBool aPreference)
/** Sets the authorisation requirement of this class.
@param aPreference If true authorisation is added to the requirements.
*/ 
	{
	if (aPreference)
		iRequirements |= EAuthorise;
	else
		iRequirements &= ~EAuthorise;
	}

EXPORT_C void TBTAccessRequirements::SetEncryption(TBool aPreference)
/** Sets the encryption requirement of this class.
@param aPreference If true encryption is added to the requirements.
*/ 
	{
	if (aPreference)
		iRequirements |= EEncrypt;
	else
		iRequirements &= ~EEncrypt;
	}

EXPORT_C void TBTAccessRequirements::SetDenied(TBool aPreference)
/** Sets the denied requirement of this class.
If this is set no connects will be accepted unless specified in the 
device overrides.
@param aPreference If true the denied attribute is added to the requirements.
*/ 
	{
	if (aPreference)
		iRequirements |= EDenied;
	else
		iRequirements &= ~EDenied;
	}

EXPORT_C TInt TBTAccessRequirements::SetPasskeyMinLength(TUint aPasskeyMinLength)
/** Sets the minimal length requrement for passkey 
If it is between 1..16 SecurityManager will check the length of passkey
@param aPasskeyMinLength If this is set to 0, means no minimal length requirement. 
1..16 is the valid range.
>16 Not valid, we set to 0
*/ 
	{
	if (1<=aPasskeyMinLength && aPasskeyMinLength<=KHCIPINCodeSize)
		{
		iPasskeyMinLength = aPasskeyMinLength;
		return KErrNone;
		}
	else
		{	
		iPasskeyMinLength = 0;
		return KErrOverflow;
		}
	}

/** Sets the Man-in-the-Middle requirements for authentication of this class.
@param aPreference The level of Man-in-the-Middle protection required for authentication.
*/
EXPORT_C void TBTAccessRequirements::SetAuthentication(TBluetoothMitmProtection aPreference)
	{
	iRequirements &= ~EMitm;
	switch(aPreference)
		{
	case EMitmNotRequired:
		iRequirements |= EAccessRequirementsMitmNotRequired;
		break;
	case EMitmDesired:
		iRequirements |= EAccessRequirementsMitmDesired;
		break;
	case EMitmRequired:
		iRequirements |= EAccessRequirementsMitmRequired;
		break;
		}
	}

EXPORT_C TBool TBTAccessRequirements::AuthenticationRequired() const
/** Getter for the authentification attribute.
@return EAuthenticate if authentification is a required attribute, zero otherwise.
@deprecated The TBTAccessRequirements::MitmProtection function should 
			be used instead.
*/
	{
	// We derive whether authentication will be requested based on the computed
	// MITM requirements.
	switch(MitmProtection())
		{
	case EMitmNotRequired:
		break;
	case EMitmDesired:
		// fall-through
	case EMitmRequired:
		return EAuthenticate;
		}
	return EFalse;
	}

EXPORT_C TBool TBTAccessRequirements::AuthorisationRequired() const
/** Getter for the authorisation attribute.
@return EAuthorise if authorisation is a required attribute, zero otherwise.
*/
	{
	return (iRequirements & EAuthorise);
	}

EXPORT_C TBool TBTAccessRequirements::EncryptionRequired() const
/** Getter for the encryption attribute.
@return EEncrypt if encryption is a required attribute, zero otherwise.
*/
	{
	return (iRequirements & EEncrypt);
	}

EXPORT_C TBool TBTAccessRequirements::Denied() const
/** Getter for the denied attribute.
@return EDenied if denied is a required attribute, zero otherwise.
*/
	{
	return (iRequirements & EDenied);
	}

EXPORT_C TUint TBTAccessRequirements::PasskeyMinLength() const
/** Getter for the minimal passkey length
@return 1..16 means set. 0 means - no requirement for minimal passkey length
*/
	{
	return iPasskeyMinLength;
	}

EXPORT_C TBool TBTAccessRequirements::operator==(const TBTAccessRequirements& aRequirements) const
/** Equals operator.
@param aRequirements The instance being compared.
@return True if the requirements specified in this instance match those of the instance passed in the argument.
*/
	{
	return (iRequirements == aRequirements.iRequirements && iPasskeyMinLength == aRequirements.iPasskeyMinLength );
	}

/** Getter for the Man-in-the-Middle protection requirements.
@return The level of Man-in-the-Middle protection required for authentication.
*/
EXPORT_C TBluetoothMitmProtection TBTAccessRequirements::MitmProtection() const
	{
	TBluetoothMitmProtection ret = EMitmNotRequired;
	switch (iRequirements & EMitm)
		{
		case EAccessRequirementsMitmUndefined:
			{
			ret = EMitmNotRequired;
			if(iRequirements & EAuthenticate)
				{
				// This constant represents at what point the minimum pin code length
				// requirements indicates a serious desire for strong protection.
				const TInt KStrongPinLengthLengthThreshold = KHCIPINCodeSize;
				if(PasskeyMinLength() >= KStrongPinLengthLengthThreshold)
					{
					ret = EMitmRequired;
					}
				else 
					{
					ret = EMitmDesired;
					}
				}
			break;
			}
		case EAccessRequirementsMitmNotRequired:
			{
			ret = EMitmNotRequired;
			break;
			}
		case EAccessRequirementsMitmDesired:
			{
			ret = EMitmDesired;
			break;
			}
		case EAccessRequirementsMitmRequired:
			{
			ret = EMitmRequired;
			break;
			}
		}
	return ret;
	}
