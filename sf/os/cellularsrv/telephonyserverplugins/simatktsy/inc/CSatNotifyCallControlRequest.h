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
* Name        : CSatNotifyCallControlRequest.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Specific notifications class
* Version     : 1.0
*
*/



#ifndef CSATNOTIFYCALLCONTROLREQUEST_H
#define CSATNOTIFYCALLCONTROLREQUEST_H


//  INCLUDES
#include <etelsat.h>
#include "MSatNotificationsBase.h"

// FORWARD DECLARATIONS
class CSatDataPackage;
class CSatNotificationsTsy;

/**
* DESCRIPTION
* SAT Call Control notification class.
* Created when client requests to be notified if a request (of any other 
* existing client) has been modified by call control.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatNotifyCallControlRequest ) : public CBase,
                           public MSatNotificationsBase
    {
    public:   // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatNotifyCallControlRequest*: created object 
        */
        static CSatNotifyCallControlRequest* NewL( 
        	CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatNotifyCallControlRequest();

	private:
        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        */
        CSatNotifyCallControlRequest( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL. 
        */
        void ConstructL();

    public: // Functions from base classes

        /**
        * Call Control Request Notification request received from client
        * @param aTsyReqHandle request handle from ETel
        * @param aPackage: Packed data
        * @return KErrNone
        */
        TInt Notify( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
        * Cancels notification request for Call Control Request.
        * @param aReqHandle notify request handle
        * @return KErrNone
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );
        
        /**
        * Unpacks and passes Alpha Id for notification completion method
        * @param aDataPackage: Packaged return data
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        TInt CompleteNotifyL( CSatDataPackage* aDataPackage, TInt aErrorCode );
                 
        /**
        * Completes Call Control Notification Request message to client
        * @param aAlphaId: Reference to the Alpha ID
        * @param aResult: Result of the call control request
        * @param aErrorCode: Possible error code
        * @return KErrNone/ KErrCorrupt
        */
        void CompleteAlphaId( RSat::TAlphaIdBuf& aAlphaId, 
            RSat::TControlResult aResult, TInt aErrorCode  );

    private: // Data

        // Notify Call Control struct. Package allocated
        // in this class and received from ETel
        RSat::TCallControlV2Pckg* iCallControlV2Pckg;
        // Requested response format
        TUint8 iRequestedRspFormat;
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
    };


#endif // CSATNOTIFYCALLCONTROLREQUEST_H