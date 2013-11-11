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
* Description:  CJ2kTileInfo class used to collect tile related
*                information such as Tile Part Header, SOT marker and
*                list of components.
*
*/


#ifndef __JP2KTILEINFO_H__
#define __JP2KTILEINFO_H__

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "JP2KMarker.h"

// CONSTANTS

// MACROS

// DATA TYPES
struct TPOCMarker;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class  CJ2kImageInfo;
class  TJ2kStreamReader;
class  CJ2kComponentInfo;

// CLASS DECLARATION

/**
*  CJ2kTileInfo class used to collect tile related
*  information such as Tile Part Header, SOT marker,
*  List of components, and etc. It's also implement the
*  MJ2kPacketHeaderReader interface for reading the
*  packet header from PPT marker. It contains the data
*  to control where to resume the parsing of the bitstream.
*
*  JP2KCodec.dll
*  @since 2.6
*/
class CJ2kTileInfo : public CBase, public MJ2kPacketHeaderReader
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CJ2kTileInfo* NewLC( CJ2kImageInfo& aImageInfo, TJ2kStreamReader& aReader );
        
        /**
        * Destructor.
        */
        virtual ~CJ2kTileInfo();

    public: // New functions
        
        /**
        * Initialize the components in the tile
        * @since 2.6
        */
        void InitializeL();

        /**
        * Verify and append COD to the tile part header
        * @since 2.6
        * @param aMarker: pointer to TCODMarker.
        * @param aLength: the marker length.
        */
        void AppendCOD( TCODMarker *aMarker, TUint32 aLength );

        /**
        * Verify and append COC to the tile part header
        * @since 2.6
        * @param aMarker: pointer to TCOCMarker.
        * @param aLength: the marker length.
        */
        void AppendCOCL( TCOCMarker *aMarker, TUint32 aLength );

        /**
        * Verify and append QCD to the tile part header
        * @since 2.6
        * @param aMarker: pointer to TQCDMarker.
        * @param aLength: the marker length.
        */
        void AppendQCD( TQCDMarker *aMarker, TUint32 aLength );

        /**
        * Verify and append QCC to the tile part header
        * @since 2.6
        * @param aMarker: pointer to TQCCMarker.
        * @param aLength: the marker length.
        */
        void AppendQCCL( TQCCMarker *aMarker, TUint32 aLength );

        /**
        * Verify and append RGN to the tile part header
        * @since 2.6
        * @param aMarker: pointer to TRGNMarker.
        * @param aLength: the marker length.
        */
        void AppendRGNL( TRGNMarker *aMarker, TUint32 aLength );

        /**
        * Verify and append POC to the tile part header
        * @since 2.6
        * @param aMarker: pointer to TPOCMarker.
        * @param aLength: the marker length.
        */
        void AppendPOCL( TPOCMarker *aMarker, TUint32 aLength );

        /**
        * Verify and append COM to the tile part header
        * @since 2.6
        * @param aMarker: pointer to TCOMMarker.
        * @param aLength: the marker length.
        */
        void AppendCOML( TCOMMarker *aMarker, TUint32 aLength );

        /**
        * Verify and append PPT to the tile part header
        * @since 2.6
        * @param aMarker: pointer to TPPTMarker.
        * @param aLength: the marker length.
        */
        void AppendPPTL( TPPTMarker *aMarker, TUint32 aLength );

        /**
        * Verify and append PLT to the tile part header
        * @since 2.6
        * @param aMarker: pointer to TPLTMarker.
        * @param aLength: the marker length.
        */
        void AppendPLTL( TPLTMarker *aMarker, TUint32 aLength );

        /**
        * Parsing the bitstream data based on progression order
        * @since 2.6
        * @param aBool: to check next SOT/EOC marker if true.
        */
        void ReadBitStreamL( TUint8 aBool = EFalse );

        /**
        * Get the color transformation
        * @since 2.6
        * @return TUint8: color transformation from tile COD or main COD.
        */
        TUint8 ColorTransformation() const;

        /**
        * Get the progression order
        * @since 2.6
        * @return TUint8: progression order from tile COD or main COD.
        */
        TUint8 ProgressionOrder() const;

        /**
        * Get the number of layers
        * @since 2.6
        * @return TUint16: number of layers from tile COD or main COD.
        */
        TUint16 NumOfLayers() const;

        /**
        * Get the number of resolution level
        * @since 2.6
        * @return TUint8: max number of resolution level from each component.
        */
        TUint8  NumOfLevels() const;

        /**
        * Get the minimum grid among the components in the tile
        * @since 2.6
        * @param aGrid: get the minimum grid size.
        */
        void GetMinGrid( TSize &aGrid ) const;

        /**
        * Set up to read the packet header from the PPT marker
        * @since 2.6
        */
        void UsePPTL();

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
        * Release unused markers in tile part header
        * @since 2.6
        */
        void DoReleaseUnusedMarkers();

        /**
        * Set the tile length
        * @since 2.6
        * @param aLength: the tile length to set.
        */
        void SetTileLength( TUint32 aLength );
        
        /**
        * Get the tile length
        * @since 2.6
        * @return TUint32&: reference to the tile length.
        */
        TUint32& TileLength();

        /**
        * Get the tile canvas
        * @since 2.6
        * @return TRect&: reference to the canvas of the tile.
        */
        const TRect& TileCanvas() const;

        /**
        * Get the number of components
        * @since 2.6
        * @return TUint16: the number of components.
        */
        TUint16 NumOfComponents() const;

        /**
        * Get the end of layer to process
        * @since 2.6
        * @return TUint16: the last layer to process.
        */
        TUint16 NumOfLayersPOC() const;

        /**
        * Get the end of level to process
        * @since 2.6
        * @return TUint8: the last level to process.
        */
        TUint8 NumOfLevelsPOC() const;

        /**
        * Get the component at specific location
        * @since 2.6
        * @param aIndex: component index.
        * @return CJ2kComponentInfo&: a specific component.
        */
        const CJ2kComponentInfo& ComponentAt( TUint16 aIndex ) const;

        /**
        * Get the last layer processed
        * @since 2.6
        * @return TUint16: the last layer processed.
        */
        TUint16 LastLayerProcessed() const;

        /**
        * Get the last resolution level processed
        * @since 2.6
        * @return TUint8: the last resolution level processed.
        */
        TUint8 LastLevelProcessed() const;

        /**
        * Get the last component processed
        * @since 2.6
        * @return TUint16: the last component processed.
        */
        TUint16 LastComponentProcessed() const;

        /**
        * Get the last position processed in vertical direction
        * @since 2.6
        * @return TInt: the last vertical position processed.
        */
        TInt LastN1Processed() const;

        /**
        * Get the last position processed in horizontal direction
        * @since 2.6
        * @return TInt: the last horizontal position processed.
        */
        TInt LastN2Processed() const;

        /**
        * Increment the last layer processed
        * @since 2.6
        */
        void IncrementLastLayerProcessed();

        /**
        * Reset the last layer processed to 0
        * @since 2.6
        */
        void ResetLastLayerProcessed();

        /**
        * 
        * @since 2.6Get the reference to the image info
        * @return CJ2kImageInfo&: a reference to CJ2kImageInfo object.
        */
        const CJ2kImageInfo& ImageInfo() const;

        /**
        * Get the reference to the stream reader
        * @since 2.6
        * @return TJ2kStreamReader&: a reference to TJ2kStreamReader object.
        */
        const TJ2kStreamReader& StreamReader() const;

        /**
        * Set the SOT marker
        * @since 2.6
        * @param aSotMarker: the TSotMarker to set.
        */
        void SetSotMarker( const TSotMarker &aSotMarker );

        /**
        * Get the SOT marker
        * @since 2.6
        * @return TSotMarker&: a reference to TSotMarker object.
        */
        const TSotMarker& SotMarker() const;

        /**
        * Get the Tile Part Header
        * @since 2.6
        * @return TTileMarker&: a reference to TTileMarker object.
        */
        const TTileMarker& TileMarker() const;

        /**
        * Set the SOT/EOC check indicator
        * @since 2.6
        * @param aCheckMarker: true if SOT/EOC need to be checked.
        */
        void SetCheckMarker( TUint8 aCheckMarker );

        /**
        * Get the packet header reader
        * @since 2.6
        * @return MJ2kPacketHeaderReader&: a reference to MJ2kPacketHeaderReader object.
        */
        MJ2kPacketHeaderReader& PacketHeaderReader();

        /**
        * Set the packet header reader
        * @since 2.6
        * @param aReader: set the packet header reader to aReader.
        */
        void SetPacketHeaderReader( MJ2kPacketHeaderReader* aReader );

        /**
        * Is this the first tile part
        * @since 2.6
        * @return TUint8: true if this is the first tile part.
        */
        TUint8 IsFirstTilePart() const;

        /**
        * Is packet header should be read from PPT marker
        * @since 2.6
        * @return TUint8: true if tile contains PPT.
        */
        TUint8 IsPPT() const;

        /**
        * Is POC marker should be used for progression order
        * @since 2.6
        * @return TUint8: true if tile contains POC.
        */
        TUint8 IsPOC() const;

        /**
        * Is this the last tile part
        * @since 2.6
        * @return TUint8: true if this is the last tile part.
        */
        TUint8 IsLastTilePart() const;

        /**
        * Is SOT/EOC marker should be checked during progression 
        * @since 2.6
        * @return TUint8: true if SOT/EOC should be checked.
        */
        TUint8 IsCheckMarker() const;

        /**
        * Is RLCP/RPCL progression order used 
        * @since 2.6
        * @return TUint8: true if RLCP/RPCL progression order used.
        */
        TUint8 IsSpeedup() const;

    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CJ2kTileInfo( CJ2kImageInfo& aImageInfo, TJ2kStreamReader& aReader );

        /**
        * Parse the bitstream with LRCP progression order
        * @since 2.6
        * @return TUint8: true if next SOT or EOC marker is found.
        */
        TUint8 LRCPProgressionL();

        /**
        * Parse the bitstream with RLCP progression order
        * @since 2.6
        * @return TUint8: true if next SOT or EOC marker is found.
        */
        TUint8 RLCPProgressionL();

        /**
        * Parse the bitstream with RPCL progression order
        * @since 2.6
        * @return TUint8: true if next SOT or EOC marker is found.
        */
        TUint8 RPCLProgressionL();

        /**
        * Parse the bitstream with PCRL progression order
        * @since 2.6
        * @return TUint8: true if next SOT or EOC marker is found.
        */
        TUint8 PCRLProgressionL();

        /**
        * Parse the bitstream with CPRL progression order
        * @since 2.6
        * @return TUint8: true if next SOT or EOC marker is found.
        */
        TUint8 CPRLProgressionL();

    public:     // Data
    
    protected:  // Data

        // Progression order 
        enum TProgression
            {
            EProgression_L_R_C_P = 0,
            EProgression_R_L_C_P = 1,
            EProgression_R_P_C_L = 2,
            EProgression_P_C_R_L = 3,
            EProgression_C_P_R_L = 4
            };
        
        // Structure to hold the information on reading packet header
        struct TPPTStream
            {
            TUint32 iPtr;     // Current byte position in buffer
            TUint32 iPtrEnd;  // End of byte position in buffer
            TUint8 iData;     // Current data in 8 bits
            TUint8 iPos;      // Current bit position
            TUint8 iNextPos;  // Bit position of next byte
            };

    private:    // Data

        // SOT marker
        TSotMarker iSot;               

        // Tile Part Header
        TTileMarker iTileMarker;        

        // List of components
        RPointerArray<CJ2kComponentInfo> iComponentList; 

        // Canvas of the tile
        TRect iTileCanvas;        
    
        // Remaining tile length
        TUint32 iTileLength;        

        // Last layer processed
        TUint16 iLastLayer;         

        // Last component processed
        TUint16 iLastComponent;
        
        // Original starting component
        TUint16 iLastComponentOrig; 

        // End of layer to process
        TUint16 iNumOfLayersPOC;    

        // End of component to process
        TUint16 iNumOfComponentsPOC;

        // Last resolution level processed
        TUint8 iLastLevel;         

        // Original starting resolution level
        TUint8 iLastLevelOrig;     

        // End of resolution level to process
        TUint8 iNumOfLevelsPOC;    

        // Last POC processed
        TUint8 iLastPOC;           

        // To indicate not to load from iPOC
        TUint8 iDoLoadPOC;         

        // To indicate whether to check SOT/EOC marker
        TUint8 iCheckMarker; 

        // Pointer to the POC in Tile Part Header or Main Header
        TPOCMarker *iPOC;               

        // Last position processed in vertical direction
        TInt iLastN1;            

        // Last position processed in horizontal direction
        TInt iLastN2;            

        // Reference to the image info
        CJ2kImageInfo &iImageInfo;    

        // Reference to the stream reader (buffer provided by ICL framework)
        TJ2kStreamReader &iReader;       

        // Control the reading of packet header
        TPPTStream *iPpt;               

        // Pointed to PPT in Tile Part Header
        HBufC8 *iPptBuffer;         

        // Packet header reader
        MJ2kPacketHeaderReader *iPacketHeaderReader; 

    public:     // Friend classes
       
    protected:  // Friend classes
        
    private:    // Friend classes        

    };

// For inliners.
#include "JP2KTileInfo.inl"

#endif // __JP2KTILEINFO_H__
