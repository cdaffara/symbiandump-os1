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
* Name        : TTlv.h
* Part of     : Series 60 / utility
* Based on 3GPP TS 11.14 V8.8.0 
* Version     : 1.0
*
*/



//  INCLUDES
#ifndef TTLV_H
#define TTLV_H

#include <cstktsy/bertlv_defs.h>			// BerTlv constants

//  CONSTANTS 

    //none

//  MACROS  

    //none

//  DATA TYPES  

    //none

//  EXTERNAL DATA STRUCTURES  

    //none

//  FUNCTION PROTOTYPES  

    //none

//  FORWARD DECLARATIONS

    //none

// DESCRIPTION
// TLV type: Tag Length Value(s)
// Generic BER-TLV constructor class, can be used to create TLVs
// You do not need to worry about lenghts; Example usage: 
//                 TTlv tlv;
//                 tlv.Begin( KxxBerTag);  <--- top level tag
//                 tlv.AddTag( KxxxTag )   <--- simple tags
//                 tlv.AddByte(...)        <--- add values for this tag
//                 tlv.AddByte(...)
//                 .... add more simple tags and values..
//                 result = tlv.End();     <--- ends construction and returns 
//                                              the BER-TLV

class TTlvBase
    {
    public: // Constructor

        /**
        * Constructor
        * @param aData: data to be inserted
        */
        IMPORT_C TTlvBase( TDes8& aData );

    public:

        /**
        * Begin TLV construction
        * @param aTag:  BER-TLV tag
        * @return None
        */
        IMPORT_C void Begin( TUint8 aTag );

        /**
        * Start new simple TLV
        * @param aTag:  simple tag
        * @return None
        */
        IMPORT_C void AddTag( TUint8 aTag );

        /**
        * Add byte value to simple TLV
        * @param aValue: byte value
        * @return None
        */
        IMPORT_C void AddByte( TUint8 aValue );

        /**
        * Add descriptor value to simple TLV
        * @param aValue: descriptor value
        * @return None
        */
        IMPORT_C void AddData( const TDesC8& aValue );

        /**
        * End TLV construction
        * @param None
        * @return descriptor containing BER-TLV
        */
        IMPORT_C const TDesC8& End();

        /**
        * Return data, calculated without
        * top level tag. 
        * @param None
        * @return descriptor containing BER-TLV
        */
        IMPORT_C const TDesC8& GetDataWithoutTopLevelTag();

    protected: // Data
        TInt       iLenIndex;
        TDes8&     iData;
    };

// DESCRIPTION
// Type for Tag Length Value buffer.
// Define TLV that is smaller than default max size of 255

template <TInt S>
class TTlvBuf : public TTlvBase
    {
    public: // Constructor

        /**
        * Default constructor
        */
        TTlvBuf():TTlvBase(iBuffer)
        	{
        	}
    private: // Data

        TBuf8<S>   iBuffer;
    };

// DESCRIPTION
// Type for Tag Length Value buffer
// default TLV with size of 255

class TTlv : public TTlvBase
    {
    public:
    
        /**
        * Default constructor
        */        
        TTlv() : TTlvBase(iBuffer)
        	{
        	}
 
    private:
        // TLV must fit in APDU limits (255 bytes)
        // therefore the maximum possible TLV size is 255
        TBuf8<KTlvMaxSize>   iBuffer;
    };

#endif // TTLV_H

// End of File
