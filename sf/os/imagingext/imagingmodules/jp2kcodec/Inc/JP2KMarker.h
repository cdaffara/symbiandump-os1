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
* Description:  Collection of structs used to gather the various
*                marker related information in the JP2 image file.
*                TxxxMarker must not exist on the stack, it must
*                be the data member of the classes derived from CBase.
*
*/


#ifndef __JP2KMARKER_H__
#define __JP2KMARKER_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

/**
 * struct TSizMarker
 * Image and tile size (SIZ)
 * @since 2.6
 */
struct TSizMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TSizMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TSizMarker();

    TUint32 iXsiz;
    TUint32 iYsiz;
    TUint32 iXOsiz;
    TUint32 iYOsiz;
    TUint32 iXTsiz;
    TUint32 iYTsiz;
    TUint32 iXTOsiz;
    TUint32 iYTOsiz;
    TUint16 iRsiz;
    TUint16 iCsiz;
    RArray<TUint> iSsiz;
    RArray<TUint> iXRsiz;
    RArray<TUint> iYRsiz;
    };

/**
 * struct TSotMarker
 * Start of tile-part (SOT)
 * @since 2.6
 */
struct TSotMarker
    {
    TUint32 iPsot;
    TUint16 iIsot;
    TUint8  iTPsot;
    TUint8  iTNsot;
    };

/**
 * struct TCODMarker
 * Coding style default (COD)
 * @since 2.6
 */
struct TCODMarker
    {
    /**
    * Constructor
    * @since 2.6
    */
    TCODMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TCODMarker();

    TUint8  iScod;
    TUint8  iProgressionOrder;
    TUint8  iColorTransformation;
    TUint8  iNumOfLevels;
    TUint8  iCodeBlockStyle;
    TUint8  iWaveletTransformation;
    TUint16 iNumOfLayers;
    TSize   iCodeBlockSiz;
    HBufC8 *iPrecinctSiz;
    };

/**
 * struct TCOCMarker
 * Coding style component (COC)
 * @since 2.6
 */
struct TCOCMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TCOCMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TCOCMarker();

    TUint16 iCcoc;
    TUint8  iScoc;
    TUint8  iNumOfLevels;
    TUint8  iCodeBlockStyle;
    TUint8  iWaveletTransformation;
    TSize   iCodeBlockSiz;
    HBufC8 *iPrecinctSiz;
    };

/**
 * struct TQCDMarker
 * Quantization default (QCD)
 * @since 2.6
 */
struct TQCDMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TQCDMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TQCDMarker();

    TUint8   iSqcd;
    HBufC8  *iExponent;
    HBufC16 *iMantissa;
    };

/**
 * struct TQCCMarker
 * Quantization component (QCC)
 * @since 2.6
 */
struct TQCCMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TQCCMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TQCCMarker();

    TUint16  iCqcc;
    TUint8   iSqcc;
    HBufC8  *iExponent;
    HBufC16 *iMantissa;
    };

/**
 * struct TRGNMarker
 * Region of interest (RGN)
 * @since 2.6
 */
struct TRGNMarker
    {
    TUint16 iCrgn;
    TUint8  iSrgn;
    TUint8  iSPrgn;
    };

/**
 * struct TPOCMarker
 * Progression order change (POC)
 * @since 2.6
 */
struct TPOCMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TPOCMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TPOCMarker();

    RArray<TUint> iRSpoc;
    RArray<TUint> iCSpoc;
    RArray<TUint> iLYEpoc;
    RArray<TUint> iREpoc;
    RArray<TUint> iCEpoc;
    RArray<TUint> iPpoc;
    };

/**
 * struct TPPMMarker
 * Packed packet headers, main header (PPM)
 * @since 2.6
 */
struct TPPMMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TPPMMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TPPMMarker();

    TUint8  iZppm;
    TUint32 iRemainder;
    TUint32 iNppm;
    HBufC8 *iIppm;
    };

/**
 * struct TTLMMarker
 * Tile-part lengths (TLM)
 * @since 2.6
 */
struct TTLMMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TTLMMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TTLMMarker();

    TUint8 iZtlm;
    TUint8 iStlm;
    RArray<TUint> iTtlm;
    RArray<TUint> iPtlm;
    };

/**
 * struct TPLMMarker
 * Packet length, main header (PLM)
 * @since 2.6
 */
struct TPLMMarker
    {
    /**
    * Constructor
    * @since 2.6
    */
    TPLMMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TPLMMarker();

    TUint8  iZplm;
    TUint8  iNplm;
    HBufC8 *iIplm;
    };

/**
 * struct TPPTMarker
 * Packed packet headers, tile-part header (PPT)
 * @since 2.6
 */
struct TPPTMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TPPTMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TPPTMarker();

    TUint8  iZppt;
    HBufC8 *iIppt;
    };

/**
 * struct TPLTMarker
 * Packet length, tile-part header (PLT)
 * @since 2.6
 */
struct TPLTMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TPLTMarker();

    /**
    * Destructor
    * @since 2.6
    */
   ~TPLTMarker();

    TUint8 iZplt;
    RArray<TUint> iIplt;
    };

/**
 * struct TCRGMarker
 * Component registration (CRG)
 * @since 2.6
 */
struct TCRGMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TCRGMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TCRGMarker();

    RArray<TPoint> iXYcrg;
    };

/**
 * struct TCOMMarker
 * Comment (COM)
 * @since 2.6
 */
struct TCOMMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TCOMMarker();
    /**
    * Destructor
    * @since 2.6
    */
    ~TCOMMarker();

    TUint16 iRemainder;
    TUint16 iRcom;
    HBufC8 *iCcom;
    };

/**
 * struct TMainMarker
 * Main header
 * @since 2.6
 */
struct TMainMarker
    {

    /**
    * Constructor
    * @since 2.6
    */
    TMainMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TMainMarker();

    TCODMarker iCod;
    TQCDMarker iQcd;
    RPointerArray<TCOCMarker> iCoc;
    RPointerArray<TQCCMarker> iQcc;
    RPointerArray<TRGNMarker> iRgn;
    TPOCMarker               *iPoc;
    RPointerArray<TPPMMarker> iPpm;
    RPointerArray<TTLMMarker> iTlm;
    RPointerArray<TPLMMarker> iPlm;
    TCRGMarker               *iCrg;
    RPointerArray<TCOMMarker> iCom;
    };

/**
 * struct TTileMarker
 * Tile-part header
 * @since 2.6
 */
struct TTileMarker
    {
    /**
    * Constructor
    * @since 2.6
    */
    TTileMarker();

    /**
    * Destructor
    * @since 2.6
    */
    ~TTileMarker();

    TCODMarker               *iCod;
    TQCDMarker               *iQcd;
    RPointerArray<TCOCMarker> iCoc;
    RPointerArray<TQCCMarker> iQcc;
    RPointerArray<TRGNMarker> iRgn;
    TPOCMarker               *iPoc;
    RPointerArray<TPPTMarker> iPpt;
    RPointerArray<TPLTMarker> iPlt;
    RPointerArray<TCOMMarker> iCom;
    };


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * MJ2kPacketHeaderReader defines the interface 
 * for reading the packet header from streams.
 * @since 2.6
 */
class MJ2kPacketHeaderReader
    {
    public:  // Constructors and destructor
        
    public: // New functions
        
        /**
        * Try to consume the EPH marker if there is one
        * @since 2.6
        * @return TUint8: true if end of buffer.
        */
        virtual TUint8 ReadEPHMarker() = 0;

        /**
        * Read a bit from the packet header stream
        * @since 2.6
        * @param aBit: get a bit from the packet header bitstream.
        * @return TUint8: true if end of buffer.
        */
        virtual TUint8 ReadBit( TUint8& aBit ) = 0;

        /**
        * Read some bits from the packet header stream
        * @since 2.6
        * @param aBit: get some bits from the packet header bitstream.
        * @param aBitLen: how many bits to read.
        * @return TUint8: true if end of buffer.
        */
        virtual TUint8 ReadBits( TUint8& aBit, TUint8 aBitLen ) = 0;

        /**
        * Read some bits from the packet header stream
        * @since 2.6
        * @param aBit: get some bits from the packet header bitstream.
        * @param aBitLen: how many bits to read.
        * @return TUint8: true if end of buffer.
        */
        virtual TUint8 ReadBits( TUint32& aBit, TUint8 aBitLen ) = 0;

        /**
        * Start reading from packet header stream
        * @since 2.6
        * @return TUint8: true if end of buffer.
        */
        virtual TUint8 StartReadBit() = 0;

        /**
        * Align the stream to the next byte boundary if necessary
        * @since 2.6
        */
        virtual void   AlignReader() = 0;

    public: // Functions from base classes
       
    protected:  // New functions        

    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data

    public:     // Friend classes
      
    protected:  // Friend classes
       
    private:    // Friend classes
      
    };

#endif // __JP2KMARKER_H__
