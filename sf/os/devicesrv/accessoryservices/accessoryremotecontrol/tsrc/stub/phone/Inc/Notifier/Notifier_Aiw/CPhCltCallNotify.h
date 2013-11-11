/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CPhCltCallNotify observer getting notifications about call requests
*                from Phone Server.
*
*/

#ifndef CPHCLTCALLNOTIFY_H
#define CPHCLTCALLNOTIFY_H

//  INCLUDES
#include <e32base.h>
#include <PhCltTypes.h>
#include "RPhCltCallNotify.h"

//  FORWARD DECLARATIONS
class RPhCltServer;
class CPhCltExtPhoneDialData;

// CLASS DECLARATION

/**
*  CCallNotify observer getting notifications about call requests
*  from Phone Server.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class CPhCltCallNotify : public CActive
    {
    public: // Constructors and destructors.

        
        IMPORT_C static CPhCltCallNotify* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhCltCallNotify();
        
    public: // New functions

        /**
        * Open subsession to server.
        * 
        * @param aServer Pre-opened server session.
        * @return Symbian OS error code of the success of the opening.
        */
        IMPORT_C TInt Open( RPhCltServer& aServer );

        /**
        * Close subsession.
        */
        IMPORT_C void Close();
    
        /**
        * Notifies when a call request arrives.
        *
        * @param aStatus Status to be changed when call request have arrived.
        * @param aCallArgs Call argument storage place.
        */
        IMPORT_C void NotifyCallAttempt( 
            TRequestStatus& aStatus, 
			CPhCltExtPhoneDialData& aCallArgs );
        /**
        * Cancel pending call attempt notification.
        */
        IMPORT_C void CancelNotifyCallAttempt() const;
        
        /**
        * Respond to client request to make a call
        *
        * @param aResultCode Result of the response.
        */
        IMPORT_C TInt RespondClient( const TInt aResultCode );
        
        /**
        * Notifies when a emergency call request arrives.
        *
        * @param aStatus Status to be changed when call request have arrived.
        */        
        IMPORT_C void NotifyEmergencyCall( 
            TRequestStatus& aStatus,
            TDesC& aEmergencyNumber );

        /**
        * Cancel pending emergency call attempt notification.
        */
		IMPORT_C void CancelNotifyEmergencyCall() const;

        /**
        * Respond to client request to make emergency call
        *
        * @param aResultCode Result of the response.
        */
		IMPORT_C TInt RespondEmergencyToClient( const TInt aResultCode );
        
        /**
        * Notifies when a Command handler request arrives.
        *
        * @param aStatus Status to be changed when command handler request
        *                has arrived.
        * @param aCommandArgs Command argument storage place. 
        *                     That is, packaged version of class 
        *                     TPhCltComHandCommandParameters.
        */
        IMPORT_C void NotifyComHandCommand( 
            TRequestStatus& aStatus, 
            TDes8& aCommandArgs );
    
        /**
        * Cancel pending command handler request notification.
        */
        IMPORT_C void CancelNotifyComHandCommand() const;
    
        /**
        * Respond to client's command handler request.
        *
        * @param aResultCode Result of the response 
        *        (previous function execution).
        * @return KErrNone if respond succeeded, otherwise Symbian error code.
        */
        IMPORT_C TInt RespondComHandClient( const TInt aResultCode );

    
    private:
    
        /*
        * Constructor.
        */
        CPhCltCallNotify();
        
        
    private: // Functions from base classes
        
        /**
        * From CActive. 
        * Called when send request is completed.
        */
        void RunL();
        
        /**
        * From CActive. 
        * Called when active request is canceled.
        */
        void DoCancel();
        
    private:
        
        //RClass instance
        RPhCltCallNotify iCallNotify;
        
        //Pointer Phone Dial Data
        CPhCltExtPhoneDialData* iDialData;
        
        //This saves Phone request status 
        TRequestStatus* iRequestStatus;
        
        //Used to get dial data length from server
        TInt iDialDataLength;
        
        //Package buffer for dial data length 
        TPckg< TInt > iDataLengthPckg;

		//Pointer to emergencynumber
		TPtr8 iEmergencyCallNumberPtr;
    
    };


#endif    // CPHCLTCALLNOTIFY_H

// End of File
