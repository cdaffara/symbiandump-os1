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
* Description:  JP2KPacket class used to collect packet related
*                information such as tag tree and list of codeblocks.
*
*/


#ifndef __JP2KPACKET_H__
#define __JP2KPACKET_H__

//  INCLUDES
#include <e32base.h>
#include "JP2KEntropyDecoder.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CJ2kSubband;
class CJ2kTileInfo;
class CJ2kComponentInfo;
class CJ2kCodeBlock;

// CLASS DECLARATION

/**
 * JP2KPacket class used to collect packet related
 * information such as tag tree, list of codeblocks,
 * and data used to control where to resume the parsing
 * of the bitstream.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class CJ2kPacket : public CBase
{
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CJ2kPacket( TUint16 aLayer );
        
        /**
        * Destructor.
        */
        virtual ~CJ2kPacket();

    public: // New functions
        
        /**
        * Set the canvas of the packet
        * @since 2.6
        * @param aX: the x position.
        * @param aY: the y position.
        * @param aWidth the width.
        * @param aHeight: the height.
        */
        void SetPacketCanvas( TInt32 aX, TInt32 aY, TInt32 aWidth, TInt32 aHeight );

        /**
        * Set the number of blocks
        * @since 2.6
        * @param aCodeBlock: the size of the codeblock.
        */
        void SetNumOfBlocksL( TSize& aCodeBlock );

        /**
        * Build the inclusive informaiton of the packet
        * @since 2.6
        */
        void BuildInclusiveInfoL();

        /**
        * Build the codeblocks in the packet
        * @since 2.6
        * @param aX: the x position.
        * @param aY: the y position.
        * @param aCodeBlock: the size of the codeblock.
        */
        void BuildCodeBlocksL( TInt32 aX, TInt32 aY, TSize& aCodeBlock );

        /**
        * Read the packet header
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponent: a reference to CJ2kComponentInfo object.
        * @param aSubband: a reference to CJ2kSubband object.
        * @return TUint8: true if incomplete.
        */
        TUint8 ReadPacketHeaderL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent, CJ2kSubband& aSubband );

        /**
        * Read the packet body
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponent: a reference to CJ2kComponentInfo object.
        * @param aSubband: a reference to CJ2kSubband object.
        * @return TUint8: true if incomplete.
        */
        TUint8 ReadPacketBodyL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent, CJ2kSubband& aSubband );

        /**
        * Reset the internal flags
        * @since 2.6
        */
        void ResetInternalFlags();

        /**
        * Get the last codeblock processed
        * @since 2.6
        * @return TUint16: the last codeblock processed.
        */
        TUint16 LastCodeBlockProcessed() const;

        /**
        * Set the last codeblock processed
        * @since 2.6
        * @param aLastCodeBlock: the last codeblock processed.
        */
        void SetLastCodeBlockProcessed( TUint16 aLastCodeBlock );

        /**
        * Get number of blocks
        * @since 2.6
        * @return TUint16: number of codeblocks.
        */
        TUint16 NumOfBlocks() const;

        /**
        * Get number of blocks in horizontal direction
        * @since 2.6
        * @return TUint16: number of codeblock in X direction.
        */
        TUint16 NumOfBlocksX() const;

        /**
        * Get number of blocks in vertical direction
        * @since 2.6
        * @return TUint16: number of codeblock in Y direction.
        */
        TUint16 NumOfBlocksY() const;

        /**
        * Get the canvas of the packet
        * @since 2.6
        * @return TRect&: a reference to the packet's canvas.
        */
        const CJ2kCodeBlock& CodeBlockAt( TUint16 aIndex ) const;

        /**
        * Is packet body read underflow
        * @since 2.6
        * @return TUint8: true if packet body incomplete.
        */
        TUint8 IsBodyIncomplete() const;

    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:
        
        /**
        * Build the tag trees
        * @since 2.6
        */
        void BuildTagtreeL();

        /**
        * Decode the included tag tree
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aWidth: the width.
        * @param aHeight: the height.
        * @param aValue: a reference to the TUint16.
        * @return TUint8: true if incomplete.
        */
        TUint8 DecodeIncTagtree( CJ2kTileInfo& aTile, TUint32 aWidth, TUint32 aHeight, TUint16& aValue );

        /**
        * Decode the msb tag tree
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aWidth: the width.
        * @param aHeight: the height.
        * @param aValue: a reference to the TUint16.
        * @return TUint8: true if incomplete.
        */
        TUint8 DecodeMsbTagtree( CJ2kTileInfo& aTile, TUint32 aWidth, TUint32 aHeight, TUint16& aValue );

        /**
        * Backup the included tag tree
        * @since 2.6
        * @param aTagValue: a reference to the value descriptor pointer.
        * @param aTagState: a reference to the state descriptor pointer.
        */
        void BackupIncTagtreeL( HBufC16*& aTagValue, HBufC16*& aTagState );

        /**
        * Backup the msb tag tree
        * @since 2.6
        * @param aTagValue: a reference to the value descriptor pointer.
        * @param aTagState: a reference to the state descriptor pointer.
        */
        void BackupMsbTagtreeL( HBufC16*& aTagValue, HBufC16*& aTagState );

        /**
        * Restore the included tag tree
        * @since 2.6
        * @param aTagValue: a pointer to the value descriptor.
        * @param aTagState: a pointer to the state descriptor.
        */
        void RestoreIncTagtree( HBufC16 *aTagValue, HBufC16 *aTagState );

        /**
        * Restore the msb tag tree
        * @since 2.6
        * @param a
        * @param aTagValue: a pointer to the value descriptor.
        * @param aTagState: a pointer to the state descriptor.
        */
        void RestoreMsbTagtree( HBufC16 *aTagValue, HBufC16 *aTagState );

        /**
        * Release the temporary backup memory
        * @since 2.6
        * @param aTagValue: a reference to the include value descriptor pointer.
        * @param aTagState: a reference to the include state descriptor pointer.
        * @param aTagValue: a reference to the msb value descriptor pointer.
        * @param aTagState: a reference to the msb state descriptor pointer.
        */
        void FreeBackupTagtree( HBufC16*& aIncTagValue, HBufC16*& aIncTagState,
                                HBufC16*& aMsbTagValue, HBufC16*& aMsbTagState );

        /**
        * Is the layer has been included in the previous packet
        * @since 2.6
        * @param aLayer: the layer
        * @return TUint8: true if layer has been included.
        */
        TUint8 IsIncludedL( TUint16 aLayer );

        /**
        * Get the inclusive information at specific layer
        * @since 2.6
        * @param aLayer: the layer.
        * @return TUint8: inclusive information.
        */
        TUint8 IncludedAt( TUint16 aLayer ) const;

        /**
        * Set the inclusive information at specific layer
        * @since 2.6
        * @param aLayer: the layer.
        */
        void SetIncluded( TUint16 aLayer );

        /**
        * Is tag tree backup is required for underflow recovery
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if backup is needed.
        */
        TUint8 IsBackupNeeded( CJ2kTileInfo& aTile );

        /**
        * Start reading from packet header stream
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if end of buffer.
        */
        TUint8 StartReadBit( CJ2kTileInfo& aTile );

        /**
        * Read a bit from the packet header stream
        * @since 2.6
        * @param aBit: get a bit from the packet header bitstream.
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadBit( TUint8& aBit, CJ2kTileInfo& aTile );

        /**
        * Read some bits from the packet header stream
        * @since 2.6
        * @param aBit: get some bits from the packet header bitstream.
        * @param aBitLen: how many bits to read.
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadBits( TUint8& aBit, TUint8 aBitLen, CJ2kTileInfo& aTile );

        /**
        * Read some bits from the packet header stream
        * @since 2.6
        * @param aBit: get some bits from the packet header bitstream.
        * @param aBitLen: how many bits to read.
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadBits( TUint32& aBit, TUint8 aBitLen, CJ2kTileInfo& aTile );

        /**
        * Align the stream to the next byte boundary if necessary
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        */
        void AlignReader( CJ2kTileInfo& aTile );

        /**
        * Try to consume the SOP marker if there is one
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadSOPMarker( CJ2kTileInfo& aTile );

        /**
        * Try to consume the EPH marker if there is one
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadEPHMarker( CJ2kTileInfo& aTile );

        /**
        * Try to re-align stream to next byte boundary if necessary,
        * Try to consume the previous leftover EPH marker if there is one,
        * Try to consume the SOP marker if there is one
        * @since 2.6
        * @param aComponent: a reference to CJ2kComponentInfo object.
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aSOP: true if to try SOP marker.
        * @return TUint8: true if end of buffer.
        */
        TUint8 ReadSOP_EPHMarker( CJ2kComponentInfo& aComponent, CJ2kTileInfo& aTile, TBool aSOP = ETrue );

        /**
        * Get the included tag tree value at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        * @return TUint16: the include tag value.
        */
        TUint16 IncTagValue( TUint16 aLevel, TUint16 aWidth ) const;

        /**
        * Get the included tag tree state at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        * @return TUint16: the include tag state.
        */
        TUint16 IncTagState( TUint16 aLevel, TUint16 aWidth ) const;

        /**
        * Get the msb tag tree value at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        * @return TUint16: the msb tag value.
        */
        TUint16 MsbTagValue( TUint16 aLevel, TUint16 aWidth ) const;

        /**
        * Get the msb tag tree state at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        * @return TUint16: the msb tag state.
        */
        TUint16 MsbTagState( TUint16 aLevel, TUint16 aWidth ) const;

        /**
        * Set the included tag tree value at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        * @param aValue: the value.
        */
        void SetIncTagValue( TUint16 aLevel, TUint16 aWidth, TUint16 aValue );

        /**
        * Set the included tag tree state at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        * @param aState: the state.
        */
        void SetIncTagState( TUint16 aLevel, TUint16 aWidth, TUint16 aState );

        /**
        * Increment the included tag tree state at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        */
        void IncrementIncTagState( TUint16 aLevel, TUint16 aWidth );

        /**
        * Set the msb tag tree value at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        * @param aValue: the value.
        */
        void SetMsbTagValue( TUint16 aLevel, TUint16 aWidth, TUint16 aValue );

        /**
        * Set the msb tag tree state at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        * @param aState: the state.
        */
        void SetMsbTagState( TUint16 aLevel, TUint16 aWidth, TUint16 aState );

        /**
        * Increment the msb tag tree state at specific level and width
        * @since 2.6
        * @param aLevel: the layer.
        * @param aWidth: the width.
        */
        void IncrementMsbTagState( TUint16 aLevel, TUint16 aWidth );

        /**
        * Is packet header has been read
        * @since 2.6
        * @return TUint8: true if packet header has been read.
        */
        TUint8 IsHeader() const;

        /**
        * Set the flag to indicate that packet header has been read
        * @since 2.6
        */
        void SetHeader();

        /**
        * Reset the flag to indicate that packet header has not been read
        * @since 2.6
        */
        void ResetHeader();

        /**
        * Is packet body has been read
        * @since 2.6
        * @return TUint8: true if packet body has been read.
        */
        TUint8 IsBody() const;

        /**
        * Set the flag to indicate that packet body has been read
        * @since 2.6
        */
        void SetBody();

        /**
        * Reset the flag to indicate that packet body has not been read
        * @since 2.6
        */
        void ResetBody();

        /**
        * Set the flag to indicate that packet body read underflow
        * @since 2.6
        */
        void SetBodyIncomplete();

        /**
        * Reset the flag to indicate that packet body read is not underflow
        * @since 2.6
        */
        void ResetBodyIncomplete();

        /**
        * Is the read recover from previous underflow
        * @since 2.6
        * @return TUint8: true if recover from previous underflow.
        */
        TUint8 IsRecoverFromIncomplete() const;

        /**
        * Set the flag to indicate that the next read is recover from underflow
        * @since 2.6
        */
        void SetRecoverFromIncomplete();

        /**
        * Reset the flag to indicate that the next read is not recover from underflow
        * @since 2.6
        */
        void ResetRecoverFromIncomplete();

        /**
        * Is codeblock number should iLastCodeBlock
        * @since 2.6
        * @return TUint8: true if codeblock number should match with iLastCodeBlock.
        */
        TUint8 IsMatch() const;

        /**
        * Set the flag to indicate that the next codeblock should match iLastCodeBlock
        * @since 2.6
        */
        void ResetMatch();

        /**
        * Reset the flag to indicate that the next codeblock should be greater than 
        * iLastCodeBlock
        * @since 2.6
        */
        void SetMatch();

    public:     // Data
    
    protected:  // Data

    private:    // Data
       
        // Flags to control the parsing of bitstream
        enum TReadPacket
            {
            EReadHeader                = 0x01,
            EReadBody                  = 0x02,
            EReadBodyIncomplete        = 0x04,
            EReadRecoverFromIncomplete = 0x08,
            EReadMatchLastCodeBlock    = 0x10
            };

        // Canvas of the packet
        TRect iPacketCanvas;      

        // Codeblock size
        TSize iCodeBlockSize;     

        // Tag tree level
        TUint8 iTagTreeLevel;      

        // Control flags
        TUint8 iReadControl;     
        
        // Inclusive layer number
        TUint16 iLayer;         
        
        // Last codeblock processed
        TUint16 iLastCodeBlock;     

        // Inclusive information
        HBufC8 *iIncluded;          

        // Tagtree information
        HBufC16 *iTagTreeInfo;  
        
        // Included tag tree value
        HBufC16 *iIncTagTreeValue;   

        // Included tag tree state
        HBufC16 *iIncTagTreeState;   

        // MSB tag tree value
        HBufC16 *iMsbTagTreeValue;   

        // MSB tag tree state
        HBufC16 *iMsbTagTreeState;   

        // List of codeblocks
        RPointerArray<CJ2kCodeBlock> iCodeBlockList; 

    public:     // Friend classes
       
    protected:  // Friend classes
        
    private:    // Friend classes
 
    };

// For inliners.
#include "JP2KPacket.inl"

#endif // __JP2KPACKET_H__

