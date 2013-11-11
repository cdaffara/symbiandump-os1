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
* Description:  CJ2kImageInfo class used to collect image related
*                information such as Main Header, SIZ marker and
*                list of tiles.
*
*/


#ifndef __JP2KIMAGEINFO_H__
#define __JP2KIMAGEINFO_H__

//  INCLUDES
#include "JP2KMarker.h"
#include "JP2KUtils.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CJ2kTileInfo;

// CLASS DECLARATION
/**
 * CJ2kImageInfo class used to collect image related
 * information such as Main Header, SIZ marker,
 * List of tiles, and etc. It's also implement the
 * MJ2kPacketHeaderReader interface for reading the
 * packet header from PPM marker.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class CJ2kImageInfo : public CBase, public MJ2kPacketHeaderReader
{
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CJ2kImageInfo();

        /**
        * Destructor.
        */
        virtual ~CJ2kImageInfo();
    
    public: // New functions
        
        /**
        * Verify and append COC to the main header
        * @since 2.6
        * @param aMarker: pointer to TCOCMarker.
        */
        void AppendCOCL( TCOCMarker *aMarker );

        /**
        * Verify and append QCC to the main header
        * @since 2.6
        * @param aMarker: pointer to TQCCMarker.
        */
        void AppendQCCL( TQCCMarker *aMarker );

        /**
        * Verify and append RGN to the main header
        * @since 2.6
        * @param aMarker: pointer to TRGNMarker.
        */
        void AppendRGNL( TRGNMarker *aMarker );

        /**
        * Verify and append POC to the main header
        * @since 2.6
        * @param aMarker: pointer to TPOCMarker.
        */
        void AppendPOCL( TPOCMarker *aMarker );

        /**
        * Verify and append CRG to the main header
        * @since 2.6
        * @param aMarker: pointer to TCRGMarker.
        */
        void AppendCRGL( TCRGMarker *aMarker );

        /**
        * Verify and append COM to the main header
        * @since 2.6
        * @param aMarker: pointer to TCOMMarker.
        */
        void AppendCOML( const TCOMMarker *aMarker );

        /**
        * Retrieve the right Coding Style Marker
        * @since 2.6
        * @param aCod: a reference to TCODMarker pointer.
        * @param aCoc: a reference to TCOCMarker pointer.
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponentIndex: the index of component.
        */
        void GetCodingStyle( TCODMarker*& aCod, TCOCMarker*& aCoc,
                             const CJ2kTileInfo& aTile, TUint16 aComponentIndex );

        /**
        * Retrieve the right Quantization Marker
        * @since 2.6
        * @param aQcd: a reference to TQCDMarker pointer.
        * @param aQcc: a reference to TQCCMarker pointer.
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponentIndex: the index of component.
        */
        void GetQuantization( TQCDMarker*& aQcd, TQCCMarker*& aQcc,
                              const CJ2kTileInfo& aTile, TUint16 aComponentIndex );

        /**
        * Retrieve the right Region of Interest Marker
        * @since 2.6
        * @param aRgn: a reference to TRGNMarker pointer.
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponentIndex: the index of component.
        */
        void GetRegion( TRGNMarker*& aRgn, const CJ2kTileInfo& aTile, TUint16 aComponentIndex ) const;

        /**
        * Retrieve the tile length field from TLM marker
        * @since 2.6
        * @param aSotMarker: a reference to TSotMarker.
        */
        void GetFromTLM( TSotMarker& aSotMarker ) const;

        /**
        * Set up to read the packet header from the PPM marker
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        */
        void UsePPM( CJ2kTileInfo& aTile );

        /**
        * Re-arrange the PPM and PLM buffers
        * @since 2.6
        */
        void DoCompactMainHeaderL();

        /**
        * Get total number of tiles in horizontal position
        * @since 2.6
        * @return TUint16: number of tiles in horizontal position.
        */
        TUint16 NumOfHorizTilesL() const;

        /**
        * Get total number of tiles in vertical position
        * @since 2.6
        * @return TUint16: number of tiles in vertical position.
        */
        TUint16 NumOfVertTilesL() const;

        /**
        * Get total number of components
        * @since 2.6
        * @return TUint16: number of components.
        */
        TUint16 NumOfComponents() const;

        /**
        * Get the bit depth of a component
        * @since 2.6
        * @param aIndex: the component index.
        * @return TUint8: bit depth of a component.
        */
        TUint8 DepthOfComponent( TUint16 aIndex ) const;

        /**
        * Get the sign bit of a component
        * @since 2.6
        * @param aIndex: the component index.
        * @return TUint8: sign bit of a component.
        */
        TUint8 SignOfComponent( TUint16 aIndex ) const;

        /**
        * Get the SIZ marker
        * @since 2.6
        * @return TSizMarker&: a reference to TSizMarker object.
        */
        const TSizMarker& SizMarker() const;

        /**
        * Get the Main Header marker
        * @since 2.6
        * @return TMainMarker&: a reference to TMainMarker object.
        */
        const TMainMarker& MainMarker() const;

        /**
        * Get the maximum block size
        * @since 2.6
        * @return TSize&: a reference to the maximum size.
        */
        const TSize& MaxBlockSize() const;

        /**
        * Get the cropped area
        * @since 2.6
        * @return TRect&: a reference to the cropped area.
        */
        const TRect& CropArea() const;
        
        /**
        * Get the total number of tiles in the tile list
        * @since 2.6
        * @return TUint16: number of tiles in the tile list.
        */
        TUint16 TileCount() const;

        /**
        * Get the tile at specific location
        * @since 2.6
        * @param aIndex: the location.
        * @return CJ2kTileInfo&: a reference to CJ2kTileInfo object.
        */
        const CJ2kTileInfo& TileAt( TUint16 aIndex ) const;

        /**
        * Append tile into the tile list
        * @since 2.6
        * @param aTile: the pointer to CJ2kTileInfo object.
        */
        void Append( CJ2kTileInfo *aTile );

        /**
        * Remove a tile at specific location
        * @since 2.6
        * @param aIndex: the location.
        */
        void Remove( TUint16 aIndex );

        /**
        * Get the mask of a tile at specific location
        * @since 2.6
        * @param aIndex: the location.
        * @return TUint8: the mask of the tile.
        */
        TUint8 TileMaskAt( TUint16 aIndex ) const;

        /**
        * Set the mask of a tile at specific location
        * @since 2.6
        * @param aIndex: the location.
        * @param aTileMask: the mask.
        */
        void SetTileMask( TUint16 aIndex, TUint8 aTileMask );

        /**
        * Set the resolution level to drop
        * @since 2.6
        * @param aDrop: the resolution level to drop.
        */
        void SetLevelDrop( TUint8 aDrop );

        /**
        * Set the extra resolution level to drop
        * @since 2.6
        * @param aDrop: the extra levels to drop.
        */
        void SetExtraLevelDrop( TUint8 aDrop );

        /**
        * Get the component to drop
        * @since 2.6
        * @return TUint8: the component to drop.
        */
        TUint8 ComponentDrop() const;

        /**
        * Get the resolution level to drop
        * @since 2.6
        * @return TUint8: the resolution level to drop.
        */
        TUint8 LevelDrop() const;

        /**
        * Get the extra resolution level to drop
        * @since 2.6
        * @return TUint8: the extra resolution level to drop.
        */
        TUint8 ExtraLevelDrop() const;

        /**
        * Get the crop
        * @since 2.6
        * @return TUint8: the crop flag.
        */
        TUint8 Crop() const;

        /**
        * Set the last tile part processed
        * @since 2.6
        * @param aLastTilePart: the last tile part processed.
        */
        void SetLastTilePartProcessed( TUint16 aLastTilePart );

        /**
        * Increment the counter for tracking the last tile part processed
        * @since 2.6
        */
        void IncrementLastTilePartProcessed();

        /**
        * Is packet header should be read from PPM marker
        * @since 2.6
        * @return TUint8: true if PPM marker exists.
        */
        TUint8 IsPPM() const;

        /**
        * Reset packet header pointer to the end of the PPM marker
        * @since 2.6
        */
        void ResetPPM();

    public: // Functions from base classes

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
               
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data
        
        // Structure to hold the information on reading packet header
        struct TPPMStream
            {
            TUint32 iPtr;      // Current byte position in buffer
            TUint32 iPtrEnd;   // End of byte position in buffer
            TUint8  iData;     // Current data in 8 bits
            TUint8  iPos;      // Current bit position
            TUint8  iNextPos;  // Bit position of next byte
            };

    private:    // Data

        // SIZ marker
        TSizMarker iSizMarker;           

        // Main Header
        TMainMarker iMainMarker;     
        
        // Current tile in progress
        CJ2kTileInfo *iTile;                

        // Current tile mask
        TUint8 iTileMask;            
    
        // Max block size
        TSize iMaxBlockSize;          

        // Cropped area
        TRect iCropArea;     
        
        // Component to drop
        TUint8 iComponentDrop;         

        // Resolution level to drop
        TUint8 iLevelDrop;             

        // Resolution drop  
        TUint8 iExtraLevelDrop;

        // Is cropped
        TUint8 iCrop;                  

        // Last tile part processed
        TInt16 iLastTilePartProcessed; 

        // Control the reading of packet header
        TPPMStream *iPpm;               

        // Pointer to PPM in Main Header
        HBufC8 *iPpmBuffer;         

    public:     // Friend classes
      
    protected:  // Friend classes
        
    private:    // Friend classes        
    };

// For inliners.
#include "JP2KImageInfo.inl"

#endif // __JP2KIMAGEINFO_H__

