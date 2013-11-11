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
* Description:  Interface for sending messages.
*
*/


#ifndef CPHCLTUSSD_H
#define CPHCLTUSSD_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
* USSD application Exit reason.
*/
enum TPhCltUssdAppExitReason
    {
    EPhCltUserExit,          // User exited the application.
    EPhCltSendCompleted,     // Send operation was completed.
    EPhCltExitReasonUnknown  // Exit reason is unknown.
    };

// The next four values are for SAT dcs.
const TUint KPhCltDcs7Bit    = 101; // 7-bit dcs.
const TUint KPhCltDcs8Bit    = 102; // 8-bit dcs.
const TUint KPhCltDcsUcs2    = 103; // UCS2 dcs.
const TUint KPhCltDcsUnknown = 104; // Unknown dcs.

// The notes are shown in SAT session.
const TUint KPhCltShowNotes = 1000;

/**
*  Interface for sending messages.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class CPhCltUssd : 
    public CBase
    
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aShowNotes If ETrue, notes are shown.
        * @return Pointer to created CPhCltUssd instance.
        */
        IMPORT_C static CPhCltUssd* NewL( TBool aShowNotes = ETrue );

    public: // New functions

        /**
        * Send Ussd string to network with default DCS.
        *
        * @param aMsgData Unicode string to be sent.
        * @return Error code.
        */
        IMPORT_C virtual TInt SendUssd( const TDesC& aMsgData ) = 0;

        /**
        * Send Ussd buffer to network with default DCS.
        *
        * @param aMsgData 7-bit buffer to be sent.
        * @return Error code.
        */
        IMPORT_C virtual TInt SendUssd( const TDesC8& aMsgData ) = 0;

        /**
        * Send Ussd buffer to network with given DCS.
        *
        * @param aMsgData 7-bit buffer to be sent.
        * @param iSendDcs Specifies DCS and the send type.
        * @return Error code.
        */
        IMPORT_C virtual TInt SendUssd( 
            const TDesC8& aMsgData, 
            const TUint8 iSendDcs ) = 0;

        /**
        * Cancels outstanding Send request.
        */
        IMPORT_C virtual void SendUssdCancel() = 0;
               
        /**
        * Set data coding scheme
        * 
        * @param aDCS data coding scheme value
        */
        virtual void SetDCS( TUint8 aDCS ) = 0;
        
        /**
        * Starts editor. Creates own window server session.
        *
        * @return Error code.
        */
        IMPORT_C virtual TInt StartUssdEditor() const = 0;
    
    protected:    
        /**
        * C++ constructor.
        */
        CPhCltUssd();
        
    };

#endif // CPHCLTUSSD_H

// End of File
