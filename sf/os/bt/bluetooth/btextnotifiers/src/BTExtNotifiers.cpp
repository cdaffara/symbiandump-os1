// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <btextnotifiers.h>
#include <bluetooth/logger.h>
#include <btextnotifierspartner.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BTNOTIFIERS);
#endif

/**
Constructor.
Sets iValidDeviceClass and iValidUuid to EFalse.
**/
EXPORT_C TBTDeviceSelectionParams::TBTDeviceSelectionParams()
	{
	iValidDeviceClass=EFalse;
	iValidUuid=EFalse;
	}

/**
Sets the UUID to aUUID.
@param aUUID "The UUID"
**/
EXPORT_C void TBTDeviceSelectionParams::SetUUID(const TUUID& aUUID)
	{
	iSdpUuid=aUUID;
	iValidUuid=ETrue;
	}

/**
Sets the device class to aClass.
@param aClass "The device class"
**/
EXPORT_C void TBTDeviceSelectionParams::SetDeviceClass(TBTDeviceClass aClass)
	{
	iDeviceClass=aClass;
	iValidDeviceClass=ETrue;
	}

/**
Returns the UUID contained within these params.
@return The UUID.  The reference will remain valid while this object is in scope.
**/
EXPORT_C const TUUID& TBTDeviceSelectionParams::UUID()
	{
	return iSdpUuid;
	}

/**
Returns the device class contained within these params.
@return The device class.
**/
EXPORT_C TBTDeviceClass TBTDeviceSelectionParams::DeviceClass()
	{
	return iDeviceClass;
	}

/**
Returns ETrue if the device class has been set using SetDeviceClass(.).
@return ETrue if the device class has been set, EFalse if not.
**/
EXPORT_C TBool TBTDeviceSelectionParams::IsValidDeviceClass()
	{
	return iValidDeviceClass;
	}

/**
Returns ETrue if the UUID has been set using SetUUID(.).
@return ETrue if the UUID has been set, EFalse if not.
**/
EXPORT_C TBool TBTDeviceSelectionParams::IsValidUUID()
	{
	return iValidUuid;
	}

/**
Constructor.
Sets iValidBDAddr, iValidDeviceName and iValidDeviceClass to EFalse;
**/
EXPORT_C TBTDeviceResponseParams::TBTDeviceResponseParams()
	{
	iValidBDAddr=EFalse;
	iValidDeviceName=EFalse;
	iValidDeviceClass=EFalse;
	}

/**
Sets the device address to aBDAddr.
@param aBDAddr "The device address"
**/
EXPORT_C void TBTDeviceResponseParams::SetDeviceAddress(const TBTDevAddr& aBDAddr)
	{
	iBDAddr=aBDAddr;
	iValidBDAddr=ETrue;
	}

/**
Sets the device name to aName.
@param aName "The device name"
**/
EXPORT_C void TBTDeviceResponseParams::SetDeviceName(const TDesC& aName)
	{
	iDeviceName=aName;
	iValidDeviceName=ETrue;
	}

/**
Sets the device class to aClass.
@param aClass "The device class"
**/
EXPORT_C void TBTDeviceResponseParams::SetDeviceClass(TBTDeviceClass aClass)
	{
	iDeviceClass=aClass;
	iValidDeviceClass=ETrue;
	}


/**
Returns the device address contained within these params.
@return The device address.  The reference will be valid while this object is in scope.
**/
EXPORT_C const TBTDevAddr& TBTDeviceResponseParams::BDAddr() const
	{
	return iBDAddr;
	}

/**
Returns the device name contained within these params.
@return The device name. The reference will be valid while this object is in scope.
**/
EXPORT_C const TDesC& TBTDeviceResponseParams::DeviceName() const
	{
	return iDeviceName;
	}

/**
Returns the device class contained within these params.
@return The device class.
**/
EXPORT_C TBTDeviceClass TBTDeviceResponseParams::DeviceClass()
	{
	return iDeviceClass;
	}

/**
Returns ETrue if the device address has been set using SetDeviceAddress.
@return ETrue if the device address has been set, EFalse if not.
**/
EXPORT_C TBool TBTDeviceResponseParams::IsValidBDAddr() const
	{
	return iValidBDAddr;
	}

/**
Returns ETrue if the device name has been set using SetDeviceName.
@return ETrue if the device name has been set, EFalse if not.
**/
EXPORT_C TBool TBTDeviceResponseParams::IsValidDeviceName() const
	{
	return iValidDeviceName;
	}

/**
Returns ETrue if the device class has been set using SetDeviceClass(.).
@return ETrue if the device class has been set, EFalse if not.
**/
EXPORT_C TBool TBTDeviceResponseParams::IsValidDeviceClass()
	{
	return iValidDeviceClass;
	}

EXPORT_C TBTDeviceList::TBTDeviceList() :
	iPtr(iDevices), iKNullDevAddr(MAKE_TINT64(0x0000, 0x00000000))
/**

*/
	{
	iDevices.Reset();
	iPosition = 0;
	}

EXPORT_C TInt TBTDeviceList::AddDevice(const TBTDevAddr& aDevAddr)
/**
Add a new device to the list
@note This always appends the new device to the end of the list
@return KErrNone if successful; KErrOverflow if there is no space left in the array
*/
	{
	for(TInt i=0; i < iDevices.Count(); ++i)
		{
		if(iDevices[i] == iKNullDevAddr)
			{
			iDevices[i] = aDevAddr;
			return KErrNone;
			}
		}
	return KErrOverflow;
	}

EXPORT_C TInt TBTDeviceList::GetDevice(TBTDevAddr& aDevAddr)
/**
Get the next device address from the list
@note This method is stateful, and advances automatically to the next device address each time
*/
	{
	if(iDevices[iPosition]!=iKNullDevAddr)
		{
		aDevAddr = iDevices[iPosition];
		++iPosition;
		return KErrNone;
		}
	else
		{
		return KErrEof;
		}
	}

EXPORT_C void TBTDeviceList::Reset()
/**
Reset the contents of the array
*/
	{
	iDevices.Reset();
	iPosition = 0;
	}
	
EXPORT_C TBTDeviceList::operator const TDesC8&() 
	{
	return iPtr;
	}
	
EXPORT_C TBTDeviceList::operator TDes8&()
	{
	return iPtr;
	}
	
EXPORT_C TUint TBTDeviceList::MaxNumberOfDevices()
	{
	return KMaxDevicesForSimultaneousSelection;
	}

/**
Constructor.
Sets iRealmTruncated, iValidRealm, iValidRemoteAddr to EFalse;
**/
EXPORT_C TPbapAuthNotifierParams::TPbapAuthNotifierParams()
	{
	iRealmTruncated = EFalse;
	iValidRealm = EFalse;
	iValidRemoteAddr = EFalse;
	}

/**
Sets the realm to aRealm. If the length of aRealm is greater than the maximum
length of iRealm then aRealm will be truncated.
@param aRealm "The Realm"
**/
EXPORT_C void TPbapAuthNotifierParams::SetRealm(const TDesC& aRealm)
	{
	iRealm = aRealm.Left(KPbapAuthRealmLength);
	iValidRealm = ETrue;
	
	// check if iRealm was truncated
	iRealmTruncated = (aRealm.Length() > KPbapAuthRealmLength) ? ETrue : EFalse;
	}

/**
Sets the remote device address to aBDAddr.
@param aBDAddr "The remote device address"
**/
EXPORT_C void TPbapAuthNotifierParams::SetRemoteAddr(const TBTDevAddr& aBDAddr)
	{
	iRemoteAddr = aBDAddr;
	iValidRemoteAddr = ETrue;
	}

/**
Returns the realm contained within these params.
@return The realm. The reference will be valid while this object is in scope.
**/
EXPORT_C const TDesC& TPbapAuthNotifierParams::Realm() const
	{
	return iRealm;
	}

/**
Returns ETrue if the realm was truncated during call to SetRealm.
@return ETrue if the realm was truncated during call to SetRealm, EFalse if not.
**/
EXPORT_C TBool TPbapAuthNotifierParams::RealmTruncated() const
	{
	return iRealmTruncated;
	}

/**
Returns the remote device address contained within these params.
@return The remote device address. The reference will be valid while this object is in scope.
**/
EXPORT_C const TBTDevAddr& TPbapAuthNotifierParams::RemoteAddr() const
	{
	return iRemoteAddr;
	}

/**
Returns ETrue if the realm has been set.
@return ETrue if the realm has been set, EFalse if not.
**/
EXPORT_C TBool TPbapAuthNotifierParams::IsValidRealm() const
	{
	return iValidRealm;
	}

/**
Returns ETrue if the remote device address has been set.
@return ETrue if the remote device address has been set, EFalse if not.
**/
EXPORT_C TBool TPbapAuthNotifierParams::IsValidRemoteAddr() const
	{
	return iValidRemoteAddr;
	}


/**
Constructor.
Sets iValidPassword to EFalse;
**/
EXPORT_C TPbapAuthNotifierResponse::TPbapAuthNotifierResponse()
	{
	iValidPassword = EFalse;
	}

/**
Sets the password to aPassword. If the length of aPassword is greater than the maximum
length of iPassword then the password will not be set and EFalse will be returned.
@param aPassword "The Authentication Password"
@return ETrue if the password was successfully set, EFalse if not.
**/
EXPORT_C TBool TPbapAuthNotifierResponse::SetPassword(const TDesC& aPassword)
	{
	if (aPassword.Length() <= KPbapAuthPasswordLength)
		{
		iPassword = aPassword;
		iValidPassword = ETrue;
		}
	else
		{
		iPassword.Zero();
		iValidPassword = EFalse;
		}
			
	return iValidPassword;
	}

/**
Clears any password contained within these params.
**/
EXPORT_C void TPbapAuthNotifierResponse::ResetPassword()
	{
	iPassword.Zero();
	iValidPassword = EFalse;
	}

/**
Returns the password contained within these params.
@return The password. The reference will be valid while this object is in scope.
**/
EXPORT_C const TDesC& TPbapAuthNotifierResponse::Password() const
	{
	return iPassword;
	}

/**
Returns ETrue if the password has been set.
@return ETrue if the password has been set, EFalse if not.
**/
EXPORT_C TBool TPbapAuthNotifierResponse::IsValidPassword() const
	{
	return iValidPassword;
	}


//
// TPanConnection
//
EXPORT_C TPanConnection::TPanConnection(TBTDevAddr& aRemoteDeviceAddress,
				   			   			TBool aUplinkAccessAllowed)
  : iRemoteDeviceAddress(aRemoteDeviceAddress),
    iUplinkAccessAllowed(aUplinkAccessAllowed)
    {
    }

EXPORT_C TPanConnection::TPanConnection()
  : iRemoteDeviceAddress(0),
    iUplinkAccessAllowed(EFalse)
    {
    }
				   
EXPORT_C const TBTDevAddr& TPanConnection::RemoteDeviceAddress() const
	{
	return iRemoteDeviceAddress;
	}
	
EXPORT_C TBool TPanConnection::UplinkAccessAllowed() const
	{
	return iUplinkAccessAllowed;
	}
	
EXPORT_C TBool TPanConnection::IsValid() const
	{
	// Check if the address is valid (0x0000 0000 0000 is not a valid address).
	return (iRemoteDeviceAddress != 0);
	}


//
// TPanConnectionList
//
EXPORT_C TPanConnectionList::TPanConnectionList()
  : iPosition(0)
  	{
	}

EXPORT_C void TPanConnectionList::AddRemoteConnectionL(const TPanConnection& aRemoteConnection)
/**
Add a new PAN connection to the list
@note This always appends the new connection to the end of the list
@leave KErrNone if successful; KErrOverflow if there is no space left in the array
*/
	{
	TInt err = KErrOverflow;
	for(TInt i=0;i<KMaxRemoteConnections;i++)
		{
		if(!iRemoteConnections[i].IsValid())
			{
			iRemoteConnections[i] = aRemoteConnection;
			err = KErrNone;
			break;
			}
		}
		
	User::LeaveIfError(err);
	}

EXPORT_C const TPanConnection* TPanConnectionList::GetNextRemoteConnection()
/**
Get the next device address from the list
@note This method is stateful, and advances automatically to the next device address each time
*/
	{
	TPanConnection* panConn = NULL;
	if(iPosition < KMaxRemoteConnections && iRemoteConnections[iPosition].IsValid())
		{
		panConn = &iRemoteConnections[iPosition++];
		}
	return panConn;
	}

EXPORT_C const TPanConnection* TPanConnectionList::GetFirstRemoteConnection()
	{
	// Reset the internal state pointer to start from the begining of the array
	iPosition = 0;
	return GetNextRemoteConnection();
	}

EXPORT_C void TPanConnectionList::SetRequestedConnectionAddr(const TBTDevAddr& aRequestedConnectionAddr)
	{
	iRequestedConnectionAddr = aRequestedConnectionAddr;
	}
	
EXPORT_C const TBTDevAddr* TPanConnectionList::RequestedConnectionAddr()
	{
	// Check if the address is valid (0x0000 0000 0000 is not a valid address).
	if(iRequestedConnectionAddr != 0)
		{
		return &iRequestedConnectionAddr;
		}
	return NULL;
	}

EXPORT_C const TUUID& TPanConnectionList::CurrentLocalRole() const
	{
	return iCurrentLocalRole;
	}
	
EXPORT_C void TPanConnectionList::SetCurrentLocalRole(const TUUID& aLocalRole)
	{
	iCurrentLocalRole = aLocalRole;
	}
	
	
EXPORT_C TUint8 TPanConnectionList::MaxNumberOfRemoteConnections()
	{
	return KMaxRemoteConnections;
	}



//
// Standard Bluetooth Authentication Notifiers
//


//
// TBTNotifierParams2
//

/**
@internalTechnology
*/
TBTNotifierParams2::TBTNotifierParams2(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName)
	: iBDAddr(aDevAddr)
	, iName(aDeviceName)
	{
	LOG_FUNC
	}

/**
Default constructor.  Required to allow the class to be packaged with the TPckg family of classes.
@internalTechnology
*/
TBTNotifierParams2::TBTNotifierParams2()
	: iBDAddr(0)
	, iName(KNullDesC)
	{
	LOG_FUNC
	}

/**
@return The Bluetooth device address of remote device with which a notifier is associated with.
*/
EXPORT_C TBTDevAddr TBTNotifierParams2::DeviceAddress() const
	{
	LOG_FUNC
	return iBDAddr;
	}

/**
@return The device name of the remote device with which a notifier is associated with.
*/
EXPORT_C TPtrC TBTNotifierParams2::DeviceName() const
	{
	LOG_FUNC
	return TPtrC(iName);
	}


//
// TBTNumericComparisonParams
//

/**
@internalTechnology
*/
EXPORT_C TBTNumericComparisonParams::TBTNumericComparisonParams(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName, TUint32 aNumericalValue, TBTNumericComparisonParams::TComparisonScenario aComparisonScenario, TBool aLocallyInitiated)
	: TBTNotifierParams2(aDevAddr, aDeviceName)
	, iNumericalValue(aNumericalValue)
	, iComparisonScenario(aComparisonScenario)
	, iLocallyInitiated(aLocallyInitiated)
	{
	LOG_FUNC
	}

/**
Default constructor.  Required to allow the class to be packaged with the TPckg family of classes.
@internalTechnology
*/
EXPORT_C TBTNumericComparisonParams::TBTNumericComparisonParams()
	: TBTNotifierParams2()
	{
	LOG_FUNC
	}

/**
Accessor for the 6 digit decimal numeric value to be used for comparison. 
@return The numeric value to display to the user for numeric comparison.
*/
EXPORT_C TUint32 TBTNumericComparisonParams::NumericalValue() const
	{
	LOG_FUNC
	return iNumericalValue;
	}

/**
Accessor for the enumeration that indicates the particular numeric comparison scenario 
that the notifier is indicating.
@return The scenario under which the numeric comparison will take place.
*/
EXPORT_C TBTNumericComparisonParams::TComparisonScenario TBTNumericComparisonParams::ComparisonScenario() const
	{
	LOG_FUNC
	return iComparisonScenario;
	}

/**
Accessor for indicating whether the numeric comparison process was initiated by a local request,
or whether it was trigger by the remote device.
@return EFalse if remote side initiated, otherwise it is locally initiated.
*/
EXPORT_C TBool TBTNumericComparisonParams::LocallyInitiated() const
	{
	LOG_FUNC
	return iLocallyInitiated;
	}


//
// TBTPasskeyDisplayParams
//

/**
@internalTechnology
*/
EXPORT_C TBTPasskeyDisplayParams::TBTPasskeyDisplayParams(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName, TUint32 aNumericalValue, TBool aLocallyInitiated)
	: TBTNotifierParams2(aDevAddr, aDeviceName)
	, iNumericalValue(aNumericalValue)
	, iLocallyInitiated(aLocallyInitiated)
	{
	LOG_FUNC
	}

/**
Default constructor.  Required to allow the class to be packaged with the TPckg family of classes.
@internalTechnology
*/
EXPORT_C TBTPasskeyDisplayParams::TBTPasskeyDisplayParams()
	: TBTNotifierParams2()
	, iNumericalValue(0)
	{
	LOG_FUNC
	}

/**
Accessor for the 6 digit decimal numeric value to be used for passkey entry. 
@return The numeric value to display to the user for entry by the keyboard
*/
EXPORT_C TUint32 TBTPasskeyDisplayParams::NumericalValue() const
	{
	LOG_FUNC
	return iNumericalValue;
	}

/**
Accessor for indicating whether the passkey entry process was initiated by a local request,
or whether it was trigger by the remote device.
@return EFalse if remote side initiated, otherwise it is locally initiated.
*/
EXPORT_C TBool TBTPasskeyDisplayParams::LocallyInitiated() const
	{
	LOG_FUNC
	return iLocallyInitiated;
	}

//
// TBTUserConfirmationParams
//

/**
@internalTechnology
*/
EXPORT_C TBTUserConfirmationParams::TBTUserConfirmationParams(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName, TBool aLocallyInitiated)
	: TBTNotifierParams2(aDevAddr, aDeviceName)
	, iLocallyInitiated(aLocallyInitiated)
	{
	LOG_FUNC
	}

/**
Default constructor.  Required to allow the class to be packaged with the TPckg family of classes.
@internalTechnology
*/
EXPORT_C TBTUserConfirmationParams::TBTUserConfirmationParams()
	: TBTNotifierParams2()
	{
	LOG_FUNC
	}

/**
Accessor for indicating whether the user confirmation process was initiated by a local request,
or whether it was trigger by the remote device.
@return EFalse if remote side initiated, otherwise it is locally initiated.
*/
EXPORT_C TBool TBTUserConfirmationParams::LocallyInitiated() const
	{
	LOG_FUNC
	return iLocallyInitiated;
	}



//
// TBTPinCodeEntryNotifierParams
//

/**
@internalTechnology
*/
EXPORT_C TBTPinCodeEntryNotifierParams::TBTPinCodeEntryNotifierParams(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName, TUint aPinCodeMinLength, TBool aLocallyInitiated, TBool aStrongPinCodeRequired, TUint aRecommendedPinCodeMinLength)
	: TBTNotifierParams2(aDevAddr, aDeviceName)
	, iPinCodeMinLength(aPinCodeMinLength)
	, iLocallyInitiated(aLocallyInitiated)
	, iStrongPinCodeRequired(aStrongPinCodeRequired)
	, iRecommendedPinCodeMinLength(aRecommendedPinCodeMinLength)
	{
	LOG_FUNC
	}
	

/**
Default constructor.  Required to allow the class to be packaged with the TPckg family of classes.
@internalTechnology
*/
EXPORT_C TBTPinCodeEntryNotifierParams::TBTPinCodeEntryNotifierParams()
	: TBTNotifierParams2()
	{
	LOG_FUNC
	}

/**
Accessor for the minimum PIN code length that should be enter into this notifier. 
@return The minimum PIN code length required.  0 means that there is no length requirement.
*/
EXPORT_C TUint TBTPinCodeEntryNotifierParams::PinCodeMinLength() const
	{
	LOG_FUNC
	return iPinCodeMinLength;
	}

/**
Accessor for the recommended minimum PIN code length that should be entered into this notifier. 
This is based on the length of the PIN code used in the previous pairing operation (if available),
and is always greater than or equal to the required minimum PIN code length (as given by PinCodeMinLength()).
If the recommended minimum PIN code length is used, then the strength of the link key used in the previous
pairing operation is at least maintained.
@return The recommended minimum PIN code length.  0 means that there is no length requirement.
*/
EXPORT_C TUint TBTPinCodeEntryNotifierParams::RecommendedPinCodeMinLength() const
	{
	LOG_FUNC
	return iRecommendedPinCodeMinLength;
	}

/**
Accessor for indicating whether the PIN code entry process was initiated by a local request,
or whether it was trigger by the remote device.
@return EFalse if remote side initiated, otherwise it is locally initiated.
*/
EXPORT_C TBool TBTPinCodeEntryNotifierParams::LocallyInitiated() const
	{
	LOG_FUNC
	return iLocallyInitiated;
	}

/**
Accessor for determining if a strong PIN code is required.  It should be used by a notifier
to ensure that a suitably strong PIN code is entered by the user.
@return EFalse if a strong PIN code is not required, otherwise a strong PIN code should be entered if possible.
*/
EXPORT_C TBool TBTPinCodeEntryNotifierParams::StrongPinCodeRequired() const
	{
	LOG_FUNC
	return iStrongPinCodeRequired;
	}



//
// TBTNotifierUpdateParams2
//

/**
@internalTechnology
*/
TBTNotifierUpdateParams2::TBTNotifierUpdateParams2(TBTNotifierUpdateParams2::TUpdateParamType aType)
	: iType(aType)
	{
	LOG_FUNC
	}

/**
Default constructor.  Required to allow the class to be packaged with the TPckg family of classes.
@internalTechnology
*/
EXPORT_C TBTNotifierUpdateParams2::TBTNotifierUpdateParams2()
	{
	LOG_FUNC
	}

/**
Accessor to determine the type of update parameter this instance is.  It should be used to determine
what concrete update parameter type should be cast to.
@return The type of update that is being made.
*/
EXPORT_C TBTNotifierUpdateParams2::TUpdateParamType TBTNotifierUpdateParams2::Type() const
	{
	LOG_FUNC
	return iType;
	}


//
// TBTDeviceNameUpdateParams
//

/**
@internalTechnology
*/
EXPORT_C TBTDeviceNameUpdateParams::TBTDeviceNameUpdateParams(const TDesC& aDeviceName, TInt aResult)
	: TBTNotifierUpdateParams2(TBTNotifierUpdateParams2::EDeviceName)
	, iDeviceName(aDeviceName)
	, iResult(aResult)
	{
	LOG_FUNC
	}

/**
Default constructor.  Required to allow the class to be packaged with the TPckg family of classes.
@internalTechnology
*/
EXPORT_C TBTDeviceNameUpdateParams::TBTDeviceNameUpdateParams()
	: TBTNotifierUpdateParams2()
	{
	LOG_FUNC
	}

/**
@return The device name of the remote device with which a notifier is associated with.
*/
EXPORT_C TPtrC TBTDeviceNameUpdateParams::DeviceName() const
	{
	LOG_FUNC
	return TPtrC(iDeviceName);
	}

/**
@return The result of the attempt to get the device name.
*/
EXPORT_C TInt TBTDeviceNameUpdateParams::Result() const
	{
	LOG_FUNC
	return iResult;
	}


//
// TBTPasskeyDisplayUpdateParams
//

/**
@internalTechnology
*/
EXPORT_C TBTPasskeyDisplayUpdateParams::TBTPasskeyDisplayUpdateParams(THCIPasskeyEntryNotificationType aNotificationType)
	: TBTNotifierUpdateParams2(TBTNotifierUpdateParams2::EPasskeyDisplay)
	, iNotificationType(aNotificationType)
	{
	LOG_FUNC
	}

/**
Default constructor.  Required to allow the class to be packaged with the TPckg family of classes.
@internalTechnology
*/
EXPORT_C TBTPasskeyDisplayUpdateParams::TBTPasskeyDisplayUpdateParams()
	: TBTNotifierUpdateParams2()
	{
	LOG_FUNC
	}

/**
Accessor for the keypress notification type that this update type represents.  The use of these
updates is to allow the UI to show the user the process of the entry of keypresses on a device with a
numeric key board but no display. 
@return The type of keypress notification that has been received from the remote device
*/
EXPORT_C THCIPasskeyEntryNotificationType TBTPasskeyDisplayUpdateParams::KeypressNotification() const
	{
	LOG_FUNC
	return iNotificationType;
	}

