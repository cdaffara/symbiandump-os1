/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef CPOSREQUESTOR_H
#define CPOSREQUESTOR_H

#include <e32base.h>


/**
The base class for a requester in the Privacy Data Types.
A requester is either a contact or a service and it is identified by a
string. The string may be a phone number, URL, email address, etc.

@see CPosContactRequestor
@see CPosServiceRequestor
@publishedPartner
@deprecated
@capabilities None
*/
class CPosRequestor : public CBase
    {
    public:
		/** Defined type for TNetworkType
		@publishedPartner
		@released
		*/
		typedef TUint32 TNetworkType;
		/** Network type 
 		Note that the TNetworkType enum may be extended in the future by adding
 		more enumerated values. To maintain compatibility any unrecognized values 
 		must be handled as ENetworkTypeUnknown.
 		@publishedPartner
 		@released
 		*/
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

		/** Defined type for TRequestType
		@publishedPartner
		@released
		*/
		typedef TUint32 TRequestType;
        /** Type of request
 		Note that the TRequestType enum may be extended in the future by adding
 		more enumerated values. To maintain compatibility any unrecognized values 
 		must be handled as ERequestTypeUnknown.
 		@publishedPartner
 		@released
 		*/
        enum _TRequestType
            {
            /** Unknown. Should be used to initialize variables */
            ERequestTypeUnknown = 0,
            /** The privacy request is related to a single location request */
            ERequestSingleShot  = 1,
            /** The privacy request is related to a periodic location request */
            ERequestPeriodic    = 2,
            /** The privacy request is related to an area event location request */
            ERequestAreaEvent   = 3
            };

        /**
        This enum specifies whether the requestor is a contact or a service.
        */
        enum TRequestorType
            {
            /** The requestor is a service. */
            ERequestorService,
   			/** The requestor is a contact. */
            ERequestorContact
            };

        /**
        This enum specifies how a requestor is identified.
        */
        enum TRequestorIdFormat
            {
			/** The requestor ID format is unknown. */
            EIdFormatUnknown,
            /** The requestor is identified by name. */
            EIdFormatGenericName,
            /** The requestor is identified by phone number. */
            EIdFormatPhoneNumber,
			/** The requestor is identified by URL. */
            EIdFormatUrl,
			/** The requestor is identified by email address. */
            EIdFormatEmail,
   			/** The requestor is identified by the URL of the SIP session. */
            EIdFormatSIPUrl,
   			/** The requestor is identified by the IMS public identity. */
            EIdFormatIMSPublicIdentity,
   			/** The requestor is identified by Mobile Indentification Number. */
            EIdFormatMIN,
   			/** The requestor is identified by Mobile Directory Number. */
            EIdFormatMDN
            };
            
    public:
        IMPORT_C virtual ~CPosRequestor();

    public:
		IMPORT_C void SetRequestType(const TRequestType aRequestType);
		IMPORT_C TRequestType RequestType() const;

		IMPORT_C void SetNetworkType(const TNetworkType aNetworkType);
		IMPORT_C TNetworkType NetworkType() const;

        IMPORT_C void SetRequestorIdL(TRequestorIdFormat aIdFormat, const TDesC& aIdString);
        IMPORT_C TRequestorType RequestorType() const;
        IMPORT_C TRequestorIdFormat RequestorIdFormat() const;
        IMPORT_C TPtrC RequestorIdString() const;

    protected:
        CPosRequestor(TRequestorType aRequestorType,
        			  TRequestorIdFormat aIdFormat,
        			  HBufC* aIdString,
        			  TNetworkType aNetworkType,
        			  TRequestType aRequestType);

    private:
        // By default, prohibit copy constructor
        CPosRequestor(const CPosRequestor&);
        // Prohibit assigment operator
        CPosRequestor& operator= (const CPosRequestor&);

    private:
        TRequestorType		iRequestorType;
        TRequestorIdFormat	iIdFormat;
        HBufC* 				iIdString;
        TNetworkType		iNetworkType;
        TRequestType		iRequestType;
    };

#endif      // CPOSREQUESTOR_H
