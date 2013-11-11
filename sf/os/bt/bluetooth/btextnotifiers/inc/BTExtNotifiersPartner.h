// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTEXTNOTIFIERSPARTNER_H
#define BTEXTNOTIFIERSPARTNER_H

#include <btextnotifiersconsts.h>
#include <bttypes.h>
#include <btdevice.h>
#include <bluetooth/hci/hcitypes.h>

/**
@publishedPartner
@released

UID of the PBAP authentication notifier
*/
const TUid KPbapAuthNotifierUid={0x10282B22};

/**
@publishedPartner
@deprecated
@see "bt_subscribe.h"

UID to notify the UI that a bluetooth link is currently active.
Now deprecated by the publish/subscribe mechanism.
**/
const TUid KUidBTBasebandNotification={0x1000A5FB};

NONSHARABLE_CLASS(TBTDeviceList)
/**
A list of TBTDevAddrs
@note Used to pass a list of devices to which to connect.
@publishedPartner
@released
*/
	{
public:
	IMPORT_C TBTDeviceList();
	
	IMPORT_C TInt AddDevice(const TBTDevAddr& aDevAddr);
	IMPORT_C TInt GetDevice(TBTDevAddr& aDevAddr);
	IMPORT_C void Reset();
	
	IMPORT_C operator const TDesC8&();
	IMPORT_C operator TDes8&();
	
	IMPORT_C TUint MaxNumberOfDevices();
	
private:
	const static TUint KMaxDevicesForSimultaneousSelection = 8;

private:
	TInt iPosition;	// the position within the list for stateful reading
	TFixedArray<TBTDevAddr, KMaxDevicesForSimultaneousSelection> iDevices;
	TPckg<TFixedArray<TBTDevAddr, KMaxDevicesForSimultaneousSelection> > iPtr;
		
	const TBTDevAddr iKNullDevAddr;

	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

NONSHARABLE_CLASS(TPbapAuthNotifierParams)
/**
Parameters passed to the PBAP authentication notifier plugin
@publishedPartner
@released
*/
	{
public:
	IMPORT_C TPbapAuthNotifierParams();

	IMPORT_C void SetRealm(const TDesC& aRealm);
	IMPORT_C void SetRemoteAddr(const TBTDevAddr& aBDAddr);

	IMPORT_C const TDesC& Realm() const;
	IMPORT_C TBool RealmTruncated() const;
	IMPORT_C const TBTDevAddr& RemoteAddr() const;

	IMPORT_C TBool IsValidRealm() const;
	IMPORT_C TBool IsValidRemoteAddr() const;
	
private:
	TBuf<KPbapAuthRealmLength> iRealm; /*!< The Realm specified by the device requesting authentication. */
	TBool iRealmTruncated; /*!< Set to ETrue if iRealm has been truncated */
	TBTDevAddr iRemoteAddr; /*!< The Bluetooth device address of the device requesting authentication. */ 

	TBool iValidRealm; /*!< ETrue if iRealm has been set, EFalse if not */
	TBool iValidRemoteAddr; /*!< ETrue if iRemoteAddr has been set, EFalse if not */

	// This data padding has been added to help prevent future binary compatibility breaks	
	// iPadding has not been zero'd because they are currently not used
	TUint32     iPadding;
	};

/**
Packaged version of <code>TPbapAuthNotifierParams</code> that allows data transfer over process boundaries.
@publishedPartner
@released
*/
typedef TPckgBuf<TPbapAuthNotifierParams> TPbapAuthNotifierParamsPckg;

NONSHARABLE_CLASS(TPbapAuthNotifierResponse)
/**
Response data from the PBAP authentication notifier plugin
@publishedPartner
@released
*/
	{
public:
	IMPORT_C TPbapAuthNotifierResponse();

	IMPORT_C TBool SetPassword(const TDesC& aPassword);
	IMPORT_C void ResetPassword();

	IMPORT_C const TDesC& Password() const;

	IMPORT_C TBool IsValidPassword() const;

private:
    /** The password used to create the challenge response. */
	TBuf<KPbapAuthPasswordLength> iPassword;

	TBool iValidPassword; /*!< ETrue if iPassword has been set, EFalse if not */

	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

/**
Packaged version of <code>TPbapAuthNotifierResponse</code> that allows data transfer over process boundaries.
@publishedPartner
@released
*/
typedef TPckgBuf<TPbapAuthNotifierResponse> TPbapAuthNotifierResponsePckg;


//
// Standard Bluetooth Authentication Notifiers
//


/**
UID of the "Numeric Comparison" confirmation request RNotifier plugin.

The notifier will be started with a parameter of type TBTNumericComparisonParamsPckg.

The notifier will receive updates of type TBTNotifierUpdateParamsPckg2.

@see TBTNotifierUpdateParamsPckg2

@publishedPartner
@released
*/
const TUid KBTNumericComparisonNotifierUid={0x10285AD5};

/**
UID of the "Passkey Entry" display RNotifier plugin.

The notifier will be started with a parameter of type TBTPasskeyDisplayParamsPckg.

The notifier will receive updates of type TBTNotifierUpdateParamsPckg2.

@see TBTNotifierUpdateParamsPckg2

@publishedPartner
@released
*/
const TUid KBTPasskeyDisplayNotifierUid={0x10285AD4};

/*
UID of the "User Confirmation" confirmation request RNotifier plugin.

The notifier will be started with a parameter of type TBTUserConfirmationParamsPckg.

The notifier will receive updates of type TBTNotifierUpdateParamsPckg2.

@see TBTNotifierUpdateParamsPckg2

@publishedPartner
@released
*/
const TUid KBTUserConfirmationNotifierUid={0x2002E224};

/**
UID of the "PIN Code Entry" RNotifier plugin.

Note that this is an SSP aware notifier taking the new parameter type.  This is
intended to replace the deprecated KBTManPinNotifierUid RNotifier plugin.
@see KBTManPinNotifierUid

The notifier will be started with a parameter of type TBTPinCodeEntryNotifierParamsPckg.

The notifier will receive updates of type TBTNotifierUpdateParamsPckg2.

@see TBTNotifierUpdateParamsPckg2

@publishedPartner
@released
*/
const TUid KBTPinCodeEntryNotifierUid={0x10285D55};

/**
The base parameter for the secure simple pairing notifiers.
	
@publishedPartner
@released
*/
NONSHARABLE_CLASS(TBTNotifierParams2)	
	{
public:
	IMPORT_C TBTDevAddr DeviceAddress() const;
	IMPORT_C TPtrC DeviceName() const;
	
protected:
	TBTNotifierParams2(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName);
	TBTNotifierParams2();
	
private:
	TBTDevAddr		iBDAddr;
	TBTDeviceName	iName;	
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32	iPadding1; 
	TUint32	iPadding2; 
	};

/** 
Secure simple pairing "Numeric Comparison" parameters.
This class packages the parameters passed to a numeric comparison notifier.
	
@publishedPartner
@released
*/
NONSHARABLE_CLASS(TBTNumericComparisonParams)
	: public TBTNotifierParams2
	{
public:
	/**
	This represents the type of comparison that is being performed.
	It is intended to allow the UI to present the user of the device
	with suitable information to help them complete the pairing.
	*/
	enum TComparisonScenario
		{
		ERemoteCannotConfirm,	/*!< Only the Symbian device can confirm the displayed numbers are the same. */
		ERemoteCanConfirm,		/*!< Both devices can confirm that the displayed numbers are the same */
		};
		
public:
	IMPORT_C TBTNumericComparisonParams(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName, TUint32 aNumericalValue, TComparisonScenario aComparisonScenario, TBool aLocallyInitiated);
	IMPORT_C TBTNumericComparisonParams();
	
public:
	IMPORT_C TUint32 NumericalValue() const;
	IMPORT_C TComparisonScenario ComparisonScenario() const;
	IMPORT_C TBool LocallyInitiated() const;
	
private:
	TUint32				iNumericalValue;
	TComparisonScenario	iComparisonScenario;
	TBool				iLocallyInitiated;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32	iPadding1;
	TUint32 iPadding2;
	};

/**
Secure simple pairing "Passkey Entry" display parameters.
This class packages the parameters passed to a passkey display notifier.
	
@publishedPartner
@released
*/
NONSHARABLE_CLASS(TBTPasskeyDisplayParams)
	: public TBTNotifierParams2	
	{
public:
	IMPORT_C TBTPasskeyDisplayParams(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName, TUint32 aNumericalValue, TBool aLocallyInitiated);
	IMPORT_C TBTPasskeyDisplayParams();
	
public:
	IMPORT_C TUint32 NumericalValue() const;
	IMPORT_C TBool LocallyInitiated() const;
	
private:
	TUint32	iNumericalValue;
	TBool	iLocallyInitiated;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32	iPadding1; 
	TUint32	iPadding2;
	};

/** 
Simple secure pairing aware "PIN Code Entry" request parameters.
This class packages the parameters passed to a PIN code entry notifier.
	
@publishedPartner
@released
*/
NONSHARABLE_CLASS(TBTPinCodeEntryNotifierParams)
	: public TBTNotifierParams2
	{
public:
	IMPORT_C TBTPinCodeEntryNotifierParams(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName, TUint aPinCodeMinLength, TBool aLocallyInitiated, TBool aStrongPinCodeRequired, TUint aRecommendedPinCodeMinLength);
	IMPORT_C TBTPinCodeEntryNotifierParams();
	
public:
	IMPORT_C TUint PinCodeMinLength() const;
	IMPORT_C TBool LocallyInitiated() const;
	IMPORT_C TBool StrongPinCodeRequired() const;
	IMPORT_C TUint RecommendedPinCodeMinLength() const;
	
private:
	TUint	iPinCodeMinLength;
	TBool	iLocallyInitiated;
	TBool	iStrongPinCodeRequired;
	//TUint32 used here as this space was once a padding field.
	TUint32	iRecommendedPinCodeMinLength;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// iPadding1 has not been zero'd because it is currently not used
	TUint32	iPadding1; 
	};

/** 
Secure simple pairing "User Confirmation" parameters.
This class packages the parameters passed to a numeric comparison notifier.
	
@publishedPartner
@released
*/
NONSHARABLE_CLASS(TBTUserConfirmationParams)
	: public TBTNotifierParams2
	{	
public:
	IMPORT_C TBTUserConfirmationParams(const TBTDevAddr& aDevAddr, const TDesC& aDeviceName, TBool aLocallyInitiated);
	IMPORT_C TBTUserConfirmationParams();
	
public:
	IMPORT_C TBool LocallyInitiated() const;
	
private:
	TBool				iLocallyInitiated;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32	iPadding1;
	TUint32 iPadding2;
	};


/**
The base update parameter for the Bluetooth pairing notifiers.
This class is intended to allow multiple different updates parameters to be
passed to the Bluetooth notifiers.
	
@publishedPartner
@released
*/
NONSHARABLE_CLASS(TBTNotifierUpdateParams2)
	{
public:
	enum TUpdateParamType
		{
		EDeviceName,
		EPasskeyDisplay,
		};
	
public:
	IMPORT_C TBTNotifierUpdateParams2();
	IMPORT_C TUpdateParamType Type() const;
	
protected:
	TBTNotifierUpdateParams2(TBTNotifierUpdateParams2::TUpdateParamType aType);
	
private:
	TUpdateParamType	iType;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32	iPadding1;
	TUint32	iPadding2;
	};

/** 
Bluetooth device name update parameter.
This class provides the parameters to indicate an update to the name of
the Bluetooth device for which the notifier is waiting for input.
	
@publishedPartner
@released
*/
NONSHARABLE_CLASS(TBTDeviceNameUpdateParams)
	: public TBTNotifierUpdateParams2
	{
public:
	IMPORT_C TBTDeviceNameUpdateParams(const TDesC& aDeviceName, TInt aResult);
	IMPORT_C TBTDeviceNameUpdateParams();
	
public:
	IMPORT_C TPtrC DeviceName() const;
	IMPORT_C TInt Result() const;
	
private:
	TBTDeviceName	iDeviceName;
	TInt			iResult;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32	iPadding1; 
	TUint32	iPadding2;
	};

/** 
Secure simple pairing "Passkey Entry" display update parameters.
This class provides the update parameters associated with a passkey entry
display notifier.  Notably it provides the notification of keypresses from
the remote device.

@publishedPartner
@released
*/
NONSHARABLE_CLASS(TBTPasskeyDisplayUpdateParams) 
	: public TBTNotifierUpdateParams2	
	{
public:
	IMPORT_C TBTPasskeyDisplayUpdateParams(THCIPasskeyEntryNotificationType aNotificationType);
	IMPORT_C TBTPasskeyDisplayUpdateParams();
	
public:
	IMPORT_C THCIPasskeyEntryNotificationType KeypressNotification() const;
	
private:
	THCIPasskeyEntryNotificationType iNotificationType;
	
	TUint32 iPadding1;
	TUint32 iPadding2;
	};

/**
Typedef'ed pckgbuf to send numeric comparison parameters to the notifier framework.
@publishedPartner
@released
*/
typedef TPckgBuf<TBTNumericComparisonParams> TBTNumericComparisonParamsPckg;
/**
Typedef'ed pckgbuf to send passkey entry display parameters to the notifier framework.
@publishedPartner
@released
*/
typedef TPckgBuf<TBTPasskeyDisplayParams> TBTPasskeyDisplayParamsPckg;
/**
Typedef'ed pckgbuf to send passkey entry display parameters to the notifier framework.
@publishedPartner
@released
*/
typedef TPckgBuf<TBTUserConfirmationParams> TBTUserConfirmationParamsPckg;
/**
Typedef'ed pckgbuf to send PIN code entry parameters to the notifier framework.
@publishedPartner
@released
*/
typedef TPckgBuf<TBTPinCodeEntryNotifierParams> TBTPinCodeEntryNotifierParamsPckg;

/**
Typedef'ed pckgbuf to represent the base class of Bluetooth update parameters.
@publishedPartner
@released
*/
typedef TPckgBuf<TBTNotifierUpdateParams2> TBTNotifierUpdateParamsPckg2;
/**
Typedef'ed pckgbuf to send device name update parameters to the notifier framework.
@publishedPartner
@released
*/
typedef TPckgBuf<TBTDeviceNameUpdateParams> TBTDeviceNameUpdateParamsPckg;
/**
Typedef'ed pckgbuf to send passkey entry display update parameters to the notifier framework.
@publishedPartner
@released
*/
typedef TPckgBuf<TBTPasskeyDisplayUpdateParams> TBTPasskeyDisplayUpdateParamsPckg;

#endif // BTEXTNOTIFIERSPARTNER_H
