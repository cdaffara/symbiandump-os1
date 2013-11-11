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
* Name        : CSatDataDownloadTsy.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* DataDownload-related functionality of Sat Tsy
* Version     : 1.0
*
*/



#ifndef CSATDATADOWNLOADTSY_H
#define CSATDATATDOWNLOADTSY_H

//  INCLUDES
#include <e32base.h>
#include <etelsat.h>
#include <et_phone.h>

// FORWARD DECLARATION
class CSatTsy;
class CSatDataPackage;

//CONSTANTS
// Cell Broadcast isi msg length
const TUint8 KCbsMsgMaxLength           = 92;
// Max length for file path
const TUint8 KMaxFilePath               = 255;
// Master file
const TUint16 KMasterFileCbmids         = 0x3F00;
// Dedicated file
const TUint16 KDedicatedFileCbmids      = 0x7F20;
// Elementary file
const TUint16 KElementaryFileCbmids     = 0x6F48;
// User data max size, Etel MM API Design Document 7.1.4 SMS message buffers
const TUint8 KTpduUserDataMaxLength     = 140;
// RP-ACK min length ()
const TUint8 KSmsDeliverReportMinLength = 5;

// 3GPP TS 11.14, 7 Data download to SIM and 3GPP TS 23 038,
// 4 SMS Data coding scheme
const TUint8 KSmsTpduAddrOffset         = 1;
const TUint8 KSmsTpduProtcolIdUSimDdl   = 0x7F;
// Parameters(1)+TP-OA(min 2)+TP-PID(1)+DCS(1)+TP-SCTS(7)+TP-UDL(2)+TP-UD(?)
const TUint8 KSmsTpduMinLength          = 13; 
const TUint8 KSmsTpduSctsLength         = 7;
// 3GPP TS 23.040, 9.2.3.27 TP-Parameter-Indicator
const TUint8 KAllOptParamsPresent       = 0x07;
// 3GPP TS 23.040, 9.2.3.22 TP-Failure-Cause
const TUint8 KDataDownloadError         = 0xD5;

// DESCRIPTION
/**
* Sat Data download handling
* @lib simatktsy
* @since S60 3.1
*/

NONSHARABLE_CLASS ( CSatDataDownloadTsy ) : public CBase
    {    
    public:  // Enumerations
   
        /*
        * This structure contains data needed for RP-ACK or Error report that
        * is sent when saving the class 2 SMS (only when SMS PP Data Download
        * is not supported )
        */
        struct TSmsTpdu 
            {
            TUint8 iParameters;                         // Parameters
            TUint8 iProtocolId;                         // Protocol ID
            TUint8 iDcs;                                // Data coding scheme
            TBuf8<KTpduUserDataMaxLength> iUserData;    // User data
            }; 
            
    public:

        /**
        * Two-phased constructor.
        * @param aSatTsy
        * return CSatTsy*: created datadownload object 
        */
        static CSatDataDownloadTsy* NewL( CSatTsy* aSatTsy );

        /**
        * C++ Destructor.
        */
        ~CSatDataDownloadTsy(  );

    private:

        /**
        * By default C++ constructor is private.
        */
        CSatDataDownloadTsy(  );

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL(  );

    public: // New functions

        /*
        * Creates envelope for cell broadcast download
        * @param aDataPackage: Packaged parameters
        */
        void CompleteCellBroadcastDdlL( const CSatDataPackage* aDataPackage );

        /*
        * Completes the SMS PP Data Download request received from DOS
        * @param aDataPackage: Packaged SMS Address and TPDU
        */
        void CompleteSmsPpDdlL( const CSatDataPackage* aDataPackage );
        
        /*
        * Creates envelope for SmsPp data download
        * @param aSmsScAddress: Address of the originating SMS
        * @param aSmsTpdu: SMS Transfer Protocol Data Unit 
        */
        void CreateSmsPpDdlEnvelopeL( const TDesC8& aSmsScAddress, 
            const TDesC8& aSmsTpdu );

        /**
        * Request to read Cbmids from Sim server
        */
        void CompleteReadCbmidsL();
        
        /*
        * Sets the support status for SMS PP Data Download
        * @param aDataPackage: ETrue = supported, otherwise EFalse
        */
        void SetSmsPpDdlStatus( const CSatDataPackage* aDataPackage );
        
        /*
        * Creates the SMS-DELIVER-REPORT as an response to SMS PP DDL request
        * @param aResult KErrNone/KErrWrite
        */
        void CreateSmsDeliverReportL( TInt aResult );
        
    private:    // methods
    
        /*
        * Creates SMS Entry in order to save class 2 SMS
        * @param aSmsScAddress: Address of the originating SMS
        * @param aSmsTpdu: SMS Transfer Protocol Data Unit 
        */
        void CreateEntryForSavingSmsL( const TDesC8& aSmsScAddress, 
            const TDesC8& aSmsTpdu );
        
        /*
        * Creates SMS Entry in order to save class 2 SMS
        * @param aSmsTpdu: SMS Transfer Protocol Data Unit 
        * @return KErrNone/KErrCorrupt
        */
        TInt ParseSmsTpdu( const TDesC8& aSmsTpdu );


    private: // Data

        // Pointer to Sat TSY
        CSatTsy* iSatTsy; 
        // Indicates whether the SMS PP Data Download is allocated in NAA
        TBool iIsSmsPpDdlSupported;  
        // TPDU data is needed later for the report
        TSmsTpdu iSmsTpdu;
    };

#endif // CSATDATADOWNLOADTSY_H

// End of File
