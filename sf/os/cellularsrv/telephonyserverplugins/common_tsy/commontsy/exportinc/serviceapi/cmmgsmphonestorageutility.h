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
*
*/




/**
 @file
 @publishedPartner
 @released
*/
#ifndef CMMGSMPHONESTORAGEUTILITY_H
#define CMMGSMPHONESTORAGEUTILITY_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <etelmm.h>
#include <badesca.h>

/** Maximum length of telephone number */
const TInt KPBStoreNumSize = 100;

/** Used for passing a correct ServiceType */
struct TServiceType
    {
    /** Number of entries */
    TInt iNumOfEntries;
    /** Number of used entries */
    TInt iUsedEntries;
    /** Length of name */
    TInt iNameLen;
    /** Length of number */
    TInt iNumLen;
    };

/** Used to buffer incoming ON store messages to the TSY. */
struct TONStoreMsg
    {
    /** Location */
    TUint16 iLocation;
    /** Name */
    TBuf<RMobileONStore::KOwnNumberTextSize> iName;
    /** Telephone number */
    TBuf<KPBStoreNumSize> iTelNumber;
    };

/** Used to buffer incoming EN store messages to the TSY. */
struct TENStoreResponse
    {
    /** Location */
    TUint8 iLocation;
    /** Emergency number */
    TBuf<RMobileENStore::KEmergencyNumberSize> iECCNumber;
    };

/**Used for passing information concerning the ADN/FDN Max Number length
  and location where a SIM contact was written */
struct TPBEntryInfo
    {
    /** Location */
    TUint16 iLocation;
    /** Maximum number length */
    TInt iMaxNumLength;
    };

/** Used for passing FDN related information in case where phonebook
  initialization fails */
struct TPBFdnInfo
    {
    /** Number of entries */
    TUint16 iFDNNumOfEntries;
    /** Maximum number length */
    TInt iFDNNumberLengthMax;
    /** Maximum text length */
    TInt iFDNTextLengthMax;
    };



// CLASS DECLARATIONS

/**
 *  Used to hold the entry information for PhoneBook store messages
 *  between DOS & Common side
 *
 */
class CPhoneBookStoreEntry : public CBase
    {
    public:

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL()
            {
            iLocation = 0;
            iText = NULL;
            iNumber = NULL;
            iEmail = new( ELeave ) CDesCArrayFlat( 1 );
            iSne = new( ELeave ) CDesCArrayFlat( 1 );   
            iAnr = new( ELeave ) CDesCArrayFlat( 1 );
            }

        /**
         * Destructor.
         */
        virtual ~CPhoneBookStoreEntry()
            {
            if ( iEmail )
                {
                delete iEmail;
                iEmail = NULL;
                }
            if ( iSne )
                {
                delete iSne;
                iSne = NULL;
                }
            if ( iAnr )
                {
                delete iAnr;
                iAnr = NULL;
                }
            if ( iText )
                {
                delete iText;
                iText = NULL;
                }
            if ( iNumber )
                {
                delete iNumber;
                iNumber = NULL;
                }
            };

        /**
         * C++ default constructor.
         */
        CPhoneBookStoreEntry()
            {
            };

  public: //PB SIM Store related data

        /**
         * Location
         */
        TUint16 iLocation;

        /**
         * Text
         */
        HBufC* iText;

        /**
         * Number 
         */
        HBufC* iNumber;
        
        /**
         * Email array
         */
        CDesCArray* iEmail;

        /**
         * Second Name array
         */
        CDesCArray* iSne;

        /**
         * Additional Number array
         */
        CDesCArray* iAnr;

    };

// CLASS DECLARATION

/**
 *  Used to keep the DLL data, used for StoreInfo
 *
 */
class CStorageInfoData
    {
    public:

        /**
         * C++ default constructor.
         */
        CStorageInfoData()
            {
            iIsPhonebookInitialized = EFalse;
            iIsPhonebookInitializeFailed = EFalse;
            iADNNumOfEntries = -1;
            iADNTextLengthMax = -1;
            iADNNumberLengthMax = -1;
            iSNENumOfEntries = -1;
            iSNENumOfEntriesPerEntry = -1;
            iSNETextLengthMax = -1;
            iEmailNumOfEntries = -1;
            iEmailNumOfEntriesPerEntry = -1;
            iEmailTextLengthMax = -1;
            iFDNNumOfEntries = -1;
            iFDNTextLengthMax = -1;
            iFDNNumberLengthMax = -1;
            iSDNNumOfEntries = -1;
            iSDNTextLengthMax = -1;
            iSDNNumberLengthMax = -1;
            iVMBXNumOfEntries = -1;
            iVMBXTextLengthMax = -1;
            iVMBXNumberLengthMax = -1;
            iVMBXCaps = 0;
            iMSISDNNumOfEntries = -1;
            iMSISDNTextLengthMax = -1;
            iMSISDNNumberLengthMax = -1;
            iGRPNumOfEntriesPerEntry = -1;
            iANRNumOfEntries = -1;
            iANRNumOfEntriesPerEntry = -1;
            iANRNumLengthMax = -1;
            iMBDNNumOfEntries = -1;
            iMBDNTextLengthMax = -1;
            iMBDNNumberLengthMax = -1;
            iMBDNCaps = 0;
            };
            
        // Let's keep these as a public, straight
        // handling possible
        TBool iIsPhonebookInitialized;
        TBool iIsPhonebookInitializeFailed;
        // AND
        TInt iADNNumOfEntries;
        TInt iADNTextLengthMax;
        TInt iADNNumberLengthMax;
        // SNE : 3G addtional entry
        TInt iSNENumOfEntries;
        TInt iSNENumOfEntriesPerEntry;
        TInt iSNETextLengthMax;
        // EMAIL : 3G additional entry
        TInt iEmailNumOfEntries;
        TInt iEmailNumOfEntriesPerEntry;
        TInt iEmailTextLengthMax;
        // FDN
        TInt iFDNNumOfEntries;
        TInt iFDNTextLengthMax;
        TInt iFDNNumberLengthMax;
        // SDN
        TInt iSDNNumOfEntries;
        TInt iSDNTextLengthMax;
        TInt iSDNNumberLengthMax;
        // VMBX
        TInt iVMBXNumOfEntries;
        TInt iVMBXTextLengthMax;
        TInt iVMBXNumberLengthMax;
        TUint32 iVMBXCaps;
        // MSISDN
        TInt iMSISDNNumOfEntries;
        TInt iMSISDNTextLengthMax;
        TInt iMSISDNNumberLengthMax;
        // GRP
        TInt iGRPNumOfEntriesPerEntry;
        // ANR
        TInt iANRNumOfEntries;
        TInt iANRNumOfEntriesPerEntry;
        TInt iANRNumLengthMax;
        //MBDN
        TInt iMBDNNumOfEntries;
        TInt iMBDNTextLengthMax;
        TInt iMBDNNumberLengthMax;
        TUint32 iMBDNCaps;
};

#endif      //  CMMGSMPHONESTORAGEUTILITY_H

// End of File
