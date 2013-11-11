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
* Description:  RCallNotify observer getting notifications about call requests
*                from Phone Server.
*
*/

#ifndef RPHCLTCALLNOTIFY_H
#define RPHCLTCALLNOTIFY_H

//  INCLUDES
#include <e32base.h>
#include <PhCltTypes.h>


//  FORWARD DECLARATIONS
class RPhCltServer;


// CLASS DECLARATION

/**
*  RCallNotify observer getting notifications about call requests
*  from Phone Server.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class RPhCltCallNotify : public RSubSessionBase
    {
    public: // Constructors and destructors.

        /**
         * Constructor.
         */
        IMPORT_C RPhCltCallNotify();


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
            TDes8& aCallArgs );

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
        
        // Stubbed for BC - DO NOT CALL
        IMPORT_C void NotifyPhoneStatusRequest( TRequestStatus& aStatus );
        IMPORT_C void CancelNotifyPhoneStatusRequest() const;
        IMPORT_C void ReportPhoneStatus( 
            TRequestStatus& aStatus, 
            const TInt aPhoneStatus );
        IMPORT_C void CancelReportPhoneStatus() const;
    };


#endif    // RPHCLTCALLNOTIFY_H

// End of File
