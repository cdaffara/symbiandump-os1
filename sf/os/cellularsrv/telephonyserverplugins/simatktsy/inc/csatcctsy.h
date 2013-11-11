/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name        : CSatCCTsy.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Interface   : -
* Call control-related functionality of Sat Tsy
* Version     : 1.0
*
*/




#ifndef CSATCCTSY_H
#define CSATCCTSY_H


//  INCLUDES
#include <e32base.h>
#include <etelsat.h>
#include "TTlv.h"
#include "TSatUtility.h"
// CONSTANTS
const TUint16 KMaxLengthDefaultBearerData         = 258;
const TUint8  KMaxBccLength                       = 16;

// MACROS
    //none

// DATA TYPES
    //none

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
class CSatTsy;
class CSatDataPackage;
class CSatNotificationsTsy;

// CLASS DECLARATION
    //none

// DESCRIPTION
/**
*  CSatCCTsy contains call control related handling functionality.
*  
*  @lib Commonsimatktsy
*  @since 2.6
*/
NONSHARABLE_CLASS ( CSatCCTsy ) : public CBase
    {
    public:
    	
   	typedef TBuf8<KMaxLengthDefaultBearerData> TBearerData;
            
	    /*
	    * This structure contains information needed in constructing Call 
	    * Control (call/SS/USSD) Envelope and handling the call control events.
	    */
   		struct TCCInfo
            {
            TUint8 iTag;                // Tag of envelope
            TBuf8<KMaxBccLength> iBcc1; // For storing bearer capability 1            
            TBuf8<KMaxBccLength> iBcc2; // For storing bearer capability 2
            TBuf8<KMaxAddressLength> iAddress; // For storing address
            TUint8 iTonNpi;             // For storing ton&npi
            TUint8 iDcs;                // For storing DCS (only USSD)
	        };

    public:

        /**
        * Two-phased constructor.
        * @param aSatTsy
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        * return CSatTsy*: created CC object 
        */
        static CSatCCTsy* NewL( CSatTsy* aSatTsy,
            CSatNotificationsTsy* aNotificationsTsy );

        /**
        * C++ Destructor.
        */
        ~CSatCCTsy();
    
    private:    // Constructors

        /**
        * By default C++ constructor is private.
        * @param aNotificationsTsy: Pointer to NotificationsTsy
        */
        CSatCCTsy( CSatNotificationsTsy* aNotificationsTsy );

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

    public: // New functions

		/**
        * Saves default bearer data for voice call.
        * @param aDataPackage contains default bearer capability
        * @param aResult KErrNone
        */
        void CompleteBearerCapability( CSatDataPackage* aDataPackage, 
			TInt aResult );
			
	    /**
        * Identifies the type of envelope and calls appropriate method
        * for constructing the envelope
        * @param aDataPackage: Contains index for iCCInfoArray
        */
        void CreateEnvelopeL( CSatDataPackage* aDataPackage );
        
        /**
        * Checks whether the Alpha ID is present in the data notification and
        * if that is the case - requests completion to the client
        * @param aDataPackage   pointer to notification data
        */
        void CheckIfAlphaIdPresent( CSatDataPackage* aDataPackage );
        
        /**
        * Handles the call control event coming in data notification
        * @param aIpc: The IPC number of the request
        * @param aDataPackage: Contains the pointer to TCCInfo data structure
        */
        void CompleteCCEventL( TInt aIpc, CSatDataPackage* aDataPackage );
            
        /**
        * Set CSatCCTsy internal Ton and Npi
        * @param aTonNpi: Type Of Number and Numbering Plan Indicator
        */
        void SetTonNpiForSS( const TUint8 aTonNpi );
        
        /*
        * Set internal flag according to EF-SST
        * @param aDataPackage: Pointer to the status of USSD support
        */
        void SetUssdStatus( CSatDataPackage* aDataPackage );
        
        /**
        * Stores proactive originated Call/SS address.
        * @param aAddress: Destination address
        */
        void StoreAddressForCC( const TDesC8& aAddress);
 
    private:    // Methods

        /**
        * Creates Call Envelope and sends it to NAA.
        * @param aCCInfo: structure containing data to be sent
        */
        void CreateCallEnvelopeL( const TCCInfo* aCCInfo );

        /**
        * Creates SS Envelope and sends it to NAA.
        * @param aCCInfo: structure containing data to be sent
        */
        void CreateSSEnvelopeL( const TCCInfo* aCCInfo );

        /**
        * Creates USSD Envelope and sends it to NAA.
        * @param aCCInfo: structure containing data to be sent
        */
        void CreateUSSDEnvelopeL( const TCCInfo* aCCInfo );
        
        /*
        * Method checks if the given Ussd string contains
        * only characters "*", "#", and the numbers 0-9.
        * @param aSsUssdString: Ussd string to be checked
		* @return ETrue = Ussd string contains only "*", "#", and 
        * the numbers 0-9. EFalse = Ussd string contains also other characters.
        */
        TBool IsOnlyDigitsInUssd( const TDesC8& aUSSDString ); 

#ifdef USING_CTSY_DISPATCHER
    public: //to allow access to HasProactiveOrigin() from the dispatcher
#endif //end USING_CTSY_DISPATCHER
        /**
        * Compares the given address with saved proactive originated 
        * Call/SS address
        * @param aAddress: Address to compare
        * @return ETrue if addresses do match, otherwise EFalse
        */
        TBool HasProactiveOrigin( const TDesC8& aAddress);

    private:    // Data
    
        // Pointer to Sat TSY
        CSatTsy* iSatTsy;
        // Pointer to the notifications tsy class
        CSatNotificationsTsy* iNotificationsTsy;
	    // Default bearer data for voice call.
	    TBearerData iBearerData;
	    // True if TonNpi is resolved from SEND SS proactive commmand.
        TBool iTonNpiPresent;
        // For setting TON/NPI for SendSSEnvelope
        TUint8 iTonNpiForSS;
        // Address of Send SS or Setup Call proactive command is stored here
        // Used to determined wether to check FDN when an event is received
        TBuf8<KMaxAddressLength> iProactiveCommandAddress;
        // The status of USSD support in EF-SST
        TUint8 iUssdTlvSupported;
    };              

#endif // CSATCCTSY_H

            
// End of File
