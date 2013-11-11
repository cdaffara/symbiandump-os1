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
//

/**
 @file
 @internalTechnology
 
*/
#ifndef LBS_SUPL_MESSAGE_COMMON_H
#define LBS_SUPL_MESSAGE_COMMON_H

#include <e32base.h>
	
/**
Enumeration for Status Code parameter.
This is used to report errors in the SUPL END message status code parameter.

@internalTechnology
*/
typedef TUint32 TSuplStatusCode;
	
enum _TSuplStatusCode
	{
	ESuplStatusUnspecified = 0, 
	ESuplStatusSystemFailure = 1, 
	ESuplStatusUnexpectedMessage = 2, 
	ESuplStatusProtocolError = 3,
	ESuplStatusDataMissing = 4, 
	ESuplStatusUnexpectedDataValue = 5, 
	ESuplStatusPosMethodFailure = 6,
	ESuplStatusPosMethodMismatch = 7, 
	ESuplStatusPosProtocolMismatch = 8, 
	ESuplStatusTargetSETnotReachable = 9,
	ESuplStatusVersionNotSupported = 10, 
	ESuplStatusResourceShortage = 11, 
	ESuplStatusInvalidSessionId = 12,
	ESuplStatusNonProxyModeNotSupported = 13, 
	ESuplStatusProxyModeNotSupported = 14,
	ESuplStatusPositioningNotPermitted = 15, 
	ESuplStatusAuthNetFailure = 16, 
	ESuplStatusAuthSuplinitFailure = 17, 
	ESuplStatusConsentDeniedByUser = 100,
	ESuplStatusConsentGrantedByUser = 101
	};


typedef TUint32 TSuplSlpAddressType;
enum _TSuplSlpAddressType
	{
	ESuplSlpAddressTypeIp = 0,
	ESuplSlpAddressTypeFqdn,
	
	ESuplSlpAddressTypeLimit
	};

typedef TUint32 TSuplIpAddressType;
enum _TSuplIpAddressType
	{
	ESuplIpAddressTypeV4 = 0,
	ESuplIpAddressTypeV6,
	
	ESuplIpAddressTypeLimit
	};

typedef TUint32 TSuplSetIdType;
enum _TSuplSetIdType
	{	
  	ESuplSetIdTypeMsisdn,
  	ESuplSetIdTypeMdn,
  	ESuplSetIdTypeMin,
  	ESuplSetIdTypeImsi,
  	ESuplSetIdTypeNai,
  	ESuplSetIdTypeIPAddress		
	};


/**
 * Supl Message IPAddress
 */
NONSHARABLE_CLASS(CSuplIpAddress) : public CBase
	{
public:
	IMPORT_C static CSuplIpAddress* NewL();
	IMPORT_C static CSuplIpAddress* NewLC();
	IMPORT_C ~CSuplIpAddress();
	IMPORT_C TBool operator==(const CSuplIpAddress& other) const;
	
private:
	CSuplIpAddress();
	void ConstructL();
		
public:	
	TSuplIpAddressType iIpAddressType;
	TBuf8<16> iIpAddress;	
	};

/**
 * Supl Message FQDN
 */
NONSHARABLE_CLASS(CSuplFqdn) : public CBase
	{
public:
	IMPORT_C static CSuplFqdn* NewL();
	IMPORT_C ~CSuplFqdn();
	IMPORT_C TBool operator==(const CSuplFqdn& other) const;
	
private:
	CSuplFqdn();
	void ConstructL();
		
public:	
	TBuf8<256> iFqdn;	
	};


/**
 * Supl Message Slp address
 */
NONSHARABLE_CLASS(CSuplSlpAddress) : public CBase
	{
public:
	IMPORT_C static CSuplSlpAddress* NewL();
	IMPORT_C ~CSuplSlpAddress();
	IMPORT_C TBool operator==(const CSuplSlpAddress& other) const;
	
private:
	CSuplSlpAddress();
	void ConstructL();

public:
	TSuplSlpAddressType iSlpAddressType;
	CSuplIpAddress* iIpAddress;	
	CSuplFqdn* iFqdn;
	};
	
/**
 * Supl Message SessionId
 */
NONSHARABLE_CLASS(CSuplSlpSessionId) : public CBase
	{
public:
	IMPORT_C static CSuplSlpSessionId* NewL();
	IMPORT_C ~CSuplSlpSessionId();
	IMPORT_C TBool operator==(const CSuplSlpSessionId& other) const;
	
private:	
	void ConstructL();
	void ConstructL(const CSuplSlpSessionId* aSlpSessionId);
	
public:
	TBuf8<4> iSessionId;
	CSuplSlpAddress* iSlpAddress;
	};

	
/**
 * Supl Message SetId
 */
NONSHARABLE_CLASS(CSuplSetId) : public CBase
	{
public:
	IMPORT_C static CSuplSetId* NewL();
	IMPORT_C ~CSuplSetId();
	IMPORT_C TBool operator==(const CSuplSetId& other) const;
	
private:	
	CSuplSetId();
	void ConstructL();
	
public:	
	TSuplSetIdType iSetIdType;
	CSuplIpAddress* iIpAddress;
	TBuf8<16> iSetId;
	};

/**
 * Supl Message SessionId
 */	
NONSHARABLE_CLASS(CSuplSetSessionId) : public CBase
	{
public:
	IMPORT_C static CSuplSetSessionId* NewL();
	IMPORT_C ~CSuplSetSessionId();
	IMPORT_C TBool operator==(const CSuplSetSessionId& other) const;
	
private:
	CSuplSetSessionId();
	void ConstructL();
	
public:
	TUint16 iSessionId;
	CSuplSetId* iSetId;
	};

/**
 * Supl Message SessionId
 */
NONSHARABLE_CLASS(CSuplSessionId) : public CBase
	{
public:
	IMPORT_C static CSuplSessionId* NewL();	
	IMPORT_C static CSuplSessionId* NewLC();	
	IMPORT_C ~CSuplSessionId();
	IMPORT_C TBool operator==(const CSuplSessionId& other) const;	
	
private:
	CSuplSessionId();
	void ConstructL();

public:
	TBool iSetSessionIdPresent;
	TBool iSlpSessionIdPresent;
	CSuplSetSessionId* iSetSessionId;
	CSuplSlpSessionId* iSlpSessionId;
	};


/**
 * Supl Message Version
 */
NONSHARABLE_CLASS(CSuplVersion) : public CBase
	{
public:
	IMPORT_C static CSuplVersion* NewL();
	IMPORT_C ~CSuplVersion();
	IMPORT_C TBool operator==(const CSuplVersion& other) const;
	
private:
	CSuplVersion();
	void ConstructL();
	
public:
	TInt iMaj;
	TInt iMin;
	TInt iServind;
	}; 

/**
 * NMR Element for GSM Cell Info
 */
NONSHARABLE_CLASS(TSuplGsmNmrElement)
	{
public:
	TInt iARFCN; // (0..1023)
	TInt iBSIC;  // (0..63)
	TInt iRxLev; // (0..63)
	};


/**
 * Supl GSM Cell Info
 */
NONSHARABLE_CLASS(CSuplGsmCellInfo) : public CBase
	{
public:
	IMPORT_C static CSuplGsmCellInfo* NewL();
	IMPORT_C ~CSuplGsmCellInfo();
	
private:
	CSuplGsmCellInfo();
	void ConstructL();
		
public:	
	TInt iRefMCC;        //  (0..999),   -- Mobile Country Code
	TInt iRefMNC;        //  (0..999),   -- Mobile Network Code
	TInt iRefLAC;        //  (0..65535), -- Location area code
	TInt iRefCI;         //  (0..65535), -- Cell identity
	TInt iNMR;     		 //  indicates number of NMR elements (0..15)
	TSuplGsmNmrElement iNmrElements[15]; // optional
	TInt iTA;            //  (0..255) OPTIONAL, --Timing Advance
	};


/**
 * Supl CDMA Cell Info
 */
NONSHARABLE_CLASS(CSuplCdmaCellInfo) : public CBase
	{
public:
	IMPORT_C static CSuplCdmaCellInfo* NewL();
	IMPORT_C ~CSuplCdmaCellInfo();
	
private:
	CSuplCdmaCellInfo();
	void ConstructL();
		
public:	
	TInt iRefNID; 		 // (0..65535),  -- Network Id
	TInt iRefSID;	 	 // (0..32767),  -- System Id
	TInt iRefBASEID;     // (0..65535),  -- Base Station Id
	TInt iRefBASELAT;    // (0..4194303),-- Base Station Latitude
	TInt iReBASELONG;    // (0..8388607),-- Base Station Longitude
	TInt iRefREFPN;      // (0..511),    -- Base Station PN Code
	TInt iRefWeekNumber; // (0..65535),  -- GPS Week Number
	TInt iRefSeconds;    // (0..4194303) -- GPS Seconds
	};

/**
 * Supl WCDMA Cell Info
 */
NONSHARABLE_CLASS(CSuplWcdmaCellInfo) : public CBase
	{
public:
	IMPORT_C static CSuplWcdmaCellInfo* NewL();
	IMPORT_C ~CSuplWcdmaCellInfo();
	
private:
	CSuplWcdmaCellInfo();
	void ConstructL();
		
public:	
	TInt iRefMCC;         // (0..999), -- Mobile Country Code
	TInt iRefMNC;         // (0..999), -- Mobile Network Code
	TInt iRefUC;          // (0..268435455), -- Cell identity
	// iRrequencyInfo          FrequencyInfo OPTIONAL,
	// TUint16 iPrimaryScramblingCode  // (0..511) OPTIONAL,
	// CSuplMeasureResultsList iMeasuredResultsList   // OPTIONAL,
	};

/**
 * Supl Location Type
 */
typedef TUint32 TSuplLocationType;
enum _TSuplLocationType
	{
	ESuplLocationTypeGsm,
	ESuplLocationTypeCdma,
	ESuplLocationTypeWcdma,
	ESuplLocationTypeLimit
	};
	
/**
 * Supl Location Status
 */
typedef TUint32 TSuplLocationStatus;
enum _TSuplLocationStatus 
	{
	ESuplLocStatusStale,
	ESuplLocStatusCurrent,
	ESuplLocStatusUnknown
	};

/**
 * Supl Location ID
 */
NONSHARABLE_CLASS(CSuplLocationId) : public CBase
	{
public:
	IMPORT_C static CSuplLocationId* NewL(TSuplLocationType aType);
	IMPORT_C ~CSuplLocationId();
	
private:
	CSuplLocationId(TSuplLocationType aType);
	void ConstructL();
		
public:	
	TSuplLocationType   iType;
	TSuplLocationStatus iStatus;

	CSuplGsmCellInfo*   iGsmCellInfo;
	CSuplCdmaCellInfo*  iCdmaCellInfo;
	CSuplWcdmaCellInfo* iWcdmaCellInfo;
	};


#endif // LBS_SUPL_MESSAGE_COMMON_H



