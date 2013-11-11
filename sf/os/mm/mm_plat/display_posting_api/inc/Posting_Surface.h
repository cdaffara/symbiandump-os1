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
* Description:     Header file for S60 display posting API
*
*/





#ifndef __POSTING_SURFACE_H__
#define __POSTING_SURFACE_H__

//- Include Files  ----------------------------------------------------------

#include <e32base.h>
#include <e32cmn.h>
#include <gdi.h>

//- Namespace ---------------------------------------------------------------

//- Constants ---------------------------------------------------------------

///Little endian bit mask
const TUint KPostFormatLE    = 0x01000000;
///Big endian bit mask
const TUint KPostFormatBE    = 0x02000000;
///Color range 0-255 bit mask
const TUint KPostColorRange0 = 0x00000010;
///Color range 16-235 bit mask
const TUint KPostColorRange1 = 0x00000020;
///ITU-R BT.709-5 standard bit mask
const TUint KPost709         = 0x00000100;
///ITU-R BT.601-5 standard bit mask
const TUint KPost601         = 0x00000200;
///Posting RGB format RGB565 bit mask
const TUint KPostRGB565      = 0x00000001;
///Posting RGB format RGB888 (24bit) bit mask
const TUint KPostRGB888      = 0x00000002;
///Posting RGB format RGBx888 (32bit) bit mask
const TUint KPostRGBx888     = 0x00000004;
///Posting YUV 420 Planar format
const TUint KPostYUV420Planar= 0x00001000;
///Platform specific
const TUint KPostPlatformDependent = 0x10000000;

//- Forward Declarations ----------------------------------------------------

class CPostingSurface;
class TPostingBuff;

//- Class Definitions -------------------------------------------------------
/**
*  CPostingSurface
*
*  Contains core data structures and member functions in the display posting API
*/
class CPostingSurface : public CBase
{
public:

    /**
    *  TPostingFormat
    *
    *  Specifies the data format. Also sampling pattern, data layout and YUV-RGB conversion coefficients for YUV data.
    */
    enum TPostingFormat
    {
            EFormatInvalidValue   = 0x00000000,     //Initialisation value, do not use
            ERgb16bit565Le        = KPostRGB565  | KPostFormatLE,
            ERgb24bit888Le        = KPostRGB888  | KPostFormatLE,
            ERgb32bit888Le        = KPostRGBx888 | KPostFormatLE,
            EYuv422LeBt709Range0  = KPost709     | KPostFormatLE | KPostColorRange0,
            EYuv422LeBt709Range1  = KPost709     | KPostFormatLE | KPostColorRange1,
            EYuv422BeBt709Range0  = KPost709     | KPostFormatBE | KPostColorRange0,
            EYuv422BeBt709Range1  = KPost709     | KPostFormatBE | KPostColorRange1,
            EYuv422LeBt601Range0  = KPost601     | KPostFormatLE | KPostColorRange0,
            EYuv422LeBt601Range1  = KPost601     | KPostFormatLE | KPostColorRange1,
            EYuv422BeBt601Range0  = KPost601     | KPostFormatBE | KPostColorRange0,
            EYuv422BeBt601Range1  = KPost601     | KPostFormatBE | KPostColorRange1,
            EYuv420PlanarBt709Range0 = KPost709     | KPostYUV420Planar | KPostColorRange0,
            EYuv420PlanarBt709Range1 = KPost709     | KPostYUV420Planar | KPostColorRange1,
            EYuv420PlanarBt601Range0 = KPost601     | KPostYUV420Planar | KPostColorRange0,
            EYuv420PlanarBt601Range1 = KPost601     | KPostYUV420Planar | KPostColorRange1,
            EYuvPlatformDependent    = KPostPlatformDependent,
            };


    /**
    *  TRotationType
    *
    *  Rotation types as defined in [MDF Video Common Elements]
    */
    enum TRotationType
    {
            ENoRotation            = 0x00000000, //0Deg
            ERotate90ClockWise     = 0x00000001, //90Deg
            ERotate180             = 0x00000002, //180Deg
            ERotate90AntiClockWise = 0x00000004, //270Deg
            };


    /**
    *  TPostingBuffering
    *
    *  Bitmask value describing how the display posting is done and how the frames should be handled.
    */
    enum TPostingBuffering
    {
            EBufferingInvalid  = 0x00000000,    //Initialisation value, do not use
            ESingleBuffering   = 0x00000004,
            EDoubleBuffering   = 0x00000001,
            ETripleBuffering   = 0x00000002,
            EProgressiveFrames = 0x00000010,
            EInterlacedFrames  = 0x00000020,
            EDisAllowFrameSkip = 0x00000100,
            EAllowFrameSkip    = 0x00000200,
            EInternalBuffers   = 0x00001000,
            EExternalBuffers   = 0x00002000,
            };


    /**
    *  TPostageUsageHint
    *
    *  Tells the intended usage of the posting surface. The display driver uses these hints to prioritize the HW usage and route content.
    *  Vendor specific meanings may be added to the end of the enum space.
    */
    enum TPostingUsageHint
    {
            EUsageInvalid   = 0x00000000,   //Initialisation value, do not use
            EGeneric        = 0x00000001,
            E3D             = 0x00000002,
            EVideoPlayback  = 0x00000004,
            EVideoCapture   = 0x00000008,
            EViewfinder     = 0x00000010,
            EStillImage     = 0x00000011,
            EVendorSpecific = 0x80000000,
            };

    /**
    *  TPostingCapab
    *
    *  Describes the Posting buffer size restrictions and posting processing capabilities.
    */
    class TPostingCapab
    {
        public:

            inline TPostingCapab();

        public:
            /** Maximum buffer size in bytes */
            TUint iMaxSourceSize;

            /** Hw limits for source size, normally cannot allocated whole this size */
            TSize iMaxPixelSize;

            /** Bit mask of supported rotations */
            TUint iSupportedRotations;

            /** ETrue when horizontal mirroring is supported */
            TBool iSupportsMirroring;

            /** ETrue when rational scales is supported */
            TBool iSupportsScaling;

            /** ETrue when brightness tuning is supported */
            TBool iSupportsBrightnessControl;

            /** ETrue when contrast tuning is supported */
            TBool iSupportsContrastControl;

            /** Bitmask (TPostingBuffering) */
            TUint iSupportedPostingBuffering;

            /** Bitmask (TBufferType) */
            TUint iSupportedBufferTypes;
    };

    enum TBufferType
    {
            EBufferTypeInvalid = 0x00000000,    //Initialisation value, do not use
            EStandardBuffer    = 0x00000001,    //Direct address
            EChunkBuffer       = 0x00000002,//RChunk handle
            EExternalBuffer    = 0x00000004,//External buffers
    };

    /**
    *  TPostingSourceParams
    *
    *  Describes the source content and buffering parameters. These values are fixed in the initialization phase.
    */
    class TPostingSourceParams
    {
        public:
                inline TPostingSourceParams();

        public:
            /** Buffering usage mode (TPostingBuffering) */
            TUint iPostingBuffering;

            /** Indicates the type of buffer requested */
            TBufferType iBufferType;

            /** Purpose of buffer (TPostingUsageHint) */
            TUint iPostingUsage;

            /** Source resolution. Values must be even */
            TSize iSourceImageSize;

            /** Source's pixel format */
            TPostingFormat iPostingFormat;

            /** Source pixel aspect ratio (width), Scaling factor = Numerator/Denominator. Tv out only */
            TUint16 iPixelAspectRatioNum;

            /** Source pixel aspect ratio (width) */
            TUint16 iPixelAspectRatioDenom;

            /**
            *  ETrue when copy protection enabled
            *  indicates if the content has copy protection restrictions. true: Restricted
            */
            TBool iContentCopyRestricted;
    };


    /**
    *  TPostingParams
    *
    *  Speficies window properties and required processing types for the posting buffer before it can be displayed on the screen.
    */
    class TPostingParams
    {
        public:

            inline TPostingParams();

        public:
            /** Size and position of the rectangle on phone's display. Even. Ui-orientation parameters.
            *   Does not affect Tv-out
            */
            TRect iDisplayedRect;

			/** Output size in pixels, defines scaling.  Relative to iDisplayedRect and must not
			*   exceed those boundaries
			*/
			TRect iScaleToRect;
        
            /** Required cropping for the content before applying other processing. Even.
            *   This is <= iSourceImageSize. This is Tv-out crop too
            */
            TRect iInputCrop;

            /** ETrue when horizontal mirroring. Does not affect Tv-out */
            TBool iMirror;
            /** Orientation of content compared to symbian videobuffer. Does not affect Tv-out */
            TRotationType iRotation;

            /** Brightness value on screen, [-100,100]%, may be ignored by the driver */
            TInt16 iBrightness;

            /** Contrast value on screen, [-100,100]% */
            TInt16 iContrast;

            /** Defined background color. Give in same format what symbian videobuffer currently is */
            TRgb iBackGround;
    };


    /**
    *  TPostingBuff
    *
    *  Contains information about the posting buffer.
    */
    class TPostingBuff
    {
        public:
            /** @return EBufferTypeInvalid or EChunkBuffer (iType) */
            IMPORT_C virtual TBufferType GetType();

            /** @return Direct address of buffer (0,0) (iBuffer) */
            IMPORT_C virtual TAny* GetBuffer();

            /** @return buffer size in pixels */
            IMPORT_C virtual TSize GetSize();

            /** @return scanline length in bytes (width*bytes_per_pixel + some_offset) */
            IMPORT_C virtual TUint GetStride();

        protected:
            /** Hidden constructor, user should never use this */
            IMPORT_C TPostingBuff();

        protected:
            /** Type of this TPostingBuff */
            TBufferType iType;

            /** Pointer to buffer */
            TAny*  iBuffer;

            /** Size of frame in pixels */
            TSize  iSize;

            /** Scanline length in bytes (width*bytes_per_pixel + some_offset) */
            TUint iStride;
    };

    /**
	* TPostingBuffExt
	*
    * Extension for TPostingBuff class. Allows external buffers to be set.
    */
    class TPostingBuffExt : public TPostingBuff
        {
        public:
            ///Set pointer to external buffer
            IMPORT_C virtual void SetBuffer(TAny* aBuffer);
            ///Set size of buffer in pixels
            IMPORT_C virtual void SetSize(TSize aSize);
            ///Set scanline length in bytes
            IMPORT_C virtual void SetStride(TUint aStride);
        protected:
            ///Hidden constructor, user should never use this
            IMPORT_C TPostingBuffExt();
        };

    /**
    *  TPostingBufferChunk
    *
    *  the chunk buffer should be requested by the client if the client application must pass the buffer across a process boundary.
    */
    class TPostingBufferChunk : public TPostingBuff
    {
        public:
            /** @return Handle to DChunk (iChunk) */
            IMPORT_C virtual RChunk GetChunk();

            /** @return buffer start address offset in chunk, (iOffsetInChunk) */
            IMPORT_C virtual TUint GetOffsetInChunk();

        protected:
            /** Hidden constructor */
            IMPORT_C TPostingBufferChunk();

        protected:
            /** Handle to DChunk */
            RChunk iChunk;

            /** Offset in chunk, to first pixel */
            TUint iOffsetInChunk;
    };

    public:
    /** Object destructor */
    virtual ~CPostingSurface(){};

    /**
    *   Initialises API for given parameters, and reserving posting for user (limited amount).
    *   If all success and aSource&aDest are filled correctly, posting is ready to use.
    *   @param  aSource  Source buffer information.
    *   @param  aDest    Destination parameters, @see CPostingSurface::SetPostingParameters
    *   @return Leaves when failed, with standard symbian error code
    */
    virtual void InitializeL(const TPostingSourceParams& aSource, const TPostingParams& aDest) = 0;

    /**
    *  Returns posting API provided properties to parameter
    *  @param aCaps Supported features, filled by driver
    *  @return KErrNone if success
    */
    virtual void GetCapabilities(TPostingCapab& aCaps) = 0;

    /**
    *  Queries the underlying implementation about its capability to process proposed input format.
    *  @param aFormat Format what to be asked.
    *  @return ETrue when aFormat is supported, otherwise EFalse
    */
    virtual TBool FormatSupported(TPostingFormat aFormat) = 0;

    /**
    *  Asynchronous freeing of reservation of posting.
    *  @param  aComplete -parameter is status of destroying, driver completes it when complete
    */
    virtual void  Destroy ( TRequestStatus& aComplete)= 0;

    /**
    *  Synchronic version of Destroy(TRequestStatus& aComplete),
    *  Note, this method can take same time what previous version takes
    */
    virtual void Destroy() = 0;

    /**
    *  Starts a wait for next free buffer
    *  It is allowed for an application to wait and acquire
    *  two or more buffers before actually posting the buffers.
    *  However, only one wait for next free buffer can be active at the time (!).
    *  @param  aComplete  Request to be complete when next buffer is available.
    *  @return KErrNone if success
    */
    virtual TInt WaitForNextBuffer(TRequestStatus& aComplete) = 0;

    /**
    *  Gets the next free buffer (must have been waited with WaitForNextBuffer()
    *  for before calling this!).
    *  @return  Next available buffer settings for user
    */
    virtual TPostingBuff* NextBuffer() = 0;

    /** Cancels the wait for the next free buffer. Can be ingnored by driver */
    virtual TInt CancelBuffer() = 0;

    /**
    *  Posts buffer previously got from the API.
    *  @param  aBuffer  Buffer object where driver to continues updating
    *  @return KErrNone if success
    */
    virtual TInt PostBuffer(TPostingBuff* aBuffer) = 0;

    /**
    *  This function can be used to change window
    *  properties and required processing types for the posting
    *  buffer before it can be displayed on the screen.
    *  Perhaps some limititations of parameter change can exists, then this fails.
    *  @param   aParams  New destination settings of Posting
    *  @return  KErrNone if success
    */
    virtual TInt SetPostingParameters(const TPostingParams& aParams) = 0;

    /**
    *  This function can be used to directly pass the DSA-region
    *  of the internal display down to this API if DSA-region is
    *  clipped in some way. Some implementations may support none:
    *  some may support only one or few clipping regions.
    *  In those cases the return value can be KErrNotSupported.
    *  If clipping region is supported, no image will be drawn outside
    *  of the clipping region.
    *  If iDisplayedRect is set to an empty region, then the posting is not
    *  active for the internal display. However, if display posting is still done,
    *  external display may get updated. Default: no clipping is done,
    *  but internal display is enabled.
    *  @param  aClipRegion  Rectangles of current DSA.
    *  @return  KErrNone if success
    */
    virtual TInt SetClipRegion(const TRegion& aClipRegion) = 0;
};

//- Inline Functions --------------------------------------------------------

#include "posting_surface.inl"

#endif //__POSTING_SURFACE_H__

// End of File
