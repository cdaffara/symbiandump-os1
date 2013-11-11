// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the major data type definitions for the LBS Host Settings API.
// 
//

/**
 @file
 @publishedPartner
 @deprecated 
*/

#ifndef LBSHOSTSETTINGSCLASSTYPES_H_
#define LBSHOSTSETTINGSCLASSTYPES_H_

#include <e32std.h>

/**
Class types for TLbsHostSettingsBase and its derived classes
 */
enum TLbsHostSettingsClassType
	{
	/**
	Unknown/undefined value 
	*/
	ELbsHostSettingsUnknownClass	= 0x00,
	/**
	Host settings base class type
	*/
	ELbsHostSettingsBaseClass		= 0x01,
	/**
	Host settings SUPL class type
	*/
	ELbsHostSettingsSuplClass		= 0x02,
	/**
	Defined for compiler optimisation
	*/
	ELbsHostSettingsLastClass		= 0xFFFFFFFF
	};

/**
Type for the ID of a host settings store.
*/
typedef TUid TLbsHostStoreId;
/**
Type for the ID of host settings.
*/
typedef TUid TLbsHostSettingsId;
/**
Type for the ID of the creator of host settings.
It identifies the creator as (for example) a "control panel" application, or network provisioned.
*/
typedef TUid TLbsHostCreatorId;

/**
Base class for host settings class types. It defines methods to
get the type of the derived host settings class and the size of the derived class.
*/
class TLbsHostSettingsClassTypeBase
	{
public:
	IMPORT_C TUint32 ClassType() const;
	IMPORT_C TUint ClassSize() const;

protected:
	IMPORT_C TLbsHostSettingsClassTypeBase();

protected:
	/** The type of the derived class */
	TUint32 iClassType;
	/** The size of the derived class */
	TUint iClassSize;
	};

/**
Base class for host settings that adds a method to get the ID of the settings creator. 
*/
class TLbsHostSettingsBase : public TLbsHostSettingsClassTypeBase
	{
public:
	IMPORT_C TLbsHostSettingsBase();
	
	IMPORT_C TLbsHostCreatorId CreatorId() const;
	
	IMPORT_C TLbsHostSettingsId HostSettingsId() const;
	
	IMPORT_C TTime LastModified() const;
public:
	/**
	Sets the creator id
	@param aCreatorId Id to set.
	*/ 
	void SetCreatorId(TLbsHostCreatorId aCreatorId);
	/**
	Sets the host settings id
	@param aId Id to set.
	*/
	void SetHostSettingsId(TLbsHostSettingsId &aId);
	/**
	Set the last modified date of the account.

	The Date must be in the format YYYYMMDD:HHMMSS.MMMMMM
	*/
	void SetLastModifiedL(const TDesC& aTime);
	/**
	Update the last modified date of the account to be the current time.
	*/  
	void UpdateLastModified();
private:
	TLbsHostSettingsId	iHostSettingsId;
	TLbsHostCreatorId	iCreatorId;
	TTime				iLastModified;
	TUint8	iReserved[20];
	};


/**
Maximum length of the array used for KLbsMaxHostNameAddressSize.
This is based on the maximum hostname URL length
 */
	
const TInt KLbsMaxHostNameAddressSize = 256;

/**
 * Maximum length of the buffer used for the name setting
 */

const TInt KLbsMaxSettingName = 256;

/**
 * Maximum length of the buffer used for the providerId setting
 */

const TInt KLbsMaxProviderId = 256;

/** 
 * Type for the network address of a SUPL server
 */
typedef TBuf8<KLbsMaxHostNameAddressSize> TLbsHostNameAddress;

/** 
 * Type for the name of a SUPL server
 */
typedef TBuf8<KLbsMaxSettingName> TLbsHostName;

/**
 *  Type for the location service provider
 */
typedef TBuf8<KLbsMaxProviderId> TLbsProviderId;

/** 
 * ConRef ID corresponding to the default setting
*/
const TInt KLbsSuplUseDefaultConRefId = -100;

/** 
 * Port ID corresponding to the default setting
*/
const TInt KLbsSuplUseDefaultPortId = 7275;

/**
Abstraction of a SUPL account  

The class has available data members to store the address, connection type as well
as connection id associated with an account. Getters/seters are also provided
to retrieve/set the connection type and id associated with an account, the operator
identifier, the name and address of the Home SUPL server.   
*/
class TLbsHostSettingsSupl : public TLbsHostSettingsBase
	{
public:
	/** Defined type for maintaining Binary Compatibility of the _TAuthModes enumeration. 
	Used to specify a bitmask of allowed authentication mechanisms. */
	typedef TUint32 TAuthModes;

	/** SUPL authentication mechanisms.
	
	To simplify interactions with a user, a UI (Control Panel) application may offer 
        only the following combinations:
	  - EAuthNone & EAuthTls & EAuthAcaTls & EAuthPskTls
	  - EAuthTls & EAuthAcaTls & EAuthPskTls
	  - EAuthAcaTls & EAuthPskTls
	  - EAuthPskTls
	  	  
	and translate them as No, Low, Medium and High Security when interacting with a user.
	It is the responsibility of a SUPL Server to use the highest security level supported 
	by both the Server and the SUPL Protocol Module.

	Note that the enumeration may be extended in the future by adding more values. 
	To maintain compatibility any unrecognized values must be handled as 
	EAuthUnknown.*/
	enum _TAuthModes
		{
		/** Unknown/undefined value */
		EAuthUnknown = 0x00,
		/** No authentication allowed. */
		EAuthNone    = 0x01,
		/** TLS authentication allowed. */
		EAuthTls	 = 0x02,
		/** TLS + Alternative Client authentication allowed. */
		EAuthAcaTls	 = 0x04,
		/** PSK-TLS authentication allowed. */
		EAuthPskTls	 = 0x08
		};
	 
	/**
	 * Type of connection point to use for the connection to the SUPL server.
	 * Other connection types are expected in the future. 
	 */
	enum TLbsConnectionType
		{
		/** Connection type is invalid */
		ELbsConnectionTypeInvalid = 0x00,
		/** Network access point */
		ELbsConnectionTypeNap = 0x01,
		/** Proxy connection type */
		ELbsConnectionTypeProxy = 0x02
		};
	
	IMPORT_C TLbsHostSettingsSupl();
	
	IMPORT_C void GetHostNameAddress(TDes8& aHostAddress) const;
	IMPORT_C void SetHostNameAddress(const TDesC8& aHostAddress);

	IMPORT_C void GetConnectionPoint(TInt& aConnectionId, TLbsConnectionType& aConnectionType) const;
	IMPORT_C void SetConnectionPoint(const TInt aConnectionId, const TLbsConnectionType aConnectionType);
		
	IMPORT_C void GetReadOnly(TBool& aReadOnly) const;
	IMPORT_C void SetReadOnly(const TBool aReadOnly);
	
	IMPORT_C void GetPortId(TInt& aPortId) const;
	IMPORT_C void SetPortId(const TInt aPortId);
		
	IMPORT_C void GetName(TDes8& aName) const;
	IMPORT_C void SetName(const TDesC8& aName);
	
	IMPORT_C void GetProviderId(TDes8& aProviderId) const;
	IMPORT_C void SetProviderId(const TDesC8& aProviderId);
	
	IMPORT_C void GetAuthModesMOLR(TAuthModes& aAuthModes) const;
	IMPORT_C void SetAuthModesMOLR(TAuthModes  aAuthModes);

	IMPORT_C void GetAuthModesMTLR(TAuthModes& aAuthModes) const;
	IMPORT_C void SetAuthModesMTLR(TAuthModes  aAuthModes);
protected:
	/**
	SUPL server host address for these settings
	*/
	TLbsHostNameAddress iHostAddress;
	
	/**
	SUPL server host name
	*/
	TLbsHostName iName;
	
	/**
	Provider of the host settings
	*/
	TLbsProviderId iProviderId;
	
	/**
	Network access point for these host settings
	*/
	TInt    			iConnectionId;
	TLbsConnectionType 	iConnectionType;
	
	/**
	 * Whether the host settings are read only
	*/
	TBool iReadOnly;
	
	/**
	Port to use to connect to SUPL server 
	*/
	TInt iPortId;
	
	/**
	Supported authentication modes for terminal initiated location requests
	*/
	TAuthModes			iAuthModesMOLR;		//includes MOLR-Cell/MOLR-AGPS/MOLR-X3P
	
	/**
	Supported authentication modes for network initiated location requests
	*/
	TAuthModes			iAuthModesMTLR;
	
	/**
	Reserved for future use
	*/
	TUint8				iReserved[20];
	};

#endif // LBSHOSTSETTINGSCLASSTYPES_H_


