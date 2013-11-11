/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of TFrameFormatExt and CVisualFrameImpl
*
*/

#ifndef _ICLEXTJPEGAPIFRAMEIMPL_
#define _ICLEXTJPEGAPIFRAMEIMPL_

#include <icl/icl_propertyuids.h>
#include "IclExtJpegApiConst.h"

class CVisualFrame::TFrameLayout;   

#include "imageframe.h"

/**
* TFrameFormatExt
* @lib IclExtJpegApi.lib
* @since 3.2
*/
class TFrameFormatExt : public TFrameFormatBase
    {
    public:
    
        /**
        * C++ default constructor.
        */
        TFrameFormatExt( TInt aFormatCode ) : 
            TFrameFormatBase( KUidExtIclImageFrameFormat ), 
            iFormatCode( aFormatCode )
            {
            Constructor();
            };

        /**
        * Duplicate
        * @since 3.2
        * @return TFrameFormatExt
        */
        TFrameFormatBase* DuplicateL() const
            {
            return new ( ELeave ) TFrameFormatExt( iFormatCode );
            };

        /**
        * Colour space
        * @since 3.2
        * @return TUid
        */
        TUid ColourSpace() const;
            
        /**
        * Sampling
        * @since 3.2
        * @return TUid
        */
        TUid Sampling() const;
            
        /**
        * Format code
        * @since 3.2
        * @return TInt
        */
        TInt FormatCode() const;
            
        /**
        * Set colour space
        * @since 3.2
        * @param aColourSpace
        * @return void
        */
        void SetColourSpace( TUid aColourSpace );
    

    protected:
          
        /**
        * constructor.
        */
        void Constructor();

    protected: // Data
    
        // The image frame colour space
        TUid    iColourSpace;
        // The frame sampling 
        TUid    iSampling;
        // The image frame format code which uniquely identifies all other parameters.  
        TInt    iFormatCode; 
    
    };  
    

/**
* CVisualFrameImpl
* @lib IclExtJpegApi.lib
* @since 3.2
*/  
class CVisualFrameImpl : public CImageFrame 
       	{
        public:
            
            /**
            * Two-phased constructor.
            * @since 3.2
            * @param aBuffer		A descriptor reference to buffer containing the image data.
            * @param aDimension		The dimensions of the corresponding image data.
            * @param aFrameFormat	The frame format of the corresponding image data.
            */
            static CVisualFrameImpl* NewL(  
                                    TDes8& aBuffer, 
                                    const TSize& aDimension,
                                    TInt aFrameFormat );
                       
            /**
            * Two-phased constructor.
            * @param aChunk         An RChunk reference to buffer containing the image data.
        	* @param aSize          The amount of image data in bytes.
        	* @param amaxSize       The maximum size of the memory reserved in the chunk.
        	* @param aDataOffset    The offset value specifying the location of the image 
        	*                       data in the chunk.
        	* @param aDimension     The dimensions of the corresponding image data.
        	* @param aFrameFormat   The frame format of the corresponding image data.
            */                                
            static CVisualFrameImpl* NewL( 
                                    RChunk& aChunk, 
                                    TInt aSize,
                                    TInt aMaxSize, 
                                    TInt aDataOffset,
                                    const TSize& aDimension, 
                                    TInt aFrameFormat );
            /**
            * Two-phased constructor.
            * @param aBuffer        A descriptor reference to buffer containing the image data.
        	* @param aDimension     The dimensions of the corresponding image data.
        	* @param aFrameLayout   The layout of the image color components.
            */
            static CVisualFrameImpl* NewL( 
                                    TDes8& aBuffer, 
                                    const TSize& aDimension, 
                                    TInt aFrameFormat, 
                                    const CVisualFrame::TFrameLayout& aFrameLayout ); 
            
            /**
            * Two-phased constructor.
            * @param aChunk         An RChunk reference to buffer containing the image data.
        	* @param aSize          The amount of image data in bytes.
        	* @param aMaxSize       The maximum size of the memory reserved in the chunk for 
        	*                       the image data.
        	* @param aDimension     The dimensions of the corresponding image data.
        	* @param aFrameFormat   The frame format of the corresponding image data.
        	* @param aFrameLayout   The layout of the image color components. TFrameLayout 
        	* 						contains the scan line lengths and offsets for each component 
        	*						in planar YUV case
            */
            static CVisualFrameImpl* NewL( 
                                    RChunk& aChunk, 
                                    TInt aSize, 
                                    TInt aMaxSize,
                                    const TSize& aDimension, 
                                    TInt aFrameFormat, 
                                    const CVisualFrame::TFrameLayout& aFrameLayout );       
           
            static CVisualFrameImpl* NewL( const CImageFrame* aImageFrame ); 
            
            /**
            * Get image frame
            * @since 3.2
            * @return CImageFrame
            */
            CImageFrame* GetImageFrameL();
                                    
            /**
            * Returns a constant reference to the chunk containing 
        	* the image data, if exists.
            * @since 3.2
            * @return RChunk
            */                                    
            const RChunk& DataChunkL() const;
            
            /**
            * Returns a descriptor pointer to the image data location, if exists
            * @since 3.2
            * @return TPtr8
            */
            TPtr8 DataPtrL() const;
            
            /**
            * Returns the dimensions of the image.
            * @since 3.2
            * @return TSize
            */
            TSize Dimension() const;
            
            /**
            * Return the image data format.
            * @since 3.2
            * @return TInt
            */
            TInt FrameFormat2() const;
            
            /**
            * Returns the offset of the image data for non-planar, 
        	* single-component or compressed images. 
            * @since 3.2
            * @return TInt
            */
            TInt DataOffset() const;
            
            /**
            * Returns the amount of stored image data in bytes.
            * @since 3.2
            * @return TInt
            */
            TInt Size() const;

            /**
            * Sets the amount of image data to the given value. 
        	* Should be called when the image data amount is modified explicitly.
            * @since 3.2
            * @param
            * @return void
            */
            void SetSizeL( TInt aSize );

            /**
            * Returns the maximum size of the memory space reserved for image data
            * @since 3.2
            * @return TInt
            */
            TInt MaxSize() const;
            
            /**
            * Returns the storage type of the image data.
            * @since 3.2
            * @return TInt
            */
            TInt DataContainer() const;
            
            /**
            * Returns the layout of the image data for planar compressed images.
            * @since 3.2
            * @param
            * @return CVisualFrame::TFrameLayout
            */
            const CVisualFrame::TFrameLayout& FrameLayout2() const;    

            /**
            * Destructor.
            */
            virtual ~CVisualFrameImpl();      
            
        private: // Data
        
            CVisualFrame::TFrameLayout iFrameLayout2;

            TInt iSize;
        };


#endif // _ICLEXTJPEGAPIFRAMEIMPL_


// End of File
