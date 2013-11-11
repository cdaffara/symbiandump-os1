/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This Custom API provides use case hints to camera.
*               Application can use this custom interface to notify camera
*               which use case it's going to use before reserving camera. 
*               Based on this information camera adaptation can optimize 
*               resource allocation so that camera is able to start up faster
*               for the particular use case.
*
*/


#ifndef ECAMUSECASEHINTCUSTOMAPI_H
#define ECAMUSECASEHINTCUSTOMAPI_H

#include <e32base.h>
#include <ecam.h>

// CONSTANTS

const TUid KCameraUseCaseHintUid = { 0x2002EA1A };

// CLASS DECLARATION

/**
* Custom interface for providing use case hints to camera.
*/
class MCameraUseCaseHint
    {
public: // Enumerations
    
    /**
    * List of codecs for encoded video
    * @see HintDirectVideoCaptureL()
    */
    enum TVideoCodec
        {
        ECodecUnknown = 0,
        ECodecMpeg4,
        ECodecH263,
        ECodecH264,
        ECodecVC1,
        ECodecSorenson,
        ECodecOn2Vp6,
        ECodecOn2Vp7,
        };
    
    /**
    * List of profiles for encoded video
    * @see HintDirectVideoCaptureL()
    */
    enum TVideoProfile
        {
        EProfileUnknown = 0,
        
        EProfileMPEG4SpL0 = 0x1000,
        EProfileMPEG4SpL0b,
        EProfileMPEG4SpL1,
        EProfileMPEG4SpL2,
        EProfileMPEG4SpL3,
        EProfileMPEG4SpL4a,
        EProfileMPEG4SpL5,
        EProfileMPEG4SpL6,
        
        EProfileMPEG4AspL0,
        EProfileMPEG4AspL0b,
        EProfileMPEG4AspL1,
        EProfileMPEG4AspL2,
        EProfileMPEG4AspL3,
        EProfileMPEG4AspL3b,
        EProfileMPEG4AspL4,
        EProfileMPEG4AspL4a,
        EProfileMPEG4AspL5,
        EProfileMPEG4AspL6, 

        EProfileH263P0L10 = 0x2000,
        EProfileH263P0L20,
        EProfileH263P0L30,
        EProfileH263P0L40,
        EProfileH263P0L45,
        
        EProfileH264BpL1 = 0x3000,
        EProfileH264BpL1b,
        EProfileH264BpL1_1,
        EProfileH264BpL1_2,
        EProfileH264BpL1_3,
        EProfileH264BpL2,
        EProfileH264BpL2_1,
        EProfileH264BpL2_2,
        EProfileH264BpL3,
        EProfileH264BpL3_1,
        EProfileH264BpL3_2,
        EProfileH264BpL4,
        
        EProfileH264MpL1,
        EProfileH264MpL1b,
        EProfileH264MpL1_1,
        EProfileH264MpL1_2,
        EProfileH264MpL1_3,
        EProfileH264MpL2,
        EProfileH264MpL2_1,
        EProfileH264MpL2_2,
        EProfileH264MpL3,
        EProfileH264MpL3_1,
        EProfileH264MpL3_2,
        EProfileH264MpL4,
        
        EProfileH264HpL1,
        EProfileH264HpL1b,
        EProfileH264HpL1_1,
        EProfileH264HpL1_2,
        EProfileH264HpL1_3,
        EProfileH264HpL2,
        EProfileH264HpL2_1,
        EProfileH264HpL2_2,
        EProfileH264HpL3,
        EProfileH264HpL3_1,
        EProfileH264HpL3_2,
        EProfileH264HpL4,
        
        EProfileVC1SpLL = 0x4000,
        EProfileVC1SpLM,
        EProfileVC1MpLL,
        EProfileVC1MpLM,
        EProfileVC1MpLH,
        EProfileVC1ApL0,
        EProfileVC1ApL1,
        EProfileVC1ApL2,
        
        EProfileSSGeneric = 0x5000,
        
        EProfileVP6Generic = 0x6000,
        };
    
public: // Methods

    /**
    * Gives a hint to camera that client wants to capture encoded video.
    * 
    * This method should be called after calling CCamera::New2L() and
    * before CCamera::Reserve().
    * 
    * @param aCodec Codec of the encoded video.
    * @param aProfile Profile of the encoded video.
    * @param aResolution Resolution of the encoded video.
    * @leave KErrInUse if this method is not called at the right time.
    * @leave KErrNotSupported if given codec/profile/resolution is not
    *                         supported.
    */
    virtual void HintDirectVideoCaptureL(
        TVideoCodec aCodec,
        TVideoProfile aProfile,
        const TSize& aResolution ) = 0;
    
    /**
    * Gives a hint to camera that client wants to capture non-encoded video.
    * 
    * This method should be called after calling CCamera::New2L() and
    * before CCamera::Reserve().
    * 
    * @param aFormat Format. Must be one of the supported video frame formats 
                     (see TCameraInfo::iVideoFrameFormatsSupported).
    * @param aSizeIndex Size index. Must be in the range 0 to 
                        TCameraInfo::iNumVideoFrameSizesSupported-1 inclusive.
    * @param aRateIndex Rate index. Must be in the range 0 to 
                        TCameraInfo::iNumVideoFrameRatesSupported-1 inclusive.
    * @leave KErrInUse if this method is not called at the right time.
    * @leave KErrNotSupported if given format/size/rate is not supported.
    */
    virtual void HintVideoCaptureL(
        CCamera::TFormat aFormat,
        TInt aSizeIndex,
        TInt aRateIndex ) = 0;
    
    /**
    * Gives a hint to camera that client wants to capture still images.
    * 
    * This method should be called after calling CCamera::New2L() and
    * before CCamera::Reserve().
    * 
    * @param aFormat Format. Must be one of the supported still formats
    *                (see TCameraInfo::iImageFormatsSupported).
    * @param aSizeIndex Size index. Must be in the range of 0 to 
    *                   TCameraInfo::iNumImageSizesSupported-1 inclusive.
    * @leave KErrInUse if this method is not called at the right time.
    * @leave KErrNotSupported if given format/size/rate is not supported.
    */
    virtual void HintStillCaptureL(
        CCamera::TFormat aFormat,
        TInt aSizeIndex ) = 0;
    };

#endif //ECAMUSECASEHINTCUSTOMAPI_H
