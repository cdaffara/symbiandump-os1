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
* Description:  JP2KCodeBlock class used to collect the codeblock
*                related information such as compressed data and data length.
*
*/


#ifndef __JP2KCODEBLOCK_H__
#define __JP2KCODEBLOCK_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CJ2kPacket;

// CLASS DECLARATION

/**
 * JP2KCodeBlock class used to collect the code block
 * related information such as compressed data, data
 * length, and etc.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class CJ2kCodeBlock : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CJ2kCodeBlock();

        /**
        * Destructor.
        */
        virtual ~CJ2kCodeBlock();

    public: // New functions
        
        /**
        * Set the canvas of the code block
        * @since 2.6
        * @param aX: x position of the codeblock.
        * @param aY: y position of the codeblock.
        * @param aWidth: width of the codeblock.
        * @param aHeight: height of the codeblock.
        */
        void SetCodeBlockCanvas( TInt32 aX, TInt32 aY, TInt32 aWidth, TInt32 aHeight );

        /**
        * Initialize the number of segment
        * @since 2.6
        * @param aLayer: number of layer.
        */
        void InitializeL( TUint16 aLayer );

        /**
        * Get the data buffer descriptor
        * @since 2.6
        * @return HBufC8*: pointer to the descriptor of the data buffer.
        */
        HBufC8* DataL();

        /**
        * Get the pointer to the data portion
        * @since 2.6
        * @return TUint8*: pointer to the data buffer.
        */
        const TUint8* Data() const;

        /**
        * Get the last pass
        * @since 2.6
        * @return TInt8: the last pass.
        */
        TInt8 LastPass() const;

        /**
        * Get the pass index
        * @since 2.6
        * @return TInt8: the pass index.
        */
        TUint8 PassIndex() const;

        /**
        * Get the number of empty bit planes
        * @since 2.6
        * @return TInt8: the empty bit planes.
        */
        TUint8 EmptyBitplanes() const;

        /**
        * Get the canvas of the codeblock
        * @since 2.6
        * @return TRect&: reference to the canvas of the codeblock.
        */
        const TRect& CodeBlockCanvas() const;

        /**
        * Get the data length
        * @since 2.6
        * @return TInt32: the data length.
        */
        TUint32 DataLength() const;

        /**
        * Get the data size at specific segment
        * @since 2.6
        * @param aIndex: the specific segment.
        * @return TInt16: the data size at specific segment.
        */
        TUint16 CblkLength( TUint16 aIndex ) const;

        /**
        * Get the pass per segment at specific segment
        * @since 2.6
        * @param aIndex: the specific segment.
        * @return TInt16: the pass per segment at specific segment.
        */
        TUint16 PassesPerSegment( TUint16 aIndex ) const;

        /**
        * Reset the pass index to 0
        * @since 2.6
        */
        void ResetPassIndex();

        /**
        * Increment the pass index
        * @since 2.6
        */
        void IncrementPassIndex();

        /**
        * Set to indicate that the codeblock has been decoded
        * @since 2.6
        */
        void SetCodeBlockDecoded();

    public: // Functions from base classes
         
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Last pass
        TInt8 iLastPass;        

        // Pass index
        TUint8 iPassIndex;       
        
        // Empty bit plane
        TUint8 iEmptyBitplane;

        // Bits length
        TUint8 iLengthBits;      

        // Data length
        TUint32 iDataLength;      

        // Data size of each segment
        HBufC16 *iDataSize;        

        // Number of pass per segment
        HBufC16 *iPassesPerSegment;

        // Number of segment
        HBufC16 *iNumSegment;      

        // Canvas of the code block
        TRect iCodeBlockCanvas; 

        // Compressed image data
        HBufC8 *iData;           
        
        // True if codeblock has been decoded
        TUint8 iIsDecoded;       

    public:     // Friend classes
        friend class CJ2kPacket;

    protected:  // Friend classes
        
    private:    // Friend classes        
    };

// For inliners.
#include "JP2KCodeBlock.inl"

#endif // __JP2KCODEBLOCK_H__

