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
#include <MPhCltUssdRequestHandler.h>
#include <aknprogressdialog.h>


// FORWARD DECLARATIONS
class CPhCltUssdRequestHandler;
class CAknWaitDialog;
class CAknResourceNoteDialog;
class RConeResourceLoader;


// CLASS DECLARATION

/**
*  Interface for sending messages.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class CPhCltUssd : 
    public CBase, 
    public MPhCltUssdRequestHandler, 
    public MProgressDialogCallback
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aShowNotes If ETrue, notes are shown.
        * @return Pointer to created CPhCltUssd instance.
        */
        IMPORT_C static CPhCltUssd* NewL( TBool aShowNotes = ETrue );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhCltUssd();
        

    public: // New functions

        /**
        * Send Ussd string to network with default DCS.
        *
        * @param aMsgData Unicode string to be sent.
        * @return Error code.
        */
        IMPORT_C TInt SendUssd( const TDesC& aMsgData  );

        /**
        * Send Ussd buffer to network with default DCS.
        *
        * @param aMsgData 7-bit buffer to be sent.
        * @return Error code.
        */
        IMPORT_C TInt SendUssd( const TDesC8& aMsgData );

        /**
        * Send Ussd buffer to network with given DCS.
        *
        * @param aMsgData 7-bit buffer to be sent.
        * @param iSendDcs Specifies DCS and the send type.
        * @return Error code.
        */
        IMPORT_C TInt SendUssd( 
            const TDesC8& aMsgData, 
            const TUint8 iSendDcs );

        /**
        * Cancels outstanding Send request.
        */
        IMPORT_C void SendUssdCancel();
               
        /**
        * Set data coding scheme
        * 
        * @param aDCS data coding scheme value
        */
        void SetDCS( TUint8 aDCS );
        
    protected:  // From base classes
        

        // Called when send completes.
        void HandleSendEventL( const TInt aError );

        // Called whrn the user cancel the wait dialog.
        void DialogDismissedL( TInt aButtonId );
        

    protected: // New functions

        // Constructor. 
        CPhCltUssd();

        // Symbian 2nd phase constructor.
        void ConstructL( TBool aShowNotes );

        // Loads the resource file.
        void LoadResourceFileL();

        // The note type enumeration, used inside the class.
        enum TPhCltUssdNoteType
            {
            EPhCltUssdInformationNote,
            EPhCltUssdGlobalInformationNote,
            EPhCltUssdGlobalConfirmationNote
            };

        // Shows the note of given type. 
        void ShowNoteL( TPhCltUssdNoteType aType, TInt aResourceId );

        // Encodes the message.
        void EncodeL( const TDesC& aSrc, TDes8& aDes );
        
        // Finds CR character        
        void CPhCltUssd::FindFirstCarriageReturnL( 
        const TDesC& aBuffer ,
        TUint& aSkipChars , 
        TUint& aStartBit );

    private:
         
        // Launches wait dialog
        void LaunchWaitDialogL();

    private:    // Data

        // The handler of the request.
        CPhCltUssdRequestHandler* iRequestHandler;

        // Wait scheduler used in this class. 
        CActiveSchedulerWait* iWait;

        // Pointer to sending error store place.
        TInt* iSendError;

        // Resource loader.
        RConeResourceLoader* iResourceLoader;

        // Wait note dialog.
        CAknWaitDialog* iWaitNote;

        // Resourcce note dialog.
        CAknResourceNoteDialog* iDlg;

        // Buffer for messages.
        HBufC* iMessageBuffer;
        
        // Data coding scheme value
        TUint8 iDCS;
        
        // Data coding scheme value for ussd sending
        TUint8 iDCS2;
        
    };

#endif // CPHCLTUSSD_H

// End of File
