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
* Name        : CTlv.h
* Part of     : Series 60 / utility
* Based on 3GPP TS 11.14 V8.8.0 
* Version     : 1.0
*
*/



//  INCLUDES
#ifndef CTLV_H
#define CTLV_H
#include <e32base.h>				// for class CBase
#include <cstktsy/BerTlv_defs.h>		    // Ber tlv related constants

// DESCRIPTION
// TLV Base class. methods common for CTlv and CBerTlv

class CTlvBase : public CBase
    {
    public:

        /**
        * Constructor
        */
        CTlvBase();

        /**
        * Get pointer to TLV data
        * @return Pointer to TLV data
        */
        IMPORT_C TPtrC8 Data() const;

        /**
        * Set TLV data
        * @param aData: Data to be inserted
        * @return None
        */
        IMPORT_C void SetData( TPtrC8 aData );

        /**
        * Get total size, including tag
        * @return size
        */
        IMPORT_C TInt GetSize() const;

        /**
        * Get tag part of TLV
        * @return tag value
        */
        IMPORT_C TUint8 GetTag() const;
                        
        /**
        * Returns true if comprehension flag is on
        * @return boolean
        */
        IMPORT_C TBool GetComprehensionRequired() const;

        /**
        * Get length part of TLV
        * @return length
        */
        IMPORT_C TUint8 GetLength () const;

        /**
        * Get value part of TLV
        * @return value
        */
        IMPORT_C TPtrC8 GetValue() const;

    protected: // Data

        TPtrC8 iData;
    };


// DESCRIPTION
// CTlv contains methods to read simple TLV data

class CTlv : public CTlvBase
    {
    public: // Constructor

        /**
        * Default constructor
        */
        IMPORT_C CTlv();

    public: // Methods

		/**
        * Returns short info defined by aType.
        * @param aType: wanted data type.
        * @return wanted data.
        */
        IMPORT_C TUint8 GetShortInfo( TTlvSpesificDataType aType );
		
        /**
        * Returns the Data defined by aType. Index and length of the 
        * data depends on data type.
        * @param aType: data type.
        * @return pointer to the data.
        */
        IMPORT_C TPtrC8 GetData( TTlvSpesificDataType aType );

		/**
        * Returns long info by aType.
        * @param aType: data type.
        * @return returned data
        */
        IMPORT_C TUint16 GetLongInfo( TTlvSpesificDataType aType );
    };


#endif // CTLV_H

// End of File
