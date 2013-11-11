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
* Description:  API for PhoneClient Command Handler.
*
*/


#ifndef CPHCLTCOMMANDHANDLER_H
#define CPHCLTCOMMANDHANDLER_H


//  INCLUDES
#include <e32base.h>
#include <PhCltTypes.h>


// FORWARD DECLARATIONS
class RPhCltServer;

/**
* Commands.
*
* EPhCltCommandAtd      Dial command.
* 
* EPhCltCommandAta      Answer command.
* 
* EPhCltCommandChld     Call hold and multiparty handling command.
* 
* EPhCltCommandChup     Hangup command.
* 
* EPhCltCommandVts      DTMF sending (start and stop) command.
* 
* EPhCltCommandBvra     Bluetooth Voice Recognition Activation command.
* 
* EPhCltCommandMuteMic          Microphone mute command.
* 
* EPhCltCommandMuteRingingTone  Ringing tone mute command.
*/
enum TPhCltComHandCommand
    {
    EPhCltCommandAtd  = 0,
    EPhCltCommandAta  = 1,
    EPhCltCommandChld = 2,
    EPhCltCommandChup = 3,
    EPhCltCommandVts  = 4,
    EPhCltCommandBvra    = 5,
    EPhCltCommandMuteMic = 6,
    EPhCltCommandMuteRingingTone = 7
    };
    
    
/**
* CHLD commands.
*
* EPhCltChldZero   
*      Release held calls or set UDUB for a waiting call.
* 
* EPhCltChldOne
*      Release all active calls and accept other (held or waiting) call.
* 
* EPhCltChldTwo 
*      Place all active calls on hold and accept the other 
*      (held or waiting) call.
* 
* EPhCltChldThree
*      Add a held call to the conversation.
* 
* EPhCltChldFour
*      Connect the two calls and disconnect the subscriber from both 
*      calls (Explicit call transfer).
*/
enum TPhCltChldCommand
    {
    EPhCltChldZero  = 0,
    EPhCltChldOne   = 1,
    EPhCltChldTwo   = 2,
    EPhCltChldThree = 3,
    EPhCltChldFour  = 4,
    };
    
/** 
* Type of DTMF tone. 
* One tone, i.e. one character.
*/
typedef TChar TPhCltDtmfTone;


/**
* DTMF actions.
*
* EPhCltDtmfNotUsed  
*      Play DTMF tone default time. In this case 
*      DTMF sending need not be stoped by the client.
* 
* EPhCltDtmfStart
*      Start Dtmf sending. Remember to always stop it with EPhCltDtmfStop.
* 
* EPhCltDtmfStop
*      Stop DTMF sending that was started with EPhCltDtmfStart.
* 
*/
enum TPhCltDtmfAction
    {
    EPhCltDtmfNotUsed = 0,
    EPhCltDtmfStart   = 1,
    EPhCltDtmfStop    = 2
    };
    

/*
* The CommandHandler command parameters defined as in one class.
* The handler of received command handler commads must first check the
* performed command, and then read the appropriate parameters.
*/
class TPhCltComHandCommandParameters
    {
    public:    // Data  

        // The command handler command to be performed.
        TPhCltComHandCommand                iCommandHandlerCommand;

        // Dial command parameter.
        TPhCltTelephoneNumber 				iTelNumber;
		
        // Chld command parameter.
        TPhCltChldCommand                   iChldCommand;
        // Chld command parameter.
        TUint                               iChldCallNumber;

        // Vts command parameter.
        TPhCltDtmfTone                      iDtmfTone;
        // Vts command parameter.
        TPhCltDtmfAction                    iDtmfAction;

        // Bvra command parameter.
        TBool                               iBvraActivation;

        // Mic Mute command parameter.
        TBool                               iMute;
        
    };



// CLASS DECLARATION

/**
*  Interface for Command Handler requests.
*  At most one asynchronous request can be ongoing at the same time!
*
*  @lib PhoneClient.lib
*  @since 3.2
*/
class CPhCltCommandHandler : public CBase
    {
    
    
    public:  // Constructors    
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPhCltCommandHandler* NewL();
        
        
    public: // New functions

        /**
        * Dial. Only voice call is supported.
        *
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        * @param aTelephoneNumber The dial information.
        */
		IMPORT_C virtual void Atd(
            TRequestStatus& aStatus, 
            const TPhCltTelephoneNumber& aTelephoneNumber ) = 0;

        /**
        * Answer the call. 
        * Does not need response according to specification.
        *  
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        *                KErrNotReady if there is no call to be answered.
        */
        IMPORT_C virtual void Ata(
            TRequestStatus& aStatus ) = 0;


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
        IMPORT_C virtual void Chld(
            TRequestStatus& aStatus, 
            const TPhCltChldCommand aChldCommand,
            const TUint aCallNumber ) = 0;


        /**
        * Hang up current call.
        * Does not need response according to specification.
        *  
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        */
        IMPORT_C virtual void Chup(
            TRequestStatus& aStatus ) = 0;


        /**
        * DTMF sending, starting and stopping in same function. 
        * Does not need response according to specification.
        *
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        * @param aDtmfTone The DTMF tone to be sent.
        * @param aActivation Desired DTMF action.
        */
        IMPORT_C virtual void Vts(
            TRequestStatus& aStatus, 
            const TPhCltDtmfTone aDtmfTone,
            const TPhCltDtmfAction aAction ) = 0;


        /**
        * Bluetooth Voice Recognition Activation.
        *
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        * @param aActivation
        *           ETrue:  Start voice recognition.
        *           EFalse: Stop voice recognition.
        */
        IMPORT_C virtual void Bvra( 
            TRequestStatus& aStatus, 
            const TBool aActivation ) = 0;


        /**
        * Cancel the ongoing asynchronous request.
        *
        * @param aReqToCancel The request to be canceled.
        */
        IMPORT_C virtual void CancelAsyncRequest( 
            const TPhCltComHandCommand aReqToCancel ) = 0;


        /**
        * Mute microphone
        *
        * @since 2.8
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        * @param aMute ETrue if Microphone Mute is se on.
        */
        IMPORT_C virtual void MuteMicrophone( 
            TRequestStatus& aStatus,
            const TBool aMute ) = 0;


        /**
        * Mute RingingTone
        *
        * @since 2.8
        * @param aStatus The status updated when function call is completed.
        *                KErrNone if successful, Symbian error code otherwise.
        */
        IMPORT_C virtual void MuteRingingTone( 
            TRequestStatus& aStatus ) = 0;
            
    protected:

        /**
        * C++ constructor.
        */
        CPhCltCommandHandler();
            
    };





#endif      // CPHCLTCOMMANDHANDLER_H 

// End of File
