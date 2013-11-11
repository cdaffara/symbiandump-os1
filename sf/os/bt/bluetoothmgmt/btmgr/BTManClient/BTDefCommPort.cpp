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
// Implements classes dealing with setting virtual serial ports
// 
//

#include <btdefcommport.h>
#include <bttypes.h>
#include <e32base.h>
#include <s32strm.h>
#include "btmanclientserver.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_CLIENT);
#endif


/**
Constructor
@param aName The name of the virtual serial port
@param aUUID the UUID to use on the remote device
@param aBTCommPort The virtual serial port number
@param aSecurityLevel The Bluetooth security desired for the connection
@param aBDAddr The Bluetooth device address of the remote device
@see Bluetooth Assigned numbers for UUIDs

@publishedAll
@released
*/
EXPORT_C TBTCommPortSettings::TBTCommPortSettings(const TDesC& aName, TUUID aUUID, 
		TUint32 aBTCommPort, TUint8 aSecurityLevel, TBTDevAddr aBDAddr) :
	iName(aName),
	iUUID(aUUID),
	iPort(aBTCommPort),
	iSecurityLevel(aSecurityLevel),
	iBDAddr(aBDAddr)
	{
	LOG_FUNC
	}

/**
Default constructor
@publishedAll
@released
*/
EXPORT_C TBTCommPortSettings::TBTCommPortSettings() :
	iUUID(),
	iPort(0),
	iSecurityLevel(0),
	iBDAddr(TInt64(0))
	{
	LOG_FUNC
	iName.Zero();
	}

/**
Getter for the name of the virtual serial port
@return name of virtual serial port
@publishedAll
@released
*/
EXPORT_C const TDesC& TBTCommPortSettings::Name() const
	{
	LOG_FUNC
	return iName;
	}

/**
Getter for the UUID of the virtual serial port
@return UUID of virtual serial port
@publishedAll
@released
*/
EXPORT_C TUUID TBTCommPortSettings::UUID() const
	{
	LOG_FUNC
	return iUUID;
	}

/**
Getter for the port number of the virtual serial port
@return port number of virtual serial port
@publishedAll
@released
*/
EXPORT_C TUint32 TBTCommPortSettings::Port() const
	{
	LOG_FUNC
	return iPort;
	}

/**
Setter for the name of the virtual serial port
@param aName name of the virtual serial port
@publishedAll
@released
*/
EXPORT_C void TBTCommPortSettings::SetName(const TDesC& aName)
	{
	LOG_FUNC
	iName = aName;
	}


/**
Setter for the UUID of the virtual serial port
@param aUUID UUID of the virtual serial port
@publishedAll
@released
*/
EXPORT_C void TBTCommPortSettings::SetUUID(TUUID aUUID)
	{
	LOG_FUNC
	iUUID = aUUID;
	}

/**
Getter for the remote device address of the virtual serial port
@return remote device address of the virtual serial port
@publishedAll
@released
*/
EXPORT_C TBTDevAddr TBTCommPortSettings::BDAddr() const
	{
	LOG_FUNC
	return iBDAddr;
	}

/**
Setter for the remote device address of the virtual serial port
@param aBDAddr remote device address of the virtual serial port
@publishedAll
@released
*/
EXPORT_C void TBTCommPortSettings::SetBTAddr(TBTDevAddr aBDAddr)
	{
	LOG_FUNC
	iBDAddr = aBDAddr;
	}

/**
Setter for the port number of the virtual serial port
@param aBTCommPort port number of the virtual serial port
@publishedAll
@released
*/
EXPORT_C void TBTCommPortSettings::SetPort(TUint32 aBTCommPort)
	{
	LOG_FUNC
	iPort=aBTCommPort;
	}

/**
Setter to request authentication of the virtual serial connection when connection occurs
@publishedAll
@released
*/
EXPORT_C void TBTCommPortSettings::SetAuthentication()
	{
	LOG_FUNC
	iSecurityLevel=KBTDefaultServiceSecurityAuthenticateOnly;
	}

/**
Setter to request authentication and encryption of the virtual serial connection when connection occurs
@publishedAll
@released
*/
EXPORT_C void TBTCommPortSettings::SetAuthenticationAndEncryption() 
	{
	LOG_FUNC
	iSecurityLevel=KBTDefaultServiceSecurityAuthenticateAndEncrypt;
	}

/** 
Setter to clear any setting of security settings
@publishedAll
@released
*/
EXPORT_C void TBTCommPortSettings::SetNoSecurity() 
	{
	LOG_FUNC
	iSecurityLevel=KBTDefaultServiceSecurityNone;
	}

/**
Sets the security level for this default service.
@param aLevel the security level:
KBTDefaultServiceSecurityNone                   = 0;
KBTDefaultServiceSecurityAuthenticateOnly       = 1;
KBTDefaultServiceSecurityAuthenticateAndEncrypt = 2;

@publishedAll
@released
*/
EXPORT_C void TBTCommPortSettings::SetSecurityLevel(TUint8 aLevel)
	{
	LOG_FUNC
	iSecurityLevel=aLevel;
	}


/**
Getter for the security level of the virtual serial port
@return the level
@see  TBTCommPortSettings::SetSecurityLevel
@publishedAll
@released
*/
EXPORT_C TUint8 TBTCommPortSettings::SecurityLevel() const
	{
	LOG_FUNC
	return iSecurityLevel;
	}

/**
Getter to return whether any security level is set
@return EFalse if no security has been set, otherwise ETrue
@publishedAll
@released
*/
EXPORT_C TBool TBTCommPortSettings::IsSecuritySet() const
	{
	LOG_FUNC
	return (iSecurityLevel);
	}


/**
Getter to return whether encryption has been set for the virtual serial port connection
@return ETrue if encryption has been set, otherwise EFalse
@publishedAll
@released
*/
EXPORT_C TBool TBTCommPortSettings::IsEncryptionSet() const
	{
	LOG_FUNC
	return (iSecurityLevel==KBTDefaultServiceSecurityAuthenticateAndEncrypt);
	}

/**
Getter to return whether only authentication has been set for the virtual serial port connection
@return ETrue if only authentication has been set, otherwise EFalse
@publishedAll
@released
*/
EXPORT_C TBool TBTCommPortSettings::IsAuthenticationOnlySet() const
	{
	LOG_FUNC
	return (iSecurityLevel==KBTDefaultServiceSecurityAuthenticateOnly);
	}

/**
Default constructor
@publishedAll
@released
*/
EXPORT_C RBTCommPortSettings::RBTCommPortSettings()
: RBTManSubSession()
	{
	LOG_FUNC
	}

/**
Open the virtual serial port subsession on the registry server
@param aSession the session on the Registry server on which to open this subsession
@return Systemwide error code
@publishedAll
@released
*/
EXPORT_C TInt RBTCommPortSettings::Open(RBTRegServ& aSession)
	{
	LOG_FUNC
	return CreateSubSession(aSession.Session(), EBTManCreateCommPortSettingsSubSession, TIpcArgs(NULL));
	}

/**
Get the virtual serial port settings
@param aPortSettings reference to a TBTCommPortSettings used for both an 'input' and 'output'.  For the input the aPortSettings MUST have a serial
port number.  This is used to retrieve the remaining details for that virtual serial port from the Registry.
When the call completes the client's TBTCommPortSettings will have been fully set with all values.
Note this is a synchronous call.
@return Systemwide error code
@publishedAll
@released
*/
EXPORT_C TInt RBTCommPortSettings::Get(TBTCommPortSettings& aPortSettings)
/**
	Get the port settings for a given port
	At present the lookup is based on the port number in aPortSettings

	Synchronous

	@param	aPortSettings Reference to TBTCommPortSettings for result, and uses the port number within it to search
	@return	System-wide error
**/
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		TPckg<TBTCommPortSettings> pckg(aPortSettings);	// TPckg=>Server updates client variable
		return SendReceive(EBTRegistryGetCommPortSettings, TIpcArgs(&pckg, NULL, NULL));	// synchronous
		}
	else
		{
		//Client has a bad handle therefore complete the call from here
		return KErrBadHandle;
		}
	}

/**
Modify or add the virtual serial port settings
@param aPortSettings reference to a const TBTCommPortSettings.  The aPortSettings MUST have a serial
port number.  This is used by the Registry to find the current settings and update with the other settings in aSettings.
If the Registry cannot find the port specified it will add the settings in a new record.
@param aStatus reference to a TRequestStatus. Note this is an asynchronous call.
@publishedAll
@released
*/
EXPORT_C void RBTCommPortSettings::Modify(const TBTCommPortSettings& aSettings, TRequestStatus& aStatus)
/**
	Asynchronously update virtual serial port settings

	@param aSettings	The new settings
**/
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			iPortPckg = aSettings;
			iClientServerMsg().iClientStatusToCancel = &aStatus;
			SendReceive(EBTRegistryUpdateCommPortSettings, TIpcArgs(&iPortPckg, NULL, &iClientServerMsg), aStatus);
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}



/**
Delete virtual serial settings
@param aSettings reference to a const TBTCommPortSettings.  aSettings MUST have a serial
port number.  This is used by the Registry to find the current settings and delete them from the Registry.
@param aStatus reference to a TRequestStatus. Note this is an asynchronous call.
@publishedAll
@released
*/
EXPORT_C void RBTCommPortSettings::Delete(const TBTCommPortSettings& aSettings, TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			iPortPckg = aSettings;
			iClientServerMsg().iClientStatusToCancel = &aStatus;
			SendReceive(EBTRegistryDeleteCommPortSettings, TIpcArgs(&iPortPckg, NULL, &iClientServerMsg), aStatus);
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}


/**
Close the virtual serial port subsession on the Registry
@publishedAll
@released
*/
EXPORT_C void RBTCommPortSettings::Close()
	{
	LOG_FUNC
	RSubSessionBase::CloseSubSession(EBTManCloseSubSession);
	}
