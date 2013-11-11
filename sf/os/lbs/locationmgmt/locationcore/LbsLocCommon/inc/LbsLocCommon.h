// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBSLOCCOMMON_H
#define LBSLOCCOMMON_H

#include <e32std.h>

class TLbsLocClassTypeBase
/** Base class for classes used to store Lbs information
@publishedPartner
@released
*/
	{
public:
	IMPORT_C TUint32 ClassType() const;
	IMPORT_C TUint ClassSize() const;

protected:
	IMPORT_C TLbsLocClassTypeBase();

protected:
	/** The type of the derived class */
	TUint32 iClassType;
	/** The size of the derived class */
	TUint iClassSize;
	};


/** Maximum length of the array used for TRequesterId.
@publishedPartner
@released
 */
const TInt KLbsMaxRequesterIdSize = 64;

/** Maximum length of the array used for TRequesterId2.
@publishedPartner
@released
 */
const TInt KLbsMaxRequesterIdSize2 = 255;

/** Maximum length of the array used for TClientName.
@publishedPartner
@released
 */
const TInt KLbsMaxClientNameSize = 128;

/** Maximum length of the array used for TClientName2.
@publishedPartner
@released
 */
const TInt KLbsMaxClientNameSize2 = 255;

/** Maximum length of the array used for TClientExternalId.
@publishedPartner
@released
 */
const TInt KLbsMaxClientExternalIdSize = 64;

/** Requester Id buffer.
@publishedPartner
@released
 */
typedef TBuf8<KLbsMaxRequesterIdSize> TLbsRequesterId;

/** Requester Id 2 buffer.
@publishedPartner
@released
*/
typedef TBuf16<KLbsMaxRequesterIdSize2> TLbsRequesterId2;

/** Client name buffer.
@publishedPartner
@released
*/
typedef TBuf8<KLbsMaxClientNameSize> TLbsClientName;

/** Client name 2 buffer.
@publishedPartner
@released
*/
typedef TBuf16<KLbsMaxClientNameSize2> TLbsClientName2;

/** Client external Id buffer.
@publishedPartner
@released
*/
typedef TBuf8<KLbsMaxClientExternalIdSize> TLbsClientExternalId;


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
/** Maximum length of the internal Requester buffer extension .
@internalComponent
@released
*/
const TInt KLbsMaxRequesterIdSizeExt2 = KLbsMaxRequesterIdSize2 - KLbsMaxRequesterIdSize/2;
   
/** Maximum length of the internal Client Name buffer extension
@internalComponent
@released
*/
const TInt KLbsMaxClientNameSizeExt2 = KLbsMaxClientNameSize2 - KLbsMaxClientNameSize/2;
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

/**
Contains information about the external location requester
when a location request is received from the network (MT-LR).
 
@publishedPartner
@released
*/
class TLbsExternalRequestInfo : public TLbsLocClassTypeBase
	{
public:
		/** Defined type for TRequestType 
		@released */
		typedef TUint32 TRequestType;
        /** Type of request
 		Note that the TRequestType enum may be extended in the future by adding
 		more enumerated values. To maintain compatibility any unrecognized values 
 		must be handled as ERequestTypeUnknown. 
 		@released */
        enum _TRequestType
		{
		/** Unknown. Should be used to initialize variables */
		ERequestTypeUnknown = 0,
		/** The privacy request is related to a single location request */
		ERequestSingleShot  = 1,
		/** The privacy request is related to a periodic location request */
		ERequestPeriodic    = 2,
		/** The privacy request is related to an area event location request */
		ERequestAreaEvent   = 3,
		/** The privacy request is related to a single location request
		 and is advised to be handled silently (no notification or verification)*/
		ERequestSingleShotSilent    = 4,
		/** The privacy request is related to a periodic location request
		 and is advised to be handled silently (no notification or verification)*/
		ERequestPeriodicSilent      = 5,
		/** The privacy request is related to an area event location request
		 and it is advised to be handled silently (no notification or verification)*/
		ERequestAreaEventSilent     = 6,
		
		ERequestTypeMaxEnum = 7
		};


		/** Defined type for TRequestSource
 		@released */
		typedef TUint32 TRequestSource;
        /** Source of request
 		Note that the TRequestSource enum may be extended in the future by adding
 		more enumerated values. To maintain compatibility any unrecognized values 
 		must be handled as ERequestSourceUnknown.
 		@released */
        enum _TRequestSource
            {
            /** Unknown. Should be used to initialize variables */
            ERequestSourceUnknown	= 0,
            /** The privacy request originates from a local application.
            @capability Location The capability is checked when CLbsPrivacyRequest::NewPrivacyRequest() or CLbsPrivacyRequest::RepeatPrivacyRequest() is called.
            @ref CLbsPrivacyRequest::NewPrivacyRequest()
            @ref CLbsPrivacyRequest::RepeatPrivacyRequest()
            @capability ReadDeviceData The capability is checked when CLbsPrivacyRequest::NewPrivacyRequest() or CLbsPrivacyRequest::RepeatPrivacyRequest() is called.
            @ref CLbsPrivacyRequest::NewPrivacyRequest()
            @ref CLbsPrivacyRequest::RepeatPrivacyRequest()
            */
            ERequestSourceLocal		= 1,
            /** The privacy request originates from a network.
            To get more information about a network @see TNetworkType
            @capability NetworkServices The capability is checked when CLbsPrivacyRequest::NewPrivacyRequest() or CLbsPrivacyRequest::RepeatPrivacyRequest() is called.
            @ref CLbsPrivacyRequest::NewPrivacyRequest()
            @ref CLbsPrivacyRequest::RepeatPrivacyRequest()
            */
            ERequestSourceNetwork	= 2
            };


		/** Defined type for TNetworkType
		@released  */
		typedef TUint32 TNetworkType;
		/** Network type 
 		Note that the TNetworkType enum may be extended in the future by adding
 		more enumerated values. To maintain compatibility any unrecognized values 
 		must be handled as ENetworkTypeUnknown. 
 		@released */
        enum _TNetworkType
            {
            /** Unknown network. Should be used to initialize variables */
            ENetworkTypeUnknown = 0,
            /** A request received over GSM */
            ENetworkGSM  		= 1,
            /** A request received over SUPL*/
            ENetworkSUPL    	= 2,
            /** A request received over WCDMA*/
            ENetworkWCDMA		= 3
            };


		/** Defined type for TFormatIndicator 
		@released */
		typedef TUint32 TFormatIndicator;
        /**
        This enum specifies how a requestor or an 
        LCS client or requestor can be identified.
 		Note that the TFormatIndicator enum may be extended in the future by adding
 		more enumerated values. To maintain compatibility any unrecognized values 
 		must be handled as EFormatUnknown. 
 		@released */
        enum _TFormatIndicator
            {
            /** Unknown. Should be used to initialize variables */
            EFormatUnknown			 = 0,
  			/** Logical name. */
            EFormatLogicalName		 = 1,
     		/** Email address. */
            EFormatEmailAddress		 = 2,
			/** Mobile subscriber phone number. */
            EFormatMSISDN			 = 3,
			/** URL. */
            EFormatURL				 = 4,
            /** URL of user in a SIP session. */
            EFormatSIPURL			 = 5,
			/** User Identifier in an IP Multimedia Service session. */
            EFormatIMSPublicIdentity = 6,
			/** Mobile Identification Number. */
            EFormatMIN               = 7,
			/** Mobile Directory Number. */
            EFormatMDN               = 8,
   			/** Application UID. 
   			The identifier represents a 32-bit value (@see TUid) 
   			and is stored as a hexadecimal string. e.g."0x12345678" */
            EFormatAppUID    		 = 9
            };		

            
		/** Defined type for TCodingScheme 
		@released */
		typedef TUint32 TCodingScheme;

        /**
        This enum specifies string encoding type.
 		Note that the TCodingScheme enum may be extended in the future by adding
 		more enumerated values. To maintain compatibility any unrecognized values 
 		must be handled as ECodingSchemeUnknown. 
 		@released */
        enum _TCodingScheme
            {
            /** Unknown. Should be used to initialize variables */
            ECodingSchemeUnknown	 = 0,
  			/** 2-byte Universal Character Set */
            ECodingSchemeUCS2		 = 1,
     		/** 7-bit default alphabet and the SMS 
     		packaging specified in [3GPP 23.038] */
            ECodingSchemeGSMDefault	 = 2,
			/** 8-bit UCS/Unicode Transformation Format */
            ECodingSchemeUTF8		 = 3
            };

public:
	IMPORT_C TLbsExternalRequestInfo();
	
	IMPORT_C void GetRequesterId(TDes8& aRequesterId) const;
	IMPORT_C void GetClientName(TDes8& aClientName) const;
	IMPORT_C void GetClientExternalId(TDes8& aClientExternalId) const;

	IMPORT_C void GetRequesterId(TDes16& aRequesterId) const;
	IMPORT_C void GetClientName(TDes16& aClientName) const;
	
	IMPORT_C void SetRequesterId(const TDesC8& aRequesterId);
	IMPORT_C void SetClientName(const TDesC8& aClientName);
	IMPORT_C void SetClientExternalId(const TDesC8& aClientExternalId);

	IMPORT_C void SetRequesterId(const TDesC16& aRequesterId);
	IMPORT_C void SetClientName(const TDesC16& aClientName);
	
	IMPORT_C void SetRequestType(const TRequestType aRequestType);
	IMPORT_C TRequestType RequestType() const;

	IMPORT_C void SetRequestSource(const TRequestSource aRequestSource);
	IMPORT_C TRequestSource RequestSource() const;

	IMPORT_C void SetNetworkType(const TNetworkType aNetworkType);
	IMPORT_C TNetworkType NetworkType() const;

	IMPORT_C void SetRequesterIdFormat(const TFormatIndicator aFormat);
	IMPORT_C TFormatIndicator RequesterIdFormat() const;

	IMPORT_C void SetClientNameFormat(const TFormatIndicator aFormat);
	IMPORT_C TFormatIndicator ClientNameFormat() const;

	IMPORT_C void SetRequesterIdCodingScheme(const TCodingScheme aCoding);
	IMPORT_C TCodingScheme RequesterIdCodingScheme() const;

	IMPORT_C void SetClientNameCodingScheme(const TCodingScheme aCoding);
	IMPORT_C TCodingScheme ClientNameCodingScheme() const;

private:	
	/** Requestor Id. */
	TLbsRequesterId iRequesterId;
	/** Client name. */
	TLbsClientName iClientName;
	/** Client external Id. */
	TLbsClientExternalId iClientExternalId;	
	
	/** Request type */
	TInt8 iRequestType;

	/** Request source */
	TInt8 iRequestSource;

	/** Network type */
	TInt8 iNetworkType;
	
	/** Requester format */
	TInt8 iRequesterFormat;
	/** Client format */
	TInt8 iClientFormat;
	
	/** Requester coding */	
	TUint8 iRequesterCoding;
	/** Client coding */	
	TUint8 iClientCoding;
		
	/** Reserved for future use to help maintain BC. */
	TUint8 iReserved[9];
	};


/**
This class contains information about the external location requester
when a location request is received from the network (MT-LR).

This class extends the TLbsExternalRequestInfo class to allow 
for longer Requester Id and Client Name strings.

It is backward compatible with the TLbsExternalRequestInfo
class. Applications that can handle only the TLbsExternalRequestInfo
class, may still access the information, but the length of the strings
is limitted by the size of the buffers defined in the TLbsExternalRequestInfo
class.

Unlike in the TLbsExternalRequestInfo class, only unicode (UCS2) strings
are supported.

Because of a significant size it should be considered to store objects defined 
by this class on heap rather than on stack.

@publishedPartner
@released

The length of the buffers defined in the TLbsExternalRequestInfo
class is enough to store the Requestor Id and the LCS Client Name strings used
by the RRC, RRLP and SUPL protocols.
Use the TLbsExternalRequestInfo class instead.
*/
class TLbsExternalRequestInfo2 : public TLbsExternalRequestInfo
	{
public:
	IMPORT_C TLbsExternalRequestInfo2();
	
	IMPORT_C void SetRequesterId(const TDesC16& aRequesterId);
	IMPORT_C void GetRequesterId(TDes16& aRequesterId) const;
	
	IMPORT_C void SetClientName(const TDesC16& aClientName);
	IMPORT_C void GetClientName(TDes16& aClientName) const;

protected:
	IMPORT_C void SetRequesterIdCodingScheme(const TCodingScheme aCoding);
	IMPORT_C TCodingScheme RequesterIdCodingScheme() const;

	IMPORT_C void SetClientNameCodingScheme(const TCodingScheme aCoding);
	IMPORT_C TCodingScheme ClientNameCodingScheme() const;

private:	

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
	TBuf16<KLbsMaxRequesterIdSizeExt2> iRequesterIdExt;
	TBuf16<KLbsMaxClientNameSizeExt2>  iClientNameExt;
#else
	TBuf16<KLbsMaxRequesterIdSize2 - KLbsMaxRequesterIdSize/2> iRequesterIdExt;
	TBuf16<KLbsMaxClientNameSize2 - KLbsMaxClientNameSize/2>  iClientNameExt;
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

	TUint8 iReserved2[32];
	};

#endif // LBSLOCCOMMON_H
