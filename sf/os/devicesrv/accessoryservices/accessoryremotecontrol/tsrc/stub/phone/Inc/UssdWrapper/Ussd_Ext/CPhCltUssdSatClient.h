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
* Description:  Ussd client for SAT usage.
*
*/


#ifndef CPHCLTUSSDSATCLIENT_H
#define CPHCLTUSSDSATCLIENT_H


//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CPhCltUssdSatActive;
class CPhCltUssdImp;


// CLASS DECLARATION

/**
*  Ussd client for SAT Server.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class CPhCltUssdSatClient :public CBase
    {
    public:

    /**
    * Values to tell the used DCS in SAT message received from network.
    */
    enum TPhCltDcs
        {
        EPhCltDcs7Bit = 1,   // DCS was 7bit.
        EPhCltDcs8Bit,       // DCS wass 8bit.
        EPhCltDcsUCS2,       // DCS was UCS2.
        EPhCltDcsUnknown     // DCS was unknown.
        };


    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @param aShowNotes Show notes on first and last message?
        * @return Pointer to created CPhCltUssdSatClient instance.
        */
        IMPORT_C static CPhCltUssdSatClient* NewL( TBool aShowNotes );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhCltUssdSatClient();
        

    public: // New functions
        
        /**
        * Start Ussd session from SAT.
        * 
        * @param aSendMessage The string which is sent to network.
        * @param iSendDcs Specifies DCS and the send type.
        * @param aReceiveMessage Here is copied the return message.
        * @param aSendCompletedFirst ETrue if send is completed before receive.
        * @param aReceivedDcs The used DCS in message sent by network.
        * @return Error code. KErrNone no errors
        *                     KErrInUse send is already active.
        *                     Others from the network or from connection.
        */
        IMPORT_C TInt SendSatMessage( 
            const TDesC& aSendMessage, 
            const TUint8 iSendDcs,
            TDes& aReceiveMessage,
            TBool& aSendCompletedFirst,
            TUint8& aReceivedDcs 
            );

        /**
        * Cancels outstanding Send request.
        */
        IMPORT_C void SendSatMessageCancel();


    private:
        
        /**
        * C++ default constructor.
        */
        CPhCltUssdSatClient();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TBool aShowNotes );

        /**
        * Decodes the dcs received from Phone Server. Puts the result to
        * parameter aReceivedDcs.
        */
        void DecodeDcs( TUint8& aReceivedDcs );


    private:    // Data

        // Storage place for note show information.
        TBool                iShowNotes;

        // Storage for SAT error code.
        TInt                 iSatError;

        // Storage for received message.
        TPtrC                iReceiveMessage;

        // Send completion information.
        TBool*               iSendCompletedFirst;
        
        // Helper class, private
        CPhCltUssdSatActive* iSatActive;


        // USSD wrapper class.
        CPhCltUssdImp*          iUssdWrapper;

        // This variable has two purposes:
        // 1) Client -> Server: Tells whether to show the notes or not.
        // 2) Server -> Client: Tells the used dcs in received message.
        TUint                iShowNotesAndDcs;
    };

#endif      // CPHCLTUSSDSATCLIENT_H

// End of File
