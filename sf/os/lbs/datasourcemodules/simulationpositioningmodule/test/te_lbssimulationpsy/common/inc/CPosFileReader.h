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
*
*/



#ifndef CPOSFILEREADER_H
#define CPOSFILEREADER_H

//  INCLUDES
#include <s32file.h>
#include <badesca.h>
// CONSTANTS
_LIT(KFileBasePathFmt, "\\system\\data\\%S");

const TInt KPosFileNameMaxLength = 30;
const TInt KPosFileBasePathMaxLength = 46;
const TInt KMaxSentenceLength = 80; 

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Used to access files and read from files.
 *
 */
class CPosFileReader : public CBase
    {
	public:

        /**
        * Two-phased constructor.
        */
        static CPosFileReader* NewL();

        /**
        * Two-phased constructor.
        */
        static CPosFileReader* NewLC();

        /**
        * Destructor.
        */
        virtual ~CPosFileReader();    
       
    public:     // New functions

        /** 
        * 
        */
        TInt PrepareForRead(const TDesC& aFileName);        

        /**
        * Reads one line from Serial port (asynchroneously)
        *
        * @param aDes     A buffer to store the incoming data.
        */
        void ReadL(            
            /* OUT */   CDesC8Array*& aArray,
                        TInt aItemsToRead,
                        TBool aNmeaMode
            );                

        /**
        * Cancels read.
        */
        void CloseResources();    
        //void Cancel();

    private:

        /**
        * C++ default constructor.
        */
        CPosFileReader();

        /**
        * EPOC constructor.
        */
        void ConstructL();

        // Prohibit copy constructor
        CPosFileReader( const CPosFileReader& );
        // Prohibit assigment operator
        CPosFileReader& operator= ( const CPosFileReader& );

    private:

        /**
        * Opens a file. If the file does not exist, the file is created.
        *
        * @param aFileName  The name of the file to be opened.
        *
        * @param aFileMode file mode to open the file in ( TFileMode )
        *
        * @return error code
        */
        TInt Open(const TDesC& aFilename, const TInt aFileMode);

        /**
        * Updates the member variable iFileName.
        *        
        */
        void UpdateFileName();
    
    private:    // Data
        RFs                         iFsSession;  
        RFile                       iFile;
        TBuf<KPosFileNameMaxLength> iFileName;        
        RFileReadStream             iStream;
    };

#endif      // CPOSFILEREADER_H

// End of File
