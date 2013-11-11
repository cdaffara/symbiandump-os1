/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Domain API for Video Decoder Scaling Custom Interface    
*
*/





#ifndef VIDEODECODERSCALINGCI_H
#define VIDEODECODERSCALINGCI_H

//  INCLUDES
#include <mmf/devvideo/devvideobase.h>


// CONSTANTS

/** Video Decoder Scaling Custom Interface UID */
const TUid KMmfVideoDecoderScalingUid = { 0x2001E2E7 };


// CLASS DECLARATIONS



/**
 * Video Decoder Scaling Custom Interface main interface class. 
 *
 * The Video Decoder Scaling Custom Interface allows the MDF video
 * decoders to signal the capability to do scaling. 
 * Decoders which support this custom interface will implement this class.
 * 
 * 
 */

class MMmfVideoDecoderScaling
    {
public:
		/**
		 * Gets the maximum output size supported by the decoder.
		 * Clips with larger frame size will be scaled down by the decoder.
     * 
		 * @param aSize Max size supported by decoder
		 **/
    virtual void   GetMaxOutputSize(TSize& aSize) = 0; 	

    };
#endif      // VIDEODECODERSCALINGCI_H
            
// End of File
