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
* Name        : CSatIconTsy.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Interface   : -
* Icon related functionality of Sat Tsy
* Version     : 1.0
*
*/




#ifndef CSATICONTSY_H
#define CSATICONTSY_H


//  INCLUDES
#include <e32base.h>
#include <etelsat.h>

#include "CSatTsy.h"                            // Sat Tsy class

// CONSTANTS
const TUint8 KFilePathLength            = 8;
const TUint8 KClutEntrySize             = 3;
const TUint8 KMaxRecordDataSize         = 255;
const TUint8 KIconInfoLength            = 6;
const TUint8 KGetIconDataPckgLength     = 2;

// From 3GPP TS 51.011, 10.6.11 EFimg (Image)
const TUint8 KImageCodingSchemeByteNr   = 3;
const TUint8 KImageInstanceIdOffset  	= 4;
const TUint8 KImageInstanceFileLength  	= 9;

// File ID
const TUint16 KMasterFile            = 0x3F00;  
const TUint16 KDedicatedFile         = 0x7F10;  
const TUint16 KDedicatedFileGraphics = 0x5F50;  
const TUint16 KElementaryFileImg     = 0x4F20;  
const TUint16 KIconDataMaxSize       = 0x3FE8;

// FORWARD DECLARATIONS
class CSatDataPackage;


// CLASS DECLARATION
    //none

// DESCRIPTION
/**
*  CSatIconTsy contains Icon handling functionality.
*  
*  @lib Commonsimatktsy
*  @since 3.1
*/
NONSHARABLE_CLASS ( CSatIconTsy ) : public CBase
    {
    public: // Enumerations

        enum TEtelIconRequestType
            {
            ERequestTypeUnknown,
			EGetIconReq,
            EGetImageInstanceReq,
            EGetClutReq
            };
        /*
	    * The data in this structure contains information 
	    * needed in Get Icon, Get image instance and get clut.
	    */
	    struct TIconInfo
		    {
            TUint16 iOffset;                      // Offset
            TUint16 iLength;                      // Length
			TBuf8<KFilePathLength> iSimFilePath;  // Sim file path
	        };

        /**
        * Two-phased constructor.
        * @param aSatTsy
        * return CSatTsy*: created Icon object 
        */
        static CSatIconTsy* NewL( CSatTsy* aSatTsy );

        /**
        * C++ Destructor.
        */
        virtual ~CSatIconTsy();
    
    public: // New functions

        /**
        * Handles extended client requests. 
        * @param aTsyReqHandle TSY request handle
        * @param aIpc Request IPC number
        * @param aPackage Packed request parameters
        * @return Result of the request
        */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, const TDataPackage& aPackage );

        /**
        * Completes GetIcon request.
        * @param aDataPackage: Icon data
        * @param aResult: KErrNone or error value
        */
        void CompleteGetIcon( CSatDataPackage* aDataPackage, TInt aResult );

        /**
        * Completes GetImageInstance request.
        * @param aDataPackage: Image instance data
        * @param aResult: KErrNone or error value
        */
        void CompleteGetImageInstance( CSatDataPackage* aDataPackage, 
            TInt aResult );

        /**
        * Completes GetClut request.
        * @param aDataPackage: Clut data
        * @param aResult: KErrNone or error value
        */
        void CompleteGetClut( CSatDataPackage* aDataPackage, TInt aResult );

    private:  // Constructors

        /**
        * By default C++ constructor is private.
        */
        CSatIconTsy();

        /**
        * Class attributes are created in ConstructL.
        * @return None
        */
        void ConstructL();

    private:  // Methods

        /**
        * This method allows the client to get the content of  
        * the EF_IMG record specified by aRecordNumber.
        *
        * @param aTsyReqHandle Request handle
        * @param aRecordNumber record number
        * @param aIconEf Icon elementary file
        * @return KErrNone
        */
        TInt GetIconL( const TTsyReqHandle aTsyReqHandle,
            TUint8* aRecordNumber, RSat::TIcon* aIconEf );

        /**
        * This method allows the client to get the image instance 
        * specified by the InstanceNumber, Offset and Length contained 
        * in the given InstanceInfo. 
        *
        * @param aTsyReqHandle Request handle
        * @param aInstanceInfo Instance info
        * @param aInstance instance 
        * @return KErrNone
        */
        TInt GetImageInstanceL( const TTsyReqHandle aTsyReqHandle,
            TDesC8* aInstanceInfo, TDes8* aInstance );

        /**
        * This method allows the client to get the CLUT specified by 
        * InstanceNumber and Offset.
        *
        * @param aTsyReqHandle Request handle
        * @param aNumberAndOffset Instance number and offset
        * @param aClut Colour look-up table
        * @return KErrNone
        */
        TInt GetClutL( const TTsyReqHandle aTsyReqHandle,
            RSat::TInstanceNumberAndOffset* aNumberAndOffset,
            TDes8* aClut );  

    protected: // Data

        // Pointer to Sat TSY
        CSatTsy* iSatTsy;

    private:    // Data

        // Used to track what Etel request is ongoing.
        TEtelIconRequestType iIconReqTypeOngoing;
		// Icon data
        TDes8* iIconData;
        // Record number from which to read in NAA
        TUint8 iRecordNumber;
        // Record data from NAA
        TBuf8<KMaxRecordDataSize> iRecordData;
        // Number of instance in record
        TInt iImageInstanceNoInRecord;
    };              

#endif // CSATICONTSY_H

            
// End of File
