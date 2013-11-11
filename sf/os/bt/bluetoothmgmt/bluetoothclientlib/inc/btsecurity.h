// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTSECURITY_H
#define BTSECURITY_H

/**
 @file
 @publishedAll
 @released
*/

#include <e32base.h>

/**
An enumeration to represent the possible levels of Man-in-the-Middle protection
that a Bluetooth service may specify.
*/
enum TBluetoothMitmProtection
	{
	EMitmNotRequired	= 0x0, /*!< No Man-in-the-Middle protection is required. */
	EMitmDesired		= 0x1, /*!< Man-in-the-Middle protection should be used where possible. */
	EMitmRequired		= 0x2  /*!< Man-in-the-Middle protection is required. */
	};

NONSHARABLE_CLASS(TBTAccessRequirements)
/** The access requirements set up by a bluetooth service.

An incoming connection must satisfy these criteria before the connection may proceed.
Not spectacularly useful for applications; mainly used by other Bluetooth libraries
@publishedAll
@released
*/
	{
public:
	IMPORT_C TBTAccessRequirements();
	IMPORT_C void SetAuthentication(TBool aPreference);
	IMPORT_C void SetAuthorisation(TBool aPreference);
	IMPORT_C void SetEncryption(TBool aPreference);
	IMPORT_C void SetDenied(TBool aPreference);
	IMPORT_C void SetAuthentication(TBluetoothMitmProtection aPreference);
	IMPORT_C TInt SetPasskeyMinLength(TUint aPasskeyMinLength);	
	IMPORT_C TBool AuthenticationRequired() const;
	IMPORT_C TBool AuthorisationRequired() const;
	IMPORT_C TBool EncryptionRequired() const;
	IMPORT_C TBool Denied() const;
	IMPORT_C TUint PasskeyMinLength() const;
	IMPORT_C TBool operator==(const TBTAccessRequirements& aRequirements) const;
	IMPORT_C TBluetoothMitmProtection MitmProtection() const;
	
private:
	TUint8 iRequirements;
	TUint  iPasskeyMinLength;
	
private:
	enum TBTServiceSecuritySettings
		{
		EAuthenticate = 0x01,
		EAuthorise = 0x02,
		EEncrypt = 0x04,
		EDenied = 0x08,
		EMitm = 0x30, // 2 bit field for MITM
		};
	
	enum TBTAccessRequirementsMitmProtection
		{
		EAccessRequirementsMitmUndefined = 0x00,
		EAccessRequirementsMitmNotRequired = 0x10,
		EAccessRequirementsMitmDesired = 0x20,
		EAccessRequirementsMitmRequired = 0x30
		};
	};

NONSHARABLE_CLASS(TBTServiceSecurity)
/** The security settings of a bluetooth service.

Contains information regarding the service UID and the access requirements.
@publishedAll
@released
*/
	{
public:
	IMPORT_C TBTServiceSecurity(const TBTServiceSecurity& aService);
	IMPORT_C TBTServiceSecurity();
	IMPORT_C void SetUid(TUid aUid);
	IMPORT_C void SetAuthentication(TBool aPreference);
	IMPORT_C void SetAuthorisation(TBool aPreference);
	IMPORT_C void SetEncryption(TBool aPreference);
	IMPORT_C void SetDenied(TBool aPreference);
	IMPORT_C void SetAuthentication(TBluetoothMitmProtection aPreference);
	IMPORT_C TInt SetPasskeyMinLength(TUint aPasskeyMinLength);	
	IMPORT_C TBool AuthorisationRequired() const;
	IMPORT_C TBool EncryptionRequired() const;
	IMPORT_C TBool AuthenticationRequired() const;
	IMPORT_C TBool Denied() const;
	IMPORT_C TUint PasskeyMinLength() const;
	IMPORT_C TUid Uid() const;
	IMPORT_C TBluetoothMitmProtection MitmProtection() const;
	
private:
	TUid iUid;	//<The UID of the service.  Will be used by the UI to work out the name of the service when prompting the user.
	TBTAccessRequirements iSecurityRequirements;	//<Whether the service requires authentication, authorisation, encryption or min passkey len.
	};

typedef TPckgBuf<TBTServiceSecurity> TBTServiceSecurityPckg;	/*!< Package definition for securty settings */


#endif // BTSECURITY_H
