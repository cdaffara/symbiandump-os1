/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer getting notifications about 
*                command handler requests from Phone Server.
*
*/

#ifndef RPHCLTCOMMANDHANDLERNOTIFY_H
#define RPHCLTCOMMANDHANDLERNOTIFY_H

//  INCLUDES
#include <e32base.h>


//  FORWARD DECLARATIONS
class RPhCltServer;


// CLASS DECLARATION

/**
*  Observer getting notifications about command handler requests
*  from Phone Server.
*
*  @lib PhoneClient.lib
*  @since 3.2
*/
class RPhCltCommandHandlerNotify 
:   public RSubSessionBase
    {
    public: // Constructors and destructors.

        /**
         * Constructor.
         */
        RPhCltCommandHandlerNotify();


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
        TInt RespondClient( const TInt aResultCode );


    private:

        // Prohibit copy constructor if not deriving from CBase.
        RPhCltCommandHandlerNotify( const RPhCltCommandHandlerNotify& );

        // Prohibit assigment operator if not deriving from CBase.
        RPhCltCommandHandlerNotify& operator=( 
            const RPhCltCommandHandlerNotify& );

    };

#endif    // RPHCLTCOMMANDHANDLERNOTIFY_H

// End of File
