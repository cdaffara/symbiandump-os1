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




/**
 @file
 @internalComponent
 @released
*/

#ifndef CPOSPRIVMANAGER_H
#define CPOSPRIVMANAGER_H

//  INCLUDES
#include <e32base.h>
#include "EPos_PrivacyInternal.h"
#include <lbs/lbsloccommon.h>

// DATA TYPES
enum TPosVerActivity
    {
    EPosInactive,
    EPosResolvingForVerification,
    EPosResolvingForNotification,
    EPosVerifying,
    EPosCancelBeforeDialog,
    EPosCancelDuringDialog,
    EPosCancelAfterDialog1,
    EPosCancelAfterDialog2
    };

// FORWARD DECLARATIONS
class CPosDialogCtrl;

// CLASS DECLARATION

/**
*  Handles all requests to the privacy server.
*/
class CPosPrivManager : public CActive
    {
    public:  // Constructors and destructor

        enum TEPosPrivacyResult
            {
            EVerifyReject,
            EVerifyAccept,
            EVerifyAskUser
            };

        /**
        * Two-phased constructor.
        * @return A new instance of this class.
        */
        static CPosPrivManager* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPrivManager();

    public: // New functions

        /**
        * Performs verification.
        * @param aMessage The verification request.
        * @param aRequestorStack The involved contacts and services.
        * @param aRequestData Contains data for verification requests.
        * @param aRequestId The request id.
        */
        void VerifyL(
        /* IN  */   TRequestStatus&           aRequestStatus,
        /* IN */	const TLbsExternalRequestInfo& aRequestInfo,
        /* IN  */   const TPosRequestData&    aRequestData,
        /* IN  */   TInt                      aRequestId
        );

        /**
        * Cancels verification.
        */
        void CancelVerify();

        /**
        * Cancels verification.
        * @param aCancelReason A cancel reason.
        * @param aMessage The cancel request.
        */
        void CancelVerifyL(
        /* IN  */   TPosVerifyCancelReason aCancelReason
        );

        /**
        * Performs notification
        * @param aMessage The notification request.
        * @param aRequestorStack The involved contacts and services.
        * @param aRequestData Contains data for notification requests.
        * @param aRequestId The request id associated with this notification.
        */
        void NotifyL(
        /* IN  */   TRequestStatus& aRequestStatus,
        /* IN */	const TLbsExternalRequestInfo& aRequestInfo,
        /* IN  */   const TPosRequestData& aRequestData,
        /* IN  */   TInt aRequestId
        );

    protected: // Functions from base classes

        /**
        * From CActive Handles asynchronous request completion.
        */
        void RunL();

        /**
        * From CActive Cancels any outstanding request.
        */
        void DoCancel();

        /**
        * From CActive Handles a leave in RunL.
        * @param aError The RunL leave code.
        * @return Always KErrNone, because all errors are handled
        */
        TInt RunError(
        /* IN  */   TInt aError
        );

    private:

        /**
        * C++ default constructor.
        */
        CPosPrivManager();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CPosPrivManager( const CPosPrivManager& );
        // Prohibit assigment operator
        CPosPrivManager& operator= ( const CPosPrivManager& );

    private: // New functions

        void ResolveL();

        void CompleteRequestL(TInt aCompleteCode);

        void NotifyPluginL();

        TBool IsVerifying() const;

    private:    // Data

        CPosDialogCtrl*                 iDlgCtrl;

		TRequestStatus*					iRequestStatus;
        TLbsExternalRequestInfo*		iRequestInfo;
        TPosVerActivity                 iActivity;

        TInt                            iRequestId;

        TPosRequestData                 iRequestData;
        TInt                            iUserResponse;

        TUint                           iLastVerificationResult;

    };

#endif      // CPOSPRIVMANAGER_H

// End of File
