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
* Description:  This class is used to perform command handler originated 
*                commands.
*
*/


#ifndef RPHCLTCOMMANDHANDLER_H
#define RPHCLTCOMMANDHANDLER_H


//  INCLUDES
#include <e32base.h>
#include <PhCltTypes.h>
//#include "CPhCltCommandHandlerNotify.h"
#include "CPhCltCommandHandler.h"

// FORWARD DECLARATIONS
class RPhCltServer;

// CLASS DECLARATION

/**
*  PhoneClient command handler interface.
*  The user of this API has to take care that there is only at most one 
*  request, i.e. function call, ongoing (active) at the time. However, the 
*  corresponding Cancel method can be called while request is ongoing.
*  
*  @lib PhoneClient.lib
*  @since 3.2

*/
class RPhCltCommandHandler
:   public RSubSessionBase
    {
    public:  // Constructors and destructor
        
        /**
        * Constructor.
        */
        RPhCltCommandHandler();


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
        * Dial. Only voice call is supported.
        *
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        * @param aTelephoneNumber The telephony number .
        */
            
		void Atd(
            TRequestStatus& aStatus, 
            const TPhCltTelephoneNumber& aTelephoneNumber );
		

        /**
        * Answer the call. 
        * Does not need response according to specification.
        *  
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        *                KErrNotReady if there is no call to be answered.
        */
        void Ata(
            TRequestStatus& aStatus );


        /**
        * Call hold and multiparty handling.
        * 
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        * @param aChldCommand The command to be performed.
        * @param aCallNumber The call number where the command specified in
        *                    aChldCommand is targeted. 
        *                    Not used currently.
        */
        void Chld(
            TRequestStatus& aStatus, 
            const TPhCltChldCommand aChldCommand,
            const TUint aCallNumber );


        /**
        * Hang up current call.
        * Does not need response according to specification.
        *  
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        */
        void Chup(
            TRequestStatus& aStatus );


        /**
        * DTMF sending, starting and stopping in same function. 
        * Does not need response according to specification.
        *
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        * @param aDtmfTone The DTMF tone to be sent.
        * @param aActivation Desired DTMF action.
        */
        void Vts(
            TRequestStatus& aStatus, 
            const TPhCltDtmfTone aDtmfTone,
            const TPhCltDtmfAction aAction );


        /**
        * Bluetooth Voice Recognition Activation.
        *
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        * @param aActivation
        *           ETrue:  Start voice recognition.
        *           EFalse: Stop voice recognition.
        */
        void Bvra( 
            TRequestStatus& aStatus, 
            const TBool aActivation );


        /**
        * Cancel the ongoing asynchronous request.
        *
        * @param aReqToCancel The request to be canceled.
        */
        void CancelAsyncRequest( 
            const TPhCltComHandCommand aReqToCancel );

        /**
        * Mute Microphone.
        *
        * @since 2.8
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        * @param aMute ETrue if Mic Mute is set on.
        */
        void MuteMicrophone(
            TRequestStatus& aStatus,
            const TBool aMute );


        /**
        * Mute RingingTone.
        *
        * @since 2.8
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        */
        void MuteRingingTone(
            TRequestStatus& aStatus );
            
    private:

        // Prohibit copy constructor if not deriving from CBase.
        RPhCltCommandHandler( const RPhCltCommandHandler& );

        // Prohibit assigment operator if not deriving from CBase.
        RPhCltCommandHandler& operator=( const RPhCltCommandHandler& );


    private:    // Data

        // Command handler arguments.
        TPckgBuf< TPhCltComHandCommandParameters >  iComHandParams;
    };

#endif      // RPHCLTCOMMANDHANDLER_H
            
// End of File
