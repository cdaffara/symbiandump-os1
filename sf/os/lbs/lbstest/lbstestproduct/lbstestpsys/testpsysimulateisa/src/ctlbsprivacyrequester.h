/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_PRIVACY_REQUESTER_H__
#define __CT_LBS_PRIVACY_REQUESTER_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPositionInfo;
class RPrivacyServer;

// CLASS DECLARATION

/**
*  Makes requests to the privacy server.
*/
class CT_LbsPrivacyRequester :public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsPrivacyRequester* NewL();

        /**
        * Destructor.
        */
        virtual ~CT_LbsPrivacyRequester();

    public: // New functions

        void VerifyAndGetPosition(
		/* IN  */		TRequestStatus&     aStatus,
		/* IN  */		TInt                aQueryId,
		/* OUT */		CPositionInfo*&     aPosition
        );

    private: // Functions from base classes

        /**
        * From CActive
        * Called when an event has occured.
        */
        void RunL();

        /**
        * From CActive
        * Called by the Cancel method in the base class.
        */
        void DoCancel();

    protected:

        /**
        * C++ default constructor.
        */
        CT_LbsPrivacyRequester();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

    private:

        // By default, prohibit copy constructor
        CT_LbsPrivacyRequester( const CT_LbsPrivacyRequester& );
        // Prohibit assigment operator
        CT_LbsPrivacyRequester& operator= ( const CT_LbsPrivacyRequester& );

    private:    // Data
        RPrivacyServer* iPrivacyServer;
        TRequestStatus* iPositionStatus;
        CPositionInfo** iPosition;

    };

#endif      // __CT_LBS_PRIVACY_REQUESTER_H__

// End of File
