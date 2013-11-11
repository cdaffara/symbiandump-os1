// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Mda\Common\MmfVideo.h
// 
//

#ifndef __MMF_COMMON_VIDEO_ENUMS_H__
#define __MMF_COMMON_VIDEO_ENUMS_H__

#include <mmf/common/mmfutilities.h>


/**
Video playback rate capabilities. This structure defines playback rate
capabilities for the current controller and video clip, and can be used
to determine if fast forward, rewind, step forward, or step backward
is possible.

@publishedPartner
@released

@see CVideoPlayerUtility::GetPlayRateCapabilitiesL, CVideoPlayerUtility::SetPlayVelocityL
*/
class TVideoPlayRateCapabilities
    {
public:
    /**
    ETrue if fast or slow play forward is possible (i.e. playback
    velocity 1-99 or 101-). Normal play forward (velocity 100) is
    supported by default.
    */
    TBool iPlayForward;

    /**
    ETrue if play backward is possible (i.e. playback velocity below0).
    This includes normal speed, slow, and fast reverse playback.
    */
    TBool iPlayBackward;

    /** ETrue if step forward is possible */
    TBool iStepForward;

    /** Etrue if step backward is possible */
    TBool iStepBackward;
    };

/**
Video quality.

@publishedPartner
@released

@see CVideoPlayerUtility::SetVideoQualityL
*/
enum TVideoQuality 
	{
	EVideoQualityLow = 25,
	EVideoQualityNormal = 50,
	EVideoQualityHigh = 75,
	EVideoQualityLossless = 100
	};
	
#endif
