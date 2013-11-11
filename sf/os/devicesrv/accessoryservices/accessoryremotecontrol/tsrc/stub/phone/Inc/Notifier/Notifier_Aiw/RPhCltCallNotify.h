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


//  FORWARD DECLARATIONS
class RPhCltServer;
class CPhCltExtPhoneDialData;

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
        RPhCltCallNotify();


    public: // New functions

        /**
        * Open subsession to server.
        * 
        * @param aServer Pre-opened server session.
        * @return Symbian OS error code of the success of the opening.
        */
        TInt Open( RPhCltServer& aServer );

        /**
        * Close subsession.
        */
        void Close();
    
        /**
        * Notifies when a call request arrives.
        *
        * @param aStatus Status to be changed when call request have arrived.
        * @param aDatalength  Dial data length.
        */
        void NotifyCallAttempt( 
            TRequestStatus& aStatus, 
		   TPckg< TInt >& aDataLength );
        /**
        * Cancel pending call attempt notification.
        */
        void CancelNotifyCallAttempt() const;
        
        /**
        * Respond to client request to make a call
        *
        * @param aResultCode Result of the response.
        */
        TInt RespondClient( const TInt aResultCode );
        
        void GetDialData( TDes8& aData );
        
        /**
        * Notifies when a emergency call request arrives.
        *
        * @param aStatus Status to be changed when call request have arrived.
        * @param aEmergencyNumber 
        *
        */
        void NotifyEmergencyCall( 
            TRequestStatus& aStatus,
            TDes8& aEmergencyNumber
			 );
        /**
        * Cancel pending call attempt notification.
        */
        void CancelNotifyEmergencyCall() const;
        
        /**
        * Respond to client request to make a emergency call
        *
        * @param aResultCode Result of the response.
        * @return Symbian OS error code of the success of the opening.
        */
        TInt RespondEmergencyToClient( const TInt aResultCode ); 
        
        /**
        * Notifies when a Command handler request arrives.
        *
        * @param aStatus Status to be changed when command handler request
        *                has arrived.
        * @param aCommandArgs Command argument storage place. 
        *                     That is, packaged version of class 
        *                     TPhCltComHandCommandParameters.
        */
        void NotifyComHandCommand( 
            TRequestStatus& aStatus, 
            TDes8& aCommandArgs );

        /**
        * Cancel pending command handler request notification.
        */
        void CancelNotifyComHandCommand() const;
        
        /**
        * Respond to client's command handler request.
        *
        * @param aResultCode Result of the response 
        *        (previous function execution).
        * @return KErrNone if respond succeeded, otherwise Symbian error code.
        */
        TInt RespondComHandClient( const TInt aResultCode );
 
    };


#endif    // RPHCLTCALLNOTIFY_H

// End of File
