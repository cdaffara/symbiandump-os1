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
//

#ifndef BTDEFCOMMPORT_H__
#define BTDEFCOMMPORT_H__

#include <e32std.h>
#include <e32base.h>
#include <bttypes.h>
#include <s32mem.h>
#include <btmanclient.h>

/**
KBTDefaultServiceSecurityNone
Security setting for virtual serial port - in this case no security
@publishedAll
@released
*/
static const TUint8 KBTDefaultServiceSecurityNone                   = 0;

/**
KBTDefaultServiceSecurityAuthenticateOnly
Security setting for virtual serial port
The virtual serial connection will apply Bluetooth authentication
@publishedAll
@released
*/
static const TUint8 KBTDefaultServiceSecurityAuthenticateOnly       = 1;

/**
KBTDefaultServiceSecurityAuthenticateAndEncrypt
Security setting for virtual serial port
The virtual serial connection will apply Bluetooth authentication and encryption
@publishedAll
@released
*/
static const TUint8 KBTDefaultServiceSecurityAuthenticateAndEncrypt = 2;

/**
KCommHighUnit
The maximum virtual serial port number supported
ie. BTCOMM::KCommHighUnit is the maximum Bluetooth serial port that can be opened
@publishedAll
@released
*/
static const TUint8 KCommHighUnit=63;

NONSHARABLE_CLASS(TBTCommPortSettings)
/**
Used to configure a virtual serial port.  The very least that is needed to configure a virtual serial port
is the Bluetooth device address of the remote, and the SDP UUID to use on that device.

This info can then be saved by the registry and retrieved when needed.
@publishedAll
@released
*/
	{
public:
	IMPORT_C TBTCommPortSettings(const TDesC& aName, TUUID aUUID,TUint32 aBTCommPort, TUint8 aSecurityLevel, TBTDevAddr aBDAddr);
	IMPORT_C TBTCommPortSettings();
	IMPORT_C const TDesC& Name() const;	//< Returns the name of the service.
	IMPORT_C TUUID UUID() const;			//< Returns the unique id of the service.
	IMPORT_C TBTDevAddr BDAddr() const;		//< Returns the address of the default device associated with the service.
	IMPORT_C TUint32 Port() const;
	IMPORT_C TBool IsSecuritySet() const;
	IMPORT_C TBool IsEncryptionSet() const;
	IMPORT_C TBool IsAuthenticationOnlySet() const;
	IMPORT_C void SetName(const TDesC& aName);	//< Sets the name of the service.
	IMPORT_C void SetUUID(TUUID aUUID);			//< Sets the unique id of the service.
	IMPORT_C void SetPort(TUint32 aBTCommPort);
	IMPORT_C void SetAuthentication();
	IMPORT_C void SetAuthenticationAndEncryption(); // enforces authentication as well
	IMPORT_C void SetNoSecurity(); // resets the above two
	IMPORT_C void SetBTAddr(TBTDevAddr aBDAddr);	//< Sets the address of the default device associated with the service.

	IMPORT_C TUint8 SecurityLevel() const;
	IMPORT_C void SetSecurityLevel(TUint8 aLevel);
private:
	TBuf<32>	iName;		    // The name of the service 
	TUUID		iUUID;			// The UUID of the service
	TUint32		iPort;          // The port no of the virtual serial port opened by the legacy app
	TUint8		iSecurityLevel; // for that default service (i.e 0,1,2 = none,auth,encrypt+auth)
	TBTDevAddr	iBDAddr;	    // The address of the default device
	
    // This data padding has been added to help prevent future binary compatibility breaks	
    // Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1;
	TUint32     iPadding2;
	};


typedef TPckgBuf<TBTCommPortSettings> TBTCommPortSettingsPckg; /*!< Handy package to send TBTCommPortSettings over IPC*/



/**
A subsession on the Bluetooth Registry server
This subsession allows the virtual serial ports to be updated, modified, examined etc.

@publishedAll
@released
*/
NONSHARABLE_CLASS(RBTCommPortSettings) : public RBTManSubSession
	{
public:
	IMPORT_C RBTCommPortSettings();
	IMPORT_C TInt Open(RBTRegServ& aSession);	// creates the subsession server side
	IMPORT_C TInt Get(TBTCommPortSettings& aSettings);	//synchronous
	IMPORT_C void Modify(const TBTCommPortSettings& aSettings, TRequestStatus& aStatus); // may add if not present
	IMPORT_C void Delete(const TBTCommPortSettings& aSettings, TRequestStatus& aStatus);
	IMPORT_C void Close();
private:
	TPckgBuf<TBTCommPortSettings>	iPortPckg;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 
	};


#endif
