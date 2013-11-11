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
* Description:  USSD subsession.
*
*/


#ifndef RPHCLTUSSD_H
#define RPHCLTUSSD_H

// INCLUDES
#include <e32std.h>
#include <PhCltTypes.h>
#include "CPhCltUssd.h"

// FORWARD DECLARATIONS
class RPhCltServer;


// CLASS DECLARATION

/**
*  USSD subsession.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class RPhCltUssd : public RSubSessionBase
    {

    public: // New functions        

        /**
        * Constructor.
        */      
        RPhCltUssd();

        /**
        * Creates a subsession with the phone server.
        * 
        * @param aServer Pre-opened PhCltServer session.
        * @return Symbian OS standard error code.
        */      
        TInt Open( RPhCltServer& aServer );

        /**
        * Closes the session. Cancels all notify requests.
        * Remember to call this to a connected session in some point.
        */
        void Close();

        //------------------From app and phone idle-----------------------

        /**
        * Tells server to start sending the given string.
        *
        * @param aReqStatus Status request to be completed after sending.
        * @param aMsgData The 7-bit buffer.
        * @param aMsgAttribute Send definitions.
        */
        void SendUssd( 
            TRequestStatus& aReqStatus, 
            const TDesC8& aMsgData, 
            const TDesC8& aMsgAttribute );

        /**
        * Informs the server that the active object taking care of notify 
        * events has issued Cancel() so server can delete all notifys/events 
        * concerning it and sent a cancel signal.
        */      
        void CancelSend();

        //---------------From msg center-------------------

        /**
        * Starts editor. Creates own window server session.
        *
        * @return Error code.
        */
        TInt StartUssdEditor() const ;


        // ------------ For SAT -------------------

        /**
        * Starts SAT session.
        *
        * @param aStatus Completed status.
        * @param aReceiveMessage Received message.
        * @param aShowNotesAndDcs Contains the information of note showing,
        *                         ( Phone Client -> Phone Server ) and
        *                         The used dcs in received message
        *                         ( Phone Server -> Phone Client ).
        */
        void StartSAT( 
            TRequestStatus& aStatus, 
            TDes& aReceiveMessage, 
            TPckg< TUint >& aShowNotesAndDcs );

        /**
        * Cancels SAT session.
        */ 
        void StopSAT();



        //--------------------- From app ------------------------------

        /**
        * UI informs that it is been created
        * 
        * @return Error code.
        */
       TInt AppStarting();

        /**
        * UI informs that it is terminating.
        *
        * @param aExitReason The reason why the application was terminated.
        * @return Error code.
        */
        TInt AppTerminating( 
            TPhCltUssdAppExitReason aExitReason );

        /**
        * UI informs that it is brougth to foreground.
        *
        * @return Should the editor be emptied?
        */
        TBool AppToForeground();

        /**
        * UI informs that it is gone background.
        *
        * @return Error code.
        */
        TInt AppToBackground();




    };

#endif // RPHCLTUSSD_H
