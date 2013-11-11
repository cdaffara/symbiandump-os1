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



#ifndef CPOSSUPLPRIVACYREQUESTINFO_H
#define CPOSSUPLPRIVACYREQUESTINFO_H

#include <e32base.h>
#include <lbs/epos_cposnetworkprivacyrequestinfo.h>


/**
Represents SUPL privacy request information.

This includes information about the LCS client and an ID of the requesting
entity (requestor).

@see CPosNetworkPrivacy
@see CPosNetworkPrivacyRequestInfo

@publishedPartner
@released
@capability None
*/
class CPosSUPLPrivacyRequestInfo : public CPosNetworkPrivacyRequestInfo
    {
    public:
        /**
        This enum specifies how a requestor or an 
        LCS client or requestor can be identified.
        */
        enum TIdType
            {
  			/** Logical name. */
            EIdTypeLogicalName = 0,
			/** Mobile subscriber phone number. */
            EIdTypeMSISDN,
     		/** Email address. */
            EIdTypeEmailAddress,
			/** URL. */
            EIdTypeURL,
            /** URL of user in a SIP session. */
            EIdTypeSIPURL,
			/** User Identifier in an IP Multimedia Service session. */
            EIdTypeIMSPublicIdentity,
			/** Mobile Identification Number. */
            EIdTypeMIN,
			/** Mobile Directory Number. */
            EIdTypeMDN
            };

    public:
        IMPORT_C static CPosSUPLPrivacyRequestInfo* NewLC();
        IMPORT_C static CPosSUPLPrivacyRequestInfo* NewL();

        virtual ~CPosSUPLPrivacyRequestInfo();

    public:
        IMPORT_C void SetLCSClientL(const TDesC& aLCSClientId, TIdType aIdType);
        IMPORT_C void SetRequestorL(const TDesC& aRequestorId, TIdType aIdType);
        IMPORT_C TInt GetLCSClient(TPtrC& aLCSClientId, TIdType& aIdType) const;
        IMPORT_C TInt GetRequestor(TPtrC& aRequestorId, TIdType& aIdType) const;

    private:
        CPosSUPLPrivacyRequestInfo();
        void ConstructL();

        // By default, prohibit copy constructor
        CPosSUPLPrivacyRequestInfo(const CPosSUPLPrivacyRequestInfo&);
        // Prohibit assigment operator
        CPosSUPLPrivacyRequestInfo& operator= (const CPosSUPLPrivacyRequestInfo&);

    private:
        HBufC* iLCSClientId;
        TIdType iLCSClientIdType;
        HBufC* iRequestorId;
        TIdType iRequestorIdType;
    };

#endif      // CPOSSUPLPRIVACYREQUESTINFO_H
