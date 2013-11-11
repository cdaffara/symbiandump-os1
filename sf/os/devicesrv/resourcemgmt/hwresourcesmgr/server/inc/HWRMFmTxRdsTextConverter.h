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



#ifndef HWRMFMTXRDSTEXTCONVERTER_H
#define HWRMFMTXRDSTEXTCONVERTER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "HWRMFmTxData.h"

// CONSTANTS

/** 
* The conversion table supports the following Unicode latin ranges:
*   Basic Latin 0000–007F
*   Latin-1 Supplement 0080–00FF
*   Latin Extended-A 0100–017F
*   Latin Extended-B 0180–024F
*/
const TInt KConvertFromTableLength = 0x250;

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

/**
* Utility for converting between Unicode and RDS text
* HWRM Common Data upkeeps the instance.
*/
class CHWRMFmTxRdsTextConverter : public CBase
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        */
        static CHWRMFmTxRdsTextConverter* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CHWRMFmTxRdsTextConverter();
        
    public: // New functions
    
		/**
        * Converts from Unicode to RDS text
        */ 	
    	void ConvertFromUnicodeL(const TDesC& aInput, TDes8& aOutput);

		/**
        * Returns the RDS character used, to replace 
        * unsupported Unicode characters.
        */ 	
		inline TText8 UnsupportedCharReplacement() const;	

		/**
        * Sets the RDS character used, to replace 
        * unsupported Unicode characters.
        */ 	    	
    	void SetUnsupportedCharReplacement(const TText8 aChar);

    private:
    
        /**
        * Helper for ConvertFromUnicodeL. Returns whether aChar is within RDS character range.
        */
		TBool SupportedCharacter( TUint8 aChar );
    
        /**
        * C++ default constructor.
        */
        CHWRMFmTxRdsTextConverter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();       

    private:

		// This symbol is used to replace unsupported Unicode characters
		TText8 iUnsupportedCharReplacement;

		// Conversion table
        RFs iFileServer;
        RFile iConvertFromUnicodeFile;
        TBuf8<KConvertFromTableLength> iConvertFromUnicodeTable;
    };
    
#include "HWRMFmTxRdsTextConverter.inl"    

#endif  // HWRMFMTXRDSTEXTCONVERTER_H
            
// End of File
