/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : CSatEventDownloadTsy.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* EventDownload-related functionality of Sat Tsy
* Version     : 1.0
*
*/



#ifndef CSATEVENTDOWNLOADTSY_H
#define CSATEVENTDOWNLOADTSY_H

//  INCLUDES
#include <e32base.h>
#include <etelsat.h>
#include <et_phone.h>

#include "TSatEventList.h"      // Event list class 
#include "TSatUtility.h"        // Utilities

// CONSTANTS
const TUint8  KMaxCauseLength	   = 248;

// FORWARD DECLARATION
class CSatTsy;
class CSatDataPackage;

// DESCRIPTION
/**
* Sat event download handling
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatEventDownloadTsy ) : public CBase
   {

   public:

        /**
        * This structure contains data needed for CallDisconnected
        * envelope.
        */
        struct TCallDisconnectedEnvelope 
            {
            TUint8 iCmdNumber;      		// Command number
            TBool iNearEnd;     			// MO flag
            TBool iCauseGiven;     	    	// Cause given
            TBuf8<KMaxCauseLength>  iCause; // Cause
            };
      
        /**
        * This structure contains data needed for MTCall
        * envelope.
        */
        struct TMTCallEnvelope 
            {
		    TUint8 iCmdNumber;               // Command number
		    TUint8 iTONandNPI;               // Ton and Npi
		    TBuf8<KNumberMaxLength> iDiallingNumber;// Calling Party BCD number
		    TPtrC8 iCalledPartySubAddress;   // Calling Party Subaddress
            };    
     public:

        /**
        * Two-phased constructor.
        * @param aSatTsy
        * return CSatTsy*: created eventdownload object 
        */
        static CSatEventDownloadTsy* NewL( CSatTsy* aSatTsy );

        /**
        * C++ Destructor.
        */
        ~CSatEventDownloadTsy();

    private:

        /**
        * By default C++ constructor is private.
        */
        CSatEventDownloadTsy();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

    public: // New functions
        
        /**
        * Handles extended client requests.
        * @param aTsyReqHandle: TSY request handle
        * @param aIpc: Request IPC number
        * @param aPackage: Packed request parameters
        * @return: Result of the request
        */
        TInt DoExtFuncL(const TTsyReqHandle aTsyReqHandle, const TInt aIpc,
                const TDataPackage& aPackage );

        /**
        * Sets on events
        * @param aEvents: event mask
        */
        void SetUpEventList( TUint32 aEvents );
        
        /**
        * Method to set internal flag to EFalse or ETrue.
        * @param aStatus EFalse or ETrue
        */
        void SetSetUpCallStatus( const TBool aStatus );
        
        /**
        * Cancels request that's IPC number and request handle are given in
        * parameters
        * @param aIpc: Request IPC number
        * @param aTsyReqHandle: TSY request handle
        * @return: Result of the request
        */
        TInt CancelService(
            const TInt aIpc,
            const TTsyReqHandle aTsyReqHandle );

    public:

        /**
        * Creates MT call envelope to NAA
        * @param aDataPackage: Packaged parameters
        * @return phonet sender status
        */ 
        TInt CompleteMTCallL( const CSatDataPackage* aDataPackage );

        /**
        * Creates call connected envelope to NAA
        * @param aDataPackage: Packed parameters
        * @return success/failure indication
        */ 
        TInt CompleteCallConnectedL( const CSatDataPackage* aDataPackage );
        
        /**
        * Creates call disconnected envelope to NAA
        * @param aDataPackage: Packed parameters
        * @return success/failure indication
        */
        TInt CompleteCallDisconnectedL( const CSatDataPackage* aDataPackage );        

        /**
        * Creates location status envelope to NAA
        * @param aDataPackage: Packed parameters
        * @return success/failure indication
        */
        TInt CompleteLocationStatusL( const CSatDataPackage* aDataPackage );
        
        /**
        * Access Technology change
        * Creates Access technology change envelope to NAA
        * @param aDataPackage: Packed parameters
        * @return phonet sender status
        */
        TInt CompleteAccessTechnologyChangeL( 
        	const CSatDataPackage* aDataPackage );

	private:

        /**
        * Creates user activity envelope to NAA
        * @return success/failure indication
        */
        TInt CreateUserActivityEnvelopeL();
        
        /**
        * Creates idle screen available envelope to NAA
        * @return success/failure indication
        */
        TInt CreateIdleScreenAvailableEnvelopeL();
        
        /**
        * Creates browser termination envelope to NAA
        * @param aCause: Browser termination cause
        * @return success/failure indication
        */
        TInt CreateBrowserTerminationEnvelopeL(
            const RSat::TBrowserTerminationCause aCause );
        
        /**
        * Creates language selection envelope to NAA
        * @param aLanguage: selected language
        * @return success/failure indication
        */
        TInt CreateLanguageSelectionEnvelopeL( const TUint16 aLanguage );

        /**
        * Bearer Independent Protocol
        * Creates data available envelope to NAA. The NAA may sent a
        * ReceiveData PCmd after a DataAvailable event.
        * @param aStatus channel status
        * @param aLength data length to be received
        * @return phonet sender status
        */
        TInt CreateDataAvailableEnvelopeL( const RSat::TChannelStatus aStatus,
            const TInt8 aLength );
            
        /**
        * Bearer Independent Protocol
        * Creates channel status envelope to NAA
        * @param aStatus channel status
        * @return phonet sender status
        */
        TInt CreateChannelStatusEnvelopeL( const RSat::TChannelStatus aStatus );        
        

    private: // Data

        // Pointer to Sat TSY
        CSatTsy* iSatTsy;
        // Event list
        TSatEventList iEventList;
        // Flag for SetUpCall
        TBool iSetUpCallOngoing;
        // Location status
        TUint8 iLocationStatus;
        // Operator code
        TBuf8<3> iOperatorCode;
        // Location area code
        TUint16 iLocationaAreaCode;
        // Cell Id
        TUint16 iCellId;           
        // Flag for forcing the Location status envelope is every time after 
        // SetUpEventList PCmd
        TBool iForceLocationStatusEnvelope;
    };

#endif // CSATEVENTDOWNLOADTSY_H

// End of File
