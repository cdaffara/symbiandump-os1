/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  TJ2kStreamReader class is to implement the
*                MJ2kPacketHeaderReader interface for reading the
*                packet header and packet body from the stream
*                buffer provided by ICL framework.
*
*/


#ifndef __JP2KSTREAMREADER_H__
#define __JP2KSTREAMREADER_H__

//  INCLUDES
#include <e32base.h>
#include "JP2KMarker.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* TJ2kStreamReader class is to implement the
* MJ2kPacketHeaderReader interface for reading the
* packet header and packet body from the stream
* buffer provided by ICL framework.
*
* JP2KCodec.dll
* @since 2.6
*/
class TJ2kStreamReader : public MJ2kPacketHeaderReader
    {
    public:  // Constructors and destructor
        
    public: // New functions
        
        /**
        * Update the codestream length read and offset of SOT marker
        * @since 2.6
        */
        void  UpdateMainHeader();

        /**
        * Update the codestream length read and length of data consumed
        * @since 2.6
        */
        void  UpdateTileHeader();

        /**
        * From MJ2kPacketHeaderReader
        * Try to consume the EPH marker if there is one
        * @since 2.6
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadEPHMarker();

        /**
        * From MJ2kPacketHeaderReader
        * Read a bit from the packet header stream
        * @since 2.6
        * @param aBit: get a bit from the packet header bitstream.
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadBit( TUint8& aBit );

        /**
        * From MJ2kPacketHeaderReader
        * Read some bits from the packet header stream
        * @since 2.6
        * @param aBit: get some bits from the packet header bitstream.
        * @param aBitLen: how many bits to read.
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadBits( TUint8& aBit, TUint8 aBitLen );

        /**
        * From MJ2kPacketHeaderReader
        * Read some bits from the packet header stream
        * @since 2.6
        * @param aBit: get some bits from the packet header bitstream.
        * @param aBitLen: how many bits to read.
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadBits( TUint32& aBit, TUint8 aBitLen );

        /**
        * From MJ2kPacketHeaderReader
        * Start reading from packet header stream
        * @since 2.6
        * @return TUint8: true if end of buffer.
        */
        TUint8 StartReadBit();

        /**
        * From MJ2kPacketHeaderReader
        * Align the stream to the next byte boundary if necessary
        * @since 2.6
        */
        void AlignReader();

        /**
        * Try to consume the SOP marker if there is one
        * @since 2.6
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadSOPMarker();

        /**
        * Try to consume the EPH marker after failure on previous try
        * @since 2.6
        * @return TUint8: true if end of buffer.
        */
        TUint8 TryReadEPHMarker();

        /**
        * Try to align to byte boundary after failure on previous try
        * @since 2.6
        * @return TUint8: true if end of buffer.
        */
        TUint8 TryReAlignReader();

    public: // Functions from base classes
       
    protected:  // New functions       

    protected:  // Functions from base classes

    private:

    public:     // Data

        // Iterator pointed to current position in the buffer
        const TUint8 *iPtr;            

        // Iterator pointed to the start of buffer
        TUint8 *iPtrStart;       

        // Iterator pointed to the end of buffer
        TUint8 *iPtrEnd;         

        // Iterator pointed to the start of marker
        TUint8 *iPtrStartMarker; 

        // Data consumed during decoding
        TUint32 iDataUsed;       

        // Length of codestream left to be read
        TUint32 iCSLength;       

        // Offset position of first SOT
        TUint32 iStartSOT;       

        // Offset position of next tile to be processed
        TUint32 iNewDataStart;   

        // Length of data to skip
        TUint32 iSkipLength;     

        // Current data in 8 bits
        TUint8 iData;           

        // Current bit position
        TUint8 iPos;            

        // Bit position of next byte
        TUint8 iNextPos;        

        // To indicate byte alignment is required
        TUint8 iAlign;          

        // To indicate EPH marker should be tried
        TUint8 iTryEPHMarker;   
    
    protected:  // Data

    private:    // Data

    public:     // Friend classes
    
    protected:  // Friend classes
       
    private:    // Friend classes

    };

#endif // __JP2KSTREAMREADER_H__

