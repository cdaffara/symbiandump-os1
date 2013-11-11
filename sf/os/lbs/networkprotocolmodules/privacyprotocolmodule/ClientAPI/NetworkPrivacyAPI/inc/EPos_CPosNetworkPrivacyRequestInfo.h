/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSNETWORKPRIVACYREQUESTINFO_H
#define CPOSNETWORKPRIVACYREQUESTINFO_H

#include <e32base.h>


/**
Abstract base class for privacy request information which is passed to
CPosNetworkPrivacy methods.

This class contains no exported methods. Clients must use a subclass.

@publishedPartner
@released
@capability None
*/
class CPosNetworkPrivacyRequestInfo : public CBase
    {
    public:
		/** Defined type for TRequestType 
		@released */
		typedef TUint32 TRequestType;
        /** The type of privacy request.
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

    public:
        virtual ~CPosNetworkPrivacyRequestInfo();
        
    public:
        IMPORT_C void SetRequestType(const TRequestType aRequestType);
        IMPORT_C TRequestType RequestType() const;

    public:
    	/**
    	Returns information of which subclass the instance is.
    	This function is only used internally and is therefore not exported.

    	@return Subclass type.
    	*/
        TUint RequestInfoType() const;

    protected:
    	/**
    	Default constructor
    	*/
        CPosNetworkPrivacyRequestInfo(TUint aRequestInfoType);

    private:
        // By default, prohibit copy constructor
        CPosNetworkPrivacyRequestInfo(
            const CPosNetworkPrivacyRequestInfo& );
        // Prohibit assigment operator
        CPosNetworkPrivacyRequestInfo& operator= (
            const CPosNetworkPrivacyRequestInfo& );

    private:
        TUint iRequestInfoType;
        TRequestType iRequestType;
    };

#endif      // CPOSNETWORKPRIVACYREQUESTINFO_H
