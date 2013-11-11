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




#ifndef BTGPSNMEAPARSER_H
#define BTGPSNMEAPARSER_H

//  INCLUDES

#include <e32base.h>
#include <delimitedparser8.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  NMEA sentence parser. This class detects the validity of NMEA sentence. It provides
*  functions to get different field value and field data.
*  
*/
class TBTGPSNmeaParser: public TDelimitedParserBase8
    {
    public: 

        /**
        * Construction.
        */
        TBTGPSNmeaParser();

        /**
        * Set NMEA sentence to parser. This function will check sentence validity.
		* @param aNmea Reference to nmea sentences.
        */
        void SetNmeaSentence(const TDesC8& aNmea);
        
        /**
        * Get NMEA Sentence
        */
        const TDesC8& NmeaSentence() const;

        /**
        * Check if the NMEA sentece is valid
		* @return ETrue if the sentence is a valid NMEA sentence.
        */
        TBool IsValid() const;

        /**
        * Get NMEA message Id
		* @return The NMEA sentence message id.
        */
        TInt MessageId() const;

        /**
        * Get the field block
		* @param aFieldId Field id
		* @param aField On return contains the field string.
        * @return KErrNone if the field if found. Otherwise system
        *                  wide error code
        */
        TInt GetFieldBlock(TInt aFieldId, TPtrC8& aField) const;

        /**
        * Check if filed is null field
		* @param aFieldId Field id.
		* @return ETrue if the field is NULL field.
        */
        TBool IsNullField(TInt aFieldId) const;
        
        /**
        * Get field data.
		* @param aFieldId Field id
		* @param aData On return contain the field value
		* @return KErrNone if the field is found and contain valid information, 
		*         otherwise system wide error code.
        */
        template <class T> 
        TInt GetFieldData(TInt aFieldId, T& aData) const;

        /**
        * Get next field data. This is iteration function that will get all field
		* data
		* @param aData On return contains next valid field value.
		* @return KErrNone if next field data is retrieved. KErrNotFound if end of 
		*         NMEA sentence is reached.
        */
        template <class T>
        TInt NextFieldData(T& aData) const;

    private:
        /**
        * Getmessage id
        */
        void GetMessageId();
        
        /**
        * Check validity of NMEA sentence
        */
        TBool CheckValidity() const;

        /**
        * Remove '$', Checksum delimiter and check sum field
        */
        const TPtrC8 TrimSentence() const;

        /**
        * Verify checksum
        */
        TBool VerifyChecksum(const TDesC8& aSentence) const;

        /**
        * Convert char to nibble
        */
        TUint8 CharToNibble(const TUint8 aChar) const;


    private:
        //NMEA sentence
        TPtrC8 iNmea;

        //Message id. //ENmeaNull means message not valid
        TInt iId;

    };
#endif
// End of File

